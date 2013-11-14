/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyRec_H
#define MboyRec_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
 
/////////////////////////////////////////////////////////////////////////////
#include "MuonboyAth/MuonboyAthMisc.h"

class IMboyGetPrepDataFromEventTool;
class IMboyAthToolHelper;
class IMboyFullRecTool ;

  /**
   @class MboyRec

   This class is the top algorithm performing the Muon reconstruction
   in Muon Spectrometer

  @author samusog@cern.ch
  
  */

class MboyRec:public AthAlgorithm {
public:
    MboyRec(const std::string& name, ISvcLocator* pSvcLocator);
   ~MboyRec();
   
public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////
//Data

   std::string m_DigiStoreLocation ; //!< Location of the MuonRecDigiStoreTDS
   int m_PrintDigitsSummary        ; //!< Print short summary

   std::string m_TrackStoreLocation ; //!< Location of the MB_TrackStoreTDS
   int m_ShortSummaryTrack          ; //!< Print short summary

   std::string m_SegmentStoreLocation ; //!< Location of the MB_SegmentStoreTDS
   int m_ShortSummarySegment          ; //!< Print short summary

   std::string m_SegmentCollectionLocation              ; //!< Location of the SegmentCollection

   std::string m_TrackCollectionLocation                ; //!< Location of the TrackCollection
   std::string m_TrackParticleContainerLocation         ; //!< Location of the TrackParticleContainer

   std::string m_TrackCollection_MSonlyLocation         ; //!< Location of the TrackCollection
   std::string m_TrackParticleContainer_MSonlyLocation  ; //!< Location of the TrackParticleContainer


   int m_DoDbg  ; //!< Print some debug
   
   int m_ProcessingStage  ; //!< Control processing stage

   int m_SwitchOff  ; //!<  Switch Off Option

   int m_AthPosOff ; //!<  use Amdc or Athena positions option
   
   /**Evt count */
   int m_EvtTot                 ;
   StatusCode  KountThem()      ;
   StatusCode  PrintKountThem() ;
   
   /**MB_SegmentStoreTDS count */
   std::vector<int>  m_KounterMB_SegmentStoreTDS ; 
   StatusCode  PrintKountMB_SegmentStoreTDS()    ;

   /**SegmentCollection count */
   std::vector<int>  m_KounterMB_TrackStoreTDS ; 
   StatusCode  PrintKountMB_TrackStoreTDS()    ;
   
   /**SegmentCollection count */
   std::vector<int>  m_KounterSegmentCollection ; 
   StatusCode  PrintKountSegmentCollection()    ;

   /**TrackCollection count */
   std::vector<int>  m_KounterTrackCollection ;  
   StatusCode  PrintKountTrackCollection()    ;

   /**Dump Containers */
   int m_DoDump                ; 
   StatusCode DumpContainers() ;

   std::string    m_MboyDumpSegmentCollectionOutputFile         ; //!< Dump SegmentCollection      
   std::ofstream  m_OutFile_MboyDumpSegmentCollectionOutputFile ; //!< Dump SegmentCollection      

   std::string    m_MboyDumpTrackCollectionOutputFile                ; //!< Dump TrackCollection        
   std::ofstream  m_OutFile_MboyDumpTrackCollectionOutputFile        ; //!< Dump TrackCollection        
   std::ofstream  m_OutFile_MboyDumpTrackCollectionOutputFile_MSonly ; //!< Dump TrackCollection        

   std::string    m_MboyDumpTrackParticleContainerOutputFile                ; //!< Dump TrackParticleContainer 
   std::ofstream  m_OutFile_MboyDumpTrackParticleContainerOutputFile        ; //!< Dump TrackParticleContainer 
   std::ofstream  m_OutFile_MboyDumpTrackParticleContainerOutputFile_MSonly ; //!< Dump TrackParticleContainer 

   ToolHandle< IMboyGetPrepDataFromEventTool > p_IMboyGetPrepDataFromEventTool ; //!< Pointer On IMboyGetPrepDataFromEventTool
   
   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper
   ToolHandle< IMboyFullRecTool >     p_IMboyFullRecTool      ; //!< Pointer On IMboyFullRecTool

};

#endif

