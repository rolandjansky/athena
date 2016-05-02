// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODEGAMMACNV_IELECTRONCNVTOOL_H
#define XAODEGAMMACNV_IELECTRONCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"
// Forward declaration(s):
class egammaContainer;

namespace xAODMaker {

   /// The interface provided by IElectronCnvTool
   static const InterfaceID
   IID_IElectronCnvTool( "xAODMaker::IElectronCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating ElectronContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (Electron) Container into an xAOD::ElectronContainer.
    *
    * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
    *
    * $Revision$
    * $Date$
    */
   class IElectronCnvTool : public virtual IAlgTool {

   public:
     /// Function that fills an existing xAOD::ElectronContainer (Trigger)
     virtual StatusCode convert( const egammaContainer* aod,
				 xAOD::ElectronContainer* xaod) const = 0;
     /// Function that fills an existing xAOD::ElectronContainer
     virtual StatusCode convert( const egammaContainer* aod,
				 xAOD::ElectronContainer* xaod,
				 xAOD::ElectronContainer* xaodFrwd) const = 0;
     
     /// Gaudi interface definition
     static const InterfaceID& interfaceID() {
       return IID_IElectronCnvTool;
     }
     
   }; // class IElectronCnvTool
  
} // namespace xAODMaker

#endif // XAODEGAMMACNV_IELECTRONCNVTOOL_H
