/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleUtils/HistHandler.h"
#include "CalibNtupleUtils/HistSetting.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <algorithm>
#include <sstream>
#include "TString.h"

HistHandler::HistHandler( std::string title, Int_t binning, Float_t min, Float_t max, const HistSetting* setting) 
  : m_title(title), m_bin(binning), m_min(min), m_max(max), m_binY(0), m_minY(0.0), m_maxY(0.0), m_setting(setting) {
  }

HistHandler::HistHandler( std::string title, Int_t binningX, Float_t minX, Float_t maxX,
			  Int_t binningY, Float_t minY, Float_t maxY, const HistSetting* setting)
  : m_title(title), m_bin(binningX), m_min(minX), m_max(maxX), m_binY(binningY), m_minY(minY), m_maxY(maxY), m_setting(setting) {
  }

void HistHandler::fill( const MuonCalib::MuonFixedId& Id, Float_t entry ) {
  std::string stationName = Id.stationNumberToFixedStationString( Id.stationName() );
  int abseta = std::abs(Id.eta());
  int phi = Id.phi();
  std::string side;
  if( Id.eta() > 0) side = "A";
  else side = "C" ;
  
  std::ostringstream histStream("");
  histStream << m_title << "_" << stationName << abseta << side << phi ;
  std::string histName = histStream.str();
  TString ThistName(histName);

  std::map<std::string, TH1F*>::iterator position = m_myMap.find( histName );
  if(position == m_myMap.end() ){
    TH1F* h = new TH1F(ThistName, ThistName, m_bin, m_min, m_max);
    if( m_setting ) m_setting->applySetting( h );
    m_myMap[ histName ] = h;
  }
  m_myMap[ histName ]->Fill(entry);

}

void HistHandler::fill( const MuonCalib::MuonFixedId& Id, Float_t entryX, Float_t entryY ) {
  std::string stationName = Id.stationNumberToFixedStationString( Id.stationName() );
  int abseta = std::abs(Id.eta());
  int phi = Id.phi();
  std::string side;
  if( Id.eta() > 0) side = "A";
  else side = "C" ;
  
  std::ostringstream histStream("");
  histStream << m_title << "_" << stationName << abseta << side << phi ;
  std::string histName = histStream.str();
  TString ThistName(histName);

  std::map<std::string, TH2F*>::iterator position = m_my2Map.find( histName );
  if(position == m_my2Map.end() ){
    TH2F* h  = new TH2F(ThistName, ThistName, m_bin, m_min, m_max, m_binY, m_minY, m_maxY);
    if( m_setting ) m_setting->applySetting( h );
    m_my2Map[ histName ] = h;
  }
  m_my2Map[ histName ]->Fill(entryX, entryY);

}

void HistHandler::write(){
  for( std::map<std::string, TH1F*>::const_iterator CI = m_myMap.begin(); CI != m_myMap.end(); ++ CI ){
    TString name( CI->first ) ;
    (CI->second)->Write(name);
    MsgStream log(Athena::getMessageSvc(),"HistHandler");
    log<<MSG::INFO<<" writing "<<name<<endmsg;
  }
}
 
TH1F* HistHandler::Hist( const MuonCalib::MuonFixedId& Id ) const {
  std::string stationName = Id.stationNumberToFixedStationString( Id.stationName() );
  int abseta = std::abs(Id.eta());
  int phi = Id.phi();
  std::string side;
  if( Id.eta() > 0) side = "A";
  else side = "C" ;
  
  std::ostringstream histStream("");
  histStream << m_title << "_" << stationName << abseta << side << phi ;
  std::string histName = histStream.str();
  std::map< std::string, TH1F*>::const_iterator position = m_myMap.find(histName);
  if( position != m_myMap.end() ) {
    return position->second;
  } else {
    MsgStream log(Athena::getMessageSvc(),"HistHandler");
    log<<MSG::INFO<<" This ID (" << histName << ") has no TH1F* assigned in the map"<<endmsg;
  }
  return 0;
}
 
 
TH1F* HistHandler::Hist(  const std::string tag  ) const {  
  std::ostringstream histStream("");
  histStream << m_title << "_" << tag ;
  std::string histName = histStream.str();
  std::map< std::string, TH1F*>::const_iterator position = m_myMap.find( histName );
  if( position != m_myMap.end() ) {
    return position->second;
  } else {
    MsgStream log(Athena::getMessageSvc(),"HistHandler");
    log<<MSG::INFO<<" This ID (" << histName << ") has no TH1F* assigned in the map"<<endmsg;
  }
  return 0;
}

TH2F* HistHandler::Hist2( const MuonCalib::MuonFixedId& Id ) const {
  std::string stationName = Id.stationNumberToFixedStationString( Id.stationName() );
  int abseta = std::abs(Id.eta());
  int phi = Id.phi();
  std::string side;
  if( Id.eta() > 0) side = "A";
  else side = "C" ;
  
  std::ostringstream histStream("");
  histStream << m_title << "_" << stationName << abseta << side << phi ;
  std::string histName = histStream.str();
   
  std::map< std::string, TH2F*>::const_iterator position2 = m_my2Map.find(histName);
  if( position2 != m_my2Map.end() ){ 
    return position2->second;
  } else {
    MsgStream log(Athena::getMessageSvc(),"HistHandler");
    log<<MSG::INFO<<" This ID (" << histName << ") has no TH2F* assigned in the map"<<endmsg;
  }
  return NULL;
}

TH2F* HistHandler::Hist2(  const std::string tag  ) const {  
  std::ostringstream histStream("");
  histStream << m_title << "_" << tag ;
  std::string histName = histStream.str();
  std::map< std::string, TH2F*>::const_iterator position = m_my2Map.find( histName );
  if( position != m_my2Map.end() ) {
    return position->second;
  } else {
    MsgStream log(Athena::getMessageSvc(),"HistHandler");
    log<<MSG::INFO<<" This ID (" << histName << ") has no TH2F* assigned in the map"<<endmsg;
  }
  return 0;
}
void HistHandler::fill(std::string tag , Float_t entry ) {
  std::ostringstream histStream("");
  histStream << m_title << "_" << tag;
  std::string histName = histStream.str();
  TString ThistName(histName);

  std::map<std::string, TH1F*>::iterator position = m_myMap.find( histName );
  if(position == m_myMap.end() ){
    m_myMap[ histName ] = new TH1F(ThistName, ThistName, m_bin, m_min, m_max);
  }
  m_myMap[ histName ]->Fill(entry);

}


void HistHandler::fill(std::string tag , Float_t entryX, Float_t entryY ) {
  std::ostringstream histStream("");
  histStream << m_title << "_" << tag;
  std::string histName = histStream.str();
  TString ThistName(histName);

  std::map<std::string, TH2F*>::iterator position = m_my2Map.find( histName );
  if(position == m_my2Map.end() ){
    m_my2Map[ histName ] = new TH2F(ThistName, ThistName, m_bin, m_min, m_max, m_binY, m_minY, m_maxY);
  }
  m_my2Map[ histName ]->Fill(entryX, entryY);
}
  
void HistHandler::fill(std::string tag , Float_t entryX, Float_t entryY, Float_t weight ) {
  std::ostringstream histStream("");
  histStream << m_title << "_" << tag;
  std::string histName = histStream.str();
  TString ThistName(histName);

  std::map<std::string, TH2F*>::iterator position = m_my2Map.find( histName );
  if(position == m_my2Map.end() ){
    m_my2Map[ histName ] = new TH2F(ThistName, ThistName, m_bin, m_min, m_max, m_binY, m_minY, m_maxY);
  }
  m_my2Map[ histName ]->Fill(entryX, entryY, weight);
}
  
double  HistHandler::maximum( std::string tag ) {
  std::ostringstream histStream("");
  histStream << m_title << "_" << tag;
  std::string histName = histStream.str();
  TString ThistName(histName);

  std::map<std::string, TH2F*>::iterator position = m_my2Map.find( histName );
  if(position == m_my2Map.end() ){
   return 0.;
  }
  return m_my2Map[ histName ]->GetMaximum();
  
}
