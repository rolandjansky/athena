/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
/// Rec::MuonPringingTool.h, (c) ATLAS Detector software        ///
///                                                             ///
/// This tool creates dump output of different detail to strings///
/// strings or files for an Analysis::Muon or a whole collection.///
///////////////////////////////////////////////////////////////////
#include "MuonPrintingTool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "AthLinks/ElementLink.h"


/** constructor */
Rec::MuonPrintingTool::MuonPrintingTool( const std::string& type,
                const std::string& name, 
                const IInterface* parent )
  : AthAlgTool( type, name, parent ),
    m_edmPrinter("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
 {
   declareProperty("MuonStationPrinter",m_edmPrinter);
   declareInterface<IMuonPrintingTool>( this );
}

/** destructor */
Rec::MuonPrintingTool::~MuonPrintingTool( )
{ }

/** initialization */
StatusCode  Rec::MuonPrintingTool::initialize() {

  if (m_edmPrinter.retrieve().isSuccess()){
    ATH_MSG_DEBUG ("Retrieved " << m_edmPrinter);
  } else {
    ATH_MSG_ERROR ("Could not get " << m_edmPrinter);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ("Initialize() successful in "<<name());
  return StatusCode::SUCCESS;
}

/** end of the job - finalize */
StatusCode  Rec::MuonPrintingTool::finalize() {

  ATH_MSG_DEBUG ("Nothing to finalize.");
  return StatusCode::SUCCESS;
}


std::string Rec::MuonPrintingTool::print( const xAOD::TrackParticle& tp ) const {
  std::ostringstream sout;
  sout << "  pt : " <<  tp.pt()
       << " eta : " <<  tp.eta()
       << " phi : " <<  tp.phi();
  return sout.str();
}

std::string Rec::MuonPrintingTool::print( const xAOD::Muon& muon ) const {

  std::ostringstream sout;

  sout << "#####   Muon,  pt : " <<  muon.pt()
       << " eta : " <<  muon.eta()
       << " phi : " <<  muon.phi()
       << " mass : " <<  muon.m() << " author " << muon.author() 
       << " type : " << muon.muonType()
       << " secondary authors: ";
  for( int a = 0;a<xAOD::Muon::NumberOfMuonAuthors;++a ){
    xAOD::Muon::Author author = static_cast<xAOD::Muon::Author>(a);
    if( author != muon.author() && muon.isAuthor(author) ) sout << " " << a;
  }
  sout << std::endl;    

  sout << " ParamDef available:" << std::endl;
  float spectrometerFieldIntegral=0;
  if(muon.parameter(spectrometerFieldIntegral, xAOD::Muon::spectrometerFieldIntegral))
    sout << "  spectrometerFieldIntegral : " << spectrometerFieldIntegral << std::endl;
  float scatteringCurvatureSignificance=0;
  if(muon.parameter(scatteringCurvatureSignificance, xAOD::Muon::scatteringCurvatureSignificance))
    sout << "  scatteringCurvatureSignificance : " << scatteringCurvatureSignificance << std::endl;
  float scatteringNeighbourSignificance=0;
  if(muon.parameter(scatteringNeighbourSignificance, xAOD::Muon::scatteringNeighbourSignificance))
    sout << "  scatteringNeighbourSignificance : " << scatteringNeighbourSignificance << std::endl;
  float momentumBalanceSignificance=0;
  if(muon.parameter(momentumBalanceSignificance, xAOD::Muon::momentumBalanceSignificance))
    sout << "  momentumBalanceSignificance : " << momentumBalanceSignificance << std::endl;
  float segmentDeltaEta=0;
  if(muon.parameter(segmentDeltaEta, xAOD::Muon::segmentDeltaEta))
    sout << "  segmentDeltaEta : " << segmentDeltaEta << std::endl;
  float segmentDeltaPhi=0;
  if(muon.parameter(segmentDeltaPhi, xAOD::Muon::segmentDeltaPhi))
    sout << "  segmentDeltaPhi : " << segmentDeltaPhi << std::endl;
  float segmentChi2OverDoF=0;
  if(muon.parameter(segmentChi2OverDoF, xAOD::Muon::segmentChi2OverDoF))
    sout << "  segmentChi2OverDoF : " << segmentChi2OverDoF << std::endl;
  float t0=0;
  if(muon.parameter(t0, xAOD::Muon::t0))
    sout << "  t0 : " << t0 << std::endl;
  float beta=0;
  if(muon.parameter(beta, xAOD::Muon::beta))
    sout << "  beta : " << beta << std::endl;
  float annBarrel=0;
  if(muon.parameter(annBarrel, xAOD::Muon::annBarrel))
    sout << "  annBarrel : " << annBarrel << std::endl;
  float annEndCap=0;
  if(muon.parameter(annEndCap, xAOD::Muon::annEndCap))
    sout << "  annEndCap : " << annEndCap << std::endl;
  float innAngle=0;
  if(muon.parameter(innAngle, xAOD::Muon::innAngle))
    sout << "  innAngle : " << innAngle << std::endl;
  float midAngle=0;
  if(muon.parameter(midAngle, xAOD::Muon::midAngle))
    sout << "  midAngle : " << midAngle << std::endl;
  float msInnerMatchChi2=0;
  if(muon.parameter(msInnerMatchChi2, xAOD::Muon::msInnerMatchChi2))
    sout << "  msInnerMatchChi2 : " << msInnerMatchChi2 << std::endl;
  int msInnerMatchDOF=0;
  if(muon.parameter(msInnerMatchDOF, xAOD::Muon::msInnerMatchDOF))
    sout << "  msInnerMatchDOF : " << msInnerMatchDOF << std::endl;
  float msOuterMatchChi2=0;
  if(muon.parameter(msOuterMatchChi2, xAOD::Muon::msOuterMatchChi2))
    sout << "  msOuterMatchChi2 : " << msOuterMatchChi2 << std::endl;
  int msOuterMatchDOF=0;
  if(muon.parameter(msOuterMatchDOF, xAOD::Muon::msOuterMatchDOF))
    sout << "  msOuterMatchDOF : " << msOuterMatchDOF << std::endl;
  float meanDeltaADCCountsMDT=0;
  if(muon.parameter(meanDeltaADCCountsMDT, xAOD::Muon::meanDeltaADCCountsMDT))
    sout << "  meanDeltaADCCountsMDT : " << meanDeltaADCCountsMDT << std::endl;
  float CaloLRLikelihood=0;
  if(muon.parameter(CaloLRLikelihood, xAOD::Muon::CaloLRLikelihood))
    sout << "  CaloLRLikelihood : " << CaloLRLikelihood << std::endl;
  float CaloMuonScore=0;
  if(muon.parameter(CaloLRLikelihood, xAOD::Muon::CaloMuonScore))
    sout << "  CaloMuonScore : " << CaloMuonScore << std::endl;
  int CaloMuonIDTag=0;
  if(muon.parameter(CaloMuonIDTag, xAOD::Muon::CaloMuonIDTag))
    sout << "  CaloMuonIDTag : " << CaloMuonIDTag << std::endl;
  float FSR_CandidateEnergy=0;
  if(muon.parameter(FSR_CandidateEnergy, xAOD::Muon::FSR_CandidateEnergy))
    sout << "  FSR_CandidateEnergy : " << FSR_CandidateEnergy << std::endl;
  float EnergyLoss=0;
  if(muon.parameter(EnergyLoss, xAOD::Muon::EnergyLoss))
    sout << "  EnergyLoss : " << EnergyLoss << std::endl;
  float ParamEnergyLoss=0;
  if(muon.parameter(ParamEnergyLoss, xAOD::Muon::ParamEnergyLoss))
    sout << "  ParamEnergyLoss : " << ParamEnergyLoss << std::endl;
  float MeasEnergyLoss=0;
  if(muon.parameter(MeasEnergyLoss, xAOD::Muon::MeasEnergyLoss))
    sout << "  MeasEnergyLoss : " << MeasEnergyLoss << std::endl;
  float EnergyLossSigma=0;
  if(muon.parameter(EnergyLossSigma, xAOD::Muon::EnergyLossSigma))
    sout << "  EnergyLossSigma : " << EnergyLossSigma << std::endl;
  float ParamEnergyLossSigmaPlus=0;
  if(muon.parameter(ParamEnergyLossSigmaPlus, xAOD::Muon::ParamEnergyLossSigmaPlus))
    sout << "  ParamEnergyLossSigmaPlus : " << ParamEnergyLossSigmaPlus << std::endl;
  float ParamEnergyLossSigmaMinus=0;
  if(muon.parameter(ParamEnergyLossSigmaMinus, xAOD::Muon::ParamEnergyLossSigmaMinus))
    sout << "  ParamEnergyLossSigmaMinus : " << ParamEnergyLossSigmaMinus << std::endl;
  float MeasEnergyLossSigma=0;
  if(muon.parameter(MeasEnergyLossSigma, xAOD::Muon::MeasEnergyLossSigma))
    sout << "  MeasEnergyLossSigma : " << MeasEnergyLossSigma << std::endl;
  uint8_t energyLossType = muon.energyLossType();
//  if(muon.parameter(energyLossType, xAOD::Muon::EnergyLossType)) 
  sout << "  EnergyLossType : " << static_cast<int>(energyLossType) << std::endl;
   

  uint8_t nprecisionLayers = 0;
  uint8_t nprecisionHoleLayers = 0;
  uint8_t nphiLayers = 0;         
  uint8_t ntrigEtaLayers = 0;     
  uint8_t nphiHoleLayers = 0;     
  uint8_t ntrigEtaHoleLayers = 0; 
  uint8_t mainSector = 0;
  uint8_t secondSector = 0;
  if( muon.primaryTrackParticleLink().isValid() ){
    const xAOD::TrackParticle& tp = **muon.primaryTrackParticleLink();
    if( !tp.summaryValue(nprecisionLayers,xAOD::numberOfPrecisionLayers) ) nprecisionLayers = 0;
    if( !tp.summaryValue(nprecisionHoleLayers,xAOD::numberOfPrecisionHoleLayers) ) nprecisionHoleLayers = 0;
    if( !tp.summaryValue(nphiLayers,xAOD::numberOfPhiLayers) ) nphiLayers = 0;         
    if( !tp.summaryValue(nphiHoleLayers,xAOD::numberOfPhiHoleLayers) ) nphiHoleLayers = 0;   
    if( !tp.summaryValue(ntrigEtaLayers,xAOD::numberOfTriggerEtaLayers) ) ntrigEtaLayers = 0;     
    if( !tp.summaryValue(ntrigEtaHoleLayers,xAOD::numberOfTriggerEtaHoleLayers) ) ntrigEtaHoleLayers = 0; 
  } else sout << " No valid primaryTrackParticleLink for this muon" << std::endl;

  if( !muon.summaryValue(mainSector,xAOD::primarySector) ) mainSector = 0;
  if( !muon.summaryValue(secondSector,xAOD::secondarySector) ) secondSector = 0;
  sout << " Station Layers: precision " << static_cast<int>(nprecisionLayers) << " holes " << static_cast<int>(nprecisionHoleLayers )
       << " phi " << static_cast<int>(nphiLayers) << " holes " << static_cast<int>(nphiHoleLayers)
       << " trigEta " << static_cast<int>(ntrigEtaLayers) << " holes " << static_cast<int>(ntrigEtaHoleLayers)
       << " main sector " << static_cast<int>(mainSector) << " secondary " << static_cast<int>(secondSector) << std::endl;

  bool printMeasurements = true;

  if( muon.combinedTrackParticleLink().isValid() ){
    const xAOD::TrackParticle* cbtp = *muon.combinedTrackParticleLink();
    if( cbtp ){
      sout << " --- Combined Muon track ---  " << print(*cbtp); 
      if( !cbtp->trackLink().isValid() ){
	sout << " No Track link";
	ATH_MSG_DEBUG("Combined track particle without Trk::Track");
      }else{
	const Trk::Track* cbtr = *cbtp->trackLink();
	if( cbtr ) sout << std::endl
			<< m_edmPrinter->printStations(*cbtr);
	if( cbtr && printMeasurements) sout << std::endl
			<< m_edmPrinter->printMeasurements(*cbtr);
      }
      sout << std::endl;
    }    
  }

  if (muon.inDetTrackParticleLink().isValid()){

    const xAOD::TrackParticle* idtp = *muon.inDetTrackParticleLink();
    if( idtp ){
      sout << " --- InDet  ---  " << print(*idtp);

      if( !idtp->trackLink().isValid() ){
	sout << " No Track link";
	ATH_MSG_DEBUG("ID track particle without Trk::Track");
      }else{
	// uint8_t numberOfInnermostPixelLayerHits=0;
	// uint8_t numberOfPixelHits=0;
	// uint8_t numberOfSCTHits=0;
	// uint8_t numberOfTRTHits=0;
	
	// if( !muon.summaryValue(numberOfInnermostPixelLayerHits,xAOD::numberOfInnermostPixelLayerHits) ) numberOfInnermostPixelLayerHits = 0;
	// if( !muon.summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ) numberOfPixelHits = 0;
	// if( !muon.summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ) numberOfSCTHits = 0;
	// if( !muon.summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits) ) numberOfTRTHits = 0;
	
	// sout << " Hits in B Layer " << static_cast<int>(numberOfInnermostPixelLayerHits)
	//      << "   Pixels " << static_cast<int>(numberOfPixelHits)
	//      << "   SCT " << static_cast<int>(numberOfSCTHits)
	//      << "   TRT " << static_cast<int>(numberOfTRTHits);
      }
      sout << std::endl;
    }
  }

  if( muon.extrapolatedMuonSpectrometerTrackParticleLink().isValid() ){
    const xAOD::TrackParticle* satp = *muon.extrapolatedMuonSpectrometerTrackParticleLink();
    if( satp ){
      sout << " --- Extrapolated Muon track ---  " << print(*satp);
      if( !satp->trackLink().isValid() ){
	sout << " No Track link";
	ATH_MSG_DEBUG("Extrapolated track particle without Trk::Track");
      }else{
	const Trk::Track* satr = *satp->trackLink();
	if( satr ) sout << std::endl
			<< m_edmPrinter->printStations(*satr);
	if( satr && printMeasurements) sout << std::endl
			<< m_edmPrinter->printMeasurements(*satr);
      }
      sout << std::endl;
    }
  }

  if( muon.muonSpectrometerTrackParticleLink().isValid() ){
    const xAOD::TrackParticle* satp = *muon.muonSpectrometerTrackParticleLink();
    if( satp ){
      sout << " --- MuonSpectrometer track ---  " << print(*satp);
      if( !satp->trackLink().isValid() ){
	sout << " No Track link";
	ATH_MSG_DEBUG("SA track particle without Trk::Track");
      }else{
	const Trk::Track* satr = *satp->trackLink();
	if( satr ) sout << std::endl
			<< m_edmPrinter->printStations(*satr);
	if( satr && printMeasurements) sout << std::endl
			<< m_edmPrinter->printMeasurements(*satr);
      }
      sout << std::endl;
    }
  }

  // if( !muon.muonSegments().empty() ){

  //   std::vector< ElementLink< Trk::SegmentCollection > > segments = muon.muonSegments();
  //   unsigned int nseg = segments.size();
  //   sout << " --- Segments: " << nseg << std::endl;
  //   for (unsigned int i = 0; i < nseg; ++i) {
  //     const Trk::Segment* tseg = *segments[i];
  //     const Muon::MuonSegment* seg  = dynamic_cast<const  Muon::MuonSegment* > (tseg);
  //     sout << "    " <<  (seg==NULL ? "not a MuonSegment!" : m_edmPrinter->print(*seg))
  // 	   << std::endl;
  //   }
  // }

  return sout.str();
} 


std::string Rec::MuonPrintingTool::print(const xAOD::MuonContainer& muons) const {


  std::ostringstream sout;
  sout << "Muon Container Size :" << muons.size() << std::endl;
    
  for( auto m : muons ){
    sout << print(*m);
  }
  return sout.str();

}
