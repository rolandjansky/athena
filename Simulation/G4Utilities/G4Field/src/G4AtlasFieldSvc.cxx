/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Field/G4AtlasFieldSvc.h"
#include "FadsField/MagneticFieldMapT.h"

// magnetic field service interface
#include "MagFieldInterfaces/IMagFieldSvc.h"

bool G4AtlasFieldSvc::s_fieldinitialized = false;

static FADS::MagneticFieldMapT<G4AtlasFieldSvc> idCool("G4AtlasFieldSvc");

G4AtlasFieldSvc::G4AtlasFieldSvc()
  : m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc", "G4AtlasFieldSvc"),
    m_magFieldSvcQuick(0)
{
}


G4AtlasFieldSvc::G4AtlasFieldSvc(const std::string& n)
  : FADS::MagneticFieldMap(n),
    m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc", n),
    m_magFieldSvcQuick(0)
{
}


void G4AtlasFieldSvc::Initialize() {
  if (!s_fieldinitialized) {
    if ( m_magFieldSvc.retrieve().isFailure()) {
      // mlog << MSG::FATAL << "Could not find MagFieldAthenaSvc" << endmsg ;
      std::cout << "Could not find MagFieldAthenaSvc" << std::endl ;
      return;
    }

    // store a quick access pointer to remove gaudi overhead
    m_magFieldSvcQuick = &(*m_magFieldSvc);

    s_fieldinitialized = true;
  }
}

void G4AtlasFieldSvc::FieldValue(const double *pos, double *bfield) const {
  //  static AbstractMagneticField* pMagField = p_MagFieldAthenaSvc->GetUpdatedMagFieldAthena();
  //
  //  // mm to cm unit conversion + double->float type conversion
  //  float f_pos_in_cm[3] = { (float)pos[0]*.1, (float)pos[1]*.1, (float)pos[2]*.1 };
  //  // will store the BField as floats
  //  float f_bfield[3];
  //
  //  // compute BField
  //  pMagField->field_tesla_cm( f_pos_in_cm, f_bfield);
  //
  //  // BField unit conversion + float->double type conversion
  //  for (int i=0; i<3; i++)
  //    bfield[i] = (double)(f_bfield[i]*10.*CLHEP::kilogauss);
 
  m_magFieldSvcQuick->getField( pos, bfield);
  return;
}
