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

#include <RackLib.hpp>

using namespace Rack;

#include "Unicoder.hpp"

using namespace Unicoder;

#include <string>

using std::wstring;

using std::to_wstring;

#include <iostream>

using std::wcout;
using std::endl;

using std::move;

void FPrintMessage( const u32string &_Text, const bool _NewLine = true, const bool _DoubleNewLine = false, const bool _OnlyNewLine = false ) {
  if( _OnlyNewLine ) {
    wcout << endl;

    return;
  }

  if( _NewLine ) {
    if( _DoubleNewLine ) {
      wcout << UConverter::FConvert<u32string, char32_t, wstring, wchar_t>( _Text ) << endl << endl;
    } else {
      wcout << UConverter::FConvert<u32string, char32_t, wstring, wchar_t>( _Text ) << endl;
    }
  } else {
    wcout << UConverter::FConvert<u32string, char32_t, wstring, wchar_t>( _Text );
  }
}

void fInsertAlign( const uint32_t _Level ) {
  for( uint32_t c = 0; c < _Level; c++ )
    wcout << L" ";
}

void fShowFile( const RFile *_File, const uint32_t _Level ) {
  fInsertAlign( _Level );
  FPrintMessage( U"File name - " + _File->FGetName() );

  fInsertAlign( _Level );
  FPrintMessage( U"File extension - " + _File->FGetExtension() );

  fInsertAlign( _Level );
  FPrintMessage( U"File root directory - " + _File->FGetRootDirectory() );

  fInsertAlign( _Level );
  FPrintMessage( U"File relative path - " + _File->FGetRelativePath(), true, true );

  fInsertAlign( _Level );
  FPrintMessage( U"File content - " + _File->FGetData(), true, true );
}

void fShowDirectory( const RDirectory *_Directory, const uint32_t _Level ) {
  fInsertAlign( _Level );
  FPrintMessage( U"Directory name - " + _Directory->FGetName() );

  fInsertAlign( _Level );
  FPrintMessage( U"-----------" );

  fInsertAlign( _Level );
  FPrintMessage( U"Inner files:", false );

  if( _Directory->FGetFiles().size() == 0 )
    FPrintMessage( U" Have no files" );
  else {
    FPrintMessage( U"", false, false, true );

    for( const RFile *VFile : _Directory->FGetFiles() )
      fShowFile( VFile, _Level + 1 );
  }

  fInsertAlign( _Level );
  FPrintMessage( U"-----------" );

  fInsertAlign( _Level );
  FPrintMessage( U"Inner directories:", false );

  if( _Directory->FGetDirectories().size() == 0 )
    FPrintMessage( U" Have no directories" );
  else {
    FPrintMessage( U"", false, false, true );

    for( const RDirectory *VDirectory : _Directory->FGetDirectories() )
      fShowDirectory( VDirectory, _Level + 1 );
  }
}

void fShowHowToUse() {
  FPrintMessage( U"How to use:" );
  FPrintMessage( U"!Command - command[LightLoad/FullLoad/ReadStructure/Create/Change/ExtractFile/ExtractDirectory]", true, true );

  FPrintMessage( U"!LoadPath - path to the file for load" );
  FPrintMessage( U"!SavePath - path to the file or folder for save", true, true );

  FPrintMessage( U"!TargetFile - target file for extracting or changing" );
  FPrintMessage( U"!TargetDirectory - target directory for extracting", true, true );
}

void fShowPackage( RPackage &_Packager, const u32string _Path ) {
  FPrintMessage( U"Flat data: ", false );

  if( _Packager.FGetFlatData().size() == 0 )
    FPrintMessage( U"Have no flat data" );
  else {
    FPrintMessage( U"", false, false, true );

    for( const RFile *VFile : _Packager.FGetFlatData() )
      fShowFile( VFile, 1 );
  }

  FPrintMessage( U"*************************" );

  FPrintMessage( U"Structure: ", false );

  RDirectory *VRootDirectory = new RDirectory( U"Root" );

  ENErrorCodes VReadStructureResult = _Packager.FReadStructure( _Path, VRootDirectory );

  if( VReadStructureResult != ENErrorCodes::EC_OK )
    FPrintMessage( U"Invalid reading structure. Code - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VReadStructureResult ) ) ) );
  else {
    FPrintMessage( U"", false, false, true );

    fShowDirectory( VRootDirectory, 0 );
  }

  FPrintMessage( U"=========================" );
}

int __cdecl wmain( int argc, wchar_t *argv[], wchar_t *envp[] ) {
  FPrintMessage( U"Rack Console 2.1" );
  FPrintMessage( U"Rack Lib 2.1", true, true );

  FPrintMessage( U"---------------------", true, true );

  if( argc == 1 ) {
    fShowHowToUse();
    return 0;
  }

  uint32_t VCommand = 0;

  u32string VLoadPath, VSavePath;
  u32string VTargetFile, VTargetDirectory;

  for( uint32_t c = 1; c < static_cast< uint32_t >( argc ); c++ ) {
    if( wcscmp( argv [ c ], L"!Command" ) == 0 ) {
      if( wcscmp( argv [ c + 1 ], L"LightLoad" ) == 0 ) {
        ++c;
        VCommand = 0;
      } else if( wcscmp( argv [ c + 1 ], L"FullLoad" ) == 0 ) {
        ++c;
        VCommand = 1;
      } else if( wcscmp( argv [ c + 1 ], L"ReadStructure" ) == 0 ) {
        ++c;
        VCommand = 2;
      } else if( wcscmp( argv [ c + 1 ], L"Create" ) == 0 ) {
        ++c;
        VCommand = 3;
      } else if( wcscmp( argv [ c + 1 ], L"Change" ) == 0 ) {
        ++c;
        VCommand = 4;
      } else if( wcscmp( argv [ c + 1 ], L"ExtractFile" ) == 0 ) {
        ++c;
        VCommand = 5;
      } else if( wcscmp( argv [ c + 1 ], L"ExtractDirectory" ) == 0 ) {
        ++c;
        VCommand = 6;
      } else {
        FPrintMessage( U"Invalid argument Value. Argument - cmd : " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( argv [ c + 1 ] ) );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }
    } else if( wcscmp( argv [ c ], L"!LoadPath" ) == 0 ) {
      VLoadPath = UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( argv [ ++c ] );
    } else if( wcscmp( argv [ c ], L"!SavePath" ) == 0 ) {
      VSavePath = UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( argv [ ++c ] );
    } else if( wcscmp( argv [ c ], L"!TargetFile" ) == 0 ) {
      VTargetFile = UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( argv [ ++c ] );
    } else if( wcscmp( argv [ c ], L"!TargetDirectory" ) == 0 ) {
      VTargetDirectory = UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( argv [ ++c ] );
    } else {
      FPrintMessage( U"Unknown argument. Argument - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( argv [ c ] ) );

      FPrintMessage( U"---------------------", true, true );

      fShowHowToUse();

      return -1;
    }
  }

  RPackage VPackager;

  vector<RFile *> VFiles;
  RDirectory *VRootDirectory;
  switch( VCommand ) {
    //LightLoad
    case 0:
    {
      if( VLoadPath.empty() ) {
        FPrintMessage( U"Load path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      FPrintMessage( U"Load result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FLoadPackage( VLoadPath ) ) ) ), true, true );

      fShowPackage( VPackager, VLoadPath );

      FPrintMessage( U"", false, false, true );
    }
    break;

    //FullLoad
    case 1:
    {
      if( VLoadPath.empty() ) {
        FPrintMessage( U"Load path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      FPrintMessage( U"Load result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FFullLoadPackage( VLoadPath ) ) ) ), true, true );

      fShowPackage( VPackager, VLoadPath );

      FPrintMessage( U"", false, false, true );
    }
    break;

    //ReadStructure
    case 2:
    {
      if( VLoadPath.empty() ) {
        FPrintMessage( U"Load path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      VRootDirectory = new RDirectory( U"Root" );

      ENErrorCodes VReadStructureResult = VPackager.FReadStructure( VLoadPath, VRootDirectory );

      if( VReadStructureResult != ENErrorCodes::EC_OK )
        FPrintMessage( U"Invalid reading structure. Code - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VReadStructureResult ) ) ) );
      else {
        FPrintMessage( U"", false, false, true );

        fShowDirectory( VRootDirectory, 0 );

        FPrintMessage( U"", false, false, true );
      }

      delete VRootDirectory;
    }
    break;

    //Create
    case 3:
    {
      if( VSavePath.empty() ) {
        FPrintMessage( U"Save path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      VFiles.push_back( new RFile( U"Messages/Hello_World.txt", U"Hello World!" ) );
      VFiles.push_back( new RFile( U"Messages/Question1.txt", U"How are you?" ) );
      VFiles.push_back( new RFile( U"Messages/Answer1.txt", U"I'm fine." ) );

      VFiles.push_back( new RFile( U"Another messages/Question1.txt", U"And you?" ) );
      VFiles.push_back( new RFile( U"Another messages/Replies/Reply1.txt", U"The same" ) );

      FPrintMessage( U"Create package result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FCreatePackage( VSavePath, VFiles ) ) ) ) );
    }
    break;

    //Change
    case 4:
    {
      if( VLoadPath.empty() ) {
        FPrintMessage( U"Load path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      if( VSavePath.empty() ) {
        FPrintMessage( U">>>Save path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      if( VTargetFile.empty() ) {
        FPrintMessage( U">>>Target file name is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      FPrintMessage( U"Load result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FFullLoadPackage( VLoadPath, VFiles ) ) ) ) );

      for( RFile *VFile : VFiles ) {
        if( VFile->FGetName() == VTargetFile ) {
          VFile->FSetData( U"Here's a new data" );

          break;
        }
      }

      FPrintMessage( U"Save result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FSavePackage( VSavePath, VFiles ) ) ) ) );
    }
    break;

    //ExtractFile
    case 5:
    {
      if( VLoadPath.empty() ) {
        FPrintMessage( U"Load path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      if( VTargetFile.empty() ) {
        FPrintMessage( U"Target file name is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      RFile *VExtractedFile = new RFile();

      FPrintMessage( U"Load result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FExtractFile( VLoadPath, VTargetFile, VExtractedFile ) ) ) ) );

      FPrintMessage( U"Extracted file:" );

      fShowFile( VExtractedFile, 0 );

      FPrintMessage( U"", false, false, true );
    }
    break;

    //ExtractDirectory
    case 6:
    {
      if( VLoadPath.empty() ) {
        FPrintMessage( U"Load path is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      if( VTargetDirectory.empty() ) {
        FPrintMessage( U"Target directory name is empty" );

        FPrintMessage( U"---------------------", true, true );

        fShowHowToUse();

        return -1;
      }

      VRootDirectory = new RDirectory( U"Root" );

      FPrintMessage( U"Load result - " + UConverter::FConvert<wstring, wchar_t, u32string, char32_t>( to_wstring( static_cast< int64_t >( VPackager.FExtractDirectory( VLoadPath, VTargetDirectory, VRootDirectory ) ) ) ) );

      FPrintMessage( U"Extracted directory:" );

      fShowDirectory( VRootDirectory, 0 );

      delete VRootDirectory;

      FPrintMessage( U"", false, false, true );
    }
    break;
  }

  return 0;
}
