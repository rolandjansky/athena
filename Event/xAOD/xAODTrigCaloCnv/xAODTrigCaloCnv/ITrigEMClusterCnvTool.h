// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigEMClusterCnvTool.h 575635 2014-03-18 10:58:41Z aranzazu $
#ifndef XAODTRIGCALOCNV_ITRIGEMCLUSTERCNVTOOL_H
#define XAODTRIGCALOCNV_ITRIGEMCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigCalo/TrigEMClusterContainer.h"

// Forward declaration(s):
class TrigEMClusterContainer;

namespace xAODMaker {

   /// The interface provided by ITrigEMClusterCnvTool
   static const InterfaceID
   IID_ITrigEMClusterCnvTool( "xAODMaker::ITrigEMClusterCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigEMCluster from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         TrigEMCluster object from an existing AOD TrigEMCluster container into
    *         an xAOD::TrigEMCluster.
    *
    * @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2014-03-18 11:58:41 +0100 (Tue, 18 Mar 2014) $
    */
   class ITrigEMClusterCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigEMClusterContainer
      virtual StatusCode convert( const TrigEMClusterContainer* aod,
				  xAOD::TrigEMClusterContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigEMClusterCnvTool;
      }

   }; // class ITrigEMClusterCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_ITRIGEMCLUSTERCNVTOOL_H
