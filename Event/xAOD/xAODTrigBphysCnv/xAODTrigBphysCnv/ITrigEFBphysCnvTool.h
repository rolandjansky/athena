// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// // $Id:$
#ifndef xAODTrigBphysCNV_ITrigEFBphysCnvTool_H
#define xAODTrigBphysCNV_ITrigEFBphysCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphys.h"


// Forward declaration(s):
class TrigEFBphys;

namespace xAODMaker {

	/// The interface provided by ITrigPhotonCnvTool
	static const InterfaceID
		IID_ITrigEFBphysCnvTool( "xAODMaker::ITrigEFBphysCnvTool", 1, 0 );

	/**
	 *  @short Interface for the tool creating TrigBphysContainer from an AOD
	 *
	 *         This interface is implemented by the tool that converts ...
	 */
	class ITrigEFBphysCnvTool : public virtual IAlgTool {

		public:
			/// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert(const TrigEFBphys* aod,
                                   xAOD::TrigBphys* xaod ) = 0;

			/// Gaudi interface definition
			static const InterfaceID& interfaceID() {
				return IID_ITrigEFBphysCnvTool;
			}

	}; // class ITrigEFBphysCnvTool

} // namespace xAODMaker

#endif // xAODTrigBphysCNV_ITrigEFBphysCnvTool_H

