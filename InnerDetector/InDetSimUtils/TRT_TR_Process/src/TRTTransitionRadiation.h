/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTTRANSITIONRADIATION_H
#define TRTTRANSITIONRADIATION_H

#include "TRTRadiatorParameters.h"

#include "G4VDiscreteProcess.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PhysicsLogVector.hh"
#include "G4Step.hh"
#include "G4Track.hh"

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

#include <vector>
#include <iostream>

//using namespace std;

class TRRegionXMLHandler;
//class TRTParameters;

class TRTTransitionRadiation : public G4VDiscreteProcess {

public:

  /////////////////////////////////////////////////////////////////////////////
  //                             Public Methods                              //
  /////////////////////////////////////////////////////////////////////////////

  TRTTransitionRadiation(const G4String& processName="TransitionRadiation", const std::string xmlfilename="TRgeomodelgeometry.xml");
  virtual ~TRTTransitionRadiation();

  void Initialize();
  G4bool IsApplicable(const G4ParticleDefinition&);
  G4double GetMeanFreePath(const G4Track& aTrack, G4double, G4ForceCondition *);
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack, const G4Step& aStep);

  void AddRadiatorParameters(TRTRadiatorParameters p);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLvl (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

private:

  TRTTransitionRadiation (const TRTTransitionRadiation&);
  TRTTransitionRadiation& operator= (const TRTTransitionRadiation&);
  /////////////////////////////////////////////////////////////////////////////
  //                        Private Helper Methods                           //
  /////////////////////////////////////////////////////////////////////////////

  G4double ComputePhotoAbsCof( const G4Material* Material, const G4double & GammaEnergy ) const;

  G4double XEmitanNevski( const G4double & PhotonEnergy,
                          const G4double & Gamma,
                          const G4double & GasThickness,
                          const G4double & FoilThickness) const;

  G4double XEmitanArtru( const G4double & PhotonEnergy,
                         const G4double & Gamma,
                         const G4double &GasThickness,
                         const G4double &FoilThickness) const;

  G4double NeffNevski( const G4double & sigGas,
                       const G4double & sigFoil,
                       const G4double & GasThickness,
                       const G4double & FoilThickness,
                       const G4int    & FoilsTraversed ) const;

  G4double NeffArtru( const G4double & sigGas,
                      const G4double & sigFoil,
                      const G4double & GasThickness,
                      const G4double & FoilThickness,
                      const G4int    & FoilsTraversed ) const;

  G4double XFinter( const G4double & X, const G4double* A, const G4double* F ) const;
  G4double XInteg( const G4double* yy, G4double* ss ) const;

  /////////////////////////////////////////////////////////////////////////////
  //                               Member Data                               //
  /////////////////////////////////////////////////////////////////////////////

  //protected:

  TRRegionXMLHandler * m_XMLhandler;
  const std::string m_xmlfilename;
  std::vector<TRTRadiatorParameters> m_radiators;

  G4double m_MinEnergyTR;   // min TR energy in material
  G4double m_MaxEnergyTR;   // max TR energy in material
  G4int    m_NumBins;       // number of bins in log scale

  G4double m_WplasmaGas;    // plasma energy of foil and air.
  G4double m_WplasmaFoil;

  G4double m_GammaMin;      // Minimum value of Lorentz gamma for TR radiation
  G4double m_EkinMin;       // Corresponding KineticEnergy for electrons

  G4double* m_Ey;
  G4double* m_Sr;
  G4double* m_om;
  G4double* m_Omg;
  G4double* m_sigmaGas;
  G4double* m_sigmaFoil;

  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;

};

#endif
