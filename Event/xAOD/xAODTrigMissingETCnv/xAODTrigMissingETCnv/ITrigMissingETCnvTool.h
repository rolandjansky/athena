// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigMissingETCnvTool.h 592539 2014-04-11 10:17:29Z krasznaa $
#ifndef xAODTrigMissingETCNV_ITrigMissingETCNVTOOL_H
#define xAODTrigMissingETCNV_ITrigMissingETCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMissingET/TrigMissingETContainer.h"

// Forward declaration(s):
class TrigMissingETContainer;

namespace xAODMaker {

   /// The interface provided by ITrigMissingETCnvTool
   static const InterfaceID
   IID_ITrigMissingETCnvTool( "xAODMaker::ITrigMissingETCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigMissingETContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigMissingET container into
    *         an xAOD::TrigMissingETContainer.
    */
   class ITrigMissingETCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigMissingETContainer
      virtual StatusCode convert( const TrigMissingETContainer* aod,
                                  xAOD::TrigMissingETContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigMissingETCnvTool;
      }

   }; // class ITrigMissingETCnvTool

} // namespace xAODMaker

#endif // xAODTrigMissingETCNV_ITrigMissingETCNVTOOL_H
