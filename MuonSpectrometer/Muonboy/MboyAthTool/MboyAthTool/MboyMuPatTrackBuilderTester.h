/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMuPatTrackBuilderTester_H
#define MboyMuPatTrackBuilderTester_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyAthToolHelper;

#include "MboyAthToolInterfaces/IMuonBackTracker.h"

  /**
   @class MboyMuPatTrackBuilderTester

  @author samusog@cern.ch
  
  */

class MboyMuPatTrackBuilderTester:public AthAlgorithm {
public:
    MboyMuPatTrackBuilderTester(const std::string& name, ISvcLocator* pSvcLocator);
   ~MboyMuPatTrackBuilderTester();
   
public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////
//Data

   std::string m_spectroTrackLocation ; //!< Location of the input 
   std::string m_spectroPartiLocation ; //!< Location of the input 
   std::string m_extrapTrackLocation  ; //!< Location of the input 
   std::string m_extrapPartiLocation  ; //!< Location of the input 

   std::string m_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation ;    
   std::string m_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation ;    
   std::string m_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation  ;    
   std::string m_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation  ;    

   std::ofstream m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroTrackLocation ;   
   std::ofstream m_OutFile_MboyDumpMboyBackTrackerOutputFilespectroPartiLocation ;   
   std::ofstream m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapTrackLocation  ;   
   std::ofstream m_OutFile_MboyDumpMboyBackTrackerOutputFileextrapPartiLocation  ;   

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

};

#endif

