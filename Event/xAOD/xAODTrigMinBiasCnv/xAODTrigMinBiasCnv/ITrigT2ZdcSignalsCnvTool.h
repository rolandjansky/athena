// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigT2ZdcSignalsCnvTool.h 608072 2014-07-23 00:27:40Z azemla $
#ifndef xAODTrigMinBiasCNV_ITrigT2ZdcSignalsCNVTOOL_H
#define xAODTrigMinBiasCNV_ITrigT2ZdcSignalsCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigT2ZdcSignalsContainer.h"

// Forward declaration(s):
class TrigT2ZdcSignalsContainer;

namespace xAODMaker {

   /// The interface provided by ITrigT2ZdcSignalsCnvTool
   static const InterfaceID
   IID_ITrigT2ZdcSignalsCnvTool( "xAODMaker::ITrigT2ZdcSignalsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigT2ZdcSignalsContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigT2ZdcSignals container into
    *         an xAOD::TrigT2ZdcSignalsContainer.
    */
   class ITrigT2ZdcSignalsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigT2ZdcSignalsContainer
     virtual StatusCode convert( const TrigT2ZdcSignalsContainer* aod,
                                  xAOD::TrigT2ZdcSignalsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigT2ZdcSignalsCnvTool;
      }

   }; // class ITrigT2ZdcSignalsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_ITrigT2ZdcSignalsCNVTOOL_H
