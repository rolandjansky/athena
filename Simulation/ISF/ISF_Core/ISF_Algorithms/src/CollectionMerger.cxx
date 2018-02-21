/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CollectionMerger.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CollectionMerger.h"

/** Constructor */
ISF::CollectionMerger::CollectionMerger( const std::string& name, ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_inputBCMHitsSGKeys(),
  m_inputBLMHitsSGKeys(),
  m_inputPixelHitsSGKeys(),
  m_inputSCTHitsSGKeys(),
  m_inputTRTUncompressedHitsSGKeys(),
  m_inputLArEMBHitsSGKeys(),
  m_inputLArEMECHitsSGKeys(),
  m_inputLArFCALHitsSGKeys(),
  m_inputLArHECHitsSGKeys(),
  m_inputTileHitsSGKeys(),
  m_inputMBTSHitsSGKeys(),
  m_inputCSCHitsSGKeys(),
  m_inputMDTHitsSGKeys(),
  m_inputRPCHitsSGKeys(),
  m_inputTGCHitsSGKeys(),
  m_inputBCMHits(),
  m_inputBLMHits(),
  m_inputPixelHits(),
  m_inputSCTHits(),
  m_inputTRTUncompressedHits(),
  m_inputLArEMBHits(),
  m_inputLArEMECHits(),
  m_inputLArFCALHits(),
  m_inputLArHECHits(),
  m_inputTileHits(),
  m_inputMBTSHits(),
  m_inputCSCHits(),
  m_inputMDTHits(),
  m_inputRPCHits(),
  m_inputTGCHits(),
  m_outputBCMHits(),
  m_outputBLMHits(),
  m_outputPixelHits(),
  m_outputSCTHits(),
  m_outputTRTUncompressedHits(),
  m_outputLArEMBHits(),
  m_outputLArEMECHits(),
  m_outputLArFCALHits(),
  m_outputLArHECHits(),
  m_outputTileHits(),
  m_outputMBTSHits(),
  m_outputCSCHits(),
  m_outputMDTHits(),
  m_outputRPCHits(),
  m_outputTGCHits()
{
  // Input collections StoreGate keys
  declareProperty( "InputBCMHits",             m_inputBCMHitsSGKeys             );
  declareProperty( "InputBLMHits",             m_inputBLMHitsSGKeys             );
  declareProperty( "InputPixelHits",           m_inputPixelHitsSGKeys           );
  declareProperty( "InputSCTHits",             m_inputSCTHitsSGKeys             );
  declareProperty( "InputTRTUncompressedHits", m_inputTRTUncompressedHitsSGKeys );

  declareProperty( "InputLArEMBHits",          m_inputLArEMBHitsSGKeys          );
  declareProperty( "InputLArEMECHits",         m_inputLArEMECHitsSGKeys         );
  declareProperty( "InputLArFCALHits",         m_inputLArFCALHitsSGKeys         );
  declareProperty( "InputLArHECHits",          m_inputLArHECHitsSGKeys          );

  declareProperty( "InputTileHits",            m_inputTileHitsSGKeys            );
  declareProperty( "InputMBTSHits",            m_inputMBTSHitsSGKeys            );

  declareProperty( "InputCSCHits",             m_inputCSCHitsSGKeys             );
  declareProperty( "InputMDTHits",             m_inputMDTHitsSGKeys             );
  declareProperty( "InputRPCHits",             m_inputRPCHitsSGKeys             );
  declareProperty( "InputTGCHits",             m_inputTGCHitsSGKeys             );

  // Output collections
  declareProperty( "OutputBCMHits",             m_outputBCMHits             );
  declareProperty( "OutputBLMHits",             m_outputBLMHits             );
  declareProperty( "OutputPixelHits",           m_outputPixelHits           );
  declareProperty( "OutputSCTHits",             m_outputSCTHits             );
  declareProperty( "OutputTRTUncompressedHits", m_outputTRTUncompressedHits );

  declareProperty( "OutputLArEMBHits",          m_outputLArEMBHits          );
  declareProperty( "OutputLArEMECHits",         m_outputLArEMECHits         );
  declareProperty( "OutputLArFCALHits",         m_outputLArFCALHits         );
  declareProperty( "OutputLArHECHits",          m_outputLArHECHits          );

  declareProperty( "OutputTileHits",            m_outputTileHits            );
  declareProperty( "OutputMBTSHits",            m_outputMBTSHits            );

  declareProperty( "OutputCSCHits",             m_outputCSCHits             );
  declareProperty( "OutputMDTHits",             m_outputMDTHits             );
  declareProperty( "OutputRPCHits",             m_outputRPCHits             );
  declareProperty( "OutputTGCHits",             m_outputTGCHits             );
}


/** Destructor */
ISF::CollectionMerger::~CollectionMerger()
{}


/** Athena Algorithm initialize */
StatusCode ISF::CollectionMerger::initialize()
{
  ATH_CHECK( setupReadHandleKeyVector(m_inputBCMHitsSGKeys,             m_inputBCMHits            ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputBLMHitsSGKeys,             m_inputBLMHits            ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputPixelHitsSGKeys,           m_inputPixelHits          ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputSCTHitsSGKeys,             m_inputSCTHits            ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputTRTUncompressedHitsSGKeys, m_inputTRTUncompressedHits) );

  ATH_CHECK( setupReadHandleKeyVector(m_inputLArEMBHitsSGKeys,          m_inputLArEMBHits         ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputLArEMECHitsSGKeys,         m_inputLArEMECHits        ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputLArFCALHitsSGKeys,         m_inputLArFCALHits        ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputLArHECHitsSGKeys,          m_inputLArHECHits         ) );

  ATH_CHECK( setupReadHandleKeyVector(m_inputTileHitsSGKeys,            m_inputTileHits           ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputMBTSHitsSGKeys,            m_inputMBTSHits           ) );

  ATH_CHECK( setupReadHandleKeyVector(m_inputCSCHitsSGKeys,             m_inputCSCHits            ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputMDTHitsSGKeys,             m_inputMDTHits            ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputRPCHitsSGKeys,             m_inputRPCHits            ) );
  ATH_CHECK( setupReadHandleKeyVector(m_inputTGCHitsSGKeys,             m_inputTGCHits            ) );


  ATH_CHECK( initializeVarHandleKey(m_outputBCMHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputBLMHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputPixelHits          ) );
  ATH_CHECK( initializeVarHandleKey(m_outputSCTHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTRTUncompressedHits) );

  ATH_CHECK( initializeVarHandleKey(m_outputLArEMBHits         ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArEMECHits        ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArFCALHits        ) );
  ATH_CHECK( initializeVarHandleKey(m_outputLArHECHits         ) );

  ATH_CHECK( initializeVarHandleKey(m_outputTileHits           ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMBTSHits           ) );

  ATH_CHECK( initializeVarHandleKey(m_outputCSCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputMDTHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputRPCHits            ) );
  ATH_CHECK( initializeVarHandleKey(m_outputTGCHits            ) );

  return StatusCode::SUCCESS;
}


/** Athena Algorithm execute */
StatusCode ISF::CollectionMerger::execute()
{
  ATH_CHECK(mergeCollections( m_inputBCMHits,             m_outputBCMHits             ));
  ATH_CHECK(mergeCollections( m_inputBLMHits,             m_outputBLMHits             ));
  ATH_CHECK(mergeCollections( m_inputPixelHits,           m_outputPixelHits           ));
  ATH_CHECK(mergeCollections( m_inputSCTHits,             m_outputSCTHits             ));
  ATH_CHECK(mergeCollections( m_inputTRTUncompressedHits, m_outputTRTUncompressedHits ));

  ATH_CHECK(mergeCollections( m_inputLArEMBHits,          m_outputLArEMBHits          ));
  ATH_CHECK(mergeCollections( m_inputLArEMECHits,         m_outputLArEMECHits         ));
  ATH_CHECK(mergeCollections( m_inputLArFCALHits,         m_outputLArFCALHits         ));
  ATH_CHECK(mergeCollections( m_inputLArHECHits,          m_outputLArHECHits          ));

  ATH_CHECK(mergeCollections( m_inputTileHits,            m_outputTileHits            ));
  ATH_CHECK(mergeCollections( m_inputMBTSHits,            m_outputMBTSHits            ));

  ATH_CHECK(mergeCollections( m_inputCSCHits,             m_outputCSCHits             ));
  ATH_CHECK(mergeCollections( m_inputMDTHits,             m_outputMDTHits             ));
  ATH_CHECK(mergeCollections( m_inputRPCHits,             m_outputRPCHits             ));
  ATH_CHECK(mergeCollections( m_inputTGCHits,             m_outputTGCHits             ));

  return StatusCode::SUCCESS;
}


/** Athena Algorithm finalize */
StatusCode ISF::CollectionMerger::finalize()
{
  return StatusCode::SUCCESS;
}

/** Initialize the given VarHandleKey */
StatusCode ISF::CollectionMerger::initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const {
  if ( varHandleKey.key().empty() )
    return StatusCode::SUCCESS;

  return varHandleKey.initialize();
}
