// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigVertexCountsCnvTool.h 603232 2014-06-23 17:24:35Z azemla $
#ifndef xAODTrigMinBiasCNV_ITrigVertexCountsCNVTOOL_H
#define xAODTrigMinBiasCNV_ITrigVertexCountsCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"

// Forward declaration(s):
class TrigVertexCountsCollection;

namespace xAODMaker {

   /// The interface provided by ITrigVertexCountsCnvTool
   static const InterfaceID
   IID_ITrigVertexCountsCnvTool( "xAODMaker::ITrigVertexCountsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigVertexCountsContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigVertexCounts container into
    *         an xAOD::TrigVertexCountsContainer.
    */
   class ITrigVertexCountsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigVertexCountsContainer
     virtual StatusCode convert( const TrigVertexCountsCollection* aod,
                                  xAOD::TrigVertexCountsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigVertexCountsCnvTool;
      }

   }; // class ITrigVertexCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_ITrigVertexCountsCNVTOOL_H
