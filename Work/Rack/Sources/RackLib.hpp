/*
� 2019, Dark Orb.


This license is hereby grants to any person who obtained a copy of this software the next rights to:

- Use a compiled Version of this software at no cost, without any restrictions, in non-commercial and commercial purposes
- Do reverse-engineering of compiled Version of this software in non-commercial purposes only
- Use source codes of this software at no cost but with the limitations - source codes available only for non-commercial, academic and / or scientific purposes
- Copy and distribute without any Fee
- Copy of the original repository and / or create own derivative software for non-commercial,  academic and / or scientific purposes only


This license is require to:

- Keep the Full license text without any changes
- The license text must be included once in a File called 'License' which placed in the root directory of the software and in all source Files of the software


This license is deny to:

- Change license of the derivative software
- Use the copyright holder name and name of any contributor of this software for advertising derivative software without legally certified permission
- Sell this software


The product is an original source codes and original compiled Files which made by the original author and provided only under the grants and restrictions of this license.
All damages which can be happen after and while using the product will not be compensate.
*/


#pragma once

#include "RHeader.hpp"

#include "Unicoder.hpp"

using namespace Unicoder;

namespace Rack {
  enum class ENErrorCodes: int64_t {
    EC_INVALID_ARGUMENT = numeric_limits<int64_t>::lowest(),
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
  };

  class RFile {
    public:
    explicit RFile() noexcept;
    explicit RFile( const RFile &_Copy ) noexcept;
    explicit RFile( const u32string &_Path, const u32string &_Data ) noexcept;
    ~RFile();

    inline u32string FGetPath() const noexcept {
      return VPath;
    }

    inline void FSetPath( const u32string &_Path ) noexcept {
      VPath = _Path;
    }

    inline void FClearPath() noexcept {
      VPath.clear();
    }

    inline u32string FGetData() const noexcept {
      return VData;
    }

    inline void FSetData( const u32string &_Data ) noexcept {
      VData = _Data;
    }

    inline void FClearData() noexcept {
      VData.clear();
    }

    inline u32string FGetName() const noexcept {
      return path( UConverter::FConvert<u32string, char32_t, u32string, char32_t>( VPath ) ).filename().u32string();
    }

    inline u32string FGetExtension() const noexcept {
      return path( UConverter::FConvert<u32string, char32_t, u32string, char32_t>( VPath ) ).extension().u32string();
    }

    inline u32string FGetRootDirectory() const noexcept {
      vector<u32string> VGetSplittedPath = FSplitPath( path( UConverter::FConvert<u32string, char32_t, u32string, char32_t>( VPath ) ).u32string() );

      if( VGetSplittedPath.size() > 1 ) { return VGetSplittedPath.at( distance( VGetSplittedPath.begin(), VGetSplittedPath.end() ) - 2 ); } else
        return U"";
    }

    inline u32string FGetRelativePath() const noexcept {
      return path( UConverter::FConvert<u32string, char32_t, u32string, char32_t>( VPath ) ).relative_path().u32string();
    }

    private:
    u32string VPath;
    u32string VData;

    vector<u32string> FSplitPath( const u32string &_Path ) const noexcept;
  };

  class RDirectory {
    public:
    explicit RDirectory() noexcept;
    explicit RDirectory( const RDirectory &_Copy ) noexcept;
    explicit RDirectory( const u32string &_Name ) noexcept;
    explicit RDirectory( const u32string &_Name,
                         const vector<RDirectory *> &_Directories, const vector<RFile *> &_Files ) noexcept;
    ~RDirectory();

    inline u32string FGetName() const noexcept { return VName; }
    inline void FSetName( u32string &_Name ) noexcept { VName = _Name; }

    inline vector<RDirectory *> FGetDirectories() const noexcept { return VDirectories; }
    inline void FSetDirectories( const vector<RDirectory *> &_Directories ) noexcept { VDirectories = _Directories; }

    inline void FClearDirectories() noexcept { VDirectories.clear(); }

    inline vector<RFile *> FGetFiles() const noexcept { return VFiles; }
    inline void FSetFiles( const vector<RFile *> &_Files ) noexcept { VFiles = _Files; }

    inline void FClearFiles() noexcept { VFiles.clear(); }

    //Files

    RFile *FGetFile( const uint32_t _Index ) const;
    RFile *FGetFile( const u32string &_Name, const bool _Deep = false ) const;
    RFile *FGetFile( const RFile *_File, const bool _Deep = false ) const;

    ENErrorCodes FAddFile( RFile *_File );

    RFile *FRemoveFile( const uint32_t _Index );
    RFile *FRemoveFile( const u32string &_Name, const bool _Deep = false );
    RFile *FRemoveFile( const RFile *_File, const bool _Deep = false );

    ENErrorCodes FInsertFile( RFile *_File, const uint32_t _Index );

    int64_t FGetFileIndex( const u32string &_Name ) const;

    RFile *FChangeFile( const uint32_t _Index, RFile *_Substitute );
    RFile *FChangeFile( const u32string &_Name, RFile *_Substitute, const bool _Deep = false );

    // Directories

    RDirectory *FGetDirectory( const uint32_t _Index ) const;
    RDirectory *FGetDirectory( const u32string &_Name, const bool _Deep = false ) const;
    RDirectory *FGetDirectory( const RFile *_File, const bool _Deep = false ) const;
    RDirectory *FGetDirectory( const vector<RFile *> &_Files, const bool _Deep = false ) const;
    RDirectory *FGetDirectory( const RDirectory *_Directory, const bool _Deep = false ) const;
    RDirectory *FGetDirectory( const vector<RDirectory *> &_Directories, const bool _Deep = false ) const;

    ENErrorCodes FAddDirectory( RDirectory *_Directory );

    RDirectory *FRemoveDirectory( const uint32_t _Index );
    RDirectory *FRemoveDirectory( const u32string &_Name, const bool _Deep = false );
    RDirectory *FRemoveDirectory( const RFile *_File, const bool _Deep = false );
    RDirectory *FRemoveDirectory( const vector<RFile *> &_Files, const bool _Deep = false );
    RDirectory *FRemoveDirectory( const RDirectory *_Directory, const bool _Deep = false );
    RDirectory *FRemoveDirectory( const vector<RDirectory *> &_Directories, const bool _Deep = false );

    ENErrorCodes FInsertDirectory( RDirectory *_Directory, const uint32_t _Index );

    int64_t FGetDirectoryIndex( const u32string &_Name ) const;
    int64_t FGetDirectoryIndex( const RFile *_File ) const;
    int64_t FGetDirectoryIndex( const vector<RFile *> &_Files ) const;
    int64_t FGetDirectoryIndex( const RDirectory *_Directory ) const;
    int64_t FGetDirectoryIndex( const vector<RDirectory *> &_Directories ) const;

    RDirectory *FChangeDirectory( const uint32_t _Index, RDirectory *_Substitute );
    RDirectory *FChangeDirectory( const u32string &_Name, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *FChangeDirectory( const RFile *_File, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *FChangeDirectory( const vector<RFile *> &_Files, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *FChangeDirectory( const RDirectory *_Directory, RDirectory *_Substitute, const bool _Deep = false );
    RDirectory *FChangeDirectory( const vector<RDirectory *> &_Directories, RDirectory *_Substitute, const bool _Deep = false );

    private:
    u32string VName;

    vector<RDirectory *> VDirectories;
    vector<RFile *> VFiles;
  };

  class RPackage {
    public:
    enum class ENOpcodes: int64_t {
      OPC_START_ENCODED_BLOCK = 1,
      OPC_ENCODED_BLOCK_DIVIDER,
      OPC_END_ENCODED_BLOCK,

      OPC_START_PATH,
      OPC_PATH_DIVIDER,
      OPC_END_PATH,

      OPC_START_DATA,
      OPC_DATA_DIVIDER,
      OPC_END_DATA
    };

    explicit RPackage() noexcept;
    explicit RPackage( const RPackage &_Copy ) noexcept;
    explicit RPackage( const vector<RFile *> &_Data ) noexcept;
    ~RPackage();

    inline vector<RFile *> FGetFlatData() const noexcept { return VFlatData; }
    inline void FSetFlatData( const vector<RFile *> &_FlatData ) noexcept { VFlatData = _FlatData; }

    inline void FClearFlatData() noexcept { VFlatData.clear(); }

    ENErrorCodes FCreatePackage( const u32string &_Path,
                                 const vector <RFile *> &_Data );

    ENErrorCodes FLoadPackage( const u32string &_Path );
    ENErrorCodes FLoadPackage( const u32string &_Path,
                               vector<RFile *> &_Data );

    ENErrorCodes FFullLoadPackage( const u32string &_Path );
    ENErrorCodes FFullLoadPackage( const u32string &_Path,
                                   vector<RFile *> &_Data );

    ENErrorCodes FSavePackage( const u32string &_Path );
    ENErrorCodes FSavePackage( const u32string &_Path,
                               const vector <RFile *> &_Data );

    ENErrorCodes FMakeFlatDirectory( vector<RFile *> &_Destination,
                                     const RDirectory *_Directory ) const;
    ENErrorCodes FMakePackage( vector<RFile *> &_Destination,
                               const vector<RDirectory *> &_Directories ) const;

    ENErrorCodes FReadStructure( const u32string &_Path,
                                 RDirectory *_Destination );

    ENErrorCodes FExtractFile( const u32string &_Path,
                               const u32string &_Name,
                               RFile *_Destination );

    ENErrorCodes FExtractDirectory( const u32string &_Path,
                                    const u32string &_Name,
                                    RDirectory *_Destination );

    //Utilities

    bool FIsPathContainsDirectory( const u32string &_Path, const u32string &_Directory ) const;
    vector<u32string> FSplitPath( const u32string &_Path ) const;

    private:
    enum class ENExtensions: int64_t {
      InvalidExtension = -1,

      DefaultExtension
    };

    const char32_t *VDefaultExtension = U".rack-pack";

    u32string VBuffer;

    vector<RFile *> VFlatData;

    inline ENExtensions FIsRackFile( const u32string &_Path ) const noexcept {
      u32string VGetExtension = path( _Path ).extension().u32string();

      if( VGetExtension == VDefaultExtension )
        return ENExtensions::DefaultExtension;
      else
        return ENExtensions::InvalidExtension;
    }

    inline void FClearBuffer() noexcept {
      VBuffer.clear();
    }

    RFile *FParseFile( basic_ifstream<char32_t> &_FileStream ) const;
    RFile *FParseFile( uint32_t &_Index );
    RFile *FParseFile( const u32string &_Buffer ) const;

    u32string FParseFilePath( basic_ifstream<char32_t> &_FileStream ) const;
    u32string FParseFilePath( const u32string &_Buffer ) const;

    u32string FExtractFileName( const u32string &_Path ) const;

    u32string FSerializeFile( const RFile *_File ) const;

    vector<RFile *> FGetDirectoryFiles( const RDirectory *_Directory ) const;

    u32string FTruncatePath( const u32string &_Path, const u32string &_TargetDirectory ) const;
    ENErrorCodes FCreatePath( RDirectory *_Directory,
                              const vector<u32string> &_PathSequence, const uint32_t _Depth ) const;

    ENErrorCodes FInsertFileToDirectory( RDirectory *_Directory,
                                         const u32string &_Name, RFile *_File ) const;
  };
}
