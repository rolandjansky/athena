// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAccessors_v2.h 631748 2014-11-28 09:58:13Z janus $
#ifndef XAODTAU_TAUACCESSORS_V3_H
#define XAODTAU_TAUACCESSORS_V3_H

// Local include(s):
#include "xAODTau/versions/TauJet_v3.h"
#include "xAODTau/versions/TauTrack_v1.h"

namespace xAOD {

	namespace xAODTau {
	  /// Helper function for managing accessors to id input and output variables defined as enums in TauDefs.h
	  ///
	  /// This function holds on to Accessor objects that can be used by each
	  /// TauJet_v2 object at runtime to get/set id variable values on themselves.
	  ///
	  /// @param idvar The id variable for which an Accessor should be returned
	  /// @returns A pointer to an Accessor if successful, <code>0</code> if not
	  ///
#ifndef TAUTRACK_V1_ACCESSORS
	  SG::AuxElement::Accessor< float >*   discriminantAccessorV3( xAOD::TauJetParameters::TauID discriminant );

	  template <class T> SG::AuxElement::Accessor< T >* panTauDetailsAccessorV3( xAOD::TauJetParameters::PanTauDetails detail );

	  template <class T> SG::AuxElement::Accessor< T >* detailsAccessorV3( xAOD::TauJetParameters::Detail detail );

#else
	  template <class T> SG::AuxElement::Accessor< T >* trackDetailsAccessorV3( xAOD::TauJetParameters::TrackDetail detail );
#endif
	  
	#include "TauJetAccessors_v3.icc"
	}
} // namespace xAOD

#endif // XAODTAU_TAUJETACCESSORS_V3_H
