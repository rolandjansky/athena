/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGEREDMDESERIALISERALG_H
#define TRIGOUTPUTHANDLING_TRIGGEREDMDESERIALISERALG_H

#include <string>

#include "AthenaKernel/IClassIDSvc.h"
#include "TrigSteeringEvent/HLTResultMT.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"




/**
 * @class TriggerEDMDeserialiserAlg unpacks EDM from the HLTResultMT 
 * Each serialised collection is a chunk of words with the content as described below.
 * The [] == one word, 
 * the [...] more words.
 * Format is as follows:
 * [fragment size in words][CLID][size of serialised colection name][...serialised collection name ...][data payload in bytes][....data paload...]
 * It follows form the TrigEDMSerialisationTool implemetation.
 **/
class TriggerEDMDeserialiserAlg : public AthReentrantAlgorithm {
public:
  enum Offsets {
    CLIDOffset = 1,
    NameLengthOffset = 2,
    NameOffset = 3
  };
  TriggerEDMDeserialiserAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TriggerEDMDeserialiserAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::ReadHandleKey<HLT::HLTResultMT> m_resultKey { this, "ResultKey", "HLTResultMT", "Key of object that is read"  };
  Gaudi::Property<std::string> m_prefix{ this, "Prefix", "", "Set for testing to avoid clash with the input collections" };
  Gaudi::Property<int> m_moduleID{ this, "ModuleID", 0, "Module ID of HLT result ROB, default 0 is the main HLT result, others are for TLA, calibration etc." };
  Gaudi::Property<int> m_initialSerialisationBufferSize{ this, "InitialSerialisationBufferSize", 1024*1024, "Initial serialisation buffer size in bytes, can be set large to avoid reallocations in every event" };
  
  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc", "Service to translate CLID to class namex" };

  ServiceHandle<IAthenaSerializeSvc> m_serializerSvc{ this, "Serializer", "AthenaRootSerializeSvc", "Service that translates persistent to transient respresenation" };
  
  typedef  std::vector<uint32_t> Payload;
  typedef  std::vector<uint32_t>::const_iterator PayloadIterator;
  
  /**
   * returns begining of the next fragment starting point, can be == end()
   * intended to be used like this: start = advance(start); if ( start != data.end() )... decode else ... done
   **/  
  inline PayloadIterator toNextFragment( PayloadIterator start ) const {
    return start + (*start) ; // point ahead by the number of words pointed to by start iterator
  }
  /**
   * CLID of the collection stored in the next fragment
   **/
  inline CLID collectionCLID( PayloadIterator start  ) const {
    return *( start + CLIDOffset );
  }
  /**
   * Len of serialised name payload
   **/
  size_t nameLength( TriggerEDMDeserialiserAlg::PayloadIterator start ) const;

  /**
   * name of the collection stored in the next fragment
   **/
  std::string collectionName( PayloadIterator start ) const;
  /**
   * size of the buffer that is needed to decode next fragment data content
   * @warning measured in bytes
   **/  
  size_t dataSize( PayloadIterator start ) const;

  /**
   * copies fragment to the buffer, no size checking, use above to do so
   **/  
  void toBuffer( PayloadIterator start, char* buffer ) const;
  
};

#endif // TRIGOUTPUTHANDLING_TRIGGEREDMDESERIALISERALG_H
