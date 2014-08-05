// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/barcodeOrder.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Helper to sort TruthParticles by barcode.
 */


#ifndef TRUTHD3PDMAKER_BARCODEORDER_H
#define TRUTHD3PDMAKER_BARCODEORDER_H


#include "McParticleEvent/TruthParticle.h"
#include "xAODTruth/TruthParticle.h"


namespace D3PD {


/**
 * @brief Helper to sort TruthParticles by barcode.
 */
struct barcodeOrder
{
  bool operator() (const TruthParticle* p1, const TruthParticle* p2)
  { return p1->barcode() < p2->barcode(); }
  bool operator() (const xAOD::TruthParticle* p1, const xAOD::TruthParticle* p2)
  { return p1->barcode() < p2->barcode(); }
};


}


#endif // not TRUTHD3PDMAKER_BARCODEORDER_H
