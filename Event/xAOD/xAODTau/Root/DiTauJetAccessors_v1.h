// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJetAccessors_v1.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAU_DITAUACCESSORS_V1_H
#define XAODDITAU_DITAUACCESSORS_V1_H

// Local include(s):
#include "xAODTau/versions/DiTauJet_v1.h"
#include "xAODTau/DiTauDefs.h"


namespace xAOD {

    namespace xAODDiTau {

      template <class T> SG::AuxElement::Accessor< T >* detailsAccessorV1( xAOD::DiTauJetParameters::Detail detail );

    #include "DiTauJetAccessors_v1.icc"
    }
} // namespace xAOD

#endif // XAODDITAU_TAUJETACCESSORS_V1_H