// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODFORWARDCNV_IMBTSMODULECNVTOOL_H
#define XAODFORWARDCNV_IMBTSMODULECNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
//#include "DataModel/DataVector.h"

// EDM include(s):
#include "xAODForward/MBTSModuleContainer.h"

//Forward declarations
#include "TileEvent/TileContainer.h"

namespace xAODMaker {

   /// The interface provided by IMBTSModuleCnvTool
   static const InterfaceID
   IID_IMBTSModuleCnvTool( "xAODMaker::IMBTSModuleCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating MBTSModuleContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (MBTSModule) Container into an xAOD::MBTSModuleContainer.
    *
    * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
    *
    * $Revision$
    * $Date$
    */
   class IMBTSModuleCnvTool : public virtual IAlgTool {

   public:
     /// Function that fills an existing xAOD::MBTSModuleContainer
     virtual StatusCode convert( const TileCellContainer* aod,
				 xAOD::MBTSModuleContainer* xaod) const = 0;
     
     /// Gaudi interface definition
     static const InterfaceID& interfaceID() {
       return IID_IMBTSModuleCnvTool;
     }
     
   }; // class IMBTSModuleCnvTool
  
} // namespace xAODMaker

#endif // XAODFORWARDCNV_IMBTSMODULECNVTOOL_H
