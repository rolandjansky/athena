/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/PattMergeRootAlgo.h"
#include "TString.h"
#include "TrigFTKSim/FTKMergeRoot.h"
#include "TrigFTKSim/FTKSSMap.h"

// #include "TrigFTKSim/FTKRootFile.h"
// #include "TrigFTKSim/FTKPatternBySector.h"
// #include "TrigFTKSim/FTKPatternWithCoverage.h"

using namespace std;
/////////////////////////////////////////////////////////////////////////////

PattMergeRootAlgo::PattMergeRootAlgo(const std::string& name, ISvcLocator* pSvcLocator):
   AthAlgorithm(name, pSvcLocator), m_MinCov(0),m_Compression(-1),
   m_ssmap(0),m_hwmodeid(-1),m_curreg(-1)
{
   SetLogger(this);
   declareProperty("MinCoverage",m_MinCov);
   declareProperty("Compression",m_Compression);
   declareProperty("TextImportRootFile",m_TextImportRootFile);
   declareProperty("RootOutFile",m_RootOutFile);
   declareProperty("TextOutFile",m_TextOutFile);
   declareProperty("NSub",m_NSub);
   declareProperty("InputFiles",m_InputFiles, "Input files.");
   declareProperty("WhereToRunMerging",m_WhereToRunMerging);
   declareProperty("DeleteFilesAfterMerging",m_DeleteAfterMerging);
   declareProperty("ssmap_path", m_ssmap_path);
   declareProperty("hwmodeid", m_hwmodeid);
   declareProperty("pmap_path", m_pmap_path);
   declareProperty("rmap_path", m_rmap_path);
   declareProperty("ModuleLUTPath", m_modulelut_path);
   declareProperty("curreg", m_curreg);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


PattMergeRootAlgo::~PattMergeRootAlgo() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PattMergeRootAlgo::initialize() {
 
   if ( !( TString(m_WhereToRunMerging).BeginsWith("i") 
	   || TString(m_WhereToRunMerging).BeginsWith("f") 
	   || TString(m_WhereToRunMerging).BeginsWith("e")) ) {
      ATH_MSG_WARNING("PattMergeRootAlgo() "<< "WhereToRunMerging must start with 'i', 'e' or 'f' referring to the three steps of an athena algorithm (initialize, execute, finalize), but is: "<<m_WhereToRunMerging);
      return StatusCode::FAILURE;
   }   

   SetLogger(this);
   ATH_MSG_INFO ("initialize()");
   if ( TString(m_WhereToRunMerging).BeginsWith("i") ) return RunMerging();
   else return StatusCode::SUCCESS;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode PattMergeRootAlgo::execute() {
   ATH_MSG_INFO ("execute()");
   if ( TString(m_WhereToRunMerging).BeginsWith("e") ) return RunMerging();
   else return StatusCode::SUCCESS;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode PattMergeRootAlgo::finalize() {
   ATH_MSG_INFO ("finalize()");
   if ( TString(m_WhereToRunMerging).BeginsWith("f") ) return RunMerging();
   else return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode PattMergeRootAlgo::RunMerging() {
   //! Do merging.
   int error=0;
   FTKMergeRoot merger(m_RootOutFile);
   merger.SetDeleteFilesAfterMerging(m_DeleteAfterMerging);
   // set text-import name if available
   if ( m_TextImportRootFile!="" && TString(m_TextImportRootFile).EndsWith(".root") )
      merger.SetTempRootFileName(m_TextImportRootFile);
   else if ( m_TextImportRootFile!="" && TString(m_TextImportRootFile).EndsWith(".root") )
      ATH_MSG_WARNING("TextImportRootFile does not end with .root. Using default TextImportName instead for temporary file.");
   // add files to merger
   merger.AddFiles(m_InputFiles);
   // set number of sub regions (only supported by 'text-output')
   //ATH_MSG_INFO ("RunMerging(). SetNSubregions: "<<m_NSub);
   merger.SetNSubregions(m_NSub);

   // HWMODEID for checking input files
   if(m_hwmodeid>=0) {
      FTKSetup &ftkset = FTKSetup::getFTKSetup();
      ftkset.setHWModeSS(m_hwmodeid);
      ATH_MSG_INFO ("HWMODEID="<<m_hwmodeid);
   }
   // SSMAP for checking input files
   FTKPlaneMap *pmap=0;
   FTKRegionMap *rmap=0;
   ATH_MSG_INFO ("pmap_path = \""<<m_pmap_path<<"\"");
   ATH_MSG_INFO ("rmap_path = \""<<m_rmap_path<<"\"");
   ATH_MSG_INFO ("ssmap_path = \""<<m_ssmap_path<<"\"");
   if(!m_pmap_path.empty()) {
      pmap = new FTKPlaneMap(m_pmap_path.c_str());
   } else {
      ATH_MSG_INFO ("no plane map specified");
   }
   if(pmap) {
      if(!m_rmap_path.empty() ) {
         rmap = new FTKRegionMap(pmap, m_rmap_path.c_str());
      } else {
         ATH_MSG_INFO ("no tower map specified");
      }
   }
   if(rmap) {
      if(m_hwmodeid ==2) {
         if(!m_modulelut_path.empty()) {
            rmap->loadModuleIDLUT(m_modulelut_path.c_str());
         } else {
            ATH_MSG_ERROR ("no module LUT specified but hwmodeid==2");
         }
      } else {
         delete rmap;
         rmap=0;
      }
   }
   if(rmap) {
      if(!m_ssmap_path.empty()) {
         m_ssmap = new FTKSSMap(rmap, m_ssmap_path.c_str(),false);
      } else {
         ATH_MSG_INFO ("no SS map specified");
      }
   }

   // do merging
   error += merger.DoMerge(m_MinCov,m_Compression,
                           m_ssmap,m_curreg,m_hwmodeid);
   // write out in addition ascii-file if output file did not ended with '.root'
   if ( m_TextOutFile!="" && !TString(m_TextOutFile).EndsWith(".root") ) 
      merger.DoTextExport(m_TextOutFile);
   //done
   return error>0? StatusCode::FAILURE : StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


void PattMergeRootAlgo::PostMessage(void) {
   if(FTKLogger::m_type==0)  ATH_MSG_FATAL(m_buffer->str());
   else if(FTKLogger::m_type==1)  ATH_MSG_ERROR(m_buffer->str());
   else if(FTKLogger::m_type==2)  ATH_MSG_WARNING(m_buffer->str());
   else if(FTKLogger::m_type==3)  ATH_MSG_INFO(m_buffer->str());
   else if(FTKLogger::m_type==4)  ATH_MSG_DEBUG(m_buffer->str());
}


// // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
