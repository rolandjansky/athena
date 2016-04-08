/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     LArBarrelPresamplerGeometry.cxx
PACKAGE:  offline/LArCalorimeter/LArG4/LArG4Barrel

AUTHORS:  G. Unal, L. Carminati
CREATED:  September, 2004

PURPOSE:  'geometrical' methods used by the LArBarrelPresamplerCalculator.
          These original methods (previously in LArBarrelPresampler Calculator) were 
          written by Bill Seligman

UPDATES:  - Calculate identifier method used by PresamplerCalibrationCalculator.
          (sept 2004)
          - Extended (added findCell method) to compute distance to electrode
          (GU oct 2005). Also improved computation of etaBin taking into account
          more correctly effect of tilted electrodes

********************************************************************/

//#undef DEBUGHITS

#include "LArG4Barrel/LArBarrelPresamplerGeometry.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Code/LArG4Identifier.h"

#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"

#include "G4ios.hh"
#include "globals.hh"

#include <cmath>
#include <limits.h>
#include <iostream>

namespace LArG4 {
  
namespace BarrelPresampler {

Geometry* Geometry::m_instance = 0;

Geometry* Geometry::GetInstance()
{
  if (m_instance == 0)
    {
      m_instance = new Geometry();
    }
  return m_instance;
}

// ==================================================================
// initialize geometry parameters
//  this should at some stage be taken from a database...
Geometry::Geometry()
{
#include "PresParameterDef.icc"

  // Access source of detector parameters.
  m_parameters = LArVG4DetectorParameters::GetInstance();
  
  // position of mother volume inside nominal Atlas frame
  m_zpres=1549.*CLHEP::mm;
  // compute positions of end of modules and of first cathode in a module in
  // nominal Atlas coordinates 
  double eps=0.007*CLHEP::mm;
  m_zminPS=3.00*CLHEP::mm;   // FIXME this should come from database
  m_end_module[0]=(m_mod[0][0]*m_cmm+2*eps)+m_zminPS+eps;
  for (int i=1;i<8;i++) m_end_module[i]=m_end_module[i-1]+(m_mod[i][0]*m_cmm+2*eps)+eps;
#ifdef DEBUGHITS
  for (int i=0;i<8;i++) {
   std::cout << "Module lenght " << m_mod[0][0]*m_cmm+2*eps << std::endl;
   std::cout << "End of Module " << m_end_module[i] << std::endl;
  }
#endif

  m_cat_th=m_cathode_th*m_cmm;
  m_first_cathod[0]=m_zminPS+m_mod[0][5]*m_cmm+m_cat_th/2.+2*eps; 
  for (int i=1;i<8;i++) m_first_cathod[i]=m_end_module[i-1]+m_mod[i][5]*m_cmm+m_cat_th/2.+2*eps;

#ifdef DEBUGHITS
  for (int i=0;i<8;i++) std::cout << "position of first cathode " << m_first_cathod[i] << std::endl;
#endif

  // number of cells in eta per module
  for (int i=0;i<7;i++) m_ncell_module[i]=8;
  m_ncell_module[7]=5;

  // electrode tild in rad
  for (int i=0;i<8;i++) m_tilt[i]=m_mod[i][3]*CLHEP::deg;

  // number of gaps per cell    module 7 is somewhat pathological (last cell is shorter)
  for (int i=0;i<7;i++) m_ngap_cell[i]=(int)((m_mod[i][1]+0.1)/m_ncell_module[i]);
  m_ngap_cell[7]=18;
#ifdef DEBUGHITS
  for (int i=0;i<8;i++) std::cout << "ngap per cell " << m_ngap_cell[i] << std::endl;
#endif

  // pitch in z of gaps
  for (int i=0;i<8;i++) m_pitch[i]=m_mod[i][4]*m_cmm;

  // LAr total gap
  m_halfThickLAr = 0.5*13.*CLHEP::mm;

  m_xElec=0;
  m_sampling=0;
  m_region=0;
  m_phiBin=0;
  m_module=0;
  m_gap=0;
  m_etaBin=0;
  m_distElec=0.;
  m_dist=0.;
  itb=0;
}

//=====================================================================================
Geometry::~Geometry() {;}

//======================================================================================
//
// The following method computes the identifiers in the Presampler volume:
// 
// 1) Navigate through the volumes hierarchy 
//
// 2) Calculate identifier using the CalculatePSActiveIdentifier method if the
//    hit is in the Module volume and CalculatePS_DMIdentifier if the hit is
//    in some dead region
//
//====================================================================================== 

LArG4Identifier Geometry::CalculateIdentifier(const G4Step* a_step, std::string strDetector )
{
  const static G4String fullPSName = "LAr::Barrel::Presampler";
  const static G4String fullCryoName = "LAr::TBBarrel::Cryostat::LAr";
  const static G4String fullModuleName = "LAr::Barrel::Presampler::Module";

  // Get all the required information from the current step
  const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory(); 
  G4int ndep = g4navigation->GetDepth();
  G4int iactive = -999;
  itb = 0 ;
  G4int idep = -999;

  //Now navigate through the volumes hierarchy 
  G4VPhysicalVolume* v1;
  if(strDetector=="")
    for (G4int ii=0;ii<=ndep;ii++) {
      v1 = g4navigation->GetVolume(ii);
      if (v1->GetName()==fullPSName) idep=ii;    // half barrel
      else if (v1->GetName()==fullCryoName) itb=1;  // TB or not ?
      else if (v1->GetName()==fullModuleName) iactive=1; 
    }
  else
    for (G4int ii=0;ii<=ndep;ii++) {
      v1 = g4navigation->GetVolume(ii);
      if (v1->GetName()==G4String(strDetector+"::LAr::Barrel::Presampler")) idep=ii;    // half barrel
      else if (v1->GetName()==G4String(strDetector+"::LAr::TBBarrel::Cryostat::LAr")) itb=1;  // TB or not ?
      else if (v1->GetName()==G4String(strDetector+"::LAr::Barrel::Presampler::Module")) iactive=1; 
    }

  if (idep < 0) std::abort();

  if ( iactive > 0 ) {    
    return CalculatePSActiveIdentifier( a_step , idep , itb );        
  }
  return CalculatePS_DMIdentifier( a_step , idep , itb);
}
 
// ==========================================================================================
// calculate identifier from a G4 step in the PS active region
// This function should always return a valid identifier which can be used for calibration hit
//  even if the hit is not really in the "fiducial" active part

LArG4Identifier Geometry::CalculatePSActiveIdentifier(const G4Step* a_step, const G4int ind, const G4int itb)
{ 
  LArG4Identifier PSActiveID = LArG4Identifier();

  G4ThreeVector p = (a_step->GetPostStepPoint()->GetPosition() + a_step->GetPreStepPoint()->GetPosition()) * 0.5;

#ifdef  DEBUGHITS
  std::cout << "Position of the step in the ATLAS frame (x,y,z) --> " << p.x() << " " << p.y() << " " << p.z() << std::endl;
  std::cout << "Eta and Phi in the atlas frame                  --> " << p.eta() << " " << p.phi() << std::endl;
#endif

// to get coordinates in the Presampler frame

  const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
  G4ThreeVector ploc = g4navigation->GetTransform(ind).TransformPoint(p);

  G4int zSide;

  /*bool status=*/findCell(ploc.x(),ploc.y(),ploc.z());

// in TB case, only 1 side,
// in Atlas case, use overall z to decide side
  if (itb==0) {
    if (p.z() > 0.)
    {
      zSide = 1;
    }
    else
    {
      zSide = -1;
    }
  } 
  else
  {
    zSide = 1;
  }

  if( zSide == -1 ) 
//following code for an Y-axis rotation to define the side C half-barrel
  {
    m_phiBin = 31 - m_phiBin; 
    if(m_phiBin < 0 ) m_phiBin += 64;
  }

  // Append the cell ID to the (empty) identifier.
  PSActiveID   << 4          // LArCalorimeter
	       << 1          // LArEM
	       << zSide
	       << m_sampling
	       << m_region
	       << m_etaBin
	       << m_phiBin;

#ifdef DEBUGHITS
  std::cout << "Here the identifier for the presampler ACTIVE ----> " << std::endl;
  std::cout << "m_zSide  ----> " << zSide << std::endl;
  std::cout << "m_sampling  ----> " << m_sampling << std::endl;
  std::cout << "m_region  ----> " <<  m_region << std::endl;
  std::cout << "m_etaBin  ----> " << m_etaBin << std::endl;
  std::cout << "m_phiBin  ----> " << m_phiBin << std::endl;
#endif

  return PSActiveID ;
}

//==========================================================================================

LArG4Identifier Geometry::CalculatePS_DMIdentifier(const G4Step* a_step, const G4int ind, const G4int itb)
{ 

  /******************************************************************************
  CaloDM_ID identifier:
      
  detector_system/subdet/type/sampling/region/eta/phi
  detector system = 10   -> Calorimeters
  subdet          = +/-4 -> LAr dead materials
  type            = 1    -> dead materials outside accordion and active presampler layers
  sampling        = 1    -> dead materials in front and in active LAr calorimeters 
                              (starting from front warm cryostat walls)
  regions:        = 0 barrel warm wall and solenoid in front of the barrel presampler, 0 < |eta| < 1.5
                  = 1 barrel cryostat cold wall in front of the barrel presampler, 0 < |eta| < 1.5
                  = 2 all materials in front of the barrel presampler at radius larger than cold wall 
                      outer radius, 0 < |eta| < 1.5
                  = 3 all materials from the active layer of the barrel presampler to the active layer 
                      of accordion, 0 < |eta| < 1.5 
  
   ---> Granularity : deta       0.1          granularity within region
                      dphi       pi/32 ~ 0.1  granularity within region 
  
  ***********************************************************************************/

  LArG4Identifier PS_DM_ID = LArG4Identifier();

  G4ThreeVector p = (a_step->GetPostStepPoint()->GetPosition() + a_step->GetPreStepPoint()->GetPosition()) * 0.5;

#ifdef  DEBUGHITS
  std::cout << "Position of the step in the ATLAS frame (x,y,z) --> " << p.x() << " " << p.y() << " " << p.z() << std::endl;
  std::cout << "Eta and Phi in the atlas frame                  --> " << p.eta() << " " << p.phi() << std::endl;
#endif

// to get coordinates in local half barrel frame, independently
// of overall presampler position/rotation

  const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
  G4ThreeVector ploc = g4navigation->GetTransform(ind).TransformPoint(p);
  G4double radius=sqrt(ploc.x()*ploc.x() + ploc.y()*ploc.y());
  
// shift z such that z=0 is eta=0 in Atlas standard frame
  G4ThreeVector ploc2(ploc.x(),ploc.y(),ploc.z()+m_zpres+m_zminPS);

#ifdef  DEBUGHITS						
  std::cout << "Position of the step after traslation (x,y,z) --> " << ploc2.x() << " " << ploc2.y() << " " << ploc2.z() << std::endl;
  std::cout << "Eta and Phi after translation                 --> " << ploc2.eta() << " " << ploc2.phi() << std::endl;
#endif  
						
  // 01-Feb-2001 WGS: Add zSide calculation.
  G4int zSide = INT_MIN;  // Initialize to a default, incorrect value.

// in TB case, only 1 side,
// in Atlas case, use overall z to decide side
  if (itb==0) {
    if (p.z() > 0.)
    {
      zSide = 1;
    }
    else
    {
      zSide = -1;
    }
  } 
  else
  {
    zSide = 1;
  }

  // eta,phi in "local" half barrel coordinates
  G4double phi = ploc2.phi();
  G4double eta = ploc2.eta();
 
  if ( phi < 0. ) phi += 2.*M_PI;
  //G4double z2=fabs(ploc2.z());  

  // chek if the hit is in front of the active layer of the presampler in order to distinguish
  // between regin 2 and 3: WARNING the method is temporary! 
  // PSModuleRmean = 1420 is the distance between the middle of the active layer (LAr) of the PS 
  // modules and the interaction point  

  const G4int numberPhiMod = 32;  
  const G4double dphi = ( 2.*M_PI ) / numberPhiMod;
  const G4double inv_dphi = 1. / dphi;
  const G4double PSModuleRmean = 1420 ;
  G4double phicheck = phi - int(phi * inv_dphi) * dphi  - (dphi /2.);
  G4double Rcheck =  PSModuleRmean  / cos(phicheck);
  if (radius > Rcheck) {
    m_region = 3;
  } else {
    m_region = 2;
  }
  
  const G4double detaDM = 0.1 ;
  const G4double dphiDM = ( 2 * M_PI ) / 64. ;

  m_phiBin = G4int( phi * (1. / dphiDM) );
  m_etaBin = G4int( eta * (1. / detaDM) );
  
  if( zSide == -1 )
    {
      m_phiBin = 31 - m_phiBin;
      if(m_phiBin < 0 ) m_phiBin += 64;
    }

  // 07-Jul-2005 WGS: Handle an extremely rare rounding problem.
  if ( m_phiBin == 64 ) m_phiBin = 0;

  // Fill identifier.
  PS_DM_ID     << 10               // ATLAS
	       << zSide*4          // LArEM
	       << 1
	       << 1
	       << m_region
	       << m_etaBin
	       << m_phiBin;

#ifdef  DEBUGHITS     
  std::cout << "Here the identifier for the presampler DEAD materials ---->" << std::endl;
  std::cout << "m_zSide  ----> " << zSide*4 << std::endl;
  std::cout << "region   ----> " << m_region << std::endl;
  std::cout << "etaBin   ----> " << m_etaBin << std::endl;
  std::cout << "phiBin   ----> " << m_phiBin << std::endl;
#endif
  
  return PS_DM_ID ;
}

//===============================================================================
// bool findCell(xloc,yloc,zloc)
//
// From local PS coordinates (half barrel tube mother volume)
//  compute etaBin,phiBin,sampling,region
//  as well as gap number, distance to closest electrode and projection
//  along electrode axis
//
// Takes into account for complexity of eta segmentation
//
// Return true if where are really within the 13mm LAr gap
// Return false for the few steps which are in the safety region at the
//   edges of module
// Assume that hit is in the "active" LAr

// note that here phiBin is computed for the + half barrel
// some care has to be taken to convert to the - half barrel, taking into
// account the rotation

// findCell always fill valids m_region, m_sampling , m_etaBin, m_phiBin
// it returns true if the hit is in the normal 13mm Ar gap, within a real module
// it returns false otherwise
//

bool  Geometry::findCell(G4double xloc,G4double yloc,G4double zloc)
{

  m_sampling = 0;
  m_region   = 0;

  // eta,phi in "local" Atlas like half barrel coordinates
  G4double phi = atan2(yloc,xloc);
  if ( phi < 0. ) phi += 2.*M_PI;
  G4double z2=fabs(zloc+m_zpres+m_zminPS);  

  // According to the memo, phi is divided into 64 regions [0..63].
  const G4int numberPhiBins = 64;
  const G4double dphi = ( 2.*M_PI ) / numberPhiBins;
  const G4double inv_dphi = 1. / dphi;
  // Convert  phi into integer bins.
  m_phiBin = G4int( phi * inv_dphi );
  if (m_phiBin >63) m_phiBin=63;
  if (m_phiBin <0)  m_phiBin=0;

// if inside LAr but outside a module, returns some etaBin value for
//  the DM identifier, but function return false to veto this step
//  in the normal calculator
  if (z2 < m_zminPS ) {
     m_etaBin=0;
     return false;
  }
  if (z2 > m_end_module[7]) {
     m_etaBin=60;
     return false;
  }
   
// find in which module in z the hit is
  m_module=0;
  for (int i=1;i<8;i++) {
    if (m_first_cathod[i]>=z2) break;
    m_module++;
  }
  if (m_module <0 || m_module > 7) 
  {
      G4cerr << "LArBarrelPresampler: invalid module/hit " << m_module << " " << z2 << G4endl;
      if (m_module<0) m_etaBin=0;
      if (m_module>7) m_etaBin=60;
      return false;
  }

// compute signed distance from middle of active layer along layer height axis
  G4int isect=G4int(phi*m_nsectors/(2.*M_PI));
  G4double phi0= ((double)isect+0.5)*2.*M_PI/((double) m_nsectors);
  static const G4double r0=1420.4*CLHEP::mm;   // FIXME should be recomputed from database
  m_dist=(xloc*cos(phi0)+yloc*sin(phi0)-r0);

#ifdef DEBUGHITS
     std::cout << "sector number, dist along height " << isect << " " << m_dist << std::endl;
     std::cout << "z2,module number,m_first_cathod " << z2 << " " << m_module << " "
                << m_first_cathod[m_module] << std::endl;
#endif

  bool status=true;
  if (fabs(m_dist)>m_halfThickLAr) {
#ifdef DEBUGHITS
     std::cout << "Outside normal LAr 13mm gap " << std::endl,
#endif
     status=false;
  }

// compute z distance from first cathode of module to step, taking into
//   account the m_tilt angle of the cathode
  G4double deltaz=z2-(m_first_cathod[m_module]+m_dist*tan(m_tilt[m_module]));
  if (deltaz<0 ) {
    if (m_module>0) {
      m_module=m_module-1;
      deltaz=z2-(m_first_cathod[m_module]+m_dist*tan(m_tilt[m_module]));
    }
    else deltaz=0;
  }

// compute gap number
  m_gap = ((int)(deltaz/m_pitch[m_module]));

#ifdef DEBUGHITS
   std::cout << "deltaz from first cathode,gap number " << deltaz << " " << m_gap << std::endl;
#endif

// compute cell number in eta 
  m_etaBin= m_gap/m_ngap_cell[m_module];
#ifdef DEBUGHITS
  std::cout << "etaBin inside module " << m_etaBin;
#endif
  if (m_etaBin >= m_ncell_module[m_module]) m_etaBin=m_ncell_module[m_module]-1;

  for (int i=0;i<m_module;i++) m_etaBin=m_etaBin+m_ncell_module[i];
#ifdef DEBUGHITS
  std::cout << " final etaBin " << m_etaBin << std::endl;
#endif

  if (m_etaBin < 0 || m_etaBin > 60) {
    std::cout << "LArBarrelPresamplerGeometry::findCell  etaBin outside range " << m_etaBin << std::endl;
  }

// z of the centre of the anode of the gap
  G4double zmiddle=m_first_cathod[m_module]+((double)(m_gap+0.5))*m_pitch[m_module];

// compute step position in electrode reference frame
//   m_distElec => signed distance to electrode
//   m_xElec => projection along electrode axis
  m_xElec=m_dist*cos(m_tilt[m_module])+(z2-zmiddle)*sin(m_tilt[m_module]);
  m_distElec=(z2-zmiddle)*cos(m_tilt[m_module]) - m_dist*sin(m_tilt[m_module]);
#ifdef DEBUGHITS
   std::cout << "zmiddle,xloc,yloc " << zmiddle << " " << m_distElec << " " << m_xElec << std::endl;
#endif

  return status;
}

} //end of BarrelPresampler namespace

} // end of LArG4 namespace
