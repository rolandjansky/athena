/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKPattGenRootAlgo.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTKBankGen/FTKSectorSlice.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKSetup.h"
#include "CLHEP/Random/RandomEngine.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <TMath.h>
#include <boost/unordered_map.hpp>
#include <list>
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/FTKPatternWithCoverage.h"
#include "AthenaKernel/IAtRndmGenSvc.h" // random numbers

using namespace std;
/////////////////////////////////////////////////////////////////////////////

FTKPattGenRootAlgo::FTKPattGenRootAlgo(const std::string& name, ISvcLocator* pSvcLocator):
   AthAlgorithm(name, pSvcLocator),
   m_rndmSvc("AtRanluxGenSvc", name),
   // m_pmap(0x0),
   // m_rmap(0x0),
   // m_ssmap(0x0), m_ssmap_tsp(0x0),
   // m_constbank(0x0),
   // m_sectorslice(0x0),
   // m_force_am_hashmap(true),
   m_ssmap_path(),
   //m_ssmaptsp_path(),
   m_pmap_path(),
   m_rmap_path(),
   m_modulelut_path(),
   m_slices_path(),
   m_fitconstants_path(),
   m_sectors_path(),
   m_IBLMode(1), // flag to change the IBLMode
   m_fixEndcapL0(false), 
   m_ITkMode(false),
   m_rndStreamName("RANDOM"),
   // set vector dimension to 2 for ranges
   m_slice_phiRange(0.,0.), m_slice_cRange(0.,0.), m_slice_d0Range(0.,0.),
   m_slice_z0Range(0.,0.), m_slice_cotRange(0.,0.),
   m_track_phiRange(0.,0.),m_track_cRange(0.,0.),m_track_d0Range(0.,0.),
   m_track_z0Range(0.,0.),m_track_etaRange(0.,0.),
   m_usePhiSlice(true),m_useCSlice(false),m_useD0Slice(true),
   m_useZ0Slice(true),m_useCotSlice(true),
   m_beamspotX(0.),m_beamspotY(0.),
   m_keep7of8(0),
   m_tolerance7of8(0.1),

   m_sectorSelection(FTKPattGenRoot::SectorSelection::ModuleGeometry),

   m_pattgen(0),
   m_totalPatterns(0),m_totalThrown(0)
{
   SetLogger(this);
   declareProperty("ssmap_path", m_ssmap_path);
   declareProperty("hwmodeid", m_HWMODEID);
   //declareProperty("ssmaptsp_path", m_ssmaptsp_path);
   declareProperty("pmap_path", m_pmap_path);
   declareProperty("rmap_path", m_rmap_path);
   declareProperty("ModuleLUTPath", m_modulelut_path);
   declareProperty("slices", m_slices_path);
   declareProperty("fitconsts", m_fitconstants_path);
   declareProperty("sectors", m_sectors_path);
   // declareProperty("dbghit_path", m_dbghit_path);
   declareProperty("slice_phi_min", m_slice_phiRange.first);
   declareProperty("slice_phi_max", m_slice_phiRange.second);
   declareProperty("slice_c_min", m_slice_cRange.first);
   declareProperty("slice_c_max", m_slice_cRange.second);
   declareProperty("slice_d0_min", m_slice_d0Range.first);
   declareProperty("slice_d0_max", m_slice_d0Range.second);
   declareProperty("slice_z0_min", m_slice_z0Range.first);
   declareProperty("slice_z0_max", m_slice_z0Range.second);
   declareProperty("slice_cot_min", m_slice_cotRange.first);
   declareProperty("slice_cot_max", m_slice_cotRange.second);
   declareProperty("track_phi_min", m_track_phiRange.first);
   declareProperty("track_phi_max", m_track_phiRange.second);
   declareProperty("track_c_min", m_track_cRange.first);
   declareProperty("track_c_max", m_track_cRange.second);
   declareProperty("track_d0_min", m_track_d0Range.first);
   declareProperty("track_d0_max", m_track_d0Range.second);
   declareProperty("track_z0_min", m_track_z0Range.first);
   declareProperty("track_z0_max", m_track_z0Range.second);
   declareProperty("track_eta_min", m_track_etaRange.first);
   declareProperty("track_eta_max", m_track_etaRange.second);
   declareProperty("usePhiSlice",m_usePhiSlice);
   declareProperty("useCSlice",m_useCSlice);
   declareProperty("useD0Slice",m_useD0Slice);
   declareProperty("useZ0Slice",m_useZ0Slice);
   declareProperty("useCotSlice",m_useCotSlice);

   declareProperty("beamspotX", m_beamspotX);
   declareProperty("beamspotY", m_beamspotY);
   declareProperty("rndmSvc", m_rndmSvc);
   declareProperty("trials", m_trials);
   declareProperty("curreg", m_curreg);
   declareProperty("pconst_overlap", m_overlap);
   declareProperty("flat_in_cot", m_flat_in_cot);
   declareProperty("OutputFile",m_OutputFile);
   declareProperty("sectorSelection", m_sectorSelection);

   declareProperty("IBLMode",m_IBLMode);
   declareProperty("FixEndcapL0",m_fixEndcapL0);
   declareProperty("ITkMode",m_ITkMode);
   declareProperty("keep7of8",m_keep7of8);
   declareProperty("tolerance7of8",m_tolerance7of8);
   declareProperty("rndStreamName",m_rndStreamName);

   declareProperty("ModuleGeometryFile",m_ModuleGeometryFile);

#ifdef DEBUG_BEAMSPOT
  m_propagator = new ToolHandle<Trk::IPropagator>
     ( "Trk::RungeKuttaPropagator/DefaultPropagator");
#endif
}

FTKPattGenRootAlgo::~FTKPattGenRootAlgo() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FTKPattGenRootAlgo::initialize() {

#ifdef DEBUG_BEAMSPOT
   if( (*m_propagator).retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Propagator tool not found");
      return StatusCode::FAILURE;
   }   
#endif

   MsgStream log(msgSvc(), name());
   
   // --- check input
   if (m_pmap_path.empty()) { // check pmap file
      log << MSG::FATAL << "Main plane map definition missing" << endmsg;
      return StatusCode::FAILURE;
   }
   if (m_rmap_path.empty()) { // check rmap file
      log << MSG::FATAL << "Region map file needed" << endmsg;
      return StatusCode::FAILURE;
   }
   if (m_ssmap_path.empty() ) {// check ss file
      log << MSG::FATAL << "No SS configuration file given" << endmsg;
      return StatusCode::FAILURE;
   }
   if (m_slices_path.empty()) { // check slices file
      log << MSG::FATAL << "Slices file missing" << endmsg;
      return StatusCode::FAILURE;
   }
   if (m_fitconstants_path.empty()) { // check constants file
     log << MSG::FATAL << "Fit constants bank missing" << endmsg;
     return StatusCode::FAILURE;
   }
   if (m_sectors_path.empty()) {
      log << MSG::FATAL << "Sector file missing" << endmsg;
     return StatusCode::FAILURE;
   }
   // --- ---

   // --- set HWMODEID
   FTKSetup &ftkset = FTKSetup::getFTKSetup();
   ftkset.setHWModeSS(m_HWMODEID);
   ftkset.setIBLMode(m_IBLMode);
   ftkset.setfixEndcapL0(m_fixEndcapL0);
   ftkset.setITkMode(m_ITkMode);
   log << MSG::INFO <<"HWMODEID="<<m_HWMODEID
       <<" IBLMode="<<m_IBLMode<<" fixEndcapL0="<<m_fixEndcapL0
       <<" ITKMode="<<m_ITkMode<<"\n";

   
   // --- Create the pmap file object
      log << MSG::INFO << "make FTKPlaneMap "<<m_pmap_path << endmsg;
   FTKPlaneMap* pmap = new FTKPlaneMap(m_pmap_path.c_str());
   if (!(*pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endmsg;
      return StatusCode::FAILURE;
   }

   // --- Create region map object
   log << MSG::INFO << "Make FTKRegionMap "<<m_rmap_path << endmsg;
   FTKRegionMap* rmap = new FTKRegionMap(pmap, m_rmap_path.c_str());

   if(m_HWMODEID==2) {
      if(m_modulelut_path.empty()) {
         log << MSG::FATAL <<"A module LUT is required when HW SS calculation is required"<<endmsg;
	 return StatusCode::FAILURE;
      } else {
         log << MSG::INFO << "Load LUT "<<m_modulelut_path<<endmsg;
         rmap->loadModuleIDLUT(m_modulelut_path.c_str());
      }
   }

   // --- Create ssmap
   const bool force_am_hashmap = false;
   log << MSG::INFO << "Make FTKSSMap "<<m_ssmap_path << endmsg;
   FTKSSMap* ssmap = new FTKSSMap(rmap, m_ssmap_path.c_str(), force_am_hashmap);

   // --- Create the slices file object
   log << MSG::INFO << "Read FTKSectorSlice from : "
       <<m_slices_path << endmsg;
   log<< MSG::INFO << "Job option slice parameters:"
      <<"  phi: [ "<<m_slice_phiRange.first<<" ; "<<m_slice_phiRange.second
      <<" ]  c: [ "<<m_slice_cRange.first<<" ; "<<m_slice_cRange.second
      <<" ]  d0: ["<<m_slice_d0Range.first<<" ; "<<m_slice_d0Range.second
      <<" ]  z0: ["<<m_slice_z0Range.first<<" ; "<<m_slice_z0Range.second
      <<"< ]  cot(theta): [ "<<m_slice_cotRange.first<<" ; "<<m_slice_cotRange.second
      <<" ]"<<endmsg;
   FTKSectorSlice* sectorslice = new FTKSectorSlice();
   if(!sectorslice->loadSlices(m_slices_path,m_slice_phiRange,m_slice_cRange,
                               m_slice_d0Range,m_slice_z0Range,m_slice_cotRange)) {
      log << MSG::FATAL << "Could not load slices"<< endmsg;
   }
   log<< MSG::INFO << "Slice parameters really used:"
      <<"  phi: [ "<<m_slice_phiRange.first<<" ; "<<m_slice_phiRange.second
      <<" ]  c: [ "<<m_slice_cRange.first<<" ; "<<m_slice_cRange.second
      <<" ]  d0: ["<<m_slice_d0Range.first<<" ; "<<m_slice_d0Range.second
      <<" ]  z0: ["<<m_slice_z0Range.first<<" ; "<<m_slice_z0Range.second
      <<"< ]  cot(theta): [ "<<m_slice_cotRange.first<<" ; "<<m_slice_cotRange.second
      <<" ]"<<endmsg;
   
   log<< MSG::INFO << "Slices to be used for the sector selection "
      <<" phi="<<m_usePhiSlice
      <<" c="<<m_useCSlice
      <<" d0="<<m_useD0Slice
      <<" z0="<<m_useZ0Slice
      <<" cot="<<m_useCotSlice<<endmsg;
   sectorslice->selectSlices
      (m_usePhiSlice,m_useCSlice,m_useD0Slice,m_useZ0Slice,m_useCotSlice);

   // --- create  FTKConstantBank
   log << MSG::INFO << "Make FTKConstantBank "
       <<m_fitconstants_path << endmsg;
   FTKConstantBank* constbank = new FTKConstantBank(pmap->getTotalDim(), m_fitconstants_path.c_str());

  // --- create pattgen object
  log << MSG::INFO << "Make FTKPattGenRoot "
      <<" keep7of8="<<m_keep7of8<<" tolerance7of8="<<m_tolerance7of8
      << endmsg;

  m_pattgen=new FTKPattGenRoot(m_curreg,ssmap,sectorslice,constbank,m_keep7of8,
                               m_tolerance7of8
#ifdef DEBUG_BEAMSPOT
                               ,m_propagator
#endif
                               );
  log << MSG::INFO << "beam spot at "
      <<m_beamspotX<<" "<<m_beamspotY
      << endmsg;
  m_pattgen->SetRandomNumberGenerator(m_rndmSvc->GetEngine(m_rndStreamName));
  m_pattgen->setBeamspot(m_beamspotX,m_beamspotY);
  log << MSG::INFO << "Read sector path "<<m_sectors_path<<endmsg;
  m_pattgen->ReadSectorFile(m_sectors_path); // set sectors path
  log<< MSG::INFO << "Job option track parameter ranges:"
     <<"  phi: [ "<<m_track_phiRange.first<<" ; "<<m_track_phiRange.second
     <<" ]  c: [ "<<m_track_cRange.first<<" ; "<<m_track_cRange.second
     <<" ]  d0: ["<<m_track_d0Range.first<<" ; "<<m_track_d0Range.second
     <<" ]  z0: ["<<m_track_z0Range.first<<" ; "<<m_track_z0Range.second
     <<" ]  eta: ["<<m_track_etaRange.first<<" ; "<<m_track_etaRange.second
     <<" ]"<<endmsg;
  m_pattgen->SetTrackParameterRange
     (m_track_phiRange,m_track_cRange,m_track_d0Range,m_track_z0Range,
      m_track_etaRange);
  log<< MSG::INFO << "Track parameter ranges really used:"
     <<"  phi: [ "<<m_track_phiRange.first<<" ; "<<m_track_phiRange.second
     <<" ]  c: [ "<<m_track_cRange.first<<" ; "<<m_track_cRange.second
     <<" ]  d0: ["<<m_track_d0Range.first<<" ; "<<m_track_d0Range.second
     <<" ]  z0: ["<<m_track_z0Range.first<<" ; "<<m_track_z0Range.second
     <<" ]  eta: ["<<m_track_etaRange.first<<" ; "<<m_track_etaRange.second
     <<" ]"<<endmsg;

  log<< MSG::INFO <<"Generate patterns flat in cot(theta): "<<m_flat_in_cot<<endmsg;
  m_pattgen->SetFlatInCot(m_flat_in_cot);

  log<< MSG::INFO <<"Overlap removal: "<<m_overlap<<endmsg;
  m_pattgen->SetOverlapRemoval(m_overlap);
  log<<MSG::INFO <<"Module boundary check: "<<m_sectorSelection
     <<" module position file: \""<<m_ModuleGeometryFile<<"\""<<endmsg;
  m_pattgen->SetModuleGeometryCheck
     (m_ModuleGeometryFile,(FTKPattGenRoot::SectorSelection)m_sectorSelection);

  // open output file
  log<<MSG::INFO <<"Output file: "<<m_OutputFile<<endmsg;
  m_pattgen->SetRootOutput(m_OutputFile); 

  return StatusCode::SUCCESS;
} 


// // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FTKPattGenRootAlgo::execute() {
   uint64_t n=m_pattgen->trackInvertion(m_trials);
   m_totalPatterns+=m_trials;
   m_totalThrown+=n;
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Number of trials: "
       <<m_totalPatterns<<" / "<<m_totalThrown<<endmsg;
   
   return StatusCode::SUCCESS;
}


// // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode FTKPattGenRootAlgo::finalize() {
   delete m_pattgen;
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Total number of patterns: "
       <<m_totalPatterns<<" / "<<m_totalThrown<<endmsg;
   return StatusCode::SUCCESS;
}

 
// // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


void FTKPattGenRootAlgo::PostMessage(void) {
   int fType=getLoggerMsgType();
   if     (fType==0)  ATH_MSG_FATAL(getLoggerMsg());
   else if(fType==1)  ATH_MSG_ERROR(getLoggerMsg());
   else if(fType==2)  ATH_MSG_WARNING(getLoggerMsg());
   else if(fType==3)  ATH_MSG_INFO(getLoggerMsg());
   else if(fType==4)  ATH_MSG_DEBUG(getLoggerMsg());
}


