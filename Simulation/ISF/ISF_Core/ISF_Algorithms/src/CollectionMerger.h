/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGS_COLLECTIONMERGER_H
#define ISF_ALGS_COLLECTIONMERGER_H 1

// STL includes
#include <string>

// base class header
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Framework includes
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"

// ATLAS C++

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
  class CollectionMerger final : public AthReentrantAlgorithm {

  /** Allow the test class access to all methods */
  friend class ISFTesting::CollectionMerger_test;

  public:
    /** Constructor */
    CollectionMerger( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~CollectionMerger() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;
    virtual StatusCode  execute(const EventContext& ctx) const override final;

  private:
    /** Merge all hits of inputReadHandleKeys's collections into outputWriteHandleKey */
    template <typename T>
    StatusCode mergeCollections( const SG::ReadHandleKeyArray<T>& inputReadHandleKeys,
                                 const SG::WriteHandleKey<T>& outputWriteHandleKey,
                                 const EventContext& ctx ) const;

    /** Copy the given hit into the given output collection, container or DataHandle */
    template <typename HitType_t, typename OutputType_t>
      void insertCopy(const HitType_t& hit, OutputType_t& outputHandle) const;

    /** Copy the given const pointer to a hit into the given output collection,
        container or DataHandle */
    template <typename HitType_t, typename OutputType_t>
      void insertCopy(HitType_t * const hit, OutputType_t& outputHandle) const;

    /** Input collection ReadHandleKeys */
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputPLRHits{this, "InputPLRHits", {}, ""};
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputBCMHits{this, "InputBCMHits", {}, ""};
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputBLMHits{this, "InputBLMHits", {}, ""};
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputPixelHits{this, "InputPixelHits", {}, ""};
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputSCTHits{this, "InputSCTHits", {}, ""};
    SG::ReadHandleKeyArray<TRTUncompressedHitCollection> m_inputTRTUncompressedHits{this, "InputTRTUncompressedHits", {}, ""};

    SG::ReadHandleKeyArray<SiHitCollection>              m_inputITkPixelHits{this, "InputITkPixelHits", {}, ""};
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputITkStripHits{this, "InputITkStripHits", {}, ""};
    SG::ReadHandleKeyArray<SiHitCollection>              m_inputHGTDHits{this, "InputHGTDHits", {}, ""};

    SG::ReadHandleKeyArray<LArHitContainer>              m_inputLArEMBHits{this, "InputLArEMBHits", {}, ""};
    SG::ReadHandleKeyArray<LArHitContainer>              m_inputLArEMECHits{this, "InputLArEMECHits", {}, ""};
    SG::ReadHandleKeyArray<LArHitContainer>              m_inputLArFCALHits{this, "InputLArFCALHits", {}, ""};
    SG::ReadHandleKeyArray<LArHitContainer>              m_inputLArHECHits{this, "InputLArHECHits", {}, ""};

    SG::ReadHandleKeyArray<TileHitVector>                m_inputTileHits{this, "InputTileHits", {}, ""};
    SG::ReadHandleKeyArray<TileHitVector>                m_inputMBTSHits{this, "InputMBTSHits", {}, ""};

    SG::ReadHandleKeyArray<CSCSimHitCollection>          m_inputCSCHits{this, "InputCSCHits", {}, ""};
    SG::ReadHandleKeyArray<MDTSimHitCollection>          m_inputMDTHits{this, "InputMDTHits", {}, ""};
    SG::ReadHandleKeyArray<RPCSimHitCollection>          m_inputRPCHits{this, "InputRPCHits", {}, ""};
    SG::ReadHandleKeyArray<TGCSimHitCollection>          m_inputTGCHits{this, "InputTGCHits", {}, ""};

    /** Output collection WriteHandleKeys */
    SG::WriteHandleKey<SiHitCollection>                 m_outputPLRHits{this, "OutputPLRHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputBCMHits{this, "OutputBCMHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputBLMHits{this, "OutputBLMHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputPixelHits{this, "OutputPixelHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputSCTHits{this, "OutputSCTHits", "", ""};
    SG::WriteHandleKey<TRTUncompressedHitCollection>    m_outputTRTUncompressedHits{this, "OutputTRTUncompressedHits", "", ""};

    SG::WriteHandleKey<SiHitCollection>                 m_outputITkPixelHits{this, "OutputITkPixelHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputITkStripHits{this, "OutputITkStripHits", "", ""};
    SG::WriteHandleKey<SiHitCollection>                 m_outputHGTDHits{this, "OutputHGTDHits", "", ""};

    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMBHits{this, "OutputLArEMBHits", "", ""};
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArEMECHits{this, "OutputLArEMECHits", "", ""};
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArFCALHits{this, "OutputLArFCALHits", "", ""};
    SG::WriteHandleKey<LArHitContainer>                 m_outputLArHECHits{this, "OutputLArHECHits", "", ""};

    SG::WriteHandleKey<TileHitVector>                   m_outputTileHits{this, "OutputTileHits", "", ""};
    SG::WriteHandleKey<TileHitVector>                   m_outputMBTSHits{this, "OutputMBTSHits", "", ""};

    SG::WriteHandleKey<CSCSimHitCollection>             m_outputCSCHits{this, "OutputCSCHits", "", ""};
    SG::WriteHandleKey<MDTSimHitCollection>             m_outputMDTHits{this, "OutputMDTHits", "", ""};
    SG::WriteHandleKey<RPCSimHitCollection>             m_outputRPCHits{this, "OutputRPCHits", "", ""};
    SG::WriteHandleKey<TGCSimHitCollection>             m_outputTGCHits{this, "OutputTGCHits", "", ""};
  };


//
// templated methods below
//

/** Merge all hits of inputReadHandleKeys's collections into outputWriteHandleKey */
template <typename T>
inline StatusCode ISF::CollectionMerger::mergeCollections( const SG::ReadHandleKeyArray<T>& inputReadHandleKeys,
                                                           const SG::WriteHandleKey<T>& outputWriteHandleKey,
                                                           const EventContext& ctx ) const {
  // skip if not input collection
  if ( inputReadHandleKeys.empty() ) {
    return StatusCode::SUCCESS;
  }
  // TODO: is there a way to conveniently get the total number of hits in all inputReadHandleKeys
  //       and reserve the corresponding size in the outputHandle
  SG::WriteHandle<T> outputHandle{outputWriteHandleKey, ctx};
  ATH_CHECK( outputHandle.record(std::make_unique<T>()) );

  for ( const auto& collKey: inputReadHandleKeys ) {
    SG::ReadHandle<T> inputHandle{collKey, ctx};

    for ( const auto& hit: *inputHandle ) {
      this->insertCopy(hit, outputHandle);
    }
  }

  return StatusCode::SUCCESS;
}


/** Copy the given hit into the given output collection, container or DataHandle */
template <typename HitType_t, typename OutputType_t>
inline void ISF::CollectionMerger::insertCopy(const HitType_t& hit,
                                           OutputType_t& outputHandle) const {
  static_assert(!std::is_pointer<HitType_t>::value,
                "The hit provided to ISF::CollectionMerger::insertCopy(..) must not be a pointer!");
  outputHandle->Emplace( hit );
}


/** Copy the given const pointer to a hit into the given output collection, container or DataHandle */
template <typename HitType_t, typename OutputType_t>
inline void ISF::CollectionMerger::insertCopy(HitType_t * const hit,
                                           OutputType_t& outputHandle) const {
  auto&& hitCopy = std::make_unique<HitType_t>(*hit);
  outputHandle->push_back( hitCopy.release() );
}


}

#endif //> !ISF_ALGS_COLLECTIONMERGER_H
