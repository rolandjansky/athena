/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCPatchPanel.hh"
#include "TrigT1TGC/TGCPatchPanelOut.hh"
#include "TrigT1TGC/TGCHitPattern.hh"
#include "TrigT1TGC/TGCConnectionInPP.hh"
#include "TrigT1TGC/TGCBIDOut.hh"
#include "TrigT1TGC/TGCASDOut.h"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include <iostream>
#include <string>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


namespace LVL1TGCTrigger {
  extern bool g_DEBUGLEVEL;

void TGCPatchPanel::showResult() const
{
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) return ;
  MsgStream m_log(msgSvc, "LVL1TGCTrigger::TGCPatchPanel");

  int i,j,k;
  if(hasASDOut){
    m_log << MSG::INFO
	  <<"#PP I "<<getTypeName(type)<<" Rgn: "<<region<<" PPID: "<<id
	  <<endmsg;
    for( i=0; i<MaxNumberOfConnector; i+=1){
      for( j=0; j<NChOfPPOutputConnector; j+=1){
        if(ASDOut[j][i]!=0){
          m_log << MSG::INFO   <<"\t "
		<<ASDOut[j][i]->GetSignalType()<<" "
		<<ASDOut[j][i]->GetTGCReadoutIndex().GetLayerNumber()<<" "
		<<ASDOut[j][i]->GetTGCReadoutIndex().GetRNumber()<<" "
		<<ASDOut[j][i]->GetHitID()<<" "
		<<id<<" "<<i<<" "<<j;
        }
      }
    }
    m_log  << endmsg;
  }

  if(hasBIDOut){
    m_log << MSG::INFO   
	  <<"#PP B "<<getTypeName(type)<<" Rgn: "<<region<<" PPID: "<<id;
    for( i=0; i<MaxNumberOfConnector; i+=1){
      for( j=0; j<NChOfPPOutputConnector; j+=1)
          if(BIDOut[j][i][0]!=0) m_log <<"\t con: "<<i<<" ch: "<<j;
    }
    m_log  << endmsg;
  }

  if(nHit>0){ // 18-Jan-01 Added by KH
     m_log << MSG::INFO   
	   << "#PP O "<< getTypeName(type)<<" Rgn: "<<region<<" PPID: "<<id;
    for( i=0; i<NumberOfPatchPanelOut; i+=1) {
      if(PPOut[i]!=0){
        for( k=0; k<NumberOfConnectorPerPPOut; k+=1) {
          const TGCHitPattern* pattern = PPOut[i]->getHitPattern(k);
          if(pattern!=0){
            int nCh = connectionInPP
              ->getNumberOfChannel(NumberOfConnectorPerPPOut*i+k);
            for( j=0; j<nCh; j+=1)
              if(pattern->getChannel(j))
                m_log <<"\t con: "<<NumberOfConnectorPerPPOut*i+k<<" ch: "<<j;
            pattern = 0;
          }
        }
      }
    }
    m_log << endmsg;
  } else {
    m_log << MSG::INFO 
	  <<"#PP O "<<getTypeName(type)<<" Rgn: "<<region<<" PPID: "<<id
	  << "  NO HIT " << endmsg;
  }
}


std::string TGCPatchPanel::getTypeName(int /*typeIn*/) const
{
  if(type==WTPP) return "WT";
  if(type==WDPP) return "WD";
  if(type==STPP) return "ST";
  if(type==SDPP) return "SD";
  if(type==WIPP) return "WI";
  if(type==SIPP) return "SI";
  return "NOT DEF";
}

TGCPatchPanel::TGCPatchPanel()
  :id(0), type(0), region(FORWARD),
   bunchCounter(0), hasASDOut(false), hasBIDOut(false), nHit(0)
{
  for(int i=0; i<NumberOfPatchPanelOut; i+=1) {
    idSlaveBoard[i] = 0;
    PPOut[i]=0;
  }
  for(int i=0; i<NChOfPPOutputConnector; i+=1){
    for(int j=0; j<MaxNumberOfConnector; j+=1){
      ASDOut[i][j]=0;
      for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
        BIDOut[i][j][k] = 0;
      }
    }
  }
  for(int i=0; i<2; i+=1) PPAdj[i] = 0;
  connectionInPP = 0;
}

void TGCPatchPanel::connect()
{
}

TGCPatchPanel::TGCPatchPanel(const TGCPatchPanel& right)
{
  id = right.id;
  type = right.type;
  region = right.region;
  bunchCounter = right.bunchCounter;
  hasASDOut = right.hasASDOut; 
  hasBIDOut = right.hasBIDOut;
  nHit = right.nHit;

  for(int i=0; i<NumberOfPatchPanelOut; i+=1){
    idSlaveBoard[i] = right.idSlaveBoard[i];
    PPOut[i] = new TGCPatchPanelOut(*right.PPOut[i]);
  }

  for(int i=0; i<NChOfPPOutputConnector; i+=1){
    for(int j=0; j<MaxNumberOfConnector; j+=1){
      ASDOut[i][j] = new TGCASDOut(*right.ASDOut[i][j]);
      for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
        BIDOut[i][j][k] = new TGCBIDOut(*right.BIDOut[i][j][k]);
      }
    }
  }

  for(int i=0; i<2; i+=1) PPAdj[i] = right.PPAdj[i];

  connectionInPP = new TGCConnectionInPP(*right.connectionInPP);
}

TGCPatchPanel&
TGCPatchPanel::operator=(const TGCPatchPanel& right)
{
  if (this != &right) {
    id = right.id;
    type = right.type;
    region = right.region;
    bunchCounter = right.bunchCounter;
    hasASDOut = right.hasASDOut; 
    hasBIDOut = right.hasBIDOut;

    for(int i=0; i<NumberOfPatchPanelOut; i+=1){
      idSlaveBoard[i] = right.idSlaveBoard[i];
      if ( PPOut[i] != 0 ) delete PPOut[i];
      PPOut[i] = new TGCPatchPanelOut(*right.PPOut[i]);
    }
    
    for(int i=0; i<NChOfPPOutputConnector; i+=1){
      for(int j=0; j<MaxNumberOfConnector; j+=1){
	if (ASDOut[i][j] !=0) delete ASDOut[i][j];
	ASDOut[i][j] = new TGCASDOut(*right.ASDOut[i][j]);
	for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
	  if(BIDOut[i][j][k]!=0) delete BIDOut[i][j][k];
	  BIDOut[i][j][k] = new TGCBIDOut(*right.BIDOut[i][j][k]);
	}
      }
    }

    for(int i=0; i<2; i+=1) PPAdj[i] = right.PPAdj[i];
    
    if(connectionInPP!=0) delete connectionInPP;
    connectionInPP = new TGCConnectionInPP(*right.connectionInPP);
  }
  return *this;
}

TGCPatchPanel::~TGCPatchPanel()
{
  for(int i=0; i<NumberOfPatchPanelOut; i+=1){
    if ( PPOut[i] != 0 ) delete PPOut[i];
    PPOut[i] = 0;
  }

  for(int i=0; i<NChOfPPOutputConnector; i+=1)
    for(int j=0; j<MaxNumberOfConnector; j+=1){
      ASDOut[i][j] = 0;
      for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
        if(BIDOut[i][j][k]!=0) delete BIDOut[i][j][k];
        BIDOut[i][j][k] = 0;
      }
    }

  for(int i=0; i<2; i+=1) PPAdj[i] = 0;

  if(connectionInPP!=0) delete connectionInPP;
  connectionInPP = 0;
}

void TGCPatchPanel::clockIn(int bunch, TGCDatabaseManager* db)
{
#ifdef TGCDEBUG
  std::cout << " Clock In " << getTypeName(type) << " " << id << std::endl;
#endif

  if(!connectionInPP){
    // Check if this PatchPanel is registered in TGCDatabaseManager 
    if(db) connectionInPP = db->getConnectionInPP(this);
    // If this PatchPanel is not found in TGCDatabaseManager, create new TGCConnectionInPP 
    if(!connectionInPP) {
      connectionInPP = new TGCConnectionInPP();
      connectionInPP->readConnectionTable(this);
      // Register PatchPanel and ConnectionInPP in TGCDatabaseManager
      if(db) db->addConnectionInPP(this, connectionInPP);
    }
  }

 if(bunchCounter!=bunch){
   bunchCounter = bunch;
   if(hasBIDOut) deleteBIDOut();
   if(hasASDOut){
     doBID();
#ifdef TGCDEBUG
     showResult();
#endif
     clearASDOut();
     return;
    }
  }else{
    nHit=createOutput();
    if(connectionInPP->existOredSignal()) nHit+=doOrLogic();
    // 18-Jan-01 Fixed by KH
    if( g_DEBUGLEVEL && ( (nHit>0) || hasBIDOut || hasASDOut) ) showResult();

#ifdef TGCDEBUG
    if  ( (nHit>0) || hasBIDOut || hasASDOut)  showResult();
#endif
    
  }
}


void TGCPatchPanel::deleteBIDOut()
{
  int i,j,l;

  for( i=0; i<MaxNumberOfConnector; i+=1)
    for( j=0; j<NChOfPPOutputConnector; j+=1){
      for ( l=0; l<NumberOfBunchKeptInPP; l+=1){
        if(BIDOut[j][i][l]!=0){
          delete BIDOut[j][i][l];
          BIDOut[j][i][l]=0;
        }
      }
    }

  hasBIDOut=false;
}

void TGCPatchPanel::clearASDOut()
{
  int i,j;

  for( i=0; i<MaxNumberOfConnector; i+=1)
    for( j=0; j<NChOfPPOutputConnector; j+=1)
      ASDOut[j][i]=0;
  hasASDOut=false;
}

void TGCPatchPanel::dumpPPOut()
{
  int i; 
  for( i=0; i<NumberOfPatchPanelOut; i+=1) 
    if(PPOut[i]!=0) PPOut[i]->print(); 
}

void TGCPatchPanel::dumpPPOut(int i)
{
  if(PPOut[i]!=0) PPOut[i]->print(); 
}

void TGCPatchPanel::showProperty()
{
  std::cout<<" PPID= "<<id<<" type= "<<type<<" Region= "<<region;
  int i;
  for( i=0; i<NumberOfPatchPanelOut; i+=1)
    std::cout<<" SBID["<<i<<"]= "<<idSlaveBoard[i];
  std::cout<<std::endl;
}

int TGCPatchPanel::createOutput()
{
  int i,j,k,l,con;
  int nCount=0;

  for( i=0; i<NumberOfPatchPanelOut; i+=1){
    if ( PPOut[i] != 0 ) delete PPOut[i];
    PPOut[i] = 0;
  }

#ifdef TGCDEBUG
  std::cerr << "TGCPatchPanel::createOutput() "
	    << getTypeName(type)<<" Rgn: "<<region<<" PPID: "<<id << std::endl;
#endif

  for( i=0; i<NumberOfPatchPanelOut; i+=1) {
    for ( k=0; k<NumberOfConnectorPerPPOut; k+=1) {
      con=NumberOfConnectorPerPPOut*i+k;
      int nCh = connectionInPP->getNumberOfChannel(con);   
      for( l=0; l<NumberOfBunchKeptInPP; l+=1){
        for( j=0; j<nCh; j+=1){
#ifdef TGCDEBUG
	  std::cerr << i << ":" << con << ":" << j <<": ";
#endif
          if(connectionInPP->getPPIn(con,j)!=0){
            if(connectionInPP->getPPIn(con,j)
               ->getBIDOut(connectionInPP->getChannelIn(con,j)
                   ,connectionInPP->getConnectorIn(con,j),l)!=0){
              if(PPOut[i]==0){
                PPOut[i]=new TGCPatchPanelOut;
                PPOut[i]->setOrigin(this);
                PPOut[i]->setBid(bunchCounter);
              }
              if(PPOut[i]->getHitPattern(k)==0){
                TGCHitPattern* pattern = new TGCHitPattern(nCh);
                PPOut[i]->setHitPattern(k,pattern);
                pattern = 0;
              }
              PPOut[i]->getHitPattern(k)->onChannel(j);
#ifdef TGCDEBUG
	      PPOut[i]->getHitPattern(k)->print(64);
#endif
              nCount+=1;
            } else {
#ifdef TGCDEBUG
	      std::cerr << "NO BID OUT" << std::endl;	  
#endif  
	    }
          } else { 
#ifdef TGCDEBUG
	    std::cerr << "NO PP IN" << std::endl;	 
#endif   
	  }
        }
      }
    }
  }
  return nCount;
}

int TGCPatchPanel::doOrLogic()
{
  int i,j,k,l,con;
  int nCount=0;

  for( i=0; i<NumberOfPatchPanelOut; i+=1) {
    for ( k=0; k<NumberOfConnectorPerPPOut; k+=1) {
      con=2*i+k;
      int nCh = connectionInPP->getNumberOfChannel(con);   
      for ( l=0; l<NumberOfBunchKeptInPP; l+=1){
        for ( j=0; j<nCh; j+=1)
          if(connectionInPP->getOredPPIn(con,j)!=0){
            if(connectionInPP->getOredPPIn(con,j)
               ->getBIDOut(connectionInPP->getOredChannelIn(con,j)
                           ,connectionInPP->getOredConnectorIn(con,j),l)!=0){
#ifdef TGCDEBUG_CONNECTION
              std::cout << "#PP Oring:PPOutID= " << i
                   << " ConID= " << con << " ChID= " << j
                   << " ORConID= " << connectionInPP->getOredConnectorIn(con,j)
                   << " ORChID= " << connectionInPP->getOredChannelIn(con,j)
                   << std::endl;
#endif 
              if(PPOut[i]==0){
                PPOut[i] = new TGCPatchPanelOut;
                PPOut[i]->setOrigin(this);
                PPOut[i]->setBid(bunchCounter);
              }
              if(PPOut[i]->getHitPattern(k)==0){
                TGCHitPattern* pattern = new TGCHitPattern(nCh);
                PPOut[i]->setHitPattern(k,pattern);
                pattern = 0;
              }
              PPOut[i]->getHitPattern(k)->onChannel(j);
              nCount+=1;
            }
          }
      }
    }
  }
  return nCount;
}

TGCBIDOut* TGCPatchPanel::getBIDOut(int ch, int connector, int bunch)
{
   int index = getInputConnectorIndex(connector);

#ifdef TGCDEBUG
   //   if(BIDOut[ch][index][bunch]!=0){
   //     std::cout <<"getBIDOut: ch = "<<ch<<" index = "<<index<<" bunch  = "<<bunch<<std::endl;
   //   }
#endif 
   return BIDOut[ch][index][bunch];
}

void TGCPatchPanel::doBID()
{
  int i,j;
  for ( i=0; i<NChOfPPOutputConnector; i+=1) 
    for( j=0; j<MaxNumberOfConnector; j+=1){

      if(BIDOut[i][j][0]!=0) delete BIDOut[i][j][0];
      BIDOut[i][j][0]=0;

      if(ASDOut[i][j]!=0){
        BIDOut[i][j][0] = new TGCBIDOut (ASDOut[i][j]);
              hasBIDOut=true;
              BIDOut[i][j][0]->setBid(bunchCounter);
      }
    }
}

int TGCPatchPanel::getIdSlaveBoard(int port) const
{
  return idSlaveBoard[port];
}

void TGCPatchPanel::setIdSlaveBoard(int port, int idIn)
{
  idSlaveBoard[port] = idIn;
}

int TGCPatchPanel::getId() const
{
  return id;
}

void TGCPatchPanel::setId(int idIn)
{
  id = idIn;
}

TGCPatchPanelOut* TGCPatchPanel::getOutput(int SBId) 
{
  int i;
  for( i=0; i<NumberOfPatchPanelOut; i+=1){
    if( idSlaveBoard[i]==SBId ) 
      return PPOut[i];
  }
  std::cerr << "TGCPatchPanel::getOutput:  illeagal SBID "<< SBId << std::endl; 
  return 0; 
} 

void TGCPatchPanel::eraseOutput(int SBId)
{
  int i;
  for( i=0; i<NumberOfPatchPanelOut; i+=1) 
    if ( idSlaveBoard[i]==SBId )
      PPOut[i]=0;
}

void TGCPatchPanel::setASDOut(int ch, int connector, TGCASDOut* asdOut)
{
#ifdef TGCDEBUG
  std::cout << "TGCPatchPanel::setASDOut  "
	    <<"#PP B "<<getTypeName(type)<<" Rgn: "<<region<<" PPID: "<<id
	    << std::endl;
  std::cout <<"setASDOut0: ch= "<<ch<<" con= "<<connector
	    <<" index= "<<getInputConnectorIndex(connector)<<std::endl;
  if(ASDOut[ch][getInputConnectorIndex(connector)]!=0) {
    std::cout<<"setASDOut: Double Count.";
  }
#endif
 
  hasASDOut=true;
  ASDOut[ch][getInputConnectorIndex(connector)] = (TGCASDOut*)asdOut;
   
}

// Input       Output
//lower/upper  
//----------------------
//  3/1        0,1(8)->0
//    8
//  2/0        2,3   ->1
//----------------------
//  7/5        4,5(8)->2
//
//  6/4        6,7   ->3
//----------------------
int TGCPatchPanel::getInputConnectorIndex(const int connectorId) const
{
  return connectorId;
}

void TGCPatchPanel::setAdjacentPP(int side, TGCPatchPanel* PP)
{
  PPAdj[side]=PP; 
#ifdef TGCDEBUG
  std::cout<<"TGCPatchPanel::setAdjacentPP: connect PP(type="<<PP->getType()<<",ID="<<PP->getId()<<")to Side"<<side<<" PP(type="<<this->getType()<<",ID="<<this->getId()<<")"<<std::endl;
#endif
}

} //end of namespace bracket
