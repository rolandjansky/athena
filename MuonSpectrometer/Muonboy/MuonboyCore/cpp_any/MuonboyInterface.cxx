/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyInterface.h"
   
#include "MuonboyCore/AandBlinesLoader.h"
#include "MuonboyCore/MuonboyCocoupMaker.h"
#include "MuonboyCore/MboyMuonBackTracker.h"
#include "MuonboyCore/MuonboyDoDRAT.h"
#include "MuonboyCore/MuonboyDoProtoTrack.h"
#include "MuonboyCore/MuonboyRecMaker.h"
#include "MuonboyCore/MuonboyCollectMatter.h"
#include "MuonboyCore/MuonboyConfigure.h"
#include "MuonboyCore/AgddXml.h"
#include "MuonboyCore/MuonboyExtROALoader.h"

#include "MuonboyCore/MuonboyMaterialTagInfo.h"
#include "MuonboyCore/MuonboyFieldStatus.h"
#include "MuonboyCore/MuonboyMaterialAGDD2GeoSwitches.h"
#include "MuonboyCore/MuonboyMultiLayerOut.h"

#include "MboyCoreEvt/MboyIlineStore.h"

#include "MuonboyCore/MuonboySetBFRT.h"

#include "MboyCoreEvt/MbDRAT.h"
#include "MboyCoreEvt/MbProtoTrack.h"

MuonboyInterface::MuonboyInterface(){
 p_AandBlinesLoader     = 0 ;
 p_MuonboyCocoupMaker   = 0 ;
 p_MboyMuonBackTracker  = 0 ;
 p_MuonboyDoDRAT        = 0 ;
 p_MuonboyDoProtoTrack  = 0 ;
 p_MuonboyRecMaker      = 0 ;
 p_MuonboyCollectMatter = 0 ;
 p_MuonboyConfigure = new MuonboyConfigure;
 p_AgddXml = new AgddXml;
 p_MuonboyExtROALoader = new MuonboyExtROALoader;
 p_MuonboySetBFRT = new MuonboySetBFRT;
}
MuonboyInterface::~MuonboyInterface(){

 if (p_AandBlinesLoader) p_AandBlinesLoader->finalize();
 delete p_AandBlinesLoader;
 if (p_MuonboyCocoupMaker) p_MuonboyCocoupMaker->finalize();
 delete p_MuonboyCocoupMaker;
 if (p_MboyMuonBackTracker) p_MboyMuonBackTracker->finalize();
 delete p_MboyMuonBackTracker;
 if (p_MuonboyDoDRAT) p_MuonboyDoDRAT->finalize();
 delete p_MuonboyDoDRAT;
 if (p_MuonboyDoProtoTrack) p_MuonboyDoProtoTrack->finalize();
 delete p_MuonboyDoProtoTrack;
 if (p_MuonboyRecMaker) p_MuonboyRecMaker->finalize();
 delete p_MuonboyRecMaker;
 if (p_MuonboyCollectMatter) p_MuonboyCollectMatter->finalize();
 delete p_MuonboyCollectMatter;
 delete p_MuonboyConfigure;
 delete p_AgddXml;
 delete p_MuonboyExtROALoader;
 delete p_MuonboySetBFRT;

}

void MuonboyInterface::initialize(     
                   std::string AmdcString,
                   std::string AgddString,
                   MuonboyBFAbstract* pMuonboyBFAbstract
){

  p_AandBlinesLoader = new AandBlinesLoader() ;
  p_AandBlinesLoader->initialize();

  p_MuonboyCocoupMaker = new MuonboyCocoupMaker() ;
  p_MuonboyCocoupMaker->initialize();

  p_MboyMuonBackTracker = new MboyMuonBackTracker() ;
  p_MboyMuonBackTracker->initialize();

  p_MuonboyDoDRAT = new MuonboyDoDRAT() ;
  p_MuonboyDoDRAT->initialize();

  p_MuonboyDoProtoTrack = new MuonboyDoProtoTrack() ;
  p_MuonboyDoProtoTrack->initialize();

  p_MuonboyRecMaker = new MuonboyRecMaker;
  p_MuonboyRecMaker->initialize(AmdcString,AgddString,pMuonboyBFAbstract);

  p_MuonboyCollectMatter = new MuonboyCollectMatter;
  p_MuonboyCollectMatter->initialize();

}

void MuonboyInterface::initialize(     
                   MuonboyBFAbstract* pMuonboyBFAbstract
){

  p_AandBlinesLoader = new AandBlinesLoader() ;
  p_AandBlinesLoader->initialize();

  p_MuonboyCocoupMaker = new MuonboyCocoupMaker() ;
  p_MuonboyCocoupMaker->initialize();

  p_MboyMuonBackTracker = new MboyMuonBackTracker() ;
  p_MboyMuonBackTracker->initialize();

  p_MuonboyDoDRAT = new MuonboyDoDRAT() ;
  p_MuonboyDoDRAT->initialize();

  p_MuonboyDoProtoTrack = new MuonboyDoProtoTrack() ;
  p_MuonboyDoProtoTrack->initialize();

  p_MuonboyRecMaker = new MuonboyRecMaker;
  p_MuonboyRecMaker->initialize(pMuonboyBFAbstract);

  p_MuonboyCollectMatter = new MuonboyCollectMatter;
  p_MuonboyCollectMatter->initialize();

}

//---------------------------------------------------------------------------------------------------------------//
MB_Track* MuonboyInterface::MboyMuonBackTrack(MboyVarByName* pMboyVarByName,MB_Track* pMB_Track )
{
  SetConfiguration(pMboyVarByName);

  MB_Track* pMB_Track_New = 0 ;
  
  p_MboyMuonBackTracker->execute(pMB_Track,pMB_Track_New) ;

  p_MuonboyConfigure->Reset();

  return pMB_Track_New;

}


//---------------------------------------------------------------------------------------------------------------//
int MuonboyInterface::DoRec(
                     MboyVarByName*           pMboyVarByName,
                     MuonboyBFAbstract*       pMuonboyBFAbstract,
                     MuonboyRTAbstract*       pMuonboyRTAbstract,
                     const MboyPosDigiChoice* pMboyPosDigiChoice ,
                     MuonRecDigiStore*        pMuonRecDigiStore,
                     const MboyGenTrackStore* pMboyGenTrackStore,
                     MB_TrackStore*	      pMB_TrackStore,
                     MB_SegmentStore*	      pMB_SegmentStore,
                     MboyROAStore*	      pMboyROAExternalStore 
){

  int ToBeReturned = 0 ;

  SetConfiguration(pMboyVarByName);

  p_MuonboyExtROALoader->execute(pMboyROAExternalStore);
  
  p_MuonboySetBFRT->Set( 
                        pMuonboyBFAbstract,
                        pMuonboyRTAbstract
                       );
  
  p_MuonboyCocoupMaker->execute(
                                pMboyPosDigiChoice,
                                pMuonRecDigiStore,
                                pMboyGenTrackStore
                               );


  p_MuonboyRecMaker->execute(
  			     pMuonboyBFAbstract,
  			     pMuonboyRTAbstract,
  			     pMuonRecDigiStore,
  			     pMB_TrackStore,
  			     pMB_SegmentStore
  			    );

  p_MuonboyConfigure->Reset();

  p_MuonboyExtROALoader->Reset();

  return ToBeReturned;

}


int MuonboyInterface::DoRec(
                     MboyVarByName*           pMboyVarByName,
                     MboyAlineStore*          pMboyAlineStore,
                     MboyBlineStore*          pMboyBlineStore,
                     MuonboyBFAbstract*       pMuonboyBFAbstract,
                     MuonboyRTAbstract*       pMuonboyRTAbstract,
                     const MboyPosDigiChoice* pMboyPosDigiChoice ,
                     MuonRecDigiStore*        pMuonRecDigiStore,
                     const MboyGenTrackStore* pMboyGenTrackStore,
                     MB_TrackStore*	      pMB_TrackStore,
                     MB_SegmentStore*	      pMB_SegmentStore,
                     MboyROAStore*	      pMboyROAExternalStore 
){

  int ToBeReturned = 0 ;

  SetConfiguration(pMboyVarByName);

  MboyIlineStore aMboyIlineStore ; aMboyIlineStore.Reset(); aMboyIlineStore.SetActive(1);
  
  int StatusSetAliLine = SetAliLine(pMboyAlineStore, pMboyBlineStore, &aMboyIlineStore);
  if ( StatusSetAliLine !=0 ){
    std::cout << "SetAliLine failed " << std::endl;
    return StatusSetAliLine;
  }
							  

  p_MuonboyExtROALoader->execute(pMboyROAExternalStore);
  
  p_MuonboySetBFRT->Set( 
                        pMuonboyBFAbstract,
                        pMuonboyRTAbstract
                       );
  
  p_MuonboyCocoupMaker->execute(
                                pMboyPosDigiChoice,
                                pMuonRecDigiStore,
                                pMboyGenTrackStore
                               );


  p_MuonboyRecMaker->execute(
  			     pMuonboyBFAbstract,
  			     pMuonboyRTAbstract,
  			     pMuonRecDigiStore,
  			     pMB_TrackStore,
  			     pMB_SegmentStore
  			    );

  p_MuonboyConfigure->Reset();

  p_MuonboyExtROALoader->Reset();
 
  return ToBeReturned;

}

//---------------------------------------------------------------------------------------------------------------//
// find track in the muon system 
MbProtoTrack* MuonboyInterface::find(
        MboyVarByName*           pMboyVarByName,
        MbProtoTrack*            pMbProtoTrack ,
        MbProtoTrack*            pTargetSegmentsProtoTrack 
){

  SetConfiguration(pMboyVarByName);

  MbProtoTrack* New_pMbProtoTrack = p_MuonboyDoProtoTrack->find(
                                 pMbProtoTrack ,
                                 pTargetSegmentsProtoTrack 
                          );

  p_MuonboyConfigure->Reset();

  return New_pMbProtoTrack;

}
MbProtoTrack* MuonboyInterface::refine(
        MboyVarByName*           pMboyVarByName,
        MbProtoTrack*            pMbProtoTrack,
        const MuonRecDigiStore*  pMuonRecDigiStore,
        MB_TrackStore*           pMB_TrackStore
){

  SetConfiguration(pMboyVarByName);

  MbProtoTrack* New_pMbProtoTrack = p_MuonboyDoProtoTrack->refine(
                                 pMbProtoTrack,
                                 pMuonRecDigiStore,
                                 pMB_TrackStore
                          );

  p_MuonboyConfigure->Reset();

  return New_pMbProtoTrack;


}
int MuonboyInterface::SetAMbDRAT(
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
){
  int ToBeReturned = 0 ;

  SetConfiguration(pMboyVarByName);

  p_MuonboyDoDRAT->execute(
                           pMbDRAT
                          );


  p_MuonboyConfigure->Reset();

  return ToBeReturned;

}
int MuonboyInterface::LoadEvent(
        MboyVarByName*           pMboyVarByName,
        MuonboyBFAbstract*       pMuonboyBFAbstract,
        MuonboyRTAbstract*       pMuonboyRTAbstract,
        const MboyPosDigiChoice* pMboyPosDigiChoice ,
        MuonRecDigiStore*        pMuonRecDigiStore
){
  if ( pMboyVarByName == 0 ) return 1 ;
  if ( pMuonboyBFAbstract == 0 ) return 1 ;
  if ( pMuonboyRTAbstract == 0 ) return 1 ;
  if ( pMboyPosDigiChoice == 0 ) return 1 ;
  if ( pMuonRecDigiStore == 0 ) return 1 ;

  SetConfiguration(pMboyVarByName);

  MboyGenTrackStore*  pMboyGenTrackStore = 0 ;

  p_MuonboySetBFRT->Set( 
                        pMuonboyBFAbstract,
                        pMuonboyRTAbstract
                       );
  
  p_MuonboyCocoupMaker->execute(
                                pMboyPosDigiChoice,
                                pMuonRecDigiStore,
                                pMboyGenTrackStore
                               );

  p_MuonboyConfigure->Reset();

  return 0;

}
void MuonboyInterface::DoCollectMatter(
                                MuonboyBFAbstract*   pMuonboyBFAbstract,
                                int AtCosmMatter,
                                double Xpos_in,
                                double Ypos_in,
                                double Zpos_in,
                                double Xmom_in,
                                double Ymom_in,
                                double Zmom_in,
                                double Charge,
                                std::vector<MbMuSc*>* pVectorOfMbMuSc
){

  p_MuonboyCollectMatter->execute(
  			          pMuonboyBFAbstract,
                                  AtCosmMatter,
                                  Xpos_in,
                                  Ypos_in,
                                  Zpos_in,
                                  Xmom_in,
                                  Ymom_in,
                                  Zmom_in,
                                  Charge,
                                  pVectorOfMbMuSc
  			         );

}

void MuonboyInterface::SetVariablesDefault(MboyVarByName*  pMboyVarByName)
{

  p_MuonboyConfigure->SetVariablesDefault(pMboyVarByName);

}
void MuonboyInterface::Reset()
{

  p_MuonboyConfigure->Reset();

}

void MuonboyInterface::GetAgddXmlVarNb    (int& NbVar )                                   {p_AgddXml->GetAgddXmlVariableNb    ( NbVar )              ;}
void MuonboyInterface::GetAgddXmlVarName  (int  IVar , std::string& NameVar )             {p_AgddXml->GetAgddXmlVariableName  ( IVar , NameVar )     ;}
void MuonboyInterface::GetAgddXmlVarValue (std::string  NameVar, int& Iok, double& Value) {p_AgddXml->GetAgddXmlVariableValue ( NameVar , Iok, Value);}

void MuonboyInterface::SetMatiereTagInfo(std::vector< std::pair<std::string,std::string> > TagInfoXML)
{
  MuonboyMaterialTagInfo aMuonboyMaterialTagInfo ;
  aMuonboyMaterialTagInfo.SetMatiereTagInfo(TagInfoXML);
}

void MuonboyInterface::SetMatiereAGDD2GeoSwitches( std::vector<std::string> AGDD2GeoSwitches)
{
  MuonboyMaterialAGDD2GeoSwitches aMuonboyMaterialAGDD2GeoSwitches ;
  aMuonboyMaterialAGDD2GeoSwitches.SetMatiereAGDD2GeoSwitches(AGDD2GeoSwitches);
}
int MuonboyInterface::SetAliLine(
                          MboyAlineStore* pMboyAlineStore,
                          MboyBlineStore* pMboyBlineStore, 
                          MboyIlineStore* pMboyIlineStore 
){
  int ToBeReturned = 0 ;

  int StatusAandBlinesLoader = p_AandBlinesLoader->execute(
                                                           pMboyAlineStore,
                                                           pMboyBlineStore,
                                                           pMboyIlineStore
                                                          );
  if ( StatusAandBlinesLoader !=0 ){
    std::cout << "p_AandBlinesLoader->execute failed " << std::endl;
    return StatusAandBlinesLoader;
  }

  return ToBeReturned;

}
void MuonboyInterface::SetFieldStatus(
                               int getToroidBarrelOn ,
                               int getToroidECTAOn   ,
                               int getToroidECTCOn   ,
                               int getSolenoidOn     ,
                               int getAllToroidOn    ,
                               int getFieldStatusOn 
){
  MuonboyFieldStatus aMuonboyFieldStatus ;
  aMuonboyFieldStatus.SetFieldStatus(
                                     getToroidBarrelOn ,
                                     getToroidECTAOn   ,
                                     getToroidECTCOn   ,
                                     getSolenoidOn     ,
                                     getAllToroidOn    ,
                                     getFieldStatusOn 
                                    );
}
void MuonboyInterface::SetDeadChannels( std::vector<std::string> MultiLayerOut)
{
 MuonboyMultiLayerOut aMuonboyMultiLayerOut ;
 aMuonboyMultiLayerOut.SetMultiLayerOut(MultiLayerOut);
}
void MuonboyInterface::SetConfiguration( MboyVarByName*  pMboyVarByName)
{
  p_MuonboyConfigure->IniReco(pMboyVarByName);
}
		  	  

