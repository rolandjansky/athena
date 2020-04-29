/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_LARFASTSHOWER_H
#define LARG4FASTSIMULATION_LARFASTSHOWER_H

// STL includes
#include <string>
#include <map>

// Geant4 includes
#include "G4VFastSimulationModel.hh"

// Local includes
#include "FastShowerConfigStruct.h"

// Forward declarations
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/IO_GenEvent_fwd.h"

class IFastSimDedicatedSD;
class ILArG4ShowerLibSvc;


/// NEEDS DOCUMENTATION
class LArFastShower : public G4VFastSimulationModel
{
  public:

    /// Constructor
    LArFastShower(const std::string& name, const FastShowerConfigStruct& config,
                  IFastSimDedicatedSD* fastSimDedicatedSD);

    /// Empty destructor
    virtual ~LArFastShower() {}

    /** Determines the applicability of the fast sim model to this particle type
      Called once for each track picked up by Geant as it enters a region with the fast sim assigned to it.
      Currently returns false for all but photons and electrons.*/
    G4bool IsApplicable(const G4ParticleDefinition&) override;

    /** Determines the applicability of the fast sim model to this particular track.
      Checks that geometric location, energy, and particle type are within bounds.  Also checks for
      containment of the particle's shower within a specific detector region.*/
    virtual G4bool ModelTrigger(const G4FastTrack &) override;

    virtual G4bool ForcedAccept(const G4FastTrack &);//!< If it returns true, the particle will be parameterized without further checks
    virtual G4bool ForcedDeny  (const G4FastTrack &); //!< If it returns true, the particle will be returned to G4 without further checks

    /** Assigns the track to the appropriate method for application of the fast simulation.
      Only called if ModelTrigger returns true.*/
    void DoIt(const G4FastTrack&, G4FastStep&) override;

  protected:

    IFastSimDedicatedSD* fastShowerSD();
    ILArG4ShowerLibSvc* showerLibSvc();

    /// Method to kill a particle and deposit its energy using exponential decay function
    void KillParticle(const G4FastTrack&, G4FastStep&);
    /// Function for the application of shower library
    void UseShowerLib(const G4FastTrack&, G4FastStep&);

    /// Function to check the containment of a shower within a regular detector region
    virtual G4bool CheckContainment(const G4FastTrack &fastTrack);

    HepMC::GenEvent* GetGenEvent(const G4FastTrack &fastTrack);

    /// get switch for frozen showers
    bool   flagToShowerLib  ( const G4ParticleDefinition& particleType ) const;
    /// get lower energy limit for frozen showers
    double maxEneToShowerLib( const G4ParticleDefinition& particleType ) const;
    /// get upper energy limit for frozen showers
    double minEneToShowerLib( const G4ParticleDefinition& particleType ) const;

    bool generateFSStartingPoint( const HepMC::GenEvent * ge) const;

  private:
    const FastShowerConfigStruct m_configuration;

    /// Shower library sensitive detector for this shower
    IFastSimDedicatedSD *m_fastSimDedicatedSD;
    /// Pointer to the shower library service
    ILArG4ShowerLibSvc *m_showerLibSvc;

    // data members for configuration
    bool m_generate_starting_points;
    HepMC::IO_GenEvent* m_starting_points_file;
    std::string m_detector_tag_str;
    std::map<std::string,int> m_detmap;

    int m_eventNum;
    std::map<int,bool> m_applicableMap;

};

#endif //LARG4FASTSIMULATION_LARFASTSHOWER_H
