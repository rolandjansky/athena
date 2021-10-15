/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGORITHMS_RENAMEHITCOLLECTIONSALG_H
#define ISF_ALGORITHMS_RENAMEHITCOLLECTIONSALG_H 1

// STL includes
#include <string>

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// ATLAS C++
#include "CxxUtils/make_unique.h"

#include "GeneratorObjects/McEventCollection.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "TrackRecord/TrackRecordCollection.h"

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
  class RenameHitCollectionsAlg_test;
}


namespace ISF {

  /** @class RenameHitCollectionsAlg

      @brief Athena Algorithm that makes copies of existing
      collections in StoreGate and stores them under new keys.

  */
  class RenameHitCollectionsAlg final : public AthAlgorithm {

    /** Allow the test class access to all methods */
    friend class ISFTesting::RenameHitCollectionsAlg_test;

  public:
    /** Constructor */
    RenameHitCollectionsAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~RenameHitCollectionsAlg() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;
    virtual StatusCode  execute()    override final;

  private:
    /** Initialize the given VarHandleKey */
    StatusCode initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const;

    /** Merge all hits of inputReadHandleKeys's collections into outputWriteHandleKey */
    template <typename T>
      StatusCode copyCollections( const SG::ReadHandleKey<T>& inputReadHandleKeys,
                                   SG::WriteHandleKey<T>& outputWriteHandleKey ) const;

    /** Copy the given hit into the given output collection, container or DataHandle */
    template <typename HitType_t, typename OutputType_t>
      void insertCopy(const HitType_t& hit, OutputType_t& outputHandle) const;

    /** Copy the given const pointer to a hit into the given output collection,
        container or DataHandle */
    template <typename HitType_t, typename OutputType_t>
      void insertCopy(HitType_t * const hit, OutputType_t& outputHandle) const;


    /** Input collection ReadHandleKeys */
    SG::ReadHandleKey<McEventCollection>            m_inputMcEventCollection;
    SG::ReadHandleKey<SiHitCollection>              m_inputBCMHits;
    SG::ReadHandleKey<SiHitCollection>              m_inputBLMHits;
    SG::ReadHandleKey<SiHitCollection>              m_inputPixelHits;
    SG::ReadHandleKey<SiHitCollection>              m_inputSCTHits;
    SG::ReadHandleKey<TRTUncompressedHitCollection> m_inputTRTUncompressedHits;

    SG::ReadHandleKey<LArHitContainer>              m_inputLArEMBHits;
    SG::ReadHandleKey<LArHitContainer>              m_inputLArEMECHits;
    SG::ReadHandleKey<LArHitContainer>              m_inputLArFCALHits;
    SG::ReadHandleKey<LArHitContainer>              m_inputLArMiniFCALHits;
    SG::ReadHandleKey<LArHitContainer>              m_inputLArHECHits;
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputLArActiveCalibHits;
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputLArInactiveCalibHits;
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputLArDeadCalibHits;

    SG::ReadHandleKey<TileHitVector>                m_inputTileHits;
    SG::ReadHandleKey<TileHitVector>                m_inputMBTSHits;
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputTileActiveCalibHits;
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputTileInactiveCalibHits;
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputTileDeadCalibHits;

    SG::ReadHandleKey<CSCSimHitCollection>          m_inputCSCHits;
    SG::ReadHandleKey<MDTSimHitCollection>          m_inputMDTHits;
    SG::ReadHandleKey<RPCSimHitCollection>          m_inputRPCHits;
    SG::ReadHandleKey<TGCSimHitCollection>          m_inputTGCHits;

    SG::ReadHandleKey<TrackRecordCollection>      m_inputCaloEntryLayer;
    SG::ReadHandleKey<TrackRecordCollection>      m_inputMuonEntryLayer;
    SG::ReadHandleKey<TrackRecordCollection>      m_inputMuonExitLayer;

    /** Output collection WriteHandleKeys */
    SG::WriteHandleKey<McEventCollection>               m_outputMcEventCollection;
    SG::WriteHandleKey<SiHitCollection>                 m_outputBCMHits;
    SG::WriteHandleKey<SiHitCollection>                 m_outputBLMHits;
    SG::WriteHandleKey<SiHitCollection>                 m_outputPixelHits;
    SG::WriteHandleKey<SiHitCollection>                 m_outputSCTHits;
    SG::WriteHandleKey<TRTUncompressedHitCollection>    m_outputTRTUncompressedHits;

    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMBHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMECHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArFCALHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArMiniFCALHits;
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArHECHits;
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputLArActiveCalibHits;
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputLArInactiveCalibHits;
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputLArDeadCalibHits;

    SG::WriteHandleKey<TileHitVector>                   m_outputTileHits;
    SG::WriteHandleKey<TileHitVector>                   m_outputMBTSHits;
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputTileActiveCalibHits;
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputTileInactiveCalibHits;
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputTileDeadCalibHits;

    SG::WriteHandleKey<CSCSimHitCollection>             m_outputCSCHits;
    SG::WriteHandleKey<MDTSimHitCollection>             m_outputMDTHits;
    SG::WriteHandleKey<RPCSimHitCollection>             m_outputRPCHits;
    SG::WriteHandleKey<TGCSimHitCollection>             m_outputTGCHits;

    SG::WriteHandleKey<TrackRecordCollection>      m_outputCaloEntryLayer;
    SG::WriteHandleKey<TrackRecordCollection>      m_outputMuonEntryLayer;
    SG::WriteHandleKey<TrackRecordCollection>      m_outputMuonExitLayer;
  };


  //
  // templated methods below
  //

  /** Copy all hits of inputReadHandleKey collection into outputWriteHandleKey */
  template <typename T>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<T>& inputReadHandleKey,
                                                                   SG::WriteHandleKey<T>& outputWriteHandleKey ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<T> outputHandle{outputWriteHandleKey};
    ATH_CHECK( outputHandle.record(std::make_unique<T>()) );

    SG::ReadHandle<T> inputHandle{inputReadHandleKey};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }

    return StatusCode::SUCCESS;
  }


  template <>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<LArHitContainer>& inputReadHandleKey,
                                                                   SG::WriteHandleKey<LArHitContainer>& outputWriteHandleKey ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<LArHitContainer> outputHandle{outputWriteHandleKey};
    ATH_CHECK( outputHandle.record(std::make_unique<LArHitContainer>()) );

    SG::ReadHandle<LArHitContainer> inputHandle{inputReadHandleKey};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }
    outputHandle->setName(inputHandle->Name());

    return StatusCode::SUCCESS;
  }


  template <>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<TileHitVector>& inputReadHandleKey,
                                                                   SG::WriteHandleKey<TileHitVector>& outputWriteHandleKey ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<TileHitVector> outputHandle{outputWriteHandleKey};
    ATH_CHECK( outputHandle.record(std::make_unique<TileHitVector>()) );

    SG::ReadHandle<TileHitVector> inputHandle{inputReadHandleKey};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }
    outputHandle->setName(inputHandle->Name());

    return StatusCode::SUCCESS;
  }


  template <>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<TrackRecordCollection>& inputReadHandleKey,
                                                                   SG::WriteHandleKey<TrackRecordCollection>& outputWriteHandleKey ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<TrackRecordCollection> outputHandle{outputWriteHandleKey};
    ATH_CHECK( outputHandle.record(std::make_unique<TrackRecordCollection>()) );

    SG::ReadHandle<TrackRecordCollection> inputHandle{inputReadHandleKey};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }
    outputHandle->setName(inputHandle->Name());

    return StatusCode::SUCCESS;
  }


  /** Copy the given hit into the given output collection, container or DataHandle */
  template <typename HitType_t, typename OutputType_t>
  inline void ISF::RenameHitCollectionsAlg::insertCopy(const HitType_t& hit,
                                                       OutputType_t& outputHandle) const {
    static_assert(!std::is_pointer<HitType_t>::value,
                  "The hit provided to ISF::RenameHitCollectionsAlg::insertCopy(..) must not be a pointer!");
    outputHandle->Emplace( hit );
  }


  /** Copy the given const pointer to a hit into the given output collection, container or DataHandle */
  template <typename HitType_t, typename OutputType_t>
  inline void ISF::RenameHitCollectionsAlg::insertCopy(HitType_t * const hit,
                                                       OutputType_t& outputHandle) const {
    auto&& hitCopy = std::make_unique<HitType_t>(*hit);
    outputHandle->push_back( hitCopy.release() );
  }


}

#endif //> !ISF_ALGORITHMS_RENAMEHITCOLLECTIONSALG_H
