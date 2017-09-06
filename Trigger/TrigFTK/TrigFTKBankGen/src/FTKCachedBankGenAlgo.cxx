/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKCachedBankGenAlgo.h"

#include "GaudiKernel/MsgStream.h"

#include "TrigFTKSim/FTK_CompressedAMBank.h"
// #include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKSetup.h"

#include <iostream>
#include <fstream>

#ifdef UNUSED
#undef UNUSED
#endif

using namespace std;
/////////////////////////////////////////////////////////////////////////////

FTKCachedBankGenAlgo::FTKCachedBankGenAlgo(const std::string& name, ISvcLocator* pSvcLocator):
   AthAlgorithm(name, pSvcLocator),
   m_pmap_path(""),
   m_rmap_path(""),
   m_modulelut_path(""),
   m_ssmapTSP_path(""),
   m_ssmapAM_path(""),
   m_in_bank_path(""),
   m_in_cache_path(""),
   m_out_ccache_path(""),
   m_out_pcache_path(""),
   m_hwmodeid_default(0), //! HWMODE input
   m_hwmodeid_tspcc(-1),
   m_hwmodeid_dc(-1),
   m_curreg(0), //! Region of interest
   m_nSubReg(0), //! number of subregions
   m_iSubReg(0), //! current subregion
   m_verbosity(1), //! for debugging
   m_maxpatt(0), //! max. number of patterns
   m_IBLMode(1), //! IBLMode
   m_ITkMode(false), //! ITkMode
   m_WhereToRun("initialize"),
   m_flatFormat(0),
   m_partitionList(""),
   m_nSectorMax(-1),
   m_nLamb(0),
   m_ndcMax(-1),
   m_wildcardPenalty(0)
{
   declareProperty("pmap_path", m_pmap_path);
   declareProperty("rmap_path", m_rmap_path);
   declareProperty("ModuleLUTPath",m_modulelut_path);
   declareProperty("ssmapTSP_path", m_ssmapTSP_path);
   declareProperty("ssmapAM_path", m_ssmapAM_path);

   declareProperty("infile", m_in_bank_path);
   declareProperty("cacheIn", m_in_cache_path);
   declareProperty("pcacheOut", m_out_pcache_path);
   declareProperty("ccacheOut", m_out_ccache_path);

   declareProperty("sectordefHW0",m_sectordefHW0);
   declareProperty("sectordefHW2",m_sectordefHW2);

   declareProperty("hwmodeid_tspcc", m_hwmodeid_tspcc);
   declareProperty("hwmodeid_dc", m_hwmodeid_dc);
   declareProperty("curreg", m_curreg);
   declareProperty("maxpatt", m_maxpatt);
   declareProperty("verbosity", m_verbosity);
   declareProperty("nSub", m_nSubReg);
   declareProperty("iSub", m_iSubReg);
   declareProperty("WhereToRun",m_WhereToRun);
   declareProperty("IBLMode",m_IBLMode);
   declareProperty("ITkMode",m_ITkMode);
   declareProperty("hwmodeid_default",m_hwmodeid_default);
   declareProperty("flatFormat",m_flatFormat);
   declareProperty("partitionList",m_partitionList);
   declareProperty("nSectorMax",m_nSectorMax);
   declareProperty("nLamb",m_nLamb);
   declareProperty("ndcMax",m_ndcMax);
   declareProperty("ndcMaxPlane",m_ndcMaxPlane);
   declareProperty("wildcardPenalty",m_wildcardPenalty);
   declareProperty("wildcardPenaltyPlane",m_wildcardPenaltyPlane);

   declareProperty("badmap_path",m_badmap_path);
   declareProperty("badmap_path_for_hit",m_badmap_path2);
}

FTKCachedBankGenAlgo::~FTKCachedBankGenAlgo() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FTKCachedBankGenAlgo::initialize() {

   // where to run the pattern generation?
   if ( !( TString(m_WhereToRun).BeginsWith("i",TString::kIgnoreCase) 
	   || TString(m_WhereToRun).BeginsWith("f",TString::kIgnoreCase) 
	   || TString(m_WhereToRun).BeginsWith("e",TString::kIgnoreCase)) ) {
      ATH_MSG_WARNING("FTKPattGenRootAlgo() "<< "WhereToRun must start with 'i', 'e' or 'f' referring to the three steps of an athena algorithm (initialize, execute, finalize), but is: "<<m_WhereToRun);
      return StatusCode::FAILURE;
   }   


   //ATH_MSG_INFO ("initialize()");
   if ( TString(m_WhereToRun).BeginsWith("i",TString::kIgnoreCase) ) return RunCachedBankGenerator();
   else return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FTKCachedBankGenAlgo::execute() {
   //ATH_MSG_INFO ("execute()");
   if ( TString(m_WhereToRun).BeginsWith("e",TString::kIgnoreCase) ) return RunCachedBankGenerator();
   else return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode FTKCachedBankGenAlgo::finalize() {
   //ATH_MSG_INFO ("finalize()");
   if ( TString(m_WhereToRun).BeginsWith("f",TString::kIgnoreCase) ) return RunCachedBankGenerator();
   else return StatusCode::SUCCESS;
}
 
 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FTKCachedBankGenAlgo::RunCachedBankGenerator(){
   // MsgStream log(msgSvc(), name());
   ATH_MSG_INFO("initialize() Make FTKCachedBankGenAlgo object.");

   // set HWMODEID
   FTKSetup &ftkset = FTKSetup::getFTKSetup();
   ftkset.setHWModeSS(m_hwmodeid_default);
   // set IBL mode
   ftkset.setIBLMode(m_IBLMode);
   // set ITk mode
   ftkset.setITkMode(m_ITkMode);
   // set debugging level
   ftkset.setVerbosity(m_verbosity);
   // set ITK mode
   ftkset.setITkMode(m_ITkMode);

   ATH_MSG_INFO(" Setting IBLMode="+TString::Format("%d",m_IBLMode));
   FTKSetup::getFTKSetup().setIBLMode(m_IBLMode);

   // --- check input
   if (m_ssmapAM_path.empty() ) {// check ss file
      ATH_MSG_FATAL("No AM-SS configuration file given");
      return StatusCode::FAILURE;
   }
   if (m_in_bank_path.empty() && m_in_cache_path.empty() ) {// check input file
      ATH_MSG_FATAL("No input file given");
      return StatusCode::FAILURE;
   }
   if (m_out_ccache_path.empty() &&  m_out_pcache_path.empty()) {// check ss file
      ATH_MSG_FATAL("No output file given");
      return StatusCode::FAILURE;
   }

   // --- get maps ---
   const bool force_am_hashmap = false;
   FTKPlaneMap* pmap = NULL;
   FTKRegionMap* rmap = NULL;
   FTKSSMap* ssmapTSP = NULL;


   // check whether maps are available in TSP-pattern-file
   ATH_MSG_INFO("Checking maps in input file.");
   FTKSSMap* ssmapTSPPatt = 0;
#ifdef UNUSED
   if(!m_in_bank_path.empty()) {
      ssmapTSPPatt=FTKSSMap::GetSSMapFromRootFile
         (FTKRootFile::Instance()->OpenRootFileReadonly
          (m_in_bank_path.c_str()),"TSP");
      if ( ssmapTSPPatt->getIsOK() ) {
         ATH_MSG_INFO("Using maps from input file.");
         ssmapTSP = ssmapTSPPatt;
         // pmap = ssmapTSP->getPlaneMap();
         // rmap = ssmapTSP->getRegionMap();
         pmap = ssmapTSP->getModifyablePlaneMap();
            //FTKPlaneMap::GetPMapFromRootFile(FTKRootFile::Instance()->OpenRootFileReadonly(m_in_bank_path.c_str()));
         rmap =ssmapTSP->getModifyableRegionMap();
         //FTKRegionMap::GetRMapFromRootFile(FTKRootFile::Instance()->OpenRootFileReadonly(m_in_bank_path.c_str()));
      } else {
         ATH_MSG_INFO("No meta-tags (pmap, ssmap, etc.) found in input file.");
      }
   }
#endif
   if(ssmapTSPPatt) delete ssmapTSPPatt;
   ssmapTSPPatt=0;

   // cross checks
   if ( !ssmapTSP ) {
      if (m_pmap_path.empty()) { // check pmap file
	 ATH_MSG_FATAL("Main plane map definition missing");
	 return StatusCode::FAILURE;
      } else {
	 // --- Create the pmap file object
	 ATH_MSG_INFO("initialize() Make FTKPlaneMap.");
	 pmap = new FTKPlaneMap(m_pmap_path.c_str());
	 if (!(*pmap)) {
	    ATH_MSG_FATAL("Error using plane map: "+m_pmap_path);
	    return StatusCode::FAILURE;
	 }
      }
      if (m_rmap_path.empty()) { // check rmap file
	 ATH_MSG_FATAL("Region map file needed");
	 return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("initialize() Make FTKRegionMap.");
      rmap = new FTKRegionMap(pmap, m_rmap_path.c_str());

      if (m_ssmapTSP_path.empty() ) {// check ss file
	 ATH_MSG_FATAL("No TSP-SS configuration file given");
	 return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("initialize() Make FTKSSMapTSP.");
      ssmapTSP = new FTKSSMap(rmap, m_ssmapTSP_path.c_str(), force_am_hashmap);
   } else {
      // cross check maps
      if ( !m_pmap_path.empty() && !m_rmap_path.empty() && !m_ssmapTSP_path.empty() ) { // check pmap file
	 // --- Create the pmap file object
	 FTKPlaneMap* pmap2 = new FTKPlaneMap(m_pmap_path.c_str());
	 if (!(*pmap2)) {
	    ATH_MSG_FATAL("Error using plane map: "+m_pmap_path);
	    return StatusCode::FAILURE;
	 }
	 if ( pmap->CalcChecksum() != pmap2->CalcChecksum() ) {
	    ATH_MSG_FATAL("The given plane-map does not fit to the TSP pattern.");
	    return StatusCode::FAILURE;
	 }

	 // check rmap file
	 FTKRegionMap* rmap2 = new FTKRegionMap(pmap, m_rmap_path.c_str());
	 if ( rmap2->CalcChecksum() != rmap->CalcChecksum() ) {
            ATH_MSG_FATAL("The given region-map does not fit to the TSP pattern.");
            return StatusCode::FAILURE;
         }
	 // check tsp ss file
	 FTKSSMap* ssmapTSP2 = new FTKSSMap(rmap, m_ssmapTSP_path.c_str(), force_am_hashmap);
	 if ( ssmapTSP->CalcChecksum() != ssmapTSP2->CalcChecksum() ) {
            ATH_MSG_FATAL("The given TSP ss-map does not fit to the TSP pattern.");
            return StatusCode::FAILURE;
         }
	 delete ssmapTSP;
         ssmapTSP=ssmapTSP2;
	 //delete rmap2;
	 //delete pmap2;
      }
   }
   //ATH_MSG_INFO("initialize() checking module LUT");
   if((m_hwmodeid_default==2)||(m_hwmodeid_tspcc==2)||(m_hwmodeid_dc==2)) {
      if(!rmap->isModuleIDLUTloaded()) {
         if(m_modulelut_path.empty()) {
            ATH_MSG_FATAL("A module LUT is required when HW SS calculation is required");
         } else {
            ATH_MSG_INFO("Loading module LUT from file \""+
                         m_modulelut_path+"\"");
            rmap->loadModuleIDLUT(m_modulelut_path.c_str());
         }
      }
   }
   //exit(0);
   // --- ---

   // --- Create new AM-ssmap
   //ATH_MSG_INFO("initialize() Make FTKSSMapAM.");
   FTKSSMap* ssmapAM  = new FTKSSMap(rmap, m_ssmapAM_path.c_str(), force_am_hashmap);

   // ftk setup
   

   // --- create FTK_CompressedAMBank
   //ATH_MSG_INFO("Create FTK_CompressedAMBank");
   ATH_MSG_INFO(" + default HWMODEID (used if -1 is specified): "
                +TString::Format("%d",m_hwmodeid_default));
   ATH_MSG_INFO(" + HWMODEID of input/output pcache DC bank: "
                +TString::Format("%d",m_hwmodeid_dc));
   ATH_MSG_INFO(" + HWMODEID of input/output tsp/ccache DC bank: "
                +TString::Format("%d",m_hwmodeid_tspcc));
   //  FTKPattGenRoot pattgen(pmap,rmap,m_curreg,ssmap,sectorslice,constbank);

   FTK_CompressedAMBank bank(m_curreg,m_iSubReg,ssmapAM,ssmapTSP,
                             m_hwmodeid_tspcc,m_hwmodeid_dc);

   if(bank.getHWModeSS_dc() != bank.getHWModeSS_tsp()) {
      if(m_sectordefHW0.empty() || m_sectordefHW2.empty()) {
	 ATH_MSG_FATAL("Different HWMODEID for TSP and DC bank but sector definition files not given");
      } else {
	 bank.readSectorDefinition(m_sectordefHW0.c_str(),m_sectordefHW2.c_str());
      }
   }
   ostringstream msg;
   msg<<"HWMODEID="<<bank.getHWModeSS_tsp()
      <<" on TSP level (sectorordered/ccache bank) and HWMODEID="
      <<bank.getHWModeSS_dc()<<" on DC level (pcache bank)";
   ATH_MSG_INFO(msg.str().c_str());

   // set TSP import parameters
   if(m_ndcMax>=0) {
      bank.setNDCmax(m_ndcMax);
   }
   if(m_wildcardPenalty) {
      bank.setWildcardPenalty(m_wildcardPenalty);
   }
   for(size_t plane=0;plane<m_ndcMaxPlane.size();plane++) {
      bank.setNDCmaxPlane(plane,m_ndcMaxPlane[plane]);
   }
   for(size_t plane=0;plane<m_wildcardPenaltyPlane.size();plane++) {
      bank.setWildcardPenaltyPlane(plane,m_wildcardPenaltyPlane[plane]);
   }

   // set list of bad modules
   bank.setBadSSMapPath(m_badmap_path);
   bank.setBadSSMapPath2(m_badmap_path2);

   if(!m_in_cache_path.empty()) {
      // read only one subregion (or whatever is in that file)
      bank.readROOTBankCache(m_in_cache_path.c_str());
   } else if(!m_partitionList.empty()) {
      bank.readSectorOrderedBank(m_in_bank_path.c_str(),
                                 m_partitionList.c_str(),
                                 m_nSubReg,
                                 m_nSectorMax,
                                 m_nLamb);
   } else {
      bank.readSectorOrderedBank(m_in_bank_path.c_str(),
                                 m_maxpatt,
                                 m_nSubReg,
                                 m_nSectorMax,
                                 m_nLamb);
   }
   if(!m_out_ccache_path.empty())
      bank.writeCCachedBankFile(m_out_ccache_path.c_str(),m_flatFormat);
   if(!m_out_pcache_path.empty()) {
      bank.writePCachedBankFile(m_out_pcache_path.c_str(),m_flatFormat,
                                m_nSubReg,m_nLamb);
   }

   // Important: 'return' deletes the  FTKPattGenRoot object and thus writes the patterns to disk
   return StatusCode::SUCCESS;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


