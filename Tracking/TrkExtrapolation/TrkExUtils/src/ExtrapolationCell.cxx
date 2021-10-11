/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationCell.cxx
///////////////////////////////////////////////////////////////////

#include "TrkExUtils/ExtrapolationCell.h"

const std::vector<std::string> Trk::ExtrapolationCode::s_ecodeNames = {
  "Unset",
  "InProgress",
  "SuccessDestination",
  "SuccessBoundaryReached",
  "SuccessPathLimit",
  "SuccessMaterialLimit",
  "Recovered",
  "FailureDestination",
  "FailureLoop",
  "FailureNavigation",
  "FailureUpdateKill",
  "FailureConfiguration",
  "LeftKnownWorld"
};
