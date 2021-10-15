/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RenameHitCollectionsAlg.h"

/** Constructor */
ISF::RenameHitCollectionsAlg::RenameHitCollectionsAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_inputMcEventCollection(),
  m_inputBCMHits(),
  m_inputBLMHits(),
  m_inputPixelHits(),
  m_inputSCTHits(),
  m_inputTRTUncompressedHits(),
  m_inputLArEMBHits(),
  m_inputLArEMECHits(),
  m_inputLArFCALHits(),
  m_inputLArMiniFCALHits(),
  m_inputLArHECHits(),
  m_inputLArActiveCalibHits(),
  m_inputLArInactiveCalibHits(),
  m_inputLArDeadCalibHits(),
  m_inputTileHits(),
  m_inputMBTSHits(),
  m_inputTileActiveCalibHits(),
  m_inputTileInactiveCalibHits(),
  m_inputTileDeadCalibHits(),
  m_inputCSCHits(),
  m_inputMDTHits(),
  m_inputRPCHits(),
  m_inputTGCHits(),
  m_inputCaloEntryLayer(),
  m_inputMuonEntryLayer(),
  m_inputMuonExitLayer(),
  m_outputMcEventCollection(),
  m_outputBCMHits(),
  m_outputBLMHits(),
  m_outputPixelHits(),
  m_outputSCTHits(),
  m_outputTRTUncompressedHits(),
  m_outputLArEMBHits(),
  m_outputLArEMECHits(),
  m_outputLArFCALHits(),
  m_outputLArMiniFCALHits(),
  m_outputLArHECHits(),
  m_outputTileHits(),
  m_outputMBTSHits(),
  m_outputCSCHits(),
  m_outputMDTHits(),
  m_outputRPCHits(),
  m_outputTGCHits(),
  m_outputCaloEntryLayer(),
  m_outputMuonEntryLayer(),
  m_outputMuonExitLayer()
{
  // Input collections StoreGate keys
  declareProperty( "InputMcEventCollection",   m_inputMcEventCollection   );
  declareProperty( "InputBCMHits",             m_inputBCMHits             );
  declareProperty( "InputBLMHits",             m_inputBLMHits             );
  declareProperty( "InputPixelHits",           m_inputPixelHits           );
  declareProperty( "InputSCTHits",             m_inputSCTHits             );
  declareProperty( "InputTRTUncompressedHits", m_inputTRTUncompressedHits );

  declareProperty( "InputLArEMBHits",          m_inputLArEMBHits          );
  declareProperty( "InputLArEMECHits",         m_inputLArEMECHits         );
  declareProperty( "InputLArFCALHits",         m_inputLArFCALHits         );
  declareProperty( "InputLArMiniFCALHits",     m_inputLArMiniFCALHits     );
  declareProperty( "InputLArHECHits",          m_inputLArHECHits          );
  declareProperty( "InputLArActiveCalibHits",  m_inputLArActiveCalibHits  );
  declareProperty( "InputLArInactiveCalibHits",m_inputLArInactiveCalibHits);
  declareProperty( "InputLArDeadCalibHits",    m_inputLArDeadCalibHits    );

  declareProperty( "InputTileHits",            m_inputTileHits            );
  declareProperty( "InputMBTSHits",            m_inputMBTSHits            );
  declareProperty( "InputTileActiveCalibHits",  m_inputTileActiveCalibHits  );
  declareProperty( "InputTileInactiveCalibHits",m_inputTileInactiveCalibHits);
  declareProperty( "InputTileDeadCalibHits",    m_inputTileDeadCalibHits    );

  declareProperty( "InputCSCHits",             m_inputCSCHits             );
  declareProperty( "InputMDTHits",             m_inputMDTHits             );
  declareProperty( "InputRPCHits",             m_inputRPCHits             );
  declareProperty( "InputTGCHits",             m_inputTGCHits             );

  declareProperty("InputCaloEntryLayer", m_inputCaloEntryLayer   );
  declareProperty("InputMuonEntryLayer", m_inputMuonEntryLayer  );
  declareProperty("InputMuonExitLayer", m_inputMuonExitLayer );

  // Output collections
  declareProperty( "OutputMcEventCollection",   m_outputMcEventCollection   );
  declareProperty( "OutputBCMHits",             m_outputBCMHits             );
  declareProperty( "OutputBLMHits",             m_outputBLMHits             );
  declareProperty( "OutputPixelHits",           m_outputPixelHits           );
  declareProperty( "OutputSCTHits",             m_outputSCTHits             );
  declareProperty( "OutputTRTUncompressedHits", m_outputTRTUncompressedHits );

  declareProperty( "OutputLArEMBHits",          m_outputLArEMBHits          );
  declareProperty( "OutputLArEMECHits",         m_outputLArEMECHits         );
  declareProperty( "OutputLArFCALHits",         m_outputLArFCALHits         );
  declareProperty( "OutputLArMiniFCALHits",     m_outputLArMiniFCALHits     );
  declareProperty( "OutputLArHECHits",          m_outputLArHECHits          );
  declareProperty( "OutputLArActiveCalibHits",  m_outputLArActiveCalibHits  );
  declareProperty( "OutputLArInactiveCalibHits",m_outputLArInactiveCalibHits);
  declareProperty( "OutputLArDeadCalibHits",    m_outputLArDeadCalibHits    );

  declareProperty( "OutputTileHits",            m_outputTileHits            );
  declareProperty( "OutputMBTSHits",            m_outputMBTSHits            );
  declareProperty( "OutputTileActiveCalibHits",  m_outputTileActiveCalibHits  );
  declareProperty( "OutputTileInactiveCalibHits",m_outputTileInactiveCalibHits);
  declareProperty( "OutputTileDeadCalibHits",    m_outputTileDeadCalibHits    );

  declareProperty( "OutputCSCHits",             m_outputCSCHits             );
  declareProperty( "OutputMDTHits",             m_outputMDTHits             );
  declareProperty( "OutputRPCHits",             m_outputRPCHits             );
  declareProperty( "OutputTGCHits",             m_outputTGCHits             );

  declareProperty("OutputCaloEntryLayer", m_outputCaloEntryLayer   );
  declareProperty("OutputMuonEntryLayer", m_outputMuonEntryLayer  );
  declareProperty("OutputMuonExitLayer", m_outputMuonExitLayer );
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
  ATH_CHECK( initializeVarHandleKey(m_inputLArMiniFCALHits    ) );
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
  ATH_CHECK( initializeVarHandleKey(m_outputLArMiniFCALHits    ) );
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

  ATH_CHECK( initializeVarHandleKey(m_outputCaloEntryLayer ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMuonEntryLayer) );
  ATH_CHECK( initializeVarHandleKey(m_outputMuonExitLayer  ) );

  return StatusCode::SUCCESS;
}


/** Athena Algorithm execute */
StatusCode ISF::RenameHitCollectionsAlg::execute()
{
  SG::ReadHandle<McEventCollection> inputMcEventCollection(m_inputMcEventCollection);
  if (!inputMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not find input McEventCollection called " << inputMcEventCollection.name() << " in store " << inputMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }
  SG::WriteHandle<McEventCollection> outputMcEventCollection(m_outputMcEventCollection);
  ATH_CHECK(outputMcEventCollection.record(std::make_unique<McEventCollection>(*inputMcEventCollection)));
  if (!outputMcEventCollection.isValid()) {
    ATH_MSG_ERROR("Could not record output McEventCollection called " << outputMcEventCollection.name() << " in store " << outputMcEventCollection.store() << ".");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(copyCollections( m_inputBCMHits,             m_outputBCMHits             ));
  ATH_CHECK(copyCollections( m_inputBLMHits,             m_outputBLMHits             ));
  ATH_CHECK(copyCollections( m_inputPixelHits,           m_outputPixelHits           ));
  ATH_CHECK(copyCollections( m_inputSCTHits,             m_outputSCTHits             ));
  ATH_CHECK(copyCollections( m_inputTRTUncompressedHits, m_outputTRTUncompressedHits ));

  ATH_CHECK(copyCollections( m_inputLArEMBHits,          m_outputLArEMBHits          ));
  ATH_CHECK(copyCollections( m_inputLArEMECHits,         m_outputLArEMECHits         ));
  ATH_CHECK(copyCollections( m_inputLArFCALHits,         m_outputLArFCALHits         ));
  ATH_CHECK(copyCollections( m_inputLArMiniFCALHits,     m_outputLArMiniFCALHits     ));
  ATH_CHECK(copyCollections( m_inputLArHECHits,          m_outputLArHECHits          ));
  ATH_CHECK(copyCollections( m_inputLArActiveCalibHits,  m_outputLArActiveCalibHits  ));
  ATH_CHECK(copyCollections( m_inputLArInactiveCalibHits,m_outputLArInactiveCalibHits));
  ATH_CHECK(copyCollections( m_inputLArDeadCalibHits,    m_outputLArDeadCalibHits    ));

  ATH_CHECK(copyCollections( m_inputTileHits,            m_outputTileHits            ));
  ATH_CHECK(copyCollections( m_inputMBTSHits,            m_outputMBTSHits            ));
  ATH_CHECK(copyCollections( m_inputTileActiveCalibHits, m_outputTileActiveCalibHits ));
  ATH_CHECK(copyCollections( m_inputTileInactiveCalibHits,m_outputTileInactiveCalibHits));
  ATH_CHECK(copyCollections( m_inputTileDeadCalibHits,   m_outputTileDeadCalibHits   ));

  ATH_CHECK(copyCollections( m_inputCSCHits,             m_outputCSCHits             ));
  ATH_CHECK(copyCollections( m_inputMDTHits,             m_outputMDTHits             ));
  ATH_CHECK(copyCollections( m_inputRPCHits,             m_outputRPCHits             ));
  ATH_CHECK(copyCollections( m_inputTGCHits,             m_outputTGCHits             ));

  ATH_CHECK(copyCollections(m_inputCaloEntryLayer ,m_outputCaloEntryLayer ) );
  ATH_CHECK(copyCollections(m_inputMuonEntryLayer,m_outputMuonEntryLayer) );
  ATH_CHECK(copyCollections(m_inputMuonExitLayer  ,m_outputMuonExitLayer  ) );



  return StatusCode::SUCCESS;
}


/** Initialize the given VarHandleKey */
StatusCode ISF::RenameHitCollectionsAlg::initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const {
  if ( varHandleKey.key().empty() )
    return StatusCode::SUCCESS;

  return varHandleKey.initialize();
}
