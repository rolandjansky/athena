/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RenameHitCollectionsAlg.h"

/** Constructor */
ISF::RenameHitCollectionsAlg::RenameHitCollectionsAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
}


/** Athena Algorithm initialize */
StatusCode ISF::RenameHitCollectionsAlg::initialize()
{
  ATH_CHECK( initializeVarHandleKey(m_inputMcEventCollection  ) );
  ATH_CHECK( initializeVarHandleKey(m_inputBCMHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputBLMHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputPixelHits          ) );
  ATH_CHECK( initializeVarHandleKey(m_inputSCTHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputTRTUncompressedHits) );

  ATH_CHECK( initializeVarHandleKey(m_inputLArEMBHits         ) );
  ATH_CHECK( initializeVarHandleKey(m_inputLArEMECHits        ) );
  ATH_CHECK( initializeVarHandleKey(m_inputLArFCALHits        ) );
  ATH_CHECK( initializeVarHandleKey(m_inputLArHECHits         ) );
  ATH_CHECK( initializeVarHandleKey(m_inputLArActiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_inputLArInactiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_inputLArDeadCalibHits ) );

  ATH_CHECK( initializeVarHandleKey(m_inputTileHits           ) );
  ATH_CHECK( initializeVarHandleKey(m_inputMBTSHits           ) );
  ATH_CHECK( initializeVarHandleKey(m_inputTileActiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_inputTileInactiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_inputTileDeadCalibHits ) );

  ATH_CHECK( initializeVarHandleKey(m_inputCSCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputMDTHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputRPCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputTGCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputMMHits             ) );
  ATH_CHECK( initializeVarHandleKey(m_inputsTGCHits           ) );

  ATH_CHECK( initializeVarHandleKey(m_inputITkPixelHits          ) );
  ATH_CHECK( initializeVarHandleKey(m_inputITkStripHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_inputHGTDHits) );

  ATH_CHECK( initializeVarHandleKey(m_inputCaloEntryLayer ) );
  ATH_CHECK( initializeVarHandleKey(m_inputMuonEntryLayer) );
  ATH_CHECK( initializeVarHandleKey(m_inputMuonExitLayer  ) );

  ATH_CHECK( initializeVarHandleKey(m_outputMcEventCollection  ) );
  ATH_CHECK( initializeVarHandleKey(m_outputBCMHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputBLMHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputPixelHits          ) );
  ATH_CHECK( initializeVarHandleKey(m_outputSCTHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTRTUncompressedHits) );

  ATH_CHECK( initializeVarHandleKey(m_outputLArEMBHits         ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArEMECHits        ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArFCALHits        ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArHECHits         ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArActiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArInactiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArDeadCalibHits ) );

  ATH_CHECK( initializeVarHandleKey(m_outputTileHits           ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMBTSHits           ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTileActiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTileInactiveCalibHits ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTileDeadCalibHits ) );

  ATH_CHECK( initializeVarHandleKey(m_outputCSCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMDTHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputRPCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTGCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMMHits             ) );
  ATH_CHECK( initializeVarHandleKey(m_outputsTGCHits           ) );

  ATH_CHECK( initializeVarHandleKey(m_outputITkPixelHits          ) );
  ATH_CHECK( initializeVarHandleKey(m_outputITkStripHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputHGTDHits) );

  ATH_CHECK( initializeVarHandleKey(m_outputCaloEntryLayer ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMuonEntryLayer) );
  ATH_CHECK( initializeVarHandleKey(m_outputMuonExitLayer  ) );

  return StatusCode::SUCCESS;
}


/** Athena Algorithm execute */
StatusCode ISF::RenameHitCollectionsAlg::execute(const EventContext &ctx) const
{
  SG::ReadHandle<McEventCollection> inputMcEventCollection(m_inputMcEventCollection, ctx);
  if (!inputMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find input McEventCollection called " << inputMcEventCollection.name() << " in store " << inputMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  SG::WriteHandle<McEventCollection> outputMcEventCollection(m_outputMcEventCollection, ctx);
  ATH_CHECK(outputMcEventCollection.record(std::make_unique<McEventCollection>(*inputMcEventCollection)));
  if (!outputMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output McEventCollection called " << outputMcEventCollection.name() << " in store " << outputMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(copyCollections( m_inputBCMHits,             m_outputBCMHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputBLMHits,             m_outputBLMHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputPixelHits,           m_outputPixelHits           , ctx ));
  ATH_CHECK(copyCollections( m_inputSCTHits,             m_outputSCTHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputTRTUncompressedHits, m_outputTRTUncompressedHits , ctx ));

  ATH_CHECK(copyCollections( m_inputLArEMBHits,          m_outputLArEMBHits          , ctx ));
  ATH_CHECK(copyCollections( m_inputLArEMECHits,         m_outputLArEMECHits         , ctx ));
  ATH_CHECK(copyCollections( m_inputLArFCALHits,         m_outputLArFCALHits         , ctx ));
  ATH_CHECK(copyCollections( m_inputLArHECHits,          m_outputLArHECHits          , ctx ));
  ATH_CHECK(copyCollections( m_inputLArActiveCalibHits,  m_outputLArActiveCalibHits  , ctx ));
  ATH_CHECK(copyCollections( m_inputLArInactiveCalibHits,m_outputLArInactiveCalibHits, ctx ));
  ATH_CHECK(copyCollections( m_inputLArDeadCalibHits,    m_outputLArDeadCalibHits    , ctx ));

  ATH_CHECK(copyCollections( m_inputTileHits,            m_outputTileHits            , ctx ));
  ATH_CHECK(copyCollections( m_inputMBTSHits,            m_outputMBTSHits            , ctx ));
  ATH_CHECK(copyCollections( m_inputTileActiveCalibHits, m_outputTileActiveCalibHits , ctx ));
  ATH_CHECK(copyCollections( m_inputTileInactiveCalibHits,m_outputTileInactiveCalibHits, ctx ));
  ATH_CHECK(copyCollections( m_inputTileDeadCalibHits,   m_outputTileDeadCalibHits   , ctx ));

  ATH_CHECK(copyCollections( m_inputCSCHits,             m_outputCSCHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputMDTHits,             m_outputMDTHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputRPCHits,             m_outputRPCHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputTGCHits,             m_outputTGCHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputMMHits,              m_outputMMHits               , ctx ));
  ATH_CHECK(copyCollections( m_inputsTGCHits,            m_outputsTGCHits            , ctx ));

  ATH_CHECK(copyCollections( m_inputITkPixelHits,        m_outputITkPixelHits           , ctx ));
  ATH_CHECK(copyCollections( m_inputITkStripHits,        m_outputITkStripHits             , ctx ));
  ATH_CHECK(copyCollections( m_inputHGTDHits,           m_outputHGTDHits , ctx ));

  ATH_CHECK(copyCollections(m_inputCaloEntryLayer ,m_outputCaloEntryLayer , ctx ));
  ATH_CHECK(copyCollections(m_inputMuonEntryLayer,m_outputMuonEntryLayer, ctx ));
  ATH_CHECK(copyCollections(m_inputMuonExitLayer  ,m_outputMuonExitLayer  , ctx ));



  return StatusCode::SUCCESS;
}


/** Initialize the given VarHandleKey */
StatusCode ISF::RenameHitCollectionsAlg::initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const {
  if ( varHandleKey.key().empty() )
    return StatusCode::SUCCESS;

  return varHandleKey.initialize();
}
