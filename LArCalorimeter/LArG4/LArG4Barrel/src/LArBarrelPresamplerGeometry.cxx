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

#include "LArBarrelPresamplerGeometry.h"
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

    Geometry::Geometry(const std::string& name, ISvcLocator *pSvcLocator)
      : AthService(name, pSvcLocator)
      , m_detectorName("LArMgr")
      , m_prep1_th(0)
      , m_prep2_th(0)
      , m_shell_th(0)
      , m_prot_th(0)
      , m_mech_clear(0)
      , m_rail_th(0)
      , m_rail_pos(0)
      , m_rail_width(0)
      , m_mb_th(0)
      , m_mb_width(0)
      , m_mb_length(0)
      , m_widthFront(0)
      , m_heightIn(0)
      , m_heightOut(0)
      , m_rMinPresamplerMother(0)
      , m_rMaxPresamplerMother(0)
      , m_PresamplerMother_length(0)
      , m_Phi_min(0)
      , m_Phi_span(0)
      , m_nsectors(0)
      , m_nbsectors(0)
      , m_zminPS(0)
      , m_zpres(0)
      , m_cat_th(0)
      , m_halfThickLAr(0)
      , m_parameters (nullptr)
    {
      declareProperty("DetectorName",m_detectorName);
    }

    //=====================================================================================
    Geometry::~Geometry() {;}

    // ==================================================================
    /** initialize geometry parameters
        this should at some stage be taken from a database... */
    StatusCode Geometry::initialize()
    {
#include "PresParameterDef.icc"

      /** Access source of detector parameters. */
      m_parameters = LArVG4DetectorParameters::GetInstance();

      /** position of mother volume inside nominal Atlas frame */
      m_zpres=1549.*CLHEP::mm;
      /** compute positions of end of modules and of first cathode in
       a module in nominal Atlas coordinates */
      const double eps=0.007*CLHEP::mm;
      m_zminPS=3.00*CLHEP::mm;   // FIXME this should come from database
      m_end_module[0]=(m_mod[0][0]*m_cmm+2*eps)+m_zminPS+eps;
      for (int i=1;i<8;i++) m_end_module[i]=m_end_module[i-1]+(m_mod[i][0]*m_cmm+2*eps)+eps;
#ifdef DEBUGHITS
      for (int i=0;i<8;i++) {
        ATH_MSG_VERBOSE("Module length " << m_mod[0][0]*m_cmm+2*eps);
        ATH_MSG_VERBOSE("End of Module " << m_end_module[i]);
      }
#endif

      m_cat_th=m_cathode_th*m_cmm;
      m_first_cathod[0]=m_zminPS+m_mod[0][5]*m_cmm+m_cat_th/2.+2*eps;
      for (int i=1;i<8;i++) m_first_cathod[i]=m_end_module[i-1]+m_mod[i][5]*m_cmm+m_cat_th/2.+2*eps;

#ifdef DEBUGHITS
      for (int i=0;i<8;i++) ATH_MSG_VERBOSE("position of first cathode " << m_first_cathod[i]);
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
      for (int i=0;i<8;i++) ATH_MSG_VERBOSE("ngap per cell " << m_ngap_cell[i]);
#endif

      // pitch in z of gaps
      for (int i=0;i<8;i++) m_pitch[i]=m_mod[i][4]*m_cmm;

      // LAr total gap
      m_halfThickLAr = 0.5*13.*CLHEP::mm;

      return StatusCode::SUCCESS;
    }

    // ====================================================================================

    StatusCode Geometry::queryInterface( const InterfaceID & riid,  void** ppvInterface )
    {
      if ( ILArBarrelPresamplerGeometry::interfaceID().versionMatch(riid) ) {
        *ppvInterface = dynamic_cast<ILArBarrelPresamplerGeometry*>(this);
        addRef();
        return StatusCode::SUCCESS;
      }
      /** Interface is not directly available : try out a base class */
      return AthService::queryInterface(riid, ppvInterface);
    }

    //======================================================================================
    /**
    The following method computes the identifiers in the Presampler volume:

    1) Navigate through the volumes hierarchy

    2) Calculate identifier using the CalculatePSActiveIdentifier method if the
       hit is in the Module volume and CalculatePS_DMIdentifier if the hit is
       in some dead region
    */
    //======================================================================================

    LArG4Identifier Geometry::CalculateIdentifier(const G4Step* a_step) const
    {
      const static G4String fullPSName = (m_detectorName.empty()) ? "LAr::Barrel::Presampler" : G4String(m_detectorName+"::LAr::Barrel::Presampler");
      const static G4String fullCryoName = (m_detectorName.empty()) ? "LAr::TBBarrel::Cryostat::LAr" : G4String(m_detectorName+"::LAr::TBBarrel::Cryostat::LAr");
      const static G4String fullModuleName = (m_detectorName.empty()) ? "LAr::Barrel::Presampler::Module" : G4String(m_detectorName+"::LAr::Barrel::Presampler::Module");

      /** Get all the required information from the current step */
      const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
      const G4int ndep = g4navigation->GetDepth();
      bool iactive(false);
      bool isTestBeam(false);
      G4int idep(-999);

      /** Now navigate through the volumes hierarchy */
      for (G4int ii=0;ii<=ndep;ii++) {
        // FIXME Need to find a way to avoid these string-comparisons
        const G4String& vname = g4navigation->GetVolume(ii)->GetName();
        if (idep<0 && vname==fullPSName) idep=ii;    // half barrel
        else if (!isTestBeam && vname==fullCryoName) isTestBeam=true;  // TB or not ?
        else if (!iactive && vname==fullModuleName) iactive=true;
      }

      if (idep < 0) std::abort();

      if ( iactive ) {
        return this->CalculatePSActiveIdentifier( a_step , idep , isTestBeam );
      }
      return this->CalculatePS_DMIdentifier( a_step , idep , isTestBeam);
    }

    // ==========================================================================================
    /** calculate identifier from a G4 step in the PS active region
    This function should always return a valid identifier which can be
    used for calibration hit even if the hit is not really in the
    "fiducial" active part */

    LArG4Identifier Geometry::CalculatePSActiveIdentifier(const G4Step* a_step, const G4int ind, const bool isTestBeam) const
    {
      LArG4Identifier PSActiveID = LArG4Identifier();

      const G4ThreeVector p = (a_step->GetPostStepPoint()->GetPosition() + a_step->GetPreStepPoint()->GetPosition()) * 0.5;

#ifdef  DEBUGHITS
      ATH_MSG_VERBOSE("Position of the step in the ATLAS frame (x,y,z) --> " << p.x() << " " << p.y() << " " << p.z());
      ATH_MSG_VERBOSE("Eta and Phi in the atlas frame                  --> " << p.eta() << " " << p.phi());
#endif

      /* to get coordinates in the Presampler frame */

      const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
      const G4ThreeVector ploc = g4navigation->GetTransform(ind).TransformPoint(p);

      const G4int zSide(this->determineZSide(isTestBeam, p.z()));

      CalcData currentCellData;
      (void)this->findCell(currentCellData,ploc.x(),ploc.y(),ploc.z());

      if( zSide == -1 )
        /** following code for an Y-axis rotation to define the side C half-barrel */
        {
          currentCellData.phiBin = 31 - currentCellData.phiBin;
          if(currentCellData.phiBin < 0 ) currentCellData.phiBin += 64;
        }

      /** Append the cell ID to the (empty) identifier. */
      PSActiveID   << 4          // LArCalorimeter
                   << 1          // LArEM
                   << zSide
                   << currentCellData.sampling
                   << currentCellData.region
                   << currentCellData.etaBin
                   << currentCellData.phiBin;

#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("Here the identifier for the presampler ACTIVE ----> ");
      ATH_MSG_VERBOSE("m_zSide  ----> " << zSide);
      ATH_MSG_VERBOSE("m_sampling  ----> " << currentCellData.sampling);
      ATH_MSG_VERBOSE("m_region  ----> " <<  currentCellData.region);
      ATH_MSG_VERBOSE("currentCellData.etaBin  ----> " << currentCellData.etaBin);
      ATH_MSG_VERBOSE("currentCellData.phiBin  ----> " << currentCellData.phiBin);
#endif

      return PSActiveID ;
    }

    //==========================================================================================

    LArG4Identifier Geometry::CalculatePS_DMIdentifier(const G4Step* a_step, const G4int ind, const bool isTestBeam) const
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

      const G4ThreeVector p = (a_step->GetPostStepPoint()->GetPosition() + a_step->GetPreStepPoint()->GetPosition()) * 0.5;

#ifdef  DEBUGHITS
      ATH_MSG_VERBOSE("Position of the step in the ATLAS frame (x,y,z) --> " << p.x() << " " << p.y() << " " << p.z());
      ATH_MSG_VERBOSE("Eta and Phi in the atlas frame                  --> " << p.eta() << " " << p.phi());
#endif

      /** to get coordinates in local half barrel frame, independently
          of overall presampler position/rotation */

      const G4NavigationHistory* g4navigation = a_step->GetPreStepPoint()->GetTouchable()->GetHistory();
      const G4ThreeVector ploc = g4navigation->GetTransform(ind).TransformPoint(p);
      const G4double radius=sqrt(ploc.x()*ploc.x() + ploc.y()*ploc.y());

      /** shift z such that z=0 is eta=0 in Atlas standard frame*/
      const G4ThreeVector ploc2(ploc.x(),ploc.y(),ploc.z()+m_zpres+m_zminPS);

#ifdef  DEBUGHITS
      ATH_MSG_VERBOSE("Position of the step after traslation (x,y,z) --> " << ploc2.x() << " " << ploc2.y() << " " << ploc2.z());
      ATH_MSG_VERBOSE("Eta and Phi after translation                 --> " << ploc2.eta() << " " << ploc2.phi());
#endif

      // 01-Feb-2001 WGS: Add zSide calculation.
      const G4int zSide(this->determineZSide(isTestBeam, p.z()));

      /** eta,phi in "local" half barrel coordinates */
      const G4double phi = (ploc2.phi() < 0.) ? ploc2.phi()+2.*M_PI : ploc2.phi();
      const G4double eta = ploc2.eta();
      //G4double z2=fabs(ploc2.z());

      /** check if the hit is in front of the active layer of the
      presampler in order to distinguish between regin 2 and 3:
      WARNING the method is temporary!

      PSModuleRmean = 1420 is the distance between the middle of the
      active layer (LAr) of the PS modules and the interaction
      point */

      const G4int numberPhiMod = 32;
      const G4double dphi = ( 2.*M_PI ) / numberPhiMod;
      const G4double inv_dphi = 1. / dphi;
      const G4double PSModuleRmean = 1420 ;
      const G4double phicheck = phi - int(phi * inv_dphi) * dphi  - (dphi /2.);
      const G4double Rcheck =  PSModuleRmean  / cos(phicheck);
      CalcData currentCellData;
      if (radius > Rcheck) {
        currentCellData.region = 3;
      } else {
        currentCellData.region = 2;
      }

      const G4double detaDM = 0.1 ;
      const G4double dphiDM = ( 2 * M_PI ) / 64. ;

      currentCellData.phiBin = G4int( phi * (1. / dphiDM) );
      currentCellData.etaBin = G4int( eta * (1. / detaDM) );

      if( zSide == -1 )
        {
          currentCellData.phiBin = 31 - currentCellData.phiBin;
          if(currentCellData.phiBin < 0 ) currentCellData.phiBin += 64;
        }

      // 07-Jul-2005 WGS: Handle an extremely rare rounding problem.
      if ( currentCellData.phiBin == 64 ) currentCellData.phiBin = 0;

      /** Fill identifier.*/
      LArG4Identifier PS_DM_ID = LArG4Identifier();
      PS_DM_ID     << 10               // ATLAS
                   << zSide*4          // LArEM
                   << 1
                   << 1
                   << currentCellData.region
                   << currentCellData.etaBin
                   << currentCellData.phiBin;

#ifdef  DEBUGHITS
      ATH_MSG_VERBOSE("Here the identifier for the presampler DEAD materials ---->");
      ATH_MSG_VERBOSE("m_zSide  ----> " << zSide*4);
      ATH_MSG_VERBOSE("region   ----> " << currentCellData.region);
      ATH_MSG_VERBOSE("etaBin   ----> " << currentCellData.etaBin);
      ATH_MSG_VERBOSE("phiBin   ----> " << currentCellData.phiBin);
#endif

      return PS_DM_ID ;
    }

    /**===============================================================================
     bool findCell(xloc,yloc,zloc) const

     From local PS coordinates (half barrel tube mother volume)
      compute etaBin,phiBin,sampling,region
      as well as gap number, distance to closest electrode and projection
      along electrode axis

     Takes into account for complexity of eta segmentation

     Return true if where are really within the 13mm LAr gap
     Return false for the few steps which are in the safety region at the
       edges of module
     Assume that hit is in the "active" LAr

     note that here phiBin is computed for the + half barrel
     some care has to be taken to convert to the - half barrel, taking into
     account the rotation

     findCell always fills valid currentCellData.region,
     currentCellData.sampling, currentCellData.etaBin,
     currentCellData.phiBin it returns true if the hit is in the
     normal 13mm Ar gap, within a real module it returns false
     otherwise
    */

    bool  Geometry::findCell(CalcData & currentCellData, G4double xloc,G4double yloc,G4double zloc) const
    {

      currentCellData.sampling = 0;
      currentCellData.region   = 0;

      /** eta,phi in "local" Atlas like half barrel coordinates */
      G4double phi = atan2(yloc,xloc);
      if ( phi < 0. ) phi += 2.*M_PI;
      const G4double z2=fabs(zloc+m_zpres+m_zminPS);

      /** According to the memo, phi is divided into 64 regions [0..63]. */
      const G4int numberPhiBins = 64;
      const G4double dphi = ( 2.*M_PI ) / numberPhiBins;
      const G4double inv_dphi = 1. / dphi;
      /** Convert phi into integer bins. */
      currentCellData.phiBin = G4int( phi * inv_dphi );
      if (currentCellData.phiBin >63) currentCellData.phiBin=63;
      if (currentCellData.phiBin <0)  currentCellData.phiBin=0;

      /** if inside LAr but outside a module, returns some etaBin
        value for the DM identifier, but function return false to veto
        this step in the normal calculator */
      if (z2 < m_zminPS ) {
        currentCellData.etaBin=0;
        return false;
      }
      if (z2 > m_end_module[7]) {
        currentCellData.etaBin=60;
        return false;
      }

      /** find in which module in z the hit is */
      currentCellData.module=0;
      for (int i=1;i<8;i++) {
        if (m_first_cathod[i]>=z2) break;
        currentCellData.module++;
      }
      if (currentCellData.module <0 || currentCellData.module > 7)
        {
          G4cerr << "LArBarrelPresampler: invalid module/hit " << currentCellData.module << " " << z2 << G4endl;
          if (currentCellData.module<0) currentCellData.etaBin=0;
          if (currentCellData.module>7) currentCellData.etaBin=60;
          return false;
        }

      /** compute signed distance from middle of active layer along layer height axis */
      const G4int isect=G4int(phi*m_nsectors/(2.*M_PI));
      const G4double phi0= ((double)isect+0.5)*2.*M_PI/((double) m_nsectors);
      static const G4double r0=1420.4*CLHEP::mm;   // FIXME should be recomputed from database
      currentCellData.dist=(xloc*cos(phi0)+yloc*sin(phi0)-r0);

#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("sector number, dist along height " << isect << " " << currentCellData.dist);
      ATH_MSG_VERBOSE("z2,module number,m_first_cathod " << z2 << " " << currentCellData.module << " "
                      << m_first_cathod[currentCellData.module]);
#endif

      /** compute z distance from first cathode of module to step,
         taking into account the m_tilt angle of the cathode */
      G4double deltaz=z2-(m_first_cathod[currentCellData.module]+currentCellData.dist*tan(m_tilt[currentCellData.module]));
      if (deltaz<0 ) {
        if (currentCellData.module>0) {
          currentCellData.module=currentCellData.module-1;
          deltaz=z2-(m_first_cathod[currentCellData.module]+currentCellData.dist*tan(m_tilt[currentCellData.module]));
        }
        else deltaz=0;
      }

      /** compute gap number */
      currentCellData.gap = ((int)(deltaz/m_pitch[currentCellData.module]));

#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("deltaz from first cathode,gap number " << deltaz << " " << currentCellData.gap);
#endif

      /** compute cell number in eta */
      currentCellData.etaBin= currentCellData.gap/m_ngap_cell[currentCellData.module];
#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("etaBin inside module " << currentCellData.etaBin);
#endif
      if (currentCellData.etaBin >= m_ncell_module[currentCellData.module]) currentCellData.etaBin=m_ncell_module[currentCellData.module]-1;

      for (int i=0;i<currentCellData.module;i++) currentCellData.etaBin=currentCellData.etaBin+m_ncell_module[i];
#ifdef DEBUGHITS
      ATH_MSG_VERBOSE(" final etaBin " << currentCellData.etaBin);
#endif

      if (currentCellData.etaBin < 0 || currentCellData.etaBin > 60) {
        ATH_MSG_WARNING("LArBarrelPresamplerGeometry::findCell  etaBin outside range " << currentCellData.etaBin);
      }

      /** z of the centre of the anode of the gap */
      const G4double zmiddle=m_first_cathod[currentCellData.module]+((double)(currentCellData.gap+0.5))*m_pitch[currentCellData.module];

      /** compute step position in electrode reference frame
         currentCellData.distElec => signed distance to electrode
         currentCellData.xElec => projection along electrode axis */
      currentCellData.xElec=currentCellData.dist*cos(m_tilt[currentCellData.module])+(z2-zmiddle)*sin(m_tilt[currentCellData.module]);
      currentCellData.distElec=(z2-zmiddle)*cos(m_tilt[currentCellData.module]) - currentCellData.dist*sin(m_tilt[currentCellData.module]);
#ifdef DEBUGHITS
      ATH_MSG_VERBOSE("zmiddle,xloc,yloc " << zmiddle << " " << currentCellData.distElec << " " << currentCellData.xElec);
#endif

      bool status=true;
      if (fabs(currentCellData.dist)>m_halfThickLAr) {
#ifdef DEBUGHITS
        ATH_MSG_VERBOSE("Outside normal LAr 13mm gap "),
#endif
          status=false;
      }

      return status;
    }

  } /** end of BarrelPresampler namespace */

} /** end of LArG4 namespace */
