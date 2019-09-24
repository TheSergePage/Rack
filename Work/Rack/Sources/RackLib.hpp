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

#pragma once

#include "RHeader.hpp"

namespace Rack {
  typedef enum {
    EC_INVALID_ARGUMENT = numeric_limits<long>::lowest(),
    EC_INVALID_PATH,
    EC_INVALID_READ,
    EC_INVALID_PARSE,
    EC_INVALID_READ_ENCODED_BLOCK,
    EC_INVALID_PACKAGE,
    EC_INVALID_KEY,
    EC_INVALID_INSERT,

    EC_FILE_NOT_FOUND,
    EC_DIRECTORY_NOT_FOUND,

    EC_NOT_PACKAGE,

    EC_EMPTY_FILE,
    EC_EMPTY_DATA,

    EC_OK = 0
  } enErrorCode;

  class RFile {
    public:
    explicit RFile() noexcept;
    explicit RFile( const RFile &_Copy ) noexcept;
    explicit RFile( wstring _Path, const wchar_t *_Data ) noexcept;
    ~RFile();

    inline wstring fGetPath() const noexcept { return vPath; }

    inline void fSetPath( wstring _Path ) noexcept { vPath = _Path; }

    inline wchar_t *fGetData() const { return vData; }

    inline void fSetData( const wchar_t *_Data ) {
      fClearData();

      size_t vSourceSize = wcslen( _Data ) + 1;
      vData = new wchar_t [ vSourceSize ];
      wcscpy_s( vData, vSourceSize, _Data );
    }

    inline void fClearData() {
      if( vData != nullptr ) {
        delete[] vData;
        vData = nullptr;
      }
    }

    inline wstring fGetName() const noexcept { return path( vPath ).filename(); }

    inline wstring fGetExtension() const noexcept { return path( vPath ).extension(); }

    inline wstring fGetRootDirectory() const {
      vector<wstring> vGetSplittedPath = fSplitPath( path( vPath ).generic_wstring() );

      if( vGetSplittedPath.size() > 1 ) { return vGetSplittedPath.at( distance( vGetSplittedPath.begin(), vGetSplittedPath.end() ) - 2 ); } else
        return L"";
    }

    inline wstring fGetRelativePath() const noexcept { return path( vPath ).relative_path(); }

    private:
    wstring vPath;
    wchar_t *vData;

    inline vector<wstring> fSplitPath( const wstring _Path ) const {
      vector<wstring> vResult;

      wstring vPart;
      for( const wchar_t vCharacter : _Path ) {
        if( vCharacter == path::preferred_separator ) {
          vResult.insert( vResult.end(), vPart );
          vPart.clear();
        } else {
          vPart += vCharacter;
        }
      }

      return vResult;
    }
  };

  class RDirectory {
    public:
    explicit RDirectory() noexcept;
    explicit RDirectory( const RDirectory &_Copy ) noexcept;
    explicit RDirectory( wstring _Name );
    explicit RDirectory( wstring _Name,
                         vector<RDirectory *> _Directories, vector<RFile *> _Files ) noexcept;
    ~RDirectory();

    inline wstring fGetName() const noexcept { return vName; }

    inline void fSetName( wstring _Name ) noexcept { vName = _Name; }

    inline vector<RDirectory *> fGetDirectories() const noexcept { return vDirectories; }

    inline void fSetDirectories( vector<RDirectory *> _Directories ) noexcept { vDirectories = _Directories; }

    inline void fClearDirectories() noexcept { vDirectories.clear(); }

    inline vector<RFile *> fGetFiles() const noexcept { return vFiles; }

    inline void fSetFiles( vector<RFile *> _Files ) noexcept { vFiles = _Files; }

    inline void fClearFiles() noexcept { vFiles.clear(); }

    //Files

    inline RFile *fGetFile( const size_t _Index ) const {
      if( _Index > vFiles.size() )
        return nullptr;

      return vFiles [ _Index ];
    }

    RFile *fGetFile( const wstring _Name, const bool _Deep = false ) const;
    RFile *fGetFile( const wchar_t *_Data, const bool _Deep = false ) const;
    RFile *fGetFile( const RFile *_File, const bool _Deep = false ) const;

    inline enErrorCode fAddFile( RFile *_File ) {
      if( _File == nullptr )
        return EC_INVALID_ARGUMENT;

      vFiles.push_back( _File );

      return EC_OK;
    }

    inline RFile *fRemoveFile( const size_t _Index ) {
      if( _Index >= vFiles.size() )
        return nullptr;

      RFile *vGetFile = vFiles [ _Index ];

      vFiles.erase( vFiles.begin() + _Index );

      return vGetFile;
    }

    RFile *fRemoveFile( const wstring _Name, const bool _Deep = false );
    RFile *fRemoveFile( const wchar_t *_Data, const bool _Deep = false );
    RFile *fRemoveFile( const RFile *_File, const bool _Deep = false );

    inline enErrorCode fInsertFile( RFile *_File, const size_t _Index ) {
      if( _File == nullptr || _Index >= vFiles.size() )
        return EC_INVALID_ARGUMENT;

      vFiles.insert( vFiles.begin() + _Index, _File );

      return EC_OK;
    }

    inline long fGetFileIndex( const wstring _Name ) const {
      for( size_t c = 0; c < vFiles.size(); c++ ) {
        if( vFiles [ c ]->fGetName() == _Name )
          return static_cast< long >( c );
      }

      return EC_FILE_NOT_FOUND;
    }

    inline long fGetFileIndex( const wchar_t *_Data ) const {
      if( _Data == nullptr )
        return EC_INVALID_ARGUMENT;

      for( size_t c = 0; c < vFiles.size(); c++ ) {
        if( wcscmp( vFiles [ c ]->fGetData(), _Data ) == 0 )
          return static_cast< long >( c );
      }

      return EC_FILE_NOT_FOUND;
    }

    inline RFile *fChangeFile( const size_t _Index, RFile *_Substitute ) {
      if( _Index >= vFiles.size() || _Substitute == nullptr )
        return nullptr;

      RFile *vGetFile = vFiles [ _Index ];

      vFiles [ _Index ] = _Substitute;

      return vGetFile;
    }

    RFile *fChangeFile( const wstring _Name, RFile *_Substitute, const bool _Deep = false );
    RFile *fChangeFile( const wchar_t *_Data, RFile *_Substitute, const bool _Deep = false );

    // Directories

    inline RDirectory *fGetDirectory( const size_t _Index ) const {
      if( _Index >= vDirectories.size() )
        return nullptr;

      return vDirectories [ _Index ];
    }

    RDirectory *fGetDirectory( const wstring _Name, const bool _Deep = false ) const;
    RDirectory *fGetDirectory( const RFile *_File, const bool _Deep = false ) const;
    RDirectory *fGetDirectory( const vector<RFile *> _Files, const bool _Deep = false ) const;
    RDirectory *fGetDirectory( const RDirectory *_Directory, const bool _Deep = false ) const;
    RDirectory *fGetDirectory( const vector<RDirectory *> _Directories, const bool _Deep = false ) const;

    inline enErrorCode fAddDirectory( RDirectory *_Directory ) {
      if( _Directory == nullptr )
        return EC_INVALID_ARGUMENT;

      vDirectories.push_back( _Directory );

      return EC_OK;
    }

    inline RDirectory *fRemoveDirectory( const size_t _Index ) {
      if( _Index >= vFiles.size() )
        return nullptr;

      RDirectory *vGetDirectory = vDirectories [ _Index ];

      vDirectories.erase( vDirectories.begin() + _Index );

      return vGetDirectory;
    }

    RDirectory *fRemoveDirectory( const wstring _Name, const bool _Deep = false );
    RDirectory *fRemoveDirectory( const RFile *_File, const bool _Deep = false );
    RDirectory *fRemoveDirectory( const vector<RFile *> _Files, const bool _Deep = false );
    RDirectory *fRemoveDirectory( const RDirectory *_Directory, const bool _Deep = false );
    RDirectory *fRemoveDirectory( const vector<RDirectory *> _Directories, const bool _Deep = false );

    inline enErrorCode fInsertDirectory( RDirectory *_Directory, const size_t _Index ) {
      if( _Directory == nullptr || _Index >= vDirectories.size() )
        return EC_INVALID_ARGUMENT;

      vDirectories.insert( vDirectories.begin() + _Index, _Directory );

      return EC_OK;
    }

    inline long fGetDirectoryIndex( const wstring _Name ) const {
      for( size_t c = 0; c < vDirectories.size(); c++ ) {
        if( vDirectories [ c ]->fGetName() == _Name )
          return static_cast< long >( c );
      }

      return EC_DIRECTORY_NOT_FOUND;
    }

    inline long fGetDirectoryIndex( const RFile *_File ) const {
      if( _File == nullptr )
        return EC_INVALID_ARGUMENT;

      for( size_t c = 0; c < vDirectories.size(); c++ ) {
        if( vDirectories [ c ]->fGetFile( _File ) != nullptr )
          return static_cast< long >( c );
      }

      return EC_DIRECTORY_NOT_FOUND;
    }

    inline long fGetDirectoryIndex( const vector<RFile *> _Files ) const {
      if( _Files.empty() )
        return EC_INVALID_ARGUMENT;

      for( size_t c = 0; c < vDirectories.size(); c++ ) {
        for( const RFile *vFile : _Files ) {
          if( vDirectories [ c ]->fGetFile( vFile ) != nullptr )
            return static_cast< long >( c );
        }
      }

      return EC_DIRECTORY_NOT_FOUND;
    }

    inline long fGetDirectoryIndex( const RDirectory *_Directory ) const {
      if( _Directory == nullptr )
        return EC_INVALID_ARGUMENT;

      for( size_t c = 0; c < vDirectories.size(); c++ ) {
        if( vDirectories [ c ]->fGetDirectory( _Directory ) != nullptr )
          return static_cast< long >( c );
      }

      return EC_DIRECTORY_NOT_FOUND;
    }

    inline long fGetDirectoryIndex( const vector<RDirectory *> _Directories ) const {
      if( _Directories.empty() )
        return EC_INVALID_ARGUMENT;

      for( size_t c = 0; c < vDirectories.size(); c++ ) {
        for( const RDirectory *vDirectory : _Directories ) {
          if( vDirectories [ c ]->fGetDirectory( vDirectory ) != nullptr )
            return static_cast< long >( c );
        }
      }

      return EC_DIRECTORY_NOT_FOUND;
    }

    inline RDirectory *fChangeDirectory( const size_t _Index, RDirectory *_Substitute ) {
      if( _Index >= vDirectories.size() || _Substitute == nullptr )
        return nullptr;

      RDirectory *vGetDirectory = vDirectories [ _Index ];

      vDirectories [ _Index ] = _Substitute;

      return vGetDirectory;
    }

    RDirectory *fChangeDirectory( const wstring _Name, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *fChangeDirectory( const RFile *_File, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *fChangeDirectory( const vector<RFile *> _Files, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *fChangeDirectory( const RDirectory *_Directory, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *fChangeDirectory( const vector<RDirectory *> _Directories, RDirectory *_Substitute, const bool _Deep = false );

    private:
    wstring vName;

    vector<RDirectory *> vDirectories;
    vector<RFile *> vFiles;
  };

  class RPackage {
    public:
    typedef enum {
      OPC_START_ENCODED_BLOCK = 1,
      OPC_ENCODED_BLOCK_DIVIDER,
      OPC_END_ENCODED_BLOCK,

      OPC_START_PATH,
      OPC_PATH_DIVIDER,
      OPC_END_PATH,

      OPC_START_DATA,
      OPC_DATA_DIVIDER,
      OPC_END_DATA
    } enOpcode;

    explicit RPackage() noexcept;
    explicit RPackage( const RPackage &_Copy ) noexcept;
    explicit RPackage( vector<RFile *> _Data ) noexcept;
    ~RPackage();

    inline vector<RFile *> fGetFlatData() const noexcept { return vFlatData; }

    inline void fSetFlatData( vector<RFile *> _FlatData ) noexcept { vFlatData = _FlatData; }

    inline void fClearFlatData() noexcept { vFlatData.clear(); }

    enErrorCode fCreatePackage( const wstring _Path,
                                const vector <RFile *> _Data );

    enErrorCode fLoadPackage( const wstring _Path );
    enErrorCode fLoadPackage( const wstring _Path,
                              vector<RFile *> &_Data );

    enErrorCode fFullLoadPackage( const wstring _Path );
    enErrorCode fFullLoadPackage( const wstring _Path,
                                  vector<RFile *> &_Data );

    enErrorCode fSavePackage( const wstring _Path );
    enErrorCode fSavePackage( const wstring _Path,
                              const vector <RFile *> _Data );

    enErrorCode fMakeFlatDirectory( vector<RFile *> &_Destination,
                                    const RDirectory *_Directory ) const;
    enErrorCode fMakePackage( vector<RFile *> &_Destination,
                              const vector<RDirectory *> _Directories ) const;

    enErrorCode fReadStructure( const wstring _Path,
                                RDirectory *&_Destination );

    enErrorCode fExtractFile( const wstring _Path,
                              const wstring _Name,
                              RFile *&_Destination );

    enErrorCode fExtractDirectory( const wstring _Path,
                                   const wstring _Name,
                                   RDirectory *&_Destination );

    //Utilities

    bool fIsPathContainsDirectory( const wstring _Path, const wstring _Directory ) const;
    vector<wstring> fSplitPath( const wstring _Path ) const;

    private:
    const wchar_t *vDefaultExtension = L".rpack";

    typedef enum {
      InvalidExtension = -1,

      DefaultExtension
    } enExtension;

    inline enExtension fIsRackFile( const wstring _Path ) const noexcept {
      wstring vGetExtension = path( _Path ).extension();

      if( vGetExtension == vDefaultExtension )
        return DefaultExtension;
      else
        return InvalidExtension;
    }

    wchar_t *vBuffer;

    inline void fClearBuffer() {
      if( vBuffer != nullptr ) {
        delete[] vBuffer;
        vBuffer = nullptr;
      }
    }

    vector<RFile *> vFlatData;

    RFile *fParseFile( wifstream &_FileStream ) const;
    RFile *fParseFile( size_t &_Index );
    RFile *fParseFile( const wchar_t *_Buffer ) const;

    wstring fParseFilePath( wifstream &_FileStream ) const;
    wstring fParseFilePath( const wchar_t *_Buffer ) const;

    wstring fExtractFileName( const wstring _Path ) const;

    wstring fSerializeFile( const RFile *_File ) const;

    vector<RFile *> fGetDirectoryFiles( const RDirectory *_Directory ) const;

    wstring fTruncatePath( const wstring _Path, const wstring _TargetDirectory ) const;
    enErrorCode fCreatePath( RDirectory *&_Directory,
                             const vector<wstring> _PathSequence, const size_t _Depth ) const;

    enErrorCode fInsertFileToDirectory( RDirectory *&_Directory,
                                        const wstring _Name, RFile *_File ) const;
  };
}
