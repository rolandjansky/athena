/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGORITHMS_RENAMEHITCOLLECTIONSALG_H
#define ISF_ALGORITHMS_RENAMEHITCOLLECTIONSALG_H 1

// STL includes
#include <string>
#include <memory>

// Framework includes
#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

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
#include <MuonSimEvent/MMSimHitCollection.h>
#include <MuonSimEvent/sTGCSimHitCollection.h>

// forward declarations
namespace ISFTesting {
  class RenameHitCollectionsAlg_test;
}


namespace ISF {

  /** @class RenameHitCollectionsAlg

      @brief Athena Algorithm that makes copies of existing
      collections in StoreGate and stores them under new keys.

  */
  class RenameHitCollectionsAlg final : public AthReentrantAlgorithm {

    /** Allow the test class access to all methods */
    friend class ISFTesting::RenameHitCollectionsAlg_test;

  public:
    /** Constructor */
    RenameHitCollectionsAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~RenameHitCollectionsAlg() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;
    virtual StatusCode  execute(const EventContext &ctx) const    override final;

  private:
    /** Initialize the given VarHandleKey */
    StatusCode initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const;

    /** Merge all hits of inputReadHandleKeys's collections into outputWriteHandleKey */
    template <typename T>
      StatusCode copyCollections( const SG::ReadHandleKey<T>& inputReadHandleKeys,
                                  const SG::WriteHandleKey<T>& outputWriteHandleKey, const EventContext &ctx) const;

    /** Copy the given hit into the given output collection, container or DataHandle */
    template <typename HitType_t, typename OutputType_t>
      void insertCopy(const HitType_t& hit, OutputType_t& outputHandle) const;

    /** Copy the given const pointer to a hit into the given output collection,
        container or DataHandle */
    template <typename HitType_t, typename OutputType_t>
      void insertCopy(HitType_t * const hit, OutputType_t& outputHandle) const;


    /** Input collection ReadHandleKeys */
    SG::ReadHandleKey<McEventCollection>        m_inputMcEventCollection {this, "InputMcEventCollection", "", "Input truth collection name"};
    SG::ReadHandleKey<SiHitCollection>              m_inputBCMHits{this, "InputBCMHits", "", ""};
    SG::ReadHandleKey<SiHitCollection>              m_inputBLMHits{this, "InputBLMHits", "", ""};
    SG::ReadHandleKey<SiHitCollection>              m_inputPixelHits{this, "InputPixelHits", "", ""};
    SG::ReadHandleKey<SiHitCollection>              m_inputSCTHits{this, "InputSCTHits", "", ""};
    SG::ReadHandleKey<TRTUncompressedHitCollection> m_inputTRTUncompressedHits{this, "InputTRTUncompressedHits", "", ""};

    SG::ReadHandleKey<LArHitContainer>              m_inputLArEMBHits{this, "InputLArEMBHits", "", ""};
    SG::ReadHandleKey<LArHitContainer>              m_inputLArEMECHits{this, "InputLArEMECHits", "", ""};
    SG::ReadHandleKey<LArHitContainer>              m_inputLArFCALHits{this, "InputLArFCALHits", "", ""};
    SG::ReadHandleKey<LArHitContainer>              m_inputLArHECHits{this, "InputLArHECHits", "", ""};
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputLArActiveCalibHits{this, "InputLArActiveCalibHits", "", ""};
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputLArInactiveCalibHits{this, "InputLArInactiveCalibHits", "", ""};
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputLArDeadCalibHits{this, "InputLArDeadCalibHits", "", ""};

    SG::ReadHandleKey<TileHitVector>                m_inputTileHits{this, "InputTileHits", "", ""};
    SG::ReadHandleKey<TileHitVector>                m_inputMBTSHits{this, "InputMBTSHits", "", ""};
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputTileActiveCalibHits{this, "InputTileActiveCalibHits", "", ""};
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputTileInactiveCalibHits{this, "InputTileInactiveCalibHits", "", ""};
    SG::ReadHandleKey<CaloCalibrationHitContainer>  m_inputTileDeadCalibHits{this, "InputTileDeadCalibHits", "", ""};

    SG::ReadHandleKey<CSCSimHitCollection>          m_inputCSCHits{this, "InputCSCHits", "", ""};
    SG::ReadHandleKey<MDTSimHitCollection>         m_inputMDTHits{this, "InputMDTHits", "", ""};
    SG::ReadHandleKey<RPCSimHitCollection>          m_inputRPCHits{this, "InputRPCHits", "", ""};
    SG::ReadHandleKey<TGCSimHitCollection>          m_inputTGCHits{this, "InputTGCHits", "", ""};
    SG::ReadHandleKey<MMSimHitCollection> m_inputMMHits{this, "InputMMHits", "", "Input MM hits name"};
    SG::ReadHandleKey<sTGCSimHitCollection> m_inputsTGCHits{this, "InputsTGCHits", "", "Input TGC hits name"};

    // Phase 2 Upgrade HitCollections
    SG::ReadHandleKey<SiHitCollection>              m_inputITkPixelHits{this, "InputITkPixelHits", "", ""};
    SG::ReadHandleKey<SiHitCollection>              m_inputITkStripHits{this, "InputITkStripHits", "", ""};
    SG::ReadHandleKey<SiHitCollection>              m_inputHGTDHits{this, "InputHGTDHits", "", ""};

    SG::ReadHandleKey<TrackRecordCollection>      m_inputCaloEntryLayer{this, "InputCaloEntryLayer", "", ""};
    SG::ReadHandleKey<TrackRecordCollection>      m_inputMuonEntryLayer{this, "InputMuonEntryLayer", "", ""};
    SG::ReadHandleKey<TrackRecordCollection>      m_inputMuonExitLayer{this, "InputMuonExitLayer", "", ""};

    /** Output collection WriteHandleKeys */
    SG::WriteHandleKey<McEventCollection>               m_outputMcEventCollection{this, "OutputMcEventCollection", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputBCMHits{this, "OutputBCMHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputBLMHits{this, "OutputBLMHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputPixelHits{this, "OutputPixelHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputSCTHits{this, "OutputSCTHits", "", ""};
    SG::WriteHandleKey<TRTUncompressedHitCollection>    m_outputTRTUncompressedHits{this, "OutputTRTUncompressedHits", "", ""};

    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMBHits{this, "OutputLArEMBHits", "", ""};
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMECHits{this, "OutputLArEMECHits", "", ""};
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArFCALHits{this, "OutputLArFCALHits", "", ""};
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArHECHits{this, "OutputLArHECHits", "", ""};
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputLArActiveCalibHits{this, "OutputLArActiveCalibHits", "", ""};
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputLArInactiveCalibHits{this, "OutputLArInactiveCalibHits", "", ""};
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputLArDeadCalibHits{this, "OutputLArDeadCalibHits", "", ""};

    SG::WriteHandleKey<TileHitVector>                   m_outputTileHits{this, "OutputTileHits", "", ""};
    SG::WriteHandleKey<TileHitVector>                   m_outputMBTSHits{this, "OutputMBTSHits", "", ""};
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputTileActiveCalibHits{this, "OutputTileActiveCalibHits", "", ""};
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputTileInactiveCalibHits{this, "OutputTileInactiveCalibHits", "", ""};
    SG::WriteHandleKey<CaloCalibrationHitContainer>     m_outputTileDeadCalibHits{this, "OutputTileDeadCalibHits", "", ""};

    SG::WriteHandleKey<CSCSimHitCollection>             m_outputCSCHits{this, "OutputCSCHits", "", ""};
    SG::WriteHandleKey<MDTSimHitCollection>            m_outputMDTHits{this, "OutputMDTHits", "", ""};
    SG::WriteHandleKey<RPCSimHitCollection>             m_outputRPCHits{this, "OutputRPCHits", "", ""};
    SG::WriteHandleKey<TGCSimHitCollection>             m_outputTGCHits{this, "OutputTGCHits", "", ""};
    SG::WriteHandleKey<MMSimHitCollection> m_outputMMHits{this, "OutputHits", "", "Output MM hits name"};
    SG::WriteHandleKey<sTGCSimHitCollection> m_outputsTGCHits{this, "OutputsTGCHits", "", "Output TGC hits name"};

    // Phase 2 Upgrade HitCollections
    SG::WriteHandleKey<SiHitCollection>              m_outputITkPixelHits{this, "OutputITkPixelHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>              m_outputITkStripHits{this, "OutputITkStripHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>              m_outputHGTDHits{this, "OutputHGTDHits", "", ""};

    SG::WriteHandleKey<TrackRecordCollection>      m_outputCaloEntryLayer{this, "OutputCaloEntryLayer", "", ""};
    SG::WriteHandleKey<TrackRecordCollection>      m_outputMuonEntryLayer{this, "OutputMuonEntryLayer", "", ""};
    SG::WriteHandleKey<TrackRecordCollection>      m_outputMuonExitLayer{this, "OutputMuonExitLayer", "", ""};
  };


  //
  // templated methods below
  //

  /** Copy all hits of inputReadHandleKey collection into outputWriteHandleKey */
  template <typename T>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<T>& inputReadHandleKey,
                                                                   const SG::WriteHandleKey<T>& outputWriteHandleKey, const EventContext &ctx ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<T> outputHandle{outputWriteHandleKey, ctx};
    ATH_CHECK( outputHandle.record(std::make_unique<T>()) );

    SG::ReadHandle<T> inputHandle{inputReadHandleKey, ctx};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }

    return StatusCode::SUCCESS;
  }


  template <>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<LArHitContainer>& inputReadHandleKey,
                                                                   const SG::WriteHandleKey<LArHitContainer>& outputWriteHandleKey, const EventContext &ctx ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<LArHitContainer> outputHandle{outputWriteHandleKey, ctx};
    ATH_CHECK( outputHandle.record(std::make_unique<LArHitContainer>()) );

    SG::ReadHandle<LArHitContainer> inputHandle{inputReadHandleKey, ctx};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }
    outputHandle->setName(inputHandle->Name());

    return StatusCode::SUCCESS;
  }


  template <>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<TileHitVector>& inputReadHandleKey,
                                                                   const SG::WriteHandleKey<TileHitVector>& outputWriteHandleKey, const EventContext &ctx ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<TileHitVector> outputHandle{outputWriteHandleKey, ctx};
    ATH_CHECK( outputHandle.record(std::make_unique<TileHitVector>()) );

    SG::ReadHandle<TileHitVector> inputHandle{inputReadHandleKey, ctx};
    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }
    outputHandle->setName(inputHandle->Name());

    return StatusCode::SUCCESS;
  }


  template <>
  inline StatusCode ISF::RenameHitCollectionsAlg::copyCollections( const SG::ReadHandleKey<TrackRecordCollection>& inputReadHandleKey,
                                                                   const SG::WriteHandleKey<TrackRecordCollection>& outputWriteHandleKey, const EventContext &ctx ) const {
    // skip if not input collection
    if ( inputReadHandleKey.key().empty() ) {
      return StatusCode::SUCCESS;
    }
    // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
    //       and reserve the corresponding size in the outputHandle
    SG::WriteHandle<TrackRecordCollection> outputHandle{outputWriteHandleKey, ctx};
    ATH_CHECK( outputHandle.record(std::make_unique<TrackRecordCollection>()) );

    SG::ReadHandle<TrackRecordCollection> inputHandle{inputReadHandleKey, ctx};
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
