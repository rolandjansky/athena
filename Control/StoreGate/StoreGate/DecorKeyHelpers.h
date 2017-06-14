// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file StoreGate/DecorKeyHelpers.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Some common helper functions used by decoration handles.
 */


#ifndef STOREGATE_DECORKEYHELPERS_H
#define STOREGATE_DECORKEYHELPERS_H


#include <string>


namespace SG {


/**
 * @brief Extract the container part of key.
 * @param key The decoration handle key.
 *
 * Given a key of the form CCC.DDD, returns the container part, CCC.
 */
std::string contKeyFromKey (const std::string& key);


/**
 * @brief Extract the decoration part of key.
 * @param key The decoration handle key.
 *
 * Given a key of the form CCC.DDD, returns the decoration part, DDD.
 */
std::string decorKeyFromKey (const std::string& key);


} // namespace SG


#endif // not STOREGATE_DECORKEYHELPERS_H
