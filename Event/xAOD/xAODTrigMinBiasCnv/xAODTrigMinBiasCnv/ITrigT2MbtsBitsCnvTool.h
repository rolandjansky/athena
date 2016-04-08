// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigT2MbtsBitsCnvTool.h 608072 2014-07-23 00:27:40Z azemla $
#ifndef xAODTrigMinBiasCNV_ITrigT2MbtsBitsCNVTOOL_H
#define xAODTrigMinBiasCNV_ITrigT2MbtsBitsCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"

// Forward declaration(s):
class TrigT2MbtsBitsContainer;

namespace xAODMaker {

   /// The interface provided by ITrigT2MbtsBitsCnvTool
   static const InterfaceID
   IID_ITrigT2MbtsBitsCnvTool( "xAODMaker::ITrigT2MbtsBitsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigT2MbtsBitsContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigT2MbtsBits container into
    *         an xAOD::TrigT2MbtsBitsContainer.
    */
   class ITrigT2MbtsBitsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigT2MbtsBitsContainer
     virtual StatusCode convert( const TrigT2MbtsBitsContainer* aod,
                                  xAOD::TrigT2MbtsBitsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigT2MbtsBitsCnvTool;
      }

   }; // class ITrigT2MbtsBitsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_ITrigT2MbtsBitsCNVTOOL_H
