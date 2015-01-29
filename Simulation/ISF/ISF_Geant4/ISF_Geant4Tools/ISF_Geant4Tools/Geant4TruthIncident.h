/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Geant4TruthIncident.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_Geant4TruthIncident_H
#define ISF_GEANT4TOOLS_Geant4TruthIncident_H

// std
#include <vector>

// Atlas G4 Helpers
#include "SimHelpers/SecondaryTracksHelper.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"

// HepMC includes
#include "HepMC/SimpleVector.h"

class G4Step;
class G4Track;

namespace ISF {


  /** @class Geant4TruthIncident

      @TODO: class description

      @author Andreas.Schaelicke@cern.ch
   */

  /* comments:
     renamed to Geant4TruthIncident inorder to avoid confusion with G4 internal classes.

   */

  class Geant4TruthIncident : public ITruthIncident {
    public:
      Geant4TruthIncident(const G4Step*, AtlasDetDescr::AtlasRegion geoID, int numSecondaries, SecondaryTracksHelper &sHelper);
      virtual ~Geant4TruthIncident() {};

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
      /** Return the barcode of the primary particle */
      Barcode::ParticleBarcode  primaryBarcode() const;
      /** Return the primary particle after the TruthIncident vertex (and give
          it a new barcode) */
      HepMC::GenParticle*       primaryParticleAfterIncident(Barcode::ParticleBarcode newBC,
                                                             bool setPersistent);

      /** Return total number of secondary particles */
      unsigned short            numberOfSecondaries() const;
      /** Return p of the i-th secondary particle */
      const G4ThreeVector       secondaryP(unsigned short index) const;
      /** Return p^2 of the i-th secondary particle */
      double                    secondaryP2(unsigned short index) const;
      /** Return pT^2 of the i-th secondary particle */
      double                    secondaryPt2(unsigned short index) const;
      /** Return Ekin of the i-th secondary particle */
      double                    secondaryEkin(unsigned short index) const;
      /** Return the PDG Code of the i-th secondary particle */
      int                       secondaryPdgCode(unsigned short index) const;
      /** Set the the barcode of all secondary particles to the given bc */
      void                      setAllSecondaryBarcodes(Barcode::ParticleBarcode bc);

      // only called once accepted

      /** Return the primary particle as a HepMC particle type */
      HepMC::GenParticle*       primaryParticle(bool setPersistent) const;
      /** Return the i-th secondary as a HepMC particle type and assign the given
          Barcode to the simulator particle */
      HepMC::GenParticle*       secondaryParticle(unsigned short index,
                                                  Barcode::ParticleBarcode bc,
                                                  bool setPersistent) const;

      /** Return HepMC position of the truth vertex */
      const HepMC::FourVector&  position() const;

      /** Return true if at least one secondary particle passes the given pT^2 cut
          (= at least one secondary with pT^2 >= pt2cut) */
      bool secondaryPt2Pass(double pt2cut) const;

      /** Return true if at least one secondary particle passes the given Ekin cut
          (= at least one secondary with Ekin >= ekincut) */
      bool secondaryEkinPass(double ekincut) const;

      /** Record that a particular secondary passed a check */
      inline void                      setSecondaryPassed(unsigned short index) const;
      /** Should a particular secondary be written out to the GenEvent */
      inline bool                      writeOutSecondary(unsigned short index) const;

    private:
      Geant4TruthIncident();
      /** prepare the secondary particles, using the SecondaryTracksHelper */
      inline void prepareSecondaries() const;

      HepMC::GenParticle* convert(const G4Track *particle) const; //*AS* might be put static

      mutable bool                  m_positionSet;
      mutable HepMC::FourVector     m_position;
      const G4Step*                 m_step;

      SecondaryTracksHelper        &m_sHelper;
      mutable int                   m_secondariesNum;
      mutable bool                  m_secondariesPrepared;
      mutable std::vector<G4Track*> m_secondaries;

      bool                          m_checkLastSecondaryOnly;
      mutable std::vector<bool>     m_passedFilters;

      HepMC::GenParticle           *m_primaryParticleAfterIncident;
   };
  void ISF::Geant4TruthIncident::setSecondaryPassed(unsigned short index) const {
    m_passedFilters[index] = m_passedFilters[index] || true;
    return;
  }

  bool ISF::Geant4TruthIncident::writeOutSecondary(unsigned short index) const {
    return m_wholeVertexPassed || m_passedFilters[index];
  }

}

#endif // ISF_GEANT4TOOLS_Geant4TruthIncident_H
