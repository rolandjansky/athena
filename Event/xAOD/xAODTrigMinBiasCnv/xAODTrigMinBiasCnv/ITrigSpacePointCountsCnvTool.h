// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMinBiasCNV_ITrigSpacePointCountsCNVTOOL_H
#define xAODTrigMinBiasCNV_ITrigSpacePointCountsCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"

// Forward declaration(s):
class TrigSpacePointCountsCollection;

namespace xAODMaker {

   /// The interface provided by ITrigSpacePointCountsCnvTool
   static const InterfaceID
   IID_ITrigSpacePointCountsCnvTool( "xAODMaker::ITrigSpacePointCountsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigSpacePointCountsContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigSpacePointCounts container into
    *         an xAOD::TrigSpacePointCountsContainer.
    */
   class ITrigSpacePointCountsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigSpacePointCountsContainer
      virtual StatusCode convert( const TrigSpacePointCountsCollection* aod,
                                  xAOD::TrigSpacePointCountsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigSpacePointCountsCnvTool;
      }

   }; // class ITrigSpacePointCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_ITrigSpacePointCountsCNVTOOL_H
