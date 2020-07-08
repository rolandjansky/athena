/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFTruthIncident.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_ISFEVENT_ISFTRUTHINCIDENT_H
#define ISF_ISFEVENT_ISFTRUTHINCIDENT_H

// stl includes
#include <vector>

// Barcode include
#include "BarcodeEvent/PhysicsProcessCode.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"
#include "ISF_Event/ITruthIncident.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

namespace ISF {
  class ISFParticle;

  /** @enum KillPrimary
      Basically only a boolean, which helps making the code more readable
  */
  enum KillPrimary {
    fPrimarySurvives = false,
    fKillsPrimary    = true
  };

  /** @class ISFTruthIncident

      Interface class for all truth incidents handled by the ISF.

      @author Elmar.Ritsch@cern.ch
  */

  class ISFTruthIncident : public ITruthIncident {
  public:
    ISFTruthIncident( ISF::ISFParticle &parent,
                      const ISFParticleVector &children,
                      Barcode::PhysicsProcessCode process,
                      AtlasDetDescr::AtlasRegion geoID,
                      ISF::KillPrimary killsPrimary = ISF::fPrimarySurvives,
                      const HepMC::FourVector *position = 0);
    ~ISFTruthIncident();

    /** Return HepMC position of the truth vertex */
    const HepMC::FourVector&  position() const override final;
    /** Return category of the physics process represented by the truth incident (eg hadronic, em, ..) */
    int                       physicsProcessCategory() const override final;
    /** Return specific physics process code of the truth incident (eg ionisation, bremsstrahlung, ..)*/
    Barcode::PhysicsProcessCode physicsProcessCode() const override final;

    /** Return p^2 of the parent particle */
    double                    parentP2() const override final;
    /** Return pT^2 of the parent particle */
    double                    parentPt2() const override final;
    /** Return Ekin of the parent particle */
    double                    parentEkin() const override final;
    /** Return the PDG Code of the parent particle */
    int                       parentPdgCode() const override final;
    /** Return the parent particle as a HepMC particle type
        (usually only called for particles that will enter the HepMC truth event) */
    HepMC::GenParticlePtr     parentParticle() const override final;
    /** Return the barcode of the parent particle */
    Barcode::ParticleBarcode  parentBarcode() const override final;
    /** Return the bunch-crossing identifier of the parent particle */
    int                       parentBCID() const override final;
    /** Return a boolean whether or not the parent particle survives the incident */
    bool                      parentSurvivesIncident() const override final;
    /** Return the parent particle after the TruthIncident vertex (and give
        it a new barcode) */
    HepMC::GenParticlePtr     parentParticleAfterIncident(Barcode::ParticleBarcode newBC) override final;

    /** Return p^2 of the i-th child particle */
    double                    childP2(unsigned short index) const override final;
    /** Return pT^2 of the i-th child particle */
    double                    childPt2(unsigned short index) const override final;
    /** Return Ekin of the i-th child particle */
    double                    childEkin(unsigned short index) const override final;
    /** Return the PDG Code of the i-th child particle */
    int                       childPdgCode(unsigned short index) const override final;
    /** Return the i-th child as a HepMC particle type and assign the given
        Barcode to the simulator particle (usually only called for particles that
        will enter the HepMC truth event) */
    HepMC::GenParticlePtr     childParticle(unsigned short index,
                                            Barcode::ParticleBarcode bc) const override final;
    /** Update the properties of a child particle from a pre-defined
        interaction based on the properties of the ith child of the
        current TruthIncident (only used in quasi-stable particle
        simulation) - TODO only a dummy implementation currently */
    virtual HepMC::GenParticlePtr     updateChildParticle(unsigned short index,
                                                          HepMC::GenParticlePtr existingChild) const override final;
    /** Set the the barcode of all child particles to the given bc */
    void                      setAllChildrenBarcodes(Barcode::ParticleBarcode bc) override final;
  private:
    ISFTruthIncident();

    /** return attached truth particle */
    inline HepMC::GenParticlePtr getHepMCTruthParticle( const ISF::ISFParticle& particle ) const;

    /** convert ISFParticle to GenParticle and attach to ISFParticle's TruthBinding */
    inline HepMC::GenParticlePtr updateHepMCTruthParticle( ISF::ISFParticle& particle, const ISF::ISFParticle* parent=nullptr ) const;

    ISF::ISFParticle&                  m_parent;
    const ISFParticleVector&           m_children;
    const Barcode::PhysicsProcessCode  m_process;
    const ISF::KillPrimary             m_killsPrimary;
    mutable const HepMC::FourVector   *m_position;
    mutable std::vector<bool>          m_passedFilters;
  };

} // end of namespace

#endif // ISF_ISFEVENT_ISFTRUTHINCIDENT_H
