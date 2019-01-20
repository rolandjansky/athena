/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SingleTrackValidation.h"
#include "StoreGate/StoreGateSvc.h"

// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"

// In order to be able to Ntuple & cet:
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// To get the Magnetic Field:
#include "CLHEP/GenericFunctions/FixedConstant.hh"

// To get the particle properties:
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"

// To extrapolate:
#include "AtlasBComponent.h"
#include "GeoXPEngine.h"

// To interpret LAr Geometry information:
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArID_Exception.h"
// To make it easier for us to access hit info
#include "GeoAdaptors/GeoLArHit.h"

// pi etc
#include "CLHEP/Units/PhysicalConstants.h"
#include "AthenaKernel/Units.h"

// To have histograms:
#include "GaudiKernel/ITHistSvc.h"

// System libraries
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/times.h>
#include <string>

namespace Units = Athena::Units;
using HepGeom::Point3D;
using CLHEP::HepLorentzVector;
using CLHEP::mm;
using CLHEP::pi;
using CLHEP::twopi;

inline  int iabs (int a) { return (a>=0)? a : -a;}
inline int getCpu() {
  int cpuTime = 0;
  {
    static int ticksPerJiffy = sysconf(_SC_CLK_TCK)/100;
    struct tms buff;
    times(&buff);
    cpuTime=(buff.tms_utime + buff.tms_stime + buff.tms_cutime + buff.tms_cstime)/ticksPerJiffy;
  }
  return cpuTime;
}

// We will define some actual data, later.
class SingleTrackValidation::Clockwork {

public:

  Clockwork() : partPropSvc(0), histSvc(0), mgr(0), nt(0) , coolField(true) {}

  ~Clockwork() {}

  IPartPropSvc             *partPropSvc;
  ITHistSvc                *histSvc;
  const CaloDetDescrManager *mgr;
  NTuple::Tuple *nt;

  NTuple::Item<double> eta;
  NTuple::Item<double> pt;
  NTuple::Item<double> phi;
  
  // Impact point
  NTuple::Item<double> x;
  NTuple::Item<double> y;
  NTuple::Item<double> z;

  // Indices run from 0 to 11: 4 in barrel, 4 in end cap, 3 in FCAL
  // Energy within each sampling layer (directly struck cell):
  NTuple::Item<double> s_c00[15];
  NTuple::Item<double> s_hits[15];
  NTuple::Item<double> s_sumE[15];
  NTuple::Item<double> s_deltaPhi[15];
  NTuple::Item<double> s_sigmaPhi[15];
  NTuple::Item<double> s_deltaEta[15];
  NTuple::Item<double> s_sigmaEta[15];
  NTuple::Item<double> s_t00[15];
  NTuple::Item<double> s_widthX[15];
  NTuple::Item<double> s_widthY[15];

  NTuple::Item<double> cpuTime;
  NTuple::Item<double> Energy;
  NTuple::Item<double> PDG;
  NTuple::Item<double> RunNo;
  NTuple::Item<double> EventNo;
  NTuple::Item<double> E_Deposit;
 
  bool coolField; 
};

SingleTrackValidation::SingleTrackValidation (const std::string & name, ISvcLocator * pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),m_c(new Clockwork())
{
  for (unsigned int i=0;i<162;++i) m_histos[i] = 0;
}

SingleTrackValidation::~SingleTrackValidation () {
  if (m_c!=0){ delete m_c; m_c=0; }
}

StatusCode SingleTrackValidation::initialize() {

  StatusCode status;

  //-------------------------------------------------------------------------//
  //                                                                         //
  // Get the histogram service:                                              //
  //                                                                         //
  // actually, want an THistSvc
  // m_c->histSvc=histoSvc();                                                  //
  m_c->histSvc=NULL;
  status = service("THistSvc", m_c->histSvc);
  if (!status.isSuccess()) m_c->histSvc=NULL;                          //
  //-------------------------------------------------------------------------//

  if (!m_c->histSvc) throw std::runtime_error ("STV: Histogram Svc not found");     

  std::string names[162] = {  "eta", "pt", "phi", "pos_x", "pos_y", "pos_z",
             "emb0_cell", "emb1_cell", "emb2_cell", "emb3_cell", "emec0_cell", "emec1_cell", "emec2_cell", "emec3_cell", 
             "hec0_cell", "hec1_cell", "hec2_cell", "hec3_cell", "fc1_cell", "fc2_cell", "fc3_cell",
             "emb0_hits", "emb1_hits", "emb2_hits", "emb3_hits", "emec0_hits", "emec1_hits", "emec2_hits", "emec3_hits",
             "hec0_hits", "hec1_hits", "hec2_hits", "hec3_hits", "fc1_hits", "fc2_hits", "fc3_hits",
             "emb0_sumE", "emb1_sumE", "emb2_sumE", "emb3_sumE", "emec0_sumE", "emec1_sumE", "emec2_sumE", "emec3_sumE",
             "hec0_sumE", "hec1_sumE", "hec2_sumE", "hec3_sumE", "fc1_sumE", "fc2_sumE", "fc3_sumE",
             "emb0_dPhi", "emb1_dPhi", "emb2_dPhi", "emb3_dPhi", "emec0_dPhi", "emec1_dPhi", "emec2_dPhi", "emec3_dPhi",
             "hec0_dPhi", "hec1_dPhi", "hec2_dPhi", "hec3_dPhi", "fc1_dX", "fc2_dX", "fc3_dX",
             "emb0_sPhi", "emb1_sPhi", "emb2_sPhi", "emb3_sPhi", "emec0_sPhi", "emec1_sPhi", "emec2_sPhi", "emec3_sPhi",
             "hec0_sPhi", "hec1_sPhi", "hec2_sPhi", "hec3_sPhi", "fc1_sX", "fc2_sX", "fc3_sX",
             "emb0_dEta", "emb1_dEta", "emb2_dEta", "emb3_dEta", "emec0_dEta", "emec1_dEta", "emec2_dEta", "emec3_dEta",
             "hec0_dEta", "hec1_dEta", "hec2_dEta", "hec3_dEta", "fc1_dY", "fc2_dY", "fc3_dY",
             "emb0_sEta", "emb1_sEta", "emb2_sEta", "emb3_sEta", "emec0_sEta", "emec1_sEta", "emec2_sEta", "emec3_sEta",
             "hec0_sEta", "hec1_sEta", "hec2_sEta", "hec3_sEta", "fc1_sY", "fc2_sY", "fc3_sY",
             "emb0_time", "emb1_time", "emb2_time", "emb3_time", "emec0_time", "emec1_time", "emec2_time", "emec3_time",
             "hec0_time", "hec1_time", "hec2_time", "hec3_time", "fc1_time", "fc2_time", "fc3_time",
             "emb0_widthX", "emb1_widthX", "emb2_widthX", "emb3_widthX", "emec0_widthX", "emec1_widthX", "emec2_widthX", "emec3_widthX",
             "hec0_widthX", "hec1_widthX", "hec2_widthX", "hec3_widthX", "fc1_widthX", "fc2_widthX", "fc3_widthX",
             "emb0_widthY", "emb1_widthY", "emb2_widthY", "emb3_widthY", "emec0_widthY", "emec1_widthY", "emec2_widthY", "emec3_widthY",
             "hec0_widthY", "hec1_widthY", "hec2_widthY", "hec3_widthY", "fc1_widthY", "fc2_widthY", "fc3_widthY",
             "cpuTime", "Energy", "PDG_ID", "RunNo", "EventNo", "E_Dep" };

  double lim[162][2] = { {0,5}, {0,100}, {-4,4}, {-1600,1600}, {-1600,1600}, {-4000,4000},
                         {0,0.25}, {0,3}, {0,6}, {0,0.1}, {0,0.25}, {0,2}, {0,3}, {0,0.1}, {0,1}, {0,10}, {0,10}, {0,10}, {0,0.1}, {0,0.1}, {0,0.1}, //cell
                         {0,100}, {0,600}, {0,600}, {0,50}, {0,50}, {0,400}, {0,500}, {0,200}, {0,100}, {0,1000}, {0,1000}, {0,100}, {0,150}, {0,50}, {0,10}, //hits
                         {0,0.4}, {0,5}, {0,10}, {0,0.2}, {0,0.1}, {0,3}, {0,5}, {0,0.2}, {0,1}, {0,10}, {0,10}, {0,0.1}, {0,1}, {0,0.1}, {0,0.1}, //sumE
                         {-500,500}, {-100,100}, {-15,15}, {-200,200}, {-3000,3000}, {-60,60}, {-25,25}, {-200,200}, {-50,50}, {-50,50}, {-50,50}, {-50,50}, {-60,60}, {-500,500}, {-200,200}, //dPhi
                         {0,1000}, {0,500}, {0,200}, {0,500}, {0,2000}, {0,250}, {0,300}, {0,500}, {0,200}, {0,200}, {0,200}, {0,200}, {0,100}, {0,100}, {0,50}, //sPhi
                         {-150,150}, {-15,15}, {-20,20}, {-200,200}, {-0,2500}, {-50,20}, {-15,15}, {-150,150}, {-50,50}, {-50,50}, {-50,50}, {-50,50}, {-60,60}, {-500,500}, {-200,200}, //dEta
                         {0,500}, {0,100}, {0,100}, {0,400}, {0,1000}, {0,150}, {0,100}, {0,400}, {0,200}, {0,200}, {0,200}, {0,200}, {0,60}, {0,100}, {0,50}, //sPhi
                         {0,750}, {0,25}, {0,20}, {0,1000}, {0,10000}, {0,40}, {0,30}, {0,1000}, {0,1000}, {0,100}, {0,100}, {0,200}, {0,10}, {0,500}, {0,100}, //time
                         {-150,150}, {-15,15}, {-20,20}, {-200,200}, {-0,2500}, {-50,20}, {-15,15}, {-150,150}, {-50,50}, {-50,50}, {-50,50}, {-50,50}, {-60,60}, {-500,500}, {-200,200}, //widthX
                         {-150,150}, {-15,15}, {-20,20}, {-200,200}, {-0,2500}, {-50,20}, {-15,15}, {-150,150}, {-50,50}, {-50,50}, {-50,50}, {-50,50}, {-60,60}, {-500,500}, {-200,200}, //widthY
                         {0,50}, {0,100}, {-25,25},  {0,10},  {0,1000},  {0,10}};

  std::string filename="";
  for (int i=0;i<162;i++){
    m_histos[i] = new TH1F( names[i].data(), names[i].data(),50,lim[i][0],lim[i][1]);
    filename = "/file1/Electron/";
    filename.append(names[i]);
    if (m_c->histSvc->regHist( filename.data() , m_histos[i] ).isFailure()){
      ATH_MSG_WARNING( "Failed to register historam " << names[i] << ".  Not sure what will happen now..." );
    }
  }

  //-------------------------------------------------------------------------//
  //                                                                         //
  // Initialize the Particle Property Service. This is necessary in order    //
  // to obtain charge & type & other properties of the primary particle and  //
  // other particles that may turn up in the debris.                         //
  //                                                                         //
  m_c->partPropSvc=NULL;                                                       //
  if (!service("PartPropSvc", m_c->partPropSvc).isSuccess()) m_c->partPropSvc=NULL;                      //
  //-------------------------------------------------------------------------//

  if (!m_c->partPropSvc) throw std::runtime_error ("STV: Part Prop Svc not found");

  //-------------------------------------------------------------------------//
  //                                                                         //
  // Retrieve the CaloDetDescrMgr from the detector store.                   //
  //                                                                         //
  if (!detStore()->retrieve(m_c->mgr).isSuccess()) m_c->mgr=NULL;                              //
  //-------------------------------------------------------------------------//
 
  if (!m_c->mgr) throw std::runtime_error ("STV: Calo Mgr not found");

  //----------------Now initialize the ntuples         ----------------------//
  //                                                                         //
  //==~                               ~                                   ~==//

  NTupleFilePtr     file(ntupleSvc(),"/NTUPLES/FILE");
  if (!file) throw std::runtime_error ("Ntuple MGR not open");
  NTuple::Directory *col=ntupleSvc()->createDirectory("/NTUPLES/FILE/COL");
  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE/COL/SingleTrackValidation");
  if (!nt) nt=ntupleSvc()->book(col, 1, CLID_ColumnWiseTuple, "SingleTrackValidation");
  
  if (nt->addItem("Eta",          m_c->eta	).isFailure() ||
      nt->addItem("Pt",           m_c->pt		).isFailure() ||
      nt->addItem("BarrelX",      m_c->x		).isFailure() ||
      nt->addItem("BarrelY",      m_c->y		).isFailure() ||
      nt->addItem("BarrelZ",      m_c->z		).isFailure() ||
      nt->addItem("Phi",	      m_c->phi	).isFailure() ){
    ATH_MSG_WARNING( "Registration of some of the ntuple branches failed.  No idea what will happen next..." );
  }


  char title[80];

  // Indices from 0 to 11: 4 EMB, 4 EMEC, and 4 FCAL sampling layers
  //   Handling FCAL layers separately for different titles (could do 
  //   something more complicated if we wanted)
  for (int i=0;i<15;i++){
    if (i<12) sprintf(title,"S%i_C00",i);
    else sprintf(title,"FC%i_C00",i-11);
    if (nt->addItem(title,m_c->s_c00[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_SumE",i);
    else sprintf(title,"FC%i_SumE",i-11);
    if (nt->addItem(title,m_c->s_sumE[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_Hits",i);
    else sprintf(title,"FC%i_Hits",i-11);
    if (nt->addItem(title,m_c->s_hits[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." );
    if (i<12) sprintf(title,"S%i_DeltaPhi",i);
    else sprintf(title,"FC%i_DeltaX",i-11);
    if (nt->addItem(title,m_c->s_deltaPhi[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_SigmaPhi",i);
    else sprintf(title,"FC%i_SigmaX",i-11);
    if (nt->addItem(title,m_c->s_sigmaPhi[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_DeltaEta",i);
    else sprintf(title,"FC%i_DeltaY",i-11);
    if (nt->addItem(title,m_c->s_deltaEta[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_SigmaEta",i);
    else sprintf(title,"FC%i_SigmaY",i-11);
    if (nt->addItem(title,m_c->s_sigmaEta[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_Time",i);
    else sprintf(title,"FC%i_Time",i-11);
    if (nt->addItem(title,m_c->s_t00[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_WidthX",i);
    else sprintf(title,"FC%i_WidthX",i-11);
    if (nt->addItem(title,m_c->s_widthX[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
    if (i<12) sprintf(title,"S%i_WidthY",i);
    else sprintf(title,"FC%i_WidthY",i-11);
    if (nt->addItem(title,m_c->s_widthY[i]).isFailure()) ATH_MSG_INFO( "Registration of a branch failed in the ntupler..." ); 
  }

  if (nt->addItem("CPU"        ,  m_c->cpuTime	).isFailure() ||
      nt->addItem("TrackEnergy"  ,  m_c->Energy 	).isFailure() ||
      nt->addItem("ParticleID" ,  m_c->PDG		).isFailure() ||
      nt->addItem("Run#"      ,  m_c->RunNo  	).isFailure() ||
      nt->addItem("Event#"    ,  m_c->EventNo	).isFailure() ||
      nt->addItem("DepositedEnergy", m_c->E_Deposit  ).isFailure() ){
    ATH_MSG_WARNING( "Registration of some of the ntuple branches failed.  No idea what will happen next..." );
  }
   
  m_c->cpuTime=0.0;
  m_c->nt = nt;
    
  //==~                               ~                                   ~==//
  //                                                                         //
  //------------------------Done with initializations------------------------//

  return StatusCode::SUCCESS;
}

StatusCode SingleTrackValidation::execute() {

  if (m_c->cpuTime==0) {
    m_c->cpuTime=getCpu();
    m_c->cpuTime+=1;   // Fill the histogram with -1 for the first event
  }
  m_c->cpuTime= getCpu()-m_c->cpuTime;
  m_histos[156]->Fill( m_c->cpuTime/100. , 1. );

  StatusCode sc;
  
  StoreGateSvc *stg;
  sc=service("StoreGateSvc", stg);
    
  const EventContext& context = getContext();
  int RunNum=context.eventID().run_number();
  int EvtNum=context.eventID().event_number();
  double RunStr=double(RunNum);
  double EvtStr=double(EvtNum);
  m_c->EventNo=EvtStr;
  m_c->RunNo=RunStr;
  m_histos[160]->Fill(EvtStr);
  m_histos[159]->Fill(RunNum);
  
  // Get the MC Truth Information
  const DataHandle<McEventCollection> mcEvent;
  sc=stg->retrieve(mcEvent,"TruthEvent");
  if (sc.isFailure()) return StatusCode::SUCCESS;
 
  DataVector<HepMC::GenEvent>::const_iterator e;
  for (e=mcEvent->begin();e!=mcEvent->end();e++) {
    
    // Get just the primary, call it "theParticle"
    const HepMC::GenParticle *theParticle= *((**e).particles_begin());

    // Fetch whatever particle properties will be used in the following:
    const HepPDT::ParticleDataTable * dataTable = m_c->partPropSvc->PDT();
    const HepPDT::ParticleData      * particleData = dataTable->particle(iabs(theParticle->pdg_id()));

    // Get the kinematic variables:
    HepLorentzVector momentum(theParticle->momentum().px(),
			      theParticle->momentum().py(),
			      theParticle->momentum().pz(),
			      theParticle->momentum().e());
    Point3D<double>       origin(theParticle->production_vertex()->point3d().x(),
			    theParticle->production_vertex()->point3d().y(),
			    theParticle->production_vertex()->point3d().z()); 
    //HepMC::GenVertex *decayVertex = theParticle->end_vertex();
    double           charge = theParticle->pdg_id() > 0 ? particleData->charge() : - particleData->charge();
    // Put Eta and Phi into the Ntuple
    m_c->phi = theParticle->momentum().phi();
    m_c->eta = -log(tan(theParticle->momentum().theta()/2));
    if (!finite(m_c->eta)) m_c->eta=0;
    m_c->pt  = theParticle->momentum().perp();
    int partID = theParticle->pdg_id();
    double pID = double(partID);
    m_c->PDG = pID;
    m_c->Energy = theParticle->momentum().e();
    m_histos[2]->Fill( theParticle->momentum().phi() );
    double myEta = -log(tan(theParticle->momentum().theta()/2));
    if (!finite(myEta)) m_histos[0]->Fill(0);
    else m_histos[0]->Fill( myEta );
    m_histos[158]->Fill( pID );
    m_histos[1]->Fill( theParticle->momentum().perp()/Units::GeV );
    m_histos[157]->Fill( m_c->Energy = theParticle->momentum().e()/Units::GeV );

    // Make an extrapolator:
    static const Genfun::AbsFunction  * Bx = new Genfun::AtlasBComponent(0);
    static const Genfun::AbsFunction  * By = new Genfun::AtlasBComponent(1);
    static const Genfun::AbsFunction  * Bz = new Genfun::AtlasBComponent(2);
    GeoXPEngine extrapolator(*Bx,*By,*Bz, origin, momentum, charge);
    
    // Extrapolate to the first layer in the barrel:
    m_c->x = 0;
    m_c->y = 0;
    m_c->z = 0;
    double x=0,y=0,z=0;
    bool hitsBarrel=false;
    //bool hitsEndcap=false;
    for (double t = 0; t< 50; t += 0.1) {
      x = extrapolator.x()(t);
      y = extrapolator.y()(t);
      z = extrapolator.z()(t);
      double magicZ=3640.0*mm;
      double magicR=1500.0*mm;
      if (x*x+y*y > magicR*magicR) {
	m_c->x = x;
	m_c->y = y;
	m_c->z = z;
	hitsBarrel=true;
	break;
      }
      else if (z*z > magicZ*magicZ) {
	m_c->x = x;
	m_c->y = y;
	m_c->z = z;
	//hitsEndcap=true;
          break;
      }
    }
    m_histos[3]->Fill( x );
    m_histos[4]->Fill( y );
    m_histos[5]->Fill( z );

    // You have an x,y, and z position.  Now go and get the Element corresponding to
    // that hit position. There are four, one for each sampling layer:
    double radImpact   = sqrt(x*x+y*y+z*z);
    double phiImpact   = atan2(y,x);
    double thetaImpact = acos(z/radImpact);
    double etaImpact   = -log(tan(thetaImpact/2));
      
    const CaloDetDescrElement *element[15]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

    for (int i=0;i<4;i++) {
      try {
        element[i] = m_c->mgr->get_element(CaloCell_ID::LAREM,i, hitsBarrel, etaImpact, phiImpact);
      }
      catch (const LArID_Exception & e) {
        std::cerr << "SingleTrackValidation EXCEPTION (LAREM)" << e.message() << std::endl;
      }	
    }
    for (int i=0;i<4;i++) {
      try {
        element[i+4] = m_c->mgr->get_element(CaloCell_ID::LAREM,i, hitsBarrel, etaImpact, phiImpact);
      }
      catch (const LArID_Exception & e) {
        std::cerr << "SingleTrackValidation EXCEPTION (LAREM)" << e.message() << std::endl;
      }
    }     
    for (int i=0;i<4;i++) {
      try {
        element[i+8] = m_c->mgr->get_element(CaloCell_ID::LARHEC,i, hitsBarrel, etaImpact, phiImpact);
      }
      catch (const LArID_Exception & e) {
        std::cerr << "SingleTrackValidation EXCEPTION in (LARHEC)" << e.message() << std::endl;
      }	
    }
    for (int i=1;i<4;i++) {
      try {
        element[i+11] = m_c->mgr->get_element(CaloCell_ID::LARFCAL,i, hitsBarrel, etaImpact, phiImpact);
      }
      catch (const LArID_Exception & e) {
        std::cerr << "SingleTrackValidation EXCEPTIONin LARFCAL" << e.message() << std::endl;
      }	
    }
     
     
    // Now go and find out how much energy is there:
    for (int z=0;z<15;z++){
      m_c->s_c00[z]=0;
      m_c->s_t00[z]=0;
    }
      
    std::string lArKey = hitsBarrel ? "LArHitEMB" : "LArHitEMEC" ;

    double eSum    [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double eEta    [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double eEtaEta [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double ePhi    [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double ePhiPhi [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int hit_count  [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};    
    double eX      [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double eXX     [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double eY      [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double eYY     [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double c00     [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double t00     [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//    double width   [15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double e_dep = 0;
	
    for (int i=0;i<4;i++) {  // Loop over the four LAr collections
      if (i==0)        {
        lArKey="LArHitEMB";
      } else if (i==1) {
        lArKey="LArHitEMEC";
      } else if (i==2) {
        lArKey="LArHitHEC";
      } else if (i==3) {
        lArKey="LArHitFCAL";
      }
	 
      StatusCode status;
      const DataHandle<LArHitContainer> iter;
      status=stg->retrieve(iter,lArKey);
		
      if (status==StatusCode::SUCCESS) {
        LArHitContainer::const_iterator hi=(*iter).begin();//,he=(*iter).end();
        for (hi = (*iter).begin();hi != (*iter).end(); ++hi){
          GeoLArHit ghit(**hi);
          if (!ghit) continue;
	    
          const CaloDetDescrElement *hitElement = ghit.getDetDescrElement();
          int samplingLayer =  ghit.SamplingLayer();
          double energy     =  ghit.Energy();
	    	    
          for (int j=0;j<15;j++) { 
	    if (hitElement==element[j]) {
	      c00[j] += energy;
	    }
          }
	  
          if (lArKey=="LArHitEMEC") samplingLayer += 4;
          else if (lArKey=="LArHitHEC") samplingLayer += 8;	
          else if (lArKey=="LArHitFCAL") samplingLayer += 11;
            
          // Calculate phi of hit w.r.t phiImpact
          // to avoid problems due to the 2pi modulus of phi
          // we are thus calculating deltaphi directly, so don't subtract phiImpact again later
          double hitPhi = hitElement->phi() - phiImpact;
          if (hitPhi < -pi) hitPhi += twopi;
          if (hitPhi > pi) hitPhi -= twopi;

          eSum     [samplingLayer]+=energy;
          eEta     [samplingLayer]+=energy*hitElement->eta();
          eEtaEta  [samplingLayer]+=energy*hitElement->eta()*hitElement->eta();
          ePhi     [samplingLayer]+=energy*hitPhi;
          ePhiPhi  [samplingLayer]+=energy*hitPhi*hitPhi;
          eX	   [samplingLayer]+=energy*hitElement->x();
          eXX      [samplingLayer]+=energy*hitElement->x()*hitElement->x();
          eY	   [samplingLayer]+=energy*hitElement->y();
          eYY      [samplingLayer]+=energy*hitElement->y()*hitElement->y();
          t00	   [samplingLayer]+=energy*ghit.Time();
          hit_count[samplingLayer]+=1;
        }		    
      }	
    }	
	
    for (int i=0;i<15;i++) {
      if (eSum[i]!=0) eEta[i]    /= eSum[i];
      if (eSum[i]!=0) eEtaEta[i] /= eSum[i];
      if (eSum[i]!=0) ePhi[i]    /= eSum[i];
      if (eSum[i]!=0) ePhiPhi[i] /= eSum[i];
      if (eSum[i]!=0) eY[i]      /= eSum[i];
      if (eSum[i]!=0) eYY[i]     /= eSum[i];
      if (eSum[i]!=0) eX[i]      /= eSum[i];
      if (eSum[i]!=0) eXX[i]     /= eSum[i];
      if (eSum[i]!=0) t00[i]     /= eSum[i];		
      e_dep+=eSum[i];
    }

    double dThetaDEta = -1.0/cosh(etaImpact);
	
    //Fill the E Sum, center cell E, hit count fields:
    m_c->E_Deposit=e_dep;
    for (int z=0;z<15;z++){
      m_c->s_sumE[z]=eSum[z];
      m_c->s_c00[z]=c00[z];
      m_c->s_t00[z]=t00[z];
      m_c->s_hits[z]=hit_count[z];
      if (z<12){
        m_c->s_deltaPhi[z]=radImpact*sin(thetaImpact)*(ePhi[z]);
        m_c->s_sigmaPhi[z]=radImpact*sin(thetaImpact)*sqrt(ePhiPhi[z]- ePhi[z]*ePhi[z]);
        m_c->s_deltaEta[z]=radImpact*dThetaDEta*(eEta[z]-etaImpact);
        m_c->s_sigmaEta[z]=radImpact*fabs(dThetaDEta)*sqrt(eEtaEta[z]- eEta[z]*eEta[z]);
      } else {
        m_c->s_deltaPhi[z]=(eX[z]-x);
        m_c->s_sigmaPhi[z]=sqrt(eXX[z]- eX[z]*eX[z]);
        m_c->s_deltaEta[z]=(eY[z]-y);
        m_c->s_sigmaEta[z]=sqrt(eYY[z]-eY[z]*eY[z]);
      }
      m_c->s_widthX[z]=sqrt(eXX[z]-eX[z]*eX[z]);
      m_c->s_widthY[z]=sqrt(eYY[z]-eY[z]*eY[z]);
    }
	
    m_histos[161]->Fill(e_dep/Units::GeV);
    for (int i=0;i<15;i++){
      m_histos[6+i]->Fill( c00[i]/Units::GeV );
      m_histos[21+i]->Fill( hit_count[i] );
      m_histos[36+i]->Fill( eSum[i]/Units::GeV );
      m_histos[111+i]->Fill( t00[i] );
      m_histos[126+i]->Fill( sqrt(eXX[i]-eX[i]*eX[i]) );
      m_histos[141+i]->Fill( sqrt(eYY[i]-eY[i]*eY[i]) );
      if (i<8){
        m_histos[51+i]->Fill( radImpact*sin(thetaImpact)*ePhi[i] );
        m_histos[66+i]->Fill( radImpact*sin(thetaImpact)*sqrt(ePhiPhi[i]-ePhi[i]*ePhi[i]) );
        m_histos[81+i]->Fill( radImpact*dThetaDEta*(eEta[i]-etaImpact) );
        m_histos[96+i]->Fill( radImpact*fabs(dThetaDEta)*sqrt(eEtaEta[i]-eEta[i]*eEta[i]) );
      } else {
        m_histos[51+i]->Fill( eX[i]-x );
        m_histos[66+i]->Fill( sqrt(eXX[i]-eX[i]*eX[i]) );
        m_histos[81+i]->Fill( eY[i]-y );
        m_histos[96+i]->Fill( sqrt(eYY[i]-eY[i]*eY[i]) );
      }
    }

    ntupleSvc()->writeRecord(m_c->nt);
      
  }

  m_c->cpuTime= getCpu();

  return StatusCode::SUCCESS;
  
}

StatusCode SingleTrackValidation::finalize() {
  return StatusCode::SUCCESS;
}
