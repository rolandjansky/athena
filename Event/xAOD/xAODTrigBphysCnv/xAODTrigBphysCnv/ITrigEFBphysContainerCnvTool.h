// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// // $Id:$
#ifndef xAODTrigBphysCNV_ITrigEFBphysContainerCnvTool_H
#define xAODTrigBphysCNV_ITrigEFBphysContainerCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"


// Forward declaration(s):
class TrigEFBphysContainer;

namespace xAODMaker {

	/// The interface provided by ITrigPhotonCnvTool
	static const InterfaceID
		IID_ITrigEFBphysContainerCnvTool( "xAODMaker::ITrigEFBphysContainerCnvTool", 1, 0 );

	/**
	 *  @short Interface for the tool creating TrigBphysContainer from an AOD
	 *
	 *         This interface is implemented by the tool that converts ...
	 */
	class ITrigEFBphysContainerCnvTool : public virtual IAlgTool {

		public:
			/// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert(const TrigEFBphysContainer* aod,
                                   xAOD::TrigBphysContainer* xaod ) = 0;

			/// Gaudi interface definition
			static const InterfaceID& interfaceID() {
				return IID_ITrigEFBphysContainerCnvTool;
			}

	}; // class ITrigEFBphysContainerCnvTool

} // namespace xAODMaker

#endif // xAODTrigBphysCNV_ITrigEFBphysContainerCnvTool_H

