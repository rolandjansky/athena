/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK( m_inputBCMHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputBLMHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputPixelHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputSCTHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputTRTUncompressedHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_inputITkPixelHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputITkStripHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputPLRHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputHGTDHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_inputLArEMBHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputLArEMECHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputLArFCALHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputLArHECHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_inputTileHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputMBTSHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_inputCSCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputMDTHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputRPCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputTGCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputsTGCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_inputMMHits.initialize(SG::AllowEmpty) );


  ATH_CHECK( m_outputBCMHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputBLMHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputPixelHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputSCTHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputTRTUncompressedHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_outputITkPixelHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputITkStripHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputPLRHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputHGTDHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_outputLArEMBHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputLArEMECHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputLArFCALHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputLArHECHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_outputTileHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputMBTSHits.initialize(SG::AllowEmpty) );

  ATH_CHECK( m_outputCSCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputMDTHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputRPCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputTGCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputsTGCHits.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_outputMMHits.initialize(SG::AllowEmpty) );

  return StatusCode::SUCCESS;
}


/** Athena Algorithm execute */
StatusCode ISF::CollectionMerger::execute(const EventContext& ctx) const
{
  ATH_CHECK(mergeCollections( m_inputBCMHits,             m_outputBCMHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputBLMHits,             m_outputBLMHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputPixelHits,           m_outputPixelHits,           ctx ));
  ATH_CHECK(mergeCollections( m_inputSCTHits,             m_outputSCTHits,             ctx ));
  ATH_CHECK(mergeCollections( m_inputTRTUncompressedHits, m_outputTRTUncompressedHits, ctx ));

  ATH_CHECK(mergeCollections( m_inputITkPixelHits,        m_outputITkPixelHits,        ctx ));
  ATH_CHECK(mergeCollections( m_inputITkStripHits,        m_outputITkStripHits,        ctx ));
  ATH_CHECK(mergeCollections( m_inputPLRHits,             m_outputPLRHits,             ctx ));
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
  ATH_CHECK(mergeCollections( m_inputsTGCHits,            m_outputsTGCHits,            ctx ));
  ATH_CHECK(mergeCollections( m_inputMMHits,              m_outputMMHits,              ctx ));

  return StatusCode::SUCCESS;
}
