/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GlobalBarcodeSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BARCODESERVICES_GLOBALBARCODESVC_H
#define BARCODESERVICES_GLOBALBARCODESVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

//include
#include "BarcodeEvent/Barcode.h"
#include "BarcodeInterfaces/IBarcodeSvc.h"

#include "BarcodeServices/BitCalculator.h"

class IIncidentSvc;


namespace Barcode {

  /** @class GlobalBarcodeSvc

      This BarcodeService is intended to provide a variety of different particle
      and vertex barcode encoding options.

      The options currently available:
      *) encode the physics process code in the VertexBarcode

      @author Elmar.Ritsch -at- cern.ch
  */

  class GlobalBarcodeSvc : public extends<AthService, IBarcodeSvc, IIncidentListener> {
  public:

    /** Constructor with parameters */
    GlobalBarcodeSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~GlobalBarcodeSvc();

    /** Athena algorithm's interface methods */
    StatusCode  initialize();
    StatusCode  finalize();

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

    /** Inform the BarcodeSvc about the largest particle and vertex Barcodes
        in the event input */
    virtual void registerLargestGenEvtParticleBC( ParticleBarcode bc);
    virtual void registerLargestGenEvtVtxBC( VertexBarcode bc);

    /** Return the secondary particle and vertex offsets */
    virtual Barcode::ParticleBarcode secondaryParticleBcOffset() const;
    virtual Barcode::VertexBarcode   secondaryVertexBcOffset()  const;

    /** Return the barcode increment for each generation of updated particles */
    virtual Barcode::ParticleBarcode particleGenerationIncrement() const;

    virtual inline Barcode::BitCalculator* getBitCalculator() const { return m_bitcalculator; }
    virtual inline bool hasBitCalculator() const { return (m_bitcalculator!=0); }

  private:
    ServiceHandle<IIncidentSvc>                   m_incidentSvc;   //!< IncidentSvc to catch begin of event and end of envent

    /** bitwise utility calculator for barcodes */
    Barcode::BitCalculator*                       m_bitcalculator;

    /** barcode information used for GenVertices */
    VertexBarcode                                 m_firstVertex;
    VertexBarcode                                 m_vertexIncrement;
    VertexBarcode                                 m_curVertex;

    /** barcode information used for secondary GenParticles */
    ParticleBarcode                               m_firstSecondary;
    ParticleBarcode                               m_secondaryIncrement;
    ParticleBarcode                               m_curParticle;

    /** barcode offset for each generation of updated particles */
    ParticleBarcode                               m_particleGenerationIncrement;

    /** throw error messages if a possible overflow is detected */
    bool                                          m_doUnderOverflowChecks;

    /** switch whether to include the process code in the vertex barcode */
    bool                                          m_encodePhysicsProcess;
  };


} // end 'Barcode' namespace

#endif //> !BARCODESERVICES_GLOBALBARCODESVC_H
