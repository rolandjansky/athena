/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EMECSupportCalibrationCalculator
// Prepared 09-Sep-2004 Mikhail Leltchouk
// Modified 29-Sep-2004 Mikhail Leltchouk
// Modified 2-Oct-2007 Mikhail Leltchouk

// This class calculates the values needed for calibration hits in the
// simulation.

#undef DEBUG_HITS
#undef DEBUG_VOLUMES
#undef DEBUG_DMXYZ

#include "EMECSupportCalibrationCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

// direct access to database
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/Units.h"
#include "globals.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>
#include <cmath>
#include <climits>


namespace Units = Athena::Units;


namespace LArG4 {

  ///////////////////////////////////////////////////////////
    // Local class to store detector parameters for calculator
  ///////////////////////////////////////////////////////////
  class EMECSupportCalibrationCalculator::Parameters {
    public:
      Parameters();
      ~Parameters();

      // Access to parameters.
      LArVG4DetectorParameters* m_parameters;

      double m_zShift;                 // 40.0
      double m_zEMECRefPoint;          // 3689.5000
      double m_HECzStart;              // 4277.0000
      double m_zAlignmentSafety;       // 50.0000
      double m_zInFrontOfSpanishFan;   // 3739.5000
      double m_zBehindOfSpanishFan;    // 4304.5000
      double m_zInFrontOfHEC;          // 4327.0000
      double m_zzInFrontOfPresampler;  // 3662.0000
      double m_zzInFrontOfStrips;      // 3729.5000
      double m_zzInFrontOfMiddle;      // 3797.8575
      double m_zzInFrontOfBack;        // 4051.5312
      double m_z1BeforeFCal;           // 4265.5000
      double m_z2BeforeFCal;           // 4597.5000
      double m_HECzEnd;                // 6095.0000 + 40
  };

  EMECSupportCalibrationCalculator::Parameters::Parameters()
  {

    // Access source of detector parameters.
    m_parameters = LArVG4DetectorParameters::GetInstance();

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
    const std::string AtlasVersion = geoModel->atlasVersion();
    const std::string LArVersion = geoModel->LAr_VersionOverride();
    const std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    const std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

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
    m_zEMECRefPoint = (*emecGeometry)[0]->getDouble("Z0")*CLHEP::cm;
    m_HECzStart = (*hecNominals)[0]->getDouble("INDEPTH")*CLHEP::mm;
    m_zAlignmentSafety = m_zShift + 10.*CLHEP::mm;
    m_zInFrontOfSpanishFan =  m_zEMECRefPoint + m_zAlignmentSafety;
    m_zBehindOfSpanishFan = m_zEMECRefPoint + m_zShift + 630.*CLHEP::mm - 55.*CLHEP::mm;
    // 630mm - LENGTH OF THE CALORIMETER in cold (632mm - length in warm), 55mm - length of BackSupportMother
    m_zInFrontOfHEC = m_HECzStart + m_zAlignmentSafety;
    m_zzInFrontOfPresampler = m_parameters->GetValue("LArEMECPreNomPos") - m_parameters->GetValue("LArEMECPreThickness")/2. + m_zShift;
    m_zzInFrontOfStrips = m_zEMECRefPoint + m_zShift;
    m_zzInFrontOfMiddle = 3757.8575*CLHEP::mm + m_zShift;
    m_zzInFrontOfBack = 4011.5312*CLHEP::mm + m_zShift; // for eta~2.5
    m_z1BeforeFCal = m_zEMECRefPoint + (*emecGeometry)[0]->getDouble("ETOT")*CLHEP::cm + m_zShift;
    m_z2BeforeFCal = (*hecNominals)[0]->getDouble("OUTDEPTH")*CLHEP::mm + m_zShift;
    m_HECzEnd = (*hecNominals)[7]->getDouble("OUTDEPTH")*CLHEP::mm + m_zShift;

  }

  EMECSupportCalibrationCalculator::Parameters::~Parameters()
  {
  }

  ///////////////////////////////////////////////////////////
  // Methods
  ///////////////////////////////////////////////////////////

  EMECSupportCalibrationCalculator::EMECSupportCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
    : LArCalibCalculatorSvcImp(name, pSvcLocator)
    , m_par(nullptr)
    , m_backupCalculator("EndcapCryostatCalibrationLArCalculator",name)
  {
   declareProperty("BackupCalculator",m_backupCalculator);
  }

  StatusCode EMECSupportCalibrationCalculator::initialize() {
    ATH_CHECK(m_backupCalculator.retrieve());
    m_par = new Parameters();
    return StatusCode::SUCCESS;
  }


  EMECSupportCalibrationCalculator::~EMECSupportCalibrationCalculator()
  {
    // Cleanup pointers.
    //delete m_backupCalculator;
    //m_backupCalculator = 0;
    delete m_par;
  }

  G4bool EMECSupportCalibrationCalculator::Process (const G4Step* a_step,
                  LArG4Identifier & identifier,
                  std::vector<G4double> & energies,
                  const eCalculatorProcessing a_process) const
  {
    // Use the calculators to determine the energies and the
    // identifier associated with this G4Step.  Note that the
    // default is to process both the energy and the ID.

    energies.reserve(4);
    energies.clear();
    if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyEnergy ) {
      m_energyCalculator.Energies( a_step, energies );
    } else {
      for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );
    }

    identifier.clear();
    if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyID )
    {
      static const double oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
      static const double oneOverDphi = 32./M_PI;  //   1/Dphi

      // Calculate the mid-point of the step, and the simple geometry variables.
      const G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      const G4StepPoint* post_step_point = a_step->GetPostStepPoint();

      const G4ThreeVector startPoint = pre_step_point->GetPosition();
      const G4ThreeVector endPoint   = post_step_point->GetPosition();
      const G4ThreeVector p = (startPoint + endPoint) * 0.5;

      //G4double rho = p.perp();
      G4double eta = fabs( p.pseudoRapidity() );
      G4double phi = p.phi(); 
      if ( phi < 0. ) phi += 2.*M_PI; // Normalize for phiBin calculation

      // Initialize identifier variables with (invalid) default
      // values (INT_MIN is defined in <climits>).
      const G4int subdet = ( p.z() > 0.) ? 4 : -4;
      G4int type =  INT_MIN;
      G4int sampling = INT_MIN;
      G4int region = INT_MIN;
      G4int etaBin = INT_MIN;
      G4int phiBin = (int) ( phi * oneOverDphi );
      // to handle rounding error problem
      if(phiBin > 63) phiBin = 0;

      G4String name = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

      if(name.index("LArMgr::") == 0) name.erase(0,8);

#if defined (DEBUG_VOLUMES) || defined (DEBUG_HITS)
      // The 'copy' variable is used only inside defines
      G4int copy = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
#endif 
#ifdef DEBUG_HITS
      std::cout << "LArG4::EMECSupportCalibrationCalculator::Process"
        << "   G4Step in '" << name
        << "' copy=" << copy
        << " or  G4Step in LAr at : (x,y,z) [mm] = ("
        << p.x()/Units::mm << ","
        << p.y()/Units::mm << "," 
        << p.z()/Units::mm
        << "); eta=" << eta
        << ", phi=" << phi << std::endl;
#endif

      if ( (name.index("LAr::EMEC::Front")==0) ||
           (name.index("LAr::EMEC::MotherBoard::")==0) ||
           (name.index("LAr::EMEC::Outer")==0) ||
           (name.index("LAr::EMEC::TopIndexing")==0) ||
           (name.index("LAr::EMEC::WideStretcher")==0) ||
           (name.index("LAr::EMEC::NarrowStretcher")==0) ||
           (name.index("LAr::EMEC::InnerAluCone")==0) ||
           (name.index("LAr::EMEC::ExtraCyl_afterPS")==0) || 
           (name=="LAr::EMEC::Mother" && fabs(p.z()) < m_par->m_zInFrontOfSpanishFan) )
      {
        // case 1  // Front + Outer + Inner of EMEC
        type = 1;
        if ( eta < 1.4 ) {
          sampling = 2;
          region = 5;
          etaBin = (int) ( (eta-1.) * oneOverDeta );
        } else if ( eta < 1.5 ) {
          if( fabs(p.z()) < m_par->m_zzInFrontOfMiddle) {
            sampling = 1;
            region = 6;
            etaBin = (int) ( (eta-1.3) * oneOverDeta );
          } else {
            sampling = 2;
            region = 5;
            etaBin = (int) ( (eta-1.) * oneOverDeta );
          }
        } else if ( eta < 3.2 ) {
          sampling = 1;
          region = 6;
          etaBin = (int) ( (eta-1.3) * oneOverDeta );
        } else if ( eta < 5.0 ) {          // we expect that eta < 5.0
          if(fabs(p.z()) < m_par->m_z1BeforeFCal) {
            type = 1;
            sampling = 1;
            region = 7;
            etaBin = (int) ( (eta-3.2) * oneOverDeta );
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
        } else if (eta < 8.0) {
          sampling = 3;
          region = 1;
          etaBin = (int) ( (eta-5.0) * 0.5 * oneOverDeta );
        } else {
          sampling = 3;
          region = 2;
          etaBin = 0;
          phiBin = 0;
        }

      } else if ( name == "LAr::EMEC::InnerTransversalBars" ||
                  (name=="LAr::EMEC::Mother" && fabs(p.z()) < m_par->m_zBehindOfSpanishFan) ) {
        // case 2  - crack between inner and outer EMEC wheels 
        //LArEMECSupportConstruction::middle_envelope(void)
        // LArEMECHalfCrack
        // g.p. 21/04/2006
        // Actually, here we can get hits corresponding not only to the crack
        // between inner and outer wheel, but also hits between emec and 
        // TileExt and even hits before FCAL.

        if( eta < 1.7) {
          // zone 4125 - beween emec and ExtTile
          type = 1;
          sampling = 2;
          region = 5;
          etaBin = (int) ( (eta-1.) * oneOverDeta );
        }else if( eta < 2.4) {
          // zone 4116 - before EME1 (very rare hits from LAr::EMEC::Mother)
          type = 1;
          sampling = 1;
          region = 6;
          etaBin = (int) ( (eta-1.3) * oneOverDeta );
        } else if (eta < 2.6) {
          // 4201,4211,4221, 4231 - between inner and outer wheel
          type = 2;
          region = 1;
          etaBin = 0;
          if(fabs(p.z()) > m_par->m_zzInFrontOfPresampler && fabs(p.z()) < m_par->m_zzInFrontOfStrips) {
            sampling = 0; // it's never nappens
          }else if(fabs(p.z()) < m_par->m_zzInFrontOfMiddle) {
            sampling = 1;
          }else if(fabs(p.z()) < m_par->m_zzInFrontOfBack) {
            sampling = 2;
          }else{
            sampling = 3;
          }
        }else if( eta < 3.2) {
            // zone 4116 - before EME1 (again very rare hits from LAr::EMEC::Mother)
            type = 1;
            sampling = 1;
            region = 6;
            etaBin = (int) ( (eta-1.3) * oneOverDeta );
        } else if (eta >= 3.2) {
          // 4117 - before FCAL near EMEC
          if(fabs(p.z()) < m_par->m_z1BeforeFCal) {
            type = 1;
            sampling = 1;
            region = 7;
            etaBin = (int) ( (eta-3.2) * oneOverDeta );
          // 4205 - before FCAL near HEC
          } else if (fabs(p.z()) < m_par->m_z2BeforeFCal){
            type = 2;
            sampling = 0;
            region = 5;
            etaBin = (int) ( (eta-3.0) * oneOverDeta );
          // 4215 - before FCAL1
          } else {
            type = 2;
            sampling = 1;
            region = 5;
            etaBin = (int) ( (eta-3.0) * oneOverDeta );
          }
        }

      } else if ((name.index("LAr::EMEC::Back")==0) ||
                  (name=="LAr::EMEC::Mother" && fabs(p.z()) < m_par->m_zInFrontOfHEC) ) {
        // case 3  - between EMEC and HEC
        type = 1;
        if ( eta < 1.5 ) {
          // the outer parts of 
          //"LAr::EMEC::BackOuterRing"
          //"LAr::EMEC::BackOuterLongBar",
          //"LAr::EMEC::BackOuterBarrette"
          // can be in sampling = 2, region = 5     

          sampling = 2;
          region = 5;
          etaBin = (int) ( (eta-1.) * oneOverDeta );
        } else if ( eta < 3.3 ) {
          if (fabs(p.z()) < m_par->m_zBehindOfSpanishFan && eta > 3.2) {
            // 4117 - before FCAL near EMEC
            if(fabs(p.z()) < m_par->m_z1BeforeFCal) {
              type = 1;
              sampling = 1;
              region = 7;
              etaBin = (int) ( (eta-3.2) * oneOverDeta );
            // 4205 - before FCAL near HEC
            } else if (fabs(p.z()) < m_par->m_z2BeforeFCal){
              type = 2;
              sampling = 0;
              region = 5;
              etaBin = (int) ( (eta-3.0) * oneOverDeta );
            // 4215 - before FCAL1
            } else {
              type = 2;
              sampling = 1;
              region = 5;
              etaBin = (int) ( (eta-3.0) * oneOverDeta );
            }
          } else {
            // 4123 - between EMEC and HEC
            sampling = 2;
            region = 3;
            etaBin = (int) ( (eta-1.5) * oneOverDeta );
          }
        } else if ( eta < 5.0 ) {
          // 4117 - before FCAL near EMEC
          if(fabs(p.z()) < m_par->m_z1BeforeFCal) {
            type = 1;
            sampling = 1;
            region = 7;
            etaBin = (int) ( (eta-3.2) * oneOverDeta );
          // 4205 - before FCAL near HEC
          } else if (fabs(p.z()) < m_par->m_z2BeforeFCal){
            type = 2;
            sampling = 0;
            region = 5;
            etaBin = (int) ( (eta-3.0) * oneOverDeta );
          // 4215 - before FCAL1
          } else {
            type = 2;
            sampling = 1;
            region = 5;
            etaBin = (int) ( (eta-3.0) * oneOverDeta );
          }
        }
      } else if(name=="LAr::EMEC::Mother" && fabs(p.z()) >= m_par->m_HECzEnd) {
        // 4130 - leakage behind hec and fcal (in very rare cases)
        type = 1;
        sampling = 3;
        region = 0;
        etaBin = (int) ( (eta-1.7) * oneOverDeta );
      }
      // What if we have a G4Step that isn't handled by the above
      // code?  Answer: Use a "backup" calculator to try to
      // process the step.

      if ( type     == INT_MIN  ||
           region   == INT_MIN  ||
           sampling == INT_MIN  ||
           etaBin   == INT_MIN  ||
           phiBin   <  0 || etaBin<0)
      {
// g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
        LArG4::CalibrationDefaultCalculator::Print("UNEXP LArG4EC/EMECSupportCalibrationCalculator",identifier,a_step);
#endif
#if defined (DEBUG_VOLUMES) || defined (DEBUG_HITS)
       static const G4int messageMax = 1000;
        static G4int messageCount = 0; // exists only if debug activated
        if ( messageCount++ < messageMax ) {
          std::cout << "LArG4::EMECSupportCalibrationCalculator::Process"
            << " (error " << messageCount << " of " << messageMax << " max displayed)"
            << std::endl
            << "   G4Step in '"
            << name
            << "' copy="
            << copy
            << " or  G4Step in LAr at unexpected place: (x,y,z) [mm] = ("
            << p.x()/Units::mm << ","
            << p.y()/Units::mm << "," 
            << p.z()/Units::mm
            << "); eta=" << eta
            << ", phi=" << phi << std::endl
            << ", using backup calculator"
            << std::endl;
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
    } // end of if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )

#ifdef DEBUG_HITS
//    G4double energy = accumulate(energies.begin(),energies.end(),0.);
    std::cout << "LArG4::EMECSupportCalibrationCalculator::Process"
      << " ID=" << std::string(identifier)
//      << " energy=" << energy
      << " energies=(" << energies[0]
      << "," << energies[1]
      << "," << energies[2]
      << "," << energies[3] << ")"
      << std::endl;
#endif

// g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
    LArG4::CalibrationDefaultCalculator::Print("DMXYZ LArG4EC/EMECSupportCalibrationCalculator",identifier,a_step);
#endif
    // Check for bad result.
    if ( identifier == LArG4Identifier() ) return false;

    return true;
  }

} // namespace LArG4
