/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6WarmTCCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4AffineTransform.hh"
#include "G4TouchableHistory.hh"

#include "globals.hh"
// 03-Jan-2002 WGS: For 'copysign'.
// #include "LArG4Code/ansi-compliance.h"
#include <cmath>


LArG4H6WarmTCCalculator::LArG4H6WarmTCCalculator(WTCComp icomp)
  :m_identifier(),m_addr(0),m_time(0),m_energy(0),m_isInTime(false)
{
  // Constructor initializes the geometry.

  m_OOTcut = LArVG4DetectorParameters::GetInstance()->GetValue("LArExpHallOutOfTimeCut");

  switch(icomp) {
     case WTC_X: { m_isX = true; m_isABS = false; break;}
     case WTC_Y: { m_isX = false; m_isABS = false; break;}
     case WTC_ABS: { m_isX = false; m_isABS = true; break;}
  }
}


G4bool LArG4H6WarmTCCalculator::Process(const G4Step* a_step)
{

  // First, get the energy.
  m_energy = a_step->GetTotalEnergyDeposit();

  // Find out how long it took the energy to get here.
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4double timeOfFlight = (pre_step_point->GetGlobalTime() + 
                           post_step_point->GetGlobalTime()) * 0.5;
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;
//  G4cout<<"LArG4H6WarmTCCalculator::Global point: "<<p.x()<<" "<<p.y()<<" "<<p.z()<<std::endl;
					 
  //m_time = timeOfFlight/ns - p.mag()/c_light/ns;
  m_time = timeOfFlight/CLHEP::ns;
  if (m_time > m_OOTcut)
    m_isInTime = false;
  else
    m_isInTime = true;

//  Get local coordinates of the step, independently of how it was positioned  in World
  const G4AffineTransform transformation =
          pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
  G4ThreeVector   endPointinLocal = transformation.TransformPoint  (endPoint);
  G4ThreeVector          pinLocal =(startPointinLocal+endPointinLocal)*0.5;
  //
//  G4cout<<"LArG4H6WarmTCCalculator::Local point: "<<pinLocal.x()<<" "<<pinLocal.y()<<" "<<pinLocal.z()<<std::endl;

  G4int m_zSide;
  G4int m_sampling;
  G4int m_region;
  G4int m_etaBin; 
  G4int m_phiBin; 
 
   m_zSide = 4;
   if( p.z()<0.) m_zSide =-4; 
   m_region = 0;

  // We can extract our position from the copy number of depth and module
  // First have touchable
  // G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
  // Volume name 
  G4String hitVolume = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  // And copy number
  G4int copyModule = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

  G4int gran;
// Sampling Identifier
  if(m_isABS) { m_sampling = copyModule; gran = 1; }
  else {
     switch(copyModule) {
        case 1:  { 
		 gran = 1;
		 if(m_isX)  m_sampling = copyModule; else m_sampling = copyModule + 1;
		 break; 
	         }
        case 2: case 3: { 
	   	    gran = 2; 
	  	    if(m_isX) m_sampling = 2*copyModule; else m_sampling = 2*copyModule - 1; 
		    break; 
		        }
        default: { m_sampling = -1; gran = 0; break; }
     }
  }
  
//#include "LArG4TBEndcap/LArTBEndcapWTC.icc"
  // This should go int Db:
  double WTC_sci_x = 190.0*CLHEP::mm;
  double WTC_sci_y = 1160.0*CLHEP::mm;
  double x_x = 6 * WTC_sci_x / 2;
 double x_y = WTC_sci_y / 2;
 
  if(m_isX) {
   m_etaBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
   m_phiBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  } else {
   m_phiBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
   m_etaBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  }
  m_region = 0;



  
 /* 
  if(!m_isABS) {
     std::cout<<"LArG4H6WarmTCCalculator: "<<hitVolume<<" "<<copyModule<<" :  "<<pinLocal.x()<<" "<<pinLocal.y()<<std::endl;
     std::cout <<"m_zSide = "<<m_zSide<<" , m_sampling = "<<m_sampling<<"  ,  m_region="<<m_region <<
            " , m_phiBin="<<m_phiBin<< " ,  m_etaBin="<<m_etaBin <<std::endl;
     std::cout<<m_energy<<" "<<m_time<<std::endl;
     std::cout<<m_isInTime<<" "<<m_isX<<" "<<m_isABS<<"   "<<this<<std::endl;
  }
  */
  
  m_identifier.clear();
  m_identifier << 10          // LArCalorimeter
	       << m_zSide          
	       << 1
	       << m_sampling
	       << m_region
	       << m_etaBin
	       << m_phiBin;	       

  m_addr =  100*m_sampling+10*m_etaBin+m_phiBin;
  if(m_isABS) m_addr *= -1;
  return true;
}

G4bool LArG4H6WarmTCCalculator::Process(const G4Step*, std::vector<LArHitData>&)
{
  std::cout << "Not implemented!" << std::endl;
  throw;
}


