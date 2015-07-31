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
#define DEBUG_VOLUMES
#undef DEBUG_DMXYZ

#include "LArG4EC/CryostatCalibrationLArCalculator.h"
#ifdef DEBUG_DMXYZ
#include "LArG4Code/CalibrationDefaultCalculator.h"
#endif

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/CalibrationDefaultCalculator.h"

// direct access to database
//#include "RDBAccessSvc/IRDBAccessSvc.h"
//#include "RDBAccessSvc/IRDBRecord.h"
//#include "RDBAccessSvc/IRDBRecordset.h"
//#include "GeoModelSvc/IGeoModelSvc.h"
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "StoreGate/StoreGateSvc.h"

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

    VCalibrationCalculator* CalibrationLArCalculator::m_defaultCalculator = 0;

    ///////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////

    CalibrationLArCalculator::CalibrationLArCalculator()
    {
      // Get the default calculator (hopefully a temporary piece of code):
      if ( m_defaultCalculator == 0)
	m_defaultCalculator = new CalibrationDefaultCalculator();

      // Access source of detector parameters.
      m_parameters = LArVG4DetectorParameters::GetInstance();

//      // Access to DB to get parameters
//      StatusCode status;
//      // Access the GeoModelSvc:
//      ISvcLocator *svcLocator = Gaudi::svcLocator();
//      IGeoModelSvc *geoModel;
//      status = svcLocator->service ("GeoModelSvc",geoModel);
//      if (status != StatusCode::SUCCESS) {
//        throw std::runtime_error ("Cannot locate GeoModelSvc!!");
//      }
//      
//      // Access the geometry database:
//      IRDBAccessSvc *pAccessSvc;
//      status=svcLocator->service("RDBAccessSvc",pAccessSvc);
//      if (status != StatusCode::SUCCESS) {
//        throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
//      }
//      
//      // Obtain the geometry version information:
//      std::string AtlasVersion = geoModel->atlasVersion();
//      std::string LArVersion = geoModel->LAr_VersionOverride();      
//      std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
//      std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
//   
//      pAccessSvc->connect();
//      // getting detector parameters
//      const IRDBRecordset *emecGeometry = pAccessSvc->getRecordset("EmecGeometry",detectorKey,detectorNode);
//      if (emecGeometry->size()==0) {
//        throw std::runtime_error("LArG4EC/cryostatCalibrationLArCalculator Cannot find the EmecGeometry table");
//      }
//      const IRDBRecordset *fcalMod = pAccessSvc->getRecordset("FCalMod",detectorKey,detectorNode);
//      if (fcalMod->size()==0) {
//        fcalMod = pAccessSvc->getRecordset("FCalMod","FCalMod-00");
//      }
//      if (fcalMod->size()==0) {
//        throw std::runtime_error("LArG4EC/cryostatCalibrationLArCalculator Cannot find the FcalMod table");
//      }
//
//      // Get the endcap shift from the detector parameters.
//      m_zShift = (*emecGeometry)[0]->getDouble("ZSHIFT")*CLHEP::mm;
//
//      // Get FCAL modules positions
//      m_startZFCal1 = (*fcalMod)[0]->getDouble("STARTPOSITION")*CLHEP::mm;
//      m_startZFCal2 = (*fcalMod)[1]->getDouble("STARTPOSITION")*CLHEP::mm;
//      m_startZFCal3 = (*fcalMod)[2]->getDouble("STARTPOSITION")*CLHEP::mm;
//      m_endZFCal3 = m_startZFCal3 + (*fcalMod)[2]->getDouble("FULLMODULEDEPTH")*CLHEP::mm;
//      pAccessSvc->disconnect();
//
    }


    CalibrationLArCalculator::~CalibrationLArCalculator() 
    {
      // Cleanup pointers.
      delete m_defaultCalculator;
      m_defaultCalculator = 0;
    }


    G4bool CalibrationLArCalculator::Process( const G4Step* a_step,
					      const eCalculatorProcessing a_process )
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      m_energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
	{
	  m_energyCalculator.Energies( a_step, m_energies );
	}
      else
	for (unsigned int i=0; i != 4; i++) m_energies.push_back( 0. );


      m_identifier.clear();
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

	  static const double rhoAlignmentSafety = 10.*CLHEP::mm;

	  static double HEC_MotherRouter = INT_MIN;
	  if ( HEC_MotherRouter < 0.)
            HEC_MotherRouter = m_parameters->GetValue("LArHECmoduleRouter"); //2030.*CLHEP::mm HEC Outer Radius
	  //	  static const double EMEC_MotherRouter = 2077.*CLHEP::mm;
	  // we need only lesser of EMEC_MotherRouter, HEC_MotherRouter
	  // for misalignment-safe determination what is out of them: 
	  static const double rhoOutOfEmecHec = HEC_MotherRouter - rhoAlignmentSafety;

	  static double	zShift = INT_MIN;
	  if ( zShift < 0.)
            zShift = m_parameters->GetValue("LArEMECZshift"); // 40mm
	  static const double zAlignmentSafety = zShift + 10.*CLHEP::mm;


	  // At nominal (zShift=0) endcap position absolute z-coordinates:
	  // of the faces of the EndcapPresampler
	  static double	zEndcapPresamplerFrontFace = INT_MIN;
	  if ( zEndcapPresamplerFrontFace < 0.)
            zEndcapPresamplerFrontFace =
	      (m_parameters->GetValue("LArEMECPreNomPos")
	      - m_parameters->GetValue("LArEMECPreThickness")/2.); // 3622. * CLHEP::mm       
	  static const double zInFrontOfPresampler = 
	    zEndcapPresamplerFrontFace + zShift - 20.*CLHEP::mm; 
            // in this case it is safer to decrease Z-boundary and put it 
	    // inside thick Cold Wall
	    // The E-deposit in the Cold Wall itself is handled by
	    // another calculator.

	  // The E-deposit in the Cold Wall itself is handled by
	  // another calculator.

	  static double    zEMECRefPoint = INT_MIN;
	  if ( zEMECRefPoint < 0.)
            zEMECRefPoint = m_parameters->GetValue("LArEMECRefPoint"); // Z0 = 3689.5*mm
	  static const double zInFrontOfSpanishFan = zEMECRefPoint + zAlignmentSafety;

	  static double HECzStart = INT_MIN;
	  if ( HECzStart < 0.)
            HECzStart = m_parameters->GetValue("LArHECzStart"); // 4277.*mm

	  static const double zInFrontOfHEC = HECzStart + zAlignmentSafety;
	  //static const double zInFrontOfFCAL = 4668.5*mm + zAlignmentSafety; // startZFCal1 = 4668.5 * mm;
	  static const double zBehindTile = 6100. + zShift - 50.*CLHEP::mm;	

        // FCAL related parameters
        static const double m_zShift = 40.0;
        static const double m_startZFCal1 = 4668.5 + m_zShift;
        static const double m_startZFCal2 = 5128.3 + m_zShift;
        static const double m_startZFCal3 = 5602.8 + m_zShift;
        static const double m_endZFCal3 = m_startZFCal3 + 442.3;
        static const double m_z1BeforeFCal = 4225.5 + m_zShift; // 368.95 + 53.6
        static const double m_z2BeforeFCal = 4557.5 + m_zShift; // 

        // g.p. 10/09/2008 parameters to find gap boundaries between HEC wheels
        static double m_endZHEC1Wheel = INT_MIN;
        static double m_startZHEC2Wheel = INT_MIN;
        if( m_endZHEC1Wheel < 0) {
          m_endZHEC1Wheel =   HECzStart + zShift + m_parameters->GetValue("LArHECdepthZ",0) + m_parameters->GetValue("LArHECdepthZ",1) + m_parameters->GetValue("LArHECdepthZ",2) - 10.0;
          m_startZHEC2Wheel = HECzStart + zShift + m_parameters->GetValue("LArHECdepthZ",0) + m_parameters->GetValue("LArHECdepthZ",1) + m_parameters->GetValue("LArHECdepthZ",2) + m_parameters->GetValue("LArHECfirstAbsorber",3) + m_parameters->GetValue("LArHECbetweenWheel")+10.0;
          // 5123.5 5209
        }
///* ---------------------------------------------------------------------- */
//s tatic int ifirst=1; // not a thread-safe !!!
//if(ifirst){
//ifirst=0;
//std::cout
//<< " rhoAlignmentSafety:" << rhoAlignmentSafety  << std::endl 
//<< " HEC_MotherRouter:" <<  HEC_MotherRouter  << std::endl 
//<< " rhoOutOfEmecHec:" <<  rhoOutOfEmecHec  << std::endl 
//<< " zShift:" <<   zShift << std::endl 
//<< " zAlignmentSafety:" << zAlignmentSafety  << std::endl 
//<< " zEndcapPresamplerFrontFace:" <<  zEndcapPresamplerFrontFace  << std::endl 
//<< " zInFrontOfPresampler:" <<  zInFrontOfPresampler  << std::endl 
//<< " zEMECRefPoint:" <<  zEMECRefPoint  << std::endl 
//<< " zInFrontOfSpanishFan:" <<  zInFrontOfSpanishFan  << std::endl 
//<< " HECzStart:" <<  HECzStart  << std::endl 
//<< " zInFrontOfHEC:" <<  zInFrontOfHEC  << std::endl 
//<< " zInFrontOfFCAL:" <<  zInFrontOfFCAL  << std::endl 
//<< " zBehindTile:" <<  zBehindTile  << std::endl 
//<< " m_zShift:" <<  m_zShift  << std::endl 
//<< " m_startZFCal1:" <<  m_startZFCal1  << std::endl 
//<< " m_startZFCal2:" <<  m_startZFCal2  << std::endl 
//<< " m_startZFCal3:" <<  m_startZFCal3  << std::endl 
//<< " m_endZFCal3:" <<  m_endZFCal3  << std::endl 
//<< " m_z1BeforeFCal:" <<  m_z1BeforeFCal  << std::endl 
//<< " m_z2BeforeFCal:" <<  m_z2BeforeFCal  << std::endl 
//<< std::endl;
//}
///* ---------------------------------------------------------------------- */

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
	  G4int phiBin = (int) ( phi * oneOverDphi );
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
            etaBin = (int) ( (eta-1.) * oneOverDeta );
          } else if( eta < 1.5 || (rho > rhoOutOfEmecHec && eta < 1.7) ) {
            if (absZ < zEMECRefPoint) {
              sampling = 1;
              region = 6;
              etaBin = (int) ( (eta-1.3) * oneOverDeta );            
            } else {
              sampling = 2;
              region = 5;
              etaBin = (int) ( (eta-1.) * oneOverDeta );
            }
          } else if ( eta < 1.8 && absZ < zInFrontOfPresampler ) { // eta >= 1.5 
            sampling = 1;
            region = 5;
            etaBin = (int) ( (eta-1.5) * oneOverDeta );
          } else if ( eta < 3.2 && absZ < zInFrontOfSpanishFan ) { 
            sampling = 1;
            region = 6;
            etaBin = (int) ( (eta-1.3) * oneOverDeta );
          } else if ( eta < 3.2 && absZ < zInFrontOfHEC ) { // eta >= 1.5
            sampling = 2;
            region = 3;
            etaBin = (int) ( (eta-1.5) * oneOverDeta );
          } else if ( eta >= 2.9 && eta < 5. && absZ < m_startZFCal1 ) {
            if(fabs(p.z()) < m_z1BeforeFCal) {
              sampling = 1;
              region = 7;
              etaBin = (int) ( (eta-3.2) * oneOverDeta );
            } else if (fabs(p.z()) < m_z2BeforeFCal){
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
            if(etaBin < 0) etaBin = 0;
          } else if ( eta < 1.7 && absZ > zBehindTile ) {  //GU change eta>1.7 to eta<1.7 to agree with documentation for subdet =+-5
            subdet = ( p.z() > 0.) ? 5 : -5;
            sampling = 0;
            region = 4;
            etaBin = (int) ( eta * oneOverDeta );
          // G.P. to treat material between HEC and FCAL
          } else if ( eta > 2.9 && eta < 3.3 && absZ > m_startZFCal1 && absZ < m_endZFCal3) {
            if ( fabs(p.z()) < m_startZFCal1 ) {
              type = 2;
              sampling = 1; 
              region = 5;
              etaBin = (int) ( (eta-3.0) * oneOverDeta );
            } else if ( fabs(p.z()) < m_startZFCal2 ) {
              type = 2;
              sampling = 1;
              region = 4;
              etaBin = 0;
            } else if ( fabs(p.z()) < m_startZFCal3 ) {
              type = 2;
              sampling = 2;
              region = 4;
              etaBin = 0;
            } else if ( fabs(p.z()) < m_endZFCal3 ) { //
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
          //GU change for eta above 1.7 for sampling 3
          } else if ( eta < 5. && eta >=1.7 ) {
            sampling = 3;
            region = 0;
            etaBin = (int) ( (eta-1.7) * oneOverDeta );
          } else if ( eta >= 5.  && eta < 8.0) {
            sampling = 3;
            region = 1;
            etaBin = (int) ( (eta-5.)* 0.5 * oneOverDeta );
          } else if (eta>8.) {
            sampling = 3;
            region = 2;
            etaBin = 0;
            phiBin = 0;
          // g.p. 10/09/2008 crack between two HEC wheels (after broke up the HEC into two wheels in GeoModel by M.Fincke)
          } else if ( fabs(p.z()) > m_endZHEC1Wheel && fabs(p.z()) < m_startZHEC2Wheel ) {
            type = 1;
            sampling = 2;
            region = 4;
            etaBin = (int) ( (eta-1.5) * oneOverDeta );
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
           LArG4::CalibrationDefaultCalculator::Print("UNEXP LArG4EC/CryostatCalibrationLArCalculator",m_identifier,a_step,-999.);
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
			    << p.x()/CLHEP::mm << ","
			    << p.y()/CLHEP::mm << "," 
			    << p.z()/CLHEP::mm
			    << "); eta=" << eta
			    << ", phi=" << phi << std::endl
                            << ", rho=" << sqrt(pow(p.x()/CLHEP::mm,2)+pow(p.y()/CLHEP::mm,2))
			    << "   using default calculator"
			    << std::endl;
		}
#endif
	      m_defaultCalculator->Process(a_step, kOnlyID);
	      m_identifier = m_defaultCalculator->identifier();
	    }
	  else
	    {
	      // Append the cell ID to the (empty) identifier.
	      m_identifier << 10         // Calorimeter
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
      G4double energy = accumulate(m_energies.begin(),m_energies.end(),0.);
      std::cout << "LArG4::EndcapCryostat::CalibrationLArCalculator::Process"
		<< " ID=" << std::string(m_identifier)
		<< " energy=" << energy
		<< " energies=(" << m_energies[0]
		<< "," << m_energies[1]
		<< "," << m_energies[2]
		<< "," << m_energies[3] << ")"
		<< std::endl;
#endif

// g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
      LArG4::CalibrationDefaultCalculator::Print("DMXYZ LArG4EC/CryostatCalibrationLArCalculator",m_identifier,a_step);
#endif
      // Check for bad result.
      if ( m_identifier == LArG4Identifier() )
	return false;

      return true;
    }

  } // namespace EndcapCryostat

} // namespace LArG4
