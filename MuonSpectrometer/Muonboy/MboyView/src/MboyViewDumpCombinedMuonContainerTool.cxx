/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "Particle/TrackParticle.h"

#include "TrkTrackSummary/TrackSummary.h"

#include "MuonSegment/MuonSegment.h"

//----------------------------------------------------------------//
#include "muonEvent/MuonContainer.h"
#include "TrkTrack/Track.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewDumpCombinedMuonContainerTool.h"

static const InterfaceID IID_IMboyViewDumpCombinedMuonContainerTool("MboyViewDumpCombinedMuonContainerTool", 1, 0);

const InterfaceID& MboyViewDumpCombinedMuonContainerTool::interfaceID()
{
  return IID_IMboyViewDumpCombinedMuonContainerTool;
}

MboyViewDumpCombinedMuonContainerTool::MboyViewDumpCombinedMuonContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewDumpCombinedMuonContainerTool>(this);

}

MboyViewDumpCombinedMuonContainerTool::~MboyViewDumpCombinedMuonContainerTool(){}

// Initialize
StatusCode MboyViewDumpCombinedMuonContainerTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }


  return StatusCode::SUCCESS;

}

StatusCode MboyViewDumpCombinedMuonContainerTool::ViewCombinedDump(
                     std::string MuonContainerLocation,
		     std::ofstream* pOutFile
){

  p_OutCurrent = pOutFile ;
  StatusCode sc = dump_Container(MuonContainerLocation);
  if (sc.isFailure()) ATH_MSG_WARNING( "dump_Container failed " );

  return StatusCode::SUCCESS;

}
StatusCode MboyViewDumpCombinedMuonContainerTool::DoDump(
                     const Analysis::MuonContainer* pCombinedMuonContainer,
		     std::ofstream* pOutFile
){

  p_OutCurrent = pOutFile ;
  StatusCode sc = dump_Container(pCombinedMuonContainer);
  if (sc.isFailure()) ATH_MSG_WARNING( "dump_Container(pCombinedMuonContainer) failed " );

  return StatusCode::SUCCESS;

}

// Dumps Container
StatusCode MboyViewDumpCombinedMuonContainerTool::dump_Container(
                             std::string ContainerName
){

  StatusCode sc = StatusCode::SUCCESS;

  const Analysis::MuonContainer* pCombinedMuonContainer ;
  sc = evtStore()->retrieve(pCombinedMuonContainer, ContainerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( " pCombinedMuonContainer not found at " << ContainerName );
    return StatusCode::SUCCESS;
  }

  sc = dump_Container(pCombinedMuonContainer);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( " dump_Container(pCombinedMuonContainer) failed " );
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;

}

StatusCode MboyViewDumpCombinedMuonContainerTool::dump_Container(
                             const Analysis::MuonContainer* pCombinedMuonContainer
){

  StatusCode sc = StatusCode::SUCCESS;

  if (!pCombinedMuonContainer) {
    ATH_MSG_WARNING( " pCombinedMuonContainer null " );
    return StatusCode::SUCCESS;
  }

  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( " retrieve TheEventInfo failed" );
    return StatusCode::SUCCESS;
  }

  *p_OutCurrent 
      << " " 
      <<std::endl;
  *p_OutCurrent 
      << "* For the event : " 
      << TheEventInfo->event_ID()->event_number()
      << std::endl;

  int Kounter = 0 ;
  for (Analysis::MuonContainer::const_iterator it = pCombinedMuonContainer->begin(); it!=pCombinedMuonContainer->end(); ++it){

    Kounter = Kounter + 1 ;
    *p_OutCurrent 
        << " " 
        <<std::endl;
    *p_OutCurrent 
        << "* Track number  " 
        << Kounter 
        <<std::endl;

//Hopefully Temporary
    bool TemphasMuonSpectrometerTrackParticle = false ;
    if ( (*it)->muonSpectrometerTrackParticle() ) TemphasMuonSpectrometerTrackParticle = true ;

    *p_OutCurrent << "############################  "   << std::endl;
    *p_OutCurrent << "#####   Combined Muon  #####  "   << std::endl;
    *p_OutCurrent << "############################  "   << std::endl;

    *p_OutCurrent << " 	author ()                         ? : " << std::setw(12) << (*it)->author ()                        << std::endl;
    *p_OutCurrent << " 	allAuthors ()                     ? : " << std::setw(12) << (*it)->allAuthors ()                        << std::endl;

    *p_OutCurrent << " parameter()" << std::endl;
    const std::vector< std::pair<MuonParameters::ParamDef,float> > ParamDef_Vec =   (*it)->parameter();
    typedef std::pair<MuonParameters::ParamDef,float> muonParamsLocal;
    std::vector<muonParamsLocal>::const_iterator pLoop = ParamDef_Vec.begin();
    for (;pLoop !=ParamDef_Vec.end(); pLoop++) {
      *p_OutCurrent << " parameter()" 
                    << " (*pLoop).first " << (*pLoop).first
                    << " (*pLoop).second " << (*pLoop).second
                    << std::endl;
    }

    *p_OutCurrent << "matchChi2 ()                          : " << std::setw(12)<<std::setprecision(5)<< (*it)->matchChi2 ()                          << std::endl;
    *p_OutCurrent << "matchChi2OverDoF ()                   : " << std::setw(12)<<std::setprecision(5)<< (*it)->matchChi2OverDoF ()                   << std::endl;
    *p_OutCurrent << "matchNumberDoF ()                     : " << std::setw(12)                      << (*it)->matchNumberDoF ()                     << std::endl;
    *p_OutCurrent << "fitChi2 ()                            : " << std::setw(12)<<std::setprecision(5)<< (*it)->fitChi2 ()                            << std::endl;
    *p_OutCurrent << "fitNumberDoF ()                       : " << std::setw(12)                      << (*it)->fitNumberDoF ()                       << std::endl;
    *p_OutCurrent << "fitChi2OverDoF ()                     : " << std::setw(12)<<std::setprecision(5)<< (*it)->fitChi2OverDoF ()                     << std::endl;


    *p_OutCurrent << "Param. energy loss and his error " 
                  << std::setw(12)<<std::setprecision(5)<< (*it)->energyLoss().first 
                  << " , " 
                  << std::setw(12)<<std::setprecision(5)<< (*it)->energyLoss().second 
                  << std::endl;
    
    *p_OutCurrent << "caloEnergyLoss()                 " 
                  << std::setw(12)<<std::setprecision(5)<< (*it)->caloEnergyLoss()->deltaE() 
                  << " , " 
                  << std::setw(12)<<std::setprecision(5)<< (*it)->caloEnergyLoss()->sigmaDeltaE() 
                  << std::endl;


    *p_OutCurrent << "bestMatch                           ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->bestMatch ()                        << std::endl;

    *p_OutCurrent << "isCorrected                         ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isCorrected ()                      << std::endl;
    *p_OutCurrent << "isCombinedMuon                      ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isCombinedMuon ()                   << std::endl;
    *p_OutCurrent << "isStandAloneMuon                    ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isStandAloneMuon ()                 << std::endl;

    *p_OutCurrent << "hasInDetTrackParticle               ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->hasInDetTrackParticle ()            << std::endl;
    *p_OutCurrent << "hasMuonExtrapolatedTrackParticle    ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->hasMuonExtrapolatedTrackParticle () << std::endl;
    *p_OutCurrent << "hasInnerExtrapolatedTrackParticle   ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->hasInnerExtrapolatedTrackParticle ()<< std::endl;
    *p_OutCurrent << "hasCombinedMuonTrackParticle        ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->hasCombinedMuonTrackParticle ()     << std::endl;
    *p_OutCurrent << "hasCluster                          ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->hasCluster ()                       << std::endl;

//Hopefully Temporary
    *p_OutCurrent << "Has MS TrackParticle           ? : " << std::setw(12)<<std::setprecision(5)<< TemphasMuonSpectrometerTrackParticle << std::endl;

    *p_OutCurrent << "isSegmentTaggedMuon                 ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isSegmentTaggedMuon ()                          << std::endl;
//  *p_OutCurrent << "isTagged                            ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isTagged ()                          << std::endl;
    *p_OutCurrent << "isCaloMuonId                        ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isCaloMuonId ()                     << std::endl;

    *p_OutCurrent << "Clusters "<< std::endl;
    if ( (*it)->hasCluster() ) {
      *p_OutCurrent << *((*it)->cluster()) << std::endl;
    }else{
      *p_OutCurrent << " No Clusters "<< std::endl;
    }
    
    
    *p_OutCurrent << " numberOfBLayerHits ()                : " << std::setw(12)                      << (*it)-> numberOfBLayerHits ()                           << std::endl;
    *p_OutCurrent << " numberOfPixelHits ()                 : " << std::setw(12)                      << (*it)-> numberOfPixelHits ()                            << std::endl;
    *p_OutCurrent << " numberOfSCTHits ()                   : " << std::setw(12)                      << (*it)-> numberOfSCTHits ()                              << std::endl;
    *p_OutCurrent << " numberOfTRTHits ()                   : " << std::setw(12)                      << (*it)-> numberOfTRTHits ()                              << std::endl;
    *p_OutCurrent << " numberOfTRTHighThresholdHits ()      : " << std::setw(12)                      << (*it)-> numberOfTRTHighThresholdHits ()                 << std::endl;
    *p_OutCurrent << " numberOfBLayerSharedHits ()          : " << std::setw(12)                      << (*it)-> numberOfBLayerSharedHits ()                     << std::endl;
    *p_OutCurrent << " numberOfPixelSharedHits ()           : " << std::setw(12)                      << (*it)-> numberOfPixelSharedHits ()                      << std::endl;
    *p_OutCurrent << " numberOfPixelHoles ()                : " << std::setw(12)                      << (*it)-> numberOfPixelHoles ()                           << std::endl;
    *p_OutCurrent << " numberOfSCTSharedHits ()             : " << std::setw(12)                      << (*it)-> numberOfSCTSharedHits ()                        << std::endl;
    *p_OutCurrent << " numberOfSCTHoles ()                  : " << std::setw(12)                      << (*it)-> numberOfSCTHoles ()                             << std::endl;
    *p_OutCurrent << " numberOfTRTOutliers ()               : " << std::setw(12)                      << (*it)-> numberOfTRTOutliers ()                          << std::endl;
    *p_OutCurrent << " numberOfTRTHighThresholdOutliers ()  : " << std::setw(12)                      << (*it)-> numberOfTRTHighThresholdOutliers ()             << std::endl;

    *p_OutCurrent << " numberOfMDTHits ()                   : " << std::setw(12)                      << (*it)-> numberOfMDTHits ()                              << std::endl;
    *p_OutCurrent << " numberOfMDTHoles ()                  : " << std::setw(12)                      << (*it)-> numberOfMDTHoles ()                             << std::endl;
    *p_OutCurrent << " numberOfCSCEtaHits ()                : " << std::setw(12)                      << (*it)-> numberOfCSCEtaHits ()                           << std::endl;
    *p_OutCurrent << " numberOfCSCEtaHoles ()               : " << std::setw(12)                      << (*it)-> numberOfCSCEtaHoles ()                          << std::endl;
    *p_OutCurrent << " numberOfCSCPhiHits ()                : " << std::setw(12)                      << (*it)-> numberOfCSCPhiHits ()                           << std::endl;
    *p_OutCurrent << " numberOfCSCPhiHoles ()               : " << std::setw(12)                      << (*it)-> numberOfCSCPhiHoles ()                          << std::endl;
    *p_OutCurrent << " numberOfRPCEtaHits ()                : " << std::setw(12)                      << (*it)-> numberOfRPCEtaHits ()                           << std::endl;
    *p_OutCurrent << " numberOfRPCEtaHoles ()               : " << std::setw(12)                      << (*it)-> numberOfRPCEtaHoles ()                          << std::endl;
    *p_OutCurrent << " numberOfRPCPhiHits ()                : " << std::setw(12)                      << (*it)-> numberOfRPCPhiHits ()                           << std::endl;
    *p_OutCurrent << " numberOfRPCPhiHoles ()               : " << std::setw(12)                      << (*it)-> numberOfRPCPhiHoles ()                          << std::endl;
    *p_OutCurrent << " numberOfTGCEtaHits ()                : " << std::setw(12)                      << (*it)-> numberOfTGCEtaHits ()                           << std::endl;
    *p_OutCurrent << " numberOfTGCEtaHoles ()               : " << std::setw(12)                      << (*it)-> numberOfTGCEtaHoles ()                          << std::endl;
    *p_OutCurrent << " numberOfTGCPhiHits ()                : " << std::setw(12)                      << (*it)-> numberOfTGCPhiHits ()                           << std::endl;
    *p_OutCurrent << " numberOfTGCPhiHoles ()               : " << std::setw(12)                      << (*it)-> numberOfTGCPhiHoles ()                          << std::endl;

    *p_OutCurrent << " numberOfGangedPixels ()              : " << std::setw(12)                      << (*it)-> numberOfGangedPixels ()                         << std::endl;
    *p_OutCurrent << " numberOfOutliersOnTrack ()           : " << std::setw(12)                      << (*it)-> numberOfOutliersOnTrack ()                      << std::endl;
    *p_OutCurrent << " numberOfTrackSummaryTypes ()         : " << std::setw(12)                      << (*it)-> numberOfTrackSummaryTypes ()                    << std::endl;


    *p_OutCurrent << "associatedEtaDigits().size()            : " << std::setw(12)                      <<((*it)->associatedEtaDigits()).size() << std::endl;
    for (unsigned int index=0; index<((*it)->associatedEtaDigits()).size(); index++){
    *p_OutCurrent << "associatedEtaDigits()                   : " 
                                                                  << std::setw(12)                      <<index 
                                                                  << std::setw(12)                      <<((*it)->associatedEtaDigits())[index] 
                                                                  << std::endl;
    }
    *p_OutCurrent << "associatedPhiDigits().size()            : " << std::setw(12)                      <<((*it)->associatedPhiDigits()).size() << std::endl;
    for (unsigned int index=0; index<((*it)->associatedPhiDigits()).size(); index++){
    *p_OutCurrent << "associatedPhiDigits()                   : " 
                                                                  << std::setw(12)                      <<index 
                                                                  << std::setw(12)                      <<((*it)->associatedPhiDigits())[index] 
                                                                  << std::endl;
    }
    *p_OutCurrent << "segmentEtaDigits().size()               : " << std::setw(12)                      <<((*it)->segmentEtaDigits()).size() << std::endl;
    for (unsigned int index=0; index<((*it)->segmentEtaDigits()).size(); index++){
    *p_OutCurrent << "segmentEtaDigits()                      : " 
                                                                  << std::setw(12)                      <<index 
                                                                  << std::setw(12)                      <<((*it)->segmentEtaDigits())[index] 
                                                                  << std::endl;
    }
    *p_OutCurrent << "segmentPhiDigits().size()               : " << std::setw(12)                      <<((*it)->segmentPhiDigits()).size() << std::endl;
    for (unsigned int index=0; index<((*it)->segmentPhiDigits()).size(); index++){
    *p_OutCurrent << "segmentPhiDigits()                      : " 
                                                                  << std::setw(12)                      <<index 
                                                                  << std::setw(12)                      <<((*it)->segmentPhiDigits())[index] 
                                                                  << std::endl;
    }

    *p_OutCurrent << "segmentDeltaEta ()                    : " << std::setw(12)<<std::setprecision(5)<< (*it)->segmentDeltaEta ()                       << std::endl;
    *p_OutCurrent << "segmentDeltaPhi ()                    : " << std::setw(12)<<std::setprecision(5)<< (*it)->segmentDeltaPhi ()                       << std::endl;
    *p_OutCurrent << "segmentChi2OverDoF ()                 : " << std::setw(12)<<std::setprecision(5)<< (*it)->segmentChi2OverDoF ()                    << std::endl;
    *p_OutCurrent << "annBarrel ()                          : " << std::setw(12)<<std::setprecision(5)<< (*it)->annBarrel ()                             << std::endl;
    *p_OutCurrent << "annEndCap ()                          : " << std::setw(12)<<std::setprecision(5)<< (*it)->annEndCap ()                             << std::endl;
    *p_OutCurrent << "innAngle ()                           : " << std::setw(12)<<std::setprecision(5)<< (*it)->innAngle ()                              << std::endl;
    *p_OutCurrent << "midAngle ()                           : " << std::setw(12)<<std::setprecision(5)<< (*it)->midAngle ()                              << std::endl;
    *p_OutCurrent << "t0 ()                                 : " << std::setw(12)<<std::setprecision(5)<< (*it)->t0 ()                                    << std::endl;
    *p_OutCurrent << "beta ()                               : " << std::setw(12)<<std::setprecision(5)<< (*it)->beta ()                                  << std::endl;

    *p_OutCurrent << "alsoFoundByLowPt                      ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->alsoFoundByLowPt ()                     << std::endl;
    *p_OutCurrent << "alsoFoundByCaloMuonId                 ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->alsoFoundByCaloMuonId ()                << std::endl;
    *p_OutCurrent << "caloMuonAlsoFoundByMuonReco           ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->caloMuonAlsoFoundByMuonReco ()          << std::endl;

    *p_OutCurrent << " numberOfSegments ()                    : " << std::setw(12)                      << (*it)-> numberOfSegments ()                    << std::endl;
    for (unsigned int index=0; index<((*it)->numberOfSegments()); index++){
      const Trk::Segment* pSegment = (*it)->muonSegment(index) ;
      *p_OutCurrent 
          << "* Segment number  " 
          << index 
          <<std::endl;

      *p_OutCurrent 
          << "* ------------------> dump as Trk::Segment " 
          <<std::endl;
      *p_OutCurrent 
          << *pSegment
          <<std::endl;

      const Muon::MuonSegment* pMuonSegment = dynamic_cast<const Muon::MuonSegment*>(pSegment);
      if ( pMuonSegment != 0 ){
        *p_OutCurrent 
            << "* ------------------> dump as Muon::MuonSegment " 
            <<std::endl;
        *p_OutCurrent 
            << *pMuonSegment
            <<std::endl;
      }

    }

    *p_OutCurrent << "isLoose                    ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isLoose ()          << std::endl;
    *p_OutCurrent << "isMedium                   ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isMedium ()         << std::endl;
    *p_OutCurrent << "isTight                    ? : " << std::setw(12)<<std::setprecision(5)<< (*it)->isTight ()          << std::endl;
    *p_OutCurrent << "isLoose2                   ? : " << std::setw(12)<<std::setprecision(5)<<((*it)->isMuon()).isLoose ()          << std::endl;
    *p_OutCurrent << "isMedium2                  ? : " << std::setw(12)<<std::setprecision(5)<<((*it)->isMuon()).isMedium ()         << std::endl;
    *p_OutCurrent << "isTight2                   ? : " << std::setw(12)<<std::setprecision(5)<<((*it)->isMuon()).isTight ()          << std::endl;
    *p_OutCurrent << "qualityWord                ? : " << std::setw(12)<<std::setprecision(5)<<((*it)->isMuon()).qualityWord ()      << std::endl;

    *p_OutCurrent << "parameter(MuonParameters::etcone10)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::etcone10) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::etcone20)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::etcone20) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::etcone30)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::etcone30) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::etcone40)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::etcone40) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::nucone10)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::nucone10) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::nucone20)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::nucone20) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::nucone30)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::nucone30) << std::endl;
    *p_OutCurrent << "parameter(MuonParameters::nucone40)     : " << std::setw(12)<<std::setprecision(5)<< (*it)->parameter(MuonParameters::nucone40) << std::endl;


    *p_OutCurrent  << "#################################################  " << std::endl;
    *p_OutCurrent  << "##### TrackParticle and Trk::Track Pointers #####  " << std::endl;
    *p_OutCurrent  << "#################################################  " << std::endl;
    if ((*it)->track()){
      const Rec::TrackParticle* pTrackParticlePRIMARY = (*it)->track();
      *p_OutCurrent   << " --------------------------  " << std::endl;
      *p_OutCurrent   << " --- Primary TrackParticle---  " << std::endl;
      *p_OutCurrent   << " --------------------------  " << std::endl;

      dumpTrackParticle(pTrackParticlePRIMARY) ;

    }else{
      *p_OutCurrent << " ----- DOES NOT HAVE Primary TrackParticle   -----  " << std::endl;
    }
  
    if ((*it)->hasInDetTrackParticle()){
      const Rec::TrackParticle* pTrackParticle = (*it)->inDetTrackParticle();
      *p_OutCurrent   << " --------------------------  " << std::endl;
      *p_OutCurrent   << " --- InDet TrackParticle---  " << std::endl;
      *p_OutCurrent   << " --------------------------  " << std::endl;

      dumpTrackParticle(pTrackParticle) ;

    }else{
      *p_OutCurrent << " ----- DOES NOT HAVE InDet TrackParticle    -----  " << std::endl;
    }
    
//Hopefully Temporary
    if(TemphasMuonSpectrometerTrackParticle){
      const Rec::TrackParticle* pTrackParticle = (*it)->muonSpectrometerTrackParticle() ;
      *p_OutCurrent   << " -------------------------------------  " << std::endl;
      *p_OutCurrent   << " --- MuonSpectormeter TrackParticle---  " << std::endl;
      *p_OutCurrent   << " -------------------------------------  " << std::endl;

      dumpTrackParticle(pTrackParticle) ;

    }else{
      *p_OutCurrent << " ----- DOES NOT HAVE MuonSpectormeter TrackParticle    -----  " << std::endl;
    }
  
    if( (*it)->hasMuonExtrapolatedTrackParticle() ){
      const Rec::TrackParticle* pTrackParticle = (*it)->muonExtrapolatedTrackParticle();
      *p_OutCurrent  << " --------------------------------------  " << std::endl;
      *p_OutCurrent  << " --- Muon Extrapolated TrackParticle---  " << std::endl;
      *p_OutCurrent  << " --------------------------------------  " << std::endl;

      dumpTrackParticle(pTrackParticle) ;

    }else{
      *p_OutCurrent  << " ----- DOES NOT HAVE Muon Extrapolated TrackParticle    -----  " << std::endl;
    }

    if( (*it)->hasInnerExtrapolatedTrackParticle() ){
      const Rec::TrackParticle* pTrackParticle = (*it)->innerExtrapolatedTrackParticle();
      *p_OutCurrent  << " --------------------------------------  " << std::endl;
      *p_OutCurrent  << " --- Inner ExtrapolatedTrack Particle---  " << std::endl;
      *p_OutCurrent  << " --------------------------------------  " << std::endl;

      dumpTrackParticle(pTrackParticle) ;

    }else{
      *p_OutCurrent  << " ----- DOES NOT HAVE Inner ExtrapolatedTrack Particle    -----  " << std::endl;
    }
    
    if( (*it)->hasCombinedMuonTrackParticle() ){
      const Rec::TrackParticle* pTrackParticle = (*it)->combinedMuonTrackParticle();
      *p_OutCurrent  << " --------------------------------------  " << std::endl;
      *p_OutCurrent   << " --- Muon Combined TrackParticle---  " <<std::endl;
      *p_OutCurrent  << " --------------------------------------  " << std::endl;

      dumpTrackParticle(pTrackParticle) ;

    }else{
      *p_OutCurrent  << " ----- DOES NOT HAVE Muon Combined TrackParticle    -----  " <<std::endl;
    }


  }

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyViewDumpCombinedMuonContainerTool::finalize(){return StatusCode::SUCCESS;}


void MboyViewDumpCombinedMuonContainerTool::dumpTrackParticle(const Rec::TrackParticle* pTrackParticle){

  if ( pTrackParticle == 0 ){
    *p_OutCurrent << "--------- pTrackParticle null " << std::endl;
    return ;
  }
  
  *p_OutCurrent 
      << "* ------------------> dump as Rec::TrackParticle " 
      <<std::endl;
  *p_OutCurrent 
      << *pTrackParticle
      <<std::endl;
  
  *p_OutCurrent   << " -----TrackSummary of Rec::TrackParticle  " << std::endl;
  if (pTrackParticle->trackSummary()) {
    *p_OutCurrent   <<  *(pTrackParticle->trackSummary())          << std::endl;
  }else{
    *p_OutCurrent   << " (pTrackParticle->trackSummary())    is 0 "      << std::endl;
  }
 
 
  *p_OutCurrent << "--------- Associated Track ---------- " << std::endl;
  const Trk::Track* pTrack = pTrackParticle->originalTrack();
  summariseTrack(pTrack);

  return;

} 
void MboyViewDumpCombinedMuonContainerTool::summariseTrack(const Trk::Track* pTrack){

  if ( pTrack == 0 ){
    *p_OutCurrent << "--------- pTrack null " << std::endl;
    return ;
  }

  *p_OutCurrent 
      << "* ------------------> dump the Trk::Track " 
      <<std::endl;
  *p_OutCurrent 
      << *pTrack
      <<std::endl;

  *p_OutCurrent 
      << "* ------------------> summarise the Trk::Track " 
      <<std::endl;

  *p_OutCurrent << "AUTHOR " << (pTrack->info()).dumpInfo()<< std::endl;
  
  if (pTrack->fitQuality()!=0)
    *p_OutCurrent << "Has chi2 " << pTrack->fitQuality()->chiSquared()<< " and " <<pTrack->fitQuality()->numberDoF()<<" degrees of freedom"<<std::endl;

  if (pTrack->trackParameters()!=0)
    *p_OutCurrent << "Has  " << pTrack->trackParameters()->size() << " tracks parameter(s)"<<std::endl;

  if (pTrack->perigeeParameters()!=0)
    *p_OutCurrent << "Has perigee of (d0,z0,phi,theta,q/p) = ("
	          <<std::setw(12)<<std::setprecision(5)<<pTrack->perigeeParameters()->parameters()[Trk::d0]    <<", "
	          <<std::setw(12)<<std::setprecision(5)<<pTrack->perigeeParameters()->parameters()[Trk::z0]    <<", "
	          <<std::setw(12)<<std::setprecision(5)<<pTrack->perigeeParameters()->parameters()[Trk::phi0]  <<", "
	          <<std::setw(12)<<std::setprecision(5)<<pTrack->perigeeParameters()->parameters()[Trk::theta] <<", "
	          <<std::setw(12)<<std::setprecision(5)<<pTrack->perigeeParameters()->parameters()[Trk::qOverP]<<")"
	          <<std::endl;

  if (pTrack->measurementsOnTrack()!=0)
   *p_OutCurrent << "Has " << pTrack->measurementsOnTrack()->size()<< " RIO_OnTrack(s)"<<std::endl;

  if (pTrack->trackStateOnSurfaces()!=0)
   *p_OutCurrent << "Has " << pTrack->trackStateOnSurfaces()->size()<< " TrackStateOnSurface(s)"<<std::endl;

  return;

} 

