// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_MBTSMODULES_V1_H
#define XAODFORWARD_VERSIONS_MBTSMODULES_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include <stdint.h>

namespace xAOD {
   /// add description 
   /// @author Andrzej Zemla < azemla@cern.ch >
   ///
   class MBTSModule_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      MBTSModule_v1();

      float e() const;
      void setE( float val_e);
      
      float eta() const;
      void setEta( float val_eta);

      float phi() const; 
      void setPhi( float val_phi);

      float time() const;
      void setTime( float val_time);

      int mbtsQuality() const;
      void setMbtsQuality( int val_quality);

      int type() const;
      void setType( int val_type);

      int mbtsModule() const;
      void setMbtsModule( int val_module);

      int channel() const;
      void setChannel( int val_channel);

   }; // class MBTSModule_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::MBTSModule_v1, SG::AuxElement);

#endif // XAODFORWARD_VERSIONS_MBTSMODULES_V1_H
