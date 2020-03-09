/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibValidation/MdtValidationAlg.h"
#include "MdtCalibValidation/FixRtEnds.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibStl
#include "MuonCalibStl/ToString.h"

//MuonCalibDbOperations
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/CalibRtDbOperations.h"
#include "MuonCalibDbOperations/CalibT0DbOperations.h"
#include "MuonCalibDbOperations/CalibHeadOperations.h"

//MdtCalibData
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MdtCalibData/RtFullInfo.h"

//MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"


namespace MuonCalib {

MdtValidationAlg::MdtValidationAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), 
p_reg_sel_svc(NULL), m_tube_chamber(NULL),  m_db(NULL), m_t0_op(NULL), m_rt_op(NULL), m_head_ops(NULL),
m_fitter(1), m_writeToDbEnable(true), m_limitslevel(0), m_makeHistos(0), m_lastdate(0), 
m_validationTask(""), m_db_ConnectionString(""),m_db_WorkingSchema(""),m_defaultRtFile("./RT_default_comm.dat"),
m_minSegs(-1), m_minDAngle(-999.), m_Histos(NULL), m_HistosList(NULL),
m_lowrun(-1), m_uprun(-1), m_ComputeLimits(false), m_debug(-1), m_tube(-1), m_eta(-1), m_phi(-1)
{
  declareProperty("FitterFlag",m_fitter);
  declareProperty("WriteToDbEnable",m_writeToDbEnable);
  declareProperty("GetLimits",m_limitslevel);
  declareProperty("makeHistos",m_makeHistos);
  declareProperty("LastDate",m_lastdate); 
  declareProperty("ConnectionString",m_db_ConnectionString);
  declareProperty("WriterConnectionString",m_writer_ConnectionString);
  declareProperty("WorkingSchema",m_db_WorkingSchema);
  declareProperty("ValidationTask",m_validationTask);
  declareProperty("defaulRtFile", m_defaultRtFile);
  declareProperty("SiteName", m_sitename);
  m_headid=-1; //take most recent header
  declareProperty("HeadId", m_headid);
  m_minSlope=-9e9;
  declareProperty("MinSlope", m_minSlope);
  m_maxSlope=9e9;
  declareProperty("MaxSlope", m_maxSlope);
  m_minT0=-9e9;
  declareProperty("MinT0", m_minT0);
  m_maxT0=9e9;
  declareProperty("MaxT0", m_maxT0);
  m_minTmax=-9e9;
  declareProperty("MinTmax", m_minTmax);
  m_maxTmax=9e9;
  declareProperty("MaxTmax", m_maxTmax);
  m_minTtot=-9e9;
  declareProperty("MinTtot", m_minTtot);
  m_maxTtot=9e9;
  declareProperty("MaxTtot", m_maxTtot);
  m_minPP=-9e9;
  declareProperty("MinPP", m_minPP);
  m_maxPP=-9e9;
  declareProperty("MaxPP", m_maxPP);
  m_db=NULL;
  m_data_connection=NULL;
  declareProperty("ReaderAccount", m_reader_account);
  declareProperty("ReaderPassword", m_reader_password);
  declareProperty("WriterAccount", m_writer_account);
  declareProperty("WriterPassword", m_writer_password);
  m_t0.tv_sec = 0;
  m_t0.tv_usec = 0;
  m_t1.tv_sec = 0;
  m_t1.tv_usec = 0;
  m_result.tv_sec = 0;
  m_result.tv_usec = 0;
}

StatusCode MdtValidationAlg::initialize() {
  ATH_MSG_INFO("Thank you for using MdtValidationAlg!");

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(serviceLocator()->service("RegionSelectionSvc", p_reg_sel_svc));
  m_region_ids=p_reg_sel_svc->GetStationsInRegions();
  ATH_MSG_INFO(" MdtValidationAlg::initialize() - number of selected regions: "<<m_region_ids.size());
  gettimeofday(&m_t0,NULL);
  
  m_debug=0;
//db connection
  StatusCode sc=ConnectDb();
  m_head_ops = new CalibHeadOperations(*m_db);
  if(!sc.isSuccess()) return sc;
//get head id if not given
  if(m_headid<0) {
    m_headid=m_head_ops->GetLatestHeadId();
    if(m_headid<0) {
	ATH_MSG_FATAL("Cannot get head_id for '"<<m_sitename<<"'!");
	return StatusCode::FAILURE;
    }
  }
  m_data_connection=m_head_ops->GetDataConnection(m_headid, true, m_writer_ConnectionString, m_writer_account, m_writer_password);
  if(m_data_connection==NULL) {
    ATH_MSG_FATAL("Cannot get data connection!");
    return StatusCode::FAILURE;
  }
  m_data_connection->OpenConnection();
//database opertations classes
  m_t0_op=new CalibT0DbOperations(*m_data_connection);
  m_rt_op=new CalibRtDbOperations(*m_data_connection);

  ATH_MSG_INFO("Validating for header="<<m_headid<<"/"<<m_sitename);
  return StatusCode::SUCCESS;
}

StatusCode MdtValidationAlg::execute() {
  /** execute function */
  ATH_MSG_INFO(" MdtValidationAlg::execute ");

  m_ComputeLimits = 0;
  if (m_limitslevel) m_ComputeLimits = 1;
  
  //loop on all the ids in the selected calibration region
  for (std::vector<MuonCalib::NtupleStationId>::const_iterator it=m_region_ids.begin(); it!=m_region_ids.end(); it++) {
    if(m_db==NULL) {
      ATH_MSG_ERROR(" MdtValidationAlg::execute - no DB connection defined ");
      return StatusCode::FAILURE;
    }
    m_Histos = TFile::Open(((*it).regionId() + ".root").c_str(), "recreate");

    if(m_validationTask=="T0Val") { 
      /**  Manage Limits file and output RootFile */  
      m_chamber = it->regionId();
      m_eta     = it->GetEta();
      m_phi     = it->GetPhi();
	
      RootFile();
	
      ATH_MSG_INFO("Reading t0s for region " << it->regionId() <<"");
	  
      m_tube_chamber = m_t0_op->LoadT0Validation(*it, m_headid, m_sitename);
      if(m_tube_chamber==NULL) {
	ATH_MSG_FATAL("Cannot load t0s for "<<(*it).regionId());
	m_Histos->Close();
	return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Validating "<<m_tube_chamber->numMultilayers() * m_tube_chamber-> numLayers() * m_tube_chamber->numTubes() <<" tubes.");
      // end reading t0s //

      NtupleStationId id(*it);
      Histos(id);
      
// T0 Validation limits can be set in MuonCalibJobTransforms/share/skeleton.mdt_calib_validate.py, 
// computed from measurements, or read from a limits file
      if(m_ComputeLimits) {
	ComputeLimits();
	  // keep default limits 
      }
      ATH_CHECK(ValidateChamberT0(id));
      int success = -1;
      if (m_debug >0) SummaryReport(success);
      
      m_HistosList->Write();
      delete m_HistosList;

    } else if (m_validationTask=="RtVal"){
	m_minSegs = 1000;
	m_minDAngle = 0.0;
	ATH_CHECK(ValidateRegionRt(*it)); 
	
    } else {
	ATH_MSG_ERROR(" No ValidationTask selected");
	return StatusCode::FAILURE;
    }
    m_Histos->Close();
  }
  return StatusCode::SUCCESS;
}

StatusCode MdtValidationAlg::finalize() {
  /** finalize function */
  ATH_MSG_INFO(" MdtValidationAlg::finalize ");

  gettimeofday(&m_t1,NULL);
  timersub(&m_t1,&m_t0,&m_result); 
  ATH_MSG_INFO("ValidationAlg execution time= "<<m_result.tv_sec+m_result.tv_usec*0.000001<<" seconds");

  return StatusCode::SUCCESS;
}
 
StatusCode MdtValidationAlg::ConnectDb() {
  ATH_MSG_INFO("Connecting to "<<m_db_ConnectionString<<" "<<m_db_WorkingSchema);
  m_db=new CalibDbConnection(m_db_ConnectionString, m_db_WorkingSchema);
  m_db->SetLogin(m_reader_account, m_reader_password);
  if(!m_db->OpenConnection()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool MdtValidationAlg::RootFile(bool good_tubes) {
  if(!good_tubes) m_HistosList = new TList();
  TH1F *hslope;
  TH1F *ht0;
  TH1F *htmax;
  TH1F *httot;
  TH1F *hsummary;
  if(good_tubes) {
    hslope = new TH1F("hslope_gd","slope parameter", 90, m_minSlope, m_maxSlope);
    ht0    = new TH1F("ht0_gd","T0",800,m_minT0,m_maxT0);
    htmax  = new TH1F("htmax_gd", "Tmax", 800, m_minTmax, m_maxTmax);
    httot  = new TH1F("httot_gd", "Total Drift Length", 300, m_minTtot, m_maxTtot);
    hsummary = new TH1F("hsummary_gd", "Summary", 50, -0.5, 49.5);
  } else {
    hslope = new TH1F("hslope","slope parameter", 90, m_minSlope, m_maxSlope);
    ht0    = new TH1F("ht0","T0",800,m_minT0,m_maxT0);
    htmax  = new TH1F("htmax", "Tmax", 800, m_minTmax, m_maxTmax);
    httot  = new TH1F("httot", "Total Drift Length", 300, m_minTtot, m_maxTtot);
    hsummary = new TH1F("hsummary", "Summary", 50, -0.5, 49.5);
  }
  m_HistosList->Add(hslope) ;
  m_HistosList->Add(ht0) ;
  m_HistosList->Add(htmax) ;
  m_HistosList->Add(httot) ;
  m_HistosList->Add(hsummary);
  return true;
}

//  Read T0, Tmax, T0slope limits from a file.
//  NOT USED (commented out above)
bool MdtValidationAlg::ReadLimitsFile() {
  ToString ts;
  std::string prefix="Limits_" ;
  std::string suffix= ".dat" ;
  std::string chambName(m_chamber) ;
  std::string middle("_"+ts(m_phi)+"_"+ts(m_eta)) ;
  std::string limitsfile(prefix+chambName+middle+suffix) ;
  m_limitsFile = limitsfile ;

  std::ifstream LimitsFile(m_limitsFile.c_str());

  if (LimitsFile) {
    char option[20];
    char chComm[] = "#";
    std::string c;

    while( LimitsFile >> option) {
      ATH_MSG_INFO(" while while .... ");
      while (strncmp(option,chComm,1)==0) {
        ATH_MSG_INFO(" ignore  .... ");
	LimitsFile.ignore(500,'\n');
	LimitsFile>>option;
      }
      if (strncmp(option,"SLOPE",5)==0 ) {
	LimitsFile >> m_minSlope;
	LimitsFile >> m_maxSlope;
	ATH_MSG_INFO("Slope Limits : " <<m_minSlope << " -- " << m_maxSlope);
      }
      if (strncmp(option,"T0",2)==0 ) {
	LimitsFile >> m_minT0;
	LimitsFile >> m_maxT0;
	ATH_MSG_INFO("T0 Limits : " << m_minT0 << " -- "<< m_maxT0);
      }
      if (strncmp(option,"TMAX",4)==0 ) {
	LimitsFile >> m_minTmax;
	LimitsFile >> m_maxTmax;
	ATH_MSG_INFO("TMax Limits : "<< m_minTmax << " -- " << m_maxTmax);
      }
      if (strncmp(option,"TTOT",4)==0 ) {
	LimitsFile >> m_minTtot;
	LimitsFile >> m_maxTtot;
	ATH_MSG_INFO("TTot Limits : "<<m_minTtot<<" -- "<< m_maxTtot);
      }
      if (strncmp(option,"PP",2)==0 ) {
	LimitsFile >> m_minPP;
	LimitsFile >> m_maxPP;
	ATH_MSG_INFO("Peak / Plateau Limits : "<<m_minPP<<" -- "<< m_maxPP);
      }
     } 
    LimitsFile.close();
    ATH_MSG_INFO("LimitsFile closed : ");
    return true;
  }
  return false;
}

bool MdtValidationAlg::ComputeLimits() {

  MsgStream log(msgSvc(), name());

  //TH1F * hchamber = (TH1F *) m_HistosList->FindObject("hchamber") ;
  TH1F * ht0      = (TH1F *) m_HistosList->FindObject("ht0") ;
  TH1F * hslope   = (TH1F *) m_HistosList->FindObject("hslope") ;
  TH1F * htmax    = (TH1F *) m_HistosList->FindObject("htmax") ;
  TH1F * httot    = (TH1F *) m_HistosList->FindObject("httot") ;

  if (m_limitslevel == 10 ) {

     /*
     m_summary[20] = hslope->GetMean() ;
     m_summary[21] = hslope->GetRMS() ;

     m_summary[20] = hslope->GetMean() ;
     m_summary[21] = hslope->GetRMS() ;
     m_summary[22] = ht0->GetMean() ;
     m_summary[23] = ht0->GetRMS() ;
     m_summary[24] = htmax->GetMean() ;
     m_summary[25] = htmax->GetRMS() ;
     m_summary[26] = httot->GetMean() ;
     m_summary[27] = httot->GetRMS() ;
     */
    m_minSlope = hslope->GetMean() - 3.*hslope->GetRMS();
    m_maxSlope = hslope->GetMean() + 3.*hslope->GetRMS();
    m_minT0    = ht0->GetMean() - 3.*ht0->GetRMS();
    m_maxT0    = ht0->GetMean() + 3.*ht0->GetRMS();
    m_minTmax  = htmax->GetMean() - 3.*htmax->GetRMS();
    m_maxTmax  = htmax->GetMean() + 3.*htmax->GetRMS();
    m_minTtot  = httot->GetMean() - 3.*httot->GetRMS();
    m_maxTtot  = httot->GetMean() + 3.*httot->GetRMS();     
  }
  
  if (m_limitslevel>0 && m_limitslevel<10 ) {
    m_minSlope = getLeftLimit(hslope);
    m_maxSlope = getRightLimit(hslope);
    m_minTmax  = getLeftLimit(htmax);
    m_maxTmax  = getRightLimit(htmax);
    m_minTtot  = getLeftLimit(httot);
    m_maxTtot  = getRightLimit(httot);
    m_minT0    = getLeftLimit(ht0);
    m_maxT0    = getRightLimit(ht0);
  }
  m_minPP = 0. ;
  m_maxPP = 10000. ;

  log<<MSG::INFO<<"slope min/max "<<m_minSlope<<"/"<<m_maxSlope<<" Tmax min/max "<<m_minTmax <<"/"<<m_maxTmax<<" Ttot min/max "<<m_minTtot<<"/"<<m_maxTtot<<" T0 min/max "<<m_minT0<<"/"<<m_maxT0<<endmsg;

  return true;
}

bool MdtValidationAlg::Histos(NtupleStationId & id, const std::vector<int> & validflags){
     //TH1F * hchamber = (TH1F *) m_HistosList->FindObject("hchamber") ;
  TH1F *ht0;
  TH1F *hslope;
  TH1F *htmax;
  TH1F *httot;
  if(validflags.size()==0) {
    ht0    = (TH1F *) m_HistosList->FindObject("ht0");
    hslope = (TH1F *) m_HistosList->FindObject("hslope");
    htmax  = (TH1F *) m_HistosList->FindObject("htmax");
    httot  = (TH1F *) m_HistosList->FindObject("httot");
  } else {
    ht0    = (TH1F *) m_HistosList->FindObject("ht0_gd");
    hslope = (TH1F *) m_HistosList->FindObject("hslope_gd");
    htmax  = (TH1F *) m_HistosList->FindObject("htmax_gd");
    httot  = (TH1F *) m_HistosList->FindObject("httot_gd");
  }   	
     
  unsigned int count(0);
  for(unsigned int ml=0; ml<m_tube_chamber->numMultilayers(); ml++) {
    for(int ly=0; ly<static_cast<int>(m_tube_chamber->numLayers()); ly++) {
      for(int tb=0; tb<static_cast<int>(m_tube_chamber->numTubes()); tb++) {
//check if tube really exists	 
	if (!exists(id, ml, ly, tb)) continue;
	count++;
	if(count-1 < validflags.size()) {
	  if(validflags[count-1] != 3) continue;
	}
	ht0->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[4]);
	hslope->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[6]);
	htmax->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[5]);
	httot->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[5]-m_tube_chamber->getFit(ml, ly, tb)->par[4]);
      }  //end loop over tubes
    }    //end loop over layers
  }      //end loop over ML

  return true;
}

/////////////////////////////////////////////////////////////////////////////
StatusCode MdtValidationAlg::ValidateChamberT0(NtupleStationId & id) {
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering ValidateChamberT0" <<endmsg;
  std::vector<int> validflags;
  int validflag;
  bool all_ok(true);

  for(unsigned int ml=0; ml<m_tube_chamber->numMultilayers(); ml++) {
    for(unsigned int ly=0; ly<m_tube_chamber->numLayers(); ly++) {
      for(unsigned int tb=0; tb<m_tube_chamber->numTubes(); tb++) {
        if(!exists(id, ml, ly, tb)) {    //check for dummy tubes
	  log<<MSG::INFO<< " tube ("<<ml+1<<","<<ly+1<<","<<tb+1<<") is a dummy tube!"<<endmsg;
	  validflags.push_back(5);
	} else {
	  ValidateTubeT0(m_tube_chamber->getFit(ml, ly, tb), validflag);
	  validflags.push_back(validflag);
	  if(validflag !=3) {
	    all_ok = false;
	  }
	}
      }  //end loop over tubes
    }    //end loop over layers
  }      //end loop over ML
  if(!all_ok) {
//fill new histograms with good tubes only
    RootFile(true);
    Histos(id, validflags);
    int count(0);
    //for failed tubes set mean values
    for(unsigned int ml=0; ml<m_tube_chamber->numMultilayers(); ml++) {
      for(unsigned int ly=0; ly<m_tube_chamber->numLayers(); ly++) {
	for(unsigned int tb=0; tb<m_tube_chamber->numTubes(); tb++) {
	  if(validflags[count]!=3 && validflags[count]!=5) {
	    TH1F * ht0      = (TH1F *) m_HistosList->FindObject("ht0_gd");
	    TH1F * hslope   = (TH1F *) m_HistosList->FindObject("hslope_gd");
	    TH1F * htmax    = (TH1F *) m_HistosList->FindObject("htmax_gd");
	    m_tube_chamber->getFit(ml, ly, tb)->par[4]=ht0->GetMean();
	    m_tube_chamber->getFit(ml, ly, tb)->par[5]=htmax->GetMean();
	    m_tube_chamber->getFit(ml, ly, tb)->par[6]=hslope->GetMean();
	    m_tube_chamber->getCalib(ml, ly, tb)->t0=ht0->GetMean();
	    m_tube_chamber->getFit(ml, ly, tb)->group_by="VAL_FIX";
	    log<<MSG::INFO<< " tube ("<<ml+1<<","<<ly+1<<","<<tb+1<<") validated with flag = "<<validflag<<endmsg;
	  }
	  count++;
	}  //end loop over tubes
      }    //end loop over layers
    }      //end loop over ML
  }
  for(std::vector<int>::iterator it=validflags.begin(); it!=validflags.end(); it++) {
    ATH_MSG_INFO(*it<<" ");
  }
  if( m_writeToDbEnable ) {
    if(!m_t0_op->WriteT0Chamber(id, m_tube_chamber, validflags, m_headid, m_sitename)) {
      log<<MSG::FATAL<<" Cannot write Chamber!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
void MdtValidationAlg::ValidateTubeT0 (const MdtTubeFitContainer::SingleTubeFit *fit, int &validflag) {
/////////////////////////////////////////////////////////////////////////////

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering ValidateTubeT0" <<endmsg;

  int T0flag = CheckT0Parameters(fit);

  if (T0flag) {
    validflag=3;
  } else {
    validflag=4;
  }
}

/////////////////////////////////////////////////////////////////////////////
int MdtValidationAlg::CheckT0Parameters (const MdtTubeFitContainer::SingleTubeFit *fit) {
/////////////////////////////////////////////////////////////////////////////
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering CheckT0Parameters" <<endmsg;

  double t0    = fit->par[4];
  double slope = fit->par[6]; 
  double tmax  = fit->par[5];
  double ttot  = tmax-t0;

  bool bsl   = (slope >= m_minSlope && slope <= m_maxSlope );
  bool bt0   = (t0 >= m_minT0 && t0 <= m_maxT0);
  bool btmax = (tmax >= m_minTmax && tmax <= m_maxTmax);
  bool btt   = (ttot >= m_minTtot && ttot <= m_maxTtot);
//  bool bPP   = true ;    //what is this? get rid if it below

  int T0flag = 0;
  if (bsl && bt0 && btmax && btt ) {T0flag=1;}

  log<<MSG::INFO<<"Result of CheckT0Parameters: T0flag=" << T0flag <<" (T0flag=0 fail, =1 ok) T0_Limits="<<bt0<<" Slope_Limits="<<bsl<<" Tmax_Limits="<<btmax<<" Total_Drifttime_Limits="<<btt <<endmsg; 

  return T0flag;
}


/////////////////////////////////////////////////////////////////////////////
StatusCode MdtValidationAlg::ValidateRegionRt(const NtupleStationId & id) {
/////////////////////////////////////////////////////////////////////////////
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<< " Entering ValidateRegionRt" <<endmsg;
  
  RtFullInfo full_info;
  std::vector<SamplePoint> points_in;
	
  if(!m_rt_op->LoadRt(id, m_headid, false, m_sitename, points_in, &full_info)) {
    log<<MSG::FATAL<<"Cannot read chamber!"<<endmsg;
    return StatusCode::FAILURE;
  }
  std::vector<SamplePoint> points;
  for(std::vector<SamplePoint>::const_iterator it=points_in.begin(); it!=points_in.end(); it++) {
    if(it->x2()>-99.) points.push_back(*it);
  }
  /** any logic for rt validation!!!!!!!! */
  int good1=CheckRtParameters(points, &full_info);
//if rt relation is good
  if (good1==1) {
    if(m_writeToDbEnable) {
      if(!m_rt_op->SetValidflag(3)) {
	log<<MSG::FATAL<<"Cannot set validflag for rt!"<<endmsg;
	return StatusCode::FAILURE;
      }
    }
//rt is bad or has been fixed
  } else {
    //load default rt
    int v_flag=5;
    if(good1==0) {
      //open file
      std::ifstream infile(m_defaultRtFile.c_str());
      if(infile.fail()) {
	log << MSG::ERROR << "Cannot open file '" << m_defaultRtFile << "' for reading!" << endmsg;
	return StatusCode::FAILURE;
      }
      //sample points
      float t,r,s;
      std::string sdummy;
      float dummy(0);
      points.clear();
      //ignore the first line
      getline(infile, sdummy);
      //read table
      while (!(infile.eof() || infile.fail())) {
	infile >> r;
	if (infile.eof() || infile.fail() || dummy > 15.0) break;
	infile >> t; 
	infile >> s; 
	SamplePoint p(t,r,s);
	points.push_back(p);
      }
      v_flag=4;
    }
    if(m_writeToDbEnable) {
      if(!m_rt_op->SetValidflag(1, false)) {
	log << MSG::FATAL << "Cannot set validflag!"<<endmsg;
	return StatusCode::FAILURE;
      }
      full_info.setImplementation("Validation");
      if(!m_rt_op->WriteUpdateRt(id, m_headid, m_sitename, points, v_flag, &full_info)) {
	log << MSG::FATAL << "Cannot write rt-relation!"<<endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
int MdtValidationAlg::CheckRtParameters (std::vector<SamplePoint> & points, const RtFullInfo * full_info) {
/////////////////////////////////////////////////////////////////////////////

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering CheckRtParameters" <<endmsg;

  double nsegs    = full_info->numSeg();
  double aveangle = full_info->rmsAng();

  bool bseg       = ( nsegs >= m_minSegs );
  bool baveangle  = ( aveangle >= m_minDAngle );

  int RTflag = 0;
  if (bseg && baveangle ) {RTflag=1;}

  log<<MSG::INFO<<"Result of CheckRtParameters is: RTflag=" << RTflag <<" (RTflag=0 fail, =1 ok) Number_Segments_Check="<<bseg<<" Ave_Angle_Check="<<baveangle<<endmsg; 

  if(RTflag==0) {
    return 0;
  }

  FixRtEnds fix_end;
  int ret=fix_end.FixEnds(points);
  log<<MSG::INFO<<"Fix End points returned "<<ret<<endmsg;
  switch(ret) {
  case FixRtEnds::NO_FIX_NEEDED:
    return 1;
    break;
  case FixRtEnds::FIX_APPLIED:
    return 2;
    break;
  case FixRtEnds::FIX_FAILED:
    return 0;
  }
  return RTflag;
}
 
/////////////////////////////////////////////////////////////////////////////
void MdtValidationAlg::SummaryReport(int success) {
/////////////////////////////////////////////////////////////////////////////
  ATH_MSG_INFO("======================Validation Summary Report ==================");
  ATH_MSG_INFO(" Success in saving Calibration data is "<<success<<" (0=fail, 1=ok), chamber is " <<m_chamber);
}

double  MdtValidationAlg::getLeftLimit(TH1F * histo) { 
  int ibin;
  double left_integral;
  double left_limit=0;
  double  histoIntegral =  histo->Integral();
  int nbins = histo->GetNbinsX();

  double r_limit = .03;
  if (m_limitslevel==2){r_limit = .05;}
  if (m_limitslevel==3){r_limit = .1;}

  for(ibin=1;ibin<nbins;ibin++) {
    left_integral = histo->Integral(1,ibin);
    left_limit = histo->GetBinCenter(ibin);
    double r = left_integral/histoIntegral;
    if(r>r_limit) break;
  }

  return left_limit ;
}

double  MdtValidationAlg::getRightLimit(TH1F * histo) { 
  int ibin;
  double right_integral;
  double right_limit=0;
  double  histoIntegral =  histo->Integral();
  int nbins = histo->GetNbinsX();

  double r_limit = .03;
  if (m_limitslevel==2){r_limit = .05;}
  if (m_limitslevel==3){r_limit = .1;}
  for(ibin=nbins;ibin>0;ibin--) {
    right_integral = histo->Integral(ibin,nbins);
    right_limit = histo->GetBinCenter(ibin);
    double r = right_integral/histoIntegral;
    if(r>r_limit) break;
  }
  return right_limit ;
}

// Check if tube is a real or dummy tube (inserted as placeholder in COOL text blob)
inline bool MdtValidationAlg::exists(NtupleStationId &id, int ml, int ly, int tb) {

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return false; 
  } 

  if(!id.InitializeGeometry(m_idHelperSvc->mdtIdHelper(), MuonDetMgr)) {
    return false;
  }
  if(ml+1>id.NMultilayers()) {
    ATH_MSG_INFO(ml+1<<" "<<id.NMultilayers());	
    return false;
  }
  if(ly+1>id.NLayers(ml)) {
    ATH_MSG_INFO(ly+1<<" "<<id.NLayers(ml+1));
    return false;
  }
  if(tb+1>id.NTubes(ml)) {
    ATH_MSG_INFO(tb+1<<" "<<id.NTubes(ml+1));
    return false;
  }
  return true;
}
	
} //namespace MuonCalib
