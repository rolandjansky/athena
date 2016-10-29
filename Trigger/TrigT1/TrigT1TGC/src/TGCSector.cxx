/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    notFound = ASDToPP[PPType]->getConnection(sideId,layer,rNumber,ch,&idPP,&conPP,&chPP);
  }

  if (g_DEBUGLEVEL) {
    IMessageSvc* msgSvc = 0;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
      return -1;
    };
    MsgStream m_log(msgSvc, "LVL1TGCTrigger::TGCSector");
    if (!notFound) {
      m_log << MSG::DEBUG
	    << "signalType= " << ((signalType==WireGroup) ? "Wire" : "Strip")
	    <<" layer= " <<layer <<" rNumber= " <<rNumber <<" ch= "<< ch
	    <<"id(PP)= " <<idPP <<" connector(PP)= " <<conPP <<" ch(PP)= " <<chPP
           <<endmsg;
    }
  } 

  if(!notFound){
    PP[PPType][idPP]->setASDOut(chPP,conPP,(TGCASDOut*)ASDOut);
    numberOfHit+=1;
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
    : id(0), regionType(FORWARD), numberOfHit(0), 
      sideId(0), octantId(0), moduleId(0), 
      forwardBackward(ForwardSector), 
      SL(0), TMDB(0)  
{
  for(unsigned int iPatchPanelType=0; iPatchPanelType<NumberOfPatchPanelType; iPatchPanelType++) {
    ASDToPP[iPatchPanelType] = 0;
    numberOfPP[iPatchPanelType] = 0;
    PP[iPatchPanelType] = 0;
  }
  for(unsigned int iSlaveBoardType=0; iSlaveBoardType<NumberOfSlaveBoardType; iSlaveBoardType++) {
    numberOfSB[iSlaveBoardType] = 0;
    SB[iSlaveBoardType] = 0;
  }
  for(unsigned int iHighPtBoardType=0; iHighPtBoardType<NumberOfHighPtBoardType; iHighPtBoardType++) {
    numberOfHPB[iHighPtBoardType] = 0;
    HPB[iHighPtBoardType] = 0;
  }
}

TGCSector::TGCSector(int idIn, TGCRegionType type, 
		     TGCForwardBackwardType forwardBackward, 
		     const TGCDatabaseManager* db,
		     const TGCTMDB*            tm)
  : id(idIn),regionType(type),numberOfHit(0),
    TMDB(tm)
{
  sideId = (idIn/NumberOfModule)/NumberOfOctant;
  octantId = (idIn/NumberOfModule)%NumberOfOctant;
  moduleId = idIn%NumberOfModule;
  this->forwardBackward = forwardBackward;

  int i;
  if (moduleId < 9) {
    for( i=0; i<=SDPP; i+=1) {
      ASDToPP[i] = db->getConnectionASDToPP((TGCRegionType)regionType, i, forwardBackward);
    } 
    for( i=WIPP; i<NumberOfPatchPanelType; i+=1) ASDToPP[i]=0;
  } else {
    // Inner Module
    for( i=0; i<=SDPP; i+=1) ASDToPP[i]=0;
    for( i=WIPP; i<NumberOfPatchPanelType; i+=1) {
      ASDToPP[i] = db->getConnectionASDToPP((TGCRegionType)regionType, i, forwardBackward);
    }
  }
  
  TGCConnectionPPToSL* PPToSL = db->getConnectionPPToSL(regionType);
  // make PatchPanel Boards, Slave Boards, HighPt Boards, SectorLogic.
  setModule(PPToSL);
  // dumpModule();

  if (moduleId < 9) {
    const TGCRPhiCoincidenceMap* map = db->getRPhiCoincidenceMap(sideId, octantId);

    const TGCInnerCoincidenceMap* mapI = db->getInnerCoincidenceMap(sideId);
    // set RPhi and Inner CoincidenceMap in SectorLogic.
    setRPhiMap(map, mapI);
    
    const TGCTileMuCoincidenceMap* mapTM = db->getTileMuCoincidenceMap();
    setTileMuMap(mapTM);
  }

  // set connection between boards;
  connectPPToSB(PPToSL->getPPToSB());
  if (moduleId < 9) {
    connectAdjacentPP();
    connectSBToHPB(PPToSL->getSBToHPB());
    connectHPBToSL(PPToSL->getHPBToSL());
    connectAdjacentHPB();
  }
}

//  copy constructor is hided 
TGCSector::TGCSector( const TGCSector& )
     : id(0), regionType(FORWARD), numberOfHit(0), 
       sideId(0), octantId(0), moduleId(0), 
       forwardBackward(ForwardSector), 
       SL(0), TMDB(0)  
{
  for(unsigned int iPatchPanelType=0; iPatchPanelType<NumberOfPatchPanelType; iPatchPanelType++) {
    ASDToPP[iPatchPanelType] = 0;
    numberOfPP[iPatchPanelType] = 0;
    PP[iPatchPanelType] = 0;
  }
  for(unsigned int iSlaveBoardType=0; iSlaveBoardType<NumberOfSlaveBoardType; iSlaveBoardType++) {
    numberOfSB[iSlaveBoardType] = 0;
    SB[iSlaveBoardType] = 0;
  }
  for(unsigned int iHighPtBoardType=0; iHighPtBoardType<NumberOfHighPtBoardType; iHighPtBoardType++) {
    numberOfHPB[iHighPtBoardType] = 0;
    HPB[iHighPtBoardType] = 0;
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
  
  if (moduleId <9) {
    // PP
    for( jpp=0; jpp<=SDPP; jpp+=1){
      numberOfPP[jpp] = connection->getPPToSB()->getNumber(jpp);
      PP[jpp] = new TGCPatchPanel* [numberOfPP[jpp]];
      for(int i=0; i<numberOfPP[jpp]; i+=1){
	if     (jpp==WTPP) { PP[jpp][i] = new TGCWireTripletPP; }
	else if(jpp==WDPP) { PP[jpp][i] = new TGCWireDoubletPP; }
	else if(jpp==STPP) { PP[jpp][i] = new TGCStripTripletPP;}
	else if(jpp==SDPP) { PP[jpp][i] = new TGCStripDoubletPP;}
	PP[jpp][i]->setId(connection->getPPToSB()->getId(jpp,i));
	PP[jpp][i]->setType(jpp);
	PP[jpp][i]->setRegion(regionType);
      }
    }
    for( jpp=WIPP; jpp<NumberOfPatchPanelType; jpp+=1){
      PP[jpp] = 0;
      numberOfPP[jpp]=0;
    }
    
    //SB
    for( jsb=0; jsb<=SDSB; jsb+=1){
      numberOfSB[jsb] = connection->getSBToHPB()->getNumber(jsb);
      SB[jsb] = new TGCSlaveBoard* [numberOfSB[jsb]];
      for(int i=0; i<numberOfSB[jsb]; i+=1) {
	if     (jsb==WTSB) { SB[jsb][i] = new TGCWireTripletSB; }
	else if(jsb==WDSB) { SB[jsb][i] = new TGCWireDoubletSB; }
	else if(jsb==STSB) { SB[jsb][i] = new TGCStripTripletSB;}
	else if(jsb==SDSB) { SB[jsb][i] = new TGCStripDoubletSB;}
	SB[jsb][i]->setId(connection->getSBToHPB()->getId(jsb,i));
	SB[jsb][i]->setType(jsb);
	SB[jsb][i]->setRegion(regionType);
      }
    }
    for( jsb=WISB; jsb<NumberOfSlaveBoardType; jsb+=1){
      SB[jsb] = 0;
      numberOfSB[jsb] =0;
    }
    
    //HPT
    for( jhp=0; jhp<NumberOfHighPtBoardType; jhp+=1){
      numberOfHPB[jhp] = connection->getHPBToSL()->getNumber(jhp);
      HPB[jhp] = new TGCHighPtBoard* [numberOfHPB[jhp]];
      for(int i=0; i<numberOfHPB[jhp]; i+=1) {
	if(jhp==WHPB) HPB[jhp][i] = new TGCWireHighPtBoard;
	if(jhp==SHPB) HPB[jhp][i] = new TGCStripHighPtBoard;
	HPB[jhp][i]->setId(connection->getHPBToSL()->getId(jhp,i));
	HPB[jhp][i]->setType(jhp);
	HPB[jhp][i]->setRegion(regionType);
      }
    }
     
    //SL
    SL = new TGCSectorLogic(regionType, id);
    SL->getSSCController()->setNumberOfWireHighPtBoard(connection->getHPBToSL()->getNumber(WHPB));
  } else {
    //Inner
    //PP
    for( jpp=0; jpp<=SDPP; jpp+=1){
      PP[jpp] = 0;
      numberOfPP[jpp]=0;
    }
    for( jpp=WIPP; jpp<NumberOfPatchPanelType; jpp+=1){
      numberOfPP[jpp] = connection->getPPToSB()->getNumber(jpp);
      PP[jpp] = new TGCPatchPanel* [numberOfPP[jpp]];
      for(int i=0; i<numberOfPP[jpp]; i+=1){
        if(jpp==WIPP) { PP[jpp][i] = new TGCWireInnerPP;   }
        else if(jpp==SIPP) { PP[jpp][i] = new TGCStripInnerPP;  }
        PP[jpp][i]->setId(connection->getPPToSB()->getId(jpp,i));
        PP[jpp][i]->setType(jpp);
        PP[jpp][i]->setRegion(regionType);
      }
    }

    //SB
    for( jsb=0; jsb<=SDSB; jsb+=1){
      SB[jsb] = 0;
      numberOfSB[jsb] = 0;
    }
    for( jsb=WISB; jsb<NumberOfSlaveBoardType; jsb+=1){
      numberOfSB[jsb] = 1;
      SB[jsb] = new TGCSlaveBoard* [numberOfSB[jsb]];
      SB[jsb][0] = new TGCInnerSB;       
      SB[jsb][0]->setType(jsb);
      SB[jsb][0]->setRegion(regionType);
      SB[jsb][0]->setId(0); 
    }

    //HPT
    for( jhp=0; jhp<NumberOfHighPtBoardType; jhp+=1){
      HPB[jhp] =0;
      numberOfHPB[jhp] =0;
    }

    // SL
    SL = 0;
  }
}
   
void TGCSector::setRPhiMap(const TGCRPhiCoincidenceMap* map,
			   const TGCInnerCoincidenceMap* mapI)
{
  if (SL) SL->setRPhiMap(map, mapI);
}

void TGCSector::setTileMuMap(const TGCTileMuCoincidenceMap* mapTM)
{
  if (SL) SL->setTileMuMap(TMDB, mapTM);
}


void TGCSector::connectPPToSB(const TGCConnectionPPToSB* connection)
{
  int iPP,iSB,iPort,i;
  int startType, endType;
  if (moduleId <9) {
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
              << "  module:" << moduleId 
              << "  type: " << i 
              << "  #PP=" <<  numberOfPP[i]
              << "  #SB=" <<  numberOfSB[i]
              << std::endl;
#endif
    for( iPP = 0; iPP < numberOfPP[i]; iPP += 1) {
      for ( iPort = 0; iPort < connection->getNumberOfPort(); iPort += 1) {
        PP[i][iPP]->setIdSlaveBoard(iPort,connection->getSBIdToPP(i,iPort,iPP));
        for ( iSB = 0; iSB < numberOfSB[i]; iSB += 1) {
          if ( PP[i][iPP]->getIdSlaveBoard(iPort) == SB[i][iSB]->getId()) {
            SB[i][iSB]->setPatchPanel(PP[i][iPP]);
            break;
          }
        } // SB loop
      } // Port loop
    } // PP loop
  } // type loop

}


void TGCSector::connectAdjacentPP()
{
  int iPP;
  for( iPP = 1; iPP < numberOfPP[WDPP]; iPP += 1) {
    PP[WDPP][iPP-1]->setAdjacentPP(1,PP[WDPP][iPP]);
    PP[WDPP][iPP]->setAdjacentPP(0,PP[WDPP][iPP-1]);
  }

  for( iPP = 1; iPP < numberOfPP[WTPP]; iPP += 1) {
    PP[WTPP][iPP-1]->setAdjacentPP(1,PP[WTPP][iPP]);
    PP[WTPP][iPP]->setAdjacentPP(0,PP[WTPP][iPP-1]);
  }

  for( iPP = 1; iPP < numberOfPP[STPP]; iPP += 1) {
    PP[STPP][iPP-1]->setAdjacentPP(1,PP[STPP][iPP]);
    PP[STPP][iPP]->setAdjacentPP(0,PP[STPP][iPP-1]);
  }

  for( iPP = 1; iPP < numberOfPP[SDPP]; iPP += 1) {
    PP[SDPP][iPP-1]->setAdjacentPP(1,PP[SDPP][iPP]);
    PP[SDPP][iPP]->setAdjacentPP(0,PP[SDPP][iPP-1]);
  }

  int i;
  for( i=0; i<=SDPP; i+=1)
    for( iPP = 0; iPP < numberOfPP[i]; iPP += 1)
      PP[i][iPP]->connect();
}

void TGCSector::connectSBToHPB(const TGCConnectionSBToHPB* connection)
{
  int iSB,iHPB,type,i;

  for( i=0; i<NumberOfSlaveBoardType; i+=1){
    if ( i==WISB )  continue;  // Inner SB is not connected to HPB
    if ( i==SISB )  continue;  // Inner SB is not connected to HPB
 
    for ( iSB = 0; iSB < numberOfSB[i]; iSB += 1) {
      SB[i][iSB]->setIdHighPtBoard(connection->getHPBIdToSB(i,iSB));
      type = i/2;     // ! assume HighPtBoardType=SlaveBoardType/2
      for ( iHPB = 0; iHPB < numberOfHPB[type]; iHPB += 1){
        if ( SB[i][iSB]->getIdHighPtBoard() == HPB[type][iHPB]->getId()) {
          if((i==WDSB)||(i==SDSB))
            HPB[type][iHPB]->setDSB(connection->getHPBPortToSB(i,iSB),SB[i][iSB]);
          else if((i==WTSB)||(i==STSB))
            HPB[type][iHPB]->setTSB(connection->getHPBPortToSB(i,iSB),SB[i][iSB]);
          break;
        }
      }
    }
  }
}

void TGCSector::connectHPBToSL(const TGCConnectionHPBToSL* connection)
{
  int iHPB;
  if (!SL) return;

  // set pointers of HighPtBoard to connect to SectorLogic
  for ( iHPB = 0; iHPB < numberOfHPB[WHPB]; iHPB += 1){
    SL->setWireHighPtBoard(connection->getSLPortToHPB(WHPB,iHPB) ,HPB[WHPB][iHPB]);
  }
  for ( iHPB = 0; iHPB < numberOfHPB[SHPB]; iHPB += 1)   
    SL->setStripHighPtBoard(HPB[SHPB][iHPB]);
}

void TGCSector::connectAdjacentHPB()
{
  switch(regionType){
  case Endcap:
    // assume there are only two WireHighPtBoards.
    if((HPB[WHPB][1])&&(HPB[WHPB][0])){
      HPB[WHPB][0]->setAdjacentHPB(1,HPB[WHPB][1]); //! 
      HPB[WHPB][1]->setAdjacentHPB(0,HPB[WHPB][0]); //!
    }
    break;
  case Forward:
    if(HPB[WHPB][0]){
      HPB[WHPB][0]->setAdjacentHPB(1,0); 
    }
    break;
  default:
    break;
  }
}

TGCSector::~TGCSector()
{
  // Don't delete ASDToPP! 22 May 2001 (MT, KH)

  int i,j;

  for( i=0; i<NumberOfPatchPanelType; i+=1){
    for( j=0; j<numberOfPP[i]; j+=1)
      if(PP[i][j]!=0) delete PP[i][j];
    if(PP[i]!=0) delete [] PP[i];
    PP[i]=0;
  }

  for( i=0; i<NumberOfSlaveBoardType; i+=1){
    for( j=0; j<numberOfSB[i]; j+=1)
      if(SB[i][j]!=0) delete SB[i][j];
    if(SB[i]!=0) delete [] SB[i];
    SB[i]=0;
  }

  for( i=0; i<NumberOfHighPtBoardType; i+=1){
    for( j=0; j<numberOfHPB[i]; j+=1) 
      if(HPB[i][j]!=0) delete HPB[i][j];
    if(HPB[i]!=0) delete [] HPB[i];
    HPB[i]=0;
  }
  if (SL) delete SL;
  SL =0;
}

void TGCSector::dumpModule()
{
  std::cout << "Side:" << sideId << "  Octant:" << octantId 
	    << "  ModuleId:" << moduleId << std::endl; 
  
  int i,j;
  std::cout << "PatchPanel    NumberOfPatchPanelType:" << NumberOfPatchPanelType << std::endl;
  for( j=0; j<NumberOfPatchPanelType; j+=1){
    std::cout << "numberOfPP(index in a type):" << numberOfPP[j] << std::endl;
    for( i=0; i<numberOfPP[j]; i+=1){
      std::cout << "PP[" << j << "][" << i << "]:" << PP[j][i];
      std::cout << " Type:" << PP[j][i]->getType();
      std::cout << " Id:" << PP[j][i]->getId();
      std::cout << " Region:" << PP[j][i]->getRegion();
      if(PP[j][i]->getRegion()==Endcap){std::cout << ":Endcap" << std::endl;}else{std::cout << ":Forward" << std::endl;}
    }
  }

  std::cout << "SlaveBoard    NumberOfSlaveBoardType:" << NumberOfSlaveBoardType << std::endl;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    std::cout << "numberOfSB(index in a type):" << numberOfSB[j] << std::endl;
    for( i=0; i<numberOfSB[j]; i+=1) { // index in a type
      std::cout << "SB[" << j << "][" << i << "]:" << SB[j][i];
      std::cout << " Type:" << SB[j][i]->getType();
      std::cout << " Id:"   << SB[j][i]->getId();
      std::cout << " Region:" << SB[j][i]->getRegion();
      if(SB[j][i]->getRegion()==Endcap){std::cout << ":Endcap" << std::endl;}else{std::cout << ":Forward" << std::endl;}
    }
  }

  std::cout << "HighPtBoard    NumberOfHighPtBoardType:" << NumberOfHighPtBoardType << std::endl;
  for( j=0; j<NumberOfHighPtBoardType; j+=1){
    std::cout << "numberOfHPB(index in a type):" << numberOfHPB[j] << std::endl;
    for( i=0; i<numberOfHPB[j]; i+=1) {
      std::cout << "HPB[" << j << "][" << i << "]:" << HPB[j][i];
      std::cout << " Type:" << HPB[j][i]->getType();
      std::cout << " Id:"   << HPB[j][i]->getId();
      std::cout << " Region:" << HPB[j][i]->getRegion();
      if(HPB[j][i]->getRegion()==Endcap){std::cout << ":Endcap" << std::endl;}else{std::cout << ":Forward" << std::endl;}
    }
  }

  std::cout << "SectorLogic" << std::endl;
  if (SL) {
    std::cout << "SL:" << SL << std::endl;
    std::cout << " Id:"   << i << " " << SL->getId();
    std::cout << " Region:" << SL->getRegion();
    if(SL->getRegion()==Endcap){
      std::cout << ":Endcap" << std::endl;
    }else{
      std::cout << ":Forward" << std::endl;
    }
    //  SL->getSSCController()->setNumberOfWireHighPtBoard(connection->getHPBToSL()->getNumber(WHPB));
  } else {
    std::cout << "NO SL" << std::endl;
  } 
}  

} //end of namespace bracket
