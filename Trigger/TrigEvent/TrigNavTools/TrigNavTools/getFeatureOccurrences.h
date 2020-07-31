// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrigNavTools/getFeatureOccurrences.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2020
 * @brief Return number of occurrences of each feature.
 *
 * Broken out from the old TrigNavigationSlimmingTool (B. Smith / T. Bold).
 */


#ifndef TRIGNAVTOOLS_GETFEATUREOCCURRENCES_H
#define TRIGNAVTOOLS_GETFEATUREOCCURRENCES_H


#include <map>
#include <string>


namespace HLT {


class NavigationCore;


namespace TrigNavTools {


/// Return number of occurrences of each feature.
std::map<std::string, int>
getFeatureOccurrences (const HLT::NavigationCore* navigation);



}} // namespace HLT::TrigNavTools


#endif // not TRIGNAVTOOLS_GETFEATUREOCCURRENCES_H
