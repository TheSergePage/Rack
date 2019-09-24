/*
© 2019, Dark Orb.

The license version - 1.0

This license is hereby grants to any person who obtained a copy of this software the next rights to:
1. Use and do reverse-engineering of compiled version of this software at no cost, without any restrictions, in non-commercial and commercial purposes
2. Use source codes of this software at no cost but with the limitations - source codes available only for non-commercial, academic and / or scientific purposes
3. Copy and distribute without any fee
4. Create a copy of the original repository and / or create own derivative software for non-commercial,  academic and / or scientific purposes only

This license is require to:
1. Keep the full license text without any changes
2. The license text must be included once in a file called 'License' which placed in the root directory of the software and in all source files of the software

This license is deny to:
1. Change license of the derivative software
2. Use the copyright holder name and name of any contributor of this software for advertising derivative software without legally certified permission
3. Sell this software without an author legally certified permission

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "rackLib.hpp"

using namespace Rack;

RPackage::RPackage() noexcept:
  vBuffer( nullptr ) {}

RPackage::RPackage( const RPackage &_Copy ) noexcept:
  vBuffer( nullptr ), vFlatData( _Copy.fGetFlatData() ) {}

RPackage::RPackage( vector<RFile *> _Data ) noexcept:
  vBuffer( nullptr ), vFlatData( _Data ) {}

RPackage::~RPackage() {
  fClearBuffer();
  vFlatData.clear();
}

enErrorCode RPackage::fCreatePackage( const wstring _Path,
                                      const vector <RFile *> _Data ) {
  if( _Path.empty() )
    return EC_INVALID_PATH;

  if( _Data.empty() )
    return EC_INVALID_ARGUMENT;

  wstring vSerializedData;

  for( const RFile *vFile : _Data )
    vSerializedData += fSerializeFile( vFile );

  wofstream vFile( _Path + L".rpack", ios::binary | ios::trunc );

  size_t vSourceSize = vSerializedData.length() + 1;

  wchar_t *vFileBuffer = new wchar_t [ vSourceSize ];

  wcscpy_s( vFileBuffer, vSourceSize, vSerializedData.c_str() );
  vSerializedData.clear();

  vFile.write( vFileBuffer, wcslen( vFileBuffer ) );

  vFile.close();

  delete[] vFileBuffer;

  return EC_OK;
}

enErrorCode RPackage::fLoadPackage( const wstring _Path ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  vFlatData.clear();

  wchar_t vControlCharacters [ 4 ];
  while( vFile.get( vControlCharacters [ 0 ] ) ) {
    vFile.get( vControlCharacters [ 1 ] );
    vFile.get( vControlCharacters [ 2 ] );
    vFile.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_START_PATH
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_PATH_DIVIDER ) {
      RFile *vGetFile = fParseFile( vFile );

      if( vGetFile != nullptr )
        vFlatData.push_back( vGetFile );
      else {
        vFile.close();

        return EC_INVALID_PARSE;
      }
    } else
      vFile.seekg( -3, ios::cur );
  }

  vFile.close();

  if( vFlatData.empty() )
    return EC_INVALID_PACKAGE;

  return EC_OK;
}

enErrorCode RPackage::fLoadPackage( const wstring _Path,
                                    vector<RFile *> &_Data ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  _Data.clear();

  wchar_t vControlCharacters [ 4 ];
  while( vFile.get( vControlCharacters [ 0 ] ) ) {
    vFile.get( vControlCharacters [ 1 ] );
    vFile.get( vControlCharacters [ 2 ] );
    vFile.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_START_PATH
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_PATH_DIVIDER ) {
      RFile *vGetFile = fParseFile( vFile );

      if( vGetFile != nullptr )
        _Data.push_back( vGetFile );
      else {
        vFile.close();

        return EC_INVALID_PARSE;
      }
    } else
      vFile.seekg( -3, ios::cur );
  }

  vFile.close();

  if( _Data.empty() )
    return EC_INVALID_PACKAGE;

  return EC_OK;
}

enErrorCode RPackage::fFullLoadPackage( const wstring _Path ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  fClearBuffer();

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  vBuffer = new wchar_t [ static_cast< uint32_t >( vFileLength ) + 1 ];

  vFile.read( vBuffer, vFileLength );

  vFile.close();

  if( wcslen( vBuffer ) == 0 ) {
    fClearBuffer();

    return EC_INVALID_READ;
  }

  vFlatData.clear();

  try {
    for( size_t c = 0; c < static_cast< uint32_t >( vFileLength ) + 1; c++ ) {
      if( vBuffer [ c ] == OPC_START_PATH
          && vBuffer [ c + 1 ] == OPC_PATH_DIVIDER
          && vBuffer [ c + 2 ] == OPC_PATH_DIVIDER
          && vBuffer [ c + 3 ] == OPC_PATH_DIVIDER ) {
        RFile *vGetFile = fParseFile( c );

        if( vGetFile != nullptr )
          vFlatData.push_back( vGetFile );
        else
          return EC_INVALID_PARSE;
      }
    }
  } catch( ... ) {
    return EC_INVALID_PARSE;
  }

  fClearBuffer();

  return EC_OK;
}

enErrorCode RPackage::fFullLoadPackage( const wstring _Path,
                                        vector<RFile *> &_Data ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  fClearBuffer();

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  vBuffer = new wchar_t [ static_cast< uint32_t >( vFileLength ) + 1 ];

  vFile.read( vBuffer, vFileLength );

  vFile.close();

  if( wcslen( vBuffer ) == 0 ) {
    fClearBuffer();

    return EC_INVALID_READ;
  }

  _Data.clear();

  try {
    for( size_t c = 0; c < static_cast< uint32_t >( vFileLength ) + 1; c++ ) {
      if( vBuffer [ c ] == OPC_START_PATH
          && vBuffer [ c + 1 ] == OPC_PATH_DIVIDER
          && vBuffer [ c + 2 ] == OPC_PATH_DIVIDER
          && vBuffer [ c + 3 ] == OPC_PATH_DIVIDER ) {
        RFile *vGetFile = fParseFile( c );

        if( vGetFile != nullptr )
          _Data.push_back( vGetFile );
        else
          return EC_INVALID_PARSE;
      }
    }
  } catch( ... ) {
    return EC_INVALID_PARSE;
  }

  fClearBuffer();

  return EC_OK;
}

enErrorCode RPackage::fSavePackage( const wstring _Path ) {
  if( _Path.empty() )
    return EC_INVALID_PATH;

  if( vFlatData.empty() )
    return EC_EMPTY_DATA;

  wstring vSerializedData;

  for( const RFile *vFile : vFlatData )
    vSerializedData += fSerializeFile( vFile );

  wofstream vFile( _Path + L".rpack", ios::binary | ios::trunc );

  vFile.write( vSerializedData.c_str(), vSerializedData.length() );

  vFile.close();

  vSerializedData.clear();

  return EC_OK;
}

enErrorCode RPackage::fSavePackage( const wstring _Path,
                                    const vector <RFile *> _Data ) {
  if( _Path.empty() )
    return EC_INVALID_PATH;

  if( _Data.empty() )
    return EC_EMPTY_DATA;

  wstring vSerializedData;

  for( const RFile *vFile : _Data )
    vSerializedData += fSerializeFile( vFile );

  wofstream vFile( _Path + L".rpack", ios::binary | ios::trunc );

  vFile.write( vSerializedData.c_str(), vSerializedData.length() );

  vFile.close();

  vSerializedData.clear();

  return EC_OK;
}

enErrorCode RPackage::fMakeFlatDirectory( vector<RFile *> &_Destination,
                                          const RDirectory *_Directory ) const {
  if( _Directory == nullptr )
    return EC_INVALID_ARGUMENT;

  _Destination.clear();

  for( RFile *vFile : fGetDirectoryFiles( _Directory ) )
    _Destination.push_back( vFile );

  return EC_OK;
}

enErrorCode RPackage::fMakePackage( vector<RFile *> &_Destination,
                                    const vector<RDirectory *> _Directories ) const {
  if( _Directories.empty() )
    return EC_INVALID_ARGUMENT;

  _Destination.clear();

  for( const RDirectory *vDirectory : _Directories ) {
    for( RFile *vFile : fGetDirectoryFiles( vDirectory ) )
      _Destination.push_back( vFile );
  }

  return EC_OK;
}

enErrorCode RPackage::fReadStructure( const wstring _Path,
                                      RDirectory *&_Destination ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  vector<wstring> vPaths;

  wchar_t vControlCharacters [ 4 ];
  while( vFile.get( vControlCharacters [ 0 ] ) ) {
    vFile.get( vControlCharacters [ 1 ] );
    vFile.get( vControlCharacters [ 2 ] );
    vFile.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_START_PATH
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_PATH_DIVIDER ) {
      wstring vGetFilePath = fParseFilePath( vFile );

      if( !vGetFilePath.empty() )
        vPaths.push_back( vGetFilePath );
      else {
        vFile.close();

        return EC_INVALID_PARSE;
      }
    } else
      vFile.seekg( -3, ios::cur );
  }

  vFile.close();

  for( const wstring vPath : vPaths )
    fCreatePath( _Destination, fSplitPath( vPath ), 0 );

  return EC_OK;
}

enErrorCode RPackage::fExtractFile( const wstring _Path,
                                    const wstring _Name,
                                    RFile *&_Destination ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  wchar_t vControlCharacters [ 4 ];
  while( vFile.get( vControlCharacters [ 0 ] ) ) {
    vFile.get( vControlCharacters [ 1 ] );
    vFile.get( vControlCharacters [ 2 ] );
    vFile.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_START_PATH
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_PATH_DIVIDER ) {
      RFile *vGetFile = fParseFile( vFile );

      if( vGetFile == nullptr ) {
        vFile.close();

        return EC_INVALID_PARSE;
      }

      if( vGetFile->fGetName() == _Name ) {
        _Destination = vGetFile;

        vFile.close();

        return EC_OK;
      }
    } else
      vFile.seekg( -3, ios::cur );
  }

  vFile.close();

  return EC_OK;
}

enErrorCode RPackage::fExtractDirectory( const wstring _Path,
                                         const wstring _Name,
                                         RDirectory *&_Destination ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return EC_INVALID_PATH;

  if( fIsRackFile( _Path ) != DefaultExtension )
    return EC_NOT_PACKAGE;

  wifstream vFile( _Path, ios::binary );

  vFile.seekg( 0, SEEK_END );
  streampos vFileLength = vFile.tellg();
  vFile.seekg( 0 );

  if( vFileLength == 0 ) {
    vFile.close();

    return EC_EMPTY_FILE;
  }

  wchar_t vControlCharacters [ 4 ];
  while( vFile.get( vControlCharacters [ 0 ] ) ) {
    vFile.get( vControlCharacters [ 1 ] );
    vFile.get( vControlCharacters [ 2 ] );
    vFile.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_START_PATH
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_PATH_DIVIDER ) {
      RFile *vGetFile = fParseFile( vFile );

      if( vGetFile == nullptr ) {
        vFile.close();

        return EC_INVALID_PARSE;
      }

      if( fIsPathContainsDirectory( vGetFile->fGetPath(), _Name ) ) {
        fCreatePath( _Destination,
                     fSplitPath( fTruncatePath( vGetFile->fGetPath(), _Name ) ), 0 );

        vGetFile->fSetPath( vGetFile->fGetName() );

        if( fInsertFileToDirectory( _Destination, _Name, vGetFile ) != EC_OK )
          return EC_INVALID_INSERT;
      }
    } else
      vFile.seekg( -3, ios::cur );
  }

  vFile.close();

  return EC_OK;
}

RFile *RPackage::fParseFile( wifstream &_FileStream ) const {
  wstring vPath, vData;

  wchar_t vControlCharacters [ 4 ];
  while( _FileStream.get( vControlCharacters [ 0 ] ) ) {
    _FileStream.get( vControlCharacters [ 1 ] );
    _FileStream.get( vControlCharacters [ 2 ] );
    _FileStream.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_END_PATH )
      break;
    else {
      _FileStream.seekg( -3, ios::cur );

      vPath += vControlCharacters [ 0 ];
    }
  }

  if( vPath.empty() )
    return nullptr;

  _FileStream.get( vControlCharacters [ 0 ] );
  _FileStream.get( vControlCharacters [ 1 ] );
  _FileStream.get( vControlCharacters [ 2 ] );
  _FileStream.get( vControlCharacters [ 3 ] );

  if( vControlCharacters [ 0 ] != OPC_START_DATA
      && vControlCharacters [ 1 ] != OPC_DATA_DIVIDER
      && vControlCharacters [ 2 ] != OPC_DATA_DIVIDER
      && vControlCharacters [ 3 ] != OPC_DATA_DIVIDER ) {
    vPath.clear();

    return nullptr;
  }

  while( _FileStream.get( vControlCharacters [ 0 ] ) ) {
    _FileStream.get( vControlCharacters [ 1 ] );
    _FileStream.get( vControlCharacters [ 2 ] );
    _FileStream.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_DATA_DIVIDER
        && vControlCharacters [ 1 ] == OPC_DATA_DIVIDER
        && vControlCharacters [ 2 ] == OPC_DATA_DIVIDER
        && vControlCharacters [ 3 ] == OPC_END_DATA )
      break;
    else {
      _FileStream.seekg( -3, ios::cur );

      vData += vControlCharacters [ 0 ];
    }
  }

  if( vData.length() == 0 ) {
    vPath.clear();

    return nullptr;
  }

  return new RFile( vPath, vData.c_str() );
}

RFile *RPackage::fParseFile( size_t &_Index ) {
  wstring vPath, vData;

  size_t c = _Index + 4;

  for( ; c < wcslen( vBuffer ); c++ ) {
    if( vBuffer [ c ] == OPC_PATH_DIVIDER
        && vBuffer [ c + 1 ] == OPC_PATH_DIVIDER
        && vBuffer [ c + 2 ] == OPC_PATH_DIVIDER
        && vBuffer [ c + 3 ] == OPC_END_PATH ) {
      c += 4;
      break;
    } else
      vPath += vBuffer [ c ];
  }

  if( vPath.empty() )
    return nullptr;

  if( vBuffer [ c ] != OPC_START_DATA
      && vBuffer [ c + 1 ] != OPC_DATA_DIVIDER
      && vBuffer [ c + 2 ] != OPC_DATA_DIVIDER
      && vBuffer [ c + 3 ] != OPC_DATA_DIVIDER ) {
    vPath.clear();

    return nullptr;
  }

  c += 4;

  for( ; c < wcslen( vBuffer ); c++ ) {
    if( vBuffer [ c ] == OPC_DATA_DIVIDER
        && vBuffer [ c + 1 ] == OPC_DATA_DIVIDER
        && vBuffer [ c + 2 ] == OPC_DATA_DIVIDER
        && vBuffer [ c + 3 ] == OPC_END_DATA ) {
      c += 3;
      break;
    } else
      vData += vBuffer [ c ];
  }

  if( vData.length() == 0 ) {
    vPath.clear();

    return nullptr;
  }

  _Index = c;

  return new RFile( vPath, vData.c_str() );
}

RFile *RPackage::fParseFile( const wchar_t *_Buffer ) const {
  if( _Buffer [ 0 ] != OPC_START_PATH
      && _Buffer [ 1 ] != OPC_PATH_DIVIDER
      && _Buffer [ 2 ] != OPC_PATH_DIVIDER
      && _Buffer [ 3 ] != OPC_PATH_DIVIDER )
    return nullptr;

  wstring vPath, vData;

  size_t c = 4;

  for( ; c < wcslen( _Buffer ); c++ ) {
    if( _Buffer [ c ] == OPC_PATH_DIVIDER
        && _Buffer [ c + 1 ] == OPC_PATH_DIVIDER
        && _Buffer [ c + 2 ] == OPC_PATH_DIVIDER
        && _Buffer [ c + 3 ] == OPC_END_PATH ) {
      c += 4;
      break;
    } else
      vPath += _Buffer [ c ];
  }

  if( vPath.empty() )
    return nullptr;

  if( _Buffer [ c ] != OPC_START_DATA
      && _Buffer [ c + 1 ] != OPC_DATA_DIVIDER
      && _Buffer [ c + 2 ] != OPC_DATA_DIVIDER
      && _Buffer [ c + 3 ] != OPC_DATA_DIVIDER ) {
    vPath.clear();

    return nullptr;
  }

  c += 4;

  for( ; c < wcslen( _Buffer ); c++ ) {
    if( _Buffer [ c ] == OPC_DATA_DIVIDER
        && _Buffer [ c + 1 ] == OPC_DATA_DIVIDER
        && _Buffer [ c + 2 ] == OPC_DATA_DIVIDER
        && _Buffer [ c + 3 ] == OPC_END_DATA ) {
      c += 3;
      break;
    } else
      vData += _Buffer [ c ];
  }

  if( vData.length() == 0 ) {
    vPath.clear();

    return nullptr;
  }

  return new RFile( vPath, vData.c_str() );
}

wstring RPackage::fParseFilePath( wifstream &_FileStream ) const {
  wstring vPath;

  wchar_t vControlCharacters [ 4 ];
  while( _FileStream.get( vControlCharacters [ 0 ] ) ) {
    _FileStream.get( vControlCharacters [ 1 ] );
    _FileStream.get( vControlCharacters [ 2 ] );
    _FileStream.get( vControlCharacters [ 3 ] );

    if( vControlCharacters [ 0 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 1 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 2 ] == OPC_PATH_DIVIDER
        && vControlCharacters [ 3 ] == OPC_END_PATH )
      break;
    else {
      _FileStream.seekg( -3, ios::cur );

      vPath += vControlCharacters [ 0 ];
    }
  }

  if( vPath.empty() )
    return L"";

  return vPath;
}

wstring RPackage::fParseFilePath( const wchar_t *_Buffer ) const {
  if( _Buffer [ 0 ] != OPC_START_PATH
      && _Buffer [ 1 ] != OPC_PATH_DIVIDER
      && _Buffer [ 2 ] != OPC_PATH_DIVIDER
      && _Buffer [ 3 ] != OPC_PATH_DIVIDER )
    return L"";

  wstring vPath;

  size_t c = 4;

  for( ; c < wcslen( _Buffer ); c++ ) {
    if( _Buffer [ c ] == OPC_PATH_DIVIDER
        && _Buffer [ c + 1 ] == OPC_PATH_DIVIDER
        && _Buffer [ c + 2 ] == OPC_PATH_DIVIDER
        && _Buffer [ c + 3 ] == OPC_END_PATH )
      break;
    else
      vPath += _Buffer [ c ];
  }

  if( vPath.empty() )
    return L"";

  return vPath;
}

wstring RPackage::fExtractFileName( const wstring _Path ) const {
  wstring vResult;

  for( size_t c = _Path.length() - 1; c > 0; c-- ) {
    if( _Path [ c ] == L'/' )
      break;
    else
      vResult.insert( vResult.begin(), _Path [ c ] );
  }

  return vResult;
}

wstring RPackage::fSerializeFile( const RFile *_File ) const {
  wstring vResult;

  vResult += OPC_START_PATH;
  vResult += OPC_PATH_DIVIDER;
  vResult += OPC_PATH_DIVIDER;
  vResult += OPC_PATH_DIVIDER;

  vResult += _File->fGetPath();

  vResult += OPC_PATH_DIVIDER;
  vResult += OPC_PATH_DIVIDER;
  vResult += OPC_PATH_DIVIDER;
  vResult += OPC_END_PATH;

  vResult += OPC_START_DATA;
  vResult += OPC_DATA_DIVIDER;
  vResult += OPC_DATA_DIVIDER;
  vResult += OPC_DATA_DIVIDER;

  vResult += _File->fGetData();

  vResult += OPC_DATA_DIVIDER;
  vResult += OPC_DATA_DIVIDER;
  vResult += OPC_DATA_DIVIDER;
  vResult += OPC_END_DATA;

  return vResult;
}

vector<RFile *> RPackage::fGetDirectoryFiles( const RDirectory *_Directory ) const {
  vector<RFile *> vResult;

  for( RFile *vFile : _Directory->fGetFiles() )
    vResult.push_back( vFile );

  for( const RDirectory *vDirectory : _Directory->fGetDirectories() ) {
    for( RFile *vFile : vDirectory->fGetFiles() )
      vResult.push_back( vFile );

    if( vDirectory->fGetDirectories().size() > 0 ) {
      for( RFile *vFile : fGetDirectoryFiles( vDirectory ) )
        vResult.push_back( vFile );
    }
  }

  return vResult;
}

bool RPackage::fIsPathContainsDirectory( const wstring _Path, const wstring _Directory ) const {
  for( const wstring vPathPart : fSplitPath( _Path ) ) {
    if( vPathPart == _Directory )
      return true;
  }

  return false;
}

vector<wstring> RPackage::fSplitPath( const wstring _Path ) const {
  vector<wstring> vPathSequence;

  wistringstream vPathStream( _Path );
  wstring vPathPart;

  while( getline( vPathStream, vPathPart, L'/' ) ) {
    if( !vPathPart.empty() )
      vPathSequence.push_back( vPathPart );
  }

  return vPathSequence;
}

wstring RPackage::fTruncatePath( const wstring _Path, const wstring _TargetDirectory ) const {
  wstring vTruncatedPath;

  vector<wstring> vGetSplittedPath = fSplitPath( _Path );

  for( size_t c = 0; c < vGetSplittedPath.size(); c++ ) {
    if( vGetSplittedPath [ c ] == _TargetDirectory ) {
      vTruncatedPath += L"/";
      vTruncatedPath.append( vGetSplittedPath [ c ] );

      for( size_t b = c + 1; b < vGetSplittedPath.size(); b++ ) {
        vTruncatedPath += L"/";
        vTruncatedPath.append( vGetSplittedPath [ b ] );
      }

      break;
    }
  }

  return vTruncatedPath;
}

enErrorCode RPackage::fCreatePath( RDirectory *&_Directory,
                                   const vector<wstring> _PathSequence, const size_t _Depth ) const {
  for( RDirectory *&vDirectory : _Directory->fGetDirectories() ) {
    if( vDirectory->fGetName() == _PathSequence [ _Depth ] )
      return fCreatePath( vDirectory, _PathSequence, _Depth + 1 );
  }

  if( _Depth == _PathSequence.size() - 1 )
    _Directory->fAddFile( new RFile( _PathSequence [ _Depth ], L"-" ) );
  else {
    _Directory->fAddDirectory( new RDirectory( _PathSequence [ _Depth ] ) );

    return fCreatePath( _Directory, _PathSequence, _Depth );
  }

  return EC_OK;
}

enErrorCode RPackage::fInsertFileToDirectory( RDirectory *&_Directory,
                                              const wstring _Name, RFile *_File ) const {
  return _Directory->fGetDirectory( _Name, true )->fChangeFile( _File->fGetName(), _File, true ) == nullptr ? EC_INVALID_INSERT : EC_OK;
}
