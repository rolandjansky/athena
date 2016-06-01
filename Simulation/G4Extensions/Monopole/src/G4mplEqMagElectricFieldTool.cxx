// class header
#include "G4mplEqMagElectricFieldTool.h"

// package headers
#include "G4mplEqMagElectricField.hh"

// Geant4 headers
#include "G4Version.hh"
#include "G4MagneticField.hh"
#include "G4Mag_EqRhs.hh"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4mplEqMagElectricFieldTool::G4mplEqMagElectricFieldTool( const std::string& type,
                                          const std::string& nam,const IInterface* parent )
  : AthAlgTool ( type, nam , parent )
{
  ATH_MSG_DEBUG("G4mplEqMagElectricFieldTool "<<type<<" "<<nam);
  declareInterface< IEquationOfMotionTool >( this ) ;
}

G4Mag_EqRhs* G4mplEqMagElectricFieldTool::makeEquationOfMotion(G4MagneticField *emField) const
{
  //FIXME thread-safety aspect here?
  return static_cast<G4Mag_EqRhs*>(new G4mplEqMagElectricField(emField));
}
