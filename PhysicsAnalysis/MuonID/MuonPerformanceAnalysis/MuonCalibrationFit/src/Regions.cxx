/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define Regions_cxx
#include "MuonCalibrationFit/Regions.h"

Regions::Regions( TString name, TString root_file_name, int debug ) {
  Init();
  m_debug = ( debug == 1 );
  LoadConfiguration( name, root_file_name );
}

Regions::~Regions() {
  m_Variables.clear();
  m_Info.clear();
}

bool Regions::Init() {
  m_debug = kFALSE;
  m_FileName = "";
  m_Variables.clear();
  m_Info.clear();
  return kTRUE;
}

bool Regions::LoadConfiguration( TString name, TString root_file_name ) {
  m_FileName = name;
  std::ifstream InputFile( m_FileName );
  if( ! InputFile.good() ) {
    std::cerr << "Input file to Regions class not found!" << std::endl;
    return kFALSE;
  }
  int LineNumber = 0;
  std::string Line = "";
  while( ! InputFile.eof() ) {
    std::getline( InputFile, Line );
    if( LineNumber++ == 0 ) {
      TString LineString = Line;
      if( ! LineString.Contains( "Name" ) ) {
        std::cerr << "Wrong formatting of file: missing 'Name'!" << std::endl;
        return kFALSE;
      }
      /*
      if( ! LineString.Contains( "Order" ) ) {
        std::cerr << "Wrong formatting of file: missing 'Order'!" << std::endl;
        return kFALSE;
      }
      */
      TObjArray* Array = LineString.Tokenize( " " );
      for( int i = 0 ; i < Array->GetEntries(); ++i ) {
        TString TempString = Array->At( i )->GetName();
        if( TempString.BeginsWith( "Var:" ) && TempString.EndsWith( "[min]" ) ) {
          TString NextTempString = Array->At( i + 1 )->GetName();
          if( NextTempString.BeginsWith( "Var:" ) && NextTempString.EndsWith( "[max]" ) ) {
            m_Variables.push_back( TempString.ReplaceAll( "Var:", "" ).ReplaceAll( "[min]", "" ) );
          }
          else {
            std::cerr << "Wrong formatting of file: missing 'max'!" << std::endl;
            return kFALSE;
          }
        }
      }
    }
    else if( Line != "" ) {
      TString LineString = Line;
      TObjArray* Array = LineString.Tokenize( " " );
      Tools::RegionInfo* tempInfo = new Tools::RegionInfo;
      std::vector< Tools::VariableInfo > tempVarInfo;
      for( std::vector< TString >::iterator It = m_Variables.begin(); It != m_Variables.end(); ++It ) {
        Tools::VariableInfo temp;
        temp.Name = *It;
        tempVarInfo.push_back( temp );
      }
      for( int i = 0 ; i < Array->GetEntries(); ++i ) {
        TString TempString = Array->At( i )->GetName();
        if( ( unsigned int ) i < m_Variables.size() * 2 ) {
          int j = i / 2;
          if( i % 2 == 0 ) tempVarInfo.at( j ).Min = TempString.Atof(); 
          else             tempVarInfo.at( j ).Max = TempString.Atof(); 
        }
        else {
          if     ( ( unsigned int ) i == ( m_Variables.size() * 2 )     ) tempInfo->Name  = TempString;
          else if( ( unsigned int ) i == ( m_Variables.size() * 2 ) + 1 ) tempInfo->Order = TempString.Atoi();
          else {
            std::cerr << "Wrong formatting of file: too many entries on a row!" << std::endl;
            delete tempInfo; 
            tempInfo = NULL;
            return kFALSE;
          }
        }
      }
      tempInfo->VarInfo = tempVarInfo; 
      if( root_file_name != "" ) {
        TFile tempFile( root_file_name );
        TVectorD* vec = ( TVectorD* ) tempFile.Get( "FinalResults___" + tempInfo->Name );
        for( int index = 0; index < Par::N; index++ ) {
          tempInfo->par[ index ] = ( *vec )[ index * 3 ];
        }
        tempInfo->Corrected = kTRUE;
      }
      else {
        for( int index = 0; index < Par::N; index++ ) {
          tempInfo->par[ index ] = 0.;
        }
        tempInfo->Corrected = kFALSE;
      }
      m_Info.push_back( tempInfo );
    }
  }
  //::: Ordering m_Info vector
  std::sort( m_Info.begin(), m_Info.end(), Tools::RegionsSorter );
  return kTRUE;
}

void Regions::Print() {
  for( std::vector< Tools::RegionInfo* >::iterator It = m_Info.begin(); It != m_Info.end(); ++It ) {
    std::cout << "Regions n." << ( *It )->Order << " with name " << ( *It )->Name << std::endl;
    for( std::vector< Tools::VariableInfo >::iterator InIt = ( *It )->VarInfo.begin(); InIt != ( *It )->VarInfo.end(); ++InIt ) {
      std::cout << "      " << std::setw( 10 ) << ( *InIt ).Name << "  " << std::setw( 10 ) << ( *InIt ).Min << "  " << std::setw( 10 ) << ( *InIt ).Max << std::endl;
    }
  }
}
