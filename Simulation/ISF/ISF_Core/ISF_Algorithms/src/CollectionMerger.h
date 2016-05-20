/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CollectionMerger.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_ALGS_COLLECTIONMERGER_H
#define ISF_ALGS_COLLECTIONMERGER_H 1

// STL includes
#include <string>

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"

// ATLAS C++
#include "CxxUtils/make_unique.h"

// SimHit EDM includes
//    InnerDetector
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
//    Calorimeter
#include "LArSimEvent/LArHitContainer.h"
#include "TileSimEvent/TileHitVector.h"
//    Muon Spectrometer
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"

// forward declarations
namespace ISFTesting {
  class CollectionMerger_test;
}


namespace ISF {

  /** @class CollectionMerger

      @brief Athena Algorithm that merges a given set of StoreGate collections in one final collection.

      @author Elmar.Ritsch -at- cern.ch

     */
  class CollectionMerger final : public AthAlgorithm {

  /** Allow the test class access to all methods */
  friend class ISFTesting::CollectionMerger_test;

  public:
    /** Constructor */
    CollectionMerger( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~CollectionMerger();

    /** Athena algorithm's interface methods */
    StatusCode  initialize() override;
    StatusCode  execute()    override;
    StatusCode  finalize()   override;

  private:
    /** Initialize the given VarHandleKey */
    StatusCode initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const;

    /** Commonly used member and function parameter types */
    typedef std::vector<std::string>  SGKeyVector_t;
    template <typename T>
    using ReadHandleKeyVector_t = typename std::vector<SG::ReadHandleKey<T>>;


    /** Setup a vector of ReadHandleKeys for the given vector of string StoreGate Keys */
    template <typename T>
    StatusCode setupReadHandleKeyVector( const SGKeyVector_t&      sgKeyVec,
                                         ReadHandleKeyVector_t<T>& readHandleVec ) const;

    /** Merge all hits of inputReadHandleKeys's collections into outputWriteHandleKey */
    template <typename T>
    void mergeCollections( const ReadHandleKeyVector_t<T>& inputReadHandleKeys,
                           SG::WriteHandleKey<T>&          outputWriteHandleKey ) const;

    /** Input collection StoreGate keys */
    SGKeyVector_t                                       m_inputBCMHitsSGKeys;
    SGKeyVector_t                                       m_inputBLMHitsSGKeys;
    SGKeyVector_t                                       m_inputPixelHitsSGKeys;
    SGKeyVector_t                                       m_inputSCTHitsSGKeys;
    SGKeyVector_t                                       m_inputTRTUncompressedHitsSGKeys;

    SGKeyVector_t                                       m_inputLArEMBHitsSGKeys;
    SGKeyVector_t                                       m_inputLArEMECHitsSGKeys;
    SGKeyVector_t                                       m_inputLArFCALHitsSGKeys;
    SGKeyVector_t                                       m_inputLArHECHitsSGKeys;

    SGKeyVector_t                                       m_inputTileHitsSGKeys;
    SGKeyVector_t                                       m_inputMBTSHitsSGKeys;

    SGKeyVector_t                                       m_inputCSCHitsSGKeys;
    SGKeyVector_t                                       m_inputMDTHitsSGKeys;
    SGKeyVector_t                                       m_inputRPCHitsSGKeys;
    SGKeyVector_t                                       m_inputTGCHitsSGKeys;

    /** Input collection ReadHandleKeys */
    ReadHandleKeyVector_t<SiHitCollection>              m_inputBCMHits;
    ReadHandleKeyVector_t<SiHitCollection>              m_inputBLMHits;
    ReadHandleKeyVector_t<SiHitCollection>              m_inputPixelHits;
    ReadHandleKeyVector_t<SiHitCollection>              m_inputSCTHits;
    ReadHandleKeyVector_t<TRTUncompressedHitCollection> m_inputTRTUncompressedHits;

    ReadHandleKeyVector_t<LArHitContainer>              m_inputLArEMBHits;
    ReadHandleKeyVector_t<LArHitContainer>              m_inputLArEMECHits;
    ReadHandleKeyVector_t<LArHitContainer>              m_inputLArFCALHits;
    ReadHandleKeyVector_t<LArHitContainer>              m_inputLArHECHits;

    ReadHandleKeyVector_t<TileHitVector>                m_inputTileHits;
    ReadHandleKeyVector_t<TileHitVector>                m_inputMBTSHits;

    ReadHandleKeyVector_t<CSCSimHitCollection>          m_inputCSCHits;
    ReadHandleKeyVector_t<MDTSimHitCollection>          m_inputMDTHits;
    ReadHandleKeyVector_t<RPCSimHitCollection>          m_inputRPCHits;
    ReadHandleKeyVector_t<TGCSimHitCollection>          m_inputTGCHits;

    /** Output collection WriteHandleKeys */
    SG::WriteHandleKey<SiHitCollection>                 m_outputBCMHits;
    SG::WriteHandleKey<SiHitCollection>                 m_outputBLMHits;
    SG::WriteHandleKey<SiHitCollection>                 m_outputPixelHits;
    SG::WriteHandleKey<SiHitCollection>                 m_outputSCTHits;
    SG::WriteHandleKey<TRTUncompressedHitCollection>    m_outputTRTUncompressedHits;

    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMBHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMECHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArFCALHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArHECHits;

    SG::WriteHandleKey<TileHitVector>                   m_outputTileHits;
    SG::WriteHandleKey<TileHitVector>                   m_outputMBTSHits;

    SG::WriteHandleKey<CSCSimHitCollection>             m_outputCSCHits;
    SG::WriteHandleKey<MDTSimHitCollection>             m_outputMDTHits;
    SG::WriteHandleKey<RPCSimHitCollection>             m_outputRPCHits;
    SG::WriteHandleKey<TGCSimHitCollection>             m_outputTGCHits;
  };

}

//
// templated methods below
// 

/** Setup a vector of ReadHandleKeys for the given vector of string StoreGate Keys */
template <typename T>
StatusCode ISF::CollectionMerger::setupReadHandleKeyVector( const SGKeyVector_t&      sgKeyVec,
                                                            ReadHandleKeyVector_t<T>& readHandleVec ) const {
  readHandleVec.reserve( sgKeyVec.size() );

  // convert string StoreGate key to ReadHandleKey
  for ( const auto& sgKey: sgKeyVec ) {
    readHandleVec.emplace_back( sgKey );
  }

  // initialize all ReadHandleKeys
  for ( auto& readHandleKey: readHandleVec ) {
    if ( readHandleKey.initialize().isFailure() ) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


/** Merge all hits of inputReadHandleKeys's collections into outputWriteHandleKey */
template <typename T>
void ISF::CollectionMerger::mergeCollections( const ReadHandleKeyVector_t<T>& inputReadHandleKeys,
                                              SG::WriteHandleKey<T>&          outputWriteHandleKey ) const {
    // skip if not input collection
    if ( inputReadHandleKeys.empty() )
        return;

    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<T>  outputHandle{outputWriteHandleKey};
    outputHandle.record( CxxUtils::make_unique<T>() );

    for ( const auto& collKey: inputReadHandleKeys ) {
        SG::ReadHandle<T>  inputHandle{collKey};

        for ( const auto& hit: *inputHandle ) {
            // TODO: replace with ->Emplace(hit) once LArHitContainer supports this
            outputHandle->push_back( hit );
        }
    }
}


#endif //> !ISF_ALGS_COLLECTIONMERGER_H
