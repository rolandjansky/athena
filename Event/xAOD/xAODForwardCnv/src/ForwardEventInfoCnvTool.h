// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODFORWARDCNV_FORWARDEVENTINFOCNVTOOL_H
#define XAODFORWARDCNV_FORWARDEVENTINFOCNVTOOL_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "TileEvent/MBTSCollisionTime.h"

#include "xAODForwardCnv/IForwardEventInfoCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the tool creating ForwardEventInfoContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (ForwardEventInfo) Container into an xAOD::ForwardEventInfoContainer.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    */
  class ForwardEventInfoCnvTool : public AthAlgTool,
			  public virtual IForwardEventInfoCnvTool {

   public:
    /// Regular AlgTool constructor
    ForwardEventInfoCnvTool( const std::string& type, const std::string& name,
		     const IInterface* parent );

    /// Function that fills an existing xAOD::ForwardEventInfoContainer
    virtual StatusCode convert( const MBTSCollisionTime* aod,
				xAOD::ForwardEventInfoContainer* xaod) const override;
    
  }; // class ForwardEventInfoCnvTool

} // namespace xAODMaker

#endif // XAODFORWARDCNV_FORWARDEVENTINFOCNVTOOL_H
