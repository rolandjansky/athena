/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyChecks_H
#define MboyChecks_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MuonboyAth/MuonboyAthMisc.h"

class IMboyDumpMboyStoreTool;

  /**
   @class MboyChecks

   This class is performing dumps of the collections of MboyCoreEvt objects

  @author samusog@cern.ch
  
  */

class MboyChecks:public AthAlgorithm {
public:
  MboyChecks(const std::string& name, ISvcLocator* pSvcLocator);
  ~MboyChecks();

public:
///////////////////////////////////

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
///////////////////////////////////
   std::string m_TrackStoreLocation   ;//!< Location of the MB_TrackStoreTDS
   std::string m_SegmentStoreLocation ;//!< Location of the MB_SegmentStoreTDS

   std::string    m_NameOfMuonboySegmentStoreOutputFile ;//!< file to dump  the Segment Store
   std::ofstream  m_OutMuonboySegmentStore              ;//!< file to dump  the Segment Store
   std::ofstream  m_OutMuonboySegmentStoreChecks        ;//!< file to dump  the Segment Store

   std::string    m_NameOfMuonboyTrackStoreOutputFile ;//!< file to dump the tracks store
   std::ofstream  m_OutMuonboyTrackStore              ;//!< file to dump the tracks store
   std::ofstream  m_OutMuonboyTrackStoreChecks        ;//!< file to dump the tracks store


   ToolHandle< IMboyDumpMboyStoreTool >          p_IMboyDumpMboyStoreTool          ; //!< Pointer On IMboyDumpMboyStoreTool

};

#endif
