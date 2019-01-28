/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  MsgStream log(msgSvc, "LVL1TGCTrigger::TGCPatchPanel");

  int i,j,k;
  if(m_hasASDOut){
    log << MSG::INFO
	  <<"#PP I "<<getTypeName(m_type)<<" Rgn: "<<m_region<<" PPID: "<<m_id
	  <<endmsg;
    for( i=0; i<MaxNumberOfConnector; i+=1){
      for( j=0; j<NChOfPPOutputConnector; j+=1){
        if(m_ASDOut[j][i]!=0){
          log << MSG::INFO   <<"\t "
		<<m_ASDOut[j][i]->GetSignalType()<<" "
		<<m_ASDOut[j][i]->GetTGCReadoutIndex().GetLayerNumber()<<" "
		<<m_ASDOut[j][i]->GetTGCReadoutIndex().GetRNumber()<<" "
		<<m_ASDOut[j][i]->GetHitID()<<" "
		<<m_id<<" "<<i<<" "<<j;
        }
      }
    }
    log  << endmsg;
  }

  if(m_hasBIDOut){
    log << MSG::INFO   
	  <<"#PP B "<<getTypeName(m_type)<<" Rgn: "<<m_region<<" PPID: "<<m_id;
    for( i=0; i<MaxNumberOfConnector; i+=1){
      for( j=0; j<NChOfPPOutputConnector; j+=1)
          if(m_BIDOut[j][i][0]!=0) log <<"\t con: "<<i<<" ch: "<<j;
    }
    log  << endmsg;
  }

  if(m_nHit>0){ // 18-Jan-01 Added by KH
     log << MSG::INFO   
	   << "#PP O "<< getTypeName(m_type)<<" Rgn: "<<m_region<<" PPID: "<<m_id;
    for( i=0; i<NumberOfPatchPanelOut; i+=1) {
      if(m_PPOut[i]!=0){
        for( k=0; k<NumberOfConnectorPerPPOut; k+=1) {
          const TGCHitPattern* pattern = m_PPOut[i]->getHitPattern(k);
          if(pattern!=0){
            int nCh = m_connectionInPP
              ->getNumberOfChannel(NumberOfConnectorPerPPOut*i+k);
            for( j=0; j<nCh; j+=1)
              if(pattern->getChannel(j))
                log <<"\t con: "<<NumberOfConnectorPerPPOut*i+k<<" ch: "<<j;
            pattern = 0;
          }
        }
      }
    }
    log << endmsg;
  } else {
    log << MSG::INFO 
	  <<"#PP O "<<getTypeName(m_type)<<" Rgn: "<<m_region<<" PPID: "<<m_id
	  << "  NO HIT " << endmsg;
  }
}


std::string TGCPatchPanel::getTypeName(int /*typeIn*/) const
{
  if(m_type==WTPP) return "WT";
  if(m_type==WDPP) return "WD";
  if(m_type==STPP) return "ST";
  if(m_type==SDPP) return "SD";
  if(m_type==WIPP) return "WI";
  if(m_type==SIPP) return "SI";
  return "NOT DEF";
}

TGCPatchPanel::TGCPatchPanel()
  :m_id(0), m_type(0), m_region(FORWARD),
   m_bunchCounter(0), m_hasASDOut(false), m_hasBIDOut(false), m_nHit(0)
{
  for(int i=0; i<NumberOfPatchPanelOut; i+=1) {
    m_idSlaveBoard[i] = 0;
    m_PPOut[i]=0;
  }
  for(int i=0; i<NChOfPPOutputConnector; i+=1){
    for(int j=0; j<MaxNumberOfConnector; j+=1){
      m_ASDOut[i][j]=0;
      for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
        m_BIDOut[i][j][k] = 0;
      }
    }
  }
  for(int i=0; i<2; i+=1) m_PPAdj[i] = 0;
  m_connectionInPP = 0;
}

void TGCPatchPanel::connect()
{
}

TGCPatchPanel::TGCPatchPanel(const TGCPatchPanel& right)
{
  m_id = right.m_id;
  m_type = right.m_type;
  m_region = right.m_region;
  m_bunchCounter = right.m_bunchCounter;
  m_hasASDOut = right.m_hasASDOut; 
  m_hasBIDOut = right.m_hasBIDOut;
  m_nHit = right.m_nHit;

  for(int i=0; i<NumberOfPatchPanelOut; i+=1){
    m_idSlaveBoard[i] = right.m_idSlaveBoard[i];
    m_PPOut[i] = new TGCPatchPanelOut(*right.m_PPOut[i]);
  }

  for(int i=0; i<NChOfPPOutputConnector; i+=1){
    for(int j=0; j<MaxNumberOfConnector; j+=1){
      m_ASDOut[i][j] = new TGCASDOut(*right.m_ASDOut[i][j]);
      for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
        m_BIDOut[i][j][k] = new TGCBIDOut(*right.m_BIDOut[i][j][k]);
      }
    }
  }

  for(int i=0; i<2; i+=1) m_PPAdj[i] = right.m_PPAdj[i];

  m_connectionInPP = new TGCConnectionInPP(*right.m_connectionInPP);
}

TGCPatchPanel&
TGCPatchPanel::operator=(const TGCPatchPanel& right)
{
  if (this != &right) {
    m_id = right.m_id;
    m_type = right.m_type;
    m_region = right.m_region;
    m_bunchCounter = right.m_bunchCounter;
    m_hasASDOut = right.m_hasASDOut; 
    m_hasBIDOut = right.m_hasBIDOut;

    for(int i=0; i<NumberOfPatchPanelOut; i+=1){
      m_idSlaveBoard[i] = right.m_idSlaveBoard[i];
      if ( m_PPOut[i] != 0 ) delete m_PPOut[i];
      m_PPOut[i] = new TGCPatchPanelOut(*right.m_PPOut[i]);
    }
    
    for(int i=0; i<NChOfPPOutputConnector; i+=1){
      for(int j=0; j<MaxNumberOfConnector; j+=1){
	if (m_ASDOut[i][j] !=0) delete m_ASDOut[i][j];
	m_ASDOut[i][j] = new TGCASDOut(*right.m_ASDOut[i][j]);
	for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
	  if(m_BIDOut[i][j][k]!=0) delete m_BIDOut[i][j][k];
	  m_BIDOut[i][j][k] = new TGCBIDOut(*right.m_BIDOut[i][j][k]);
	}
      }
    }

    for(int i=0; i<2; i+=1) m_PPAdj[i] = right.m_PPAdj[i];
    
    if(m_connectionInPP!=0) delete m_connectionInPP;
    m_connectionInPP = new TGCConnectionInPP(*right.m_connectionInPP);
  }
  return *this;
}

TGCPatchPanel::~TGCPatchPanel()
{
  for(int i=0; i<NumberOfPatchPanelOut; i+=1){
    if ( m_PPOut[i] != 0 ) delete m_PPOut[i];
    m_PPOut[i] = 0;
  }

  for(int i=0; i<NChOfPPOutputConnector; i+=1)
    for(int j=0; j<MaxNumberOfConnector; j+=1){
      m_ASDOut[i][j] = 0;
      for(int k=0; k<NumberOfBunchKeptInPP; k+=1){
        if(m_BIDOut[i][j][k]!=0) delete m_BIDOut[i][j][k];
        m_BIDOut[i][j][k] = 0;
      }
    }

  for(int i=0; i<2; i+=1) m_PPAdj[i] = 0;

  if(m_connectionInPP!=0) delete m_connectionInPP;
  m_connectionInPP = 0;
}

void TGCPatchPanel::clockIn(int bunch, TGCDatabaseManager* db)
{
#ifdef TGCDEBUG
  std::cout << " Clock In " << getTypeName(m_type) << " " << m_id << std::endl;
#endif

  if(!m_connectionInPP){
    // Check if this PatchPanel is registered in TGCDatabaseManager 
    if(db) m_connectionInPP = db->getConnectionInPP(this);
    // If this PatchPanel is not found in TGCDatabaseManager, create new TGCConnectionInPP 
    if(!m_connectionInPP) {
      m_connectionInPP = new TGCConnectionInPP();
      m_connectionInPP->readConnectionTable(this);
      // Register PatchPanel and ConnectionInPP in TGCDatabaseManager
      if(db) db->addConnectionInPP(this, m_connectionInPP);
    }
  }

 if(m_bunchCounter!=bunch){
   m_bunchCounter = bunch;
   if(m_hasBIDOut) deleteBIDOut();
   if(m_hasASDOut){
     doBID();
#ifdef TGCDEBUG
     showResult();
#endif
     clearASDOut();
     return;
    }
  }else{
    m_nHit=createOutput();
    if(m_connectionInPP->existOredSignal()) m_nHit+=doOrLogic();
    // 18-Jan-01 Fixed by KH
    if( g_DEBUGLEVEL && ( (m_nHit>0) || m_hasBIDOut || m_hasASDOut) ) showResult();

#ifdef TGCDEBUG
    if  ( (m_nHit>0) || m_hasBIDOut || m_hasASDOut)  showResult();
#endif
    
  }
}


void TGCPatchPanel::deleteBIDOut()
{
  int i,j,l;

  for( i=0; i<MaxNumberOfConnector; i+=1)
    for( j=0; j<NChOfPPOutputConnector; j+=1){
      for ( l=0; l<NumberOfBunchKeptInPP; l+=1){
        if(m_BIDOut[j][i][l]!=0){
          delete m_BIDOut[j][i][l];
          m_BIDOut[j][i][l]=0;
        }
      }
    }

  m_hasBIDOut=false;
}

void TGCPatchPanel::clearASDOut()
{
  int i,j;

  for( i=0; i<MaxNumberOfConnector; i+=1)
    for( j=0; j<NChOfPPOutputConnector; j+=1)
      m_ASDOut[j][i]=0;
  m_hasASDOut=false;
}

void TGCPatchPanel::dumpPPOut()
{
  int i; 
  for( i=0; i<NumberOfPatchPanelOut; i+=1) 
    if(m_PPOut[i]!=0) m_PPOut[i]->print(); 
}

void TGCPatchPanel::dumpPPOut(int i)
{
  if(m_PPOut[i]!=0) m_PPOut[i]->print(); 
}

void TGCPatchPanel::showProperty()
{
  std::cout<<" PPID= "<<m_id<<" type= "<<m_type<<" Region= "<<m_region;
  int i;
  for( i=0; i<NumberOfPatchPanelOut; i+=1)
    std::cout<<" SBID["<<i<<"]= "<<m_idSlaveBoard[i];
  std::cout<<std::endl;
}

int TGCPatchPanel::createOutput()
{
  int i,j,k,l,con;
  int nCount=0;

  for( i=0; i<NumberOfPatchPanelOut; i+=1){
    if ( m_PPOut[i] != 0 ) delete m_PPOut[i];
    m_PPOut[i] = 0;
  }

#ifdef TGCDEBUG
  std::cerr << "TGCPatchPanel::createOutput() "
	    << getTypeName(m_type)<<" Rgn: "<<m_region<<" PPID: "<<m_id << std::endl;
#endif

  for( i=0; i<NumberOfPatchPanelOut; i+=1) {
    for ( k=0; k<NumberOfConnectorPerPPOut; k+=1) {
      con=NumberOfConnectorPerPPOut*i+k;
      int nCh = m_connectionInPP->getNumberOfChannel(con);   
      for( l=0; l<NumberOfBunchKeptInPP; l+=1){
        for( j=0; j<nCh; j+=1){
#ifdef TGCDEBUG
	  std::cerr << i << ":" << con << ":" << j <<": ";
#endif
          if(m_connectionInPP->getPPIn(con,j)!=0){
            if(m_connectionInPP->getPPIn(con,j)
               ->getBIDOut(m_connectionInPP->getChannelIn(con,j)
                   ,m_connectionInPP->getConnectorIn(con,j),l)!=0){
              if(m_PPOut[i]==0){
                m_PPOut[i]=new TGCPatchPanelOut;
                m_PPOut[i]->setOrigin(this);
                m_PPOut[i]->setBid(m_bunchCounter);
              }
              if(m_PPOut[i]->getHitPattern(k)==0){
                TGCHitPattern* pattern = new TGCHitPattern(nCh);
                m_PPOut[i]->setHitPattern(k,pattern);
                pattern = 0;
              }
              m_PPOut[i]->getHitPattern(k)->onChannel(j);
#ifdef TGCDEBUG
	      m_PPOut[i]->getHitPattern(k)->print(64);
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
      int nCh = m_connectionInPP->getNumberOfChannel(con);   
      for ( l=0; l<NumberOfBunchKeptInPP; l+=1){
        for ( j=0; j<nCh; j+=1)
          if(m_connectionInPP->getOredPPIn(con,j)!=0){
            if(m_connectionInPP->getOredPPIn(con,j)
               ->getBIDOut(m_connectionInPP->getOredChannelIn(con,j)
                           ,m_connectionInPP->getOredConnectorIn(con,j),l)!=0){
#ifdef TGCDEBUG_CONNECTION
              std::cout << "#PP Oring:PPOutID= " << i
                   << " ConID= " << con << " ChID= " << j
                   << " ORConID= " << m_connectionInPP->getOredConnectorIn(con,j)
                   << " ORChID= " << m_connectionInPP->getOredChannelIn(con,j)
                   << std::endl;
#endif 
              if(m_PPOut[i]==0){
                m_PPOut[i] = new TGCPatchPanelOut;
                m_PPOut[i]->setOrigin(this);
                m_PPOut[i]->setBid(m_bunchCounter);
              }
              if(m_PPOut[i]->getHitPattern(k)==0){
                TGCHitPattern* pattern = new TGCHitPattern(nCh);
                m_PPOut[i]->setHitPattern(k,pattern);
                pattern = 0;
              }
              m_PPOut[i]->getHitPattern(k)->onChannel(j);
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
   //   if(m_BIDOut[ch][index][bunch]!=0){
   //     std::cout <<"getBIDOut: ch = "<<ch<<" index = "<<index<<" bunch  = "<<bunch<<std::endl;
   //   }
#endif 
   return m_BIDOut[ch][index][bunch];
}

void TGCPatchPanel::doBID()
{
  int i,j;
  for ( i=0; i<NChOfPPOutputConnector; i+=1) 
    for( j=0; j<MaxNumberOfConnector; j+=1){

      if(m_BIDOut[i][j][0]!=0) delete m_BIDOut[i][j][0];
      m_BIDOut[i][j][0]=0;

      if(m_ASDOut[i][j]!=0){
        m_BIDOut[i][j][0] = new TGCBIDOut (m_ASDOut[i][j]);
              m_hasBIDOut=true;
              m_BIDOut[i][j][0]->setBid(m_bunchCounter);
      }
    }
}

int TGCPatchPanel::getIdSlaveBoard(int port) const
{
  return m_idSlaveBoard[port];
}

void TGCPatchPanel::setIdSlaveBoard(int port, int idIn)
{
  m_idSlaveBoard[port] = idIn;
}

int TGCPatchPanel::getId() const
{
  return m_id;
}

void TGCPatchPanel::setId(int idIn)
{
  m_id = idIn;
}

TGCPatchPanelOut* TGCPatchPanel::getOutput(int SBId) 
{
  int i;
  for( i=0; i<NumberOfPatchPanelOut; i+=1){
    if( m_idSlaveBoard[i]==SBId ) 
      return m_PPOut[i];
  }
  std::cerr << "TGCPatchPanel::getOutput:  illeagal SBID "<< SBId << std::endl; 
  return 0; 
} 

void TGCPatchPanel::eraseOutput(int SBId)
{
  int i;
  for( i=0; i<NumberOfPatchPanelOut; i+=1) 
    if ( m_idSlaveBoard[i]==SBId )
      m_PPOut[i]=0;
}

void TGCPatchPanel::setASDOut(int ch, int connector, TGCASDOut* asdOut)
{
#ifdef TGCDEBUG
  std::cout << "TGCPatchPanel::setASDOut  "
	    <<"#PP B "<<getTypeName(m_type)<<" Rgn: "<<m_region<<" PPID: "<<m_id
	    << std::endl;
  std::cout <<"setASDOut0: ch= "<<ch<<" con= "<<connector
	    <<" index= "<<getInputConnectorIndex(connector)<<std::endl;
  if(m_ASDOut[ch][getInputConnectorIndex(connector)]!=0) {
    std::cout<<"setASDOut: Double Count.";
  }
#endif
 
  m_hasASDOut=true;
  m_ASDOut[ch][getInputConnectorIndex(connector)] = (TGCASDOut*)asdOut;
   
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
  m_PPAdj[side]=PP; 
#ifdef TGCDEBUG
  std::cout<<"TGCPatchPanel::setAdjacentPP: connect PP(type="<<PP->getType()<<",ID="<<PP->getId()<<")to Side"<<side<<" PP(type="<<this->getType()<<",ID="<<this->getId()<<")"<<std::endl;
#endif
}

} //end of namespace bracket
