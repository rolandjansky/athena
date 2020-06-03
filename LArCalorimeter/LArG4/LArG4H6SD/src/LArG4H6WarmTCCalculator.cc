/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6WarmTCCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
// #include "LArG4Code/LArVG4DetectorParameters.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4AffineTransform.hh"
#include "G4TouchableHistory.hh"
#include "AthenaKernel/Units.h"

#include "globals.hh"
#include <cmath>


namespace Units = Athena::Units;


LArG4H6WarmTCCalculator::LArG4H6WarmTCCalculator(const std::string& name, ISvcLocator* pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
    //, m_addr(0)
{
  declareProperty("isX", m_isX);
  declareProperty("isABS", m_isABS);
}

G4bool LArG4H6WarmTCCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const
{
  hdata.clear();
  LArHitData larhit;
  larhit.energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();

  // Find out how long it took the energy to get here.
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4double timeOfFlight = (pre_step_point->GetGlobalTime() +
                           post_step_point->GetGlobalTime()) * 0.5;
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;
  //  G4cout<<"LArG4H6WarmTCCalculator::Global point: "<<p.x()<<" "<<p.y()<<" "<<p.z()<<std::endl;
  //larhit.time = timeOfFlight/ns - p.mag()/c_light/ns;
  larhit.time = timeOfFlight/Units::ns;

  //  Get local coordinates of the step, independently of how it was positioned  in World
  const G4AffineTransform transformation =
    pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
  G4ThreeVector   endPointinLocal = transformation.TransformPoint  (endPoint);
  G4ThreeVector          pinLocal =(startPointinLocal+endPointinLocal)*0.5;
  //
  //  G4cout<<"LArG4H6WarmTCCalculator::Local point: "<<pinLocal.x()<<" "<<pinLocal.y()<<" "<<pinLocal.z()<<std::endl;

  G4int zSide;
  G4int sampling;
  G4int region;
  G4int etaBin;
  G4int phiBin;

  zSide = 4;
  if( p.z()<0.) zSide =-4;
  region = 0;

  // We can extract our position from the copy number of depth and module
  // First have touchable
  // G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
  // Volume name
  G4String hitVolume = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  // And copy number
  G4int copyModule = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

  G4int gran;
  // Sampling Identifier
  if(m_isABS) { sampling = copyModule; gran = 1; }
  else {
    switch(copyModule) {
    case 1:  {
      gran = 1;
      if(m_isX)  sampling = copyModule; else sampling = copyModule + 1;
      break;
    }
    case 2: case 3: {
      gran = 2;
      if(m_isX) sampling = 2*copyModule; else sampling = 2*copyModule - 1;
      break;
    }
    default: { sampling = -1; gran = 0; break; }
    }
  }
  //#include "LArG4TBEndcap/LArTBEndcapWTC.icc"
  // This should go int Db:
  double WTC_sci_x = 190.0*CLHEP::mm;
  double WTC_sci_y = 1160.0*CLHEP::mm;
  double x_x = 6 * WTC_sci_x / 2;
  double x_y = WTC_sci_y / 2;

  if(m_isX) {
    etaBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
    phiBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  } else {
    phiBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
    etaBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  }
  region = 0;




  /*
     if(!m_isABS) {
     std::cout<<"LArG4H6WarmTCCalculator: "<<hitVolume<<" "<<copyModule<<" :  "<<pinLocal.x()<<" "<<pinLocal.y()<<std::endl;
     std::cout <<"zSide = "<<zSide<<" , sampling = "<<sampling<<"  ,  region="<<region <<
     " , phiBin="<<phiBin<< " ,  etaBin="<<etaBin <<std::endl;
     std::cout<<m_energy<<" "<<m_time<<std::endl;
     std::cout<<m_isInTime<<" "<<m_isX<<" "<<m_isABS<<"   "<<this<<std::endl;
     }
  */

  larhit.id.clear();
  larhit.id << 10          // LArCalorimeter
            << zSide
            << 1
            << sampling
            << region
            << etaBin
            << phiBin;
  hdata.push_back(larhit);
  // m_addr =  100*sampling+10*etaBin+phiBin;
  // if(m_isABS) m_addr *= -1;
  return true;
}
