/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignWrt.cxx
// Richard Hawkings, started 15/4/04
// Algorithm to create ID Alignment database in TDS and optionally write
// it to conditions database
//
#include <fstream>
#include "AthenaBaseComps/AthCheckMacros.h" 	
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "InDetAlignGenTools/IInDetAlignDBTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h" 
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "InDetAlignGenAlgs/InDetAlignWrt.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"


InDetAlignWrt::InDetAlignWrt(const std::string& name, ISvcLocator* pSvcLocator)
  :AthAlgorithm   (name, pSvcLocator),
   m_beamcondsvc("BeamCondSvc",name),      
   p_iddbtool("InDetAlignDBTool"),
   p_migratetool("InDetAlignDBTool/InDetAlignMigrate"),
   p_eventinfo (0),
   m_setup     (false),
   par_create(true),
   par_wrt(false),
   par_wrtiov(true),
   par_print(false),
   par_migrate(false),
   par_rfile(""),
   par_wfile(""),
   par_dispfile(""),
   par_ntuple(false),
   par_wrtrun(-1),
   par_wrtevent(0),
   par_run1(IOVTime::MINRUN),
   par_run2(IOVTime::MAXRUN),
   par_evt1(IOVTime::MINEVENT),
   par_evt2(IOVTime::MAXEVENT),
   par_iovtag(""),
   par_dbtoolinst("InDetAlignDBTool"),
   par_dispmode(0),
   par_dettype(-1),
   par_bec(-1),
   par_layer(-1),
   par_ring(-1),
   par_sector(-1),
   par_rphidisp(0.1),
   par_rdisp(0.2),
   par_zdisp(0.3),
   par_systdisp(1),
   par_irskip(0),
   par_dispcsc(0),
   par_wrtbeam(false),
   par_writetop(false),
   par_topname("")
{
  
  // declare algorithm parameters
  declareProperty("BeamConditionService",m_beamcondsvc);   
  declareProperty("Create",par_create);
  declareProperty("Write",par_wrt);
  declareProperty("WriteIOV",par_wrtiov);
  declareProperty("PrintDB",par_print);
  declareProperty("Migrate",par_migrate);
  declareProperty("RFile",par_rfile);
  declareProperty("WFile",par_wfile);
  declareProperty("RWNtuple",par_ntuple);
  declareProperty("WriteRun",par_wrtrun);
  declareProperty("WriteEvent",par_wrtevent);
  declareProperty("ValidRun1",par_run1);
  declareProperty("ValidRun2",par_run2);
  declareProperty("ValidEvent1",par_evt1);
  declareProperty("ValidEvent2",par_evt2);
  declareProperty("IOVTag",par_iovtag);
  declareProperty("DBToolInst",par_dbtoolinst);
  declareProperty("DispMode",par_dispmode);
  declareProperty("DispDet",par_dettype);
  declareProperty("DispBec",par_bec);
  declareProperty("DispLayer",par_layer);
  declareProperty("DispRing",par_ring);
  declareProperty("DispSector",par_sector);
  declareProperty("DispRphi",par_rphidisp);
  declareProperty("DispR",par_rdisp);
  declareProperty("DispZ",par_zdisp);
  declareProperty("DispSyst",par_systdisp);
  declareProperty("DispFile",par_dispfile);
  declareProperty("RndmSkip",par_irskip);
  declareProperty("DispCSC",par_dispcsc);

  declareProperty("WriteBeamPos",par_wrtbeam);
  declareProperty("WriteTopTrans",par_writetop);
  declareProperty("TopTransFile",par_topname);

  declareProperty("InDetAlignDBTool",p_iddbtool);
  declareProperty("InDetAlignMigrateTool",p_migratetool);
}


InDetAlignWrt::~InDetAlignWrt(void)
{}

StatusCode InDetAlignWrt::initialize() {
  ATH_MSG_DEBUG( "InDetAlignWrt::initialize() called" );
  // get storegate pointers
  if ( (evtStore().retrieve().isFailure()) ||
       (detStore().retrieve().isFailure())) {
    ATH_MSG_VERBOSE ("StoreGate or DetStore services not found" );
    return StatusCode::FAILURE;
  }


  ATH_CHECK( p_iddbtool.retrieve() );

  // get second InDetAlignDBTool for migration if needed
  if (par_migrate) {
    ATH_CHECK (p_migratetool.retrieve() );
  }
  // get BeamCondSvc if needed
  if (par_wrtbeam) {
    
    if ( m_beamcondsvc.retrieve().isFailure()){
           msg( MSG::ERROR) <<"Cannot get BeamCondSvc - beamspots will not be put in ntuple" << endreq;
    }
  }
  
  if (msgLvl(MSG::DEBUG)) {
    if (par_create) msg() << "ID Alignment database structures will be created" << endreq;
    if (par_rfile!="") {
      if (par_ntuple) msg() << "Alignment constants will be initialised from ntuple: " << par_rfile << endreq;  
      else msg() << "Alignment constants will be initialised from text file" << par_rfile << endreq;
    }
    if (par_dispfile!="") 
      msg() << "Alignment constants will be set according to recipe in file " << par_dispfile << endreq;
    if (par_wrt) 
      msg() << "ID Alignment database structures will be written on run " 
      << par_wrtrun << " event " << par_wrtevent << endreq;
    if (par_wfile!="") 
      msg() << "Structures will be written on file " << par_wfile << endreq;
  if (par_wrtbeam) 
    msg() << "Beampos information will be written on run " << par_wrtrun 
    << " event " << par_wrtevent << endreq;
  if (par_writetop) 
    msg() << "Toplevel transforms will be read from file " << par_topname 
    << "and written on run " << par_wrtrun << " event " << par_wrtevent << endreq;
  if (!par_wrtiov) 
    msg() << "IOV writing will be suppressed - only output stream" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode InDetAlignWrt::execute() {

  if (!m_setup) {
    m_setup=true;
    if (par_create) {
      ATH_MSG_DEBUG( 
        "Creating null alignment database from geometry information in GeoModel");
      p_iddbtool->createDB();
    }
    if (par_rfile!="") {
      if (par_ntuple) {
	p_iddbtool->readNtuple(par_rfile);
      } else {
	p_iddbtool->readTextFile(par_rfile);
      }
    }
    if (par_dispmode>0 || par_dispcsc>0) DispDB();
    if (par_dispfile!="") dispFile();
    if (par_print) p_iddbtool->printDB(2);
  }
  // make a fall back solution in case event info not available to prevent crash
  int run = -1;
  int event = 0;
  if (evtStore()->contains< xAOD::EventInfo >( "EventInfo" ) && 
		  StatusCode::SUCCESS==evtStore()->retrieve(p_eventinfo)) {
    run=p_eventinfo->runNumber();
    event=p_eventinfo->eventNumber();
  }

  ATH_MSG_DEBUG(  "In AlignWrt::execute for run/event " << run << 
      "," << event );
  // trigger database write by setting filter passed to false
  if ((par_wrt || par_wrtbeam || par_writetop) && 
      par_wrtrun==run && par_wrtevent==event) doOutput();

  return StatusCode::SUCCESS;
}


StatusCode InDetAlignWrt::finalize() {
  if (par_wrtrun==-1) doOutput();
  if (par_migrate) doMigration();
  return StatusCode::SUCCESS;
}

void InDetAlignWrt::DispDB() {
  // displace silicon modules according to recipe in DispMode
  // or use DispCSC if appropriate parameter is set
  //if (par_dispcsc!=0) {
  //  ATH_MSG_DEBUG("DispCSC called for mode " << par_dispcsc );
  //  p_iddbtool->dispCSC(par_dettype, par_bec, par_layer, par_ring,
  //    par_sector, par_dispcsc);
  //} else {
  ATH_MSG_DEBUG( "DispDB called for mode " << par_dispmode );
  p_iddbtool->dispGroup(par_dettype,par_bec,par_layer,par_ring,par_sector,
			par_rphidisp,par_rdisp,par_zdisp,par_systdisp,par_dispmode,par_irskip);
  //}
}

void InDetAlignWrt::dispFile() {
  ATH_MSG_DEBUG( 
    "::dispFile() : Initialise AlignableTransforms from parameters file "
    << par_dispfile );
  std::ifstream infile;
  infile.open(par_dispfile.c_str());
  // loop over lines in file
  int nline=0;
  std::string cline;
  int level,isyst,dettype,bec,layer,ring,sector;
  float rpd,rd,zd;
  while (infile >> level >> isyst >> dettype >> bec >> layer >> ring >> 
   sector >> rpd >> 
   rd >> zd) {
    ++nline;
    ATH_MSG_DEBUG( "Add displacement level " << level << " syst " << 
       isyst << " to [det,bec,layer] [" << dettype << "," << bec << "," << 
       layer << "] {rphi,r,z} {" << rpd << "," << rd << "," << zd << "}" );
    p_iddbtool->dispGroup(dettype,bec,layer,ring,sector,
        rpd,rd,zd,isyst,level,par_irskip);
  } // end of input loop
  ATH_MSG_DEBUG( "Read " << nline << 
     " displacement recipe lines" );
}

void InDetAlignWrt::doOutput() {
  // write all objects and fill IOV database 
  if (par_wrt) {
    // first sort, just in case
    p_iddbtool->sortTrans();
    if (par_wfile=="") {
      ATH_MSG_DEBUG( "doOutput for AlignableTransforms" );
      if (StatusCode::SUCCESS!=p_iddbtool->outputObjs()) 
        msg(MSG::ERROR) << "Write of AlignableTransforms fails" << endreq;
      if (par_wrtiov)
  p_iddbtool->fillDB(par_iovtag,par_run1,par_evt1,par_run2,par_evt2);
    } else {
      p_iddbtool->writeFile(par_ntuple,par_wfile);
    }
  }
  if (par_wrtbeam) {
    m_beamcondsvc->fillRec();
  }
  if (par_writetop) {
    genTopTrans();
  }
}

void InDetAlignWrt::genTopTrans() {
  // generate top level transforms into TDS
  ATH_MSG_DEBUG( "Initialise TOPTRANS from parameters file: " <<
     par_topname);
  DetCondKeyTrans* dkp=new DetCondKeyTrans();
  std::ifstream infile;
  infile.open(par_topname.c_str());
  // loop over all lines in file
  int nline=0;
  std::string cline,key;
  float xd,yd,zd,phi,theta,psi;
  while (infile >> key >> xd >> yd >> zd >> phi >> theta >> psi) {
    ++nline;
    ATH_MSG_DEBUG( "Add transform name: " << key << " x/y/z " << xd << "," << yd <<
       "," << zd << " phi/theta/psi " << phi << "," << theta << "," << psi );
    HepGeom::Transform3D newtrans=HepGeom::Transform3D(CLHEP::HepRotation(phi,theta,psi),
             CLHEP::Hep3Vector(xd,yd,zd));
    dkp->setTransform(key,newtrans);
  }
  ATH_MSG_DEBUG( "Read " << nline << " lines from recipe file" );
  // record object in TDS
  if (StatusCode::SUCCESS==detStore()->record(dkp,"/GLOBAL/GEOM/TOPTRANS")) {
    ATH_MSG_DEBUG( "Recorded TOPTRANS in TDS" );
  } else {
    ATH_MSG_DEBUG( "Failed to record TOPTRANS in TDS");
  }
}

void InDetAlignWrt::doMigration() {
  ATH_MSG_DEBUG( "Convert Pixel/SCT alignment from old to new format");
  // create new format database
  p_migratetool->createDB();
  // copy data from old to new database via temporary file
  p_iddbtool->writeFile(false,"migrate.dat");
  p_migratetool->readTextFile("migrate.dat");
  p_migratetool->writeFile(false,"migrate.check");
  // write data to POOL file and optionally IOV
  if (StatusCode::SUCCESS!=p_migratetool->outputObjs())
    msg(MSG::ERROR) << "Write of AlignableTransform fails" << endreq;
  if (par_wrtiov)
    p_migratetool->fillDB(par_iovtag,par_run1,par_evt1,par_run2,par_evt2);
}
