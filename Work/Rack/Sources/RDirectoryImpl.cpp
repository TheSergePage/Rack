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

using Rack::ENErrorCodes;
using Rack::RFile;
using Rack::RDirectory;

RDirectory::RDirectory() noexcept {}

RDirectory::RDirectory( const RDirectory &_Copy ) noexcept:
  VName( _Copy.FGetName() ),
  VDirectories( _Copy.FGetDirectories() ), VFiles( _Copy.FGetFiles() ) {}

RDirectory::RDirectory( const u32string &_Name ) noexcept:
  VName( _Name ) {}

RDirectory::RDirectory( const u32string &_Name,
                        const vector<RDirectory *> &_Directories, const vector<RFile *> &_Files ) noexcept:
  VName( _Name ), VDirectories( _Directories ), VFiles( _Files ) {}

RDirectory::~RDirectory() {
  VName.clear();

  VDirectories.clear();
  VFiles.clear();
}

RFile *RDirectory::FGetFile( const uint32_t _Index ) const {
  if( _Index > VFiles.size() )
    return nullptr;

  return VFiles [ _Index ];
}

RFile *RDirectory::FGetFile( const u32string &_Name, const bool _Deep ) const {
  for( RFile *VFile : VFiles ) {
    if( VFile->FGetName() == _Name )
      return VFile;
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RFile *VGetFile = VDirectory->FGetFile( _Name, _Deep );

      if( VGetFile != nullptr )
        return VGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::FGetFile( const RFile *_File, const bool _Deep ) const {
  if( _File == nullptr )
    return nullptr;

  for( RFile *VFile : VFiles ) {
    if( VFile->FGetName() == _File->FGetName() &&
        VFile->FGetData() == _File->FGetData() )
      return VFile;
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RFile *VGetFile = VDirectory->FGetFile( _File, _Deep );

      if( VGetFile != nullptr )
        return VGetFile;
    }
  }

  return nullptr;
}

ENErrorCodes RDirectory::FAddFile( RFile *_File ) {
  if( _File == nullptr )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  VFiles.push_back( _File );

  return ENErrorCodes::EC_OK;
}

RFile *RDirectory::FRemoveFile( const uint32_t _Index ) {
  if( _Index >= VFiles.size() )
    return nullptr;

  RFile *VGetFile = VFiles [ _Index ];

  VFiles.erase( VFiles.begin() + _Index );

  return VGetFile;
}

RFile *RDirectory::FRemoveFile( const u32string &_Name, const bool _Deep ) {
  for( uint32_t c = 0; c < VFiles.size(); c++ ) {
    if( VFiles [ c ]->FGetName() == _Name ) {
      RFile *VGetFile = VFiles [ c ];

      VFiles.erase( VFiles.begin() + c );

      return VGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RFile *VGetFile = VDirectory->FRemoveFile( _Name, _Deep );

      if( VGetFile != nullptr )
        return VGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::FRemoveFile( const RFile *_File, const bool _Deep ) {
  if( _File == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VFiles.size(); c++ ) {
    if( VFiles [ c ]->FGetName() == _File->FGetName() &&
        VFiles [ c ]->FGetData() == _File->FGetData() ) {
      RFile *VGetFile = VFiles [ c ];

      VFiles.erase( VFiles.begin() + c );

      return VGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RFile *VGetFile = VDirectory->FRemoveFile( _File, _Deep );

      if( VGetFile != nullptr )
        return VGetFile;
    }
  }

  return nullptr;
}

ENErrorCodes RDirectory::FInsertFile( RFile *_File, const uint32_t _Index ) {
  if( _File == nullptr || _Index >= VFiles.size() )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  VFiles.insert( VFiles.begin() + _Index, _File );

  return ENErrorCodes::EC_OK;
}

int64_t RDirectory::FGetFileIndex( const u32string &_Name ) const {
  for( uint32_t c = 0; c < VFiles.size(); c++ ) {
    if( VFiles [ c ]->FGetName() == _Name )
      return static_cast< int64_t >( c );
  }

  return static_cast< int64_t >( ENErrorCodes::EC_FILE_NOT_FOUND );
}

RFile *RDirectory::FChangeFile( const uint32_t _Index, RFile *_Substitute ) {
  if( _Index >= VFiles.size() || _Substitute == nullptr )
    return nullptr;

  RFile *VGetFile = VFiles [ _Index ];

  VFiles [ _Index ] = _Substitute;

  return VGetFile;
}

RFile *RDirectory::FChangeFile( const u32string &_Name, RFile *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VFiles.size(); c++ ) {
    if( VFiles [ c ]->FGetName() == _Name ) {
      RFile *VGetFile = VFiles [ c ];

      VFiles [ c ] = _Substitute;

      return VGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RFile *VGetFile = VDirectory->FChangeFile( _Name, _Substitute, _Deep );

      if( VGetFile != nullptr )
        return VGetFile;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FGetDirectory( const uint32_t _Index ) const {
  if( _Index >= VDirectories.size() )
    return nullptr;

  return VDirectories [ _Index ];
}

RDirectory *RDirectory::FGetDirectory( const u32string &_Name, const bool _Deep ) const {
  for( RDirectory *VDirectory : VDirectories ) {
    if( VDirectory->FGetName() == _Name )
      return VDirectory;
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FGetDirectory( _Name, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FGetDirectory( const RFile *_File, const bool _Deep ) const {
  for( RDirectory *VDirectory : VDirectories ) {
    if( VDirectory->FGetFile( _File, _Deep ) != nullptr )
      return VDirectory;
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FGetDirectory( _File, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FGetDirectory( const vector<RFile *> &_Files, const bool _Deep ) const {
  for( RDirectory *VDirectory : VDirectories ) {
    for( const RFile *VFile : _Files ) {
      if( VDirectory->FGetFile( VFile, _Deep ) != nullptr )
        return VDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FGetDirectory( _Files, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FGetDirectory( const RDirectory *_Directory, const bool _Deep ) const {
  for( RDirectory *VDirectory : VDirectories ) {
    if( VDirectory->FGetName() == _Directory->FGetName() )
      return VDirectory;
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FGetDirectory( _Directory, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FGetDirectory( const vector<RDirectory *> &_Directories, const bool _Deep ) const {
  for( RDirectory *VDirectory : VDirectories ) {
    for( const RDirectory *VInnerDirectory : _Directories ) {
      if( VDirectory->FGetDirectory( VInnerDirectory, _Deep ) != nullptr )
        return VDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      for( const RDirectory *VInnerDirectory : _Directories ) {
        RDirectory *VGetDirectory = VDirectory->FGetDirectory( VInnerDirectory, _Deep );

        if( VGetDirectory != nullptr )
          return VGetDirectory;
      }
    }
  }

  return nullptr;
}

ENErrorCodes RDirectory::FAddDirectory( RDirectory *_Directory ) {
  if( _Directory == nullptr )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  VDirectories.push_back( _Directory );

  return ENErrorCodes::EC_OK;
}

RDirectory *RDirectory::FRemoveDirectory( const uint32_t _Index ) {
  if( _Index >= VFiles.size() )
    return nullptr;

  RDirectory *VGetDirectory = VDirectories [ _Index ];

  VDirectories.erase( VDirectories.begin() + _Index );

  return VGetDirectory;
}

RDirectory *RDirectory::FRemoveDirectory( const u32string &_Name, const bool _Deep ) {
  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetName() == _Name ) {
      RDirectory *VGetDirectory = VDirectories [ c ];

      VDirectories.erase( VDirectories.begin() + c );

      return VGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FRemoveDirectory( _Name, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FRemoveDirectory( const RFile *_File, const bool _Deep ) {
  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetFile( _File ) != nullptr ) {
      RDirectory *VGetDirectory = VDirectories [ c ];

      VDirectories.erase( VDirectories.begin() + c );

      return VGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FRemoveDirectory( _File, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FRemoveDirectory( const vector<RFile *> &_Files, const bool _Deep ) {
  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    for( const RFile *VFile : _Files ) {
      if( VDirectories [ c ]->FGetFile( VFile ) != nullptr ) {
        RDirectory *VGetDirectory = VDirectories [ c ];

        VDirectories.erase( VDirectories.begin() + c );

        return VGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      for( const RFile *VFile : _Files ) {
        RDirectory *VGetDirectory = VDirectory->FRemoveDirectory( VFile, _Deep );

        if( VGetDirectory != nullptr )
          return VGetDirectory;
      }
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FRemoveDirectory( const RDirectory *_Directory, const bool _Deep ) {
  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetDirectory( _Directory ) != nullptr ) {
      RDirectory *VGetDirectory = VDirectories [ c ];

      VDirectories.erase( VDirectories.begin() + c );

      return VGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FRemoveDirectory( _Directory, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FRemoveDirectory( const vector<RDirectory *> &_Directories, const bool _Deep ) {
  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    for( const RDirectory *VDirectory : _Directories ) {
      if( VDirectories [ c ]->FGetDirectory( VDirectory ) != nullptr ) {
        RDirectory *VGetDirectory = VDirectories [ c ];

        VDirectories.erase( VDirectories.begin() + c );

        return VGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      for( const RDirectory *VInnerDirectory : _Directories ) {
        RDirectory *VGetDirectory = VDirectory->FRemoveDirectory( VInnerDirectory, _Deep );

        if( VGetDirectory != nullptr )
          return VGetDirectory;
      }
    }
  }

  return nullptr;
}

ENErrorCodes RDirectory::FInsertDirectory( RDirectory *_Directory, const uint32_t _Index ) {
  if( _Directory == nullptr || _Index >= VDirectories.size() )
    return ENErrorCodes::EC_INVALID_ARGUMENT;

  VDirectories.insert( VDirectories.begin() + _Index, _Directory );

  return ENErrorCodes::EC_OK;
}

int64_t RDirectory::FGetDirectoryIndex( const u32string &_Name ) const {
  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetName() == _Name )
      return static_cast< int64_t >( c );
  }

  return static_cast< int64_t >( ENErrorCodes::EC_DIRECTORY_NOT_FOUND );
}

int64_t RDirectory::FGetDirectoryIndex( const RFile *_File ) const {
  if( _File == nullptr )
    return static_cast< int64_t >( ENErrorCodes::EC_INVALID_ARGUMENT );

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetFile( _File ) != nullptr )
      return static_cast< int64_t >( c );
  }

  return static_cast< int64_t >( ENErrorCodes::EC_DIRECTORY_NOT_FOUND );
}

int64_t RDirectory::FGetDirectoryIndex( const vector<RFile *> &_Files ) const {
  if( _Files.empty() )
    return static_cast< int64_t >( ENErrorCodes::EC_INVALID_ARGUMENT );

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    for( const RFile *VFile : _Files ) {
      if( VDirectories [ c ]->FGetFile( VFile ) != nullptr )
        return static_cast< int64_t >( c );
    }
  }

  return static_cast< int64_t >( ENErrorCodes::EC_DIRECTORY_NOT_FOUND );
}

int64_t RDirectory::FGetDirectoryIndex( const RDirectory *_Directory ) const {
  if( _Directory == nullptr )
    return static_cast< int64_t >( ENErrorCodes::EC_INVALID_ARGUMENT );

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetDirectory( _Directory ) != nullptr )
      return static_cast< int64_t >( c );
  }

  return static_cast< int64_t >( ENErrorCodes::EC_DIRECTORY_NOT_FOUND );
}

int64_t RDirectory::FGetDirectoryIndex( const vector<RDirectory *> &_Directories ) const {
  if( _Directories.empty() )
    return static_cast< int64_t >( ENErrorCodes::EC_INVALID_ARGUMENT );

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    for( const RDirectory *VDirectory : _Directories ) {
      if( VDirectories [ c ]->FGetDirectory( VDirectory ) != nullptr )
        return static_cast< int64_t >( c );
    }
  }

  return static_cast< int64_t >( ENErrorCodes::EC_DIRECTORY_NOT_FOUND );
}

RDirectory *RDirectory::FChangeDirectory( const uint32_t _Index, RDirectory *_Substitute ) {
  if( _Index >= VDirectories.size() || _Substitute == nullptr )
    return nullptr;

  RDirectory *VGetDirectory = VDirectories [ _Index ];

  VDirectories [ _Index ] = _Substitute;

  return VGetDirectory;
}

RDirectory *RDirectory::FChangeDirectory( const u32string &_Name, RDirectory *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetName() == _Name ) {
      RDirectory *VGetDirectory = VDirectories [ c ];

      VDirectories [ c ] = _Substitute;

      return VGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FChangeDirectory( _Name, _Substitute, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FChangeDirectory( const RFile *_File, RDirectory *_Substitute, const bool _Deep ) {
  if( _File == nullptr || _Substitute == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetFile( _File ) != nullptr ) {
      RDirectory *VGetDirectory = VDirectories [ c ];

      VDirectories [ c ] = _Substitute;

      return VGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FChangeDirectory( _File, _Substitute, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FChangeDirectory( const vector<RFile *> &_Files, RDirectory *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    for( const RFile *VFile : _Files ) {
      if( VDirectories [ c ]->FGetFile( VFile ) != nullptr ) {
        RDirectory *VGetDirectory = VDirectories [ c ];

        VDirectories [ c ] = _Substitute;

        return VGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      for( const RFile *VFile : _Files ) {
        RDirectory *VGetDirectory = VDirectory->FChangeDirectory( VFile, _Substitute, _Deep );

        if( VGetDirectory != nullptr )
          return VGetDirectory;
      }
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FChangeDirectory( const RDirectory *_Directory, RDirectory *_Substitute, const bool _Deep ) {
  if( _Directory == nullptr || _Substitute == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    if( VDirectories [ c ]->FGetDirectory( _Directory ) != nullptr ) {
      RDirectory *VGetDirectory = VDirectories [ c ];

      VDirectories [ c ] = _Substitute;

      return VGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      RDirectory *VGetDirectory = VDirectory->FChangeDirectory( _Directory, _Substitute, _Deep );

      if( VGetDirectory != nullptr )
        return VGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::FChangeDirectory( const vector<RDirectory *> &_Directories, RDirectory *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( uint32_t c = 0; c < VDirectories.size(); c++ ) {
    for( const RDirectory *VDirectory : _Directories ) {
      if( VDirectories [ c ]->FGetDirectory( VDirectory ) != nullptr ) {
        RDirectory *VGetDirectory = VDirectories [ c ];

        VDirectories [ c ] = _Substitute;

        return VGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *VDirectory : VDirectories ) {
      for( const RDirectory *VInnerDirectory : _Directories ) {
        RDirectory *VGetDirectory = VDirectory->FChangeDirectory( VInnerDirectory, _Substitute, _Deep );

        if( VGetDirectory != nullptr )
          return VGetDirectory;
      }
    }
  }

  return nullptr;
}
