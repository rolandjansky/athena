/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionMerger.h"

/** Constructor */
ISF::CollectionMerger::CollectionMerger( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
}


/** Athena Algorithm initialize */
StatusCode ISF::CollectionMerger::initialize()
{
  ATH_CHECK( m_inputPLRHits.initialize(not m_inputPLRHits.empty()) );
  ATH_CHECK( m_inputBCMHits.initialize(not m_inputBCMHits.empty()) );
  ATH_CHECK( m_inputBLMHits.initialize(not m_inputBLMHits.empty()) );
  ATH_CHECK( m_inputPixelHits.initialize(not m_inputPixelHits.empty()) );
  ATH_CHECK( m_inputSCTHits.initialize(not m_inputSCTHits.empty()) );
  ATH_CHECK( m_inputTRTUncompressedHits.initialize(not m_inputTRTUncompressedHits.empty()) );

  ATH_CHECK( m_inputITkPixelHits.initialize(not m_inputITkPixelHits.empty()) );
  ATH_CHECK( m_inputITkStripHits.initialize(not m_inputITkStripHits.empty()) );
  ATH_CHECK( m_inputHGTDHits.initialize(not m_inputHGTDHits.empty()) );  

  ATH_CHECK( m_inputLArEMBHits.initialize(not m_inputLArEMBHits.empty()) );
  ATH_CHECK( m_inputLArEMECHits.initialize(not m_inputLArEMECHits.empty()) );
  ATH_CHECK( m_inputLArFCALHits.initialize(not m_inputLArFCALHits.empty()) );
  ATH_CHECK( m_inputLArHECHits.initialize(not m_inputLArHECHits.empty()) );

  ATH_CHECK( m_inputTileHits.initialize(not m_inputTileHits.empty()) );
  ATH_CHECK( m_inputMBTSHits.initialize(not m_inputMBTSHits.empty()) );

  ATH_CHECK( m_inputCSCHits.initialize(not m_inputCSCHits.empty()) );
  ATH_CHECK( m_inputMDTHits.initialize(not m_inputMDTHits.empty()) );
  ATH_CHECK( m_inputRPCHits.initialize(not m_inputRPCHits.empty()) );
  ATH_CHECK( m_inputTGCHits.initialize(not m_inputTGCHits.empty()) );


  ATH_CHECK( m_outputPLRHits.initialize(not m_outputPLRHits.key().empty()            ) );
  ATH_CHECK( m_outputBCMHits.initialize(not m_outputBCMHits.key().empty()            ) );
  ATH_CHECK( m_outputBLMHits.initialize(not m_outputBLMHits.key().empty()            ) );
  ATH_CHECK( m_outputPixelHits.initialize(not m_outputPixelHits.key().empty()          ) );
  ATH_CHECK( m_outputSCTHits.initialize(not m_outputSCTHits.key().empty()            ) );
  ATH_CHECK( m_outputTRTUncompressedHits.initialize(not m_outputTRTUncompressedHits.key().empty()) );

  ATH_CHECK( m_outputITkPixelHits.initialize(not m_outputITkPixelHits.key().empty()          ) );
  ATH_CHECK( m_outputITkStripHits.initialize(not m_outputITkStripHits.key().empty()            ) );
  ATH_CHECK( m_outputHGTDHits.initialize(not m_outputHGTDHits.key().empty()          ) );

  ATH_CHECK( m_outputLArEMBHits.initialize(not m_outputLArEMBHits.key().empty()         ) );
  ATH_CHECK( m_outputLArEMECHits.initialize(not m_outputLArEMECHits.key().empty()        ) );
  ATH_CHECK( m_outputLArFCALHits.initialize(not m_outputLArFCALHits.key().empty()        ) );
  ATH_CHECK( m_outputLArHECHits.initialize(not m_outputLArHECHits.key().empty()         ) );

  ATH_CHECK( m_outputTileHits.initialize(not m_outputTileHits.key().empty()           ) );
  ATH_CHECK( m_outputMBTSHits.initialize(not m_outputMBTSHits.key().empty()           ) );

  ATH_CHECK( m_outputCSCHits.initialize(not m_outputCSCHits.key().empty()            ) );
  ATH_CHECK( m_outputMDTHits.initialize(not m_outputMDTHits.key().empty()            ) );
  ATH_CHECK( m_outputRPCHits.initialize(not m_outputRPCHits.key().empty()            ) );
  ATH_CHECK( m_outputTGCHits.initialize(not m_outputTGCHits.key().empty()            ) );

  return StatusCode::SUCCESS;
}


/** Athena Algorithm execute */
StatusCode ISF::CollectionMerger::execute(const EventContext& ctx) const
{
  ATH_CHECK(mergeCollections( m_inputPLRHits,             m_outputPLRHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputBCMHits,             m_outputBCMHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputBLMHits,             m_outputBLMHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputPixelHits,           m_outputPixelHits,           ctx ));
  ATH_CHECK(mergeCollections( m_inputSCTHits,             m_outputSCTHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputTRTUncompressedHits, m_outputTRTUncompressedHits, ctx ));

  ATH_CHECK(mergeCollections( m_inputITkPixelHits,        m_outputITkPixelHits,        ctx ));
  ATH_CHECK(mergeCollections( m_inputITkStripHits,        m_outputITkStripHits,        ctx ));
  ATH_CHECK(mergeCollections( m_inputHGTDHits,            m_outputHGTDHits,            ctx ));

  ATH_CHECK(mergeCollections( m_inputLArEMBHits,          m_outputLArEMBHits,          ctx ));
  ATH_CHECK(mergeCollections( m_inputLArEMECHits,         m_outputLArEMECHits,         ctx ));
  ATH_CHECK(mergeCollections( m_inputLArFCALHits,         m_outputLArFCALHits,         ctx ));
  ATH_CHECK(mergeCollections( m_inputLArHECHits,          m_outputLArHECHits,          ctx ));

  ATH_CHECK(mergeCollections( m_inputTileHits,            m_outputTileHits,            ctx ));
  ATH_CHECK(mergeCollections( m_inputMBTSHits,            m_outputMBTSHits,            ctx ));

  ATH_CHECK(mergeCollections( m_inputCSCHits,             m_outputCSCHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputMDTHits,             m_outputMDTHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputRPCHits,             m_outputRPCHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputTGCHits,             m_outputTGCHits,             ctx ));

  return StatusCode::SUCCESS;
}
