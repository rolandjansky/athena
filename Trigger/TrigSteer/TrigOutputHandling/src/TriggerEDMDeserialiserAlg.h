/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGEREDMDESERIALISERALG_H
#define TRIGOUTPUTHANDLING_TRIGGEREDMDESERIALISERALG_H

#include <string>

#include "GaudiKernel/IClassIDSvc.h"
#include "TrigSteeringEvent/HLTResultMT.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "TrigSerializeTP/TrigSerTPTool.h"

#include "TList.h"


/**
 * @class TriggerEDMDeserialiserAlg unpacks EDM from the HLTResultMT 
 * Each serialised collection is a chunk of words with the content as described below.
 * The [] == one word, 
 * the [...] more words.
 * Format is as follows:
 * [fragment size in words][CLID][size of serialised collection name][...serialised collection name ...][data payload in bytes][....data payload...]
 * It follows from the TrigEDMSerialiserTool implementation.
 **/

class TriggerEDMDeserialiserAlg : public AthReentrantAlgorithm {
public:
  class WritableAuxStore;

  using Payload = std::vector<uint32_t>;
  using PayloadIterator = Payload::const_iterator;
  static constexpr size_t CLIDOffset = 1;
  static constexpr size_t NameLengthOffset = 2;
  static constexpr size_t NameOffset = 3;
  static std::unique_ptr<TList> s_streamerInfoList ATLAS_THREAD_SAFE;
  static std::mutex s_mutex;

  TriggerEDMDeserialiserAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TriggerEDMDeserialiserAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

private:
  friend StatusCode tester( TriggerEDMDeserialiserAlg* );

  void add_bs_streamerinfos();

  SG::ReadHandleKey<HLT::HLTResultMT> m_resultKey { this, "ResultKey", "HLTResultMT",
    "Key of the HLT result object"  };
  Gaudi::Property<std::string> m_prefix{ this, "Prefix", "",
    "Set for testing to avoid clash with the input collections" };
  Gaudi::Property<int> m_moduleID{ this, "ModuleID", 0,
    "Module ID of HLT result ROB, default 0 is the main HLT result, others are for TLA, calibration etc." };
  Gaudi::Property<int> m_initialSerialisationBufferSize{ this, "InitialSerialisationBufferSize", 1024*1024,
    "Initial serialisation buffer size in bytes, can be set large to avoid reallocations in every event" };
  
  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc",
    "Service to translate CLID to class name" };

  ServiceHandle<IAthenaSerializeSvc> m_serializerSvc{ this, "Serializer", "AthenaRootSerializeSvc",
    "Service that translates persistent to transient representation" };
  
  ToolHandle<TrigSerTPTool> m_tpTool{ this, "TPTool", "TrigSerTPTool/TrigSerTPTool",
    "Tool to do Transient/Persistent conversion (Old EDM)"};

  /**
   * Performs actual deserialisation loop
   */ 
  StatusCode deserialise( const Payload* dataptr ) const;

  /**
   * Handle decoration
   */
  StatusCode deserialiseDynAux( const std::string& transientTypeName, const std::string& persistentTypeName,
                                const std::string& decorationName, void* obj,
                                WritableAuxStore* currentAuxStore, SG::AuxVectorBase* interface ) const;

  /**
   * Checker for data integrity, one and only one of the passed booleans can be true,
   * else FAILURE is returned and relevant diagnostics printed.
   */
  StatusCode checkSanity( const std::string& transientTypeName, bool isxAODInterfaceContainer,
                          bool isxAODAuxContainer, bool isDecoration, bool isTPContainer ) const;

};

#endif // TRIGOUTPUTHANDLING_TRIGGEREDMDESERIALISERALG_H
