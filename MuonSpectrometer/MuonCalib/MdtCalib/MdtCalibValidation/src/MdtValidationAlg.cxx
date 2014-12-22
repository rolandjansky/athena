/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//this
#include "MdtCalibValidation/MdtValidationAlg.h"
#include "MdtCalibValidation/FixRtEnds.h"

// geo model //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"


//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/NtupleStationId.h"
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

//root
#include "TRandom.h"


namespace MuonCalib {

  MdtValidationAlg::MdtValidationAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), 
  p_reg_sel_svc(NULL), m_tube_chamber(NULL),  m_detStore(NULL), m_MdtIdHelper(NULL),m_detMgr(NULL),  m_db(NULL), m_t0_op(NULL), m_rt_op(NULL), m_head_ops(NULL),
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
 
  MdtValidationAlg::~MdtValidationAlg()
  {

  }

  StatusCode MdtValidationAlg::initialize()
  {
    //greet user
    MsgStream log(messageService(), name());
    log<< MSG::INFO << "Thank you for using MdtValidationAlg!" <<endreq;
    StatusCode sc;

    //get geometry
    //retrieve detector store
    StoreGateSvc* m_detStore;
    sc = serviceLocator()->service("DetectorStore", m_detStore);
    if ( sc.isSuccess() ) 
      {
	log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
      } else {
	log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
	return sc;
      }
    //retrieve mdt id helper
    sc = m_detStore->retrieve(m_MdtIdHelper, "MDTIDHELPER" );
    if (!sc.isSuccess()) 
      {
	log << MSG::ERROR << "Can't retrieve MdtIdHelper" << endreq;
	return sc;
      }
    //retrieve detector manager
    sc = m_detStore->retrieve( m_detMgr );
    if (!sc.isSuccess()) 
      {
	log << MSG::ERROR << "Can't retrieve MuonDetectorManager" << endreq;
	return sc;
      }
    //get region selection service
    sc= serviceLocator()->service("RegionSelectionSvc", p_reg_sel_svc);
    if(!sc.isSuccess())
      {
	log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endreq;
	return sc;
      }
    m_region_ids=p_reg_sel_svc->GetStationsInRegions();
    log<< MSG::INFO << " MdtValidationAlg :: initialize() - number of selected regions: "<<m_region_ids.size()<<endreq;
		
	gettimeofday(&m_t0,NULL);

	m_debug=0;
//db connection
    sc=ConnectDb();
    m_head_ops = new CalibHeadOperations(*m_db);
    if(!sc.isSuccess()) return sc;
//get head id if not given
     if(m_headid<0)
     	{
	m_headid=m_head_ops->GetLatestHeadId();
	if(m_headid<0)
		{
		log<<MSG::FATAL<<"Cannot get head_id for '"<<m_sitename<<"'!"<<endreq;
		return StatusCode::FAILURE;
		}
	}
	m_data_connection=m_head_ops->GetDataConnection(m_headid, true, m_writer_ConnectionString, m_writer_account, m_writer_password);
	if(m_data_connection==NULL)
		{
		log<<MSG::FATAL<<"Cannot get data connection!"<<endreq;
		return StatusCode::FAILURE;
		}
	m_data_connection->OpenConnection();
//database opertations classes
	m_t0_op=new CalibT0DbOperations(*m_data_connection);
	m_rt_op=new CalibRtDbOperations(*m_data_connection);

     log<< MSG::INFO <<"Validating for header="<<m_headid<<"/"<<m_sitename<<endreq;
    return sc;
  }

  StatusCode MdtValidationAlg::execute()   
  {
    /** execute function */
    MsgStream log(messageService(), name());
    log<< MSG::INFO << " MdtValidationAlg :: execute "<<endreq;

    StatusCode sc;

    m_ComputeLimits = 0;
    if (m_limitslevel) m_ComputeLimits = 1;
    
    
    //loop on all the ids in the selected calibration region
    for (std::vector<MuonCalib::NtupleStationId>::const_iterator it=m_region_ids.begin(); it!=m_region_ids.end(); it++)
      {
	if(m_db==NULL)
		{
		  log<<MSG::ERROR<< " MdtValidationAlg::execute - no DB connection defined "<<endreq;
		  return StatusCode::FAILURE;
		}
	 m_Histos = TFile::Open(((*it).regionId() + ".root").c_str(), "recreate");
	if(m_validationTask=="T0Val"){ 
	  /**  Manage Limits file and output RootFile */  
	  m_chamber = it->regionId();
	  m_eta=it->GetEta();
	  m_phi=it->GetPhi();
	  
	  RootFile();
	  
	  log << MSG::INFO << "Reading t0s for region " << it->regionId() <<""<< endreq;
	  
	  m_tube_chamber=m_t0_op->LoadT0Validation(*it, m_headid, m_sitename);
	  if(m_tube_chamber==NULL)
	  	{
		log<<MSG::FATAL<<"Cannot load t0s for "<<(*it).regionId()<<endreq;
		m_Histos->Close();
		return StatusCode::FAILURE;
		}
	  log<<MSG::INFO<<"Validating "<<m_tube_chamber->numMultilayers() * m_tube_chamber-> numLayers() * m_tube_chamber->numTubes() <<"tubes."<<endreq;
	  // end reading t0s //


	  NtupleStationId id(*it);
	  Histos(id);
	  
	  if(m_ComputeLimits){
	    ComputeLimits();
	  } else {
	    //	  ReadLimitsFile();
	    // keep default limits 
	  }
	  sc=ValidateChamberT0(id);
	  int success = -1;
	  if (m_debug >0) SummaryReport(success);
	  
	  m_HistosList->Write();
	  delete m_HistosList;

	} else if (m_validationTask=="RtVal"){

	  m_minSegs = 1000;
	  m_minDAngle = 0.0;
	  
	  sc = ValidateRegionRt(*it); 
	  
	} else {

	  log<<MSG::ERROR<<" No ValidationTask selected" <<endreq;
	  return StatusCode::FAILURE;

	}
	m_Histos->Close();
      }
    return sc;
  }
  
  StatusCode MdtValidationAlg::finalize()
  {
    /** finalize functions */
    MsgStream log(messageService(), name());
    log<< MSG::INFO << " MdtValidationAlg :: finalize "<<endreq;

    gettimeofday(&m_t1,NULL);
    timersub(&m_t1,&m_t0,&m_result); 
    std::cout<<"ValidationAlg execution time= "<<m_result.tv_sec+m_result.tv_usec*0.000001<<" seconds"<<std::endl;

    if(!DisconnectDb()) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
 
  
StatusCode MdtValidationAlg::ConnectDb(){
MsgStream log(messageService(), name()); 
log<<MSG::INFO<<"Connecting to "<<m_db_ConnectionString<<" "<<m_db_WorkingSchema<<endreq;
  m_db=new CalibDbConnection(m_db_ConnectionString, m_db_WorkingSchema);
  m_db->SetLogin(m_reader_account, m_reader_password);
  if(!m_db->OpenConnection())
  	{
	return StatusCode::FAILURE;
	}
  return StatusCode::SUCCESS;
}

StatusCode MdtValidationAlg :: DisconnectDb()
        {
	if (m_db)
	  	{
		m_db->CloseConnection();
		delete m_db;
		}
	if(m_data_connection)
		{
		m_data_connection->CloseConnection();
		delete m_data_connection;
		}
	return StatusCode::SUCCESS;
	}
  
bool MdtValidationAlg::RootFile(bool good_tubes){

// 

//  if (m_Histos->IsZombie()) {
 //   std::cerr<<" Unable to open root file "<<std::endl;
  //  exit(1);
 // }
  if(!good_tubes)
	  m_HistosList = new TList();
/*
  m_minSlope  = 2.;
  m_maxSlope = 32.;
  m_minTmax  = 900.;
  m_maxTmax = 1700.;
  m_minTtot  = 600.;
  m_maxTtot = 900.;
  m_minT0  = 200.;
  m_maxT0   = 1000.;  
  m_minPP = 0. ;
  m_maxPP = 10000. ;*/
  /*
  TH1F *hslope = new TH1F("hslope","slope parameter",90,2.,32.);
  TH1F *ht0 = new TH1F("ht0","T0",800,200.,1000.);
  TH1F *htmax = new TH1F("htmax","Tmax",800,900.,1700.);
  TH1F *httot = new TH1F("httot","Total Drift Length",300,600.,900.);
  TH2F *hchamber = new TH2F("hchamber","Tube ValidFlagStatus",100,0.5,100.5,10,0.5,10.5);
  TH1F *hsummary = new TH1F("hsummary","Summary",50,-0.5,49.5);
  */
  TH1F *hslope;
  TH1F *ht0;
  TH1F *htmax;
  TH1F *httot;
  TH1F *hsummary;
  if(good_tubes)
  	{
	hslope = new TH1F("hslope_gd","slope parameter", 90, m_minSlope, m_maxSlope);
	ht0= new TH1F("ht0_gd","T0",800,m_minT0,m_maxT0);
	htmax = new TH1F("htmax_gd", "Tmax", 800, m_minTmax, m_maxTmax);
	httot = new TH1F("httot_gd", "Total Drift Length", 300, m_minTtot, m_maxTtot);
	hsummary = new TH1F("hsummary_gd", "Summary", 50, -0.5, 49.5);
	}
  else
  	{
	hslope = new TH1F("hslope","slope parameter", 90, m_minSlope, m_maxSlope);
	ht0= new TH1F("ht0","T0",800,m_minT0,m_maxT0);
	htmax = new TH1F("htmax", "Tmax", 800, m_minTmax, m_maxTmax);
	httot = new TH1F("httot", "Total Drift Length", 300, m_minTtot, m_maxTtot);
	hsummary = new TH1F("hsummary", "Summary", 50, -0.5, 49.5);
	}
  //TH2F *hchamber = new TH2F("hchamber","Tube ValidFlagStatus",100,0.5,100.5,10,0.5,10.5);
  m_HistosList->Add(hslope) ;
  m_HistosList->Add(ht0) ;
  m_HistosList->Add(htmax) ;
  m_HistosList->Add(httot) ;
  //m_HistosList->Add(hchamber);
  m_HistosList->Add(hsummary);
  return true;
}

bool MdtValidationAlg::ReadLimitsFile(){
  ToString ts;
  std::string prefix="Limits_" ;
  std::string suffix= ".dat" ;
  std::string chambName(m_chamber) ;
  std::string middle("_"+ts(m_phi)+"_"+ts(m_eta)) ;
  std::string limitsfile(prefix+chambName+middle+suffix) ;
  m_limitsFile = limitsfile ;

  //  for (int j=0; j<50; j++) m_summary[j]=0. ;

  std::ifstream LimitsFile(m_limitsFile.c_str());

  if (LimitsFile) {
    char option[20];
    char chComm[] = "#";
    std::string c;

    while( LimitsFile >> option) {
      std::cout << " while while .... " << std::endl ;
      while (strncmp(option,chComm,1)==0) {
        std::cout << " ignore  .... " << std::endl ;
	LimitsFile.ignore(500,'\n');
	LimitsFile>>option;
      }
      if (strncmp(option,"SLOPE",5)==0 ) {
	LimitsFile >> m_minSlope;
	LimitsFile >> m_maxSlope;
	std::cout << "Slope Limits : " <<m_minSlope << " -- " << m_maxSlope << std::endl;
      }
      if (strncmp(option,"T0",2)==0 ) {
	LimitsFile >> m_minT0;
	LimitsFile >> m_maxT0;
	std::cout << "T0 Limits : " << m_minT0 << " -- "<< m_maxT0<< std::endl;
      }
      if (strncmp(option,"TMAX",4)==0 ) {
	LimitsFile >> m_minTmax;
	LimitsFile >> m_maxTmax;
	std::cout << "TMax Limits : "<< m_minTmax << " -- " << m_maxTmax << std::endl;
      }
      if (strncmp(option,"TTOT",4)==0 ) {
	LimitsFile >> m_minTtot;
	LimitsFile >> m_maxTtot;
	std::cout << "TTot Limits : "<<m_minTtot<<" -- "<< m_maxTtot<< std::endl;
      }
      if (strncmp(option,"PP",2)==0 ) {
	LimitsFile >> m_minPP;
	LimitsFile >> m_maxPP;
	std::cout << "Peak / Plateau Limits : "<<m_minPP<<" -- "<< m_maxPP<< std::endl;
      }
     } 
    LimitsFile.close();
    std::cout << "LimitsFile closed : " << std::endl;
    return true;
  }
  return false;
}

bool MdtValidationAlg::ComputeLimits(){

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
     m_minT0 =    ht0->GetMean() - 3.*ht0->GetRMS();
     m_maxT0 =    ht0->GetMean() + 3.*ht0->GetRMS();
     m_minTmax =  htmax->GetMean() - 3.*htmax->GetRMS();
     m_maxTmax =  htmax->GetMean() + 3.*htmax->GetRMS();
     m_minTtot =  httot->GetMean() - 3.*httot->GetRMS();
     m_maxTtot =  httot->GetMean() + 3.*httot->GetRMS();     
  }

   if (m_limitslevel>0 && m_limitslevel<10 ) {

      m_minSlope  = getLeftLimit(hslope) ;
      m_maxSlope = getRightLimit(hslope) ;
      m_minTmax  = getLeftLimit(htmax) ;
      m_maxTmax = getRightLimit(htmax) ;
      m_minTtot  = getLeftLimit(httot) ;
      m_maxTtot = getRightLimit(httot) ;
      m_minT0  = getLeftLimit(ht0) ;
      m_maxT0   = getRightLimit(ht0) ;

  }
  m_minPP = 0. ;
  m_maxPP = 10000. ;

  log<<MSG::INFO<<"slope min/max "<<m_minSlope<<"/"<<m_maxSlope<<" Tmax min/max "<<m_minTmax <<"/"<<m_maxTmax<<" Ttot min/max "<<m_minTtot<<"/"<<m_maxTtot<<" T0 min/max "<<m_minT0<<"/"<<m_maxT0<<endreq;

  return true;
}

bool MdtValidationAlg::Histos(NtupleStationId & id, const std::vector<int> & validflags){
     //TH1F * hchamber = (TH1F *) m_HistosList->FindObject("hchamber") ;
     TH1F * ht0;
     TH1F * hslope;
     TH1F * htmax;
     TH1F * httot;
     if(validflags.size()==0)
     	{
	ht0 = (TH1F *) m_HistosList->FindObject("ht0");
	hslope = (TH1F *) m_HistosList->FindObject("hslope");
	htmax = (TH1F *) m_HistosList->FindObject("htmax");
	httot = (TH1F *) m_HistosList->FindObject("httot");
	}
     else
     	{
	ht0 = (TH1F *) m_HistosList->FindObject("ht0_gd");
	hslope = (TH1F *) m_HistosList->FindObject("hslope_gd");
	htmax = (TH1F *) m_HistosList->FindObject("htmax_gd");
	httot = (TH1F *) m_HistosList->FindObject("httot_gd");
	}
     	
     
     unsigned int count(0);
 //    	std::cout << "m_tube_chamber->numMultilayers()=" << m_tube_chamber->numMultilayers() << std::endl;
//	std::cout << "m_tube_chamber->numLayers()" << m_tube_chamber->numLayers() << std::endl;
//	std::cout << "m_tube_chamber->numTubes()" << m_tube_chamber->numTubes() << std::endl;
	for(unsigned int ml=0; ml<m_tube_chamber->numMultilayers(); ml++)
	for(int ly=0; ly<static_cast<int>(m_tube_chamber->numLayers()); ly++)
	for(int tb=0; tb<static_cast<int>(m_tube_chamber->numTubes()); tb++)
		{
//		std::cout<<"ml="<<ml<<" ly="<<ly<<" tb="<<tb<<std::endl;
//check if tube really exists	 
		if (!exists(id, ml, ly, tb)) continue;
		count++;
		if(count-1 < validflags.size())
			{
			if(validflags[count-1] != 3)
				continue;
			}
       //    MuonFixedId fixId((*tube)->tubeId());
//       		std::cout<<"<><><><><><><>"<<std::endl;
		ht0->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[4]);
		hslope->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[6]);
		htmax->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[5]);
		httot->Fill(m_tube_chamber->getFit(ml, ly, tb)->par[5]-m_tube_chamber->getFit(ml, ly, tb)->par[4]);
		}

     return true;
}

/////////////////////////////////////////////////////////////////////////////
StatusCode MdtValidationAlg::ValidateChamberT0(NtupleStationId & id) {
/////////////////////////////////////////////////////////////////////////////

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering ValidateChamberT0" <<endreq;
  std::vector<int> validflags;
  int validflag;
  bool all_ok(true);
  for(unsigned int ml=0; ml<m_tube_chamber->numMultilayers(); ml++)
  for(unsigned int ly=0; ly<m_tube_chamber->numLayers(); ly++)
  for(unsigned int tb=0; tb<m_tube_chamber->numTubes(); tb++)
  	{
        if(!exists(id, ml, ly, tb))
 		{
		log<<MSG::INFO<< " tube ("<<ml+1<<","<<ly+1<<","<<tb+1<<") is adummy!"<<endreq;
		validflags.push_back(5);
		}
	else 
		{
		ValidateTubeT0(m_tube_chamber->getFit(ml, ly, tb), validflag);
		validflags.push_back(validflag);
		if(validflag !=3)
			{
			all_ok = false;
			}
		}
	}
  	if(!all_ok)
		{
//fill new histograms with good tubes only
		RootFile(true);
		Histos(id, validflags);
		int count(0);
	//for failed tubes set mean values
		for(unsigned int ml=0; ml<m_tube_chamber->numMultilayers(); ml++)
		for(unsigned int ly=0; ly<m_tube_chamber->numLayers(); ly++)
		for(unsigned int tb=0; tb<m_tube_chamber->numTubes(); tb++)
  			{
			if(validflags[count]!=3 && validflags[count]!=5)
	   			{
				TH1F * ht0      = (TH1F *) m_HistosList->FindObject("ht0_gd");
				TH1F * hslope   = (TH1F *) m_HistosList->FindObject("hslope_gd");
				TH1F * htmax    = (TH1F *) m_HistosList->FindObject("htmax_gd");
				m_tube_chamber->getFit(ml, ly, tb)->par[4]=ht0->GetMean();
//				std::cout<<"CCccCC "<<m_tube_chamber->getFit(ml, ly, tb)->par[4]<<std::endl;
				m_tube_chamber->getFit(ml, ly, tb)->par[5]=htmax->GetMean();
//				std::cout<<"CCccCC "<<m_tube_chamber->getFit(ml, ly, tb)->par[4]<<std::endl;
				m_tube_chamber->getFit(ml, ly, tb)->par[6]=hslope->GetMean();
//				std::cout<<"CCccCC "<<m_tube_chamber->getFit(ml, ly, tb)->par[6]<<std::endl;
				m_tube_chamber->getCalib(ml, ly, tb)->t0=ht0->GetMean();
				m_tube_chamber->getFit(ml, ly, tb)->group_by="VAL_FIX";
				log<<MSG::INFO<< " tube ("<<ml+1<<","<<ly+1<<","<<tb+1<<") validated with flag = "<<validflag<<endreq;
				}
			count++;
			}
  	  	}
      for(std::vector<int> :: iterator it=validflags.begin(); it!=validflags.end(); it++)
      	{
	std::cout<<*it<<" ";
	}
	std::cout<<std::endl;
      if( m_writeToDbEnable )
	  	{
		if(!m_t0_op->WriteT0Chamber(id, m_tube_chamber, validflags, m_headid, m_sitename))
			{
			log<<MSG::FATAL<<" Cannot write Chamber!"<<endreq;
			return StatusCode::FAILURE;
			}
		}
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
void MdtValidationAlg::ValidateTubeT0 (const MdtTubeFitContainer::SingleTubeFit *fit, int & validflag) {
/////////////////////////////////////////////////////////////////////////////

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering ValidateTubeT0" <<endreq;

    int good1 = CheckT0Parameters(fit);

    if (good1) {

      validflag=3;
      return;

    } else {
      validflag=4;
      return;

    }
}

/////////////////////////////////////////////////////////////////////////////
int MdtValidationAlg::CheckT0Parameters (const MdtTubeFitContainer::SingleTubeFit *fit) {
/////////////////////////////////////////////////////////////////////////////
//return 1;
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Entering CheckT0Parameters" <<endreq;

  double t0    = fit->par[4];
  double slope = fit->par[6]; 
  double tmax = fit->par[5];
  double ttot = tmax-t0 ;

  bool bsl=(slope >= m_minSlope && slope <= m_maxSlope ) ;
  bool bt0=(t0 >= m_minT0 && t0 <= m_maxT0) ;
  bool btmax = (tmax >= m_minTmax && tmax <= m_maxTmax) ;
  bool btt = (ttot >= m_minTtot && ttot <= m_maxTtot) ;
  bool bPP = true ;

//  if (gRandom->Integer(2)==1)
 // 	return 0;

  int good = 0;
  if (bsl && bt0 && btmax && btt && bPP ) {good=1;}

  log<<MSG::INFO<<"Result of CheckT0Parameters is: good=" << good <<" (good=0 fail, =1 ok) "<<bsl<<" "<<bt0<<" "<<btmax<<" "<<btt<<" "<< bPP <<endreq; 

  return good;
}


/////////////////////////////////////////////////////////////////////////////
StatusCode MdtValidationAlg::ValidateRegionRt(const NtupleStationId & id) {
/////////////////////////////////////////////////////////////////////////////
	MsgStream log(msgSvc(), name());
	log<<MSG::INFO<< " Entering ValidateRegionRt" <<endreq;
  
	RtFullInfo full_info;
	std::vector<SamplePoint> points_in;
	
	if(!m_rt_op->LoadRt(id, m_headid, false, m_sitename, points_in, &full_info))
		{
		log<<MSG::FATAL<<"Cannot read chamber!"<<endreq;
		return StatusCode::FAILURE;
		}
	std::vector<SamplePoint> points;
	for(std::vector<SamplePoint>::const_iterator it=points_in.begin(); it!=points_in.end(); it++)
		{
		if(it->x2()>-99.)
			points.push_back(*it);
		}
	/** any logic for rt validation!!!!!!!! */
	int good1=CheckRtParameters(points, &full_info);
//rt relatino is good
	if (good1==1) 
		{
		if(m_writeToDbEnable)
			{
			if(!m_rt_op->SetValidflag(3))
				{
				log<<MSG::FATAL<<"Cannot set validflag for rt!"<<endreq;
				return StatusCode::FAILURE;
				}
			}
		}
//rt is bad or has been fixed
        else 
		{
	//load default rt
		int v_flag=5;
		if(good1==0)
			{
	    //open file
			std::ifstream infile(m_defaultRtFile.c_str());
			if(infile.fail())
				{
				log << MSG::ERROR << "Cannot open file '" << m_defaultRtFile << "' for reading!" << endreq;
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
			while (!(infile.eof() || infile.fail())) 
				{
				infile >> r;
				if (infile.eof() || infile.fail() || dummy > 15.0) break;
				infile >> t; 
				infile >> s; 
				SamplePoint p(t,r,s);
				points.push_back(p);
				}
			v_flag=4;
			}
	if(m_writeToDbEnable)
		{
		if(!m_rt_op->SetValidflag(1, false))
			{
			log << MSG::FATAL << "Cannot set validflag!"<<endreq;
			return StatusCode::FAILURE;
			}
		full_info.setImplementation("Validation");
    		if(!m_rt_op->WriteUpdateRt(id, m_headid, m_sitename, points, v_flag, &full_info))
			{
			log << MSG::FATAL << "Cannot write rt-relation!"<<endreq;
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
  log<<MSG::INFO<<"Entering CheckRtParameters" <<endreq;

  double nsegs = full_info->numSeg();
  double aveangle = full_info->rmsAng();

  bool bseg=(nsegs >= m_minSegs ) ;
  bool baveangle=(aveangle >= m_minDAngle ) ;

  int good = 0;
  if (bseg && baveangle ) {good=1;}

  log<<MSG::INFO<<"Result of CheckRtParameters is: good=" << good <<" (good=0 fail, =1 ok) "<<bseg<<" "<<baveangle<<endreq; 

  if(good==0)
  	{
	return 0;
	}

  FixRtEnds fix_end;
  int ret=fix_end.FixEnds(points);
 log<<MSG::INFO<<"Fix End points returned "<<ret<<endreq;
  switch(ret)
  	{
	case FixRtEnds::NO_FIX_NEEDED:
		return 1;
		break;
	case FixRtEnds::FIX_APPLIED:
		return 2;
		break;
	case FixRtEnds::FIX_FAILED:
		return 0;
	}
 return good;
}
 
/////////////////////////////////////////////////////////////////////////////
void MdtValidationAlg::SummaryReport(int success) {
/////////////////////////////////////////////////////////////////////////////
  std::cout << "======================Validation Summary Report =================="<<std::endl;
  std::cout << " Succes in saving Calibration data is "<<success<<" (0-fail, 1=ok), chamber is " <<m_chamber<<std::endl;
}

double  MdtValidationAlg::getLeftLimit(TH1F * histo) { 
  int ibin;
  double left_integral;
  double left_limit=0;
  double  histoIntegral =  histo->Integral();
  //double  histoMean =  histo->GetMean();
  //double  histoRMS =  histo->GetRMS();
  int nbins = histo->GetNbinsX();

  //std::cout << "  DEBUG MEAN and RMS : " << histoMean <<" "<< histoRMS << std::endl ;
  double r_limit = .03;
  if (m_limitslevel==2){r_limit = .05;}
  if (m_limitslevel==3){r_limit = .1;}
  //std::cout <<" "<<std::endl;
  //std::cout << "histo integral = "<<histoIntegral<<std::endl;

  for(ibin=1;ibin<nbins;ibin++){
    left_integral = histo->Integral(1,ibin);
    left_limit = histo->GetBinCenter(ibin);
    double r = left_integral/histoIntegral;
    //std::cout <<"bin,left_integral,left_limit,r= "<<ibin<<" "<<left_integral<<" "<<left_limit<<" "<<r<<std::endl;
    if(r>r_limit) break;
  }

  return left_limit ;
}

double  MdtValidationAlg::getRightLimit(TH1F * histo) { 
  int ibin;
  double right_integral;
  double right_limit=0;
  double  histoIntegral =  histo->Integral();
  //double  histoMean =  histo->GetMean();
  //double  histoRMS =  histo->GetRMS();
  int nbins = histo->GetNbinsX();

  //std::cout << "  DEBUG MEAN and RMS : " << histoMean <<" "<< histoRMS << std::endl ;
  double r_limit = .03;
  if (m_limitslevel==2){r_limit = .05;}
  if (m_limitslevel==3){r_limit = .1;}
  //std::cout <<" "<<std::endl;
  //std::cout << "histo integral = "<<histoIntegral<<std::endl;
  for(ibin=nbins;ibin>0;ibin--){
    right_integral = histo->Integral(ibin,nbins);
    right_limit = histo->GetBinCenter(ibin);
    double r = right_integral/histoIntegral;
    //std::cout <<"bin,left_integral,left_limit,r= "<<ibin<<" "<<right_integral<<" "<<right_limit<<" "<<r<<std::endl;
    if(r>r_limit) break;
  }
  return right_limit ;
}

inline bool  MdtValidationAlg :: exists(NtupleStationId & id, int ml, int ly, int tb)
	{
	if(!id.InitializeGeometry(m_MdtIdHelper, m_detMgr))
		{
		return false;
		}
	if(ml+1>id.NMultilayers())
		{
		std::cout<<ml+1<<" "<<id.NMultilayers()<<std::endl;	
		return false;
		}
	if(ly+1>id.NLayers(ml))
		{
		std::cout<<ly+1<<" "<<id.NLayers(ml+1)<<std::endl;
		return false;
		}
	if(tb+1>id.NTubes(ml))
		{
		std::cout<<tb+1<<" "<<id.NTubes(ml+1)<<std::endl;
		return false;
		}
	return true;
	}
	
	
} //namespace MuonCalib
