/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Prepared 05-Dec-2002 Bill Seligman

// A first pass at determing hit cell ID in the LAr barrel presampler.

// Modified oct-2005 by Guillaume Unal to include current map for presampler

#include "LArBarrelPresamplerCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArG4BirksLaw.h"
#include "LArBarrelPresamplerGeometry.h"
#include "PsMap.h"

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

//=============================================================================
LArBarrelPresamplerCalculator::LArBarrelPresamplerCalculator(const std::string& name, ISvcLocator *pSvcLocator)
 : LArCalculatorSvcImp(name, pSvcLocator)
 , m_geometry("LArBarrelPresamplerGeometry", name)
 , m_psmap(nullptr)
 , m_IflCur(true)
 , m_birksLaw(nullptr)
 , m_detectorName("LArMgr")
 , m_testbeam(false)
 , m_volname("LArMgr::LAr::Barrel::Presampler")
{
  declareProperty("GeometryCalculator", m_geometry);
  declareProperty("IflCur",m_IflCur);
  declareProperty("DetectorName",m_detectorName);
  declareProperty("isTestbeam",m_testbeam);
}

StatusCode LArBarrelPresamplerCalculator::initialize()
{
  ATH_MSG_DEBUG("LArBarrelPresamplerCalculator: Beginning initialization ");
  // Initialize private members.

  // Access source of detector parameters.
  LArVG4DetectorParameters* parameters = LArVG4DetectorParameters::GetInstance();

  // Initialize the geometry calculator
  ATH_CHECK(m_geometry.retrieve());

  if (m_IflCur)
    {
      ATH_MSG_INFO(" LArBarrelPresamplerCalculator: start reading current maps");
      m_psmap = PsMap::GetPsMap();
    }

  // Get the out-of-time cut from the detector parameters routine.
  m_OOTcut = parameters->GetValue("LArExpHallOutOfTimeCut"); //FIXME should be done by configurables

  if (m_BirksLaw)
    {
      const double Birks_LAr_density = 1.396;
      m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
      ATH_MSG_INFO(" LArBarrelPresamplerCalculator: Birks' law ON ");
      ATH_MSG_INFO(" LArBarrelPresamplerCalculator:   parameter k    " << m_birksLaw->k());
    }
  else
    {
      ATH_MSG_INFO(" LArBarrelPresamplerCalculator: Birks' law OFF");
    }

  if(m_detectorName.empty()) m_volname="LAr::Barrel::Presampler";
  else m_volname=m_detectorName+"::LAr::Barrel::Presampler";

  ATH_MSG_DEBUG("End of LArBarrelPresamplerCalculator initialization ");

  return StatusCode::SUCCESS;
}

//==============================================================================
StatusCode LArBarrelPresamplerCalculator::finalize()
{
  if (m_BirksLaw)   delete m_birksLaw;
  return StatusCode::SUCCESS;
}

// ==============================================================================
G4bool LArBarrelPresamplerCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const
{
  hdata.clear();
  LArG4Identifier identifier2;

  //  check the Step content is non trivial
  G4double thisStepEnergyDeposit = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();
  G4double thisStepLength = a_step->GetStepLength() / Units::mm;
  G4double dstep = .1*Units::mm;   // length of punctual charge for Current Option

#ifdef  DEBUGSTEP
  ATH_MSG_DEBUG( "******  LArBarrelPresamplerCalculator:  Step energy,length "
            << thisStepEnergyDeposit << " " << thisStepLength);
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
  ATH_MSG_DEBUG("   Global beginning step position "
            << startPoint.x() << " " << startPoint.y() << " "
            << startPoint.z());
  ATH_MSG_DEBUG("   Global end       step position "
            << endPoint.x() << " " << endPoint.y() << " "
            << endPoint.z());
#endif


  // find transformation to go inside local half presampler tube volume

  const G4NavigationHistory* g4navigation = thisStepPoint->GetTouchable()->GetHistory();
  G4int ndep = g4navigation->GetDepth();
  G4int idep = -999;

#ifdef DEBUGSTEP
  ATH_MSG_DEBUG(" Detector Name " << m_detectorName);
#endif

    const G4String presamplerName((m_detectorName.empty()) ? "LAr::Barrel::Presampler" :
                                  m_detectorName+"::LAr::Barrel::Presampler");
    for (G4int ii=0;ii<=ndep;ii++) {
      G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
#ifdef DEBUGSTEP
      ATH_MSG_DEBUG(" Level,VolumeName " << ii << " " << v1->GetName());
#endif
      // FIXME More efficient to find the comparison volume once and compare pointers?
      if (v1->GetName()==presamplerName) idep=ii;
    }

#ifdef DEBUGSTEP
  ATH_MSG_DEBUG(" idep = " << idep);
#endif

  if (idep<0) {
     ATH_MSG_INFO(" LArBarrelPresamplerCalculator::Process  Presampler volume not found !!");
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
  ATH_MSG_DEBUG("   Local beginning step position "
            << startPointinLocal.x() << " " << startPointinLocal.y() << " "
            << startPointinLocal.z());
  ATH_MSG_DEBUG("   Local end       step position "
            << endPointinLocal.x() << " " << endPointinLocal.y() << " "
            << endPointinLocal.z());
#endif

  // compute number of sub steps
  //   = 1 if no charge collection
  //   otherwise 200 microns (dstep) division

  G4int nsub_step=1;
  if (m_IflCur) nsub_step=(int) (thisStepLength*(1./dstep)) + 1;
  // delta is fraction of step between two sub steps
  G4double delta=1./((double) nsub_step);
#ifdef DEBUGSTEP
  ATH_MSG_DEBUG("   nsub_step,delta " << nsub_step << " " << delta);
#endif

  G4double energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();
  if (m_BirksLaw) {
     const double EField = 10. ; // 10 kV/cm electric field in presampler gap
     const double wholeStepLengthCm = a_step->GetStepLength() / CLHEP::cm;
     energy = (*m_birksLaw)(energy, wholeStepLengthCm, EField);
  }

  // loop over sub steps


  // share total step energy evenly in each sub step
  energy = energy / ((float) (nsub_step));
  for (G4int i=0;i<nsub_step;i++) {

#ifdef DEBUGSTEP
    ATH_MSG_DEBUG("   Energy for sub step " << energy);
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
    LArG4::BarrelPresampler::CalcData currentCellData;
    bool status = m_geometry->findCell(currentCellData,xloc,yloc,zloc);

    // check fiducical cuts
    if (status) {

      // compute cell identifier
      G4int zSide;
      if (m_testbeam)
        zSide = 1;
      else
        zSide = ( startPoint.z() > 0.) ? 1 : -1;

      G4int region = currentCellData.region;
      G4int etaBin = currentCellData.etaBin;
      G4int phiBin = currentCellData.phiBin;
      G4int sampling = currentCellData.sampling;

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
      identifier2.clear();
      identifier2 << 4          // LArCalorimeter
                  << 1          // LArEM
                  << zSide
                  << sampling
                  << region
                  << etaBin
                  << phiBin;

      // time computation is not necessarily correct for test beam
      G4double time;
      if (m_testbeam)
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
        G4int imodule = currentCellData.module;
        G4double x0 = currentCellData.distElec;
        G4double y0 = currentCellData.xElec;
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
        ATH_MSG_DEBUG(" set current map for module " << imodule);
#endif
	CurrMap* cm = m_psmap->GetMap(imodule);
        if (!cm) {
          ATH_MSG_INFO(" LArBarrelPresamplerCalculator: cannot get map for module " << imodule);
          continue;
        }
        double current0,current1,current2,gap;

        // get information from current map
        cm->GetAll(x0,y0,&gap,&current0,&current1,&current2);
#ifdef DEBUGSTEP
        ATH_MSG_DEBUG(" module,x0,y0,current0 from map " << imodule << " " << x0 << " " << y0 << " " << current0);
#endif

        // assume HV=2000 everywhere for the time being
        Current = energy*current0;

      }

      // check if we have a new hit in a different cell, or if we add this substep
      //  to an already existing hit
      G4bool found=false;
      for (unsigned int j=0; j<hdata.size(); j++) {
        if (hdata[j].id==identifier2) {
          hdata[j].energy += Current;
          hdata[j].time += time*Current;
          found=true;
          break;
        }
      }    // loop over hits
      if (!found) {
        LArHitData newdata = {identifier2, time*Current, Current};
        hdata.push_back(newdata);
      }    // hit was not existing before


    }   // hit fulfills fiducial cuts
  }   // end loop over sub steps

#ifdef DEBUGSTEP
    ATH_MSG_DEBUG("Number of hits for this step " << m_nhits << " " << hdata.size());
#endif

  // finalise time computations
  for (unsigned int i=0;i<hdata.size();i++) {
    if (std::fabs(hdata[i].energy)>1e-6) hdata[i].time=hdata[i].time/hdata[i].energy;
    else hdata[i].time=0.;
#ifdef DEBUGSTEP
     ATH_MSG_DEBUG("Hit Energy/Time " << hdata[i].energy << " " << hdata[i].time);
#endif
  }

  if (hdata.size()>0) return true;
  else           return false;

}
