/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBarcodeSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BARCODEINTERFACES_IBARCODESVC_H
#define BARCODEINTERFACES_IBARCODESVC_H 1

// Barcode includes
#include "BarcodeEvent/Barcode.h"
#include "BarcodeEvent/PhysicsProcessCode.h"

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

namespace Barcode {

  class BitCalculator;

  /** @class IBarcodeSvc

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */
  class IBarcodeSvc : virtual public IInterface {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    //TODO not fully implemented in Generic, Global, and Validation BarcodeSvcs
    //only fully implemented in LegacyBarcodeSvc (only barcode service used in production)
    virtual StatusCode initializeBarcodes() { return StatusCode::SUCCESS; };
    virtual StatusCode resetBarcodes() { return StatusCode::SUCCESS; };

    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(IBarcodeSvc, 1 , 0);

    /** Generate a new unique vertex barcode, based on the parent particle barcode and
        the physics process code causing the truth vertex*/
    virtual VertexBarcode newVertex( ParticleBarcode parentBC=Barcode::fUndefinedBarcode,
                                     PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) = 0;

    /** Generate a new unique barcode for a secondary particle, based on the parent
        particle barcode and the process code of the physics process that created
        the secondary  */
    virtual ParticleBarcode newSecondary( ParticleBarcode parentBC=Barcode::fUndefinedBarcode,
                                          PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) = 0;

    /** Generate a common barcode which will be shared by all children
        of the given parent barcode (used for child particles which are
        not stored in the mc truth event) */
    virtual ParticleBarcode sharedChildBarcode( ParticleBarcode parentBC,
                                                PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) = 0;

    /** Update the given barcode (e.g. after an interaction) */
    virtual ParticleBarcode incrementBarcode( ParticleBarcode oldBC,
                                              PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) = 0;

    /** Inform the BarcodeSvc about the largest particle and vertex Barcodes
        in the event input */
    virtual void registerLargestGenEvtParticleBC( ParticleBarcode bc) = 0;
    virtual void registerLargestGenEvtVtxBC( VertexBarcode bc) = 0;

    /** Return the secondary particle and vertex offsets */
    virtual Barcode::ParticleBarcode secondaryParticleBcOffset() const = 0;
    virtual Barcode::VertexBarcode   secondaryVertexBcOffset()  const = 0;

    /** Return the barcode increment for each generation of updated particles */
    virtual Barcode::ParticleBarcode particleGenerationIncrement() const = 0;

    /** return bit calculator for encoding extra barcode info */
    virtual const BitCalculator* getBitCalculator() const { return nullptr; }
    virtual bool hasBitCalculator() const { return false; }
  };

}

#endif //> !BARCODEINTERFACES_IBARCODESVC_H
