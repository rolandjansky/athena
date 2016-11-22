// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterCnvTool.h 785764 2016-11-22 15:40:18Z ssnyder $
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
    * $Revision: 785764 $
    * $Date: 2016-11-22 16:40:18 +0100 (Tue, 22 Nov 2016) $
    */
   class TrigCaloClusterCnvTool : public AthAlgTool,
				  public virtual ITrigCaloClusterCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigCaloClusterCnvTool( const std::string& type, const std::string& name,
			      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize() override;

      /// Function that fills an existing xAOD::TrigCaloCluster
      virtual StatusCode convert( const TrigCaloClusterContainer* aod,
                                  xAOD::TrigCaloClusterContainer* xaod ) const override;

   }; // class TrigCaloClusterCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_TRIGCALOCLUSTERCNVTOOL_H
