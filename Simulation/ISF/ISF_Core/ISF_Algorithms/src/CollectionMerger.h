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
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

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
    StatusCode  execute() override;

  private:
    /** Commonly used member and function parameter types */
    typedef std::vector<std::string>  SGKeyVector_t;
    template <typename T>
    using ReadHandleVector_t = typename std::vector<SG::ReadHandle<T>>;


    /** Setup a vector of ReadHandles for the given vector of string StoreGate Keys */
    template <typename T>
    StatusCode setupReadHandleVector( const SGKeyVector_t& sgKeyVec,
                                      ReadHandleVector_t<T>& readHandleVec ) const;

    /** Merge all hits of inputReadHandles's collections into outputWriteHandle */
    template <typename T>
    void mergeCollections( const ReadHandleVector_t<T>& inputReadHandles,
                           SG::WriteHandle<T>&          outputWriteHandle ) const;

    /** Input collection StoreGate keys */
    SGKeyVector_t  m_inputBCMHitsSGKeys;
    SGKeyVector_t  m_inputBLMHitsSGKeys;
    SGKeyVector_t  m_inputPixelHitsSGKeys;
    SGKeyVector_t  m_inputSCTHitsSGKeys;
    SGKeyVector_t  m_inputTRTUncompressedHitsSGKeys;

    SGKeyVector_t  m_inputLArEMBHitsSGKeys;
    SGKeyVector_t  m_inputLArEMECHitsSGKeys;
    SGKeyVector_t  m_inputLArFCALHitsSGKeys;
    SGKeyVector_t  m_inputLArHECHitsSGKeys;

    SGKeyVector_t  m_inputTileHitsSGKeys;
    SGKeyVector_t  m_inputMBTSHitsSGKeys;

    SGKeyVector_t  m_inputCSCHitsSGKeys;
    SGKeyVector_t  m_inputMDTHitsSGKeys;
    SGKeyVector_t  m_inputRPCHitsSGKeys;
    SGKeyVector_t  m_inputTGCHitsSGKeys;

    /** Input collection ReadHandles */
    ReadHandleVector_t<SiHitCollection>              m_inputBCMHits;
    ReadHandleVector_t<SiHitCollection>              m_inputBLMHits;
    ReadHandleVector_t<SiHitCollection>              m_inputPixelHits;
    ReadHandleVector_t<SiHitCollection>              m_inputSCTHits;
    ReadHandleVector_t<TRTUncompressedHitCollection> m_inputTRTUncompressedHits;

    ReadHandleVector_t<LArHitContainer>              m_inputLArEMBHits;
    ReadHandleVector_t<LArHitContainer>              m_inputLArEMECHits;
    ReadHandleVector_t<LArHitContainer>              m_inputLArFCALHits;
    ReadHandleVector_t<LArHitContainer>              m_inputLArHECHits;

    ReadHandleVector_t<TileHitVector>                m_inputTileHits;
    ReadHandleVector_t<TileHitVector>                m_inputMBTSHits;

    ReadHandleVector_t<CSCSimHitCollection>          m_inputCSCHits;
    ReadHandleVector_t<MDTSimHitCollection>          m_inputMDTHits;
    ReadHandleVector_t<RPCSimHitCollection>          m_inputRPCHits;
    ReadHandleVector_t<TGCSimHitCollection>          m_inputTGCHits;

    /** Output collection WriteHandles */
    SG::WriteHandle<SiHitCollection>                 m_outputBCMHits;
    SG::WriteHandle<SiHitCollection>                 m_outputBLMHits;
    SG::WriteHandle<SiHitCollection>                 m_outputPixelHits;
    SG::WriteHandle<SiHitCollection>                 m_outputSCTHits;
    SG::WriteHandle<TRTUncompressedHitCollection>    m_outputTRTUncompressedHits;

    SG::WriteHandle<LArHitContainer>                 m_outputLArEMBHits;
    SG::WriteHandle<LArHitContainer>                 m_outputLArEMECHits;
    SG::WriteHandle<LArHitContainer>                 m_outputLArFCALHits;
    SG::WriteHandle<LArHitContainer>                 m_outputLArHECHits;

    SG::WriteHandle<TileHitVector>                   m_outputTileHits;
    SG::WriteHandle<TileHitVector>                   m_outputMBTSHits;

    SG::WriteHandle<CSCSimHitCollection>             m_outputCSCHits;
    SG::WriteHandle<MDTSimHitCollection>             m_outputMDTHits;
    SG::WriteHandle<RPCSimHitCollection>             m_outputRPCHits;
    SG::WriteHandle<TGCSimHitCollection>             m_outputTGCHits;
  };

}

//
// templated methods below
// 

/** Setup a vector of ReadHandles for the given vector of string StoreGate Keys */
template <typename T>
StatusCode ISF::CollectionMerger::setupReadHandleVector( const SGKeyVector_t& sgKeyVec,
                                                         ReadHandleVector_t<T>& readHandleVec ) const {
  readHandleVec.reserve( sgKeyVec.size() );

  // convert string StoreGate key to ReadHandle
  for ( const auto& sgKey: sgKeyVec ) {
    readHandleVec.emplace_back( sgKey );
  }

  // verify all ReadHandles
  for ( auto& readHandle: readHandleVec ) {
    if ( !readHandle.isValid() ) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


/** Merge all hits of inputReadHandles's collections into outputWriteHandle */
template <typename T>
void ISF::CollectionMerger::mergeCollections( const ReadHandleVector_t<T>& inputReadHandles,
                                              SG::WriteHandle<T>& outputWriteHandle ) const {
    // skip if not input collection
    if ( inputReadHandles.empty() )
        return;

    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandles
    //       and reserve the corresponding size in the outputHandle?
    outputWriteHandle = CxxUtils::make_unique<T>();

    for ( const auto& collKey: inputReadHandles ) {
        SG::ReadHandle<T>  inputHandle{collKey};

        for ( const auto& hit: *inputHandle ) {
            // TODO: replace with ->Emplace(hit) once LArHitContainer supports this
            outputWriteHandle->push_back( hit );
        }
    }
}


#endif //> !ISF_ALGS_COLLECTIONMERGER_H
