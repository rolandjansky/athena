///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// METHelpers.h
// Header file for METHelpers functions
// Author: T.J.Khoo<khoo@cern.ch>
// Author: Bill Balunas <bill.balunas@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef METUTILITIES_MET_METHELPERS_H
#define METUTILITIES_MET_METHELPERS_H 1

// Framework includes
#include "AsgTools/MessageCheck.h"
#include "AsgTools/StatusCode.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"

namespace met {

  ANA_MSG_HEADER (msgMET)

  const static MissingETBase::Types::bitmask_t invisSource = 0x100000; // doesn't overlap with any other

  void addGhostMuonsToJets(const xAOD::MuonContainer& muons, xAOD::JetContainer& jets);

  StatusCode buildMETSum(const std::string& totalName,
                         xAOD::MissingETContainer* metCont);

  StatusCode buildMETSum(const std::string& totalName,
                         xAOD::MissingETContainer* metCont,
                         MissingETBase::Types::bitmask_t softTermsSource);

  StatusCode fillMET(xAOD::MissingET *& met,
                     xAOD::MissingETContainer * metCont,
                     const std::string& metKey,
                     const MissingETBase::Types::bitmask_t metSource);

}

#endif
