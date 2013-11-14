/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMooSegmentCombinationFinderTester_H
#define MboyMooSegmentCombinationFinderTester_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyAthToolHelper;

#include "MboyEDMToolInterfaces/IMboyGetPrepDataFromEventTool.h"
#include "MboyAthToolInterfaces/IMboyMooSegmentCombinationFinder.h"

  /**
   @class MboyMooSegmentCombinationFinderTester

  @author samusog@cern.ch
  
  */

class MboyMooSegmentCombinationFinderTester:public AthAlgorithm {
public:
    MboyMooSegmentCombinationFinderTester(const std::string& name, ISvcLocator* pSvcLocator);
   ~MboyMooSegmentCombinationFinderTester();
   
public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////
//Data

   std::string m_SegmentCollectionLocation              ; //!< Location of the SegmentCollection

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

   ToolHandle< IMboyGetPrepDataFromEventTool > p_IMboyGetPrepDataFromEventTool ; //!< Pointer On IMboyGetPrepDataFromEventTool

   ToolHandle< Muon::IMboyMooSegmentCombinationFinder > p_IMboyMooSegmentCombinationFinder ; //!< Pointer On Muon::IMboyMooSegmentCombinationFinder


   /**Counters */
   int m_EvtTot                 ;
   std::vector<int>  m_KounterSegmentCollection ; 
   void  Kount()      ;
   void  PrintKount() ;
   
   /**Dump Containers */
   int         m_DoDump                              ; //!< Print out segmentcollection content
   void DumpContainers() ;

   std::string m_MboyDumpSegmentCollectionOutputFile ; //!< Dump SegmentCollection      
   std::ofstream  m_OutFile_MboyDumpSegmentCollectionOutputFile ; //!< Dump SegmentCollection      



};

#endif

