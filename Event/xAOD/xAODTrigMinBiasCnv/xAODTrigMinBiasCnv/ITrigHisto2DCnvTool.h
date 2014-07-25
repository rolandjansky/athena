// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigHisto2DCnvTool.h 603232 2014-06-23 17:24:35Z azemla $
#ifndef xAODTrigMinBiasCNV_ITrigHisto2DCNVTOOL_H
#define xAODTrigMinBiasCNV_ITrigHisto2DCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigHisto2DContainer.h"

// Forward declaration(s):
class TrigHisto2D;

namespace xAODMaker {

   /// The interface provided by ITrigHisto2DCnvTool
   static const InterfaceID
   IID_ITrigHisto2DCnvTool( "xAODMaker::ITrigHisto2DCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigHisto2DContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigHisto2D container into
    *         an xAOD::TrigHisto2DContainer.
    */
   class ITrigHisto2DCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigHisto2DContainer
      virtual StatusCode convert( const TrigHisto2D* aod,
                                  xAOD::TrigHisto2DContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigHisto2DCnvTool;
      }

   }; // class ITrigHisto2DCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_ITrigHisto2DCNVTOOL_H
