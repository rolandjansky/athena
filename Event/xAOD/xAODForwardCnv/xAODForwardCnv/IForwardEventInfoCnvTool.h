// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODFORWARDCNV_IFORWARDEVENTINFOCNVTOOL_H
#define XAODFORWARDCNV_IFORWARDEVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
//#include "DataModel/DataVector.h"

// EDM include(s):
#include "xAODForward/ForwardEventInfoContainer.h"

//Forward declarations
class MBTSCollisionTime;

namespace xAODMaker {

   /// The interface provided by IForwardEventInfoCnvTool
   static const InterfaceID
   IID_IForwardEventInfoCnvTool( "xAODMaker::IForwardEventInfoCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating ForwardEventInfoContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (ForwardEventInfo) Container into an xAOD::ForwardEventInfoContainer.
    *
    * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
    *
    * $Revision$
    * $Date$
    */
   class IForwardEventInfoCnvTool : public virtual IAlgTool {

   public:
     /// Function that fills an existing xAOD::ForwardEventInfoContainer
     virtual StatusCode convert( const MBTSCollisionTime* aod,
				 xAOD::ForwardEventInfoContainer* xaod) const = 0;
     
     /// Gaudi interface definition
     static const InterfaceID& interfaceID() {
       return IID_IForwardEventInfoCnvTool;
     }
     
   }; // class IForwardEventInfoCnvTool
  
} // namespace xAODMaker

#endif // XAODFORWARDCNV_IFORWARDEVENTINFOCNVTOOL_H
