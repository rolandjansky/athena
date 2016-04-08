// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterCnvTool.h 592334 2014-04-10 11:01:56Z krasznaa $
#ifndef XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVTOOL_H
#define XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigCaloCnv/ITrigCaloClusterCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigCaloClusterCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigCaloClusterCnvTool.
    *
    * @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>
    *
    * $Revision: 592334 $
    * $Date: 2014-04-10 13:01:56 +0200 (Thu, 10 Apr 2014) $
    */
   class TrigCaloClusterCnvTool : public AthAlgTool,
				  public virtual ITrigCaloClusterCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigCaloClusterCnvTool( const std::string& type, const std::string& name,
			      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigCaloCluster
      virtual StatusCode convert( const TrigCaloClusterContainer* aod,
                                  xAOD::TrigCaloClusterContainer* xaod );

   }; // class TrigCaloClusterCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVTOOL_H
