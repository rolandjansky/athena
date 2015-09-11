/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeader_v2.cxx 694350 2015-09-11 12:02:19Z krasznaa $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/RODHeader_v2.h"

namespace xAOD{  
  
  RODHeader_v2::RODHeader_v2() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , version , setVersion )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , sourceId , setSourceId )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , run , setRun )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , lvl1Id , setLvl1Id )    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , bcid , setBcid )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , trigType , setTrigType )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , lvl1DetType , setLvl1DetType )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( RODHeader_v2 , std::vector<unsigned int> , statusWords , setStatusWords )    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v2 , unsigned int , payloadSize , setPayloadSize )  
  
  void RODHeader_v2::initialize(uint32_t version, uint32_t sourceId, uint32_t run,
                          uint32_t lvl1Id,  uint32_t bcId,     uint32_t trigType,
                          uint32_t lvl1DetType, const std::vector<uint32_t>& statusWords,
                          uint32_t nData) {
    setVersion(version);
    setSourceId(sourceId);
    setRun(run);
    setLvl1Id(lvl1Id);
    setBcid(bcId);
    setTrigType(trigType);
    setLvl1DetType(lvl1DetType);
    setStatusWords(statusWords);
    setPayloadSize(nData);
  }


  // Header data
  int RODHeader_v2::majorVersion()  const
  {
    return (version() >> 16) & 0xffff;
  }
  
  int RODHeader_v2::minorVersion()  const  
  {
    return version() & 0xffff;
  }
  
  int RODHeader_v2::sourceID()      const
  {
    return sourceId();
  }
    
  int RODHeader_v2::subDetectorID() const  
  {
    return (sourceId() >> 16) & 0xff;
  }
  
  int RODHeader_v2::moduleID()      const  
  {
    return sourceId() & 0xffff; 
  }
  
  int RODHeader_v2::crate()         const  
  {
    return sourceId() & 0xf; 
  }
  
  int RODHeader_v2::sLink()         const  
  {
    return (sourceId() >> 4) & 0x3; 
  }
  
  int RODHeader_v2::dataType()      const  
  {
   return (sourceId() >> 7) & 0x1; 
  }
  
  int RODHeader_v2::runType()       const  
  {
    return (run() >> 24) & 0xff;  
  }
  
  int RODHeader_v2::runNumber()     const  
  {
    return run() & 0xffffff;  
  }
  
  int RODHeader_v2::extendedL1ID()  const  
  {
    return lvl1Id();
  }
  
  int RODHeader_v2::ecrID()         const  
  {
    return (lvl1Id() >> 24) & 0xff;  
  }
  
  int RODHeader_v2::l1ID()          const  
  {
    return lvl1Id() & 0xffffff;  
  }
  
  int RODHeader_v2::bunchCrossing() const  
  {
    return bcid();
  }
  
  int RODHeader_v2::l1TriggerType() const  
  {
    return trigType();
  }
  
  int RODHeader_v2::detEventType()  const  
  {
    return lvl1DetType();
  }
  
  int RODHeader_v2::orbitCount()    const  
  {
    return (lvl1DetType() >> 16) & 0xffff;  
  }
  
  int RODHeader_v2::stepNumber()    const  
  {
    return (lvl1DetType() >> 4) & 0xfff;  
  }
  
  int RODHeader_v2::stepType()      const  
  {
    return lvl1DetType() & 0xf;  
  }
  

  // Status bits - word 1
  bool RODHeader_v2::bcnMismatch()        const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x1;  
  }
  
  bool RODHeader_v2::gLinkTimeout()       const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x4;  
  }
  
  bool RODHeader_v2::dataTransportError() const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x8;  
  }
  
  bool RODHeader_v2::rodFifoOverflow()    const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x10;  
  }
  
  bool RODHeader_v2::lvdsLinkError()      const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x10000;  
  }
  
  bool RODHeader_v2::cmmParityError()     const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x20000;  
  }
  
  bool RODHeader_v2::gLinkError()         const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x40000;  
  }
  
  // Status bits - word 2
  bool RODHeader_v2::limitedRoISet()      const  
  {
    return (statusWords().size() < 2) ? false : statusWords()[ 1 ] & 0x2;  
  }
  
  bool RODHeader_v2::triggerTypeTimeout() const  
  {
    return (statusWords().size() < 2) ? false : statusWords()[ 1 ] & 0x10000;  
  }
       
      
} // namespace xAOD
