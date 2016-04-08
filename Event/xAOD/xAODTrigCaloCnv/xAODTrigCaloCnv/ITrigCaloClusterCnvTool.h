// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigCaloClusterCnvTool.h 575635 2014-03-18 10:58:41Z aranzazu $
#ifndef XAODTRIGCALOCNV_ITRIGCALOCLUSTERCNVTOOL_H
#define XAODTRIGCALOCNV_ITRIGCALOCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigCalo/TrigCaloClusterContainer.h"

// Forward declaration(s):
class TrigCaloClusterContainer;

namespace xAODMaker {

   /// The interface provided by ITrigCaloClusterCnvTool
   static const InterfaceID
   IID_ITrigCaloClusterCnvTool( "xAODMaker::ITrigCaloClusterCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigCaloCluster from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         TrigCaloCluster object from an existing AOD TrigCaloCluster container into
    *         an xAOD::TrigCaloCluster.
    *
    * @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2014-03-18 11:58:41 +0100 (Tue, 18 Mar 2014) $
    */
   class ITrigCaloClusterCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigCaloClusterContainer
      virtual StatusCode convert( const TrigCaloClusterContainer* aod,
				  xAOD::TrigCaloClusterContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigCaloClusterCnvTool;
      }

   }; // class ITrigCaloClusterCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_ITRIGCALOCLUSTERCNVTOOL_H
