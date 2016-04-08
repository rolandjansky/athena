/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Prepared 05-Dec-2002 Bill Seligman

// A first pass at determing hit cell ID in the LAr barrel presampler.

// Modified oct-2005 by Guillaume Unal to include current map for presampler

#include "LArG4Barrel/LArBarrelPresamplerCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArG4BirksLaw.h"
#include "LArG4Barrel/LArBarrelPresamplerGeometry.h"
#include "LArG4RunControl/LArG4BarrelOptions.h"
#include "LArG4Barrel/PsMap.h"

#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"

#include "G4ios.hh"
#include "globals.hh"

#include <cmath>
#include <limits.h>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/Units.h"

namespace Units = Athena::Units;

// #define DEBUGSTEP

// Standard implementation of a singleton pattern.

LArBarrelPresamplerCalculator* LArBarrelPresamplerCalculator::m_instance = 0;

//=============================================================================
LArBarrelPresamplerCalculator* LArBarrelPresamplerCalculator::GetCalculator()
{
  if (m_instance == 0) 
    {
      m_instance = new LArBarrelPresamplerCalculator();
    }
  return m_instance;
}

//=============================================================================
LArBarrelPresamplerCalculator::LArBarrelPresamplerCalculator()
  : m_IflCur(true)
  , m_birksLaw(NULL)
  , m_nhits(0)
  , m_detectorName("LArMgr")
  , m_testbeam(false)
{

  std::cout << "LArBarrelPresamplerCalculator: Beginning initialization " << std::endl;
  // Initialize private members.
  m_identifier2 = LArG4Identifier();

  // Access source of detector parameters.
  m_parameters = LArVG4DetectorParameters::GetInstance();

  //Inizialize the geometry calculator
  m_geometry = LArG4::BarrelPresampler::Geometry::GetInstance();

  if (m_IflCur) 
     std::cout << " LArBarrelPresamplerCalculator: start reading current maps" << std::endl;

  if (m_IflCur) m_psmap = PsMap::GetPsMap();
  else m_psmap=0;

  // Get the out-of-time cut from the detector parameters routine.
  m_OOTcut = m_parameters->GetValue("LArExpHallOutOfTimeCut");

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StoreGateSvc *detStore;
  LArG4BarrelOptions *barrelOptions;

  StatusCode status = svcLocator->service("DetectorStore", detStore);

  if(status.isFailure())
    std::cout << "LArBarrelCalculator::LArBarrelCalculator() unable to get Detector Store! Using default values.\n";
  else {  
    status = detStore->retrieve(barrelOptions, "LArG4BarrelOptions");

    if(status.isFailure())
      std::cout << "LArBarrelCalculator::LArBarrelCalculator() unable to get LArG4BarrelOptions! Using default values.\n";
    else { 

      bool IflBirks = barrelOptions->EMBBirksLaw();
      if (IflBirks) {
        const double Birks_LAr_density = 1.396;
        const double Birks_k = barrelOptions->EMBBirksk();
        m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,Birks_k);
      }
    }
  }

   if(m_birksLaw) {
     std::cout << " LArBarrelPresamplerCalculator: Birks' law ON " << std::endl;
     std::cout << " LArBarrelPresamplerCalculator:   parameter k    " << m_birksLaw->k() << std::endl;
   }
   else
     std::cout << " LArBarrelPresamplerCalculator: Birks' law OFF" << std::endl;



  std::cout <<"End of LArBarrelPresamplerCalculator initialization " << std::endl;

}

//==============================================================================
LArBarrelPresamplerCalculator::~LArBarrelPresamplerCalculator()
{
  if (m_birksLaw)   delete m_birksLaw;
}


// ==============================================================================
G4bool LArBarrelPresamplerCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata)
{
  m_nhits = 0;
  hdata.clear();
  m_isInTime.clear();

//  check the Step content is non trivial
  G4double thisStepEnergyDeposit = a_step->GetTotalEnergyDeposit();
  G4double thisStepLength = a_step->GetStepLength() / Units::mm;
  G4double dstep = .1*Units::mm;   // length of punctual charge for Current Option

#ifdef  DEBUGSTEP
  std::cout << "******  LArBarrelPresamplerCalculator:  Step energy,length "
            << thisStepEnergyDeposit << " " << thisStepLength << std::endl;
#endif
  if(thisStepEnergyDeposit <= 0. || thisStepLength <= 0.)
  {
    return false;
  }

// Get Step geometrical parameters (first and end) 
  G4StepPoint *thisStepPoint = a_step->GetPreStepPoint();
  G4StepPoint *thisStepBackPoint = a_step->GetPostStepPoint();
  G4ThreeVector startPoint = thisStepPoint->GetPosition();
  G4ThreeVector endPoint = thisStepBackPoint->GetPosition();

#ifdef  DEBUGSTEP
  std::cout << "   Global beginning step position "
            << startPoint.x() << " " << startPoint.y() << " "
            << startPoint.z() << std::endl;
  std::cout << "   Global end       step position "
            << endPoint.x() << " " << endPoint.y() << " "
            << endPoint.z() << std::endl;
#endif


// find transformation to go inside local half presampler tube volume

  const G4NavigationHistory* g4navigation = thisStepPoint->GetTouchable()->GetHistory();
  G4int ndep = g4navigation->GetDepth();
  G4bool testbeam=false;
  G4int idep = -999;

#ifdef DEBUGSTEP
  std::cout << " Detector Name " << m_detectorName << std::endl;
#endif

  if(m_detectorName=="")
    for (G4int ii=0;ii<=ndep;ii++) {
      G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
      if (v1->GetName()=="LAr::Barrel::Presampler") idep=ii;    // half barrel
      if (v1->GetName()=="LAr::TBBarrel::Cryostat::LAr") testbeam=true;  // TB or not ?
    }
  else
    for (G4int ii=0;ii<=ndep;ii++) {
      G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
#ifdef DEBUGSTEP
      std::cout << " Level,VolumeName " << ii << " " << v1->GetName() << std::endl;
#endif
      if (v1->GetName()==G4String(m_detectorName+"::LAr::Barrel::Presampler")) idep=ii;   
      if (v1->GetName()==G4String(m_detectorName+"::LAr::TBBarrel::Cryostat::LAr")) testbeam=true;  // TB or not ?
    }

#ifdef DEBUGSTEP
  std::cout << " idep = " << idep << std::endl;
#endif

  if (idep<0) {
     std::cout << " LArBarrelPresamplerCalculator::Process  Presampler volume not found !!" << std::endl;
     return false;
  }

// transformation to go from global frame to LAr::Barrel::Presampler frame
  const G4AffineTransform transformation = g4navigation->GetTransform(idep);

// step beginning and end in local frame
  G4ThreeVector startPointinLocal =
         transformation.TransformPoint(startPoint);
  G4ThreeVector   endPointinLocal =
         transformation.TransformPoint  (endPoint);

#ifdef  DEBUGSTEP
  std::cout << "   Local beginning step position "
            << startPointinLocal.x() << " " << startPointinLocal.y() << " "
            << startPointinLocal.z() << std::endl;
  std::cout << "   Local end       step position "
            << endPointinLocal.x() << " " << endPointinLocal.y() << " "
            << endPointinLocal.z() << std::endl;
#endif

// compute number of sub steps
//   = 1 if no charge collection
//   otherwise 200 microns (dstep) division

  G4int nsub_step=1;
  if (m_IflCur) nsub_step=(int) (thisStepLength*(1./dstep)) + 1;
// delta is fraction of step between two sub steps
  G4double delta=1./((double) nsub_step);
#ifdef DEBUGSTEP
  std::cout << "   nsub_step,delta " << nsub_step << " " << delta << std::endl;
#endif

  G4double energy = a_step->GetTotalEnergyDeposit();
  if (m_birksLaw) {
     const double EField = 10. ; // 10 kV/cm electric field in presampler gap
     const double wholeStepLengthCm = a_step->GetStepLength() * (1./CLHEP::cm);
     energy = (*m_birksLaw)(energy, wholeStepLengthCm, EField);
  }

// loop over sub steps
  
  
// share total step energy evenly in each sub step
  energy = energy / ((float) (nsub_step));
  for (G4int i=0;i<nsub_step;i++) {

#ifdef DEBUGSTEP 
    std::cout << "   Energy for sub step " << energy << std::endl;
#endif

// position for this substep
    G4double fraction=(((G4double) i)+0.5)*delta;
    G4ThreeVector subinLocal=startPointinLocal*(1-fraction) + endPointinLocal*fraction;
    G4double xloc   = subinLocal.x();
    G4double yloc   = subinLocal.y();
    G4double zloc   = subinLocal.z();
// call geometry method to find cell from local position
//  status = true if hit is in  normal region (13mm LAr gap)
//  this method fills the cell number as well as coordinates in the electrode frame
    bool status = m_geometry->findCell(xloc,yloc,zloc);

// check fiducical cuts
    if (status) {

// compute cell identifier
      G4int zSide;
      if (testbeam)
       zSide = 1;
      else
       zSide = ( startPoint.z() > 0.) ? 1 : -1;

      G4int region = m_geometry->region();
      G4int etaBin = m_geometry->etaBin();
      G4int phiBin = m_geometry->phiBin();
      G4int sampling = m_geometry->sampling();

      if( zSide == -1 )
      {
// following code for an Y-axis rotation to define Z < 0. Barrel part
         phiBin = 31 - phiBin;
         if(phiBin < 0 ) phiBin += 64;
      }

// check identifier
      if (sampling !=0 || region != 0 || 
          etaBin <0 || etaBin > 60 || phiBin <0 || phiBin>63) continue;

// fill identifier
      m_identifier2.clear();
      m_identifier2 << 4          // LArCalorimeter
                    << 1          // LArEM
                    << zSide
                    << sampling
                    << region
                    << etaBin
                    << phiBin;

// time computation is not necessarily correct for test beam
      G4double time;
      if (testbeam)
      {
        time=0.;
      }
      else
      {
        G4double tof;
        tof = thisStepPoint->GetGlobalTime() / Units::ns;
        time  = tof - thisStepPoint->GetPosition().mag() * (1. / (CLHEP::c_light * CLHEP::ns));
      }

      G4double Current;
      if (!m_IflCur)  {
// no charge collection   Current=E from Geant
         Current=energy;
      }
      else  {
// get module number and coordinates in electrode frame
        G4int imodule = m_geometry->module();
        G4double x0 = m_geometry->distElec();
        G4double y0 = m_geometry->xElec();
// full symmetry for angle=0 electrodes
        if (imodule>1) {
          x0=std::fabs(x0);
          y0=std::fabs(y0);
        }
// reduced symmetry (point symmetry around 0) for tilted electrodes
        if (imodule==0 || imodule ==1) {
          if (x0<0) {
             x0=-1.*x0;
             y0=-1.*y0;
          } 
        }
#ifdef DEBUGSTEP
        std::cout << " set current map for module " << imodule << std::endl;
#endif
        m_psmap->SetMap(imodule);
        if (!(m_psmap->Map())) {
         std::cout << " LArBarrelPresamplerCalculator: cannot get map for module " << imodule << std::endl;
         continue;
        }
        double current0,current1,current2,gap;

// get information from current map
        m_psmap->Map()->GetAll(x0,y0,&gap,&current0,&current1,&current2);
#ifdef DEBUGSTEP
        std::cout << " module,x0,y0,current0 from map " << imodule << " " << x0 << " " << y0 << " " << current0 << std::endl;
#endif

// assume HV=2000 everywhere for the time being
        Current = energy*current0;
 
      }

// check if we have a new hit in a different cell, or if we add this substep
//  to an already existing hit
      G4bool found=false;
      for (int j=0; j<m_nhits; j++) {
        if (hdata[j].id==m_identifier2) {
           hdata[j].energy += Current;
           hdata[j].time += time*Current;
           found=true;
           break;
        }
      }    // loop over hits
      if (!found) {
        m_nhits++;
        LArHitData newdata = {m_identifier2, time*Current, Current};
        hdata.push_back(newdata);
        m_isInTime.push_back(true);
      }    // hit was not existing before


    }   // hit fulfills fiducial cuts
  }   // end loop over sub steps

#ifdef DEBUGSTEP
    std::cout << "Number of hits for this step " << m_nhits << " "
              << hdata.size() << std::endl;
#endif

// finalise time computations
  for (int i=0;i<m_nhits;i++) {
     if (std::fabs(hdata[i].energy)>1e-6) hdata[i].time=hdata[i].time/hdata[i].energy;
     else hdata[i].time=0.;
     if (std::fabs(hdata[i].time)> m_OOTcut) m_isInTime[i]=false;
#ifdef DEBUGSTEP
     std::cout << "Hit Energy/Time "
               << hdata[i].energy << " " << hdata[i].time << std::endl;
#endif
  }

  if (m_nhits>0) return true;
  else           return false;

}
