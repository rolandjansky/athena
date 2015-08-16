/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_UNCLEANABLEJET_H
#define TRIGJETHYPO_UNCLEANABLEJET_H


// ********************************************************************
//
// NAME:     UncleanableJet.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <stdexcept>
#include <string>

struct UncleanableJet: public std::runtime_error{
  UncleanableJet(std::string s):runtime_error(s.c_str()){}    
};

#endif
