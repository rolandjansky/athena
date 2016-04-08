// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// // $Id:$
#ifndef xAODTrigBphysCNV_ITrigL2BphysContainerCnvTool_H
#define xAODTrigBphysCNV_ITrigL2BphysContainerCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"


// Forward declaration(s):
class TrigL2BphysContainer;

namespace xAODMaker {

	/// The interface provided by ITrigPhotonCnvTool
	static const InterfaceID
		IID_ITrigL2BphysContainerCnvTool( "xAODMaker::ITrigL2BphysContainerCnvTool", 1, 0 );

	/**
	 *  @short Interface for the tool creating TrigBphysContainer from an AOD
	 *
	 *         This interface is implemented by the tool that converts ...
	 */
	class ITrigL2BphysContainerCnvTool : public virtual IAlgTool {

		public:
			/// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert(const TrigL2BphysContainer* aod,
                                   xAOD::TrigBphysContainer* xaod ) = 0;

			/// Gaudi interface definition
			static const InterfaceID& interfaceID() {
				return IID_ITrigL2BphysContainerCnvTool;
			}

	}; // class ITrigL2BphysContainerCnvTool

} // namespace xAODMaker

#endif // xAODTrigBphysCNV_ITrigL2BphysContainerCnvTool_H

