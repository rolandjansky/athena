/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// The Cell Identifier for the EM Barrel readout cells

// Adapted from code written by Gaston Parrour
// Adaptation by Sylvain Negroni
// 17-11-2003: G.Unal  cleanup
// 18-03-2003: G.Unal  major revision to use new current maps

// #define DEBUGSTEP
// #define DEBUGSTEP2

#include "LArBarrelCalculator.h"
#include "AccMap.h"
#include "MapEta.h"

#include "LArG4Code/LArG4BirksLaw.h"
#include "LArG4Code/LArVG4DetectorParameters.h"

//#include "LArG4RunControl/LArG4BarrelOptions.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"

#include <cmath>
#include <iostream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/Units.h"
#include "CxxUtils/make_unique.h"

#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVModuleConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVElectrodeConstLink.h"

namespace Units = Athena::Units;

// ================================================================================
LArBarrelCalculator::LArBarrelCalculator(const std::string& name, ISvcLocator* pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
  , m_geometry("LArBarrelGeometry", name)
  , m_accmap(nullptr)
  , m_IflCur(true)
  , m_IflMapTrans(true)
  , m_IflXtalk(true)
  , m_dstep(.2*CLHEP::mm)
  , m_birksLaw(nullptr)
  , m_doHV(false)
  , m_etaMaxBarrel(0)
  , m_zMinBarrel(0)
  , m_zMaxBarrel(0)
  , m_rMinAccordion(0)
  , m_rMaxAccordion(0)
  , m_ThickAbs(0)
  , m_ThickEle(0)
  , m_NCellTot(0)
  , m_NCellMax(0)
  , m_testbeam(false)
{
  ATH_MSG_DEBUG("LArBarrelCalculator: Beginning construction ");

  declareProperty("GeometryCalculator", m_geometry);
  // define RUN conditions for the Barrel
  declareProperty("EMBCurr",m_IflCur);
  declareProperty("EMBEtaTrans",m_IflMapTrans);
  declareProperty("EMBXtalk",m_IflXtalk);
  declareProperty("EMBdstep", m_dstep);
  declareProperty("EMBHVEnable",m_doHV);
}

StatusCode LArBarrelCalculator::initialize()
{
  ATH_MSG_DEBUG("LArBarrelCalculator: Beginning initialization ");
  if (m_BirksLaw) {
    const double Birks_LAr_density = 1.396;
    m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
  }

  // Access source of detector parameters.
  LArVG4DetectorParameters* parameters = LArVG4DetectorParameters::GetInstance();

  // Get the out-of-time cut from the detector parameters routine.
  m_OOTcut = parameters->GetValue("LArExpHallOutOfTimeCut"); //FIXME should be done via configurables
  ATH_MSG_DEBUG("**** OutOfTime cut " << m_OOTcut);

  // Main Barrel parameters
  // All the UNITS are implicitly the GEANT4 ONES e.g. mm, rad, etc ...
  m_etaMaxBarrel = parameters->GetValue("LArEMBMaxEtaAcceptance");
  m_zMinBarrel = parameters->GetValue("LArEMBfiducialMothZmin");
  m_zMaxBarrel = parameters->GetValue("LArEMBfiducialMothZmax");
  m_NCellMax   = (int) (parameters->GetValue("LArEMBnoOFPhysPhiCell"));
  m_rMinAccordion  =  parameters->GetValue("LArEMBRadiusInnerAccordion");
  m_rMaxAccordion  = parameters->GetValue("LArEMBFiducialRmax");

  // absorbers and electrodes thickness
  m_ThickAbs = 0.5*( parameters->GetValue("LArEMBThickAbsGlue")
                     +parameters->GetValue("LArEMBThickAbsIron")
                     +parameters->GetValue("LArEMBThickAbsLead"));

  G4double check = 0.5*( parameters->GetValue("LArEMBThinAbsGlue")
                         +parameters->GetValue("LArEMBThinAbsIron")
                         +parameters->GetValue("LArEMBThinAbsLead"));

  if (fabs(check-m_ThickAbs)>0.001)
    {
      ATH_MSG_WARNING("Thin and Thick Abs have difference thickness!");
    }

  m_ThickEle= 0.5*( parameters->GetValue("LArEMBThickElecCopper")
                    +parameters->GetValue("LArEMBThickElecKapton"));

  // === GU 11/06/2003   total number of cells in phi
  // to distinguish 1 module (testbeam case) from full Atlas
  m_NCellTot = (int) (parameters->GetValue("LArEMBnoOFPhysPhiCell"));
  if (m_NCellTot != 1024)
    {
      m_NCellMax=1024;
      m_testbeam=true;
    }
  // ===
  //  access geometry computation class
  ATH_CHECK(m_geometry.retrieve());

  // access current maps if required
  if (m_IflCur)
    {
      ATH_MSG_DEBUG(" LArBarrelCalculator: start reading of current maps");
      m_accmap = AccMap::GetAccMap();
      m_etamap1 = CxxUtils::make_unique<MapEta>(1);
      m_etamap2 = CxxUtils::make_unique<MapEta>(2);
      if (m_IflMapTrans) m_etamap3 = CxxUtils::make_unique<MapEta>(3);
      ATH_MSG_DEBUG(" LArBarrelCalculator: end of reading current maps");
    }
  // Initialize HV values
  this->InitHV();

  ATH_MSG_DEBUG(" LArBarrelCalculator: s_NCellMax       " << m_NCellMax);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_NCellTot       " << m_NCellTot);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_rMinAccordion  " << m_rMinAccordion);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_rMaxAccordion  " << m_rMaxAccordion);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_zMinBarrel     " << m_zMinBarrel);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_zMaxBarrel     " << m_zMaxBarrel);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_etaMaxBarrel   " << m_etaMaxBarrel);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_ThickAbs       " << m_ThickAbs);
  ATH_MSG_DEBUG(" LArBarrelCalculator: s_ThickEle       " << m_ThickEle);
  if(m_IflCur) ATH_MSG_DEBUG(" LArBarrelCalculator: Deposited Energy  dE/dX  Corrected ==> CURRENT Option ON");
  else ATH_MSG_DEBUG(" LArBarrelCalculator: Crude Deposited Energy  dE/dX  NO CURRENT option");
  if (m_IflCur && m_IflMapTrans) ATH_MSG_DEBUG(" LArBarrelCalculator: Compute effect of E field around eta=0.8 ");
  else ATH_MSG_DEBUG(" LArBarrelCalculator: Ignore effect of E field around eta=0.8 ");
  if(m_BirksLaw)
    {
      ATH_MSG_DEBUG(" LArBarrelCalculator: Birks' law ON ");
      ATH_MSG_DEBUG(" LArBarrelCalculator:   parameter k    " << m_birksLaw->k());
    }
  else
    {
      ATH_MSG_DEBUG(" LArBarrelCalculator: Birks' law OFF");
    }

  return StatusCode::SUCCESS;
}

// ============================================================================
StatusCode LArBarrelCalculator::finalize()
{
  if (m_BirksLaw) delete m_birksLaw;
  return StatusCode::SUCCESS;
}

// =============================================================================
G4bool LArBarrelCalculator::Process(const G4Step* step, std::vector<LArHitData>& hdata) const
{

  hdata.clear();

  LArG4Identifier identifier2;
  LArG4Identifier identifier_xt1;
  LArG4Identifier identifier_xt2;

  //  check the Step content is non trivial
  G4double thisStepEnergyDeposit = step->GetTotalEnergyDeposit() * step->GetTrack()->GetWeight();
  G4double thisStepLength = step->GetStepLength() / Units::mm;

#ifdef  DEBUGSTEP
  ATH_MSG_DEBUG("******  LArBarrelCalculator:  Step energy,length "
                << thisStepEnergyDeposit << " " << thisStepLength);
#endif

  if(thisStepEnergyDeposit <= 0. || thisStepLength <= 0.)
    {
#ifdef DEBUGSTEP2
      ATH_MSG_DEBUG("   Invalid hit  trivial content");
#endif
      return false;
    }

  // Get Step geometrical parameters (first and end)
  G4StepPoint *thisStepPoint = step->GetPreStepPoint();
  G4StepPoint *thisStepBackPoint = step->GetPostStepPoint();
  G4ThreeVector startPoint = thisStepPoint->GetPosition();
  G4ThreeVector endPoint = thisStepBackPoint->GetPosition();

#ifdef  DEBUGSTEP
  ATH_MSG_DEBUG("   Beginning step position "
                << startPoint.x() << " " << startPoint.y() << " " << startPoint.z());
#endif

  // find zside from volume name
  G4int zSide = 1;
  if (!m_testbeam) {
    const G4NavigationHistory* g4navigation = thisStepPoint->GetTouchable()->GetHistory();
    G4int ndep = g4navigation->GetDepth();
    for (G4int ii=0;ii<=ndep;ii++) {
      G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
      G4String vname = v1->GetName();
      if ( vname.find("NegPhysical") != std::string::npos) zSide=-1;
    }
  }


  // BACK directly into the LOCAL half_Barrel Z > 0. PART (mother "stac_phys1")

  const G4AffineTransform transformation =
    thisStepPoint->GetTouchable()->GetHistory()->GetTopTransform();

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

  G4double energy = step->GetTotalEnergyDeposit() * step->GetTrack()->GetWeight(); // Despite the name, this is only ionization.

  if (m_BirksLaw) {
    const double EField = 10.;         // kV/cm, assume constant for now
    const double wholeStepLengthCm = step->GetStepLength() / CLHEP::cm;
    energy = (*m_birksLaw)(energy, wholeStepLengthCm, EField);
  }


  // compute number of sub steps
  //   = 1 if no charge collection
  //   otherwise 200 microns (dstep) division

  G4int nsub_step=1;
  if (m_IflCur) nsub_step=(int) (thisStepLength/m_dstep) + 1;
  // delta is fraction of step between two sub steps
  G4double delta=1./((double) nsub_step);
#ifdef DEBUGSTEP
  ATH_MSG_DEBUG("   nsub_step,delta " << nsub_step << " " << delta);
#endif

  energy /= ((float) (nsub_step)); // Divide into substeps.

  // loop over sub steps

  for (G4int i=0;i<nsub_step;i++) {

    bool Xtalk = false;

    G4double fraction=(((G4double) i)+0.5)*delta;
    G4ThreeVector subinLocal=startPointinLocal*(1-fraction) + endPointinLocal*fraction;
    G4double xloc   = subinLocal.x();
    G4double yloc   = subinLocal.y();
    G4double zloc   = subinLocal.z();
    G4double etaloc = subinLocal.pseudoRapidity();
    G4double philoc = subinLocal.phi();
    if(philoc<0.) philoc = philoc + 2.*M_PI;
    G4double radloc = sqrt( xloc*xloc + yloc*yloc );
#ifdef DEBUGSTEP
    ATH_MSG_DEBUG("   local position sub_step "
                  << xloc << " " << yloc << " " << zloc);
#endif

    //  apply fiducial cuts
    if (!FiducialCuts(radloc,zloc,etaloc)) {
#ifdef DEBUGSTEP
      ATH_MSG_DEBUG("LArBarrelCalculator: outside fiducial cuts");
#endif
      continue;
    }
    LArG4::Barrel::CalcData currentCellData;
    m_geometry->findCell(currentCellData,xloc,yloc,zloc,radloc,etaloc,philoc,m_IflCur);

    if (currentCellData.cellID == 0)
      {
#ifdef DEBUGSTEP
        ATH_MSG_DEBUG("LArBarrelCalculator: Invalid hit CELLID == 0 ");
        ATH_MSG_DEBUG("x,y,z local " << xloc << " " << yloc << " " << zloc);
        ATH_MSG_DEBUG("radius " << radloc <<" etaloc "<< etaloc << " philoc " << philoc);
#endif
        continue;
      }
    G4int region = currentCellData.region;
    G4int etaBin = currentCellData.etaBin;
    G4int phiBin = currentCellData.phiBin;
    G4int sampling = currentCellData.sampling;

    if( zSide == -1 )
      {
        // following code for an Y-axis rotation to define Z < 0. Barrel part
        if( sampling == 1 && region ==0 )
          {
            phiBin = 31 - phiBin;
            if(phiBin < 0 ) phiBin += 64;
          }
        if( sampling == 1 && region ==1 )
          {
            phiBin = 127 - phiBin;
            if(phiBin < 0 ) phiBin += 256;
          }
        if( sampling >= 2 )
          {
            phiBin = 127 - phiBin;
            if(phiBin < 0 ) phiBin += 256;
          }
      }
#ifdef DEBUGSTEP
    ATH_MSG_DEBUG("   region,side,sampling,eta,phi " << region << " " << zSide << " "
                  << sampling << " " << etaBin << " " << phiBin);
    ATH_MSG_DEBUG("   distance to electrode,abs " << currentCellData.distElec << " "
                  << currentCellData.distAbs);
    ATH_MSG_DEBUG("    local coordinates " << currentCellData.x0 << " " << currentCellData.y0);
#endif

    if (std::fabs(currentCellData.distElec)< m_ThickEle ||
        std::fabs(currentCellData.distAbs) < m_ThickAbs) {
#ifdef DEBUGSTEP
      ATH_MSG_DEBUG("   hit in absorber or electrode radius:" << radloc);
#endif
      continue;
    }

    identifier2.clear();
    identifier2 << 4          // LArCalorimeter
                << 1          // LArEM
                << zSide
                << sampling
                << region
                << etaBin
                << phiBin;

    // time computation is not necessarily correct for test beam
    //    G4double time;
    //    if (m_testbeam)
    //      {
    //        time=0.;
    //      }
    //    else
    //      {
    //        const G4double tof = thisStepPoint->GetGlobalTime() / Units::ns;
    //        time  = tof - ( thisStepPoint->GetPosition().mag()/CLHEP::c_light ) / Units::ns;
    //      }

    const G4double time = (m_testbeam)? 0.0 :
      ( thisStepPoint->GetGlobalTime() - ( thisStepPoint->GetPosition().mag()/CLHEP::c_light ) ) / Units::ns;

#ifdef DEBUGSTEP
    ATH_MSG_DEBUG("   Energy for sub step " << energy);
#endif

    G4double Current;
    G4double Current_xt1,Current_xt2;
    Current_xt1=0;
    Current_xt2=0;
    if (!m_IflCur)  {
      // no charge collection   Current=E from Geant
      Current=energy;
    }
    else  {
      // full charge collection
      G4double xmap,ymap;
      G4int nfold = currentCellData.nfold;
      G4double x0=currentCellData.x0;
      G4double y0=currentCellData.y0;
      if (x0<1500 || x0>1960 || y0>30 || y0<-30) {
        ATH_MSG_INFO("weird x0,y0 " << x0 << " " << y0);
      }
#ifdef DEBUGSTEP
      G4double rr = sqrt(x0*x0+y0*y0);
      ATH_MSG_DEBUG("   radius,rad0 " << radloc << " " << rr);
#endif
      CurrMap* mapOfCurrent(nullptr);
      bool UseFold=false;
      // Are we close to a fold ?   (fold 0 has some pathology)
      if ((x0 > m_accmap->GetXmin(nfold) || nfold==0) &&
          x0 < m_accmap->GetXmax(nfold)  &&
          y0 > m_accmap->GetYmin(nfold) &&
          y0 < m_accmap->GetYmax(nfold) &&
          (nfold != 13 || x0 < 1957.5) &&
          (nfold>0 || x0 < 1504.6) ) {
        xmap=x0;
        ymap=y0;
#ifdef DEBUGSTEP
        ATH_MSG_DEBUG("   Map for fold xmap,ymap " << nfold << " " << xmap << " " << ymap);
#endif
        UseFold=true;
        G4int sampMap = currentCellData.sampMap;
        G4int etaMap  = currentCellData.etaMap;
        mapOfCurrent = m_accmap->GetMap(nfold,region,sampMap,etaMap);
        // catch problem to find map
        if (!mapOfCurrent) {
          ATH_MSG_WARNING(" Problem to access map fold = " << nfold);
          ATH_MSG_WARNING(" region,sampling,eta,fold " << region << " " << sampMap << " "
                          << etaMap << " " << nfold);
          return false;
        }
      }
      else {
        G4int n;
        G4int nstraight = currentCellData.nstraight;
        if (nstraight%2==0) n=22;
        else                n=21;
        mapOfCurrent = m_accmap->GetMap(n,region,sampling,etaBin);
        // catch problem to find map
        if (!mapOfCurrent) {
          ATH_MSG_WARNING(" Problem to access map straight = " << nstraight);
          return false;
        }
        xmap = currentCellData.xl;
        ymap = currentCellData.distElec;
        // special case for first straight section, which is shorter
        if (nstraight==0) xmap = 0.5*(xmap+1.);
#ifdef DEBUGSTEP
        ATH_MSG_DEBUG("   Map for straight xl,delec " << xmap << " " << ymap);
#endif
      }   // fold or straight
      double gap;
      double current0,current1,current2;
      //  get current for elementary charge
      mapOfCurrent->GetAll(xmap,ymap,&gap,&current0,&current1,&current2);
      G4double gap2=std::fabs(currentCellData.distElec)+std::fabs(currentCellData.distAbs)
        -m_ThickEle-m_ThickAbs;

      // in which HV cell are we ?
      int ipm,ielec,ieta,iside;
      if (zSide==1) ipm=1;    // A side
      else          ipm=0;    // C side
      ielec=currentCellData.phiGap;
      if (zSide==-1) {
        ielec = 511 - ielec;
        if(ielec < 0 ) ielec += 1024;
      }
      ieta=((int) (etaloc*(1./0.2)));
      if (ieta>6) ieta=6;  //part 1.4 to 1.475 is same HV as 1.2 to 1.4
      iside=0;    // phi lower than electrode 0, 1 for phi higher than electrode
      if ((currentCellData.distElec>0 && zSide==1)
          || (currentCellData.distElec<0 && zSide==-1) ) iside=1;

      // HV extrapolation
      double current;
      double hv=m_hv[ipm][ielec][ieta][iside];
      //       std::cout << " etaBin,phiBin " << etaBin << " " << phiBin << std::endl;
      //       std::cout << " ipm,ielec,ieta,iside " << ipm << " " << ielec << " " << ieta
      //            << " " << iside << std::endl;
      //       std::cout << " hv " << hv << std::endl;
      //       std::cout << " current0,current1,current2 " << current0 << " " << current1
      //         << " " << current2 << std::endl;
      if (hv>1995.) current=current0;
      else if (hv>5.) current=ScaleHV(hv,current0,current1,current2);
      else current=0.;
      //       std::cout << " current " << current << std::endl;

      // extrapolation for non nominal gap (allows to include sagging effect)
      //  i ~ (gap/gap2)**1.3
      //  gap = nominal gap from current map
      //  gap2 = real gap from geometry
      //  dgap = gap/gap2 -1
      //   at first order  i ~ (1+1.3*dgap)
      double dgap=0;
      if (gap>1e-3 && gap2 >1e-3) dgap=gap/gap2-1;
      current = current * (1. + 1.3*dgap);

#ifdef DEBUGSTEP
      ATH_MSG_DEBUG("   elementary current " << current0);
      ATH_MSG_DEBUG("   Gap from Map/calculator " << gap << " " << gap2);
#endif

      Current = energy*current;

      // check if pathology...
      if (!UseFold && std::fabs(currentCellData.distElec)>2.1 && current0 < 0.1) {
        ATH_MSG_WARNING(" xl,distEle " << currentCellData.xl << " "
                        << currentCellData.distElec
                        << " str number " << currentCellData.nstraight
                        << " sampling,eta " << sampling << " " << etaBin << " "
                        << " current/E " << current0);
      }

      // compute effect around transition at eta=0.8 from electric field
      // read from map weighting factor ( E**1.3 = E*E**0.3)
      // in x  +-9mm around transition (assume symmetry around 0)
      // in y  distance from electrode in gap
      bool InTrans=false;
      //double Current_test=Current;
      if (m_IflMapTrans) {
        float etaTrans=0.8;
        if (fabs(etaloc-etaTrans) < 0.025) {
          double x=std::fabs(zloc-radloc*sinh(etaTrans))/cosh(etaTrans);
          double y=std::fabs(currentCellData.distElec);
          if ( x < m_etamap3->Xmax() ) {
            double resp;
            m_etamap3->GetData0(x,y,&resp);
            Current = Current*resp;
            InTrans=true;
          }
        }     // eta = 0.8 +- 0.025
      }       // if m_IflMapTrans

      // simulate cross-talk effects and transitions in eta between cells
      // (only in region 0, samplings 1 and 2)

      if (region==0) {

        double resp,xt0,xt1,xt2,deta;
        if (sampling==1) {
          deta=etaloc-0.003125*((double)etaBin+0.5);
          m_etamap1->GetData(std::fabs(deta),std::fabs(currentCellData.distElec),
                             &resp,&xt0,&xt1,&xt2);
#ifdef DEBUGSTEP
          ATH_MSG_DEBUG("hit in strip etaloc,etaBin,deta,delec,resp,xt0,xt1,xt2 "
                        << etaloc << " " << etaBin << " " << deta*1000 << " " << currentCellData.distElec
                        << " " << resp << " " << xt0 << " " << xt1 << " " << xt2);
#endif
          if (!InTrans) Current = Current*resp;
          if (m_IflXtalk && etaBin > 1 && etaBin < 446) {
            Xtalk=true;
            if (deta>0) {
              identifier_xt1.clear();
              identifier_xt1 << 4 << 1 << zSide << sampling << region << (etaBin+1) << phiBin;
              Current_xt1 = Current*xt1;
              identifier_xt2.clear();
              identifier_xt2 << 4 << 1 << zSide << sampling << region << (etaBin-1) << phiBin;
              Current_xt2 = Current*xt2;
            }
            else {
              identifier_xt1.clear();
              identifier_xt1 << 4 << 1 << zSide << sampling << region << (etaBin-1) << phiBin;
              Current_xt1 = Current*xt1;
              identifier_xt2.clear();
              identifier_xt2 << 4 << 1 << zSide << sampling << region << (etaBin+1) << phiBin;
              Current_xt2 = Current*xt2;
            }
            Current = Current*xt0;
          }    // m_IflXtalk = true
        }
        else if (sampling==2 && !InTrans) {
          deta=etaloc-0.025*((double)etaBin+0.5);
          m_etamap2->GetData0(std::fabs(deta),std::fabs(currentCellData.distElec),
                              &resp);
#ifdef DEBUGSTEP
          ATH_MSG_DEBUG("hit in middle etaloc,etaBin,deta,delec,resp,xt0,xt1,xt2 "
                        << etaloc << " " << etaBin << " " << deta*1000 << " " << currentCellData.distElec
                        << " " << resp);
#endif
          Current = Current*resp;
        }   // sampling =1 or 2

      }      // region=0

    }    // switch for current simulation


    // check if we have a new hit in a different cell, or if we add this substep
    //  to an already existing hit
    G4bool found=false;
    for (unsigned int i=0; i<hdata.size(); i++) {
      if (hdata[i].id==identifier2) {
        hdata[i].energy += Current;
        hdata[i].time += time*Current;
        found=true;
        break;
      }
    }    // loop over hits
    if (!found) {
      LArHitData newdata = {identifier2, time*Current, Current};
      hdata.push_back(newdata);
    }    // hit was not existing before

    if (Xtalk) {
      for (unsigned int i=0; i<hdata.size(); i++) {
        if (hdata[i].id==identifier_xt1) {
          hdata[i].energy += Current_xt1;
          hdata[i].time += time*Current_xt1;
          found=true;
          break;
        }
      }    // loop over hits
      if (!found) {
        LArHitData newdata = {identifier_xt1, time*Current_xt1, Current_xt1};
        hdata.push_back(newdata);
      }
      found=false;
      for (unsigned int i=0; i<hdata.size(); i++) {
        if (hdata[i].id==identifier_xt2) {
          hdata[i].energy += Current_xt2;
          hdata[i].time += time*Current_xt2;
          found=true;
          break;
        }
      }    // loop over hits
      if (!found) {
        LArHitData newdata = {identifier_xt2, time*Current_xt2, Current_xt2};
        hdata.push_back(newdata);
      }
    }    // Xtalk true


  }    // *** End of loop over sub steps

#ifdef DEBUGSTEP
  ATH_MSG_DEBUG("Number of hits for this step " << m_nhits << " "
                << hdata.size());
#endif

  // finalise time computations
  for (unsigned int i=0;i<hdata.size();i++) {
    if (std::fabs(hdata[i].energy)>1e-6) hdata[i].time=hdata[i].time/hdata[i].energy;
    else hdata[i].time=0.;
#ifdef DEBUGSTEP
    ATH_MSG_DEBUG("Hit Energy/Time "
                  << hdata[i].energy << " " << hdata[i].time);
#endif
  }

  if (hdata.size()>0) return true;
  return false;
}

// ===============================================================================
// Cuts to define active region of accordion calorimeter
//   1500.024 < r < 1960.00 mm
//    |eta| < 1.475
//    4 < z < 3164 mm      (in local half barrel coordinates)

G4bool LArBarrelCalculator::FiducialCuts(G4double radloc,G4double zloc,G4double etaloc) const
{
  if (radloc < m_rMinAccordion ||
      radloc > m_rMaxAccordion ||
      etaloc > m_etaMaxBarrel ||
      zloc   < m_zMinBarrel ||
      zloc   > m_zMaxBarrel)      return false;
  else                           return true;
}

// ==========================================================================
//  HV values
//   set all to 2000V. This should be changed if one wants to simulate
//    some HV imperfections
void LArBarrelCalculator::InitHV()
{
  ATH_MSG_INFO(" **** in LArBarrelCalculator::InitHV() ");




  float defaultHvVal=2000.;
  ATH_MSG_INFO(" defaultHvVal " << defaultHvVal);
  for (int ipm=0;ipm<2;ipm++) {
    for (int ielec=0;ielec<1024;ielec++) {
      for (int ieta=0;ieta<7;ieta++) {
        for (int iside=0;iside<2;iside++) {
          m_hv[ipm][ielec][ieta][iside] = defaultHvVal;
        }
      }
    }
  }

  if (m_doHV) {
    // initialize services
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StoreGateSvc* pDetStore = nullptr;

    if(svcLocator->service("DetectorStore", pDetStore).isFailure())
      {
        std::cout << "LArBarrelCalculator::InitHV() unable to get Detector Store! Use default HV values\n";
        return;
      }

    // get EMBHV Manager
    const LArHVManager *manager = nullptr;
    if (pDetStore->retrieve(manager)==StatusCode::SUCCESS) {
      const EMBHVManager* hvManager=manager->getEMBHVManager();
      ATH_MSG_INFO(" got HV Manager ");
      // loop over HV modules
      for (unsigned int iSide=0;iSide<2;iSide++) {
        for (unsigned int iPhi=0;iPhi<16;iPhi++) {
          for (unsigned int iSector=0;iSector<2;iSector++) {
            for (unsigned int iEta=0;iEta<7;iEta++) {
              EMBHVModuleConstLink hvMod = hvManager->getHVModule(iSide,iEta+1,iPhi,iSector);
              for (unsigned int ielec=0;ielec<32;ielec++) {
                EMBHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);
                unsigned jElec = ielec+32*iSector+64*iPhi;
                for (unsigned int iGap=0;iGap<2;iGap++) {
                  double hv = electrode->voltage(iGap);
                  ATH_MSG_DEBUG(" iSide,jElec,iEta,iGap,hv " << iSide << " " << jElec << " " << iEta << " " << iGap << " " << hv);
                  if (hv>-999.) m_hv[iSide][jElec][iEta][iGap] = hv;
                }
              }
            }
          }
        }
      }
    }
    else {
      ATH_MSG_WARNING(" Unable to find HV Manager ");
    }
  }

  return;
}

//==========================================================================
// extrapolate response with HV
//  hv = actual HV
//  curr0 = response for 2000V, curr1 = for 1000V, curr2 for 400V
// assumes resp = a* HV*b between 1000-2000 and 400-1000V
//   (fixes b=0.57 for HV smaller than 400V)

double LArBarrelCalculator::ScaleHV(double hv, double curr0, double curr1, double curr2) const
{
  double b;
  double resp=0.;
  if (hv>1000.) {
    if (std::fabs(curr1)>1e-6) {
      double x=curr0/curr1;
      if (x>1e-6) {
        b=log(x)*(1./(log(2000.)-log(1000.)));
        resp = curr0*exp(b*(log(hv)-log(2000.)));
      }
    }
  }
  else if (hv>400.) {
    if (std::fabs(curr2)>1e-6) {
      double x=curr1/curr2;
      if (x>1e-6) {
        b=log(x)*(1./(log(1000.)-log(400.)));
        resp = curr1*exp(b*(log(hv)-log(1000.)));
      }
    }
  }
  else {
    if (std::fabs(curr2)>1e-6) {
      b=0.57;
      resp = curr2*exp(b*(log(hv)-log(400.)));
    }
  }
  return resp;
}
