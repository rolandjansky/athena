/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EndcapCryostat::CalibrationMixedCalculator
// Prepared 24-Aug-2004 William Seligman
// from code prepared by Mikhail Leltchouk
// 28-May-2006 Hardcoded values changed to direct DB access. G. Pospelov.

// This class calculates the values needed for calibration hits in the
// simulation.

#undef DEBUG_HITS
#undef DEBUG_VOLUMES
#undef DEBUG_DMXYZ

#include "CryostatCalibrationMixedCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

// direct access to database
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "globals.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>
#include <cmath>
#include <climits>

namespace LArG4 {

  namespace EndcapCryostat {

    ///////////////////////////////////////////////////////////
    // Local class to store detector parameters for calculator
    ///////////////////////////////////////////////////////////
    class CalibrationMixedCalculator::Parameters {
    public:
      Parameters();
      ~Parameters();

      double m_zShift;                // 4 cm
      double m_zEMECRefPoint;         // 368.95 cm
      double m_LArEMECLArThickness;   // 53.6 cm
      double m_startZFCal1;           // 4668.5 mm
      double m_startZFCal2;           // 5128.3 mm
      double m_startZFCal3;           // 5602.8 mm
      double m_hdepthFCal1;           // 444.2 mm * 0.5
      double m_hdepthFCal2;           // 444.3 mm * 0.5
      double m_hdepthFCal3;           // 444.3 mm * 0.5
      double m_endZFCal3;             // m_startZFCal3 + 2*m_hdepthFCal3
      double m_endZHec0;              // 4557.5
      double m_z1BeforeFCal;          // m_zEMECRefPoint + m_LArEMECLArThickness
      double m_z2BeforeFCal;          // m_endZHec0
    };

    CalibrationMixedCalculator::Parameters::Parameters()
    {
      // Access to DB to get parameters
      StatusCode status;
      // Access the GeoModelSvc:
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IGeoModelSvc *geoModel;
      status = svcLocator->service ("GeoModelSvc",geoModel);
      if (status != StatusCode::SUCCESS) {
        throw std::runtime_error ("Cannot locate GeoModelSvc!!");
      }

      // Access the geometry database:
      IRDBAccessSvc *pAccessSvc;
      status=svcLocator->service("RDBAccessSvc",pAccessSvc);
      if (status != StatusCode::SUCCESS) {
        throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
      }

      // Obtain the geometry version information:
      std::string AtlasVersion = geoModel->atlasVersion();
      std::string LArVersion = geoModel->LAr_VersionOverride();
      std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

      // getting HEC table
      IRDBRecordset_ptr hecNominals = pAccessSvc->getRecordsetPtr("HecNominals",detectorKey,detectorNode);
      if (hecNominals->size()==0) {
        hecNominals = pAccessSvc->getRecordsetPtr("HecNominals","HecNominals-00");
        if (hecNominals->size()==0) {
          throw std::runtime_error("LArG4EC/CryostatCalibrationMixedCalculator -> Can't find the HecNominals table.");
        }
      }
      // getting emec table
      IRDBRecordset_ptr emecGeometry = pAccessSvc->getRecordsetPtr("EmecGeometry",detectorKey,detectorNode);
      if (emecGeometry->size()==0) {
        throw std::runtime_error("LArG4EC/CryostatCalibrationMixedCalculator -> Can't find the EmecGeometry table.");
      }
      // getting FCAL parameters
      IRDBRecordset_ptr fcalMod = pAccessSvc->getRecordsetPtr("FCalMod",detectorKey,detectorNode);
      if (fcalMod->size()==0) {
        throw std::runtime_error("LArG4EC/CryostatCalibrationMixedCalculator -> Can't find FCalMod table.");
      }

      // getting parameters
      m_zShift = (*emecGeometry)[0]->getDouble("ZSHIFT")*CLHEP::cm;
      m_zEMECRefPoint = (*emecGeometry)[0]->getDouble("Z0")*CLHEP::cm + m_zShift;
      m_LArEMECLArThickness = (*emecGeometry)[0]->getDouble("ETOT")*CLHEP::cm;
      m_startZFCal1 = (*fcalMod)[0]->getDouble("STARTPOSITION")*CLHEP::mm + m_zShift;
      m_startZFCal2 = (*fcalMod)[1]->getDouble("STARTPOSITION")*CLHEP::mm + m_zShift;
      m_startZFCal3 = (*fcalMod)[2]->getDouble("STARTPOSITION")*CLHEP::mm + m_zShift;
      m_hdepthFCal1 = (*fcalMod)[0]->getDouble("FULLMODULEDEPTH")*CLHEP::mm*0.5;
      m_hdepthFCal2 = (*fcalMod)[1]->getDouble("FULLMODULEDEPTH")*CLHEP::mm*0.5;
      m_hdepthFCal3 = (*fcalMod)[2]->getDouble("FULLMODULEDEPTH")*CLHEP::mm*0.5;
      m_endZHec0    = (*hecNominals)[0]->getDouble("OUTDEPTH")*CLHEP::mm + m_zShift;
      m_endZFCal3 = m_startZFCal3 + 2.0*m_hdepthFCal3;
      m_z1BeforeFCal = m_zEMECRefPoint + m_LArEMECLArThickness;
      m_z2BeforeFCal = m_endZHec0;
    }

    CalibrationMixedCalculator::Parameters::~Parameters()
    {
    }

    ///////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////

    CalibrationMixedCalculator::CalibrationMixedCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_par(nullptr)
      , m_backupCalculator("EndcapCryostatCalibrationLArCalculator",name)
    {
      declareProperty("BackupCalculator",m_backupCalculator);
    }

    StatusCode CalibrationMixedCalculator::initialize() {
      // Get a "backup" calculator.
      //if ( m_backupCalculator == 0)
      //m_backupCalculator = new CalibrationLArCalculator();
      ATH_CHECK(m_backupCalculator.retrieve());
      m_par = new Parameters();
      return StatusCode::SUCCESS;
    }


    CalibrationMixedCalculator::~CalibrationMixedCalculator()
    {
      // Cleanup pointers.
      //delete m_backupCalculator;
      //m_backupCalculator = 0;
      delete m_par;
    }

    // This calculator is intended to apply to the endcap volumes that
    // have "mixed" identifiers:

    G4bool CalibrationMixedCalculator::Process (const G4Step* a_step,
                                                LArG4Identifier & identifier,
                                                std::vector<G4double> & energies,
                                                const eCalculatorProcessing a_process) const
    {
      const static G4String volLArMgr = "LArMgr::";
      const static G4String volLArEndcapCryostat = "LAr::Endcap::Cryostat::";
      const static G4String volCylinderMixed = "Cylinder::Mixed";
      const static G4String volCylinderMixedPresampler = "Cylinder::Mixed::PresamplerMother";
      const static G4String volExtraBeforePS = "ExtraCyl_beforePS";
      const static G4String volSectorMixed = "Sector::Mixed";
      const static G4String volEmecHecLArSectorMixed = "EmecHecLAr::Sector::Mixed";
      const static G4String volConeMixed = "Cone::Mixed";
      const static G4String volFcalLiquidArgonC = "LAr::FCAL::LiquidArgonC";
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy ) {
        m_energyCalculator.Energies( a_step, energies );
      } else {
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );
      }


      identifier.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
        {
          // Calculate the identifier.

          // Note:
          // LArG4::EndcapCryostat::CryostatCalibrationCalculator uses
          // a table-based approach to determine the identifier.  The
          // following code uses an "if-statement" approach.

          // The fixed parameters (only a couple of which are readily
          // accessible from the database):

          static const double oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
          static const double oneOverDphi = 32./M_PI;  //   1/Dphi

          // Calculate the mid-point of the step, and the simple geometry variables.

          G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
          G4StepPoint* post_step_point = a_step->GetPostStepPoint();

          G4ThreeVector startPoint = pre_step_point->GetPosition();
          G4ThreeVector endPoint   = post_step_point->GetPosition();
          G4ThreeVector p = (startPoint + endPoint) * 0.5;

          //G4double rho = p.perp();
          G4double eta = fabs( p.pseudoRapidity() );
          G4double phi = p.phi();
          if ( phi < 0. ) phi += 2.*M_PI; // Normalize for phiBin calculation

          // Initialize identifier variables with (invalid) default
          // values (INT_MIN is defined in <climits>).
          G4int subdet = INT_MIN;
          G4int type =  INT_MIN;
          G4int sampling = INT_MIN;
          G4int region = INT_MIN;
          G4int etaBin = INT_MIN;
          G4int phiBin = (int) ( phi * oneOverDphi );
          // to handle rounding error problem
          if(phiBin > 63) phiBin = 0;

          G4String name = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
          G4int copy = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

          // we meet here 3 volume types:
          // LArMgr::LAr::Endcap::Cryostat::*
          // LAr::Endcap::Cryostat::Cylinder::Mixed::PresamplerMother
          // LArMgr::LAr::FCAL::LiquidArgonC
          if(name.compare(0, 8, volLArMgr) == 0) name.erase(0,8);

          if ( name.size() == 23 && name == volFcalLiquidArgonC ) {
            subdet = ( p.z() > 0.) ? 4 : -4;
            type = 1;
            if ( eta < 5.0) {
              type = 2;
              region = 5;
              etaBin = (int) ( (eta-3.)* oneOverDeta );
              sampling = 0;
              if ( fabs(p.z()) < (m_par->m_startZFCal1+m_par->m_hdepthFCal1) ) {
                sampling = 1;
              } else if( fabs(p.z()) < (m_par->m_startZFCal2+m_par->m_hdepthFCal2) ) {
                sampling = 2;
              } else if( fabs(p.z()) < (m_par->m_startZFCal3+m_par->m_hdepthFCal3) ) {
                sampling = 3;
              } else {
                type = 1;
                sampling = 3;
                region = 0;
                etaBin = (int) ( (eta-1.7)* oneOverDeta );
              }
            } else if (eta < 8.0) {
              sampling = 3;
              region = 1;
              etaBin = (int) ( (eta-5.)* 0.5 * oneOverDeta );
            } else {
              sampling = 3;
              region = 2;
              etaBin = 0;
              phiBin = 0;
            }
          }else if(name.size() > 23){
            name.erase(0,23);
            if ( ( ( copy == 3 || copy == 14 ) && name == volCylinderMixed ) || name == volCylinderMixedPresampler || name==volExtraBeforePS)
              {
                // case 1  // front walls
                // volumeName LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed, copy number   3, 14
                // volumeName LAr::Endcap::Cryostat::Cylinder::Mixed::PresamplerMother

                // subdet = +/-4    "+" or " -" according to sign of Z in World coorinate
                subdet = ( p.z() > 0.) ? 4 : -4;
                type = 1;
                if ( eta < 1.4 ) {
                  sampling = 2;
                  region = 5;
                  etaBin = (int) ( (eta-1.) * oneOverDeta );
                }else if ( eta < 1.5 ) {
                  sampling = 1;
                  region = 6;
                  etaBin = (int) ( (eta-1.3) * oneOverDeta );
                } else if ( eta < 1.8 ) {
                  sampling = 1;
                  region = 5;
                  // these walls are behind the scintillator - so it is
                  // region=5 and not region=4 even for 1.5<eta<1.6
                  etaBin = (int) ( (eta-1.5) * oneOverDeta );
                } else if ( eta < 3.2 ) {
                  sampling = 1;
                  region = 6;
                  etaBin = (int) ( (eta-1.3) * oneOverDeta );
                } else if ( eta < 5.0 ) {          // we expect that eta < 5.0
                  type = 1;
                  sampling = 1;
                  region = 7;
                  etaBin = (int) ( (eta-3.2) * oneOverDeta );
                } else if( eta < 8.0 ) {
                  sampling = 3;
                  region = 1;
                  etaBin = (int) ( (eta-5.0) * 0.5 * oneOverDeta );
                } else {
                  sampling = 3;
                  region = 2;
                  etaBin = 0;
                  phiBin = 0;
                }
              } else if ( ( copy == 9 || copy == 12 || copy == 13 ) && name == volCylinderMixed ) {
              // case 2  // back walls near Tile
              // volumeName   "LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed"
              // copy number   9, 12, 13

              type = 1;
              if ( eta < 1.7 ) {
                subdet = ( p.z() > 0.) ? 5 : -5;
                sampling = 3;
                region = 0;
                etaBin = (int) ( eta * oneOverDeta );
              } else if ( eta < 5.0 ) {
                subdet = ( p.z() > 0.) ? 4 : -4;
                sampling = 3;
                region = 0;
                etaBin = (int) ( (eta-1.7) * oneOverDeta );
              }
            } else if ( ( ( copy == 10 || copy == 19 ) && name == volCylinderMixed ) ||
                        ( ( copy == 3 || copy == 4 ) && name == volSectorMixed ) ||
                        ( ( copy ==  1 || copy ==  2 || copy ==  8 || copy ==  9 || copy == 10 || copy == 11 || copy == 12 ) && name == volEmecHecLArSectorMixed ) ) {
              // case 3  // walls and supports at large rho
              // volumeName   "LAr::Endcap::Cryostat::Cylinder"
              // copy number   10, 19

              // volumeName    "LAr::Endcap::Cryostat::Sector"
              // copy number   3, 4 SectorDBArray: daughters of cryoMotherLogical

              // volumeName   "LAr::Endcap::Cryostat::EmecHecLAr::Sector"
              // copy number   1-2,8,9-12

              subdet = ( p.z() > 0.) ? 4 : -4;
              type = 1;
              if ( eta < 1.7 ) {
                sampling = 2;
                region = 5;
                etaBin = (int) ( (eta-1.) * oneOverDeta );
              } else {
                sampling = 3;
                region = 0;
                etaBin = (int) ( (eta-1.7) * oneOverDeta );
              }
            }else if ( copy == 27 && name == volCylinderMixed ) {
              // case 4  //  wall between HEC and FCAL
              // volumeName   "LAr::Endcap::Cryostat::Cylinder::Mixed"
              // copy number   27
              subdet = ( p.z() > 0.) ? 4 : -4;
              if ( fabs(p.z()) < m_par->m_startZFCal1 ) {
                type = 2;
                sampling = 1;
                region = 5;
                etaBin = (int) ( (eta-3.0) * oneOverDeta );
              } else if ( fabs(p.z()) < m_par->m_startZFCal2 ) {
                type = 2;
                sampling = 1;
                region = 4;
                etaBin = 0;
              } else if ( fabs(p.z()) < m_par->m_startZFCal3 ) { //
                type = 2;
                sampling = 2;
                region = 4;
                etaBin = 0;
              } else if ( fabs(p.z()) < m_par->m_endZFCal3 ) { //
                type = 2;
                sampling = 3;
                region = 4;
                etaBin = 0;
              } else{ // leakage behind FCAL+HEC
                type = 1;
                sampling = 3;
                region = 0;
                etaBin = (int) ( (eta-1.7) * oneOverDeta );
              }
            } else if ( copy==1 && name == volConeMixed) {
              // case 6  //   walls close to beam-pipe
              // volumeName   "LAr::Endcap::Cryostat::Cone::Mixed"
              // copy number   6, 7, 16, 29, 33
              subdet = ( p.z() > 0.) ? 4 : -4;
              type = 1;
              if ( eta < 5.0 ) {
                if(fabs(p.z()) < m_par->m_z1BeforeFCal) {
                  type = 1;
                  sampling = 1;
                  region = 7;
                  etaBin = (int) ( (eta-3.2) * oneOverDeta );
                  if(etaBin < 0) etaBin = 0;
                } else if (fabs(p.z()) < m_par->m_z2BeforeFCal){
                  type = 2;
                  sampling = 0;
                  region = 5;
                  etaBin = (int) ( (eta-3.0) * oneOverDeta );
                } else {
                  type = 2;
                  sampling = 1;
                  region = 5;
                  etaBin = (int) ( (eta-3.0) * oneOverDeta );
                }
                if(etaBin < 0) etaBin = 0; // FcalNose can be a litle bit further than 3.2
              } else if ( eta < 8.0 ) {
                sampling = 3;
                region = 1;
                etaBin = (int) ( (eta-5.)* 0.5 * oneOverDeta );
              } else {
                sampling = 3;
                region = 2;
                etaBin = 0;
                phiBin = 0;
              }
            }else if ( ( copy == 6 || copy == 7 || copy == 16 || copy == 33 ) && name == volCylinderMixed ) {
              // case 6  //   walls close to beam-pipe
              // volumeName   "LAr::Endcap::Cryostat::Cylinder::Mixed"
              // copy number   6, 7, 16, 29, 33
              subdet = ( p.z() > 0.) ? 4 : -4;
              type = 1;
              if ( eta < 5.0 ) {
                if(fabs(p.z()) < m_par->m_z1BeforeFCal) {
                  type = 1;
                  sampling = 1;
                  region = 7;
                  etaBin = (int) ( (eta-3.2) * oneOverDeta );
                  if(etaBin < 0) etaBin = 0;
                } else if (fabs(p.z()) < m_par->m_z2BeforeFCal){
                  type = 2;
                  sampling = 0;
                  region = 5;
                  etaBin = (int) ( (eta-3.0) * oneOverDeta );
                } else {
                  type = 2;
                  sampling = 1;
                  region = 5;
                  etaBin = (int) ( (eta-3.0) * oneOverDeta );
                }
              } else if ( eta < 8.0 ) {
                sampling = 3;
                region = 1;
                etaBin = (int) ( (eta-5.)* 0.5 * oneOverDeta );
              } else {
                sampling = 3;
                region = 2;
                etaBin = 0;
                phiBin = 0;
              }
            }
          }

          // What if we have a G4Step that isn't handled by the above
          // code?  Answer: Use a "backup" calculator to try to
          // process the step.

          if ( type  == INT_MIN  ||
               region   == INT_MIN  ||
               sampling == INT_MIN  ||
               etaBin   == INT_MIN  ||
               phiBin   <  0 || etaBin < 0)
            {
              // g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
              LArG4::CalibrationDefaultCalculator::Print("UNEXP LArG4EC/CryostatCalibrationMixedCalculator",identifier,a_step);
#endif
#if defined (DEBUG_VOLUMES) || defined (DEBUG_HITS)
              static const G4int messageMax = 1000;
              static G4int messageCount = 0; // exists only if debug activated
              if ( messageCount++ < messageMax ) {
                std::cout << "LArG4::EndcapCryostat::CalibrationMixedCalculator::Process"
                          << " (error " << messageCount << " of " << messageMax << " max displayed)"
                          << std::endl
                          << "   G4Step in '"
                          << name
                          << "' copy="
                          << copy
                          << ", using backup calculator"
                          << std::endl;
                std::cout << " type:" << type
                          << " region:" << region
                          << " sampling:" << sampling
                          << " etaBin:" << etaBin
                          << " phiBin:" << phiBin
                          << " xyz:"<<p.x()<<", "<<p.y()<<", "<<p.z()
                          << " eta:"<< eta
                          <<std::endl;
              }
#endif
              //m_backupCalculator->Process(a_step, kOnlyID);
              //identifier = m_backupCalculator->identifier();
              std::vector<G4double> tmpv;
              m_backupCalculator->Process(a_step, identifier, tmpv, kOnlyID);
            } else {
            // Append the cell ID to the (empty) identifier.
            identifier << 10         // Calorimeter
                        << subdet     // LAr +/-4 where "+" or " -" according to the sign of Z in World coorinate
                        << type
                        << sampling
                        << region
                        << etaBin
                        << phiBin;
          }
        } // if a_process == kEnergyAndID  ||  a_process == kOnlyID

#ifdef DEBUG_HITS
      //      G4double energy = accumulate(energies.begin(),energies.end(),0.);
      std::cout << "LArG4::EndcapCryostat::CalibrationMixedCalculator::Process"
                << " ID=" << std::string(identifier)
        //        << " energy=" << energy
                << " energies=(" << energies[0]
                << "," << energies[1]
                << "," << energies[2]
                << "," << energies[3] << ")"
                << std::endl;
#endif

      // g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
      LArG4::CalibrationDefaultCalculator::Print("DMXYZ LArG4EC/CryostatCalibrationMixedCalculator",identifier,a_step);
#endif
      // Check for bad result.
      if ( identifier == LArG4Identifier() ) return false;

      return true;
    }

  } // namespace EndcapCryostat

} // namespace LArG4
