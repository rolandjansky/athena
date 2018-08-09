/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloG4_SimulationEnergies_H
#define CaloG4_SimulationEnergies_H

#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>
#include <memory>
#include <vector>

// Forward declarations.
class G4Step;
class G4ParticleDefinition;
class G4Track;

namespace CaloG4
{

  /// @class SimulationEnergies
  /// This class implements the calculations requires to categorize the
  /// energies deposited during the simulation. This "Monte Carlo truth"
  /// information is used in calibration studies and other applications.
  ///
  /// 12-Aug-2009 M. Leltchouk: this code update improves performance.
  /// Many thanks to Zachary Marshall for introducing and coding or
  /// suggesting most of these changes and to Gennady Pospelov for
  /// supporting this code (including careful testing) and the whole
  /// "machinary of calibration hits" which he greatly improved and extended.
  ///
  /// 07-Jan-2004 Mikhail Leltchouk with code additions and great support
  ///        from William Glenn Seligman (WGS).
  ///  Columbia University in the City of New York,
  ///  Nevis Labs, 136 South Broadway, Irvington, NY 10533, USA
  ///
  /// 29-Jun-2004 WGS: Take advantage of the new features of Geant 4.6.2:
  /// this class no longer has to be a singleton, and it need no longer
  /// inherit from G4VSteppingVerbose.
  ///
  class SimulationEnergies
  {
  public:

    SimulationEnergies();
    virtual ~SimulationEnergies();

    /// The simple method to call from a calibration calculator:
    /// Examine the G4Step and return the energies required for a
    /// calibration hit.
    void Energies( const G4Step* , std::vector<G4double> & ) const;

    /// Accessing detailed information:
    ///
    /// It's useful to define some types here.  The following enum uses
    /// a C++ trick: the first item in the list is set to zero.
    /// Therefore, "kNumberOfEnergyCategories" will be equal to the
    /// number of entries in the enum.
    ///
    enum eEnergyCategory
      { kEm = 0,
        kNonEm,
        kInvisible0,
        /* 12-Aug-2009 we compared three different techniques
        kInvisible1, //  to determine the energy lost
        kInvisible2, // due to "invisible" processes in the detector
             and chose one of them for production runs
        */
        kEscaped,
        kNumberOfEnergyCategories };

    /// This defines the results returned by the energy classification;
    /// these detailed results are mostly used for studies.  Time is in
    /// [ns], energies are in [MeV].
    ///
    typedef struct {
      std::map<eEnergyCategory, G4double> energy;
    } ClassifyResult_t;

    /* 12-Aug-2009 We keep only info about energy in ClassifyResult_t
                   for production runs (see above).
    typedef struct {
      G4double time;
      std::map<eEnergyCategory, G4double> energy;
      G4TrackStatus trackStatus;
      const G4ParticleDefinition* particle;
      const G4DynamicParticle* dynamicParticle;
      G4String processName;
    } ClassifyResult_t;
    */

    /// This method performs the actual function of this class: It
    /// classifies the components of the energy deposited by the
    /// current G4Step.  Allow a flag to control whether the escaped
    /// energy is routed to some other volume that the one in which the
    /// G4Step occurs.
    ClassifyResult_t Classify( const G4Step*, const G4bool processEscaped = true ) const;

  private:

    /// Some private methods for internal calculations:

    G4double measurableEnergy(const G4ParticleDefinition *particleDef,
                              G4int PDGEncoding,
                              G4double totalEnergy,
                              G4double kineticEnergy) const;

    G4double measurableEnergyV2(const G4ParticleDefinition *particleDef,
                                G4int PDGEncoding,
                                G4double totalEnergy,
                                G4double kineticEnergy) const;

    // Escaped energy requires special processing.
    G4bool ProcessEscapedEnergy( G4Step* fakeStep ) const;

    G4bool ParticleIsNeutrino( G4ParticleDefinition* particle ) const;

    std::unique_ptr<G4Step> CreateFakeStep(G4Track* a_track, G4double a_energy) const;

  }; // class SimulationEnergies

} // namespace CaloG4

#endif // CaloG4_SimulationEnergies_H
