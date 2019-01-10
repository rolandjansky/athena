/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LegacyBarcodeSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BARCODESERVICES_LEGACYBARCODESVC_H
#define BARCODESERVICES_LEGACYBARCODESVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

//include
#include "BarcodeEvent/Barcode.h"
#include "BarcodeInterfaces/IBarcodeSvc.h"

#include "BarcodeServices/BitCalculator.h"

#include "tbb/concurrent_unordered_map.h"



namespace Barcode {

  /** @class LegacyBarcodeSvc

      This BarcodeService reproduces the barcode treatmend for MC12:
      http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/G4Sim/MCTruth/src/TruthStrategyManager.cxx

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */

  class LegacyBarcodeSvc : public extends<AthService, IBarcodeSvc> {
  public:

    /** Constructor with parameters */
    LegacyBarcodeSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~LegacyBarcodeSvc();

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;

    /** Construct and insert a new set of barcode members. To be called for every new thread. */
    virtual StatusCode initializeBarcodes() override;

    /** Reset barcodes. To be called at the beginning of each event. */
    virtual StatusCode resetBarcodes() override;

    /** Generate a new unique vertex barcode, based on the parent particle barcode and
        the physics process code causing the truth vertex*/
    virtual VertexBarcode newVertex( ParticleBarcode parentBC=Barcode::fUndefinedBarcode,
                                     PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) override;

    /** Generate a new unique barcode for a secondary particle, based on the parent
        particle barcode and the process code of the physics process that created
        the secondary  */
    virtual ParticleBarcode newSecondary( ParticleBarcode parentBC=Barcode::fUndefinedBarcode,
                                          PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) override;

    /** Generate a common barcode which will be shared by all children
        of the given parent barcode (used for child particles which are
        not stored in the mc truth event) */
    virtual ParticleBarcode sharedChildBarcode( ParticleBarcode parentBC,
                                                PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) override;

    /** Update the given barcode (e.g. after an interaction) */
    virtual ParticleBarcode incrementBarcode( ParticleBarcode oldBC,
                                              PhysicsProcessCode process=Barcode::fUndefinedProcessCode ) override;

    /** Inform the BarcodeSvc about the largest particle and vertex Barcodes
        in the event input */
    virtual void registerLargestGenEvtParticleBC( ParticleBarcode bc) override;
    virtual void registerLargestGenEvtVtxBC( VertexBarcode bc) override;

    /** Return the secondary particle and vertex offsets */
    virtual Barcode::ParticleBarcode secondaryParticleBcOffset() const override;
    virtual Barcode::VertexBarcode   secondaryVertexBcOffset()  const override;

    /** Return the barcode increment for each generation of updated particles */
    virtual Barcode::ParticleBarcode particleGenerationIncrement() const override;

    /** handles to get barcode bitcalculator */
    virtual inline Barcode::BitCalculator* getBitCalculator() const override { return m_bitcalculator; }
    virtual inline bool hasBitCalculator() const override { return (m_bitcalculator!=0); }

  private:

    /** bitwise utility calculator for barcodes */
    Barcode::BitCalculator*                       m_bitcalculator;

    /** barcode information used for GenVertices */
    VertexBarcode                                 m_firstVertex;
    VertexBarcode                                 m_vertexIncrement;
    VertexBarcode                                 m_currentVertex;

    /** barcode information used for secondary GenParticles */
    ParticleBarcode                               m_firstSecondary;
    ParticleBarcode                               m_secondaryIncrement;
    ParticleBarcode                               m_currentSecondary;

    /** barcode offset for each generation of updated particles */
    ParticleBarcode                               m_particleGenerationIncrement;

    /** throw error messages if a possible overflow is detected */
    bool                                          m_doUnderOverflowChecks;

    struct BarcodeInfo {
        BarcodeInfo() = delete;
        BarcodeInfo(VertexBarcode cv, ParticleBarcode cs) : currentVertex(cv), currentSecondary(cs) {};
        VertexBarcode currentVertex;
        ParticleBarcode currentSecondary;
    };

    using LegacyBarcodeSvcThreadMap_t = tbb::concurrent_unordered_map
        < std::thread::id, BarcodeInfo, std::hash<std::thread::id> >;
    LegacyBarcodeSvcThreadMap_t m_bcThreadMap;

    BarcodeInfo& getBarcodeInfo() const;

  };


} // end 'Barcode' namespace

#endif //> !BARCODESERVICES_LEGACYBARCODESVC_H
