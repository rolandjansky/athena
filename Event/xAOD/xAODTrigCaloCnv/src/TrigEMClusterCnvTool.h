// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
    * $Revision: 785764 $
    * $Date: 2016-11-22 16:40:18 +0100 (Tue, 22 Nov 2016) $
    */
   class TrigEMClusterCnvTool : public AthAlgTool,
				public virtual ITrigEMClusterCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigEMClusterCnvTool( const std::string& type, const std::string& name,
			      const IInterface* parent );

      /// Function that fills an existing xAOD::TrigEMCluster
      virtual StatusCode convert( const TrigEMClusterContainer* aod,
                                  xAOD::TrigEMClusterContainer* xaod ) const override;

   }; // class TrigEMClusterCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_TRIGEMCLUSTERCNVTOOL_H
