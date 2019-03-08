/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MetadataTest.h

#ifndef ATHENAPOOLTOOLS_METADATATEST_H
#define ATHENAPOOLTOOLS_METADATATEST_H
/**
 * @file MetadataTest.h
 * @brief class definition for MetadataTest
 */
 
/**
 * @class MetadataTest
 * @brief Counts events in a pool file, and can also count the objects 
 * and give a summary of their keys. Output is via the message logger.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created October 2004
 * $Id: MetadataTest.h,v 1.4 2007-12-20 21:54:12 cranshaw Exp $
 */
             
                                                                                
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadMetaHandleKey.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "EventInfo/EventStreamInfo.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"

class MetadataTest : public AthAlgorithm {
public:

	  
  MetadataTest(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MetadataTest();
  
  StatusCode start(); /// Algorithm interface. Cannot re-initialize with this
  StatusCode execute();    /// Algorithm interface.
  StatusCode finalize();   /// Algorithm interface.

private:
  SG::ReadMetaHandleKey<xAOD::CutBookkeeperContainer> m_hkey;
  SG::ReadMetaHandleKey<xAOD::CutBookkeeperContainer> m_hinckey;
  SG::ReadMetaHandleKey<EventStreamInfo> m_eihkey;
  SG::ReadMetaHandleKey<ByteStreamMetadataContainer> m_bshkey;

  bool m_esidone;
  std::string m_inputstream;

};
#endif
