// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODFORWARDCNV_MBTSMODULECNVTOOL_H
#define XAODFORWARDCNV_MBTSMODULECNVTOOL_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "TileEvent/TileCellContainer.h"

#include "xAODForwardCnv/IMBTSModuleCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the tool creating MBTSModuleContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (MBTSModule) Container into an xAOD::MBTSModuleContainer.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    *
    * $Revision$
    * $Date$
    */
  class MBTSModuleCnvTool : public AthAlgTool,
			  public virtual IMBTSModuleCnvTool {

   public:
    /// Regular AlgTool constructor
    MBTSModuleCnvTool( const std::string& type, const std::string& name,
		     const IInterface* parent );

    /// Function initializing the tool
    virtual StatusCode initialize();

    /// Function that fills an existing xAOD::MBTSModuleContainer 
    virtual StatusCode convert( const TileCellContainer* aod,
				xAOD::MBTSModuleContainer* xaod) const;
    
  }; // class MBTSModuleCnvTool

} // namespace xAODMaker

#endif // XAODFORWARDCNV_MBTSMODULECNVTOOL_H
