/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ValidationBarcodeSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BARCODESERVICES_VALIDATIONBARCODESVC_H
#define BARCODESERVICES_VALIDATIONBARCODESVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

//include
#include "BarcodeEvent/Barcode.h"
#include "BarcodeInterfaces/IBarcodeSvc.h"

class IIncidentSvc;


namespace Barcode {

  /** @class ValidationBarcodeSvc

      This BarcodeService reproduces the barcode treatmend for MC12:
      http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/G4Sim/MCTruth/src/TruthStrategyManager.cxx

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */

  class ValidationBarcodeSvc : public AthService,
                               public IBarcodeSvc,
                               virtual public IIncidentListener {
  public:

    /** Constructor with parameters */
    ValidationBarcodeSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~ValidationBarcodeSvc();

    /** Athena algorithm's interface methods */
    StatusCode  initialize();
    StatusCode  finalize();

    /** Query the interfaces. **/
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    /** Incident to reset the barcodes at the beginning of the event */
    void handle(const Incident& inc);

    /** Generate a new unique vertex barcode, based on the parent particle barcode and
        the physics process code causing the truth vertex*/
    virtual VertexBarcode newVertex( ParticleBarcode parentBC=Barcode::fUndefinedBarcode,
                                     PhysicsProcessCode process=Barcode::fUndefinedProcessCode );

    /** Generate a new unique barcode for a secondary particle, based on the parent
        particle barcode and the process code of the physics process that created
        the secondary  */
    virtual ParticleBarcode newSecondary( ParticleBarcode parentBC=Barcode::fUndefinedBarcode,
                                          PhysicsProcessCode process=Barcode::fUndefinedProcessCode );

    /** Generate a common barcode which will be shared by all children
        of the given parent barcode (used for child particles which are
        not stored in the mc truth event) */
    virtual ParticleBarcode sharedChildBarcode( ParticleBarcode parentBC,
                                                PhysicsProcessCode process=Barcode::fUndefinedProcessCode );

    /** Update the given barcode (e.g. after an interaction) */
    virtual ParticleBarcode incrementBarcode( ParticleBarcode oldBC,
                                              PhysicsProcessCode process=Barcode::fUndefinedProcessCode );

    /** Return the secondary particle and vertex offsets */
    virtual Barcode::ParticleBarcode secondaryParticleBcOffset() const;
    virtual Barcode::VertexBarcode   secondaryVertexBcOffset()  const;

    /** Return the barcode increment for each generation of updated particles */
    virtual Barcode::ParticleBarcode particleGenerationIncrement() const;

    /** Inform the BarcodeSvc about the largest particle and vertex Barcodes
        in the event input */
    virtual void registerLargestGenEvtParticleBC( ParticleBarcode bc);
    virtual void registerLargestGenEvtVtxBC( VertexBarcode bc);

  private:
    ServiceHandle<IIncidentSvc>                   m_incidentSvc;   //!< IncidentSvc to catch begin of event and end of envent

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
    ParticleBarcode                               m_barcodeGenerationOffset; //!< not sure why this is needed...

    /** throw error messages if a possible overflow is detected */
    bool                                          m_doUnderOverflowChecks;
  };


} // end 'Barcode' namespace

#endif //> !BARCODESERVICES_VALIDATIONBARCODESVC_H
