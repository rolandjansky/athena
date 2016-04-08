//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// class G4mplEqMagElectricField
//
// Class description:
//
// This is the right-hand side of equation of motion in a combined
// electric and magnetic field.

// History:
// - Created. V.Grichine, 10.11.98
// -------------------------------------------------------------------

#ifndef MONOPOLE_G4MPLEQMAGELECTRICFIELD_hh
#define MONOPOLE_G4MPLEQMAGELECTRICFIELD_hh

//#include "G4EquationOfMotion.hh"
#include "G4Mag_EqRhs.hh"
//#include "G4ElectroMagneticField.hh"
#include "G4MagneticField.hh"
#include "G4Version.hh"

class G4mplEqMagElectricField :
  //public G4EquationOfMotion
  public G4Mag_EqRhs
{
public:  // with description

  G4mplEqMagElectricField(G4MagneticField *emField );
  // : G4EquationOfMotion( emField ) {;}

  ~G4mplEqMagElectricField() {;}

#if G4VERSION_NUMBER > 1009
  void  SetChargeMomentumMass(G4ChargeState particleCharge,
                              G4double MomentumXc,
                              G4double mass);
#else
  void  SetChargeMomentumMass(G4double particleElCharge, // in e+ units
                              G4double particleMagCharge,
                              G4double mass);
#endif

  void EvaluateRhsGivenB(const G4double y[],
                         const G4double Field[],
                         G4double dydx[] ) const;
  // Given the value of the electromagnetic field, this function
  // calculates the value of the derivative dydx.

private:

  //  G4double        fElectroMagCof ;
  G4double        fMassCof;
  G4double  fElCharge;
  G4double fMagCharge;
  bool init;

};

#endif
