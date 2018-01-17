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
   m_par_create(true),
   m_par_wrt(false),
   m_par_wrtiov(true),
   m_par_print(false),
   m_par_migrate(false),
   m_par_rfile(""),
   m_par_wfile(""),
   m_par_dispfile(""),
   m_par_ntuple(false),
   m_par_wrtrun(-1),
   m_par_wrtevent(0),
   m_par_run1(IOVTime::MINRUN),
   m_par_run2(IOVTime::MAXRUN),
   m_par_evt1(IOVTime::MINEVENT),
   m_par_evt2(IOVTime::MAXEVENT),
   m_par_iovtag(""),
   m_par_dbtoolinst("InDetAlignDBTool"),
   m_par_dispmode(0),
   m_par_dettype(-1),
   m_par_bec(-1),
   m_par_layer(-1),
   m_par_ring(-1),
   m_par_sector(-1),
   m_par_rphidisp(0.1),
   m_par_rdisp(0.2),
   m_par_zdisp(0.3),
   m_par_systdisp(1),
   m_par_irskip(0),
   m_par_dispcsc(0),
   m_par_wrtbeam(false),
   m_par_writetop(false),
   m_par_topname("")
{
  
  // declare algorithm parameters
  declareProperty("BeamConditionService",m_beamcondsvc);   
  declareProperty("Create",m_par_create);
  declareProperty("Write",m_par_wrt);
  declareProperty("WriteIOV",m_par_wrtiov);
  declareProperty("PrintDB",m_par_print);
  declareProperty("Migrate",m_par_migrate);
  declareProperty("RFile",m_par_rfile);
  declareProperty("WFile",m_par_wfile);
  declareProperty("RWNtuple",m_par_ntuple);
  declareProperty("WriteRun",m_par_wrtrun);
  declareProperty("WriteEvent",m_par_wrtevent);
  declareProperty("ValidRun1",m_par_run1);
  declareProperty("ValidRun2",m_par_run2);
  declareProperty("ValidEvent1",m_par_evt1);
  declareProperty("ValidEvent2",m_par_evt2);
  declareProperty("IOVTag",m_par_iovtag);
  declareProperty("DBToolInst",m_par_dbtoolinst);
  declareProperty("DispMode",m_par_dispmode);
  declareProperty("DispDet",m_par_dettype);
  declareProperty("DispBec",m_par_bec);
  declareProperty("DispLayer",m_par_layer);
  declareProperty("DispRing",m_par_ring);
  declareProperty("DispSector",m_par_sector);
  declareProperty("DispRphi",m_par_rphidisp);
  declareProperty("DispR",m_par_rdisp);
  declareProperty("DispZ",m_par_zdisp);
  declareProperty("DispSyst",m_par_systdisp);
  declareProperty("DispFile",m_par_dispfile);
  declareProperty("RndmSkip",m_par_irskip);
  declareProperty("DispCSC",m_par_dispcsc);

  declareProperty("WriteBeamPos",m_par_wrtbeam);
  declareProperty("WriteTopTrans",m_par_writetop);
  declareProperty("TopTransFile",m_par_topname);

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
  if (m_par_migrate) {
    ATH_CHECK (p_migratetool.retrieve() );
  }
  // get BeamCondSvc if needed
  if (m_par_wrtbeam) {
    
    if ( m_beamcondsvc.retrieve().isFailure()){
           msg( MSG::ERROR) <<"Cannot get BeamCondSvc - beamspots will not be put in ntuple" << endmsg;
    }
  }
  
  if (msgLvl(MSG::DEBUG)) {
    if (m_par_create) msg() << "ID Alignment database structures will be created" << endmsg;
    if (m_par_rfile!="") {
      if (m_par_ntuple) msg() << "Alignment constants will be initialised from ntuple: " << m_par_rfile << endmsg;  
      else msg() << "Alignment constants will be initialised from text file" << m_par_rfile << endmsg;
    }
    if (m_par_dispfile!="") 
      msg() << "Alignment constants will be set according to recipe in file " << m_par_dispfile << endmsg;
    if (m_par_wrt) 
      msg() << "ID Alignment database structures will be written on run " 
      << m_par_wrtrun << " event " << m_par_wrtevent << endmsg;
    if (m_par_wfile!="") 
      msg() << "Structures will be written on file " << m_par_wfile << endmsg;
  if (m_par_wrtbeam) 
    msg() << "Beampos information will be written on run " << m_par_wrtrun 
    << " event " << m_par_wrtevent << endmsg;
  if (m_par_writetop) 
    msg() << "Toplevel transforms will be read from file " << m_par_topname 
    << "and written on run " << m_par_wrtrun << " event " << m_par_wrtevent << endmsg;
  if (!m_par_wrtiov) 
    msg() << "IOV writing will be suppressed - only output stream" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode InDetAlignWrt::execute() {

  if (!m_setup) {
    m_setup=true;
    if (m_par_create) {
      ATH_MSG_DEBUG( 
        "Creating null alignment database from geometry information in GeoModel");
      p_iddbtool->createDB();
    }
    if (m_par_rfile!="") {
      if (m_par_ntuple) {
	p_iddbtool->readNtuple(m_par_rfile);
      } else {
	p_iddbtool->readTextFile(m_par_rfile);
      }
    }
    if (m_par_dispmode>0 || m_par_dispcsc>0) DispDB();
    if (m_par_dispfile!="") dispFile();
    if (m_par_print) p_iddbtool->printDB(2);
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
  if ((m_par_wrt || m_par_wrtbeam || m_par_writetop) && 
      m_par_wrtrun==run && m_par_wrtevent==event) doOutput();

  return StatusCode::SUCCESS;
}


StatusCode InDetAlignWrt::finalize() {
  if (m_par_wrtrun==-1) doOutput();
  if (m_par_migrate) doMigration();
  return StatusCode::SUCCESS;
}

void InDetAlignWrt::DispDB() {
  // displace silicon modules according to recipe in DispMode
  // or use DispCSC if appropriate parameter is set
  //if (m_par_dispcsc!=0) {
  //  ATH_MSG_DEBUG("DispCSC called for mode " << m_par_dispcsc );
  //  p_iddbtool->dispCSC(m_par_dettype, m_par_bec, m_par_layer, m_par_ring,
  //    m_par_sector, m_par_dispcsc);
  //} else {
  ATH_MSG_DEBUG( "DispDB called for mode " << m_par_dispmode );
  p_iddbtool->dispGroup(m_par_dettype,m_par_bec,m_par_layer,m_par_ring,m_par_sector,
			m_par_rphidisp,m_par_rdisp,m_par_zdisp,m_par_systdisp,m_par_dispmode,m_par_irskip);
  //}
}

void InDetAlignWrt::dispFile() {
  ATH_MSG_DEBUG( 
    "::dispFile() : Initialise AlignableTransforms from parameters file "
    << m_par_dispfile );
  std::ifstream infile;
  infile.open(m_par_dispfile.c_str());
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
        rpd,rd,zd,isyst,level,m_par_irskip);
  } // end of input loop
  ATH_MSG_DEBUG( "Read " << nline << 
     " displacement recipe lines" );
}

void InDetAlignWrt::doOutput() {
  // write all objects and fill IOV database 
  if (m_par_wrt) {
    // first sort, just in case
    p_iddbtool->sortTrans();
    if (m_par_wfile=="") {
      ATH_MSG_DEBUG( "doOutput for AlignableTransforms" );
      if (StatusCode::SUCCESS!=p_iddbtool->outputObjs()) 
        msg(MSG::ERROR) << "Write of AlignableTransforms fails" << endmsg;
      if (m_par_wrtiov)
  p_iddbtool->fillDB(m_par_iovtag,m_par_run1,m_par_evt1,m_par_run2,m_par_evt2);
    } else {
      p_iddbtool->writeFile(m_par_ntuple,m_par_wfile);
    }
  }
  if (m_par_wrtbeam) {
    m_beamcondsvc->fillRec();
  }
  if (m_par_writetop) {
    genTopTrans();
  }
}

void InDetAlignWrt::genTopTrans() {
  // generate top level transforms into TDS
  ATH_MSG_DEBUG( "Initialise TOPTRANS from parameters file: " <<
     m_par_topname);
  DetCondKeyTrans* dkp=new DetCondKeyTrans();
  std::ifstream infile;
  infile.open(m_par_topname.c_str());
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
    msg(MSG::ERROR) << "Write of AlignableTransform fails" << endmsg;
  if (m_par_wrtiov)
    p_migratetool->fillDB(m_par_iovtag,m_par_run1,m_par_evt1,m_par_run2,m_par_evt2);
}
