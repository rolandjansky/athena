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
   // m_dbghit_path(),
   // m_ifile(),
   // m_okread(false),
   // m_ready(false),
   // m_phi_min(),
   // m_phi_max(),
   // m_c_min(),
   // m_c_max(),
   // m_d0_min(),
   // m_d0_max(),
   // m_z0_min(),
   // m_z0_max(),
   // //m_ctheta_min(),
   // //m_ctheta_max(),
   // m_eta_min(),
   // m_eta_max(),
   m_rndStreamName("RANDOM"),
   m_d0_alpha(0.),
   //m_pEng(0),
   // m_trials(0),
   // m_track(),
   // m_fin(),
   // m_pattern_counter(0),
   // m_npatterns_partial(0),
   // m_emptySectorList(0),
   // m_notAnyGoodSector(0),
   // m_ss(0),
   // m_dimspa(0),
   // m_dimker(0),
   // m_curreg(0),
   // m_seed(0),
   //m_PCONST_OVERLAP(0),
   // m_rndGen(0x0),
   // m_nplanes(0),
   // m_nsectors(0),
   // m_constr(0x0),
   // m_smear(),
   // m_rnd_sector_selection(),
   // m_weighted_rnd_sector_selection(),
   // m_no_sector_selection(),
   // m_rnd_pattern_selection(),
   // m_weighted_rnd_pattern_selection(),
   // m_hitVector(0x0),
   // m_npatterns_reproduced(0),
   // m_npatterns_reproduced_partial(0),
   // m_npatterns_merged(0),
   // m_sectorbank(0x0),
   // m_pattmap(0x0), m_curpatt(0x0),
   // m_found_patterns(),
   // m_sectors_covInt(0x0)
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
   declareProperty("phi_min", m_phi_min);
   declareProperty("phi_max", m_phi_max);
   declareProperty("c_min", m_c_min);
   declareProperty("c_max", m_c_max);
   declareProperty("d0_min", m_d0_min);
   declareProperty("d0_max", m_d0_max);
   declareProperty("z0_min", m_z0_min);
   declareProperty("z0_max", m_z0_max);
   declareProperty("eta_min", m_eta_min);
   declareProperty("eta_max", m_eta_max);
   declareProperty("d0_alpha", m_d0_alpha);
   declareProperty("beamspotX", m_beamspotX);
   declareProperty("beamspotY", m_beamspotY);
   //declareProperty("ctheta_min", m_ctheta_min);
   //declareProperty("ctheta_max", m_ctheta_max);
   declareProperty("rndmSvc", m_rndmSvc);
   declareProperty("trials", m_trials);
   // declareProperty("writeafternpatterns", m_writeafter);
   // declareProperty("maxopenrootfiles", m_maxopenfiles);
   declareProperty("curreg", m_curreg);
   declareProperty("pconst_overlap", m_overlap);
   declareProperty("OutputFile",m_OutputFile);
   declareProperty("sectorSelection", m_sectorSelection);

   // declareProperty("smear", m_smear);
   // declareProperty("rnd_sector_selection", m_rnd_sector_selection);
   // declareProperty("weighted_rnd_sector_selection", m_weighted_rnd_sector_selection);
   // declareProperty("no_sector_selection", m_no_sector_selection);
   // declareProperty("rnd_pattern_selection", m_rnd_pattern_selection);
   // declareProperty("weighted_rnd_pattern_selection", m_weighted_rnd_pattern_selection);
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
   
   // --- Create the pmap file object
   log << MSG::INFO << "RunPattGen() Make FTKPlaneMap." << endmsg;
   FTKPlaneMap* pmap = new FTKPlaneMap(m_pmap_path.c_str());
   if (!(*pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endmsg;
      return StatusCode::FAILURE;
   }

   // --- Create region map object
   log << MSG::INFO << "RunPattGen() Make FTKRegionMap." << endmsg;
   FTKRegionMap* rmap = new FTKRegionMap(pmap, m_rmap_path.c_str());

   if(m_HWMODEID==2) {
      if(m_modulelut_path.empty()) {
         log << MSG::FATAL <<"RunPattGen() A module LUT is required when HW SS calculation is required"<<endmsg;
	 return StatusCode::FAILURE;
      } else {
         rmap->loadModuleIDLUT(m_modulelut_path.c_str());
      }
   }

   // --- Create ssmap
   const bool force_am_hashmap = false;
   log << MSG::INFO << "RunPattGen() Make FTKSSMap." << endmsg;
   FTKSSMap* ssmap = new FTKSSMap(rmap, m_ssmap_path.c_str(), force_am_hashmap);

   // --- Create the slices file object
   log << MSG::INFO << "RunPattGen() Make FTKSectorSlice." << endmsg;
   FTKSectorSlice* sectorslice = new FTKSectorSlice();
   sectorslice->loadSlices(m_slices_path);

   // --- create  FTKConstantBank
   log << MSG::INFO << "RunPattGen() Make FTKConstantBank." << endmsg;
   FTKConstantBank* constbank = new FTKConstantBank(pmap->getTotalDim(), m_fitconstants_path.c_str());

  // --- create pattgen object
  log << MSG::INFO << "RunPattGen() Make FTKPattGenRoot."
      << endmsg;

  m_pattgen=new FTKPattGenRoot(m_curreg,ssmap,sectorslice,constbank,m_keep7of8,
                               m_tolerance7of8
#ifdef DEBUG_BEAMSPOT
                               ,m_propagator
#endif
                               );
  log << MSG::INFO << "RunPattGen() beam spot at "
      <<m_beamspotX<<" "<<m_beamspotY
      << endmsg;
  m_pattgen->SetRandomNumberGenerator(m_rndmSvc->GetEngine(m_rndStreamName));
  m_pattgen->setBeamspot(m_beamspotX,m_beamspotY);
  m_pattgen->ReadSectorFile(m_sectors_path); // set sectors path
  m_pattgen->SetSliceParameters(m_phi_min,m_phi_max, m_c_min, m_c_max,
			     m_d0_min, m_d0_max, m_z0_min, m_z0_max, m_eta_min, m_eta_max);
  m_pattgen->SetD0Exponent(m_d0_alpha);
  m_pattgen->SetOverlapRemoval(m_overlap);
  m_pattgen->SetModuleGeometryCheck
     (m_ModuleGeometryFile,(FTKPattGenRoot::SectorSelection)m_sectorSelection);

  // open output file
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
   if     (FTKLogger::m_type==0)  ATH_MSG_FATAL(m_buffer->str());
   else if(FTKLogger::m_type==1)  ATH_MSG_ERROR(m_buffer->str());
   else if(FTKLogger::m_type==2)  ATH_MSG_WARNING(m_buffer->str());
   else if(FTKLogger::m_type==3)  ATH_MSG_INFO(m_buffer->str());
   else if(FTKLogger::m_type==4)  ATH_MSG_DEBUG(m_buffer->str());
}


