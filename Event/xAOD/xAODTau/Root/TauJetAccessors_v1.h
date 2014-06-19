// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAccessors_v1.h 587943 2014-03-17 12:43:37Z felixf $
#ifndef XAODTAU_TAUACCESSORS_V1_H
#define XAODTAU_TAUACCESSORS_V1_H

// Local include(s):
#include "xAODTau/versions/TauJet_v1.h"

namespace xAOD {

	namespace xAODTau {
	  /// Helper function for managing accessors to id input and output variables defined as enums in TauDefs.h
	  ///
	  /// This function holds on to Accessor objects that can be used by each
	  /// TauJet_v1 object at runtime to get/set id variable values on themselves.
	  ///
	  /// @param idvar The id variable for which an Accessor should be returned
	  /// @returns A pointer to an Accessor if successful, <code>0</code> if not
	  ///
	  SG::AuxElement::Accessor< float >*   discriminantAccessorV1( xAOD::TauJetParameters::TauID discriminant );

	  template <class T> SG::AuxElement::Accessor< T >* panTauDetailsAccessorV1( xAOD::TauJetParameters::PanTauDetails detail );

	  template <class T> SG::AuxElement::Accessor< T >* detailsAccessorV1( xAOD::TauJetParameters::Detail detail );

	#include "TauJetAccessors_v1.icc"
	}
} // namespace xAOD

#endif // XAODTAU_TAUJETACCESSORS_V1_H
