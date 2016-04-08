// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJETCNV_IJETCNVTOOL_H
#define XAODJETCNV_IJETCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/DataLink.h"

class JetCollection;

namespace xAODMaker {

   /// The interface provided by IJetCnvTool
   static const InterfaceID
   IID_IJetCnvTool( "xAODMaker::IJetCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating JetContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         JetCollection from an existing AOD into
    *         an xAOD::JetContainer.
    *
    * @author P-A Delsart
    *
    * $Revision$
    * $Date$
    */
   class IJetCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::JetContainer
      virtual StatusCode convert( const JetCollection* aodCont,
                                  xAOD::JetContainer* xaodCont,
                                  DataLink<xAOD::IParticleContainer> xaodConstitCont = DataLink<xAOD::IParticleContainer>(),
                                  bool constitSearch = false) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IJetCnvTool;
      }

   }; // class IJetCnvTool

} // namespace xAODMaker

#endif // XAODJETCNV_IJETCNVTOOL_H
