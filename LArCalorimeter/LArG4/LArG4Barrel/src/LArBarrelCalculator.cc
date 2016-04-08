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

#include "LArG4Barrel/LArBarrelCalculator.h"
#include "LArG4Barrel/AccMap.h"
#include "LArG4Barrel/MapEta.h"

#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Code/LArG4BirksLaw.h"

#include "LArG4RunControl/LArG4BarrelOptions.h"

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


LArBarrelCalculator* LArBarrelCalculator::m_instance = 0;

// ==============================================================================
LArBarrelCalculator* LArBarrelCalculator::GetCalculator()
{
  if (m_instance == 0) m_instance = new LArBarrelCalculator();
  return m_instance;
}

// ================================================================================
LArBarrelCalculator::LArBarrelCalculator()
  : m_IflCur(true)
    , m_IflMapTrans(true)
    , m_IflXtalk(true)
    , m_dstep(.2*CLHEP::mm)
    , m_birksLaw(NULL)
    , m_doHV(false)
    , m_detectorName("LArMgr")
    , m_nhits(0)
{
  std::cout << "LArBarrelCalculator: Beginning initialisation " << std::endl;

  // define RUN conditions for the Barrel

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
      m_IflCur = barrelOptions->EMBCurr();
      m_IflMapTrans = barrelOptions->EMBEtaTrans();
      m_IflXtalk = barrelOptions->EMBXtalk();

      m_dstep = barrelOptions->EMBdstep();

      bool IflBirks = barrelOptions->EMBBirksLaw();
      if (IflBirks) {
	const double Birks_LAr_density = 1.396;
	const double Birks_k = barrelOptions->EMBBirksk();
	m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,Birks_k);
      }
      m_doHV=barrelOptions->EMBHVEnable();
    }
  }

  m_identifier2 = LArG4Identifier();
  m_identifier_xt1 = LArG4Identifier();
  m_identifier_xt2 = LArG4Identifier();

// Access source of detector parameters.
  m_parameters = LArVG4DetectorParameters::GetInstance();

// Get the out-of-time cut from the detector parameters routine.
  m_OOTcut = m_parameters->GetValue("LArExpHallOutOfTimeCut");
  std::cout << "**** OutOfTime cut " << m_OOTcut << std::endl;

// Main Barrel parameters
// All the UNITS are implicitly the GEANT4 ONES e.g. mm, rad, etc ...
  m_etaMaxBarrel = m_parameters->GetValue("LArEMBMaxEtaAcceptance");
  m_zMinBarrel = m_parameters->GetValue("LArEMBfiducialMothZmin");
  m_zMaxBarrel = m_parameters->GetValue("LArEMBfiducialMothZmax");
  m_NCellMax   = (int) (m_parameters->GetValue("LArEMBnoOFPhysPhiCell"));
  m_rMinAccordion  =  m_parameters->GetValue("LArEMBRadiusInnerAccordion");
  m_rMaxAccordion  = m_parameters->GetValue("LArEMBFiducialRmax"); 

// absorbers and electrodes thickness
  m_ThickAbs = 0.5*( m_parameters->GetValue("LArEMBThickAbsGlue")
                    +m_parameters->GetValue("LArEMBThickAbsIron")
                    +m_parameters->GetValue("LArEMBThickAbsLead"));

  G4double check = 0.5*( m_parameters->GetValue("LArEMBThinAbsGlue")
                        +m_parameters->GetValue("LArEMBThinAbsIron")
                        +m_parameters->GetValue("LArEMBThinAbsLead"));

  if (fabs(check-m_ThickAbs)>0.001) {
    std::cout << " LArBarrelCalculator: WARNING  THin and Thick Abs have difference thickness !" << std::endl;
  }

  m_ThickEle= 0.5*( m_parameters->GetValue("LArEMBThickElecCopper")
                   +m_parameters->GetValue("LArEMBThickElecKapton"));

// === GU 11/06/2003   total number of cells in phi
// to distinguish 1 module (testbeam case) from full Atlas
  m_testbeam=false;
  m_NCellTot = (int) (m_parameters->GetValue("LArEMBnoOFPhysPhiCell"));
  if (m_NCellTot != 1024) {
   m_NCellMax=1024;
   m_testbeam=true;
  }
// ===
//  access geometry computation class
   m_geometry = LArG4::Barrel::Geometry::GetInstance();

// access current maps if required
    if (m_IflCur)
     std::cout << " LArBarrelCalculator: start reading of current maps" << std::endl;
   if (m_IflCur) m_accmap = AccMap::GetAccMap();
   else        m_accmap = 0;
   if (m_IflCur) {
        m_etamap1 = CxxUtils::make_unique<MapEta>(1);
        m_etamap2 = CxxUtils::make_unique<MapEta>(2);
        if (m_IflMapTrans) m_etamap3 = CxxUtils::make_unique<MapEta>(3);
   } 
   if (m_IflCur)
     std::cout << " LArBarrelCalculator: end of reading current maps" << std::endl;

// Initialize HV values
   InitHV();

   std::cout << " LArBarrelCalculator: m_NCellMax       " << m_NCellMax << std::endl;
   std::cout << " LArBarrelCalculator: m_NCellTot       " << m_NCellTot << std::endl;
   std::cout << " LArBarrelCalculator: m_rMinAccordion  " << m_rMinAccordion << std::endl;
   std::cout << " LArBarrelCalculator: m_rMaxAccordion  " << m_rMaxAccordion << std::endl;
   std::cout << " LArBarrelCalculator: m_zMinBarrel     " << m_zMinBarrel << std::endl;
   std::cout << " LArBarrelCalculator: m_zMaxBarrel     " << m_zMaxBarrel << std::endl;
   std::cout << " LArBarrelCalculator: m_etaMaxBarrel   " << m_etaMaxBarrel << std::endl;
   std::cout << " LArBarrelCalculator: m_ThickAbs       " << m_ThickAbs << std::endl;
   std::cout << " LArBarrelCalculator: m_ThickEle       " << m_ThickEle << std::endl;
   if(m_IflCur) std::cout <<" LArBarrelCalculator: Deposited Energy  dE/dX  Corrected ==> CURRENT Option ON"<<std::endl;
   else  std::cout << " LArBarrelCalculator: Crude Deposited Energy  dE/dX  NO CURRENT option"<< std::endl;
   if (m_IflCur && m_IflMapTrans) std::cout<<" LArBarrelCalculator: Compute effect of E field around eta=0.8 " << std::endl;
   else std::cout <<" LArBarrelCalculator: Ignore effect of E field around eta=0.8 " << std::endl;
   if(m_birksLaw) {
     std::cout << " LArBarrelCalculator: Birks' law ON " << std::endl;
     std::cout << " LArBarrelCalculator:   parameter k    " << m_birksLaw->k() << std::endl;
   }
   else
     std::cout << " LArBarrelCalculator: Birks' law OFF" << std::endl;	 

}

// ============================================================================
LArBarrelCalculator::~LArBarrelCalculator()
{
  if (m_birksLaw) delete m_birksLaw;
}

// =============================================================================
G4bool LArBarrelCalculator::Process(const G4Step* step, std::vector<LArHitData>& hdata)
{

  m_nhits = 0;
  //m_identifier.clear();
  //m_energy.clear();
  //m_time.clear();
  hdata.clear();
  m_isInTime.clear();

//  check the Step content is non trivial
  G4double thisStepEnergyDeposit = step->GetTotalEnergyDeposit();
  G4double thisStepLength = step->GetStepLength() / Units::mm;

#ifdef  DEBUGSTEP
  std::cout << "******  LArBarrelCalculator:  Step energy,length " 
            << thisStepEnergyDeposit << " " << thisStepLength << std::endl;
#endif

  if(thisStepEnergyDeposit <= 0. || thisStepLength <= 0.) 
  {
#ifdef DEBUGSTEP2
    std::cout << "   Invalid hit  trivial content" << std::endl;
#endif
    return false;
  }

// Get Step geometrical parameters (first and end) 
  G4StepPoint *thisStepPoint = step->GetPreStepPoint();
  G4StepPoint *thisStepBackPoint = step->GetPostStepPoint();
  G4ThreeVector startPoint = thisStepPoint->GetPosition();
  G4ThreeVector endPoint = thisStepBackPoint->GetPosition();

#ifdef  DEBUGSTEP
  std::cout << "   Beginning step position " 
            << startPoint.x() << " " << startPoint.y() << " " << startPoint.z() << std::endl;
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
  std::cout << "   Local beginning step position "
            << startPointinLocal.x() << " " << startPointinLocal.y() << " " 
            << startPointinLocal.z() << std::endl;
  std::cout << "   Local end       step position "
            << endPointinLocal.x() << " " << endPointinLocal.y() << " "
            << endPointinLocal.z() << std::endl;
#endif 

  G4double energy = step->GetTotalEnergyDeposit(); // Despite the name, this is only ionization.
    
  if (m_birksLaw) {
      const double EField = 10.;         // kV/cm, assume constant for now
      const double wholeStepLengthCm = step->GetStepLength() / Units::cm;
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
  std::cout << "   nsub_step,delta " << nsub_step << " " << delta << std::endl;
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
    std::cout << "   local position sub_step "
              << xloc << " " << yloc << " " << zloc << std::endl;
#endif

//  apply fiducial cuts
    if (!FiducialCuts(radloc,zloc,etaloc)) {
#ifdef DEBUGSTEP
       std::cout << "LArBarrelCalculator: outside fiducial cuts" << std::endl;
#endif
       continue;
    }

     m_geometry->findCell(xloc,yloc,zloc,radloc,etaloc,philoc,m_IflCur,m_detectorName);

    if (m_geometry->cellID() == 0)
    {
#ifdef DEBUGSTEP
      std::cout << "LArBarrelCalculator: Invalid hit CELLID == 0 "<<std::endl;
      std::cout << "x,y,z local " << xloc << " " << yloc << " " << zloc << std::endl;
      std::cout << "radius " << radloc <<" etaloc "<< etaloc << " philoc " << philoc <<std::endl;
#endif
      continue;
    }
    G4int region = m_geometry->region();
    G4int etaBin = m_geometry->etaBin();
    G4int phiBin = m_geometry->phiBin();
    G4int sampling = m_geometry->sampling();

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
    std::cout << "   region,side,sampling,eta,phi " << region << " " << zSide << " "
              << sampling << " " << etaBin << " " << phiBin << std::endl;
    std::cout << "   distance to electrode,abs " << m_geometry->distElec() << " "
              << m_geometry->distAbs() << std::endl;
    std::cout << "    local coordinates " << m_geometry->x0() << " " << m_geometry->y0() << std::endl;
#endif

    if (std::fabs(m_geometry->distElec())< m_ThickEle ||
        std::fabs(m_geometry->distAbs()) < m_ThickAbs) {
#ifdef DEBUGSTEP
      std::cout << "   hit in absorber or electrode radius:" << radloc << std::endl;
#endif
      continue;
    }

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
    if (m_testbeam)
    {
     time=0.;
    }
    else
    {
     G4double tof;
     tof = thisStepPoint->GetGlobalTime() / Units::ns;
     time  = tof - ( thisStepPoint->GetPosition().mag()/Units::c_light ) / Units::ns;
    }
    
#ifdef DEBUGSTEP
    std::cout << "   Energy for sub step " << energy << std::endl;
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
       G4int nfold = m_geometry->nfold();
       G4double x0=m_geometry->x0();
       G4double y0=m_geometry->y0();
       if (x0<1500 || x0>1960 || y0>30 || y0<-30) {
          std::cout << "weird x0,y0 " << x0 << " " << y0 << std::endl;
       }
#ifdef DEBUGSTEP
       G4double rr = sqrt(x0*x0+y0*y0);
       std::cout << "   radius,rad0 " << radloc << " " << rr << std::endl;
#endif
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
         std::cout << "   Map for fold xmap,ymap " << nfold << " " << xmap << " " << ymap << std::endl;
#endif
         UseFold=true;
         G4int sampMap = m_geometry->sampMap();
         G4int etaMap  = m_geometry->etaMap();
         m_accmap->SetMap(nfold,region,sampMap,etaMap);
// catch problem to find map
         if (!m_accmap->Map()) {
           std::cout << " Problem to access map fold = " << nfold << std::endl;
           std::cout << " region,sampling,eta,fold " << region << " " << sampMap << " "
                   << etaMap << " " << nfold << std::endl;
           return false;
         }
       }
       else {
         G4int n;
         G4int nstraight = m_geometry->nstraight();
         if (nstraight%2==0) n=22;
         else                n=21;
         m_accmap->SetMap(n,region,sampling,etaBin);
// catch problem to find map
         if (!m_accmap->Map()) {
           std::cout << " Problem to access map straight = " << nstraight << std::endl;
           return false;
         }    
         xmap = m_geometry->xl();
         ymap = m_geometry->distElec();
// special case for first straight section, which is shorter
         if (nstraight==0) xmap = 0.5*(xmap+1.);
#ifdef DEBUGSTEP
         std::cout << "   Map for straight xl,delec " << xmap << " " << ymap << std::endl;
#endif
       }   // fold or straight
       double gap;
       double current0,current1,current2;
//  get current for elementary charge
       m_accmap->Map()->GetAll(xmap,ymap,&gap,&current0,&current1,&current2);
       G4double gap2=std::fabs(m_geometry->distElec())+std::fabs(m_geometry->distAbs())
                    -m_ThickEle-m_ThickAbs;

// in which HV cell are we ?
       int ipm,ielec,ieta,iside;
       if (zSide==1) ipm=1;    // A side
       else          ipm=0;    // C side
       ielec=m_geometry->phiGap();
       if (zSide==-1) {
          ielec = 511 - ielec; 
          if(ielec < 0 ) ielec += 1024;
       }
       ieta=((int) (etaloc*(1./0.2)));
       if (ieta>6) ieta=6;  //part 1.4 to 1.475 is same HV as 1.2 to 1.4
       iside=0;    // phi lower than electrode 0, 1 for phi higher than electrode
       if ((m_geometry->distElec()>0 && zSide==1)
           || (m_geometry->distElec()<0 && zSide==-1) ) iside=1;

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
       std::cout << "   elementary current " << current0 << std::endl;
       std::cout << "   Gap from Map/calculator " << gap << " " << gap2 << std::endl;
#endif

       Current = energy*current;

// check if pathology...
       if (!UseFold && std::fabs(m_geometry->distElec())>2.1 && current0 < 0.1) {
          std::cout << " xl,distEle " << m_geometry->xl() << " " 
                                      << m_geometry->distElec() 
                    << " str number " << m_geometry->nstraight()
                    << " sampling,eta " << sampling << " " << etaBin << " " 
                    << " current/E " << current0 << std::endl;
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
           double y=std::fabs(m_geometry->distElec());
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
            m_etamap1->GetData(std::fabs(deta),std::fabs(m_geometry->distElec()),
                               &resp,&xt0,&xt1,&xt2);
#ifdef DEBUGSTEP
            std::cout << "hit in strip etaloc,etaBin,deta,delec,resp,xt0,xt1,xt2 "
                << etaloc << " " << etaBin << " " << deta*1000 << " " << m_geometry->distElec()
                << " " << resp << " " << xt0 << " " << xt1 << " " << xt2 << std::endl;
#endif
            if (!InTrans) Current = Current*resp;
            if (m_IflXtalk && etaBin > 1 && etaBin < 446) {
               Xtalk=true;
               if (deta>0) {
                 m_identifier_xt1.clear();
                 m_identifier_xt1 << 4 << 1 << zSide << sampling << region << (etaBin+1) << phiBin;
                 Current_xt1 = Current*xt1;
                 m_identifier_xt2.clear();
                 m_identifier_xt2 << 4 << 1 << zSide << sampling << region << (etaBin-1) << phiBin;
                 Current_xt2 = Current*xt2;
               }
               else {
                 m_identifier_xt1.clear();
                 m_identifier_xt1 << 4 << 1 << zSide << sampling << region << (etaBin-1) << phiBin;
                 Current_xt1 = Current*xt1;
                 m_identifier_xt2.clear();
                 m_identifier_xt2 << 4 << 1 << zSide << sampling << region << (etaBin+1) << phiBin;
                 Current_xt2 = Current*xt2;
               }
               Current = Current*xt0;
            }    // m_IflXtalk = true
          }
          else if (sampling==2 && !InTrans) {
            deta=etaloc-0.025*((double)etaBin+0.5);
            m_etamap2->GetData0(std::fabs(deta),std::fabs(m_geometry->distElec()),
                               &resp);
#ifdef DEBUGSTEP
            std::cout << "hit in middle etaloc,etaBin,deta,delec,resp,xt0,xt1,xt2 "
                << etaloc << " " << etaBin << " " << deta*1000 << " " << m_geometry->distElec()
                << " " << resp << std::endl;
#endif
            Current = Current*resp;
          }   // sampling =1 or 2

       }      // region=0

    }    // switch for current simulation


// check if we have a new hit in a different cell, or if we add this substep
//  to an already existing hit
    G4bool found=false;
    for (int i=0; i<m_nhits; i++) {
        if (hdata[i].id==m_identifier2) {
           hdata[i].energy += Current;
           hdata[i].time += time*Current;
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

    if (Xtalk) {
      for (int i=0; i<m_nhits; i++) {
        if (hdata[i].id==m_identifier_xt1) {
           hdata[i].energy += Current_xt1;
           hdata[i].time += time*Current_xt1;
           found=true;
           break;
        }
      }    // loop over hits
      if (!found) {
       m_nhits++;
       LArHitData newdata = {m_identifier_xt1, time*Current_xt1, Current_xt1};
       hdata.push_back(newdata);
       m_isInTime.push_back(true);
      }  
      found=false;
      for (int i=0; i<m_nhits; i++) {
        if (hdata[i].id==m_identifier_xt2) {
           hdata[i].energy += Current_xt2;
           hdata[i].time += time*Current_xt2;
           found=true;
           break; 
        }
      }    // loop over hits
      if (!found) {
       m_nhits++;
       LArHitData newdata = {m_identifier_xt2, time*Current_xt2, Current_xt2};
       hdata.push_back(newdata);
       m_isInTime.push_back(true);
      }
    }    // Xtalk true


  }    // *** End of loop over sub steps

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

// ===============================================================================
// Cuts to define active region of accordion calorimeter
//   1500.024 < r < 1960.00 mm
//    |eta| < 1.475
//    4 < z < 3164 mm      (in local half barrel coordinates)

G4bool LArBarrelCalculator::FiducialCuts(G4double radloc,G4double zloc,G4double etaloc)
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
  std::cout << " **** in LArBarrelCalculator::InitHV() " << std::endl;




  float defaultHvVal=2000.;
  std::cout << " defaultHvVal " << defaultHvVal <<std::endl;
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
    StoreGateSvc* pDetStore;
    
    StatusCode status = svcLocator->service("DetectorStore", pDetStore);
    if(status.isFailure())
      {
	std::cout << "LArBarrelCalculator::InitHV() unable to get Detector Store! Use default HV values\n";
	return;
      }
    
    // get EMBHV Manager
    const LArHVManager *manager = NULL;
    if (pDetStore->retrieve(manager)==StatusCode::SUCCESS) {
      const EMBHVManager* hvManager=manager->getEMBHVManager();
      std::cout << " got HV Manager " << std::endl;
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
		  std::cout << " iSide,jElec,iEta,iGap,hv " << iSide << " " << jElec << " " << iEta << " " << iGap << " " << hv << std::endl;
		  if (hv>-999.) m_hv[iSide][jElec][iEta][iGap] = hv; 
		}
	      }
	    } 
	  }
	}
      }    
    } 
    else {
      std::cout << " Unable to find HV Manager " << std::endl;
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

double LArBarrelCalculator::ScaleHV(double hv, double curr0, double curr1, double curr2)
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

