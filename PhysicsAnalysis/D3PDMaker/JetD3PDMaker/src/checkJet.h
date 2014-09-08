// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/checkJet.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Check for pathological jets.
 */


#ifndef JETD3PDMAKER_CHECKJET_H
#define JETD3PDMAKER_CHECKJET_H


class Jet;


namespace D3PD {


/**
 * @brief Check for bad jets.
 * @param jet Jet to check.
 *
 * The reconstruction can make infinitely-recursive jets, than contain
 * themselves as constituents.  If we try to do anything on those jets
 * that accesses the consituents, we'll crash.  This function tests for
 * this pathology, allowing us to skip those jets.
 */
bool checkJet (const Jet& jet);
  

} // namespace


#endif // not JETD3PDMAKER_CHECKJET_H
