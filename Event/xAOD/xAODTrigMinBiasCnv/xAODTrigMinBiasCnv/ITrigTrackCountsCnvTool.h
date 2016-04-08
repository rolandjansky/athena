// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigTrackCountsCnvTool.h 603232 2014-06-23 17:24:35Z azemla $
#ifndef xAODTrigMinBiasCNV_ITrigTrackCountsCNVTOOL_H
#define xAODTrigMinBiasCNV_ITrigTrackCountsCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"

// Forward declaration(s):
class TrigTrackCountsCollection;

namespace xAODMaker {

   /// The interface provided by ITrigTrackCountsCnvTool
   static const InterfaceID
   IID_ITrigTrackCountsCnvTool( "xAODMaker::ITrigTrackCountsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigTrackCountsContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigTrackCounts container into
    *         an xAOD::TrigTrackCountsContainer.
    */
   class ITrigTrackCountsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigTrackCountsContainer
     virtual StatusCode convert( const TrigTrackCountsCollection* aod,
                                  xAOD::TrigTrackCountsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigTrackCountsCnvTool;
      }

   }; // class ITrigTrackCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_ITrigTrackCountsCNVTOOL_H
