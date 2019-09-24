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

using Rack::enErrorCode;
using Rack::RFile;
using Rack::RDirectory;

RDirectory::RDirectory() noexcept {}
RDirectory::RDirectory( const RDirectory &_Copy ) noexcept:
  vName( _Copy.fGetName() ),
  vDirectories( _Copy.fGetDirectories() ), vFiles( _Copy.fGetFiles() ) {}

RDirectory::RDirectory( wstring _Name ) :
  vName( _Name ) {}

RDirectory::RDirectory( wstring _Name,
                        vector<RDirectory *> _Directories, vector<RFile *> _Files ) noexcept:
  vName( _Name ), vDirectories( _Directories ), vFiles( _Files ) {}

RDirectory::~RDirectory() {
  vName.clear();

  vDirectories.clear();
  vFiles.clear();
}

RFile *RDirectory::fGetFile( const wstring _Name, const bool _Deep ) const {
  for( RFile *vFile : vFiles ) {
    if( vFile->fGetName() == _Name )
      return vFile;
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fGetFile( _Name, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fGetFile( const wchar_t *_Data, const bool _Deep ) const {
  if( _Data == nullptr )
    return nullptr;

  for( RFile *vFile : vFiles ) {
    if( wcscmp( vFile->fGetData(), _Data ) == 0 )
      return vFile;
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fGetFile( _Data, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fGetFile( const RFile *_File, const bool _Deep ) const {
  if( _File == nullptr )
    return nullptr;

  for( RFile *vFile : vFiles ) {
    if( vFile->fGetName() == _File->fGetName() &&
        wcscmp( vFile->fGetData(), _File->fGetData() ) == 0 )
      return vFile;
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fGetFile( _File, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fRemoveFile( const wstring _Name, const bool _Deep ) {
  for( size_t c = 0; c < vFiles.size(); c++ ) {
    if( vFiles [ c ]->fGetName() == _Name ) {
      RFile *vGetFile = vFiles [ c ];

      vFiles.erase( vFiles.begin() + c );

      return vGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fRemoveFile( _Name, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fRemoveFile( const wchar_t *_Data, const bool _Deep ) {
  if( _Data == nullptr )
    return nullptr;

  for( size_t c = 0; c < vFiles.size(); c++ ) {
    if( wcscmp( vFiles [ c ]->fGetData(), _Data ) == 0 ) {
      RFile *vGetFile = vFiles [ c ];

      vFiles.erase( vFiles.begin() + c );

      return vGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fRemoveFile( _Data, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fRemoveFile( const RFile *_File, const bool _Deep ) {
  if( _File == nullptr )
    return nullptr;

  for( size_t c = 0; c < vFiles.size(); c++ ) {
    if( vFiles [ c ]->fGetName() == _File->fGetName() &&
        wcscmp( vFiles [ c ]->fGetData(), _File->fGetData() ) == 0 ) {
      RFile *vGetFile = vFiles [ c ];

      vFiles.erase( vFiles.begin() + c );

      return vGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fRemoveFile( _File, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fChangeFile( const wstring _Name, RFile *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vFiles.size(); c++ ) {
    if( vFiles [ c ]->fGetName() == _Name ) {
      RFile *vGetFile = vFiles [ c ];

      vFiles [ c ] = _Substitute;

      return vGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fChangeFile( _Name, _Substitute, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RFile *RDirectory::fChangeFile( const wchar_t *_Data, RFile *_Substitute, const bool _Deep ) {
  if( _Data == nullptr || _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vFiles.size(); c++ ) {
    if( wcscmp( vFiles [ c ]->fGetData(), _Data ) == 0 ) {
      RFile *vGetFile = vFiles [ c ];

      vFiles [ c ] = _Substitute;

      return vGetFile;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RFile *vGetFile = vDirectory->fChangeFile( _Data, _Substitute, _Deep );

      if( vGetFile != nullptr )
        return vGetFile;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fGetDirectory( const wstring _Name, const bool _Deep ) const {
  for( RDirectory *vDirectory : vDirectories ) {
    if( vDirectory->fGetName() == _Name )
      return vDirectory;
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fGetDirectory( _Name, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fGetDirectory( const RFile *_File, const bool _Deep ) const {
  for( RDirectory *vDirectory : vDirectories ) {
    if( vDirectory->fGetFile( _File, _Deep ) != nullptr )
      return vDirectory;
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fGetDirectory( _File, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fGetDirectory( const vector<RFile *> _Files, const bool _Deep ) const {
  for( RDirectory *vDirectory : vDirectories ) {
    for( const RFile *vFile : _Files ) {
      if( vDirectory->fGetFile( vFile, _Deep ) != nullptr )
        return vDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fGetDirectory( _Files, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fGetDirectory( const RDirectory *_Directory, const bool _Deep ) const {
  for( RDirectory *vDirectory : vDirectories ) {
    if( vDirectory->fGetName() == _Directory->fGetName() )
      return vDirectory;
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fGetDirectory( _Directory, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fGetDirectory( const vector<RDirectory *> _Directories, const bool _Deep ) const {
  for( RDirectory *vDirectory : vDirectories ) {
    for( const RDirectory *vInnerDirectory : _Directories ) {
      if( vDirectory->fGetDirectory( vInnerDirectory, _Deep ) != nullptr )
        return vDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      for( const RDirectory *vInnerDirectory : _Directories ) {
        RDirectory *vGetDirectory = vDirectory->fGetDirectory( vInnerDirectory, _Deep );

        if( vGetDirectory != nullptr )
          return vGetDirectory;
      }
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fRemoveDirectory( const wstring _Name, const bool _Deep ) {
  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    if( vDirectories [ c ]->fGetName() == _Name ) {
      RDirectory *vGetDirectory = vDirectories [ c ];

      vDirectories.erase( vDirectories.begin() + c );

      return vGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fRemoveDirectory( _Name, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fRemoveDirectory( const RFile *_File, const bool _Deep ) {
  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    if( vDirectories [ c ]->fGetFile( _File ) != nullptr ) {
      RDirectory *vGetDirectory = vDirectories [ c ];

      vDirectories.erase( vDirectories.begin() + c );

      return vGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fRemoveDirectory( _File, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fRemoveDirectory( const vector<RFile *> _Files, const bool _Deep ) {
  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    for( const RFile *vFile : _Files ) {
      if( vDirectories [ c ]->fGetFile( vFile ) != nullptr ) {
        RDirectory *vGetDirectory = vDirectories [ c ];

        vDirectories.erase( vDirectories.begin() + c );

        return vGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      for( const RFile *vFile : _Files ) {
        RDirectory *vGetDirectory = vDirectory->fRemoveDirectory( vFile, _Deep );

        if( vGetDirectory != nullptr )
          return vGetDirectory;
      }
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fRemoveDirectory( const RDirectory *_Directory, const bool _Deep ) {
  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    if( vDirectories [ c ]->fGetDirectory( _Directory ) != nullptr ) {
      RDirectory *vGetDirectory = vDirectories [ c ];

      vDirectories.erase( vDirectories.begin() + c );

      return vGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fRemoveDirectory( _Directory, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fRemoveDirectory( const vector<RDirectory *> _Directories, const bool _Deep ) {
  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    for( const RDirectory *vDirectory : _Directories ) {
      if( vDirectories [ c ]->fGetDirectory( vDirectory ) != nullptr ) {
        RDirectory *vGetDirectory = vDirectories [ c ];

        vDirectories.erase( vDirectories.begin() + c );

        return vGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      for( const RDirectory *vInnerDirectory : _Directories ) {
        RDirectory *vGetDirectory = vDirectory->fRemoveDirectory( vInnerDirectory, _Deep );

        if( vGetDirectory != nullptr )
          return vGetDirectory;
      }
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fChangeDirectory( const wstring _Name, RDirectory *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    if( vDirectories [ c ]->fGetName() == _Name ) {
      RDirectory *vGetDirectory = vDirectories [ c ];

      vDirectories [ c ] = _Substitute;

      return vGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fChangeDirectory( _Name, _Substitute, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fChangeDirectory( const RFile *_File, RDirectory *_Substitute, const bool _Deep ) {
  if( _File == nullptr || _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    if( vDirectories [ c ]->fGetFile( _File ) != nullptr ) {
      RDirectory *vGetDirectory = vDirectories [ c ];

      vDirectories [ c ] = _Substitute;

      return vGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fChangeDirectory( _File, _Substitute, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fChangeDirectory( const vector<RFile *> _Files, RDirectory *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    for( const RFile *vFile : _Files ) {
      if( vDirectories [ c ]->fGetFile( vFile ) != nullptr ) {
        RDirectory *vGetDirectory = vDirectories [ c ];

        vDirectories [ c ] = _Substitute;

        return vGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      for( const RFile *vFile : _Files ) {
        RDirectory *vGetDirectory = vDirectory->fChangeDirectory( vFile, _Substitute, _Deep );

        if( vGetDirectory != nullptr )
          return vGetDirectory;
      }
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fChangeDirectory( const RDirectory *_Directory, RDirectory *_Substitute, const bool _Deep ) {
  if( _Directory == nullptr || _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    if( vDirectories [ c ]->fGetDirectory( _Directory ) != nullptr ) {
      RDirectory *vGetDirectory = vDirectories [ c ];

      vDirectories [ c ] = _Substitute;

      return vGetDirectory;
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      RDirectory *vGetDirectory = vDirectory->fChangeDirectory( _Directory, _Substitute, _Deep );

      if( vGetDirectory != nullptr )
        return vGetDirectory;
    }
  }

  return nullptr;
}

RDirectory *RDirectory::fChangeDirectory( const vector<RDirectory *> _Directories, RDirectory *_Substitute, const bool _Deep ) {
  if( _Substitute == nullptr )
    return nullptr;

  for( size_t c = 0; c < vDirectories.size(); c++ ) {
    for( const RDirectory *vDirectory : _Directories ) {
      if( vDirectories [ c ]->fGetDirectory( vDirectory ) != nullptr ) {
        RDirectory *vGetDirectory = vDirectories [ c ];

        vDirectories [ c ] = _Substitute;

        return vGetDirectory;
      }
    }
  }

  if( _Deep ) {
    for( RDirectory *vDirectory : vDirectories ) {
      for( const RDirectory *vInnerDirectory : _Directories ) {
        RDirectory *vGetDirectory = vDirectory->fChangeDirectory( vInnerDirectory, _Substitute, _Deep );

        if( vGetDirectory != nullptr )
          return vGetDirectory;
      }
    }
  }

  return nullptr;
}
