/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHits_v1.cxx 687949 2015-08-06 15:48:49Z amazurov $

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXJetHits_v1.h"


namespace {


std::string hitsSourceComponent(uint8_t source)
{
    switch (source)
    {
    case xAOD::CMXJetHits_v1::Sources::REMOTE_MAIN:
      return "REMOTE_MAIN";
    case xAOD::CMXJetHits_v1::Sources::LOCAL_MAIN:
      return "LOCAL_MAIN";
    case xAOD::CMXJetHits_v1::Sources::TOTAL_MAIN:
      return "TOTAL_MAIN";
    case xAOD::CMXJetHits_v1::Sources::REMOTE_FORWARD:
      return "REMOTE_FORWARD";
    case xAOD::CMXJetHits_v1::Sources::LOCAL_FORWARD:
      return "LOCAL_FORWARD";
    case xAOD::CMXJetHits_v1::Sources::TOTAL_FORWARD:
      return "TOTAL_FORWARD";
    case xAOD::CMXJetHits_v1::Sources::TOPO_CHECKSUM:
      return "TOPO_CHECKSUM";
    case xAOD::CMXJetHits_v1::Sources::TOPO_OCCUPANCY_MAP:
      return "TOPO_OCCUPANCY_MAP";
    case xAOD::CMXJetHits_v1::Sources::TOPO_OCCUPANCY_COUNTS:
      return "TOPO_OCCUPANCY_COUNTS";
    default:
      return std::to_string(source);
    }
}


} // anonymous namespace


namespace xAOD{

  CMXJetHits_v1::CMXJetHits_v1() :
    SG::AuxElement()
  {
  }
  
  /// initialize
  void CMXJetHits_v1::initialize(const uint8_t crate,const uint8_t source)
  {
    setCrate( crate );
    setSourceComponent( source );
  }
  
  /// initialize
  void CMXJetHits_v1::initialize(const uint8_t crate,const uint8_t source,
                                 const std::vector<uint32_t>& hitsVec0,
                                 const std::vector<uint32_t>& hitsVec1,
                                 const std::vector<uint32_t>& errorVec0,
                                 const std::vector<uint32_t>& errorVec1,
                                 const uint8_t peak)
  {
    setCrate( crate );
    setSourceComponent( source );
    setHitsVec0( hitsVec0 );
    setHitsVec1( hitsVec1 );
    setErrorVec0( errorVec0 );
    setErrorVec1( errorVec1 );
    setPeak( peak );
  }
  
  
  /// add data to existing object
  void CMXJetHits_v1::addHits(const std::vector<uint32_t>& hitsVec0,
                              const std::vector<uint32_t>& hitsVec1,
                              const std::vector<uint32_t>& errorVec0,
                              const std::vector<uint32_t>& errorVec1)
  {
    setHitsVec0( hitsVec0 );
    setHitsVec1( hitsVec1 );
    setErrorVec0( errorVec0 );
    setErrorVec1( errorVec1 );    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetHits_v1 , uint8_t , crate , setCrate )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetHits_v1 , uint8_t , sourceComponent , setSourceComponent )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMXJetHits_v1 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v1 , std::vector<uint32_t> , hitsVec0 , setHitsVec0 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v1 , std::vector<uint32_t> , hitsVec1 , setHitsVec1 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v1 , std::vector<uint32_t> , errorVec0 , setErrorVec0 )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( CMXJetHits_v1 , std::vector<uint32_t> , errorVec1 , setErrorVec1 )

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get hitsVec0 at peak bunch crossing
  uint32_t CMXJetHits_v1::hits0() const
  {
    return hitsVec0()[ peak() ];
  }

  /// get hitsVec1 at peak bunch crossing
  uint32_t CMXJetHits_v1::hits1() const
  {
    return hitsVec1()[ peak() ];
  }

  /// get errorVec0 at peak bunch crossing
  uint32_t CMXJetHits_v1::error0() const
  {
    return errorVec0()[ peak() ];
  }

  /// get errorVec1 at peak bunch crossing
  uint32_t CMXJetHits_v1::error1() const
  {
    return errorVec1()[ peak() ];
  }

  std::ostream &operator<<(std::ostream &os, const xAOD::CMXJetHits_v1 &el)
  {
    os << "xAOD::CMXJetHits crate=" << int(el.crate())
       << " sourceComponent=" << hitsSourceComponent(el.sourceComponent())
       << " peak=" << int(el.peak()) << " hits0=0x" << std::hex << el.hits0() << std::dec
       << " hits1=0x" << std::hex << el.hits1() << std::dec << " hits0Vec=[";
    
    std::for_each(el.hitsVec0().begin(), el.hitsVec0().end(), [&](uint32_t n) { os << " 0x" << std::hex << int(n) << std::dec; });
    os << "]";
    return os;
  }

} // namespace xAOD

