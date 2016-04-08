// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODEGAMMACNV_IPHOTONCNVTOOL_H
#define XAODEGAMMACNV_IPHOTONCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"

// Forward declaration(s):
class egammaContainer; 

namespace xAODMaker {

   /// The interface provided by IPhotonCnvTool
   static const InterfaceID
   IID_IPhotonCnvTool( "xAODMaker::IPhotonCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating PhotonContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (Photon) Container into an xAOD::PhotonContainer.
    *
    * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
    *
    * $Revision$
    * $Date$
    */
   class IPhotonCnvTool : public virtual IAlgTool {

   public:
     /// Function that fills an existing xAOD::PhotonContainer
     virtual StatusCode convert( const egammaContainer* aod,
				 xAOD::PhotonContainer* xaod ) const = 0;
     
     /// Gaudi interface definition
     static const InterfaceID& interfaceID() {
       return IID_IPhotonCnvTool;
     }
     
   }; // class IPhotonCnvTool
  
} // namespace xAODMaker

#endif // XAODEGAMMACNV_IPHOTONCNVTOOL_H
