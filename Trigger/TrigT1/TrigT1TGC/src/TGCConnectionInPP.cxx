/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionInPP.cxx,v 1.4 2009-03-20 17:48:53 isaya Exp $
#include "TrigT1TGC/TGCConnectionInPP.hh"
#include "TrigT1TGC/TGCPatchPanel.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "ctype.h"

#include "PathResolver/PathResolver.h"

namespace LVL1TGCTrigger {

void TGCConnectionInPP::readConnectionTable(TGCPatchPanel* PP)
{
  enum { BufferSize = 1024 };
  char buf[BufferSize];
  std::string label,Label;

  bool found = true;
  // select label used in database.
  if(PP->getRegion()==Endcap){
    if(PP->getType()==WTPP)      label="EWT";
    else if(PP->getType()==WDPP) label="EWD";
    else if(PP->getType()==STPP) label="EST"; 
    else if(PP->getType()==SDPP) label="ESD"; 
    else if(PP->getType()==WIPP) label="EWI"; 
    else if(PP->getType()==SIPP) label="ESI";
    else found = false;
  }else if(PP->getRegion()==Forward){
    if(PP->getType()==WTPP) label="FWT";
    else if(PP->getType()==WDPP) label="FWD";
    else if(PP->getType()==STPP) label="FST";
    else if(PP->getType()==SDPP) label="FSD";
    else if(PP->getType()==WIPP) label="FWI"; 
    else if(PP->getType()==SIPP) label="FSI";
    else found = false;
  }
  if (!found) return;


  std::string fn, fullName ;
//  fn = "PP.db" ;
  fn = TGCDatabaseManager::getFilename(1);

  fullName = PathResolver::find_file(fn.c_str(), "PWD");
  if( fullName.length() == 0 ) 
    fullName = PathResolver::find_file(fn.c_str(),"DATAPATH");
  std::ifstream file(fullName.c_str() ,std::ios::in);

  // find entries match in PatchPanel type.
  int oPP,oCon,oCh,i1PP,i2PP,totalCh;
  while(file.getline(buf,BufferSize)){
    std::istringstream line(buf);
    line>>Label>>totalCh;
    if(label==Label){
      // create arrays store entries in database.
      for( int i=0; i<NumberOfPPOutputConnector; i+=1){
        line>>m_nCh[i];
        m_patchPanelIn[i] = new TGCPatchPanel* [m_nCh[i]];
        for( int j=0; j<m_nCh[i]; j+=1) m_patchPanelIn[i][j]=0;
        m_connectorIn[i] = new int [m_nCh[i]];
        m_channelIn[i] = new int [m_nCh[i]];
      }
      while(file.getline(buf,BufferSize)&&(isspace(buf[0])||isdigit(buf[0]))){
        int iCon = 0;
        int iCh  = 0; 
        iCon=iCh=0;
        std::istringstream line2(buf);
        line2 >> oPP >> oCon >> oCh >> i1PP >> iCon >> iCh;
        // When i1PP is equal to -1, the channel isn't used.
        if((i1PP!=-1)&&(PP->getId()==oPP)){
          if(PP->getId()==i1PP){
            // Channel correspond to this output channel is in same board.
            m_patchPanelIn[oCon][oCh]=PP;
          }else{
            // Channel correspond to this output channel is in adjacent boards.
            if(PP->getAdjacentPP(0)!=0){
              if(PP->getAdjacentPP(0)->getId()==i1PP){
                m_patchPanelIn[oCon][oCh]=PP->getAdjacentPP(0);
              }
            }
            if(PP->getAdjacentPP(1)!=0){
              if(PP->getAdjacentPP(1)->getId()==i1PP){
                m_patchPanelIn[oCon][oCh]=PP->getAdjacentPP(1);
              }
            }
          }
          m_connectorIn[oCon][oCh]=iCon;
          m_channelIn[oCon][oCh]=iCh;
#ifdef TGCDEBUG_CONNECTION
          std::cout << "TGCConnectionInPP: " << label << " PPIn= " << i1PP 
               << " " << m_connectorIn[oCon][oCh]
               << " " << m_channelIn[oCon][oCh]
               << " PPOut= " << oPP  << " " << oCon << " " << oCh;
#endif 

          i2PP=iCon=iCh=0;
          line2 >> i2PP >> iCon >> iCh;
          // i2PP, iCon and iCh are PatchPanel ID, connector
          // ID and channel number ored.
          if((i2PP>0)||(iCon!=0)||(iCh!=0)){
            if(m_oredPatchPanelIn[oCon]==0){
              m_oredPatchPanelIn[oCon] = new TGCPatchPanel* [m_nCh[oCon]];
              m_oredConnectorIn[oCon] = new int [m_nCh[oCon]];
              m_oredChannelIn[oCon] = new int [m_nCh[oCon]];
              for(int  j=0; j<m_nCh[oCon]; j+=1) m_oredPatchPanelIn[oCon][j]=0;
            }
            m_oredConnectorIn[oCon][oCh]=iCon;
            m_oredChannelIn[oCon][oCh]=iCh;
#ifdef TGCDEBUG_CONNECTION
            std::cout << "  PPOR= " << i2PP << " " << m_oredConnectorIn[oCon][oCh]
                                       << " " << m_oredChannelIn[oCon][oCh];
#endif 

            if(PP->getId()==i2PP){
              m_oredPatchPanelIn[oCon][oCh]=PP;
            }else{
              if(PP->getAdjacentPP(0)!=0){
                if(PP->getAdjacentPP(0)->getId()==i2PP){
                  m_oredPatchPanelIn[oCon][oCh]=PP->getAdjacentPP(0);
                }
              }
              if(PP->getAdjacentPP(1)!=0){
                if(PP->getAdjacentPP(1)->getId()==i2PP){
                  m_oredPatchPanelIn[oCon][oCh]=PP->getAdjacentPP(1);
                }
              }
            }
          }

#ifdef TGCDEBUG_CONNECTION
          std::cout << std::endl;
#endif 

        }
      }
      break;
    }
  }
  file.close();
}

bool TGCConnectionInPP::existOredSignal() const
{
  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    if(m_oredPatchPanelIn[i]!=0) return true;
  }
  return false;
}

void TGCConnectionInPP::dumpConnection(int PPId) const
{
  std::ofstream file;
  file.open("dumpConnection.out", std::ios::out | std::ios::app);
  file.setf(std::ios::right);

  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    for( int j=0; j<m_nCh[i]; j+=1){
      if(m_patchPanelIn[i]!=0){
        file.width(3); file<<PPId;
        file.width(3); file<<i;
        file.width(3); file<<j;
        if(m_patchPanelIn[i][j]!=0){
          file.width(3);  file<<m_patchPanelIn[i][j]->getId();
          file.width(3);  file<<m_connectorIn[i][j];
          file.width(3);  file<<m_channelIn[i][j];
          if(m_oredPatchPanelIn[i]!=0){
            if(m_oredPatchPanelIn[i][j]!=0){
              file.width(3);  file<<m_oredPatchPanelIn[i][j]->getId();
              file.width(3);  file<<m_oredConnectorIn[i][j];
              file.width(3);  file<<m_oredChannelIn[i][j];
            }
          }
        }else{
          file.width(3);  file<<-1;
          file.width(3);  file<<-1;
          file.width(3);  file<<-1;
        }  
        file<<std::endl;
      }
    }
  }

  file.close();
}

TGCConnectionInPP::TGCConnectionInPP()
{
  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    m_nCh[i] = 0;
    m_patchPanelIn[i] = 0;
    m_channelIn[i] = 0;
    m_connectorIn[i] = 0;
    m_oredPatchPanelIn[i] = 0;
    m_oredChannelIn[i] = 0;
    m_oredConnectorIn[i] = 0;
  }
}

TGCConnectionInPP::~TGCConnectionInPP()
{
  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    if(m_patchPanelIn[i]!=0) delete [] m_patchPanelIn[i];
    m_patchPanelIn[i] = 0;
    if(m_channelIn[i]!=0) delete [] m_channelIn[i];
    m_channelIn[i] = 0;
    if(m_connectorIn[i]!=0) delete [] m_connectorIn[i];
    m_connectorIn[i] = 0;

    if(m_oredPatchPanelIn[i]!=0) delete [] m_oredPatchPanelIn[i];
    m_oredPatchPanelIn[i] = 0;
    if(m_oredChannelIn[i]!=0) delete [] m_oredChannelIn[i];
    m_oredChannelIn[i] = 0;
    if(m_oredConnectorIn[i]!=0) delete [] m_oredConnectorIn[i];
    m_oredConnectorIn[i] = 0;
  }
}


TGCConnectionInPP::TGCConnectionInPP(const TGCConnectionInPP& right)
{
  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    m_nCh[i] = right.m_nCh[i];
    int nPPOutCh = m_nCh[i];

    m_patchPanelIn[i] = 0;
    if(right.m_patchPanelIn[i]) {
      m_patchPanelIn[i] = new TGCPatchPanel* [nPPOutCh]; 
      for(int iCh=0; iCh<nPPOutCh; iCh++) m_patchPanelIn[i][iCh] = right.m_patchPanelIn[i][iCh];
    }

    m_channelIn[i] = 0;
    if(right.m_channelIn[i]) {
      m_channelIn[i] = new int [nPPOutCh];
      for(int iCh=0; iCh<nPPOutCh; iCh++) m_channelIn[i][iCh] = right.m_channelIn[i][iCh];
    }

    m_connectorIn[i] = 0;
    if(right.m_connectorIn[i]) {
      m_connectorIn[i] = new int [nPPOutCh];
      for(int iCh=0; iCh<nPPOutCh; iCh++) m_connectorIn[i][iCh] = right.m_connectorIn[i][iCh];
    }

    m_oredPatchPanelIn[i] = 0;
    if(right.m_oredPatchPanelIn[i]) {
       m_oredPatchPanelIn[i] = new TGCPatchPanel* [nPPOutCh]; 
       for(int iCh=0; iCh<nPPOutCh; iCh++) m_oredPatchPanelIn[i][iCh] = right.m_oredPatchPanelIn[i][iCh];
     }
 
    m_oredChannelIn[i] = 0;
     if(right.m_oredChannelIn[i]) {
       m_oredChannelIn[i] = new int [nPPOutCh];
       for(int iCh=0; iCh<nPPOutCh; iCh++) m_oredChannelIn[i][iCh] = right.m_oredChannelIn[i][iCh];
     }
     
     m_oredConnectorIn[i] = 0;
     if(right.m_oredConnectorIn[i]) {
       m_oredConnectorIn[i] = new int [nPPOutCh];
       for(int iCh=0; iCh<nPPOutCh; iCh++) m_oredConnectorIn[i][iCh] = right.m_oredConnectorIn[i][iCh];
     }
  }
}

TGCConnectionInPP& 
TGCConnectionInPP::operator=(const TGCConnectionInPP& right)
{
  if( this != &right ) {
    for( int i=0; i<NumberOfPPOutputConnector; i+=1){
      m_nCh[i] = right.m_nCh[i];
      int nPPOutCh = m_nCh[i];

      delete [] m_patchPanelIn[i];
      m_patchPanelIn[i] = 0;
      if(right.m_patchPanelIn[i]) {
	m_patchPanelIn[i] = new TGCPatchPanel* [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) m_patchPanelIn[i][iCh] = right.m_patchPanelIn[i][iCh];
      }

      delete [] m_channelIn[i];
      m_channelIn[i] = 0;
      if(right.m_channelIn[i]) {
	m_channelIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) m_channelIn[i][iCh] = right.m_channelIn[i][iCh];
      }
    
      delete [] m_connectorIn[i];
      m_connectorIn[i] = 0;
      if(right.m_connectorIn[i]) {
	m_connectorIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) m_connectorIn[i][iCh] = right.m_connectorIn[i][iCh];
      }

      delete [] m_oredPatchPanelIn[i];
      m_oredPatchPanelIn[i] = 0;
      if(right.m_oredPatchPanelIn[i]) {
	m_oredPatchPanelIn[i] = new TGCPatchPanel* [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) m_oredPatchPanelIn[i][iCh] = right.m_oredPatchPanelIn[i][iCh];
      }

      delete [] m_oredChannelIn[i];
      m_oredChannelIn[i] = 0;
      if(right.m_oredChannelIn[i]) {
	m_oredChannelIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) m_oredChannelIn[i][iCh] = right.m_oredChannelIn[i][iCh];
      }

      delete [] m_oredConnectorIn[i];
      m_oredConnectorIn[i] = 0;
      if(right.m_oredConnectorIn[i]) {
	m_oredConnectorIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) m_oredConnectorIn[i][iCh] = right.m_oredConnectorIn[i][iCh];
      }
    }
  }
  return *this;
}

int TGCConnectionInPP::getChannelIn(int connectorOut, int chOut)
{
  return m_channelIn[connectorOut][chOut];
}

int TGCConnectionInPP::getConnectorIn(int connectorOut, int chOut)
{
  return m_connectorIn[connectorOut][chOut];
}

TGCPatchPanel* TGCConnectionInPP::getPPIn(int connectorOut, int chOut)
{
  if(m_patchPanelIn[connectorOut]!=0)
    return m_patchPanelIn[connectorOut][chOut];
  else
    return 0;
}

int TGCConnectionInPP::getOredChannelIn(int connectorOut, int chOut)
{
  return m_oredChannelIn[connectorOut][chOut];
}

int TGCConnectionInPP::getOredConnectorIn(int connectorOut, int chOut)
{
  return m_oredConnectorIn[connectorOut][chOut];
}

TGCPatchPanel* TGCConnectionInPP::getOredPPIn(int connectorOut, int chOut)
{
  if(m_oredPatchPanelIn[connectorOut]!=0)
    return m_oredPatchPanelIn[connectorOut][chOut];
  else
    return 0;
}

void TGCConnectionInPP::setConnection(int out, int chOut, int in, int chIn, TGCPatchPanel* PP)
{
  if (m_patchPanelIn[out][chOut]==0) {
    m_patchPanelIn[out][chOut] = PP;
    m_connectorIn[out][chOut] = in;
    m_channelIn[out][chOut] = chIn;
  }else{
    // When the output channel correspond to the input channel has
    // already connected, the input channel is connected as ored channel.
    if(!existOredSignal()){
      // prepare arrays for ored channel.s
      int nPPOutCh = m_nCh[out];
      m_oredPatchPanelIn[out] = new TGCPatchPanel* [nPPOutCh];
      m_channelIn[out] = new int [nPPOutCh];
      m_connectorIn[out] = new int [nPPOutCh];
      m_oredPatchPanelIn[out] = new TGCPatchPanel* [nPPOutCh];
      m_oredChannelIn[out] = new int [nPPOutCh];
      m_oredConnectorIn[out] = new int [nPPOutCh];
    }
    if (m_oredPatchPanelIn[out][chOut]==0) {
      m_oredPatchPanelIn[out][chOut] = PP;
      m_oredConnectorIn[out][chOut] = in;
      m_oredChannelIn[out][chOut] = chIn;
    }
   } 
}

bool TGCConnectionInPP::replacePatchPanelPointers(TGCPatchPanel* newPatchPanel, 
						  const std::vector<const TGCPatchPanel*> oldPatchPanels)
{
  // The size of oldPatchPanels should be 3. 
  // oldPatchPanels.at(0) : oldPatchPanel
  // oldPatchPanels.at(1) : oldPatchPanel->getAdjacentPP(0)
  // oldPatchPanels.at(2) : oldPatchPanel->getAdjacentPP(1)
  if(oldPatchPanels.size()!=3) return false;

  for(int i=0; i<NumberOfPPOutputConnector; i++) {
    int nPPOutCh = m_nCh[i];
    if(m_patchPanelIn[i]) {
      for(int iCh=0; iCh<nPPOutCh; iCh++) {
	// Null pointers are NOT updated. 
	if(m_patchPanelIn[i][iCh]) {
	  if(       m_patchPanelIn[i][iCh]==oldPatchPanels.at(0)) {
	    m_patchPanelIn[i][iCh] = newPatchPanel;
	  } else if(m_patchPanelIn[i][iCh]==oldPatchPanels.at(1)) {
	    m_patchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(0);
	  } else if(m_patchPanelIn[i][iCh]==oldPatchPanels.at(2)) {
	    m_patchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(1);
	  } else { // This should not happen. 
	    return false;
	  }
	}
      }
    }

    if(m_oredPatchPanelIn[i]) {
      for(int iCh=0; iCh<nPPOutCh; iCh++) {
	// Null pointers are NOT updated. 
	if(m_oredPatchPanelIn[i][iCh]) {
	  if(       m_oredPatchPanelIn[i][iCh]==oldPatchPanels.at(0)) {
	    m_oredPatchPanelIn[i][iCh] = newPatchPanel;
	  } else if(m_oredPatchPanelIn[i][iCh]==oldPatchPanels.at(1)) {
	    m_oredPatchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(0);
	  } else if(m_oredPatchPanelIn[i][iCh]==oldPatchPanels.at(2)) {
	    m_oredPatchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(1);
	  } else { // This should not happen. 
	    return false;
	  }
	}
      }
    }
  }

  return true;
}

} //end of namespace bracket
