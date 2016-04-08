// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterCnvTool.h 592334 2014-04-10 11:01:56Z krasznaa $
#ifndef XAODTRIGCALOCNV_TRIGEMCLUSTERCNVTOOL_H
#define XAODTRIGCALOCNV_TRIGEMCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigCaloCnv/ITrigEMClusterCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigEMClusterCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigEMClusterCnvTool.
    *
    * @author Arantxa Ruiz Martinez <aranzazu.ruiz.martinez@cern.ch>
    *
    * $Revision: 592334 $
    * $Date: 2014-04-10 13:01:56 +0200 (Thu, 10 Apr 2014) $
    */
   class TrigEMClusterCnvTool : public AthAlgTool,
				public virtual ITrigEMClusterCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigEMClusterCnvTool( const std::string& type, const std::string& name,
			      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigEMCluster
      virtual StatusCode convert( const TrigEMClusterContainer* aod,
                                  xAOD::TrigEMClusterContainer* xaod );

   }; // class TrigEMClusterCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_TRIGEMCLUSTERCNVTOOL_H
