/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_LINESPLITTER_H
#define TRIGHLTJETHYPO_LINESPLITTER_H
/********************************************************************
 *
 * NAME:     LineSplitter.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJetHypo
 *********************************************************************/

#include <string>
#include <vector>

std::vector<std::string> lineSplitter(const std::string&, char delimiter='\n');

#endif
