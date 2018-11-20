/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_NONEXISTANTVARIABLE_H
#define TRIGHLTJETHYPO_NONEXISTANTVARIABLE_H


// ********************************************************************
//
// NAME:     NonexistantVariable.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  A. Steinhebel
//
// ********************************************************************
//

#include <stdexcept>
#include <string>

struct NonexistantVariable: public std::runtime_error{
  NonexistantVariable(std::string s):runtime_error(s.c_str()){}    
};

#endif

