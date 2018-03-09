/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004DeadCalibrationCalculator.h"

#include "G4TouchableHistory.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "globals.hh"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

//#include <algo.h>

//#include "LArG4Code/CalibrationDefaultCalculator.h"
#undef DEBUG_DMXYZ


using CLHEP::Hep3Vector;
using CLHEP::HepRotation;


static double yshift=-538.66*CLHEP::mm;
static double zshift = -7343.366*CLHEP::mm;
static double alpha = 4.668*CLHEP::degree;
static double yrot = 45.*CLHEP::degree;

static Hep3Vector shift;
static HepRotation m13,my3;

static const double zSamplings[10] = {
  3691.*CLHEP::mm,                                                   // z-start of EMEC active part
  (3691. + 536.)*CLHEP::mm,                                          // z-end of EMEC active part
  (4317. + 12.5)*CLHEP::mm,                                          // z-start of HEC-1 active part
  (4317. + 280.5 - 12.5 + 2*268.)*CLHEP::mm,                         // z-end of HEC-1
  (4317. + 280.5 - 12.5 + 2*268. + 40.5 + 25.)*CLHEP::mm,            // z-start of HEC-2 active part
  (4317. + 280.5 - 12.5 + 2*268. + 40.5 + 25. + 2*234.)*CLHEP::mm,   // z-end of HEC-2
  4668.5*CLHEP::mm,                                                  // z-start of FCAL1
  5128.3*CLHEP::mm,                                                  // z-start of FCAL2
  5602.8*CLHEP::mm,                                                  // z-start of FCAL3
  (5602.8+280.)*CLHEP::mm                                            // z-end of FCAL3
};

enum smp_start_keys { zStartEMEC, zEndEMEC, zStartHEC1, zEndHEC1, zStartHEC2, zEndHEC2,
    zStartFCAL1, zStartFCAL2, zStartFCAL3, zEndFCAL3};

static const double dHec12 = 2*268.*CLHEP::mm; // depth of second sampling in HEC
static const double dHec22 = 2*234.*CLHEP::mm; // depth of fourth sampling in HEC
//static const double shiftTB = 7339*CLHEP::mm; // Z_TB -> Z_ATLAS
static const double EMrouter = 698.6*CLHEP::mm; // Outer part of the  EMEC inner wheel
static const double HECrouter = 1159.0*CLHEP::mm; // Outer part of our HEC module
//static const double EMrinner = 301.4*CLHEP::mm; //  Inner r of the EMEC
static const double HECrinner1 = 372.0*CLHEP::mm; // Inner part of the HEC
static const double HECrinner2 = 475.0*CLHEP::mm; // Inner part of the HEC
static const double FCALrinner = 71.8*CLHEP::mm; // Inner part of the FCAL
static const double FCALrouter = 450.*CLHEP::mm; // Outer part of the FCAL
static const double TBzStart = 11067.*CLHEP::mm;
//static const double TBzEnd = 12500.*CLHEP::mm;
static const double offset = 20.*CLHEP::mm; // safety offset while calculated edges of different zones


namespace {

inline
int etaToBin1 (G4double eta, G4double eta0)
{
  return static_cast<int> ((eta - eta0) * (1./0.1));
}


inline
int etaToBin2 (G4double eta, G4double eta0)
{
  return static_cast<int> ((eta - eta0) * (1./0.2));
}

}


LArG4H62004DeadCalibrationCalculator::LArG4H62004DeadCalibrationCalculator(const std::string& name, ISvcLocator * pSvcLocator)
  : LArCalibCalculatorSvcImp(name, pSvcLocator)
{
  // Make sure there are no uninitialized variables.

  Hep3Vector sh(0., yshift, zshift);
  const Hep3Vector colX(cos(yrot),sin(yrot),0);
  const Hep3Vector colY(-cos(alpha)*sin(yrot),cos(alpha)*cos(yrot),-sin(alpha));
  const Hep3Vector colZ(-sin(alpha)*sin(yrot),sin(alpha)*cos(yrot),cos(alpha));
  m13 = HepRotation(colX, colY, colZ);
  const Hep3Vector colXX(cos(yrot),sin(yrot),0);
  const Hep3Vector colYY(-sin(yrot),cos(yrot),0);
  const Hep3Vector colZZ(0,0,1);
  my3 = HepRotation(colXX, colYY, colZZ);

  shift = my3*sh;
}


LArG4H62004DeadCalibrationCalculator::~LArG4H62004DeadCalibrationCalculator()
{
}


G4bool LArG4H62004DeadCalibrationCalculator::Process( const G4Step* a_step, LArG4Identifier & identifier,
                                                      std::vector<G4double> & energies,
                                                      const LArG4::eCalculatorProcessing a_process ) const
{
  // Some hard-coded numbers.
  // Think how to put it more correctly !!!!
  // Still not finished !!!!!!
  // Make first transformation to ATLAS, and then use the data from DB
  energies.clear();
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyEnergy ) {
    /*
    CaloG4::SimulationEnergies::ClassifyResult_t category =
    m_energyCalculator->Classify( a_step );

    // This is where we decide which energy categories are assigned
    // to the energy fields in a calibration hit.

    m_energies.push_back( category.energy[CaloG4::SimulationEnergies::kEm] );
    m_energies.push_back( category.energy[CaloG4::SimulationEnergies::kNonEm] );
    m_energies.push_back( category.energy[CaloG4::SimulationEnergies::kInvisible0] );
    m_energies.push_back( category.energy[CaloG4::SimulationEnergies::kEscaped] );
    */
    m_energyCalculator.Energies( a_step, energies );
  } else {
    for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );
  }

  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyID ) {
    // identifier = LArG4Identifier();
    identifier.clear();
    // Calculate the identifier.
    G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
    G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
    // Volume name
    G4String hitVolume = theTouchable->GetVolume(0)->GetName();
    //   if(hitVolume.contains("::") ) {
    //     int last = hitVolume.last(':');
    //     hitVolume.remove(0,last+1);
    //   }
    int h_num = pre_step_point->GetPhysicalVolume()->GetCopyNo();
    int type,sampling,region,etaBin,phiBin;

//       std::cout<<"LArG4H62004DeadCalibrationCalculator::Process "<< hitVolume<<std::endl;

    G4ThreeVector tbPoint = pre_step_point->GetPosition();
    G4ThreeVector gPoint;
       // Move all points before calorimeter to one plane
    if(tbPoint.z() < TBzStart) {
      tbPoint -= G4ThreeVector(0.,0.,tbPoint.z()-TBzStart);
    }

    gPoint = m13*tbPoint + shift;
    double gz = gPoint.z();

    type = INT_MIN;
    sampling = INT_MIN;
    region = INT_MIN;
    etaBin = INT_MIN;
    phiBin = INT_MIN;

    //  std::cout<<"Global: "<<tbPoint<<" ,Atlas: "<<gPoint<<std::endl;
    type = 1; sampling = 0; region = 0; etaBin = 0; phiBin = 0;
    // First check, if we are not in the leakage det.
//    if(h_num > 10000) {
//    type = 3; sampling = 0; region = 0; etaBin = 0; phiBin = h_num - 10000;
//    } else { // We should compute the ATLAS-like ID
      // First eta, phi
      double gr = sqrt(gPoint.x()*gPoint.x() + gPoint.y()*gPoint.y());
      G4double eta = fabs( gPoint.pseudoRapidity() );
      G4double phi = gPoint.phi();
      if ( phi < 0. ) phi += 2.*M_PI; // Normalize for phiBin calculation
      if(eta<1.5) eta = 1.5;

      phiBin = int(phi * (64 / (2*M_PI)));

      // First the type, sampling and region
      if( gz < zSamplings[zStartEMEC]) { // before the EMEC
        type = 1; sampling = 1;
        if(eta < 3.2) {
           region = 6;
           etaBin = etaToBin1 (eta, 1.3);
        } else {
           region = 7;
           etaBin = etaToBin1 (eta, 3.2);
        }
      } else if( gz < zSamplings[zEndEMEC]) { // in EMEC
        if(gr > EMrouter) { // outer
          if( gz < zSamplings[zStartEMEC]) {
            type = 1; sampling = 1; region = 6;
            etaBin = etaToBin1 (eta, 1.3);
          // material "between" inner and outer wheel ('excluder')
          }else if( gz < (zSamplings[zStartEMEC]+68.3575) ) {
            type = 2; sampling = 1; region = 1; etaBin = 0;
          }else if( gz < (zSamplings[zStartEMEC]+68.3575+253.6737) ) {
            type = 2; sampling = 2; region = 1; etaBin = 0;
          }else{
            type = 2; sampling = 3; region = 1; etaBin = 0;
          }
        }else{
          if(eta<2.6){
            type = 2; sampling = 1; region = 1;
            etaBin = 0;
          }else if(eta<3.2) {
            type = 1; sampling = 1; region = 6;
            etaBin = etaToBin1 (eta, 1.3);
          }else if(eta<5.0) {
            type = 1; sampling = 1; region = 7;
            etaBin = etaToBin1 (eta, 3.2);
          }else if(eta < 8.0) {
            type = 1; sampling = 3; region = 1;
            etaBin = etaToBin2 (eta, 5.0);
          }else{
            type = 1; sampling = 3; region = 2;
            etaBin = 0;
          }
        }
      } else if( gz < zSamplings[zStartHEC1]) { // crack EMEC - HEC
        if(eta<3.2){ // crack emec-hec
          type = 1; sampling = 2; region = 3;
          etaBin = etaToBin1 (eta, 1.5);
        }else if (eta<5.0) { // before FCAL
          type = 1; sampling = 1; region = 7;
          etaBin = etaToBin1 (eta, 3.2);
        }else if (eta<8.0) {
          type = 1; sampling = 3; region = 1;
          etaBin = etaToBin2 (eta, 5.0);
        }else{
          type = 1; sampling = 3; region = 2;
          etaBin = 0;
        }
      } else if(gr > HECrinner2 || (gr > HECrinner1 && gz < zSamplings[zEndHEC1]-dHec12) ) { // HEC

        if( gz < zSamplings[zEndHEC1]) { // HEC1
          if(gr > HECrouter) {
            type =1;
            if(eta<1.7){
              sampling = 2; region = 5;
              etaBin = etaToBin1 (eta, 1.0);
            }else{
              sampling = 3; region = 0;
              etaBin = etaToBin1 (eta, 1.7);
            }
          } else { // 4202
            type = 2;
            if(zSamplings[zEndHEC1] - gz < dHec12) sampling = 1; else sampling = 0;
            phiBin = int(phi * (32/(2*M_PI)));
            if(eta > 2.5) {
              region = 3;
              etaBin = etaToBin2 (eta, 2.5);
            } else {
              region = 2;
              etaBin = etaToBin1 (eta, 1.5);
            }
          }
        } else if( gz < zSamplings[zStartHEC2]) { // crack HEC1 - HEC2
          type = 1; sampling = 2; region = 4;
          etaBin = etaToBin1 (eta, 1.5);
        } else if( gz < zSamplings[zEndHEC2]) { // HEC2
          if(gr > HECrouter) {
            type =1;
            if(eta<1.7){
              sampling = 2; region = 5;
              etaBin = etaToBin1 (eta, 1.0);
            }else{
              sampling = 3; region = 0;
              etaBin = etaToBin1 (eta, 1.7);
            }
          } else {
            type = 2;
            sampling = 2;
            phiBin = int(phi * (32/(2 * M_PI)));
            if(eta > 2.5) {
              region = 3;
              etaBin = etaToBin2 (eta, 2.5);
            } else {
              region = 2;
              etaBin = etaToBin1 (eta, 1.5);
            }
          }
/*          std::cout << " XXX_3: gz: " << gz << " gr: "  << gr
              << " zSamplings[zEndHEC1]: " << zSamplings[zEndHEC1]
              << " zSamplings[zStartHEC2]: " << zSamplings[zStartHEC2]
              << " zSamplings[zEndHEC2]: " << zSamplings[zEndHEC2]
              << " dHec22: " << dHec22
              << " zSamplings[zEndHEC1] - gz" << zSamplings[zEndHEC1] - gz
              << std::endl;
          std::cout << " XXX_3 type: " << type
              << " sampling: " << sampling
              << " region: " << region
              << " etaBin: " << etaBin
              << " phiBin: " << phiBin
              << " eta: " << eta
              << " phi: " << phi
              << std::endl;*/
        } else { // material behind HEC2
          if(eta<1.7){
            type = 1; sampling = 2; region = 5;
            etaBin = etaToBin1 (eta, 1.0);
          }else{
            type = 1; sampling = 3; region = 0;
            etaBin = etaToBin1 (eta, 1.7);
          }
        }
      } else if(gz < zSamplings[zStartFCAL1]) { //before the FCAL
        if(eta>=8.0) {
          type = 1; sampling = 3; region = 2;
          etaBin = 0;
        }else if(eta>=5.0){
          type = 1; sampling = 3; region = 1;
          etaBin = etaToBin2 (eta, 5.0);
          if(etaBin < 0) etaBin = 0;
        }else{
          if(gz < zSamplings[zEndEMEC]){ // 4117
            type = 1; sampling = 1; region = 7;
            etaBin = etaToBin1 (eta, 3.2);
            if(etaBin < 0) etaBin = 0;
          }else if(gz < (zSamplings[zEndEMEC] + 280.5)){ // 4205
            type = 2; sampling = 0; region = 5;
            etaBin = etaToBin1 (eta, 3.0);
            if(etaBin < 0) etaBin = 0;
          }else{ // 4215
            type = 2; sampling = 1; region = 5;
            etaBin = etaToBin1 (eta, 3.0);
            if(etaBin < 0) etaBin = 0;
          }
        }


//         if(eta<3.2){ // crack emec-hec
//           // LeakageDet::ColdNose it's a small corner between HEC0 and FCAL1
//           type = 1; sampling = 1; region = 7;
//           etaBin = 0;
//         }else if (eta<5.0) { // before FCAL
//           if(gz < zSamplings[zEndEMEC]){ // 4117
//             type = 1; sampling = 1; region = 7;
//             etaBin = int((eta - 3.2)/0.1);
//           }else if(gz < zSamplings[zStartHEC1]){ // 4105
//             type = 2; sampling = 0; region = 5;
//             etaBin = int((eta - 3.0)/0.1);
//           }else{ // 4215
//             type = 2; sampling = 1; region = 5;
//             etaBin = int((eta - 3.0)/0.1);
//           }
//         }else if (eta<8.0) {
//           type = 1; sampling = 3; region = 1;
//           etaBin = int((eta - 5.0)/0.2);
//         }else{
//           type = 1; sampling = 3; region = 2;
//           etaBin = 0;
//         }




      } else if(gz > zSamplings[zEndFCAL3]) { //after the FCAL
        if(eta < 3.2) {
        }else if(eta < 5) {
          type = 1; sampling = 3; region = 0;
          etaBin = etaToBin1 (eta, 1.7);
        } else if(eta < 8) {
          type = 1; sampling = 3; region = 1;
          etaBin = etaToBin2 (eta, 5.0);
        } else {
          type = 1; sampling = 3; region = 2; etaBin = 0;
        }
      }else if(gr>FCALrouter - offset) { // FCAL - HEC crack
        type = 2; region = 4; etaBin = 0;
        if(gz < zSamplings[zEndHEC1]-dHec12) sampling = 0;
        else if(gz < zSamplings[zEndHEC1]) sampling = 1;
        else if(gz < zSamplings[zEndHEC2] - dHec22) sampling = 2;
        else sampling = 3;


      } else if(gr> FCALrinner) { // between FCAL modules (and before/after fcal for safety)
        if( eta < 5.0) {
          if(gz < zSamplings[zStartFCAL1] + offset*2.0) {
            type = 2; sampling = 1; region = 5;
            etaBin = etaToBin1 (eta, 3.0);
            if(etaBin < 0) etaBin = 0;
//             type = 1; sampling = 1; region = 7; // 4117
//             etaBin = int((eta - 3.2)/0.1);
//             if(etaBin < 0) etaBin = 0;
          }else if(gz > zSamplings[zStartFCAL2]-offset && gz < zSamplings[zStartFCAL2]+offset) {
            type = 2; sampling = 2; region = 5; // 4225
            etaBin = etaToBin1 (eta, 3.0);
          }else if(gz > zSamplings[zStartFCAL3]-offset && gz < zSamplings[zStartFCAL3]+offset) {
            type = 2; sampling = 3; region = 5; // 4235
            etaBin = etaToBin1 (eta, 3.0);
          } else {
            type = 1; sampling = 3; region = 0; // 4130
            etaBin = etaToBin1 (eta, 1.7);
          }
        } else {
          type = 1; sampling = 3; region = 1; // 4131
          etaBin = etaToBin2 (eta, 5.0);
        }
      } else {
        // God knows what could be here
        if(eta<5.0){
          if(gz < zSamplings[zStartFCAL1]+offset*2.0 ) {
            type = 2; sampling = 1; region = 5;
            etaBin = etaToBin1 (eta, 3.0);
            if(etaBin < 0) etaBin = 0;
//             type = 1; sampling = 1; region = 7; // 4117
//             etaBin = int((eta - 3.2)/0.1);
//             if(etaBin < 0) etaBin = 0;
          }else{
            type = 1; sampling = 3; region = 0; // 4130
            etaBin = etaToBin2 (eta, 1.7);
          }
        }else{
          type = 1; sampling = 3; region = 1; // 4131
          etaBin = etaToBin2 (eta, 5.0);
        }
      }
//    }

    if ( type == INT_MIN || region == INT_MIN || sampling == INT_MIN ||
         etaBin == INT_MIN || phiBin <  0 || etaBin < 0) {
      std::cout << "LArG4H62004DeadCalibrationCalculator -> Error! G4Step in unexpected place!" << std::endl;
      std::cout << " gPoint >>>   eta:" << gPoint.pseudoRapidity()
          << " x: " << gPoint.x()
          << " y: " << gPoint.y()
          << " z: " << gPoint.z()
          << " rho: " << sqrt(gPoint.x()*gPoint.x()+gPoint.y()*gPoint.y())
          << " hitVolume: " << hitVolume
          << " CopyNo: " << h_num
          << " type: " << type
          << " region: " << region
          << " sampling: " << sampling
          << " etaBin: " << etaBin
          << " phiBin: " << phiBin
          << std::endl;
#ifdef DEBUG_DMXYZ
//          std::cout<<"DMXYZ task: "<< a_process <<std::endl;
//          std::cout<<"UNEXP LArG4H62004DeadCalibrationCalculator "<<std::string(identifier)<<std::endl;

         char str[1024];
         sprintf(str,"UNEXP LArG4H6SD/LArG4H62004DeadCalibrationCalculator_%d",(int)a_process);
         LArG4::CalibrationDefaultCalculator::Print(str,identifier,a_step,energies);
#endif
         } else{

  if(phiBin == 64) phiBin = 63;
  identifier  << 10          // Calorimeter
      << 4          // Sub-det
      << type
      << sampling
      << region
      << etaBin
      << phiBin;
#ifdef DEBUG_DMXYZ
//     std::cout<<"DMXYZ task: "<< a_process <<std::endl;
//     std::cout<<"DMXYZ LArG4H62004DeadCalibrationCalculator: "<<std::string(identifier)<<std::endl;
    char str[1024];
    sprintf(str,"DMXYZ LArG4H6SD/LArG4H62004DeadCalibrationCalculator_%d",(int)a_process);
    LArG4::CalibrationDefaultCalculator::Print(str,identifier,a_step,energies);

//    LArG4::CalibrationDefaultCalculator::Print("DMXYZ LArG4H62004DeadCalibrationCalculator",identifier,a_step,energies);
#endif
         }
  }
  else
// identifier = LArG4Identifier();
    identifier.clear();

      // Check for bad result.
  if ( identifier == LArG4Identifier() ){
     std::cout<<"LArG4H62004DeadCalibrationCalculator bad identifier, return false...."<<std::endl;
    return false;
  }

  return true;
}
