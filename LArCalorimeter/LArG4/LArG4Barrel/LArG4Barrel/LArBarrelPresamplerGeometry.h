/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelPresamplerGeometry.hh

#ifndef LARBARRELPRESAMPLERGEOMETRY_H
#define LARBARRELPRESAMPLERGEOMETRY_H

//#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"

#include "globals.hh"


// Forward declarations.
class LArG4Identifier;
class G4Step;

namespace LArG4 {

namespace BarrelPresampler {

class Geometry {
  
 public: 

  // Accessor method for singleton pattern.
  static Geometry* GetInstance();
  virtual ~Geometry();

  G4int itb;

 private:
#include "LArG4Barrel/PresParameterDef.h"
  static Geometry* m_instance;

  // end z of the various modules
  G4double m_end_module[8];
  G4double m_zminPS;
  G4double m_zpres;
  G4double m_cat_th;
  // z of first cathode in each module
  G4double m_first_cathod[8];
  // tilt of electrodes
  G4double m_tilt[8];
  // number of gaps per cell
  G4int m_ngap_cell[8];
  // pitch in z of gaps
  G4double m_pitch[8];
  // number of cells per modules
  G4int    m_ncell_module[8];
  // total LAr thickness
  G4double m_halfThickLAr;

  // Pointer to detector parameters routine.
  LArVG4DetectorParameters* m_parameters;

  G4int m_sampling;
  G4int m_region;
  G4int m_etaBin; 
  G4int m_phiBin;
  G4int m_gap;
  G4int m_module;
  G4double m_distElec;
  G4double m_xElec;
  G4double m_dist;

 protected:
  
  Geometry();

 public:

  LArG4Identifier CalculateIdentifier( const G4Step*, std::string strDetector="" );
  LArG4Identifier CalculatePSActiveIdentifier( const G4Step* , const G4int indPS , const G4int itb );
  LArG4Identifier CalculatePS_DMIdentifier( const G4Step* , const G4int indPS , const G4int itb);
  bool findCell(G4double,G4double,G4double);
  G4int etaBin()    {return m_etaBin;};
  G4int phiBin()    {return m_phiBin;};
  G4int sampling()  {return m_sampling;};
  G4int region()    {return m_region;};
  G4int module()    {return m_module;};
  G4int gap()       {return m_gap;};
  G4double distElec() {return m_distElec;};
  G4double xElec()    {return m_xElec;};
  G4double dtrans()   {return m_dist;};
 
} ;

} //end of Barrel namespace

} // end of LArG4 namespace

#endif // LARBARRELGEOMETRY_H
