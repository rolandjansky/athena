/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGEREDMSERIALISERTOOL_H
#define TRIGOUTPUTHANDLING_TRIGGEREDMSERIALISERTOOL_H

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODCore/AuxSelection.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"
#include "TrigSerializeTP/TrigSerTPTool.h"
#include "Gaudi/Parsers/Factory.h" // Needed to declare less common Property types

// Forward declarations
class DataObject;

/**
 * @class TriggerEDMSerialiserTool
 * @brief Tool responsible for filling an HLT result object with serialised EDM collections
 *
 * Its configuration is a list of EDM classes + keys + designation of dynamic variables + HLT result IDs. According to
 * this list, objects are looked up in the main store and if found the serialisation service is used to convert the
 * collections into arrays of bytes. Serialised data is wrapped info a vector of 32-bit integers and prefixed with
 * header containing description of the payload (details may change). Such fragments for subsequent collections are
 * concatenated together to form EDM payload in HLTResultMT. The HLTResultMT object stores serialised payload for all
 * HLT result ROBs and all of them are filled together following the IDs in the configuration list.
 *
 * For unpacking, the @see TriggerEDMDeserialiserAlg is used
 **/
class TriggerEDMSerialiserTool: public extends<AthAlgTool, HLTResultMTMakerTool>
{

 public:

  TriggerEDMSerialiserTool( const std::string& type,
	     const std::string& name,
	     const IInterface* parent );

  virtual ~TriggerEDMSerialiserTool() override = default;
  virtual StatusCode fill( HLT::HLTResultMT& resultToFill, const EventContext& ctx ) const override;

  virtual StatusCode  initialize() override;

 private:
  friend StatusCode tester( TriggerEDMSerialiserTool* );
  Gaudi::Property< std::vector< std::string > > m_collectionsToSerialize {
    this, "CollectionsToSerialize", {},
    "EDM streaming configuration \'collectionKeyType;module1,module2,module3\' where collectionKeyType is a string "
    "formatted like for AthenaOutputStream, e.g. TYPE#SG.aux1.aux2..etc. For xAOD classes the typedef type should be "
    "used and the _vN version number is automatically detected. For old T/P classes the persistent version has to be "
    "given. Module IDs following the semicolon are the HLT result ROB module IDs to which the collection should be "
    "written. ID=0 is the main result, other IDs are used for data scouting."
  };
  Gaudi::Property<bool> m_saveDynamic {
    this, "SaveDynamic", true, "If false skips serialising of dynamic variables. Use for test purpose only."
  };
  Gaudi::Property<std::map<uint16_t,uint32_t>> m_truncationThresholds {
    this, "TruncationThresholds", {}, "HLT result truncation thresholds. Key is module ID, value is max size in bytes"
  };
  Gaudi::Property<int> m_eventSizeHardLimitMB {
    this, "EventSizeHardLimitMB", -1,
    "Hard limit for output event size in megabytes. Set automatically in initialize from run parameters. "
    "In partition, it corresponds to DCM sbaBlockSize_MiB. Value <0 means no limit."
  };
  Gaudi::Property<float> m_fullResultTruncationFrac {
    this, "FullResultTruncationFrac", 0.8,
    "Fraction of EventSizeHardLimitMB which defines the limit on the total size of HLT output (sum of all modules)"
  };
  /// StoreGate key for the truncation debug info object
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_debugInfoWHKey {
    this, "DebugInfoWHKey", "TruncationDebugInfo"
  };


  /**
   * @class Address
   * Internal structure to keep configuration organised conveniently
   **/
  struct Address {
    enum class Category : uint8_t { xAODInterface, xAODAux, OldTP, xAODDecoration, None };

    std::string transType;
    std::string persType; // actual versioned type
    CLID clid;
    std::string key;
    std::vector<uint16_t> moduleIdVec{};
    Category category{Category::None};
    xAOD::AuxSelection sel{}; //!< xAOD dynamic variables selection, relevant only for xAODAux category

    std::string transTypeName() const {return transType+"#"+key;}
    std::string persTypeName() const {return persType+"#"+key;}
  };

  /**
   * @class TruncationInfo
   * Internal structure to keep information for truncation debugging
   **/
  struct TruncationInfo {
    const Address* addrPtr{nullptr};
    size_t size{0};
    bool recorded{false};
  };

  /// Typedef for collection of TruncationInfo objects for full event
  using TruncationInfoMap = std::unordered_map<uint16_t, std::vector<TruncationInfo>>;

  std::vector< Address > m_toSerialise; // postprocessed configuration info

  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc",
      "Service to translate class name to CLID" };
  ServiceHandle<IAthenaSerializeSvc> m_serializerSvc{ this, "Serializer", "AthenaRootSerializeSvc",
      "Service to translate transient to persistent representation" };

  ToolHandle<TrigSerTPTool> m_tpTool{ this, "TPTool", "TrigSerTPTool/TrigSerTPTool",
      "Tool to do Transient/Persistent conversion (Old EDM)"};

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

  /**
   * Parse entry from m_collectionsToSerialize and add it to m_toSerialise
   */
  StatusCode addCollectionToSerialise(const std::string& typeKeyAuxIDs, std::vector<Address>& addressVec) const;

  /**
   * Given the ID of the collection (in address arg) insert basic streaming info into the buffer.
   */
  static StatusCode makeHeader( const TriggerEDMSerialiserTool::Address& address, std::vector<uint32_t>& buffer  );

  /**
   * Copy bytes from the memory into the buffer converting from char[] to uint32_t[]
   * This function is candidate to be made global function at some point
   * and we will need also readPayload function
   */
  StatusCode fillPayload( const void* data, size_t sz, std::vector<uint32_t>& buffer ) const;


  /**
   * Place inside the buffer the serialised container (can be either TP, xAOD)
   * involves simple invocation of serialiser
   */
  StatusCode serialiseContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;
  /**
   * Place inside the buffer serialised the xAOD Aux container
   * involves selection and recording of dynamic variables
   */
  StatusCode serialisexAODAuxContainer( void* data, const Address& address, std::vector<uint32_t>& buffer, SGImplSvc* evtStore ) const;

  /**
   * Place inside the buffer the serialised old type of container
   * involves T/P conversion
   */
  StatusCode serialiseTPContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;

  /**
   * Add dynamic variables to the payload
   */
  StatusCode serialiseDynAux( DataObject* dObject, const Address& address, std::vector<uint32_t>& buffer, size_t& nDynWritten ) const;

  /**
   * Retrieve data from event store, serialise and fill the buffer
   * using one of the specific serialise methods, depending on the data type
   */
  StatusCode serialise( const Address& address, std::vector<uint32_t>& buffer, SGImplSvc* evtStore ) const;

  /**
   * Try appending serialised data to HLT result. If data would exceed truncation threshold, don't add and flag the
   * corresponding module ID as truncated.
   * @return FAILURE in case the truncation threshold is undefined
   */
  StatusCode tryAddData(HLT::HLTResultMT& hltResult, const uint16_t id, const std::vector<uint32_t>& data) const;

  /**
   * Parse the truncation debug information, fill monitoring histograms, fill and record the debug info collection
   */
  StatusCode fillDebugInfo(const TruncationInfoMap& truncationInfoMap, xAOD::TrigCompositeContainer& debugInfoCont, HLT::HLTResultMT& resultToFill, SGImplSvc* evtStore) const;

  /**
   * Obtain version from the actual type name
   */
  static std::string version( const std::string& name );

};


#endif //> !TRIGOUTPUTHANDLING_TRIGGEREDMSERIALISERTOOL_H
