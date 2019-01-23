/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCSector.cxx,v 1.10 2009-05-14 01:28:04 isaya Exp $
#include "TrigT1TGC/TGCSector.hh"
#include "TrigT1TGC/TGCElectronicsSystem.hh"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include <iostream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

namespace LVL1TGCTrigger {

  extern bool g_DEBUGLEVEL;

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

  if (g_DEBUGLEVEL) {
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
    m_PP[PPType][idPP]->setASDOut(chPP,conPP,(TGCASDOut*)ASDOut);
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
    : m_id(0), m_regionType(FORWARD), m_numberOfHit(0), 
      m_sideId(0), m_octantId(0), m_moduleId(0), 
      m_forwardBackward(ForwardSector), 
      m_SL(0), m_TMDB(0)  
{
  for(unsigned int iPatchPanelType=0; iPatchPanelType<NumberOfPatchPanelType; iPatchPanelType++) {
    m_ASDToPP[iPatchPanelType] = 0;
    m_numberOfPP[iPatchPanelType] = 0;
    m_PP[iPatchPanelType] = 0;
  }
  for(unsigned int iSlaveBoardType=0; iSlaveBoardType<NumberOfSlaveBoardType; iSlaveBoardType++) {
    m_numberOfSB[iSlaveBoardType] = 0;
    m_SB[iSlaveBoardType] = 0;
  }
  for(unsigned int iHighPtBoardType=0; iHighPtBoardType<NumberOfHighPtBoardType; iHighPtBoardType++) {
    m_numberOfHPB[iHighPtBoardType] = 0;
    m_HPB[iHighPtBoardType] = 0;
  }
}

TGCSector::TGCSector(int idIn, TGCRegionType type, 
		     TGCForwardBackwardType forwardBackward, 
		     const TGCDatabaseManager* db,
		     const TGCTMDB*            tm)
  : m_id(idIn),m_regionType(type),m_numberOfHit(0),
    m_TMDB(tm)
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
  
  TGCConnectionPPToSL* PPToSL = db->getConnectionPPToSL(m_regionType);
  // make PatchPanel Boards, Slave Boards, HighPt Boards, SectorLogic.
  setModule(PPToSL);
  // dumpModule();

  if (m_moduleId < 9) {
    const TGCRPhiCoincidenceMap* map = db->getRPhiCoincidenceMap(m_sideId, m_octantId);

    const TGCInnerCoincidenceMap* mapI = db->getInnerCoincidenceMap(m_sideId);
    // set RPhi and Inner CoincidenceMap in SectorLogic.
    setRPhiMap(map, mapI);
    
    const TGCTileMuCoincidenceMap* mapTM = db->getTileMuCoincidenceMap();
    setTileMuMap(mapTM);
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

//  copy constructor is hided 
TGCSector::TGCSector( const TGCSector& )
     : m_id(0), m_regionType(FORWARD), m_numberOfHit(0), 
       m_sideId(0), m_octantId(0), m_moduleId(0), 
       m_forwardBackward(ForwardSector), 
       m_SL(0), m_TMDB(0)  
{
  for(unsigned int iPatchPanelType=0; iPatchPanelType<NumberOfPatchPanelType; iPatchPanelType++) {
    m_ASDToPP[iPatchPanelType] = 0;
    m_numberOfPP[iPatchPanelType] = 0;
    m_PP[iPatchPanelType] = 0;
  }
  for(unsigned int iSlaveBoardType=0; iSlaveBoardType<NumberOfSlaveBoardType; iSlaveBoardType++) {
    m_numberOfSB[iSlaveBoardType] = 0;
    m_SB[iSlaveBoardType] = 0;
  }
  for(unsigned int iHighPtBoardType=0; iHighPtBoardType<NumberOfHighPtBoardType; iHighPtBoardType++) {
    m_numberOfHPB[iHighPtBoardType] = 0;
    m_HPB[iHighPtBoardType] = 0;
  }
}

// assignment operator is hided 
TGCSector& TGCSector::operator=( const TGCSector& )
{
   return *this;
}
void TGCSector::setModule(const TGCConnectionPPToSL* connection)
{
  int jpp, jsb, jhp;
  
  if (m_moduleId <9) {
    // m_PP
    for( jpp=0; jpp<=SDPP; jpp+=1){
      m_numberOfPP[jpp] = connection->getPPToSB()->getNumber(jpp);
      m_PP[jpp] = new TGCPatchPanel* [m_numberOfPP[jpp]];
      for(int i=0; i<m_numberOfPP[jpp]; i+=1){
	if     (jpp==WTPP) { m_PP[jpp][i] = new TGCWireTripletPP; }
	else if(jpp==WDPP) { m_PP[jpp][i] = new TGCWireDoubletPP; }
	else if(jpp==STPP) { m_PP[jpp][i] = new TGCStripTripletPP;}
	else if(jpp==SDPP) { m_PP[jpp][i] = new TGCStripDoubletPP;}
	m_PP[jpp][i]->setId(connection->getPPToSB()->getId(jpp,i));
	m_PP[jpp][i]->setType(jpp);
	m_PP[jpp][i]->setRegion(m_regionType);
      }
    }
    for( jpp=WIPP; jpp<NumberOfPatchPanelType; jpp+=1){
      m_PP[jpp] = 0;
      m_numberOfPP[jpp]=0;
    }
    
    //m_SB
    for( jsb=0; jsb<=SDSB; jsb+=1){
      m_numberOfSB[jsb] = connection->getSBToHPB()->getNumber(jsb);
      m_SB[jsb] = new TGCSlaveBoard* [m_numberOfSB[jsb]];
      for(int i=0; i<m_numberOfSB[jsb]; i+=1) {
	if     (jsb==WTSB) { m_SB[jsb][i] = new TGCWireTripletSB; }
	else if(jsb==WDSB) { m_SB[jsb][i] = new TGCWireDoubletSB; }
	else if(jsb==STSB) { m_SB[jsb][i] = new TGCStripTripletSB;}
	else if(jsb==SDSB) { m_SB[jsb][i] = new TGCStripDoubletSB;}
	m_SB[jsb][i]->setId(connection->getSBToHPB()->getId(jsb,i));
	m_SB[jsb][i]->setType(jsb);
	m_SB[jsb][i]->setRegion(m_regionType);
      }
    }
    for( jsb=WISB; jsb<NumberOfSlaveBoardType; jsb+=1){
      m_SB[jsb] = 0;
      m_numberOfSB[jsb] =0;
    }
    
    //HPT
    for( jhp=0; jhp<NumberOfHighPtBoardType; jhp+=1){
      m_numberOfHPB[jhp] = connection->getHPBToSL()->getNumber(jhp);
      m_HPB[jhp] = new TGCHighPtBoard* [m_numberOfHPB[jhp]];
      for(int i=0; i<m_numberOfHPB[jhp]; i+=1) {
	if(jhp==WHPB) m_HPB[jhp][i] = new TGCWireHighPtBoard;
	if(jhp==SHPB) m_HPB[jhp][i] = new TGCStripHighPtBoard;
	m_HPB[jhp][i]->setId(connection->getHPBToSL()->getId(jhp,i));
	m_HPB[jhp][i]->setType(jhp);
	m_HPB[jhp][i]->setRegion(m_regionType);
      }
    }
     
    //m_SL
    m_SL = new TGCSectorLogic(m_regionType, m_id);
    m_SL->getSSCController()->setNumberOfWireHighPtBoard(connection->getHPBToSL()->getNumber(WHPB));
  } else {
    //Inner
    //m_PP
    for( jpp=0; jpp<=SDPP; jpp+=1){
      m_PP[jpp] = 0;
      m_numberOfPP[jpp]=0;
    }
    for( jpp=WIPP; jpp<NumberOfPatchPanelType; jpp+=1){
      m_numberOfPP[jpp] = connection->getPPToSB()->getNumber(jpp);
      m_PP[jpp] = new TGCPatchPanel* [m_numberOfPP[jpp]];
      for(int i=0; i<m_numberOfPP[jpp]; i+=1){
        if(jpp==WIPP) { m_PP[jpp][i] = new TGCWireInnerPP;   }
        else if(jpp==SIPP) { m_PP[jpp][i] = new TGCStripInnerPP;  }
        m_PP[jpp][i]->setId(connection->getPPToSB()->getId(jpp,i));
        m_PP[jpp][i]->setType(jpp);
        m_PP[jpp][i]->setRegion(m_regionType);
      }
    }

    //m_SB
    for( jsb=0; jsb<=SDSB; jsb+=1){
      m_SB[jsb] = 0;
      m_numberOfSB[jsb] = 0;
    }
    for( jsb=WISB; jsb<NumberOfSlaveBoardType; jsb+=1){
      m_numberOfSB[jsb] = 1;
      m_SB[jsb] = new TGCSlaveBoard* [m_numberOfSB[jsb]];
      m_SB[jsb][0] = new TGCInnerSB;       
      m_SB[jsb][0]->setType(jsb);
      m_SB[jsb][0]->setRegion(m_regionType);
      m_SB[jsb][0]->setId(0); 
    }

    //HPT
    for( jhp=0; jhp<NumberOfHighPtBoardType; jhp+=1){
      m_HPB[jhp] =0;
      m_numberOfHPB[jhp] =0;
    }

    // m_SL
    m_SL = 0;
  }
}
   
void TGCSector::setRPhiMap(const TGCRPhiCoincidenceMap* map,
			   const TGCInnerCoincidenceMap* mapI)
{
  if (m_SL) m_SL->setRPhiMap(map, mapI);
}

void TGCSector::setTileMuMap(const TGCTileMuCoincidenceMap* mapTM)
{
  if (m_SL) m_SL->setTileMuMap(m_TMDB, mapTM);
}


void TGCSector::connectPPToSB(const TGCConnectionPPToSB* connection)
{
  int iPP,iSB,iPort,i;
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
  for( i=startType; i<=endType; i+=1){
#ifdef TGCDEBUG
    std::cerr << "connectionPPToSB :" 
              << "  module:" << m_moduleId 
              << "  type: " << i 
              << "  #PP=" <<  m_numberOfPP[i]
              << "  #SB=" <<  m_numberOfSB[i]
              << std::endl;
#endif
    for( iPP = 0; iPP < m_numberOfPP[i]; iPP += 1) {
      for ( iPort = 0; iPort < connection->getNumberOfPort(); iPort += 1) {
        m_PP[i][iPP]->setIdSlaveBoard(iPort,connection->getSBIdToPP(i,iPort,iPP));
        for ( iSB = 0; iSB < m_numberOfSB[i]; iSB += 1) {
          if ( m_PP[i][iPP]->getIdSlaveBoard(iPort) == m_SB[i][iSB]->getId()) {
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
  int iPP;
  for( iPP = 1; iPP < m_numberOfPP[WDPP]; iPP += 1) {
    m_PP[WDPP][iPP-1]->setAdjacentPP(1,m_PP[WDPP][iPP]);
    m_PP[WDPP][iPP]->setAdjacentPP(0,m_PP[WDPP][iPP-1]);
  }

  for( iPP = 1; iPP < m_numberOfPP[WTPP]; iPP += 1) {
    m_PP[WTPP][iPP-1]->setAdjacentPP(1,m_PP[WTPP][iPP]);
    m_PP[WTPP][iPP]->setAdjacentPP(0,m_PP[WTPP][iPP-1]);
  }

  for( iPP = 1; iPP < m_numberOfPP[STPP]; iPP += 1) {
    m_PP[STPP][iPP-1]->setAdjacentPP(1,m_PP[STPP][iPP]);
    m_PP[STPP][iPP]->setAdjacentPP(0,m_PP[STPP][iPP-1]);
  }

  for( iPP = 1; iPP < m_numberOfPP[SDPP]; iPP += 1) {
    m_PP[SDPP][iPP-1]->setAdjacentPP(1,m_PP[SDPP][iPP]);
    m_PP[SDPP][iPP]->setAdjacentPP(0,m_PP[SDPP][iPP-1]);
  }

  int i;
  for( i=0; i<=SDPP; i+=1)
    for( iPP = 0; iPP < m_numberOfPP[i]; iPP += 1)
      m_PP[i][iPP]->connect();
}

void TGCSector::connectSBToHPB(const TGCConnectionSBToHPB* connection)
{
  int iSB,iHPB,type,i;

  for( i=0; i<NumberOfSlaveBoardType; i+=1){
    if ( i==WISB )  continue;  // Inner m_SB is not connected to m_HPB
    if ( i==SISB )  continue;  // Inner m_SB is not connected to m_HPB
 
    for ( iSB = 0; iSB < m_numberOfSB[i]; iSB += 1) {
      m_SB[i][iSB]->setIdHighPtBoard(connection->getHPBIdToSB(i,iSB));
      type = i/2;     // ! assume HighPtBoardType=SlaveBoardType/2
      for ( iHPB = 0; iHPB < m_numberOfHPB[type]; iHPB += 1){
        if ( m_SB[i][iSB]->getIdHighPtBoard() == m_HPB[type][iHPB]->getId()) {
          if((i==WDSB)||(i==SDSB))
            m_HPB[type][iHPB]->setDSB(connection->getHPBPortToSB(i,iSB),m_SB[i][iSB]);
          else if((i==WTSB)||(i==STSB))
            m_HPB[type][iHPB]->setTSB(connection->getHPBPortToSB(i,iSB),m_SB[i][iSB]);
          break;
        }
      }
    }
  }
}

void TGCSector::connectHPBToSL(const TGCConnectionHPBToSL* connection)
{
  int iHPB;
  if (!m_SL) return;

  // set pointers of HighPtBoard to connect to SectorLogic
  for ( iHPB = 0; iHPB < m_numberOfHPB[WHPB]; iHPB += 1){
    m_SL->setWireHighPtBoard(connection->getSLPortToHPB(WHPB,iHPB) ,m_HPB[WHPB][iHPB]);
  }
  for ( iHPB = 0; iHPB < m_numberOfHPB[SHPB]; iHPB += 1)   
    m_SL->setStripHighPtBoard(m_HPB[SHPB][iHPB]);
}

void TGCSector::connectAdjacentHPB()
{
  switch(m_regionType){
  case Endcap:
    // assume there are only two WireHighPtBoards.
    if((m_HPB[WHPB][1])&&(m_HPB[WHPB][0])){
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

  int i,j;

  for( i=0; i<NumberOfPatchPanelType; i+=1){
    for( j=0; j<m_numberOfPP[i]; j+=1)
      if(m_PP[i][j]!=0) delete m_PP[i][j];
    if(m_PP[i]!=0) delete [] m_PP[i];
    m_PP[i]=0;
  }

  for( i=0; i<NumberOfSlaveBoardType; i+=1){
    for( j=0; j<m_numberOfSB[i]; j+=1)
      if(m_SB[i][j]!=0) delete m_SB[i][j];
    if(m_SB[i]!=0) delete [] m_SB[i];
    m_SB[i]=0;
  }

  for( i=0; i<NumberOfHighPtBoardType; i+=1){
    for( j=0; j<m_numberOfHPB[i]; j+=1) 
      if(m_HPB[i][j]!=0) delete m_HPB[i][j];
    if(m_HPB[i]!=0) delete [] m_HPB[i];
    m_HPB[i]=0;
  }
  if (m_SL) delete m_SL;
  m_SL =0;
}

void TGCSector::dumpModule()
{
  std::cout << "Side:" << m_sideId << "  Octant:" << m_octantId 
	    << "  ModuleId:" << m_moduleId << std::endl; 
  
  int i,j;
  std::cout << "PatchPanel    NumberOfPatchPanelType:" << NumberOfPatchPanelType << std::endl;
  for( j=0; j<NumberOfPatchPanelType; j+=1){
    std::cout << "numberOfPP(index in a type):" << m_numberOfPP[j] << std::endl;
    for( i=0; i<m_numberOfPP[j]; i+=1){
      std::cout << "PP[" << j << "][" << i << "]:" << m_PP[j][i];
      std::cout << " Type:" << m_PP[j][i]->getType();
      std::cout << " Id:" << m_PP[j][i]->getId();
      std::cout << " Region:" << m_PP[j][i]->getRegion();
      if(m_PP[j][i]->getRegion()==Endcap){std::cout << ":Endcap" << std::endl;}else{std::cout << ":Forward" << std::endl;}
    }
  }

  std::cout << "SlaveBoard    NumberOfSlaveBoardType:" << NumberOfSlaveBoardType << std::endl;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    std::cout << "numberOfSB(index in a type):" << m_numberOfSB[j] << std::endl;
    for( i=0; i<m_numberOfSB[j]; i+=1) { // index in a type
      std::cout << "SB[" << j << "][" << i << "]:" << m_SB[j][i];
      std::cout << " Type:" << m_SB[j][i]->getType();
      std::cout << " Id:"   << m_SB[j][i]->getId();
      std::cout << " Region:" << m_SB[j][i]->getRegion();
      if(m_SB[j][i]->getRegion()==Endcap){std::cout << ":Endcap" << std::endl;}else{std::cout << ":Forward" << std::endl;}
    }
  }

  std::cout << "HighPtBoard    NumberOfHighPtBoardType:" << NumberOfHighPtBoardType << std::endl;
  for( j=0; j<NumberOfHighPtBoardType; j+=1){
    std::cout << "numberOfHPB(index in a type):" << m_numberOfHPB[j] << std::endl;
    for( i=0; i<m_numberOfHPB[j]; i+=1) {
      std::cout << "HPB[" << j << "][" << i << "]:" << m_HPB[j][i];
      std::cout << " Type:" << m_HPB[j][i]->getType();
      std::cout << " Id:"   << m_HPB[j][i]->getId();
      std::cout << " Region:" << m_HPB[j][i]->getRegion();
      if(m_HPB[j][i]->getRegion()==Endcap){std::cout << ":Endcap" << std::endl;}else{std::cout << ":Forward" << std::endl;}
    }
  }

  std::cout << "SectorLogic" << std::endl;
  if (m_SL) {
    std::cout << "SL:" << m_SL << std::endl;
    std::cout << " Id:"   << i << " " << m_SL->getId();
    std::cout << " Region:" << m_SL->getRegion();
    if(m_SL->getRegion()==Endcap){
      std::cout << ":Endcap" << std::endl;
    }else{
      std::cout << ":Forward" << std::endl;
    }
    //  m_SL->getSSCController()->setNumberOfWireHighPtBoard(connection->getHPBToSL()->getNumber(WHPB));
  } else {
    std::cout << "NO SL" << std::endl;
  } 
}  

} //end of namespace bracket
