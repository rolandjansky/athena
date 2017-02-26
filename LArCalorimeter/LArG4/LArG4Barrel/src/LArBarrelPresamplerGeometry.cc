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

#include "AthenaKernel/Units.h"
namespace Units = Athena::Units;

namespace LArG4 {

  namespace BarrelPresampler {

    // ==================================================================
    // initialize geometry parameters
    //  this should at some stage be taken from a database...
    Geometry::Geometry(const std::string& name, ISvcLocator * pSvcLocator)
      : AthService(name, pSvcLocator)
      , m_detectorName("LArMgr")
    {
      declareProperty("DetectorName",m_detectorName);
    }

    // ==========================================================================
    StatusCode Geometry::initialize()
    {
#include "PresParameterDef.icc"

      // position of mother volume inside nominal Atlas frame
      m_zpres=1549.*Units::mm;
      // compute positions of end of modules and of first cathode in a module in
      // nominal Atlas coordinates
      double eps=0.007*Units::mm;
      m_zminPS=3.00*Units::mm;   // FIXME this should come from database
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
      for (int i=0;i<8;i++) m_tilt[i]=m_mod[i][3]*Units::deg;

      // number of gaps per cell    module 7 is somewhat pathological (last cell is shorter)
      for (int i=0;i<7;i++) m_ngap_cell[i]=(int)((m_mod[i][1]+0.1)/m_ncell_module[i]);
      m_ngap_cell[7]=18;
#ifdef DEBUGHITS
      for (int i=0;i<8;i++) std::cout << "ngap per cell " << m_ngap_cell[i] << std::endl;
#endif

      // pitch in z of gaps
      for (int i=0;i<8;i++) m_pitch[i]=m_mod[i][4]*m_cmm;

      // LAr total gap
      m_halfThickLAr = 0.5*13.*Units::mm;

      return StatusCode::SUCCESS;
    }

    //=====================================================================================
    Geometry::~Geometry() {;}

    //=====================================================================================
    StatusCode Geometry::queryInterface( const InterfaceID & riid,  void** ppvInterface )
    {
      if ( IPresamplerGeometryCalculator::interfaceID().versionMatch(riid) ) {
        *ppvInterface = dynamic_cast<IPresamplerGeometryCalculator*>(this);
        addRef();
        return StatusCode::SUCCESS;
      }
      // Interface is not directly available : try out a base class
      return AthService::queryInterface(riid, ppvInterface);
    }

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

    LArG4Identifier Geometry::CalculateIdentifier(const G4Step* a_step ) const
    {
      //FIXME The string comparison in this method seems very
      //expensive investigate switching to pointer comparison.
      const static G4String fullPSName = "LAr::Barrel::Presampler";
      const static G4String fullCryoName = "LAr::TBBarrel::Cryostat::LAr";
      const static G4String fullModuleName = "LAr::Barrel::Presampler::Module";

      // Get all the required information from the current step
      const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
      G4int ndep = g4navigation->GetDepth();
      G4int iactive = -999;
      G4int itb = 0;
      G4int idep = -999;

      //Now navigate through the volumes hierarchy
      G4VPhysicalVolume *v1(nullptr);
      if(m_detectorName=="")
        for (G4int ii=0;ii<=ndep;ii++) {
          v1 = g4navigation->GetVolume(ii);
          if (v1->GetName()==fullPSName) idep=ii;    // half barrel
          else if (v1->GetName()==fullCryoName) itb=1;  // TB or not ?
          else if (v1->GetName()==fullModuleName) iactive=1;
        }
      else
        for (G4int ii=0;ii<=ndep;ii++) {
          v1 = g4navigation->GetVolume(ii);
          if (v1->GetName()==G4String(m_detectorName+"::LAr::Barrel::Presampler")) idep=ii;    // half barrel
          else if (v1->GetName()==G4String(m_detectorName+"::LAr::TBBarrel::Cryostat::LAr")) itb=1;  // TB or not ?
          else if (v1->GetName()==G4String(m_detectorName+"::LAr::Barrel::Presampler::Module")) iactive=1;
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

    LArG4Identifier Geometry::CalculatePSActiveIdentifier(const G4Step* a_step, const G4int ind, const G4int itb) const
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

      //G4int zSide;

      LArGeomData cellInfo;
      /*bool status=*/findCell(cellInfo, ploc.x(),ploc.y(),ploc.z());

      // in TB case, only 1 side,
      // in Atlas case, use overall z to decide side
      if (itb==0) {
        if (p.z() > 0.)
          {
            cellInfo.zSide = 1;
          }
        else
          {
            cellInfo.zSide = -1;
          }
      }
      else
        {
          cellInfo.zSide = 1;
        }

      if( cellInfo.zSide == -1 )
        //following code for an Y-axis rotation to define the side C half-barrel
        {
          cellInfo.phiBin = 31 - cellInfo.phiBin;
          if(cellInfo.phiBin < 0 ) cellInfo.phiBin += 64;
        }

      // Append the cell ID to the (empty) identifier.
      PSActiveID   << 4          // LArCalorimeter
                   << 1          // LArEM
                   << cellInfo.zSide
                   << cellInfo.sampling
                   << cellInfo.region
                   << cellInfo.etaBin
                   << cellInfo.phiBin;

#ifdef DEBUGHITS
      std::cout << "Here the identifier for the presampler ACTIVE ----> " << std::endl;
      std::cout << "zSide  ----> " << cellInfo.zSide << std::endl;
      std::cout << "sampling  ----> " << cellInfo.sampling << std::endl;
      std::cout << "region  ----> " <<  cellInfo.region << std::endl;
      std::cout << "etaBin  ----> " << cellInfo.etaBin << std::endl;
      std::cout << "phiBin  ----> " << cellInfo.phiBin << std::endl;
#endif

      return PSActiveID ;
    }

    //==========================================================================================

    LArG4Identifier Geometry::CalculatePS_DMIdentifier(const G4Step* a_step, const G4int ind, const G4int itb) const
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

      LArGeomData cellInfo;
      // 01-Feb-2001 WGS: Add zSide calculation.
      cellInfo.zSide = INT_MIN;  // Initialize to a default, incorrect value.

      // in TB case, only 1 side,
      // in Atlas case, use overall z to decide side
      if (itb==0) {
        if (p.z() > 0.)
          {
            cellInfo.zSide = 1;
          }
        else
          {
            cellInfo.zSide = -1;
          }
      }
      else
        {
          cellInfo.zSide = 1;
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
        cellInfo.region = 3;
      } else {
        cellInfo.region = 2;
      }

      const G4double detaDM = 0.1 ;
      const G4double dphiDM = ( 2 * M_PI ) / 64. ;

      cellInfo.phiBin = G4int( phi * (1. / dphiDM) );
      cellInfo.etaBin = G4int( eta * (1. / detaDM) );

      if( cellInfo.zSide == -1 )
        {
          cellInfo.phiBin = 31 - cellInfo.phiBin;
          if(cellInfo.phiBin < 0 ) cellInfo.phiBin += 64;
        }

      // 07-Jul-2005 WGS: Handle an extremely rare rounding problem.
      if ( cellInfo.phiBin == 64 ) cellInfo.phiBin = 0;

      // Fill identifier.
      PS_DM_ID     << 10               // ATLAS
                   << cellInfo.zSide*4 // LArEM
                   << 1
                   << 1
                   << cellInfo.region
                   << cellInfo.etaBin
                   << cellInfo.phiBin;

#ifdef  DEBUGHITS
      std::cout << "Here the identifier for the presampler DEAD materials ---->" << std::endl;
      std::cout << "zSide    ----> " << cellInfo.zSide*4 << std::endl;
      std::cout << "region   ----> " << cellInfo.region << std::endl;
      std::cout << "etaBin   ----> " << cellInfo.etaBin << std::endl;
      std::cout << "phiBin   ----> " << cellInfo.phiBin << std::endl;
#endif

      return PS_DM_ID ;
    }

    //===============================================================================
    // bool findCell(xloc,yloc,zloc) const
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

    bool  Geometry::findCell(LArG4::BarrelPresampler::LArGeomData& result, G4double xloc,G4double yloc,G4double zloc) const
    {

      result.sampling = 0;
      result.region   = 0;

      // eta,phi in "local" Atlas like half barrel coordinates
      G4double phi = atan2(yloc,xloc);
      if ( phi < 0. ) phi += 2.*M_PI;
      G4double z2=fabs(zloc+m_zpres+m_zminPS);

      // According to the memo, phi is divided into 64 regions [0..63].
      const G4int numberPhiBins = 64;
      const G4double dphi = ( 2.*M_PI ) / numberPhiBins;
      const G4double inv_dphi = 1. / dphi;
      // Convert  phi into integer bins.
      result.phiBin = G4int( phi * inv_dphi );
      if (result.phiBin >63) result.phiBin=63;
      if (result.phiBin <0)  result.phiBin=0;

      // if inside LAr but outside a module, returns some etaBin value for
      //  the DM identifier, but function return false to veto this step
      //  in the normal calculator
      if (z2 < m_zminPS ) {
        result.etaBin=0;
        return false;
      }
      if (z2 > m_end_module[7]) {
        result.etaBin=60;
        return false;
      }

      // find in which module in z the hit is
      result.module=0;
      for (int i=1;i<8;i++) {
        if (m_first_cathod[i]>=z2) break;
        result.module++;
      }
      if (result.module <0 || result.module > 7)
        {
          G4cerr << "LArBarrelPresampler: invalid module/hit " << result.module << " " << z2 << G4endl;
          if (result.module<0) result.etaBin=0;
          if (result.module>7) result.etaBin=60;
          return false;
        }

      // compute signed distance from middle of active layer along layer height axis
      G4int isect=G4int(phi*m_nsectors/(2.*M_PI));
      G4double phi0= ((double)isect+0.5)*2.*M_PI/((double) m_nsectors);
      static const G4double r0=1420.4*Units::mm;   // FIXME should be recomputed from database
      result.dist=(xloc*cos(phi0)+yloc*sin(phi0)-r0);

#ifdef DEBUGHITS
      std::cout << "sector number, dist along height " << isect << " " << result.dist << std::endl;
      std::cout << "z2,module number,m_first_cathod " << z2 << " " << result.module << " "
                << m_first_cathod[m_module] << std::endl;
#endif

      bool status=true;
      if (fabs(result.dist)>m_halfThickLAr) {
#ifdef DEBUGHITS
        std::cout << "Outside normal LAr 13mm gap " << std::endl,
#endif
          status=false;
      }

      // compute z distance from first cathode of module to step, taking into
      //   account the m_tilt angle of the cathode
      G4double deltaz=z2-(m_first_cathod[result.module]+result.dist*tan(m_tilt[result.module]));
      if (deltaz<0 ) {
        if (result.module>0) {
          result.module=result.module-1;
          deltaz=z2-(m_first_cathod[result.module]+result.dist*tan(m_tilt[result.module]));
        }
        else deltaz=0;
      }

      // compute gap number
      result.gap = ((int)(deltaz/m_pitch[result.module]));

#ifdef DEBUGHITS
      std::cout << "deltaz from first cathode,gap number " << deltaz << " " << result.gap << std::endl;
#endif

      // compute cell number in eta
      result.etaBin= result.gap/m_ngap_cell[result.module];
#ifdef DEBUGHITS
      std::cout << "etaBin inside module " << result.etaBin;
#endif
      if (result.etaBin >= m_ncell_module[result.module]) result.etaBin=m_ncell_module[result.module]-1;

      for (int i=0;i<result.module;i++) result.etaBin=result.etaBin+m_ncell_module[i];
#ifdef DEBUGHITS
      std::cout << " final etaBin " << result.etaBin << std::endl;
#endif

      if (result.etaBin < 0 || result.etaBin > 60) {
        std::cout << "LArBarrelPresamplerGeometry::findCell  etaBin outside range " << result.etaBin << std::endl;
      }

      // z of the centre of the anode of the gap
      G4double zmiddle=m_first_cathod[result.module]+((double)(result.gap+0.5))*m_pitch[result.module];

      // compute step position in electrode reference frame
      //   result.distElec => signed distance to electrode
      //   result.xElec => projection along electrode axis
      result.xElec=result.dist*cos(m_tilt[result.module])+(z2-zmiddle)*sin(m_tilt[result.module]);
      result.distElec=(z2-zmiddle)*cos(m_tilt[result.module]) - result.dist*sin(m_tilt[result.module]);
#ifdef DEBUGHITS
      std::cout << "zmiddle,xloc,yloc " << zmiddle << " " << result.distElec << " " << result.xElec << std::endl;
#endif

      return status;
    }

  } //end of BarrelPresampler namespace

} // end of LArG4 namespace
