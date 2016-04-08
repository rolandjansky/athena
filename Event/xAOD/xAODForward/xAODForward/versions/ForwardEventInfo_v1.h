// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_FORWARDEVENTINFOS_V1_H
#define XAODFORWARD_VERSIONS_FORWARDEVENTINFOS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {
   /// add description 
   /// @author Andrzej Zemla < azemla@cern.ch >
   ///
   class ForwardEventInfo_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      ForwardEventInfo_v1();
    
      float timeDiff() const;
      void setTimeDiff(float val);

      float timeA() const;
      void setTimeA(float val);

      float timeC() const;
      void setTimeC(float val);

      unsigned short countA() const;
      void setCountA(unsigned short val);

      unsigned short countC() const;
      void setCountC(unsigned short val);

   }; // class ForwardEventInfo_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::ForwardEventInfo_v1, SG::AuxElement);

#endif // XAODFORWARD_VERSIONS_FORWARDEVENTINFOS_V1_H
