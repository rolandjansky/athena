/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeader_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/RODHeader_v1.h"

namespace xAOD{  
  
  RODHeader_v1::RODHeader_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , version , setVersion )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , sourceId , setSourceId )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , run , setRun )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , lvl1Id , setLvl1Id )    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , bcid , setBcid )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , trigType , setTrigType )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , detType , setDetType )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( RODHeader_v1 , std::vector<unsigned int> , statusWords , setStatusWords )    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( RODHeader_v1 , unsigned int , payloadSize , setPayloadSize )  
  
  // Header data
  int RODHeader_v1::majorVersion()  const
  {
    return (version() >> 16) & 0xffff;
  }
  
  int RODHeader_v1::minorVersion()  const  
  {
    return version() & 0xffff;
  }
  
  int RODHeader_v1::sourceID()      const
  {
    return sourceId();
  }
    
  int RODHeader_v1::subDetectorID() const  
  {
    return (sourceId() >> 16) & 0xff;
  }
  
  int RODHeader_v1::moduleID()      const  
  {
    return sourceId() & 0xffff; 
  }
  
  int RODHeader_v1::crate()         const  
  {
    return sourceId() & 0xf; 
  }
  
  int RODHeader_v1::sLink()         const  
  {
    return (sourceId() >> 4) & 0x3; 
  }
  
  int RODHeader_v1::dataType()      const  
  {
   return (sourceId() >> 7) & 0x1; 
  }
  
  int RODHeader_v1::runType()       const  
  {
    return (run() >> 24) & 0xff;  
  }
  
  int RODHeader_v1::runNumber()     const  
  {
    return run() & 0xffffff;  
  }
  
  int RODHeader_v1::extendedL1ID()  const  
  {
    return lvl1Id();
  }
  
  int RODHeader_v1::ecrID()         const  
  {
    return (lvl1Id() >> 24) & 0xff;  
  }
  
  int RODHeader_v1::l1ID()          const  
  {
    return lvl1Id() & 0xffffff;  
  }
  
  int RODHeader_v1::bunchCrossing() const  
  {
    return bcid();
  }
  
  int RODHeader_v1::l1TriggerType() const  
  {
    return trigType();
  }
  
  int RODHeader_v1::detEventType()  const  
  {
    return detType();
  }
  
  int RODHeader_v1::orbitCount()    const  
  {
    return (detType() >> 16) & 0xffff;  
  }
  
  int RODHeader_v1::stepNumber()    const  
  {
    return (detType() >> 4) & 0xfff;  
  }
  
  int RODHeader_v1::stepType()      const  
  {
    return detType() & 0xf;  
  }
  

  // Status bits - word 1
  bool RODHeader_v1::bcnMismatch()        const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x1;  
  }
  
  bool RODHeader_v1::gLinkTimeout()       const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x4;  
  }
  
  bool RODHeader_v1::dataTransportError() const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x8;  
  }
  
  bool RODHeader_v1::rodFifoOverflow()    const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x10;  
  }
  
  bool RODHeader_v1::lvdsLinkError()      const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x10000;  
  }
  
  bool RODHeader_v1::cmmParityError()     const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x20000;  
  }
  
  bool RODHeader_v1::gLinkError()         const  
  {
    return (statusWords().empty()) ? false : statusWords()[ 0 ] & 0x40000;  
  }
  
  // Status bits - word 2
  bool RODHeader_v1::limitedRoISet()      const  
  {
    return (statusWords().size() < 2) ? false : statusWords()[ 1 ] & 0x2;  
  }
  
  bool RODHeader_v1::triggerTypeTimeout() const  
  {
    return (statusWords().size() < 2) ? false : statusWords()[ 1 ] & 0x10000;  
  }
       
      
} // namespace xAOD
