/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EndcapCryostat::CalibrationLArCalculator
// 19-Jul-2004 M.Leltchouk - prepared based on examples in
//                           BarrelCryostat::CalibrationLArCalculator

// This class calculates the values needed for calibration hits in the
// volumes:
//      "LAr::Endcap::Cryostat::MotherVolume"
//      "LAr::Endcap::Cryostat::EmecHecLAr"
//      "LAr::Endcap::Cryostat::FcalLAr"

#undef DEBUG_HITS
#undef DEBUG_VOLUMES
#undef DEBUG_DMXYZ

#include "CryostatCalibrationLArCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "AthenaKernel/Units.h"

#include <map>
#include <cmath>
#include <climits>


namespace Units = Athena::Units;


namespace LArG4 {

  namespace EndcapCryostat {

    ///////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////

    CalibrationLArCalculator::CalibrationLArCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_defaultCalculator("CalibrationDefaultCalculator", name)
    {
      declareProperty("CalibrationDefaultCalculator", m_defaultCalculator);
    }

    StatusCode CalibrationLArCalculator::initialize() {

      ATH_CHECK(m_defaultCalculator.retrieve());

      // Access source of detector parameters.
      m_parameters = LArVG4DetectorParameters::GetInstance();

      // -----------------------------------------------------------------------
      //  Load geometrical parameters

      // The fixed parameters (only a couple of which are readily
      // accessible from the database):

      const double rhoAlignmentSafety = 10.*CLHEP::mm;

      const double HEC_MotherRouter = m_parameters->GetValue("LArHECmoduleRouter"); // 2030.*CLHEP::mm
      // HEC Outer Radius
      // for misalignment-safe determination what is out of them:
      m_rhoOutOfEmecHec = HEC_MotherRouter - rhoAlignmentSafety;

      const double zShift = m_parameters->GetValue("LArEMECZshift"); // 40mm

      const double zAlignmentSafety = zShift + 10.*CLHEP::mm;


      // At nominal (zShift=0) endcap position absolute z-coordinates:
      // of the faces of the EndcapPresampler
      const double zEndcapPresamplerFrontFace =
        m_parameters->GetValue("LArEMECPreNomPos") - m_parameters->GetValue("LArEMECPreThickness")/2.;
      // 3622. * CLHEP::mm

      m_zInFrontOfPresampler = zEndcapPresamplerFrontFace + zShift - 20.*CLHEP::mm;
      // in this case it is safer to decrease Z-boundary and put it
      // inside thick Cold Wall
      // The E-deposit in the Cold Wall itself is handled by
      // another calculator.

      // The E-deposit in the Cold Wall itself is handled by
      // another calculator.

      m_zEMECRefPoint = m_parameters->GetValue("LArEMECRefPoint"); // Z0 = 3689.5*mm
      m_zInFrontOfSpanishFan = zEMECRefPoint() + zAlignmentSafety;

      const double HECzStart = m_parameters->GetValue("LArHECzStart"); // 4277.*mm

      m_zInFrontOfHEC = HECzStart + zAlignmentSafety;

      m_zBehindTile = 6100. + zShift - 50.*CLHEP::mm;

      // g.p. 10/09/2008 parameters to find gap boundaries between HEC wheels
      m_endZHEC1Wheel = HECzStart + zShift +
        m_parameters->GetValue("LArHECdepthZ",0) +
        m_parameters->GetValue("LArHECdepthZ",1) +
        m_parameters->GetValue("LArHECdepthZ",2) - 10.0;

      m_startZHEC2Wheel = HECzStart + zShift +
        m_parameters->GetValue("LArHECdepthZ",0) +
        m_parameters->GetValue("LArHECdepthZ",1) +
        m_parameters->GetValue("LArHECdepthZ",2) +
        m_parameters->GetValue("LArHECfirstAbsorber",3) +
        m_parameters->GetValue("LArHECbetweenWheel") + 10.0;

      return StatusCode::SUCCESS;
    }


    CalibrationLArCalculator::~CalibrationLArCalculator()
    {
      // Cleanup pointers.
      //delete m_defaultCalculator;
      //m_defaultCalculator = 0;
    }

    G4bool CalibrationLArCalculator::Process (const G4Step* a_step,
                                              LArG4Identifier & _identifier,
                                              std::vector<G4double> & _energies,
                                              const eCalculatorProcessing a_process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      _energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy ) {
        m_energyCalculator.Energies( a_step, _energies );
      } else {
        for (unsigned int i=0; i != 4; i++)  {
          _energies.push_back( 0. );
        }
      }


      _identifier.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID ) {
        // Calculate the identifier.

        // Note:
        // LArG4::EndcapCryostat::CryostatCalibrationCalculator uses
        // a table-based approach to determine the identifier.  The
        // following code uses an "if-statement" approach.
        static const double sc_oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
        static const double sc_oneOverDphi = 32./M_PI;  //   1/Dphi

        // FCAL related parameters
        static const double sc_zShift = 40.0;
        static const double sc_startZFCal1 = 4668.5 + sc_zShift;
        static const double sc_startZFCal2 = 5128.3 + sc_zShift;
        static const double sc_startZFCal3 = 5602.8 + sc_zShift;
        static const double sc_endZFCal3 = sc_startZFCal3 + 442.3;
        static const double sc_z1BeforeFCal = 4225.5 + sc_zShift; // 368.95 + 53.6
        static const double sc_z2BeforeFCal = 4557.5 + sc_zShift; //

        // Calculate the mid-point of the step, and the simple geometry variables.

        const G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
        const G4StepPoint* post_step_point = a_step->GetPostStepPoint();

        const G4ThreeVector startPoint = pre_step_point->GetPosition();
        const G4ThreeVector endPoint   = post_step_point->GetPosition();
        const G4ThreeVector p = (startPoint + endPoint) * 0.5;

        const G4double rho = p.perp();
        const G4double eta = fabs( p.pseudoRapidity() );
        G4double phi = p.phi();
        if ( phi < 0. ) phi += 2.*M_PI; // Normalize for phiBin calculation

        const G4double absZ = fabs(p.z());

        // subdet = +/-4    "+" or " -" according to sign of Z in World coorinate
        G4int subdet = ( p.z() > 0.) ? 4 : -4;
        G4int type = 1;
        G4int phiBin = (int) ( phi * sc_oneOverDphi );
        // to handle rounding error problem
        if(phiBin > 63) phiBin = 0;

        // Initialize identifier variables with (invalid) default
        // values (INT_MIN is defined in <climits>).
        G4int sampling = INT_MIN;
        G4int region = INT_MIN;
        G4int etaBin = INT_MIN;

        if( eta < 1.4) {
          sampling = 2;
          region = 5;
          etaBin = (int) ( (eta-1.) * sc_oneOverDeta );
        } else if( eta < 1.5 || (rho > rhoOutOfEmecHec() && eta < 1.7) ) {
          if (absZ < zEMECRefPoint()) {
            sampling = 1;
            region = 6;
            etaBin = (int) ( (eta-1.3) * sc_oneOverDeta );
          } else {
            sampling = 2;
            region = 5;
            etaBin = (int) ( (eta-1.) * sc_oneOverDeta );
          }
        } else if ( eta < 1.8 && absZ < zInFrontOfPresampler() ) { // eta >= 1.5
          sampling = 1;
          region = 5;
          etaBin = (int) ( (eta-1.5) * sc_oneOverDeta );
        } else if ( eta < 3.2 && absZ < zInFrontOfSpanishFan() ) {
          sampling = 1;
          region = 6;
          etaBin = (int) ( (eta-1.3) * sc_oneOverDeta );
        } else if ( eta < 3.2 && absZ < zInFrontOfHEC() ) { // eta >= 1.5
          sampling = 2;
          region = 3;
          etaBin = (int) ( (eta-1.5) * sc_oneOverDeta );
        } else if ( eta >= 2.9 && eta < 5. && absZ < sc_startZFCal1 ) {
          if(fabs(p.z()) < sc_z1BeforeFCal) {
            sampling = 1;
            region = 7;
            etaBin = (int) ( (eta-3.2) * sc_oneOverDeta );
          } else if (fabs(p.z()) < sc_z2BeforeFCal){
            type = 2;
            sampling = 0;
            region = 5;
            etaBin = (int) ( (eta-3.0) * sc_oneOverDeta );
          } else {
            type = 2;
            sampling = 1;
            region = 5;
            etaBin = (int) ( (eta-3.0) * sc_oneOverDeta );
          }
          if(etaBin < 0) etaBin = 0;
        } else if ( eta < 1.7 && absZ > zBehindTile() ) {  //GU change eta>1.7 to eta<1.7 to agree with documentation for subdet =+-5
          subdet = ( p.z() > 0.) ? 5 : -5;
          sampling = 0;
          region = 4;
          etaBin = (int) ( eta * sc_oneOverDeta );
          // G.P. to treat material between HEC and FCAL
        } else if ( eta > 2.9 && eta < 3.3 && absZ > sc_startZFCal1 && absZ < sc_endZFCal3) {
          if ( fabs(p.z()) < sc_startZFCal1 ) {
            type = 2;
            sampling = 1;
            region = 5;
            etaBin = (int) ( (eta-3.0) * sc_oneOverDeta );
          } else if ( fabs(p.z()) < sc_startZFCal2 ) {
            type = 2;
            sampling = 1;
            region = 4;
            etaBin = 0;
          } else if ( fabs(p.z()) < sc_startZFCal3 ) {
            type = 2;
            sampling = 2;
            region = 4;
            etaBin = 0;
          } else if ( fabs(p.z()) < sc_endZFCal3 ) { //
            type = 2;
            sampling = 3;
            region = 4;
            etaBin = 0;
          } else{ // leakage behind FCAL+HEC
            type = 1;
            sampling = 3;
            region = 0;
            etaBin = (int) ( (eta-1.7) * sc_oneOverDeta );
          }
          //GU change for eta above 1.7 for sampling 3
        } else if ( eta < 5. && eta >=1.7 ) {
          sampling = 3;
          region = 0;
          etaBin = (int) ( (eta-1.7) * sc_oneOverDeta );
        } else if ( eta >= 5.  && eta < 8.0) {
          sampling = 3;
          region = 1;
          etaBin = (int) ( (eta-5.)* 0.5 * sc_oneOverDeta );
        } else if (eta>8.) {
          sampling = 3;
          region = 2;
          etaBin = 0;
          phiBin = 0;
          // g.p. 10/09/2008 crack between two HEC wheels (after broke up the HEC into two wheels in GeoModel by M.Fincke)
        } else if ( fabs(p.z()) > endZHEC1Wheel() && fabs(p.z()) < startZHEC2Wheel() ) {
          type = 1;
          sampling = 2;
          region = 4;
          etaBin = (int) ( (eta-1.5) * sc_oneOverDeta );
        } else {
          std::cout << "CryostatCalibrationLArCalculator id not found " << eta << " " << absZ << std::endl;
          std::cout << "type: " << type << " region:" << region << " sampling: " << sampling << " etaBin: " << etaBin << " phiBin: " << phiBin << std::endl;
        }

        // This is a "quick fix" for a complex issue: We're still
        // developing code for the cryostat.  What if, somehow, we have
        // a G4Step in a LAr or Mother volume that isn't handled by the above
        // code?  Answer: Use the default calibration calculator (the
        // same one used for volumes without sensitive detectors) to get
        // the identifier.

        if ( type     == INT_MIN  ||
             region   == INT_MIN  ||
             sampling == INT_MIN  ||
             etaBin   == INT_MIN  ||
             phiBin   <  0 || etaBin < 0)
          {
            // g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
            LArG4::CalibrationDefaultCalculator::Print("UNEXP LArG4EC/CryostatCalibrationLArCalculator",_identifier,a_step,-999.);
#endif
#if defined (DEBUG_VOLUMES) || defined (DEBUG_HITS)
            static const G4int messageMax = 10;
            static G4int messageCount = 0; // exists only if debug activated
            if ( messageCount++ < messageMax )
              {
                std::cout << "LArG4::EndcapCryostat::CalibrationLArCalculator::Process"
                          << " (error " << messageCount << " of " << messageMax << " max displayed)"
                          << std::endl
                          << "   G4Step in LAr at unexpected place: (x,y,z) [mm] = ("
                          << p.x()/Units::mm << ","
                          << p.y()/Units::mm << ","
                          << p.z()/Units::mm
                          << "); eta=" << eta
                          << ", phi=" << phi << std::endl
                          << ", rho=" << sqrt(pow(p.x()/Units::mm,2)+pow(p.y()/Units::mm,2))
                          << "   using default calculator"
                          << std::endl;
              }
#endif
            //m_defaultCalculator->Process(a_step, kOnlyID);
            //_identifier = m_defaultCalculator->identifier();
            std::vector<G4double> _tmpv;
            m_defaultCalculator->Process(a_step, _identifier, _tmpv, kOnlyID);
          }
        else
          {
            // Append the cell ID to the (empty) identifier.
            _identifier << 10         // Calorimeter
                        << subdet     // LAr +/-4,5 where "+" or " -" according to
              // the sign of Z in World coorinate
                        << type
                        << sampling
                        << region
                        << etaBin
                        << phiBin;
            if (type==1 && sampling==3 && region==0 && etaBin>32) {
              std::cout << "Invalid ID S3 r3 CryostatLArCalculator " << eta << " " << etaBin << std::endl;
            }
          }
      }

#ifdef DEBUG_HITS
      G4double energy = accumulate(_energies.begin(),_energies.end(),0.);
      std::cout << "LArG4::EndcapCryostat::CalibrationLArCalculator::Process"
                << " ID=" << std::string(_identifier)
                << " energy=" << energy
                << " energies=(" << _energies[0]
                << "," << _energies[1]
                << "," << _energies[2]
                << "," << _energies[3] << ")"
                << std::endl;
#endif

      // g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
      LArG4::CalibrationDefaultCalculator::Print("DMXYZ LArG4EC/CryostatCalibrationLArCalculator",_identifier,a_step);
#endif
      // Check for bad result.
      if ( _identifier == LArG4Identifier() ) {
        return false;
      }

      return true;
    }

  } // namespace EndcapCryostat

} // namespace LArG4
