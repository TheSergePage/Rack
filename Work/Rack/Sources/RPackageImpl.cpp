/*
© 2019, Serge Page.

This license is hereby grants to any person who obtained a copy of this product or the product source files the next rights to:

- Use a compiled version of this product at no cost, without any restrictions, in non-commercial and commercial purposes
- Do reverse-engineering of this product in non-commercial purposes only
- Use source codes of this product at no cost but with the limitations - source codes available only for non-commercial, academic and / or scientific purposes
- Copy and distribute without any fee
- Copy of the original repository and / or create own derivative product for non-commercial,  academic and / or scientific purposes only
- Link the product source code with an another product source code which licensed under any of Dark Orb licenses or one of these licenses:
  - MIT License
  - Microsoft Public License
  - Beerware License
  - Academic Free License
  - WTFPL
  - Unlicense
  - Original BSD license
  - Modified BSD License
  - Simplified BSD License
  - Zero Clause BSD
- Link the product source code with an another product source code if between them no any patent collision

This license is require to:

- Keep the full license text without any changes
- The license text must be included once in a file called 'License' which placed in the root directory of the product and in all source files of the product

This license is deny to:

- Change license of the derivative product
- Use the product’s author name and name of any contributor of this product for advertising derivative software without legally certified permission
- Resell this product
- Use the product or the product source code for any purpose which refers to any government of any country

The product is an original source codes and original compiled files which made by the original author and provided only under the grants and restrictions of this license. All damages which can be happen after and while using the product will not be compensate.
*/

#include "rackLib.hpp"

using namespace Rack;

RPackage::RPackage() noexcept:
  VBuffer( U"" ) {}

RPackage::RPackage( const RPackage &_Copy ) noexcept:
  VBuffer( U"" ), VFlatData( _Copy.FGetFlatData() ) {}

RPackage::RPackage( const vector<RFile *> &_Data ) noexcept:
  VBuffer( U"" ), VFlatData( _Data ) {}

RPackage::~RPackage() {
  FClearBuffer();
  VFlatData.clear();
}

ENErrorCodes RPackage::FCreatePackage( const u32string &_Path,
                                       const vector <RFile *> &_Data ) {
  if( _Path.empty() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( _Data.empty() )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  u32string VSerializedData;

  for( const RFile *VFile : _Data )
    VSerializedData += FSerializeFile( VFile );

  basic_ofstream<char32_t> VFile( _Path + U".rpack", ios::binary | ios::trunc );

  const uint32_t VSourceSize = static_cast< uint32_t >( VSerializedData.length() + 1 );

  char32_t *VFileBuffer = new char32_t [ VSourceSize ];
  VFileBuffer [ VSourceSize ] = U'\0';

  for( uint32_t c = 0; c < VSourceSize - 1; c++ ) {
    VFileBuffer [ c ] = VSerializedData [ c ];
  }

  if( char_traits<char32_t>::length( VFileBuffer ) == 0 ) {
    delete[] VFileBuffer;
    VFileBuffer = nullptr;

    return ENErrorCodes::EC_INVALID_READ;
  }

  VSerializedData.clear();

  VFile.write( VFileBuffer, VSourceSize );

  VFile.close();

  delete[] VFileBuffer;
  VFileBuffer = nullptr;

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FLoadPackage( const u32string &_Path ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  const streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  VFlatData.clear();

  char32_t VControlCharacters [ 4 ];
  while( VFile.get( VControlCharacters [ 0 ] ) ) {
    VFile.get( VControlCharacters [ 1 ] );
    VFile.get( VControlCharacters [ 2 ] );
    VFile.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
      RFile *VGetFile = FParseFile( VFile );

      if( VGetFile != nullptr )
        VFlatData.push_back( VGetFile );
      else {
        VFile.close();

        return ENErrorCodes::EC_INVALID_PARSE;
      }
    } else
      VFile.seekg( -3, ios::cur );
  }

  VFile.close();

  if( VFlatData.empty() )
    return ENErrorCodes::EC_INVALID_PACKAGE;

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FLoadPackage( const u32string &_Path,
                                     vector<RFile *> &_Data ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  const streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  _Data.clear();

  char32_t VControlCharacters [ 4 ];
  while( VFile.get( VControlCharacters [ 0 ] ) ) {
    VFile.get( VControlCharacters [ 1 ] );
    VFile.get( VControlCharacters [ 2 ] );
    VFile.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
      RFile *VGetFile = FParseFile( VFile );

      if( VGetFile != nullptr )
        _Data.push_back( VGetFile );
      else {
        VFile.close();

        return ENErrorCodes::EC_INVALID_PARSE;
      }
    } else
      VFile.seekg( -3, ios::cur );
  }

  VFile.close();

  if( _Data.empty() )
    return ENErrorCodes::EC_INVALID_PACKAGE;

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FFullLoadPackage( const u32string &_Path ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  FClearBuffer();

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  const streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  char32_t *VTemporaryBuffer = new char32_t [ static_cast< uint32_t >( VFileLength ) + 1 ];
  VTemporaryBuffer [ static_cast< uint32_t >( VFileLength ) ] = U'\0';

  VFile.read( VTemporaryBuffer, VFileLength );

  VFile.close();

  if( char_traits<char32_t>::length( VTemporaryBuffer ) == 0 ) {
    delete[] VTemporaryBuffer;
    VTemporaryBuffer = nullptr;

    FClearBuffer();

    return ENErrorCodes::EC_INVALID_READ;
  }

  VBuffer = move( VTemporaryBuffer );

  delete[] VTemporaryBuffer;
  VTemporaryBuffer = nullptr;

  VFlatData.clear();

  try {
    for( uint32_t c = 0; c < static_cast< uint32_t >( VFileLength ) + 1; c++ ) {
      if( VBuffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
          && VBuffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
          && VBuffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
          && VBuffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
        RFile *VGetFile = FParseFile( c );

        if( VGetFile != nullptr )
          VFlatData.push_back( VGetFile );
        else
          return ENErrorCodes::EC_INVALID_PARSE;
      }
    }
  } catch( ... ) {
    return ENErrorCodes::EC_INVALID_PARSE;
  }

  FClearBuffer();

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FFullLoadPackage( const u32string &_Path,
                                         vector<RFile *> &_Data ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  FClearBuffer();

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  char32_t *VTemporaryBuffer = new char32_t [ static_cast< uint32_t >( VFileLength ) + 1 ];
  VTemporaryBuffer [ static_cast< uint32_t >( VFileLength ) ] = U'\0';

  VFile.read( VTemporaryBuffer, VFileLength );

  VFile.close();

  if( char_traits<char32_t>::length( VTemporaryBuffer ) == 0 ) {
    delete[] VTemporaryBuffer;
    VTemporaryBuffer = nullptr;

    FClearBuffer();

    return ENErrorCodes::EC_INVALID_READ;
  }

  VBuffer = move( VTemporaryBuffer );

  delete[] VTemporaryBuffer;
  VTemporaryBuffer = nullptr;

  _Data.clear();

  try {
    for( uint32_t c = 0; c < static_cast< uint32_t >( VFileLength ) + 1; c++ ) {
      if( VBuffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
          && VBuffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
          && VBuffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
          && VBuffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
        RFile *VGetFile = FParseFile( c );

        if( VGetFile != nullptr )
          _Data.push_back( VGetFile );
        else
          return ENErrorCodes::EC_INVALID_PARSE;
      }
    }
  } catch( ... ) {
    return ENErrorCodes::EC_INVALID_PARSE;
  }

  FClearBuffer();

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FSavePackage( const u32string &_Path ) {
  if( _Path.empty() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( VFlatData.empty() )
    return ENErrorCodes::EC_EMPTY_DATA;

  u32string VSerializedData;

  for( const RFile *VFile : VFlatData )
    VSerializedData += FSerializeFile( VFile );

  basic_ofstream<char32_t> VFile( _Path + U".rpack", ios::binary | ios::trunc );

  VFile.write( VSerializedData.c_str(), VSerializedData.length() );

  VFile.close();

  VSerializedData.clear();

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FSavePackage( const u32string &_Path,
                                     const vector <RFile *> &_Data ) {
  if( _Path.empty() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( _Data.empty() )
    return ENErrorCodes::EC_EMPTY_DATA;

  u32string VSerializedData;

  for( const RFile *VFile : _Data )
    VSerializedData += FSerializeFile( VFile );

  basic_ofstream<char32_t> VFile( _Path + U".rpack", ios::binary | ios::trunc );

  VFile.write( VSerializedData.c_str(), VSerializedData.length() );

  VFile.close();

  VSerializedData.clear();

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FMakeFlatDirectory( vector<RFile *> &_Destination,
                                           const RDirectory *_Directory ) const {
  if( _Directory == nullptr )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  _Destination.clear();

  for( RFile *VFile : FGetDirectoryFiles( _Directory ) )
    _Destination.push_back( VFile );

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FMakePackage( vector<RFile *> &_Destination,
                                     const vector<RDirectory *> &_Directories ) const {
  if( _Directories.empty() )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  _Destination.clear();

  for( const RDirectory *VDirectory : _Directories ) {
    for( RFile *VFile : FGetDirectoryFiles( VDirectory ) )
      _Destination.push_back( VFile );
  }

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FReadStructure( const u32string &_Path,
                                       RDirectory *_Destination ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  const streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  vector<u32string> VPaths;

  char32_t VControlCharacters [ 4 ];
  while( VFile.get( VControlCharacters [ 0 ] ) ) {
    VFile.get( VControlCharacters [ 1 ] );
    VFile.get( VControlCharacters [ 2 ] );
    VFile.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
      u32string VGetFilePath = FParseFilePath( VFile );

      if( !VGetFilePath.empty() )
        VPaths.push_back( VGetFilePath );
      else {
        VFile.close();

        return ENErrorCodes::EC_INVALID_PARSE;
      }
    } else
      VFile.seekg( -3, ios::cur );
  }

  VFile.close();

  for( const u32string VPath : VPaths )
    FCreatePath( _Destination, FSplitPath( VPath ), 0 );

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FExtractFile( const u32string &_Path,
                                     const u32string &_Name,
                                     RFile *_Destination ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  const streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  char32_t VControlCharacters [ 4 ];
  while( VFile.get( VControlCharacters [ 0 ] ) ) {
    VFile.get( VControlCharacters [ 1 ] );
    VFile.get( VControlCharacters [ 2 ] );
    VFile.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
      RFile *VGetFile = FParseFile( VFile );

      if( VGetFile == nullptr ) {
        VFile.close();

        return ENErrorCodes::EC_INVALID_PARSE;
      }

      if( VGetFile->FGetName() == _Name ) {
        _Destination = VGetFile;

        VFile.close();

        return ENErrorCodes::EC_OK;
      }
    } else
      VFile.seekg( -3, ios::cur );
  }

  VFile.close();

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FExtractDirectory( const u32string &_Path,
                                          const u32string &_Name,
                                          RDirectory *_Destination ) {
  if( _Path.empty() ||
      !exists( _Path ) || !directory_entry( _Path ).is_regular_file() )
    return ENErrorCodes::EC_INVALID_PATH;

  if( FIsRackFile( _Path ) != ENExtensions::DefaultExtension )
    return ENErrorCodes::EC_NOT_PACKAGE;

  basic_ifstream<char32_t> VFile( _Path, ios::binary );

  VFile.seekg( 0, SEEK_END );
  const streampos VFileLength = VFile.tellg();
  VFile.seekg( 0 );

  if( VFileLength == 0 ) {
    VFile.close();

    return ENErrorCodes::EC_EMPTY_FILE;
  }

  char32_t VControlCharacters [ 4 ];
  while( VFile.get( VControlCharacters [ 0 ] ) ) {
    VFile.get( VControlCharacters [ 1 ] );
    VFile.get( VControlCharacters [ 2 ] );
    VFile.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) ) {
      RFile *VGetFile = FParseFile( VFile );

      if( VGetFile == nullptr ) {
        VFile.close();

        return ENErrorCodes::EC_INVALID_PARSE;
      }

      if( FIsPathContainsDirectory( VGetFile->FGetPath(), _Name ) ) {
        FCreatePath( _Destination,
                     FSplitPath( FTruncatePath( VGetFile->FGetPath(), _Name ) ), 0 );

        VGetFile->FSetPath( VGetFile->FGetName() );

        if( FInsertFileToDirectory( _Destination, _Name, VGetFile ) != ENErrorCodes::EC_OK )
          return ENErrorCodes::EC_INVALID_INSERT;
      }
    } else
      VFile.seekg( -3, ios::cur );
  }

  VFile.close();

  return ENErrorCodes::EC_OK;
}

RFile *RPackage::FParseFile( basic_ifstream<char32_t> &_FileStream ) const {
  u32string VPath, VData;

  char32_t VControlCharacters [ 4 ];
  while( _FileStream.get( VControlCharacters [ 0 ] ) ) {
    _FileStream.get( VControlCharacters [ 1 ] );
    _FileStream.get( VControlCharacters [ 2 ] );
    _FileStream.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_PATH ) )
      break;
    else {
      _FileStream.seekg( -3, ios::cur );

      VPath += VControlCharacters [ 0 ];
    }
  }

  if( VPath.empty() )
    return nullptr;

  _FileStream.get( VControlCharacters [ 0 ] );
  _FileStream.get( VControlCharacters [ 1 ] );
  _FileStream.get( VControlCharacters [ 2 ] );
  _FileStream.get( VControlCharacters [ 3 ] );

  if( VControlCharacters [ 0 ] != static_cast< char32_t >( ENOpcodes::OPC_START_DATA )
      && VControlCharacters [ 1 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
      && VControlCharacters [ 2 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
      && VControlCharacters [ 3 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER ) ) {
    VPath.clear();

    return nullptr;
  }

  while( _FileStream.get( VControlCharacters [ 0 ] ) ) {
    _FileStream.get( VControlCharacters [ 1 ] );
    _FileStream.get( VControlCharacters [ 2 ] );
    _FileStream.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_DATA ) )
      break;
    else {
      _FileStream.seekg( -3, ios::cur );

      VData += VControlCharacters [ 0 ];
    }
  }

  if( VData.length() == 0 ) {
    VPath.clear();

    return nullptr;
  }

  return new RFile( VPath, VData.c_str() );
}

RFile *RPackage::FParseFile( uint32_t &_Index ) {
  u32string VPath, VData;

  uint32_t c = _Index + 4;

  for( ; c < VBuffer.length(); c++ ) {
    if( VBuffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VBuffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VBuffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VBuffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_PATH ) ) {
      c += 4;
      break;
    } else
      VPath += VBuffer [ c ];
  }

  if( VPath.empty() )
    return nullptr;

  if( VBuffer [ c ] != static_cast< char32_t >( ENOpcodes::OPC_START_DATA )
      && VBuffer [ c + 1 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
      && VBuffer [ c + 2 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
      && VBuffer [ c + 3 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER ) ) {
    VPath.clear();

    return nullptr;
  }

  c += 4;

  for( ; c < VBuffer.length(); c++ ) {
    if( VBuffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && VBuffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && VBuffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && VBuffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_DATA ) ) {
      c += 3;
      break;
    } else
      VData += VBuffer [ c ];
  }

  if( VData.length() == 0 ) {
    VPath.clear();

    return nullptr;
  }

  _Index = c;

  return new RFile( VPath, VData.c_str() );
}

RFile *RPackage::FParseFile( const u32string &_Buffer ) const {
  if( _Buffer [ 0 ] != static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
      && _Buffer [ 1 ] != static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
      && _Buffer [ 2 ] != static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
      && _Buffer [ 3 ] != static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) )
    return nullptr;

  u32string VPath, VData;

  uint32_t c = 4;

  for( ; c < _Buffer.length(); c++ ) {
    if( _Buffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && _Buffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && _Buffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && _Buffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_PATH ) ) {
      c += 4;
      break;
    } else
      VPath += _Buffer [ c ];
  }

  if( VPath.empty() )
    return nullptr;

  if( _Buffer [ c ] != static_cast< char32_t >( ENOpcodes::OPC_START_DATA )
      && _Buffer [ c + 1 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
      && _Buffer [ c + 2 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
      && _Buffer [ c + 3 ] != static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER ) ) {
    VPath.clear();

    return nullptr;
  }

  c += 4;

  for( ; c < _Buffer.length(); c++ ) {
    if( _Buffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && _Buffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && _Buffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER )
        && _Buffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_DATA ) ) {
      c += 3;
      break;
    } else
      VData += _Buffer [ c ];
  }

  if( VData.length() == 0 ) {
    VPath.clear();

    return nullptr;
  }

  return new RFile( VPath, VData.c_str() );
}

u32string RPackage::FParseFilePath( basic_ifstream<char32_t> &_FileStream ) const {
  u32string VPath;

  char32_t VControlCharacters [ 4 ];
  while( _FileStream.get( VControlCharacters [ 0 ] ) ) {
    _FileStream.get( VControlCharacters [ 1 ] );
    _FileStream.get( VControlCharacters [ 2 ] );
    _FileStream.get( VControlCharacters [ 3 ] );

    if( VControlCharacters [ 0 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && VControlCharacters [ 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_PATH ) )
      break;
    else {
      _FileStream.seekg( -3, ios::cur );

      VPath += VControlCharacters [ 0 ];
    }
  }

  if( VPath.empty() )
    return U"";

  return VPath;
}

u32string RPackage::FParseFilePath( const u32string &_Buffer ) const {
  if( _Buffer [ 0 ] != static_cast< char32_t >( ENOpcodes::OPC_START_PATH )
      && _Buffer [ 1 ] != static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
      && _Buffer [ 2 ] != static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
      && _Buffer [ 3 ] != static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER ) )
    return U"";

  u32string VPath;

  uint32_t c = 4;

  for( ; c < _Buffer.length(); c++ ) {
    if( _Buffer [ c ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && _Buffer [ c + 1 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && _Buffer [ c + 2 ] == static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER )
        && _Buffer [ c + 3 ] == static_cast< char32_t >( ENOpcodes::OPC_END_PATH ) )
      break;
    else
      VPath += _Buffer [ c ];
  }

  if( VPath.empty() )
    return U"";

  return VPath;
}

u32string RPackage::FExtractFileName( const u32string &_Path ) const {
  u32string VResult;

  for( uint32_t c = static_cast< uint32_t >( _Path.length() - 1 ); c > 0; c-- ) {
    if( _Path [ c ] == U'/' )
      break;
    else
      VResult.insert( VResult.begin(), _Path [ c ] );
  }

  return VResult;
}

u32string RPackage::FSerializeFile( const RFile *_File ) const {
  u32string VResult;

  VResult += static_cast< char32_t >( ENOpcodes::OPC_START_PATH );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER );

  VResult += _File->FGetPath();

  VResult += static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_PATH_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_END_PATH );

  VResult += static_cast< char32_t >( ENOpcodes::OPC_START_DATA );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER );

  VResult += _File->FGetData();

  VResult += static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_DATA_DIVIDER );
  VResult += static_cast< char32_t >( ENOpcodes::OPC_END_DATA );

  return VResult;
}

vector<RFile *> RPackage::FGetDirectoryFiles( const RDirectory *_Directory ) const {
  vector<RFile *> VResult;

  for( RFile *VFile : _Directory->FGetFiles() )
    VResult.push_back( VFile );

  for( const RDirectory *VDirectory : _Directory->FGetDirectories() ) {
    for( RFile *VFile : VDirectory->FGetFiles() )
      VResult.push_back( VFile );

    if( VDirectory->FGetDirectories().size() > 0 ) {
      for( RFile *VFile : FGetDirectoryFiles( VDirectory ) )
        VResult.push_back( VFile );
    }
  }

  return VResult;
}

bool RPackage::FIsPathContainsDirectory( const u32string &_Path, const u32string &_Directory ) const {
  for( const u32string VPathPart : FSplitPath( _Path ) ) {
    if( VPathPart == _Directory )
      return true;
  }

  return false;
}

vector<u32string> RPackage::FSplitPath( const u32string &_Path ) const {
  vector<u32string> VPathSequence;

  basic_istringstream VPathStream( _Path );
  u32string VPathPart;

  while( getline( VPathStream, VPathPart, U'/' ) ) {
    if( !VPathPart.empty() )
      VPathSequence.push_back( VPathPart );
  }

  return VPathSequence;
}

u32string RPackage::FTruncatePath( const u32string &_Path, const u32string &_TargetDirectory ) const {
  u32string VTruncatedPath;

  vector<u32string> VGetSplittedPath = FSplitPath( _Path );

  for( uint32_t c = 0; c < VGetSplittedPath.size(); c++ ) {
    if( VGetSplittedPath [ c ] == _TargetDirectory ) {
      VTruncatedPath += U"/";
      VTruncatedPath.append( VGetSplittedPath [ c ] );

      for( uint32_t b = c + 1; b < VGetSplittedPath.size(); b++ ) {
        VTruncatedPath += U"/";
        VTruncatedPath.append( VGetSplittedPath [ b ] );
      }

      break;
    }
  }

  return VTruncatedPath;
}

ENErrorCodes RPackage::FCreatePath( RDirectory *_Directory,
                                    const vector<u32string> &_PathSequence, const uint32_t _Depth ) const {
  for( RDirectory *VDirectory : _Directory->FGetDirectories() ) {
    if( VDirectory->FGetName() == _PathSequence [ _Depth ] )
      return FCreatePath( VDirectory, _PathSequence, _Depth + 1 );
  }

  if( _Depth == _PathSequence.size() - 1 )
    _Directory->FAddFile( new RFile( _PathSequence [ _Depth ], U"-" ) );
  else {
    _Directory->FAddDirectory( new RDirectory( _PathSequence [ _Depth ] ) );

    return FCreatePath( _Directory, _PathSequence, _Depth );
  }

  return ENErrorCodes::EC_OK;
}

ENErrorCodes RPackage::FInsertFileToDirectory( RDirectory *_Directory,
                                               const u32string &_Name, RFile *_File ) const {
  return _Directory->FGetDirectory( _Name, true )->FChangeFile( _File->FGetName(), _File, true ) == nullptr ? ENErrorCodes::EC_INVALID_INSERT : ENErrorCodes::EC_OK;
}
