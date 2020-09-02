/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_Geant4TruthIncident_H
#define ISF_GEANT4TOOLS_Geant4TruthIncident_H

// std
#include <vector>

// ISF includes
#include "ISF_Event/ITruthIncident.h"

// HepMC includes
#include "AtlasHepMC/SimpleVector.h"

//Geant4 includes
#include "G4ThreeVector.hh"

// forward declarations
class G4Step;
class G4Track;
class AtlasG4EventUserInfo;

namespace ISF {
  class ISFParticle;
}


namespace iGeant4 {

  /** @class Geant4TruthIncident

      ISF_Geant4 specific implementation of the ISF::ITruthIncident

      @author Andreas.Schaelicke@cern.ch
   */

  class Geant4TruthIncident : public ISF::ITruthIncident {
    public:
      Geant4TruthIncident( const G4Step*,
                           const ISF::ISFParticle& baseISP,
                           AtlasDetDescr::AtlasRegion geoID,
                           AtlasG4EventUserInfo* atlasG4EvtUserInfo);
      virtual ~Geant4TruthIncident() {};

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
      /** Return the barcode of the parent particle */
      Barcode::ParticleBarcode  parentBarcode() const override final;
      /** Return the bunch-crossing identifier of the parent particle */
      int                       parentBCID() const override final;
      /** Return a boolean whether or not the parent particle survives the incident */
      bool                      parentSurvivesIncident() const override final;
      /** Return the parent particle after the TruthIncident vertex (and give
          it a new barcode) */
      HepMC::GenParticlePtr     parentParticleAfterIncident(Barcode::ParticleBarcode newBC) override final;

      /** Return p of the i-th child particle */
      const G4ThreeVector       childP(unsigned short index) const;
      /** Return p^2 of the i-th child particle */
      double                    childP2(unsigned short index) const override final;
      /** Return pT^2 of the i-th child particle */
      double                    childPt2(unsigned short index) const override final;
      /** Return Ekin of the i-th child particle */
      double                    childEkin(unsigned short index) const override final;
      /** Return the PDG Code of the i-th child particle */
      int                       childPdgCode(unsigned short index) const override final;
      /** Set the the barcode of all child particles to the given bc */
      void                      setAllChildrenBarcodes(Barcode::ParticleBarcode bc) override final;

      /**  The interaction classifications are described as follows:
           STD_VTX: interaction of a particle without a pre-defined decay;
           QS_SURV_VTX: a particle with a pre-defined decay under-going a
           non-destructive interaction;
           QS_DEST_VTX: a particle with a pre-defined decay under-going a
           destructive interaction other than its pre-defined decay;
           QS_PREDEF_VTX: a particle under-going its pre-defined decay */
      ISF::InteractionClass_t interactionClassification() const override final;

      // only called once accepted

      /** Return the parent particle as a HepMC particle type */
      HepMC::GenParticlePtr   parentParticle() const override final;
      /** Return the i-th child as a HepMC particle type and assign the given
          Barcode to the simulator particle */
      HepMC::GenParticlePtr   childParticle(unsigned short index,
                                            Barcode::ParticleBarcode bc) const override final;
      /** Update the properties of a child particle from a pre-defined
          interaction based on the properties of the ith child of the
          current TruthIncident (only used in quasi-stable particle
          simulation). */
      HepMC::GenParticlePtr   updateChildParticle(unsigned short index,
                                                  HepMC::GenParticlePtr existingChild) const override final;
    private:
      Geant4TruthIncident();
      /** prepare the child particles */
      inline void prepareChildren() const;

      /** check if the given G4Track represents a particle that is alive in ISF or ISF-G4 */
      inline bool particleAlive(const G4Track *track) const;

      HepMC::GenParticlePtr convert(const G4Track *particle, const int barcode, const bool secondary) const; //*AS* might be put static

      mutable bool                  m_positionSet;
      mutable HepMC::FourVector     m_position;
      const G4Step*                 m_step{};
      const ISF::ISFParticle&       m_baseISP;

      AtlasG4EventUserInfo*             m_atlasG4EvtUserInfo{};
      mutable bool                  m_childrenPrepared;
      mutable std::vector<const G4Track*> m_children;

      HepMC::GenParticlePtr         m_parentParticleAfterIncident{};
   };

}

#endif // ISF_GEANT4TOOLS_Geant4TruthIncident_H
