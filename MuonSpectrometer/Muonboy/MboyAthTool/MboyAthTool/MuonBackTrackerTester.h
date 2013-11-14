/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonBackTrackerTester_H
#define MuonBackTrackerTester_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyAthToolHelper;

#include "MboyAthToolInterfaces/IMuonBackTracker.h"

  /**
   @class MuonBackTrackerTester

  @author samusog@cern.ch
  
  */

class MuonBackTrackerTester:public AthAlgorithm {
public:
    MuonBackTrackerTester(const std::string& name, ISvcLocator* pSvcLocator);
   ~MuonBackTrackerTester();
   
public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////
//Data

   std::string m_TrackCollectionLocationIN              ; //!< Location of the input  TrackCollection
   std::string m_TrackCollectionLocationOUT             ; //!< Location of the output TrackCollection
   std::string m_TrackParticleCollectionLocationOUT     ; //!< Location of the output TrackParticleCollection

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper
   ToolHandle< Muon::IMuonBackTracker > p_IMuonBackTracker ; //!< Pointer On Muon::IMuonBackTracker

   void DumpContainers() ;

   std::string m_MboyDumpMboyBackTrackerOutputFile ;  
   std::ofstream  m_OutFile_MboyDumpMboyBackTrackerOutputFile  ;



};

#endif

