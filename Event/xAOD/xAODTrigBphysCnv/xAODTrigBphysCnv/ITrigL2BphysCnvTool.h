// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// // $Id:$
#ifndef xAODTrigBphysCNV_ITrigL2BphysCnvTool_H
#define xAODTrigBphysCNV_ITrigL2BphysCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphys.h"


// Forward declaration(s):
class TrigL2Bphys;

namespace xAODMaker {

	/// The interface provided by ITrigPhotonCnvTool
	static const InterfaceID
		IID_ITrigL2BphysCnvTool( "xAODMaker::ITrigL2BphysCnvTool", 1, 0 );

	/**
	 *  @short Interface for the tool creating TrigBphysContainer from an AOD
	 *
	 *         This interface is implemented by the tool that converts ...
	 */
	class ITrigL2BphysCnvTool : public virtual IAlgTool {

		public:
			/// Function that fills an existing xAOD::TrigBphysContainer
        virtual StatusCode convert( const TrigL2Bphys* aod,
                                   xAOD::TrigBphys* xaod ) = 0;

			/// Gaudi interface definition
			static const InterfaceID& interfaceID() {
				return IID_ITrigL2BphysCnvTool;
			}

	}; // class ITrigBphysCnvTool

} // namespace xAODMaker

#endif // xAODTrigBphysCNV_ITrigL2BphysCnvTool_H

