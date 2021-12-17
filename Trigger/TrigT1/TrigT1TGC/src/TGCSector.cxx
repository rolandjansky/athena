/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSector.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCConnectionPPToSL.h"
#include "TrigT1TGC/TGCConnectionHPBToSL.h"
#include "TrigT1TGC/TGCConnectionPPToSB.h"
#include "TrigT1TGC/TGCConnectionASDToPP.h"
#include "TrigT1TGC/TGCASDOut.h"
#include "TrigT1TGC/TGCPatchPanel.h"
#include "TrigT1TGC/TGCSlaveBoard.h"
#include "TrigT1TGC/TGCHighPtBoard.h"
#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCInnerSB.h"
#include "TrigT1TGC/TGCStripHighPtBoard.h"
#include "TrigT1TGC/TGCWireHighPtBoard.h"
#include "TrigT1TGC/TGCStripDoubletSB.h"
#include "TrigT1TGC/TGCWireDoubletSB.h"
#include "TrigT1TGC/TGCWireTripletSB.h"


#include <iostream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

namespace LVL1TGCTrigger {

int TGCSector::distributeSignal(const TGCASDOut* ASDOut)
{
  int idPP,conPP,chPP;

  TGCSignalType signalType = ASDOut->GetSignalType();
  
  TGCReadoutIndex readoutIndex;
  readoutIndex = ASDOut->GetTGCReadoutIndex();
  
  int layer = readoutIndex.GetLayerNumber();
  int rNumber = readoutIndex.GetRNumber();
  int ch = ASDOut->GetHitID();

  int PPType = getPatchPanelType(signalType,layer);
  int notFound;
  if (PPType<0){
    notFound = -1;
  } else {
    notFound = m_ASDToPP[PPType]->getConnection(m_sideId,layer,rNumber,ch,&idPP,&conPP,&chPP);
  }

  if (tgcArgs()->MSGLEVEL() <= MSG::DEBUG) {
    IMessageSvc* msgSvc = 0;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
      return -1;
    };
    MsgStream log(msgSvc, "LVL1TGCTrigger::TGCSector");
    if (!notFound) {
      log << MSG::DEBUG
	    << "signalType= " << ((signalType==WireGroup) ? "Wire" : "Strip")
	    <<" layer= " <<layer <<" rNumber= " <<rNumber <<" ch= "<< ch
	    <<"id(PP)= " <<idPP <<" connector(PP)= " <<conPP <<" ch(PP)= " <<chPP
           <<endmsg;
    }
  } 

  if(!notFound){
    m_PP[PPType][idPP]->setASDOut(chPP,conPP,ASDOut);
    m_numberOfHit+=1;
    return 0;
  }
  return -1;
}
  
int TGCSector::getPatchPanelType(TGCSignalType signal, int layer) const
{
  if((layer>=0)&&(layer<=2)){ 
    if(signal==WireGroup) return WTPP;
    else return STPP;
  } else if((layer>=3)&&(layer<=6)){
    if(signal==WireGroup) return WDPP;
    else return SDPP;
  } else if (layer<=8) {
    if(signal==WireGroup) return WIPP;
    else return SIPP;
  } else {
    return NOPP;
  }
}

TGCSector::TGCSector()
 : m_regionType(FORWARD), m_numberOfHit(0), 
   m_sideId(0), m_octantId(0), m_moduleId(0), 
   m_forwardBackward(ForwardSector), 
   m_SL(0), m_TMDB(0), m_NSW(0),m_BIS78(0),
   m_tgcArgs(nullptr), m_dbMgr(nullptr)
{
  for(unsigned int iPatchPanelType=0; iPatchPanelType<NumberOfPatchPanelType; iPatchPanelType++) {
    m_ASDToPP[iPatchPanelType] = 0;
    m_PP[iPatchPanelType].clear();
  }
  for(unsigned int iSlaveBoardType=0; iSlaveBoardType<NumberOfSlaveBoardType; iSlaveBoardType++) {
    m_SB[iSlaveBoardType].clear();
  }
  for(unsigned int iHighPtBoardType=0; iHighPtBoardType<NumberOfHighPtBoardType; iHighPtBoardType++) {
    m_HPB[iHighPtBoardType].clear();
  }
}

TGCSector::TGCSector(TGCArguments* tgcargs,
                     int idIn, TGCRegionType type, 
		     TGCForwardBackwardType forwardBackward, 
		     const TGCDatabaseManager* db,
		     const TGCTMDB*            tm,
		     std::shared_ptr<const TGCNSW>  nsw,
		     std::shared_ptr<const TGCBIS78>  bis78)
 : m_id(idIn), m_regionType(type), m_numberOfHit(0),
   m_TMDB(tm), m_NSW(nsw), m_BIS78(bis78),
   m_tgcArgs(tgcargs), m_dbMgr(db)
{
  m_sideId = (idIn/NumberOfModule)/NumberOfOctant;
  m_octantId = (idIn/NumberOfModule)%NumberOfOctant;
  m_moduleId = idIn%NumberOfModule;
  m_forwardBackward = forwardBackward;

  int i;
  if (m_moduleId < 9) {
    for( i=0; i<=SDPP; i+=1) {
      m_ASDToPP[i] = db->getConnectionASDToPP((TGCRegionType)m_regionType, i, forwardBackward);
    } 
    for( i=WIPP; i<NumberOfPatchPanelType; i+=1) m_ASDToPP[i]=0;
  } else {
    // Inner Module
    for( i=0; i<=SDPP; i+=1) m_ASDToPP[i]=0;
    for( i=WIPP; i<NumberOfPatchPanelType; i+=1) {
      m_ASDToPP[i] = db->getConnectionASDToPP((TGCRegionType)m_regionType, i, forwardBackward);
    }
  }
  
  const TGCConnectionPPToSL* PPToSL = db->getConnectionPPToSL(m_regionType);
  // make PatchPanel Boards, Slave Boards, HighPt Boards, SectorLogic.
  setModule(PPToSL);
  // dumpModule();

  if (m_moduleId < 9) {
    // set TMDB
    m_SL->setTMDB(m_TMDB);
 
    // set NSW
    m_SL->setNSW(m_NSW);

    // set BIS78
    m_SL->setBIS78(m_BIS78);
  }

  // set connection between boards;
  connectPPToSB(PPToSL->getPPToSB());
  if (m_moduleId < 9) {
    connectAdjacentPP();
    connectSBToHPB(PPToSL->getSBToHPB());
    connectHPBToSL(PPToSL->getHPBToSL());
    connectAdjacentHPB();
  }
}

void TGCSector::setModule(const TGCConnectionPPToSL* connection)
{
  if (m_moduleId <9) {   // BW Sectors
    // PP ASIC
    for (int jpp=0; jpp <= SDPP; jpp++) {
      m_PP[jpp].resize(connection->getPPToSB()->getNumber(jpp));
      for (unsigned int i=0; i < m_PP[jpp].size(); i++) {
	if     (jpp==WTPP) { m_PP[jpp][i] = new TGCWireTripletPP(tgcArgs()); }
	else if(jpp==WDPP) { m_PP[jpp][i] = new TGCWireDoubletPP(tgcArgs()); }
	else if(jpp==STPP) { m_PP[jpp][i] = new TGCStripTripletPP(tgcArgs());}
	else if(jpp==SDPP) { m_PP[jpp][i] = new TGCStripDoubletPP(tgcArgs());}
	m_PP[jpp][i]->setId(connection->getPPToSB()->getId(jpp,i));
	m_PP[jpp][i]->setType(jpp);
	m_PP[jpp][i]->setRegion(m_regionType);
      }
    }
    for (int jpp=WIPP; jpp<NumberOfPatchPanelType; jpp++) {
      m_PP[jpp].clear();
    }
    
    // SLB
    for (int jsb=0; jsb <= SDSB; jsb++) {
      m_SB[jsb].resize(connection->getSBToHPB()->getNumber(jsb));
      for (unsigned int i=0; i < m_SB[jsb].size(); i++) {
	if      (jsb == WTSB) { m_SB[jsb][i] = new TGCWireTripletSB(); }
	else if (jsb == WDSB) { m_SB[jsb][i] = new TGCWireDoubletSB(); }
	else if (jsb == STSB) { m_SB[jsb][i] = new TGCStripTripletSB();}
	else if (jsb == SDSB) { m_SB[jsb][i] = new TGCStripDoubletSB();}
	m_SB[jsb][i]->setId(connection->getSBToHPB()->getId(jsb,i));
	m_SB[jsb][i]->setType(jsb);
	m_SB[jsb][i]->setRegion(m_regionType);
      }
    }
    for (int jsb=WISB; jsb < NumberOfSlaveBoardType; jsb++) {
      m_SB[jsb].clear();
    }
    
    // HPT
    for (int jhp=0; jhp<NumberOfHighPtBoardType; jhp++) {
      m_HPB[jhp].resize(connection->getHPBToSL()->getNumber(jhp));
      for (unsigned int i=0; i < m_HPB[jhp].size(); i++) {
	if(jhp == WHPB) m_HPB[jhp][i] = new TGCWireHighPtBoard;
	if(jhp == SHPB) m_HPB[jhp][i] = new TGCStripHighPtBoard;
	m_HPB[jhp][i]->setId(connection->getHPBToSL()->getId(jhp,i));
	m_HPB[jhp][i]->setType(jhp);
	m_HPB[jhp][i]->setRegion(m_regionType);
      }
    }
     
    // Sector Logic
    m_SL = new TGCSectorLogic(tgcArgs(), m_dbMgr, m_regionType, m_id);
    m_SL->getSSCController()->setNumberOfWireHighPtBoard(connection->getHPBToSL()->getNumber(WHPB));
  } else {   // Inner Small Wheel
    // PP ASIC
    for (int jpp=0; jpp <= SDPP; jpp++) {
      m_PP[jpp].clear();
    }
    for (int jpp=WIPP; jpp < NumberOfPatchPanelType; jpp++) {
      m_PP[jpp].resize(connection->getPPToSB()->getNumber(jpp));
      for (unsigned int i=0; i < m_PP[jpp].size(); i++) {
        if      (jpp == WIPP) { m_PP[jpp][i] = new TGCWireInnerPP(tgcArgs());  }
        else if (jpp == SIPP) { m_PP[jpp][i] = new TGCStripInnerPP(tgcArgs()); }
        m_PP[jpp][i]->setId(connection->getPPToSB()->getId(jpp,i));
        m_PP[jpp][i]->setType(jpp);
        m_PP[jpp][i]->setRegion(m_regionType);
      }
    }

    // SLB
    for (int jsb=0; jsb<=SDSB; jsb+=1) {
      m_SB[jsb].clear();
    }
    for (int jsb=WISB; jsb<NumberOfSlaveBoardType; jsb++) {
      m_SB[jsb].resize(1);
      m_SB[jsb][0] = new TGCInnerSB();
      m_SB[jsb][0]->setType(jsb);
      m_SB[jsb][0]->setRegion(m_regionType);
      m_SB[jsb][0]->setId(0); 
    }

    // HPT
    for(int jhp=0; jhp<NumberOfHighPtBoardType; jhp+=1){
      m_HPB[jhp].clear();
    }

    // no specific Sector Logic
    m_SL = 0;
  }
}
   
void TGCSector::connectPPToSB(const TGCConnectionPPToSB* connection)
{
  int iPort;
  int startType, endType;
  if (m_moduleId <9) {
    startType =0;
    endType   = SDPP;
  } else {
    startType = WIPP;
    endType   = SIPP;
  }

  // set a Slave Board ID to connect for each Patch Panel.
  // find the Slave Board and give a pointer of the Patch Panel to it.
  for (int i=startType; i <= endType; i++) {
#ifdef TGCDEBUG
    std::cerr << "connectionPPToSB :" 
              << "  module:" << m_moduleId 
              << "  type: " << i 
              << "  #PP=" <<  m_PP[i].size()
              << "  #SB=" <<  m_SB[i].size()
              << std::endl;
#endif
    for(unsigned int iPP = 0; iPP < m_PP[i].size(); iPP++) {
      for ( iPort = 0; iPort < connection->getNumberOfPort(); iPort += 1) {
        m_PP[i][iPP]->setIdSlaveBoard(iPort,connection->getSBIdToPP(i,iPort,iPP));
        for (unsigned int iSB = 0; iSB < m_SB[i].size(); iSB++) {
          if (m_PP[i][iPP]->getIdSlaveBoard(iPort) == m_SB[i][iSB]->getId()) {
            m_SB[i][iSB]->setPatchPanel(m_PP[i][iPP]);
            break;
          }
        } // m_SB loop
      } // Port loop
    } // m_PP loop
  } // type loop

}


void TGCSector::connectAdjacentPP()
{
  for (unsigned int iPP = 1; iPP < m_PP[WDPP].size(); iPP++) {
    m_PP[WDPP][iPP-1]->setAdjacentPP(1,m_PP[WDPP][iPP]);
    m_PP[WDPP][iPP]->setAdjacentPP(0,m_PP[WDPP][iPP-1]);
  }

  for (unsigned int iPP = 1; iPP < m_PP[WTPP].size(); iPP++) {
    m_PP[WTPP][iPP-1]->setAdjacentPP(1,m_PP[WTPP][iPP]);
    m_PP[WTPP][iPP]->setAdjacentPP(0,m_PP[WTPP][iPP-1]);
  }

  for (unsigned int iPP = 1; iPP < m_PP[STPP].size(); iPP++) {
    m_PP[STPP][iPP-1]->setAdjacentPP(1,m_PP[STPP][iPP]);
    m_PP[STPP][iPP]->setAdjacentPP(0,m_PP[STPP][iPP-1]);
  }

  for (unsigned int iPP = 1; iPP < m_PP[SDPP].size(); iPP++) {
    m_PP[SDPP][iPP-1]->setAdjacentPP(1,m_PP[SDPP][iPP]);
    m_PP[SDPP][iPP]->setAdjacentPP(0,m_PP[SDPP][iPP-1]);
  }

  for (int i=0; i<=SDPP; i++) {
    for (TGCPatchPanel* ppasic : m_PP[i]) {
      ppasic->connect();
    }
  }
}

void TGCSector::connectSBToHPB(const TGCConnectionSBToHPB* connection)
{
  for (int i=0; i < NumberOfSlaveBoardType; i++) {
    if ( i==WISB )  continue;  // Inner m_SB is not connected to m_HPB
    if ( i==SISB )  continue;  // Inner m_SB is not connected to m_HPB
 
    for (unsigned int iSB = 0; iSB < m_SB[i].size(); iSB++) {
      m_SB[i][iSB]->setIdHighPtBoard(connection->getHPBIdToSB(i,iSB));
      int type = i/2;     // ! assume HighPtBoardType=SlaveBoardType/2
      for (unsigned int iHPB = 0; iHPB < m_HPB[type].size(); iHPB += 1){
        if ( m_SB[i][iSB]->getIdHighPtBoard() == m_HPB[type].at(iHPB)->getId()) {
          if((i==WDSB)||(i==SDSB))
            m_HPB[type].at(iHPB)->setDSB(connection->getHPBPortToSB(i,iSB),m_SB[i][iSB]);
          else if((i==WTSB)||(i==STSB))
            m_HPB[type].at(iHPB)->setTSB(connection->getHPBPortToSB(i,iSB),m_SB[i][iSB]);
          break;
        }
      }
    }
  }
}

void TGCSector::connectHPBToSL(const TGCConnectionHPBToSL* connection)
{
  if (!m_SL) return;

  // set pointers of HighPtBoard to connect to SectorLogic
  for (unsigned int iHPB = 0; iHPB < m_HPB[WHPB].size(); iHPB++){
    m_SL->setWireHighPtBoard(connection->getSLPortToHPB(WHPB,iHPB), m_HPB[WHPB].at(iHPB));
  }
  for (unsigned int iHPB = 0; iHPB < m_HPB[SHPB].size(); iHPB++) {
    m_SL->setStripHighPtBoard(m_HPB[SHPB].at(iHPB));
  }
}

void TGCSector::connectAdjacentHPB()
{
  switch(m_regionType){
  case Endcap:
    // assume there are only two WireHighPtBoards.
    if((m_HPB[WHPB][1]) && (m_HPB[WHPB][0])){
      m_HPB[WHPB][0]->setAdjacentHPB(1,m_HPB[WHPB][1]); //! 
      m_HPB[WHPB][1]->setAdjacentHPB(0,m_HPB[WHPB][0]); //!
    }
    break;
  case Forward:
    if(m_HPB[WHPB][0]){
      m_HPB[WHPB][0]->setAdjacentHPB(1,0); 
    }
    break;
  default:
    break;
  }
}

TGCSector::~TGCSector()
{
  // Don't delete m_ASDToPP! 22 May 2001 (MT, KH)
  for (int i=0; i < NumberOfPatchPanelType; i++) {
    for (unsigned int j=0; j<m_PP[i].size(); j++) {
      if (m_PP[i][j] != nullptr) delete m_PP[i][j];
    }
    m_PP[i].clear();
  }

  for (int i=0; i < NumberOfSlaveBoardType; i++) {
    for (unsigned int j=0; j < m_SB[i].size(); j+=1) {
      if(m_SB[i][j] != nullptr) delete m_SB[i][j];
    }
    m_SB[i].clear();
  }

  for(int i=0; i<NumberOfHighPtBoardType; i+=1){
    for(unsigned int j=0; j < m_HPB[i].size(); j+=1) {
      if(m_HPB[i][j] != nullptr) delete m_HPB[i][j];
    }
    m_HPB[i].clear();
  }
  if (m_SL) delete m_SL;
  m_SL = 0;
}

void TGCSector::dumpModule()
{
  std::cout << "Side:" << m_sideId << "  Octant:" << m_octantId 
	    << "  ModuleId:" << m_moduleId << std::endl; 
  
  std::cout << "PatchPanel    NumberOfPatchPanelType: " << NumberOfPatchPanelType << std::endl;
  for (int j=0; j < NumberOfPatchPanelType; j++) {
    std::cout << "numberOfPP(index in a type): " << m_PP[j].size() << std::endl;
    for (TGCPatchPanel* ppasic : m_PP[j]) {
      std::cout << " Type:" << ppasic->getType();
      std::cout << " Id:" << ppasic->getId();
      std::cout << " Region:" << ppasic->getRegion();
      std::cout << ((ppasic->getRegion() == Endcap) ? ":Endcap" : ":Forward") << std::endl;
    }
  }

  std::cout << "SlaveBoard    NumberOfSlaveBoardType: " << NumberOfSlaveBoardType << std::endl;
  for (int j=0; j < NumberOfSlaveBoardType; j++) {
    std::cout << "numberOfSB(index in a type): " << m_SB[j].size() << std::endl;
    for (unsigned int i=0; i < m_SB[j].size(); i++) {  // index in a type
      std::cout << " Type:" << m_SB[j][i]->getType();
      std::cout << " Id:"   << m_SB[j][i]->getId();
      std::cout << " Region:" << m_SB[j][i]->getRegion();
      std::cout << ((m_SB[j][i]->getRegion() == Endcap) ? ":Endcap" : ":Forward") << std::endl;
    }
  }

  std::cout << "HighPtBoard    NumberOfHighPtBoardType:" << NumberOfHighPtBoardType << std::endl;
  for(int j=0; j<NumberOfHighPtBoardType; j+=1){
    std::cout << "numberOfHPB(index in a type):" << m_HPB[j].size() << std::endl;
    for(unsigned int i=0; i < m_HPB[j].size(); i+=1) {
      std::cout << " Type:" << m_HPB[j][i]->getType();
      std::cout << " Id:"   << m_HPB[j][i]->getId();
      std::cout << " Region:" << m_HPB[j][i]->getRegion();
      std::cout << ((m_HPB[j][i]->getRegion()==Endcap) ? ":Endcap" : ":Forward") << std::endl;
    }
  }

  std::cout << "SectorLogic" << std::endl;
  if (m_SL) {
    std::cout << "SL:" << m_SL << std::endl;
    std::cout << " Id:"  << m_SL->getId();
    std::cout << " Region:" << m_SL->getRegion();
    if(m_SL->getRegion()==Endcap){
      std::cout << ":Endcap" << std::endl;
    }else{
      std::cout << ":Forward" << std::endl;
    }
  } else {
    std::cout << "NO SL" << std::endl;
  } 
}  

}  // namespace LVL1TGCTrigger
