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
#include "BarcodeInterfaces/PhysicsProcessCode.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ITruthBinding.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

namespace ISF {
  class ISFParticle;

  typedef TruthBindingT<HepMC::GenParticle>   GenParticleTruth;

  /** @enum KillPrimary
      Basically only a boolean, which helps making the code more readable
  */
  enum KillPrimary {
    fPrimarySurvives = false,
    fKillsPrimary    = true
  };

  /** @class ISFTruthIncident

      @TODO: class description

      @author Elmar.Ritsch@cern.ch
  */

  class ISFTruthIncident : public ITruthIncident {
  public:
    ISFTruthIncident( ISF::ISFParticle &primary,
                      const ISFParticleVector &secondary,
                      Barcode::PhysicsProcessCode process,
                      AtlasDetDescr::AtlasRegion geoID,
                      ISF::KillPrimary killsPrimary = ISF::fPrimarySurvives,
                      const HepMC::FourVector *position = 0);
    ~ISFTruthIncident();

    /** Return HepMC position of the truth vertex */
    const HepMC::FourVector&  position() const;
    /** Return Physics process code of the truth incident */
    Barcode::PhysicsProcessCode physicsProcessCode() const;

    /** Return p^2 of the primary particle */
    double                    primaryP2() const;
    /** Return pT^2 of the primary particle */
    double                    primaryPt2() const;
    /** Return Ekin of the primary particle */
    double                    primaryEkin() const;
    /** Return the PDG Code of the primary particle */
    int                       primaryPdgCode() const;
    /** Return the primary particle as a HepMC particle type
        (usually only called for particles that will enter the HepMC truth event) */
    HepMC::GenParticle*       primaryParticle(bool setPersistent) const;
    /** Return the barcode of the primary particle */
    Barcode::ParticleBarcode  primaryBarcode() const;
    /** Return the extra barcode of the primary particle */
    Barcode::ParticleBarcode  primaryExtraBarcode() const;
    /** Return the primary particle after the TruthIncident vertex (and give
        it a new barcode) */
    HepMC::GenParticle*       primaryParticleAfterIncident(Barcode::ParticleBarcode newBC,
                                                           bool setPersistent);

    /** Return total number of secondary particles */
    unsigned short            numberOfSecondaries() const;
    /** Return p^2 of the i-th secondary particle */
    double                    secondaryP2(unsigned short index) const;
    /** Return pT^2 of the i-th secondary particle */
    double                    secondaryPt2(unsigned short index) const;
    /** Return Ekin of the i-th secondary particle */
    double                    secondaryEkin(unsigned short index) const;
    /** Return the PDG Code of the i-th secondary particle */
    int                       secondaryPdgCode(unsigned short index) const;
    /** Return the i-th secondary as a HepMC particle type and assign the given
        Barcode to the simulator particle (usually only called for particles that
        will enter the HepMC truth event) */
    HepMC::GenParticle*       secondaryParticle(unsigned short index,
                                                Barcode::ParticleBarcode bc,
                                                bool setPersistent) const;
    /** Set the the barcode of all secondary particles to the given bc */
    void                      setAllSecondaryBarcodes(Barcode::ParticleBarcode bc);
    /** Set the the extra barcode of all secondary particles to the given bc */
    void                      setAllSecondaryExtraBarcodes(Barcode::ParticleBarcode bc);
    /** Set the the extra barcode of a secondary particles to the given bc */
    void                      setSecondaryExtraBarcode(unsigned short index, Barcode::ParticleBarcode bc);
    /** Record that a particular secondary passed a check */
    inline void                      setSecondaryPassed(unsigned short index) const;
    /** Should a particular secondary be written out to the GenEvent */
    inline bool                      writeOutSecondary(unsigned short index) const;
  private:
    ISFTruthIncident();
    inline HepMC::GenParticle* convert( ISF::ISFParticle *particle,
                                        bool setPersistent) const;

    ISF::ISFParticle&                  m_primary;
    const ISFParticleVector&           m_secondary;
    const Barcode::PhysicsProcessCode  m_process;
    const ISF::KillPrimary             m_killsPrimary;
    mutable const HepMC::FourVector   *m_position;
    mutable std::vector<bool>          m_passedFilters;
  };

  void ISF::ISFTruthIncident::setSecondaryPassed(unsigned short index) const {
    m_passedFilters[index] = m_passedFilters[index] || true;
    return;
  }

  bool ISF::ISFTruthIncident::writeOutSecondary(unsigned short index) const {
    return m_wholeVertexPassed || m_passedFilters[index];
  }

} // end of namespace

#endif // ISF_ISFEVENT_ISFTRUTHINCIDENT_H
