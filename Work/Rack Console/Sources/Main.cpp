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

#include <RackLib.hpp>

using namespace Rack;

#include <iostream>

using std::wcout;
using std::endl;

using std::move;

wstring_convert<codecvt_utf8_utf16<wchar_t>> vConverter;

void fInsertAlign( const size_t _Level ) {
  for( size_t c = 0; c < _Level; c++ )
    wcout << L" ";
}

void fShowFile( const RFile *_File, const size_t _Level ) {
  fInsertAlign( _Level );
  wcout << L"File name - " << _File->fGetName() << endl;

  fInsertAlign( _Level );
  wcout << L"File extension - " << _File->fGetExtension() << endl;

  fInsertAlign( _Level );
  wcout << L"File root directory - " << _File->fGetRootDirectory() << endl;

  fInsertAlign( _Level );
  wcout << L"File relative path - " << _File->fGetRelativePath() << endl << endl;

  fInsertAlign( _Level );
  wcout << L"File content - " << _File->fGetData() << endl << endl;
}

void fShowDirectory( const RDirectory *_Directory, const size_t _Level ) {
  fInsertAlign( _Level );
  wcout << L"Directory name - " << _Directory->fGetName() << endl;

  fInsertAlign( _Level );
  wcout << L"-----------" << endl;

  fInsertAlign( _Level );
  wcout << L"Inner files:";

  if( _Directory->fGetFiles().size() == 0 )
    wcout << L" Have no files" << endl;
  else {
    wcout << endl;

    for( const RFile *vFile : _Directory->fGetFiles() )
      fShowFile( vFile, _Level + 1 );
  }

  fInsertAlign( _Level );
  wcout << L"-----------" << endl;

  fInsertAlign( _Level );
  wcout << L"Inner directories:";

  if( _Directory->fGetDirectories().size() == 0 )
    wcout << L" Have no directories" << endl;
  else {
    wcout << endl;

    for( const RDirectory *vDirectory : _Directory->fGetDirectories() )
      fShowDirectory( vDirectory, _Level + 1 );
  }
}

void fShowHowToUse() {
  wcout << L"How to use:" << endl;
  wcout << L"-cmd - command[LightLoad/FullLoad/ReadStructure/Create/Change/ExtractFile/ExtractDirectory]" << endl << endl;

  wcout << L"-lpath - path to the file for load" << endl;
  wcout << L"-spath - path to the file or folder for save" << endl << endl;

  wcout << L"-tf - target file for extracting or changing" << endl;
  wcout << L"-td - target directory for extracting" << endl << endl;
}

void fShowPackage( RPackage &_Packager, const wstring _Path ) {
  wcout << L"Flat data: ";

  if( _Packager.fGetFlatData().size() == 0 )
    wcout << L"Have no flat data" << endl;
  else {
    wcout << endl;

    for( const RFile *vFile : _Packager.fGetFlatData() )
      fShowFile( vFile, 1 );
  }

  wcout << L"*************************" << endl;

  wcout << L"Structure: ";

  RDirectory *vRootDirectory = new RDirectory( L"Root" );

  enErrorCode vReadStructureResult = _Packager.fReadStructure( _Path, vRootDirectory );

  if( vReadStructureResult != EC_OK )
    wcout << L"Invalid reading structure. Code - " << vReadStructureResult << endl;
  else {
    wcout << endl;

    fShowDirectory( vRootDirectory, 0 );
  }

  wcout << L"=========================" << endl;
}

int __cdecl main( int argc, char **argv ) {
  wcout << L">>>Rack Console 2.1" << endl;
  wcout << L">>>Rack Lib 2.1" << endl << endl;

  wcout << L"=========================" << endl << endl;

  if( argc == 1 ) {
    fShowHowToUse();
    return 0;
  }

  size_t vCommand = 0;

  wstring vLoadPath, vSavePath;
  wstring vTargetFile, vTargetDirectory;

  for( size_t c = 1; c < static_cast< size_t >( argc ); c++ ) {
    if( strcmp( argv [ c ], "-cmd" ) == 0 ) {
      if( strcmp( argv [ c + 1 ], "LightLoad" ) == 0 ) {
        ++c;
        vCommand = 0;
      } else if( strcmp( argv [ c + 1 ], "FullLoad" ) == 0 ) {
        ++c;
        vCommand = 1;
      } else if( strcmp( argv [ c + 1 ], "ReadStructure" ) == 0 ) {
        ++c;
        vCommand = 2;
      } else if( strcmp( argv [ c + 1 ], "Create" ) == 0 ) {
        ++c;
        vCommand = 3;
      } else if( strcmp( argv [ c + 1 ], "Change" ) == 0 ) {
        ++c;
        vCommand = 4;
      } else if( strcmp( argv [ c + 1 ], "ExtractFile" ) == 0 ) {
        ++c;
        vCommand = 5;
      } else if( strcmp( argv [ c + 1 ], "ExtractDirectory" ) == 0 ) {
        ++c;
        vCommand = 6;
      } else {
        wcout << L">>>Invalid argument value. Argument - cmd : " << argv [ c + 1 ] << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }
    } else if( strcmp( argv [ c ], "-lpath" ) == 0 ) {
      vLoadPath = vConverter.from_bytes( argv [ ++c ] );
    } else if( strcmp( argv [ c ], "-spath" ) == 0 ) {
      vSavePath = vConverter.from_bytes( argv [ ++c ] );
    } else if( strcmp( argv [ c ], "-tf" ) == 0 ) {
      vTargetFile = vConverter.from_bytes( argv [ ++c ] );
    } else if( strcmp( argv [ c ], "-td" ) == 0 ) {
      vTargetDirectory = vConverter.from_bytes( argv [ ++c ] );
    } else {
      wcout << L">>>Unknown argument. Argument - " << argv [ c ] << endl;

      wcout << L"=========================" << endl;

      fShowHowToUse();

      return -1;
    }
  }

  RPackage vPackager;

  vector<RFile *> vFiles;
  RDirectory *vRootDirectory;
  switch( vCommand ) {
    //LightLoad
    case 0:
    {
      if( vLoadPath.empty() ) {
        wcout << L">>>Load path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      wcout << L"Load result - " << vPackager.fLoadPackage( vLoadPath ) << endl << endl;

      fShowPackage( vPackager, vLoadPath );

      wcout << endl;
    }
    break;

    //FullLoad
    case 1:
    {
      if( vLoadPath.empty() ) {
        wcout << L">>>Load path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      wcout << L"Load result - " << vPackager.fFullLoadPackage( vLoadPath ) << endl << endl;

      fShowPackage( vPackager, vLoadPath );

      wcout << endl;
    }
    break;

    //ReadStructure
    case 2:
    {
      if( vLoadPath.empty() ) {
        wcout << L">>>Load path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      vRootDirectory = new RDirectory( L"Root" );

      enErrorCode vReadStructureResult = vPackager.fReadStructure( vLoadPath, vRootDirectory );

      if( vReadStructureResult != EC_OK )
        wcout << L"Invalid reading structure. Code - " << vReadStructureResult << endl;
      else {
        wcout << endl;

        fShowDirectory( vRootDirectory, 0 );

        wcout << endl;
      }

      delete vRootDirectory;
    }
    break;

    //Create
    case 3:
    {
      if( vSavePath.empty() ) {
        wcout << L">>>Save path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      vFiles.push_back( new RFile( L"Messages/Hello_World.txt", L"Hello World!" ) );
      vFiles.push_back( new RFile( L"Messages/Question1.txt", L"How are you?" ) );
      vFiles.push_back( new RFile( L"Messages/Answer1.txt", L"I'm fine." ) );

      vFiles.push_back( new RFile( L"Another messages/Question1.txt", L"And you?" ) );
      vFiles.push_back( new RFile( L"Another messages/Replies/Reply1.txt", L"The same" ) );

      wcout << L"Create package result - " << vPackager.fCreatePackage( vSavePath, vFiles ) << endl;
    }
    break;

    //Change
    case 4:
    {
      if( vLoadPath.empty() ) {
        wcout << L">>>Load path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      if( vSavePath.empty() ) {
        wcout << L">>>Save path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      if( vTargetFile.empty() ) {
        wcout << L">>>Target file name is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      wcout << L"Load result - " << vPackager.fFullLoadPackage( vLoadPath, vFiles ) << endl;

      for( RFile *&vFile : vFiles ) {
        if( vFile->fGetName() == vTargetFile ) {
          vFile->fSetData( L"Here's a new data" );

          break;
        }
      }

      wcout << L"Save result - " << vPackager.fSavePackage( vSavePath, vFiles ) << endl;
    }
    break;

    //ExtractFile
    case 5:
    {
      if( vLoadPath.empty() ) {
        wcout << L">>>Load path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      if( vTargetFile.empty() ) {
        wcout << L">>>Target file name is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      RFile *vExtractedFile = new RFile();

      wcout << L"Load result - " << vPackager.fExtractFile( vLoadPath, vTargetFile, vExtractedFile ) << endl;

      wcout << L"Extracted file:" << endl;

      fShowFile( vExtractedFile, 0 );

      wcout << endl;
    }
    break;

    //ExtractDirectory
    case 6:
    {
      if( vLoadPath.empty() ) {
        wcout << L">>>Load path is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      if( vTargetDirectory.empty() ) {
        wcout << L">>>Target directory name is empty" << endl;

        wcout << L"=========================" << endl;

        fShowHowToUse();

        return -1;
      }

      vRootDirectory = new RDirectory( L"Root" );

      wcout << L"Load result - " << vPackager.fExtractDirectory( vLoadPath, vTargetDirectory, vRootDirectory ) << endl;

      wcout << L"Extracted directory:" << endl;

      fShowDirectory( vRootDirectory, 0 );

      delete vRootDirectory;

      wcout << endl;
    }
    break;
  }

  return 0;
}
