/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Barcode.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BARCODEINTERFACES_BARCODE_H
#define BARCODEINTERFACES_BARCODE_H 1

#include <stdint.h>

namespace Barcode {

  /** Identifier type for particles */
  typedef int ParticleBarcode;

  // until HepMC::GenParticle::suggest_barcode(...) takes 'int' as argument,
  // better not use fancy types like 'uint32_t' (may cause overflows in
  // implicit type conversions)
  //typedef uint32_t ParticleBarcode;

  /** Identifier type for vertices (usually negative numbers) */
  // similar to what is said above: one could use 'uint32_t' instead of 'int' here
  typedef int VertexBarcode;


  /** enum to make the code more readable */
  enum BarcodeDefinitions {
    fUndefinedBarcode = 0
  };

}

#endif 
