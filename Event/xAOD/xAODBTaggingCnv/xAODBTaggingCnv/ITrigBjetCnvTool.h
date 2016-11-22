// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigBjetCnvTool.h 785653 2016-11-22 06:07:48Z ssnyder $ 
#ifndef xAODBTaggingCNV_ITrigBjetCNVTOOL_H
#define xAODBTaggingCNV_ITrigBjetCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODBTagging/BTaggingContainer.h"


// Forward declaration(s):
class TrigEFBjet;
class TrigEFBjetContainer;


namespace xAODMaker {

   /// The interface provided by ITrigBjetCnvTool
   static const InterfaceID
   IID_ITrigBjetCnvTool( "xAODMaker::ITrigBjetCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigBjetContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD TrigBjet container into
    *         an xAOD::BjetContainer.
    */
   class ITrigBjetCnvTool : public virtual IAlgTool {

   public:
      // default /// Function that fills an existing xAOD::TrigBjetContainer
      // virtual StatusCode convert( const TrigBjet* aod,
      //                             xAOD::TrigBjetContainer* xaod ) = 0;
      /// Function that fills an existing xAOD::TrigBjetContainer
       virtual StatusCode convert( const TrigEFBjetContainer* aod,
                                   xAOD::BTaggingContainer* xaod ) const = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigBjetCnvTool;
      }

   }; // class ITrigBjetCnvTool

} // namespace xAODMaker

#endif //  xAODBTaggingCNV_ITrigBjetCNVTOOL_H

