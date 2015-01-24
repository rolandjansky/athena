/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
        line>>nCh[i];
        patchPanelIn[i] = new TGCPatchPanel* [nCh[i]];
        for( int j=0; j<nCh[i]; j+=1) patchPanelIn[i][j]=0;
        connectorIn[i] = new int [nCh[i]];
        channelIn[i] = new int [nCh[i]];
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
            patchPanelIn[oCon][oCh]=PP;
          }else{
            // Channel correspond to this output channel is in adjacent boards.
            if(PP->getAdjacentPP(0)!=0){
              if(PP->getAdjacentPP(0)->getId()==i1PP){
                patchPanelIn[oCon][oCh]=PP->getAdjacentPP(0);
              }
            }
            if(PP->getAdjacentPP(1)!=0){
              if(PP->getAdjacentPP(1)->getId()==i1PP){
                patchPanelIn[oCon][oCh]=PP->getAdjacentPP(1);
              }
            }
          }
          connectorIn[oCon][oCh]=iCon;
          channelIn[oCon][oCh]=iCh;
#ifdef TGCDEBUG_CONNECTION
          std::cout << "TGCConnectionInPP: " << label << " PPIn= " << i1PP 
               << " " << connectorIn[oCon][oCh]
               << " " << channelIn[oCon][oCh]
               << " PPOut= " << oPP  << " " << oCon << " " << oCh;
#endif 

          i2PP=iCon=iCh=0;
          line2 >> i2PP >> iCon >> iCh;
          // i2PP, iCon and iCh are PatchPanel ID, connector
          // ID and channel number ored.
          if((i2PP>0)||(iCon!=0)||(iCh!=0)){
            if(oredPatchPanelIn[oCon]==0){
              oredPatchPanelIn[oCon] = new TGCPatchPanel* [nCh[oCon]];
              oredConnectorIn[oCon] = new int [nCh[oCon]];
              oredChannelIn[oCon] = new int [nCh[oCon]];
              for(int  j=0; j<nCh[oCon]; j+=1) oredPatchPanelIn[oCon][j]=0;
            }
            oredConnectorIn[oCon][oCh]=iCon;
            oredChannelIn[oCon][oCh]=iCh;
#ifdef TGCDEBUG_CONNECTION
            std::cout << "  PPOR= " << i2PP << " " << oredConnectorIn[oCon][oCh]
                                       << " " << oredChannelIn[oCon][oCh];
#endif 

            if(PP->getId()==i2PP){
              oredPatchPanelIn[oCon][oCh]=PP;
            }else{
              if(PP->getAdjacentPP(0)!=0){
                if(PP->getAdjacentPP(0)->getId()==i2PP){
                  oredPatchPanelIn[oCon][oCh]=PP->getAdjacentPP(0);
                }
              }
              if(PP->getAdjacentPP(1)!=0){
                if(PP->getAdjacentPP(1)->getId()==i2PP){
                  oredPatchPanelIn[oCon][oCh]=PP->getAdjacentPP(1);
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
    if(oredPatchPanelIn[i]!=0) return true;
  }
  return false;
}

void TGCConnectionInPP::dumpConnection(int PPId) const
{
  std::ofstream file;
  file.open("dumpConnection.out", std::ios::out | std::ios::app);
  file.setf(std::ios::right);

  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    for( int j=0; j<nCh[i]; j+=1){
      if(patchPanelIn[i]!=0){
        file.width(3); file<<PPId;
        file.width(3); file<<i;
        file.width(3); file<<j;
        if(patchPanelIn[i][j]!=0){
          file.width(3);  file<<patchPanelIn[i][j]->getId();
          file.width(3);  file<<connectorIn[i][j];
          file.width(3);  file<<channelIn[i][j];
          if(oredPatchPanelIn[i]!=0){
            if(oredPatchPanelIn[i][j]!=0){
              file.width(3);  file<<oredPatchPanelIn[i][j]->getId();
              file.width(3);  file<<oredConnectorIn[i][j];
              file.width(3);  file<<oredChannelIn[i][j];
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
    nCh[i] = 0;
    patchPanelIn[i] = 0;
    channelIn[i] = 0;
    connectorIn[i] = 0;
    oredPatchPanelIn[i] = 0;
    oredChannelIn[i] = 0;
    oredConnectorIn[i] = 0;
  }
}

TGCConnectionInPP::~TGCConnectionInPP()
{
  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    if(patchPanelIn[i]!=0) delete [] patchPanelIn[i];
    patchPanelIn[i] = 0;
    if(channelIn[i]!=0) delete [] channelIn[i];
    channelIn[i] = 0;
    if(connectorIn[i]!=0) delete [] connectorIn[i];
    connectorIn[i] = 0;

    if(oredPatchPanelIn[i]!=0) delete [] oredPatchPanelIn[i];
    oredPatchPanelIn[i] = 0;
    if(oredChannelIn[i]!=0) delete [] oredChannelIn[i];
    oredChannelIn[i] = 0;
    if(oredConnectorIn[i]!=0) delete [] oredConnectorIn[i];
    oredConnectorIn[i] = 0;
  }
}


TGCConnectionInPP::TGCConnectionInPP(const TGCConnectionInPP& right)
{
  for( int i=0; i<NumberOfPPOutputConnector; i+=1){
    nCh[i] = right.nCh[i];
    int nPPOutCh = nCh[i];

    patchPanelIn[i] = 0;
    if(right.patchPanelIn[i]) {
      patchPanelIn[i] = new TGCPatchPanel* [nPPOutCh]; 
      for(int iCh=0; iCh<nPPOutCh; iCh++) patchPanelIn[i][iCh] = right.patchPanelIn[i][iCh];
    }

    channelIn[i] = 0;
    if(right.channelIn[i]) {
      channelIn[i] = new int [nPPOutCh];
      for(int iCh=0; iCh<nPPOutCh; iCh++) channelIn[i][iCh] = right.channelIn[i][iCh];
    }

    connectorIn[i] = 0;
    if(right.connectorIn[i]) {
      connectorIn[i] = new int [nPPOutCh];
      for(int iCh=0; iCh<nPPOutCh; iCh++) connectorIn[i][iCh] = right.connectorIn[i][iCh];
    }

    oredPatchPanelIn[i] = 0;
    if(right.oredPatchPanelIn[i]) {
       oredPatchPanelIn[i] = new TGCPatchPanel* [nPPOutCh]; 
       for(int iCh=0; iCh<nPPOutCh; iCh++) oredPatchPanelIn[i][iCh] = right.oredPatchPanelIn[i][iCh];
     }
 
    oredChannelIn[i] = 0;
     if(right.oredChannelIn[i]) {
       oredChannelIn[i] = new int [nPPOutCh];
       for(int iCh=0; iCh<nPPOutCh; iCh++) oredChannelIn[i][iCh] = right.oredChannelIn[i][iCh];
     }
     
     oredConnectorIn[i] = 0;
     if(right.oredConnectorIn[i]) {
       oredConnectorIn[i] = new int [nPPOutCh];
       for(int iCh=0; iCh<nPPOutCh; iCh++) oredConnectorIn[i][iCh] = right.oredConnectorIn[i][iCh];
     }
  }
}

TGCConnectionInPP& 
TGCConnectionInPP::operator=(const TGCConnectionInPP& right)
{
  if( this != &right ) {
    for( int i=0; i<NumberOfPPOutputConnector; i+=1){
      nCh[i] = right.nCh[i];
      int nPPOutCh = nCh[i];

      delete [] patchPanelIn[i];
      patchPanelIn[i] = 0;
      if(right.patchPanelIn[i]) {
	patchPanelIn[i] = new TGCPatchPanel* [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) patchPanelIn[i][iCh] = right.patchPanelIn[i][iCh];
      }

      delete [] channelIn[i];
      channelIn[i] = 0;
      if(right.channelIn[i]) {
	channelIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) channelIn[i][iCh] = right.channelIn[i][iCh];
      }
    
      delete [] connectorIn[i];
      connectorIn[i] = 0;
      if(right.connectorIn[i]) {
	connectorIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) connectorIn[i][iCh] = right.connectorIn[i][iCh];
      }

      delete [] oredPatchPanelIn[i];
      oredPatchPanelIn[i] = 0;
      if(right.oredPatchPanelIn[i]) {
	oredPatchPanelIn[i] = new TGCPatchPanel* [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) oredPatchPanelIn[i][iCh] = right.oredPatchPanelIn[i][iCh];
      }

      delete [] oredChannelIn[i];
      oredChannelIn[i] = 0;
      if(right.oredChannelIn[i]) {
	oredChannelIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) oredChannelIn[i][iCh] = right.oredChannelIn[i][iCh];
      }

      delete [] oredConnectorIn[i];
      oredConnectorIn[i] = 0;
      if(right.oredConnectorIn[i]) {
	oredConnectorIn[i] = new int [nPPOutCh];
	for(int iCh=0; iCh<nPPOutCh; iCh++) oredConnectorIn[i][iCh] = right.oredConnectorIn[i][iCh];
      }
    }
  }
  return *this;
}

int TGCConnectionInPP::getChannelIn(int connectorOut, int chOut)
{
  return channelIn[connectorOut][chOut];
}

int TGCConnectionInPP::getConnectorIn(int connectorOut, int chOut)
{
  return connectorIn[connectorOut][chOut];
}

TGCPatchPanel* TGCConnectionInPP::getPPIn(int connectorOut, int chOut)
{
  if(patchPanelIn[connectorOut]!=0)
    return patchPanelIn[connectorOut][chOut];
  else
    return 0;
}

int TGCConnectionInPP::getOredChannelIn(int connectorOut, int chOut)
{
  return oredChannelIn[connectorOut][chOut];
}

int TGCConnectionInPP::getOredConnectorIn(int connectorOut, int chOut)
{
  return oredConnectorIn[connectorOut][chOut];
}

TGCPatchPanel* TGCConnectionInPP::getOredPPIn(int connectorOut, int chOut)
{
  if(oredPatchPanelIn[connectorOut]!=0)
    return oredPatchPanelIn[connectorOut][chOut];
  else
    return 0;
}

void TGCConnectionInPP::setConnection(int out, int chOut, int in, int chIn, TGCPatchPanel* PP)
{
  if (patchPanelIn[out][chOut]==0) {
    patchPanelIn[out][chOut] = PP;
    connectorIn[out][chOut] = in;
    channelIn[out][chOut] = chIn;
  }else{
    // When the output channel correspond to the input channel has
    // already connected, the input channel is connected as ored channel.
    if(!existOredSignal()){
      // prepare arrays for ored channel.s
      int nPPOutCh = nCh[out];
      oredPatchPanelIn[out] = new TGCPatchPanel* [nPPOutCh];
      channelIn[out] = new int [nPPOutCh];
      connectorIn[out] = new int [nPPOutCh];
      oredPatchPanelIn[out] = new TGCPatchPanel* [nPPOutCh];
      oredChannelIn[out] = new int [nPPOutCh];
      oredConnectorIn[out] = new int [nPPOutCh];
    }
    if (oredPatchPanelIn[out][chOut]==0) {
      oredPatchPanelIn[out][chOut] = PP;
      oredConnectorIn[out][chOut] = in;
      oredChannelIn[out][chOut] = chIn;
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
    int nPPOutCh = nCh[i];
    if(patchPanelIn[i]) {
      for(int iCh=0; iCh<nPPOutCh; iCh++) {
	// Null pointers are NOT updated. 
	if(patchPanelIn[i][iCh]) {
	  if(       patchPanelIn[i][iCh]==oldPatchPanels.at(0)) {
	    patchPanelIn[i][iCh] = newPatchPanel;
	  } else if(patchPanelIn[i][iCh]==oldPatchPanels.at(1)) {
	    patchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(0);
	  } else if(patchPanelIn[i][iCh]==oldPatchPanels.at(2)) {
	    patchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(1);
	  } else { // This should not happen. 
	    return false;
	  }
	}
      }
    }

    if(oredPatchPanelIn[i]) {
      for(int iCh=0; iCh<nPPOutCh; iCh++) {
	// Null pointers are NOT updated. 
	if(oredPatchPanelIn[i][iCh]) {
	  if(       oredPatchPanelIn[i][iCh]==oldPatchPanels.at(0)) {
	    oredPatchPanelIn[i][iCh] = newPatchPanel;
	  } else if(oredPatchPanelIn[i][iCh]==oldPatchPanels.at(1)) {
	    oredPatchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(0);
	  } else if(oredPatchPanelIn[i][iCh]==oldPatchPanels.at(2)) {
	    oredPatchPanelIn[i][iCh] = newPatchPanel->getAdjacentPP(1);
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
