/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_CONDITIONSSORTER_H
#define TRIGJETHYPO_CONDITIONSSORTER_H

/********************************************************************
 *
 * NAME:     ConditionsSorter.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

class ConditionBridge;

struct ConditionsSorter{
  bool operator()(const ConditionBridge& l, const ConditionBridge& r);
};

#endif
