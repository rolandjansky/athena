/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCondInterface/IMultiLevelAlignmentDbTool.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"
#include "TrkAlignEvent/AlignParList.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "MuonAlignEvent/MdtAlignModule.h"
#include "MuonAlignGenTools/MuonAlignDBTool.h"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <map>
#include "Identifier/Identifier.h"

using namespace MuonGM;

namespace Muon {
  
  //________________________________________________________________________
  MuonAlignDBTool::MuonAlignDBTool(const std::string& type,
				   const std::string& name,
				   const IInterface* parent)
    : AthAlgTool(type,name,parent)
    , m_multilevelDbTool("MultiLevelAlignmentDbTool")
    , m_idTool("MuonCalib::IdToFixedIdTool")
    , m_alignModuleTool("Muon::MuonAlignModuleTool")
    , p_muonMgr(0)
    , m_ntuple(0)
    , m_rdbAccessSvc(0)
    , m_muonDetMgr(0)
    , m_alines(0)
    , m_detElemLookup_identifier(0)
    , m_detElemLookup_alignMod(0)
    , m_detElemLookup_detElem(0)
    , m_detElemLookup_alineIndex(0)
    , m_detElemLookup_stName(0)
    , m_detElemLookup_stPhi(0)
    , m_detElemLookup_stEta(0)
    , m_useMultiLevelDbTool(true)
  {
    
    declareInterface<ITrkAlignDBTool>(this);
    
    declareProperty("AlternateASZTFile", m_asztInputFile="aszt.txt");
    declareProperty("ASZTOutputFile",    m_asztOutputFile="aszt_out.txt");
    declareProperty("SigmaInputFile",    m_sigmaInputFile="");
    declareProperty("SigmaOutputFile",   m_sigmaOutputFile="sigmas_out.txt");
    declareProperty("AlignModuleTool",   m_alignModuleTool);
    //declareProperty("StoreL3Params",     m_storeL3Params=true);
    declareProperty("UseMultiLevelDbTool", m_useMultiLevelDbTool=true);
    declareProperty("MultiLevelAlignmentDbTool",  m_multilevelDbTool);
    
  }
  
  //________________________________________________________________________
  MuonAlignDBTool::~MuonAlignDBTool() 
  {

    delete m_detElemLookup_identifier;
    delete m_detElemLookup_alignMod;
    delete m_detElemLookup_detElem;
    delete m_detElemLookup_alineIndex;
   
  }
  
  //________________________________________________________________________
  StatusCode MuonAlignDBTool::initialize() 
  {
    ATH_MSG_INFO("initialize() of MuonAlignDBTool");  
    
    StatusCode sc;
    
    sc=detStore()->retrieve(p_muonMgr);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve MuonDetectorManager");
      return sc;
    }
    else ATH_MSG_DEBUG("retrieved MuonDetectorManager");
    
    if (m_useMultiLevelDbTool) {
      if (m_multilevelDbTool.retrieve().isSuccess())
	ATH_MSG_INFO("Retrieved " << m_multilevelDbTool);
      else {      
	ATH_MSG_WARNING("Could not get " << m_multilevelDbTool);
      }
    }

    if (m_idTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_idTool);
    else{
      ATH_MSG_FATAL("Could not get " << m_idTool); 
      return StatusCode::FAILURE;
    }
    
    if (m_alignModuleTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_alignModuleTool);
    else{
      ATH_MSG_FATAL("Could not get " << m_alignModuleTool); 
      return StatusCode::FAILURE;
    }
    
    IGeoModelSvc *geoModel;
    sc = service ("GeoModelSvc",geoModel);
    if (sc.isSuccess())
      ATH_MSG_INFO("Retrieved GeoModelSvc");
    else {
      ATH_MSG_FATAL("Couldn't get GeoModelSvc");
      return StatusCode::FAILURE;
    }

    std::string AtlasVersion = geoModel->atlasVersion();
    std::string MuonVersion  = geoModel->muonVersionOverride();
    m_detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
    m_detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";    
    sc = service("RDBAccessSvc",m_rdbAccessSvc);
    if (sc.isSuccess())
      ATH_MSG_INFO("Retrieved RDBAccessSvc");
    else {
      ATH_MSG_FATAL("Couldn't get RDBAccessSvc");
      return StatusCode::FAILURE;
    }
    
    // get muon detector manager
    StoreGateSvc* detStore = 0;
    if (StatusCode::SUCCESS != service("DetectorStore", detStore)) {
      ATH_MSG_FATAL("DetectorStore not found");
      return StatusCode::FAILURE;
    }
    if (StatusCode::SUCCESS != detStore->retrieve(m_muonDetMgr)) {
      ATH_MSG_FATAL("Couldn't retrieve muon detector manager");
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonAlignDBTool::finalize() 
  {  
    ATH_MSG_DEBUG("finalize() of MuonAlignDBTool");
    
    // delete ALines if created here
    if (m_alines != p_muonMgr->ALineContainer() && 
	m_alines != m_multilevelDbTool->level1Container()) {
      ATH_MSG_DEBUG("deleting ALines");
      delete m_alines;
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonAlignDBTool::preSolve()
  {
    StatusCode sc=getInitialALines();
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to get initial A-lines!");
      return StatusCode::FAILURE;
    }    
    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  StatusCode MuonAlignDBTool::getInitialALines()
  {
       
    using namespace MuonGM;
        
    ATH_MSG_DEBUG("alignModuleListType="<<m_alignModuleTool->alignModuleListType());

    if (m_alignModuleTool->alignModuleListType()==Trk::L3) {
      
      ATH_MSG_DEBUG("firstEventInitialize for L3");

      if (m_asztInputFile!="") { 
	ATH_MSG_DEBUG("asztInputFile: "<<m_asztInputFile);
	std::map<std::string, std::string> asciiFileDBMap;
	asciiFileDBMap.insert(std::make_pair("ASZT",m_asztInputFile));
	
	ATH_MSG_INFO("getting alines from ascii file");
	m_alines = asciiToALines(m_asztInputFile);
	ATH_MSG_DEBUG("ALines from ascii");      
      }
      else {
	m_alines = p_muonMgr->ALineContainer();
	ATH_MSG_DEBUG("ALines from muon manager, size "<<m_alines->size());      
      }

      createL3LookupTable();
    }
    else if (m_alignModuleTool->alignModuleListType()==Trk::L1) {
      ATH_MSG_DEBUG("firstEventInitialize for L1");
      m_alines = m_multilevelDbTool->level1Container();
      createL1LookupTable();
    }
    else if (m_alignModuleTool->alignModuleListType()==Trk::L2) {
      ATH_MSG_DEBUG("firstEventInitialize for L2.3");
      //ATH_MSG_ERROR("L2.3 is not yet implemented in the MultilevelDbTool");
      //return StatusCode::FAILURE;
      m_alines = m_multilevelDbTool->level23Container();
      createL23LookupTable();
    }
    else {
      ATH_MSG_ERROR("bad option -- should be L1, L2.3 or L3");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("returning from firstEventInitialize()");
    
    return StatusCode::SUCCESS;
  }
  
  //__________________________________________________________________________
  void MuonAlignDBTool::decomposeTransform(const Amg::Transform3D& /*transform*/, double* /*values*/) const
  { 
    ATH_MSG_ERROR("migrate decomposeTransform to Eigen");
    return;

    /*
    // copied from AlignModule 
    HepGeom::Scale3D scale; 
    HepGeom::Rotate3D   rotation; 
    HepGeom::Translate3D translation; 
    transform.getDecomposition(scale,rotation,translation); 
    
    values[0]=translation.dx(); 
    values[1]=translation.dy(); 
    values[2]=translation.dz(); 
    
    
    // a12 = cosbeta singamma     
    // a13 = -sinbeta 
    // a23 =  sinalpha cosbeta 
    
    double sinbeta = -rotation[0][2]; 
    double cosbeta = std::sqrt(1.-sinbeta*sinbeta); 
    
    double sinalpha = rotation[1][2]/cosbeta;     
    double singamma = rotation[0][1]/cosbeta; 
    
    values[3]=-std::asin(sinalpha); 
    values[4]=-std::asin(sinbeta); 
    values[5]=-std::asin(singamma); 
    
    return;
    */
  } 


  //________________________________________________________________________
  void MuonAlignDBTool::writeAlignPar()
  {
    ATH_MSG_ERROR("migrate MuonAlignDBTool::writeAlignPar() to Eigen");
    return;

    /*

    //using namespace CLHEP;

    long long int chamberId;
    int    nDoF,nTracks;
    double trackChi2;
    double init_tras, init_traz, init_trat;
    double init_rots, init_rotz, init_rott;
    double delta_tras, delta_traz, delta_trat;
    double delta_rots, delta_rotz, delta_rott;
    double err_tras, err_traz, err_trat;
    double err_rots, err_rotz, err_rott;
    double firstderiv_tras;
    double firstderiv_traz;
    double firstderiv_trat;
    double firstderiv_rots;
    double firstderiv_rotz;
    double firstderiv_rott;
    double secndderiv_tras;
    double secndderiv_traz;
    double secndderiv_trat;
    double secndderiv_rots;
    double secndderiv_rotz;
    double secndderiv_rott;

    ATH_MSG_DEBUG("creating alignPar tree");
    TTree* tree(0);
    if (m_ntuple) {

      m_ntuple->cd();

      std::string treename="alignPar";

      tree=new TTree(treename.c_str(),treename.c_str());

      tree->Branch("chamberId",       &chamberId,       "chamberId/L");

      tree->Branch("nDoF",            &nDoF,            "nDoF/I");
      tree->Branch("nTracks",         &nTracks,         "nTracks/I");
      tree->Branch("trackChi2",       &trackChi2,       "trackChi2/D");
      tree->Branch("delta_tras",      &delta_tras,      "delta_tras/D"); //in mm 
      tree->Branch("delta_traz",      &delta_traz,      "delta_traz/D"); 
      tree->Branch("delta_trat",      &delta_trat,      "delta_trat/D"); 
      tree->Branch("delta_rots",      &delta_rots,      "delta_rots/D"); //in rad
      tree->Branch("delta_rotz",      &delta_rotz,      "delta_rotz/D"); 
      tree->Branch("delta_rott",      &delta_rott,      "delta_rott/D"); 
      tree->Branch("err_tras",        &err_tras,        "err_tras/D"); //in mm 
      tree->Branch("err_traz",        &err_traz,        "err_traz/D"); 
      tree->Branch("err_trat",        &err_trat,        "err_trat/D"); 
      tree->Branch("err_rots",        &err_rots,        "err_rots/D"); //in rad
      tree->Branch("err_rotz",        &err_rotz,        "err_rotz/D"); 
      tree->Branch("err_rott",        &err_rott,        "err_rott/D"); 
      tree->Branch("init_tras",       &init_tras,       "init_tras/D");  //in mm 
      tree->Branch("init_traz",       &init_traz,       "init_traz/D");  
      tree->Branch("init_trat",       &init_trat,       "init_trat/D");  
      tree->Branch("init_rots",       &init_rots,       "init_rots/D");  //in rad
      tree->Branch("init_rotz",       &init_rotz,       "init_rotz/D"); 
      tree->Branch("init_rott",       &init_rott,       "init_rott/D"); 
      tree->Branch("firstderiv_tras", &firstderiv_tras, "firstderiv_tras/D");
      tree->Branch("firstderiv_traz", &firstderiv_traz, "firstderiv_traz/D");
      tree->Branch("firstderiv_trat", &firstderiv_trat, "firstderiv_trat/D");
      tree->Branch("firstderiv_rots", &firstderiv_rots, "firstderiv_rots/D");
      tree->Branch("firstderiv_rotz", &firstderiv_rotz, "firstderiv_rotz/D");
      tree->Branch("firstderiv_rott", &firstderiv_rott, "firstderiv_rott/D");
      tree->Branch("secndderiv_tras", &secndderiv_tras, "secndderiv_tras/D");
      tree->Branch("secndderiv_traz", &secndderiv_traz, "secndderiv_traz/D");
      tree->Branch("secndderiv_trat", &secndderiv_trat, "secndderiv_trat/D");
      tree->Branch("secndderiv_rots", &secndderiv_rots, "secndderiv_rots/D");
      tree->Branch("secndderiv_rotz", &secndderiv_rotz, "secndderiv_rotz/D");
      tree->Branch("secndderiv_rott", &secndderiv_rott, "secndderiv_rott/D");
    }
        
    const Trk::AlignModuleList* modList=
      m_alignModuleTool->getAlignModuleCollection();
    
    ALineMapContainer* newALines=new ALineMapContainer();

    int ialine(0);
    ATH_MSG_DEBUG("have "<<m_alines->size()<<" ALines");

    for (ciALineMap cialine = m_alines->begin(); 
	 cialine != m_alines->end(); ++cialine,ialine++) {

      
      if (m_alignModuleTool->alignModuleListType()==Trk::L1 &&
	  ialine>(int)modList->size()-1) {
	ATH_MSG_WARNING("more a-lines in input file than modules created in MuonGeometryManagerTool!");
	continue;
      }
      

      Identifier ALineId = (*cialine).first;
      ALinePar* ALine    = (*cialine).second;

      std::string name; int jff,jzz,obj;
      ALine->getAmdbId(name,jff,jzz,obj);

      ATH_MSG_DEBUG("ialine: "<<ialine<<", "<<name<<" "<<jff<<" "<<jzz);

      DataVector<Trk::AlignPar>* alignParVec=m_ALineContainerToAlignPar[ialine];

      if (!alignParVec) {
	float tras,traz,trat,rots,rotz,rott;
	ALine->getParameters(tras,traz,trat,rots,rotz,rott);
	
	ALinePar* newALine=new ALinePar();
	newALine->setAmdbId(name,jff,jzz,obj);

	if (m_alignModuleTool->alignModuleListType()==Trk::L1) {

	  double* initParsGF=new double[6];
	  initParsGF[0]=tras; initParsGF[3]=rots;
	  initParsGF[1]=traz; initParsGF[4]=rotz;
	  initParsGF[2]=trat; initParsGF[5]=rott;

	  double* initPars=new double[6];
	  globalToL1AlignFrame(m_ALineContainerToMod[ialine],initParsGF,initPars);

	  tras = initPars[0];
	  traz = initPars[1];
	  trat = initPars[2];
	  rots = initPars[0];
	  rotz = initPars[1];
	  rott = initPars[2];

	  delete [] initPars;
	  delete [] initParsGF;
	}

	newALine->setParameters(tras,traz,trat,
				rots,rotz,rott);
	ATH_MSG_DEBUG("created newALines with following params: ["<<
		      tras<<","<<traz<<","<<trat<<","<<
		      rots<<","<<rotz<<","<<rott<<"]");
	newALines->insert(std::make_pair(ALineId,newALine));
	continue;	
      }
      
      ATH_MSG_DEBUG("have alignParVec");
      
      chamberId=0;
      nTracks=0;
      nDoF=0;
      trackChi2=0.;

      //bool useAlignModuleTransform=false;
      bool useAlignModule=true;
      const MuonGM::MuonReadoutElement* muonRE(0);
      if (m_alignModuleTool->alignModuleListType()==Trk::L3) {
	
	Trk::AlignModule::DetectorType detType=Trk::AlignModule::NDetectorTypes;
	if (p_muonMgr->mdtIdHelper()->is_mdt(ALineId)) {
	  detType=Trk::AlignModule::MDT;
	  muonRE=p_muonMgr->getMdtReadoutElement(ALineId);
	}
	else if (p_muonMgr->mdtIdHelper()->is_mdt(ALineId)) {
	  detType=Trk::AlignModule::CSC;
	  muonRE=p_muonMgr->getCscReadoutElement(ALineId);	  
	}
	if (!muonRE) continue;
	
	// get AlignPar from AlignModuleTool (from current alignment job)
	m_alignModuleTool->fillDetAlignmentParameters(muonRE,alignParVec,detType);
	
	// get chamberId
	chamberId = 0;
	if (muonRE) chamberId = m_idTool->idToFixedId(muonRE->identifyATLAS()).getIdInt();
	if (useAlignModule) {
	  ATH_MSG_DEBUG("using chamberId for alignmodule");
	  const Trk::AlignModule* module=
	    m_alignModuleTool->findAlignModule(muonRE,detType);
	  ATH_MSG_DEBUG("module: "<<module);
	  if (!module) continue;	
	  chamberId = (long long int)module->identify().get_compact();
	  nTracks  = module->nTracks();
	  nDoF     = module->nDoF();
	  trackChi2= module->trackChi2();
	}
      }
      else {
	//useAlignModuleTransform=false;
	//useAlignModule=true;
	ATH_MSG_DEBUG("m_ALineContainerToModId["<<ialine<<"]:");
	        
        const Trk::AlignModule* module=0;
        if(ialine<(int)m_ALineContainerToMod.size()){
         module=m_ALineContainerToMod[ialine];
        }
        if(module){
          ATH_MSG_DEBUG("module name: "<<module->name());
          chamberId = m_idTool->idToFixedId(module->identify()).getIdInt();
          //chamberId = module->identify().get_compact();
          nTracks   = module->nTracks();
          nDoF      = module->nDoF();
          trackChi2 = module->trackChi2();
	}

        ATH_MSG_DEBUG("chamberId: "<<chamberId);
      }

      // get alignment parameters
      init_tras=delta_tras=err_tras=firstderiv_tras=secndderiv_tras=0.;
      init_traz=delta_traz=err_traz=firstderiv_traz=secndderiv_traz=0.;
      init_trat=delta_trat=err_trat=firstderiv_trat=secndderiv_trat=0.;
      init_rots=delta_rots=err_rots=firstderiv_rots=secndderiv_rots=0.;
      init_rotz=delta_rotz=err_rotz=firstderiv_rotz=secndderiv_rotz=0.;
      init_rott=delta_rott=err_rott=firstderiv_rott=secndderiv_rott=0.;
      
      if (alignParVec->size()>0) {
	if ((*alignParVec)[0]) {
	  init_tras      = (*alignParVec)[0]->initPar();
	  delta_tras     = (*alignParVec)[0]->par();
	  err_tras       = (*alignParVec)[0]->err();
	  firstderiv_tras= (*alignParVec)[0]->firstDeriv();
	  secndderiv_tras= (*alignParVec)[0]->secndDeriv();
	}
	if ((*alignParVec)[1]) {	
	  init_traz      = (*alignParVec)[1]->initPar();
	  delta_traz     = (*alignParVec)[1]->par();
	  err_traz       = (*alignParVec)[1]->err();
	  firstderiv_traz= (*alignParVec)[1]->firstDeriv();
	  secndderiv_traz= (*alignParVec)[1]->secndDeriv();
	}
	if ((*alignParVec)[2]) {
	  init_trat      = (*alignParVec)[2]->initPar();
	  delta_trat     = (*alignParVec)[2]->par();
	  err_trat       = (*alignParVec)[2]->err();
	  firstderiv_trat= (*alignParVec)[2]->firstDeriv();
	  secndderiv_trat= (*alignParVec)[2]->secndDeriv();
	}
	if ((*alignParVec)[3]) {
	  init_rots      = (*alignParVec)[3]->initPar();
	  delta_rots     = (*alignParVec)[3]->par();
	  err_rots       = (*alignParVec)[3]->err();
	  firstderiv_rots= (*alignParVec)[3]->firstDeriv();
	  secndderiv_rots= (*alignParVec)[3]->secndDeriv();
	}
	if ((*alignParVec)[4]) {
	  init_rotz      = (*alignParVec)[4]->initPar();
	  delta_rotz     = (*alignParVec)[4]->par();
	  err_rotz       = (*alignParVec)[4]->err();
	  firstderiv_rotz= (*alignParVec)[4]->firstDeriv();
	  secndderiv_rotz= (*alignParVec)[4]->secndDeriv();
	}
	if ((*alignParVec)[5]) {
	  init_rott      = (*alignParVec)[5]->initPar();
	  delta_rott     = (*alignParVec)[5]->par();
	  err_rott       = (*alignParVec)[5]->err();
	  firstderiv_rott= (*alignParVec)[5]->firstDeriv();
	  secndderiv_rott= (*alignParVec)[5]->secndDeriv();
	}
      }

      bool haveAlignPar=false;
      if (std::fabs(delta_tras-0.)>.001 ||
	  std::fabs(delta_traz-0.)>.001 ||
	  std::fabs(delta_trat-0.)>.001 ||
	  std::fabs(delta_rots-0.)>.001 ||
	  std::fabs(delta_rotz-0.)>.001 ||
	  std::fabs(delta_rott-0.)>.001 ) {
	haveAlignPar=true;
	ATH_MSG_DEBUG("init_tras="<<init_tras);
	ATH_MSG_DEBUG("init_traz="<<init_traz);
	ATH_MSG_DEBUG("init_trat="<<init_trat);
	ATH_MSG_DEBUG("init_rots="<<init_rots);
	ATH_MSG_DEBUG("init_rotz="<<init_rotz);
	ATH_MSG_DEBUG("init_rott="<<init_rott);
	ATH_MSG_DEBUG("delta_tras="<<delta_tras);
	ATH_MSG_DEBUG("delta_traz="<<delta_traz);
	ATH_MSG_DEBUG("delta_trat="<<delta_trat);
	ATH_MSG_DEBUG("delta_rots="<<delta_rots);
	ATH_MSG_DEBUG("delta_rotz="<<delta_rotz);
	ATH_MSG_DEBUG("delta_rott="<<delta_rott);
	ATH_MSG_DEBUG("firstderiv_tras="<<firstderiv_tras);
	ATH_MSG_DEBUG("firstderiv_traz="<<firstderiv_traz);
	ATH_MSG_DEBUG("firstderiv_trat="<<firstderiv_trat);
	ATH_MSG_DEBUG("firstderiv_rots="<<firstderiv_rots);
	ATH_MSG_DEBUG("firstderiv_rotz="<<firstderiv_rotz);
	ATH_MSG_DEBUG("firstderiv_rott="<<firstderiv_rott);
      }

      // add to newALines
      //std::string name; int jff,jzz,obj;
      //ALine->getAmdbId(name,jff,jzz,obj);
      //float tras,traz,trat,rots,rotz,rott;
      //ALine->getParameters(tras,traz,trat,rots,rotz,rott);

      ALinePar* newALine=new ALinePar();
      newALine->setAmdbId(name,jff,jzz,obj);
      
      HepGeom::Transform3D initParXform(HepGeom::TranslateX3D(init_tras)*
				  HepGeom::TranslateY3D(init_traz)*
				  HepGeom::TranslateZ3D(init_trat)*
				  HepGeom::RotateX3D(init_rots)*
				  HepGeom::RotateY3D(init_rotz)*
				  HepGeom::RotateZ3D(init_rott));
      
      HepGeom::Transform3D deltaParXform(HepGeom::TranslateX3D(delta_tras)*
				   HepGeom::TranslateY3D(delta_traz)*
				   HepGeom::TranslateZ3D(delta_trat)*
				   HepGeom::RotateX3D(delta_rots)*
				   HepGeom::RotateY3D(delta_rotz)*
				   HepGeom::RotateZ3D(delta_rott));
      
      HepGeom::Transform3D finalXform=deltaParXform*initParXform;
      double* finalPars=new double[6];
      decomposeTransform(finalXform,finalPars);
      newALine->setParameters(finalPars[0],finalPars[1],finalPars[2],
			      finalPars[3],finalPars[4],finalPars[5]);
      delete [] finalPars;

      if (haveAlignPar) {
	ATH_MSG_DEBUG(name<<" "<<jff<<" "<<jzz<<" "<<obj);
	ATH_MSG_DEBUG(std::setw(10)<<std::setprecision(5)<<
		      finalPars[0]<<" "<<finalPars[1]<<" "<<finalPars[2]);
	ATH_MSG_DEBUG(finalPars[3]<<" "<<finalPars[4]<<" "<<finalPars[5]);
      }
      newALines->insert(std::make_pair(ALineId,newALine));
      
      // fill tree for this chamber
      if (m_ntuple) tree->Fill();
    }
    
    if (m_ntuple) {
      tree->Write();
      delete tree;
    }

    writeALinesToAsciiFile(newALines, m_asztOutputFile);
    ATH_MSG_DEBUG("wrote ASZT values to "<<m_asztOutputFile);
    
    delete newALines;
    return;
    */
  }

  //________________________________________________________________________  
  MuonGM::DblQ00Aszt* MuonAlignDBTool::initSigmasFromAsciiFile() const
  {
    //using namespace CLHEP;
    
    if (m_sigmaInputFile=="") return 0;    
    
    std::map<std::string, std::string> asciiFileDBMap;
    asciiFileDBMap.insert(std::make_pair("ASZT",m_sigmaInputFile));
    
    ATH_MSG_INFO("getting sigmas from ascii file");     
    DblQ00Aszt* sigmas = new DblQ00Aszt(asciiFileDBMap.find("ASZT")->second);
    if (sigmas->size()==0){ 
      ATH_MSG_FATAL("Couldn't read sigmas from ascii file!");
        delete sigmas;
        return 0;
    }
    else ATH_MSG_INFO("N. of lines read = "<<sigmas->size());  
      
    return sigmas;
  }
  
  //________________________________________________________________________
  DblQ00Aszt* MuonAlignDBTool::initAlinesEqZero() const
  {
    ATH_MSG_DEBUG("in initAlinesEqZero");

    std::string filename="initAlines.txt";
    std::ofstream asztFile;
    asztFile.open(filename.c_str());
    const std::vector<Trk::AlignModule*>* alignModColl=
      m_alignModuleTool->getAlignModuleCollection();
    for (int j=0;j<(int)alignModColl->size();j++) {
      asztFile
	<<"A "
	<< (*alignModColl)[j]->name()        <<"       " 
	<< (*alignModColl)[j]->identifyHash() <<" " 
	<< (*alignModColl)[j]->identify()  <<" " 
	<< 0   <<" " // job
	<< 0.  <<" " // tras
	<< 0.  <<" " // traz?
	<< 0.  <<" " // trat
	<< 0.  <<" " // rots
	<< 0.  <<" " // rotz
	<< 0.  <<" " // rott
	<< "\n";
    }
    asztFile.close(); 
    ATH_MSG_DEBUG("did initAlinesEqZero");
    return new DblQ00Aszt(filename);
  }

  //________________________________________________________________________
  const Trk::TrkDetElementBase* MuonAlignDBTool::getDetElement(Identifier id) const
  {
    ATH_MSG_DEBUG("in getDetElement(Identifier)");

    if (!m_detElemLookup_identifier) setDetElemLookup();
    
    for (int i=0;i<(int)m_detElemLookup_identifier->size();i++) {
      if (id == (*m_detElemLookup_identifier)[i]) {
	
	ATH_MSG_DEBUG("getting MDT");
	const MuonGM::MdtReadoutElement* mdt=
	  dynamic_cast<const MuonGM::MdtReadoutElement*>((*m_detElemLookup_detElem)[i]);
	if (mdt) return mdt;
	
	ATH_MSG_DEBUG("no MDT, getting CSC");
	const MuonGM::CscReadoutElement* csc=
	  dynamic_cast<const MuonGM::CscReadoutElement*>((*m_detElemLookup_detElem)[i]);
	if (csc) return csc;
	
	ATH_MSG_WARNING("no MDT or CSC!");
	
      }
    }
    
    return 0;
  }
  
  //________________________________________________________________________
  void MuonAlignDBTool::setDetElemLookup() const
  {
    ATH_MSG_DEBUG("in setDetElemLookup");
    
    const Trk::AlignModuleList* alignModList=
      m_alignModuleTool->getAlignModuleCollection();
    
    ATH_MSG_DEBUG("m_detElemLookup_identifier="<<m_detElemLookup_identifier);
    if (!m_detElemLookup_identifier) {
      
      ATH_MSG_DEBUG("doing detector element lookup arrays");      
      m_detElemLookup_identifier = new std::vector<Identifier>;
      m_detElemLookup_alignMod   = new std::vector<const Trk::AlignModule*>;
      m_detElemLookup_detElem    = new std::vector<const Trk::TrkDetElementBase*>;
      m_detElemLookup_stEta      = new std::vector<int>;
      m_detElemLookup_stPhi      = new std::vector<int>;
      m_detElemLookup_stName     = new std::vector<std::string>;
      
      for (int imod=0;imod<(int)alignModList->size();imod++) {
	ATH_MSG_DEBUG("imod="<<imod<<"/"<<alignModList->size());
	const Trk::AlignModule* alignMod=(*alignModList)[imod];
	ATH_MSG_DEBUG("alignmod: "<<alignMod->identify());

	const std::vector<const Trk::TrkDetElementBase*>* detElementColl=
	  alignMod->detElementCollection(Trk::AlignModule::MDT);
	if (!detElementColl) 
	  detElementColl=alignMod->detElementCollection(Trk::AlignModule::CSC);

	if (!detElementColl) { 
	   ATH_MSG_WARNING("not MDT or CSC!");
	} else {
           unsigned int lastDetIdHash=99999999;
           for (int i=0;i<(int)detElementColl->size();i++) {
             ATH_MSG_DEBUG("idet="<<i<<"/"<<detElementColl->size());
             const MuonGM::MuonReadoutElement* muonRE=getMuonRE((*detElementColl)[i]);
             if (!muonRE) continue;       
             
             ATH_MSG_DEBUG("idHash="<<muonRE->identifyHash()<<", last "<<lastDetIdHash);
             
             if (lastDetIdHash!=muonRE->identifyHash()) {
               lastDetIdHash=muonRE->identifyHash();
               m_detElemLookup_identifier->push_back(muonRE->identify());
               m_detElemLookup_alignMod->push_back(alignMod);
               m_detElemLookup_detElem ->push_back(muonRE);	    
               m_detElemLookup_stEta   ->push_back(muonRE->getStationEta());
               m_detElemLookup_stPhi   ->push_back(muonRE->getStationPhi());
               m_detElemLookup_stName  ->push_back(std::string(muonRE->getStationName(),0,3));
             }
           }
        }

      }
      
      ATH_MSG_DEBUG("done");
      
      if (!m_detElemLookup_alineIndex && m_alines && m_alines->size()>0) {
	ATH_MSG_DEBUG("doing alineIndex lookup");
	m_detElemLookup_alineIndex = new std::vector<int>;
	
	for (int imod=0;imod<(int)alignModList->size();imod++) {
	  const Trk::AlignModule* alignMod=(*alignModList)[imod];
	  const std::vector<const Trk::TrkDetElementBase*>* detElementColl =  
	    alignMod->detElementCollection(Trk::AlignModule::MDT);
	  if (!detElementColl) detElementColl =
	    alignMod->detElementCollection(Trk::AlignModule::CSC);
	  if (!detElementColl) {
	    ATH_MSG_WARNING("no detElementColl for MDT or CSC");
	  } else {
	    unsigned int lastDetIdHash=0;
	    for (int i=0;i<(int)detElementColl->size();i++) {
	      const MuonGM::MuonReadoutElement* muonRE=getMuonRE((*detElementColl)[i]);
	    
	      if (!muonRE) continue;
	      ATH_MSG_DEBUG("idHash="<<muonRE->identifyHash()<<", last "<<lastDetIdHash);
	    
	      if (lastDetIdHash!=muonRE->identifyHash()) {
		lastDetIdHash=muonRE->identifyHash();
	      
		// now loop through ALines and get corresponding entry
		int ialine(0);
		ATH_MSG_DEBUG("have "<< m_alines->size()<<" ALines");
		for (ciALineMap cialine = m_alines->begin();
		     cialine != m_alines->end(); ++cialine,ialine++) {
		  Identifier ALineId = (*cialine).first;
		  if (ALineId == muonRE->identify()) {
		    m_detElemLookup_alineIndex->push_back(ialine);
		    ATH_MSG_DEBUG("have ALine");
		    break;
		  }
		}
	      }
	    }
	  }
	}
      }
      
      ATH_MSG_DEBUG("done with setDetElemLookup");
      return;    
    }
  }
  
  //________________________________________________________________________
  void MuonAlignDBTool::createL3LookupTable()
  {
    ATH_MSG_DEBUG("in createL3LookupTable");

    if (m_alines)
      ATH_MSG_DEBUG("have "<<m_alines->size()<<" A-lines");
    else {
      ATH_MSG_WARNING("no A-lines!");
      return;
    }

    int ialine(0);
    for (ciALineMap cialine = m_alines->begin(); 
	 cialine != m_alines->end(); ++cialine,ialine++) {
      ATH_MSG_DEBUG("ialine "<<ialine);
      
      Identifier ALineId = (*cialine).first;
      std::stringstream chname;
      ATH_MSG_DEBUG("ALineId: "<<ALineId);
      if (p_muonMgr->mdtIdHelper()->is_muon(ALineId)) {
	std::string stnamestr=p_muonMgr->mdtIdHelper()->stationNameString(p_muonMgr->mdtIdHelper()->stationName(ALineId));
	chname<<stnamestr;
	if (p_muonMgr->mdtIdHelper()->is_mdt(ALineId)) {
	  int stEta=p_muonMgr->mdtIdHelper()->stationEta(ALineId);
	  chname<<std::abs(stEta);
	  if (stEta<0) chname<<"C"; else chname<<"A";
	  int stPhi=p_muonMgr->mdtIdHelper()->stationPhi(ALineId);
	  int detphi=2*stPhi;
	  if (stnamestr.substr(2,1)=="L") detphi--;
	  else if (stnamestr.substr(2,1)!="S"&&stnamestr.substr(0,3)!="BEE") detphi=stPhi;
	  chname<<std::setfill('0')<<std::setw(2)<<detphi;
	  ATH_MSG_DEBUG("ALineId: "<<ALineId<<" is MDT "<<chname.str());
	}
	else if (p_muonMgr->cscIdHelper()->is_csc(ALineId)) {
	  ATH_MSG_DEBUG("ALineId: "<<ALineId<<" is CSC");
	}
	else if (p_muonMgr->tgcIdHelper()->is_tgc(ALineId)) {
	  ATH_MSG_DEBUG("ALineId: "<<ALineId<<" is TGC");
	}
	else if (p_muonMgr->rpcIdHelper()->is_rpc(ALineId)) {
	  ATH_MSG_DEBUG("ALineId: "<<ALineId<<" is RPC");
	}
      }
      else
	ATH_MSG_WARNING("ALineId "<<ALineId<<" not muon chamber!");

      // get MuonReadoutElement for this ALine
      const Trk::TrkDetElementBase* detElement=getDetElement(ALineId);
      if (!detElement) {
	ATH_MSG_DEBUG("no detElement, pushing back 0");
	m_ALineContainerToAlignPar.push_back(0);
	continue;
      }
            
      ALinePar* ALine = (*cialine).second;
      float s,z,t,rots,rotz,rott;
      ALine->getParameters(s,z,t,rots,rotz,rott);
      
      const int NPAR=6;
      float* initPars=new float[NPAR];
      initPars[0]=s;
      initPars[1]=z;
      initPars[2]=t;
      initPars[3]=rots;
      initPars[4]=rotz;
      initPars[5]=rott;
      
      float* initErrs=new float[NPAR];
      for (int i=0;i<NPAR;i++) {
	initErrs[i]=0.; 
	ATH_MSG_DEBUG("initPars["<<i<<"]="<<initPars[i]);
      }
      
      // get AlignModule for this detElement
      Trk::AlignModule::DetectorType detType=Trk::AlignModule::NDetectorTypes;
      if (dynamic_cast<const MdtReadoutElement*>(detElement))
	detType=Trk::AlignModule::MDT;
      else if (dynamic_cast<const CscReadoutElement*>(detElement))
	detType=Trk::AlignModule::CSC;
      else {
	ATH_MSG_WARNING("not MDT or CSC");
      }
     
      const Trk::AlignModule* alignModule=
	m_alignModuleTool->findAlignModule(detElement,detType);
      if (!alignModule) {
         ATH_MSG_WARNING("no AlignModule! (detType "<<detType<<", ID: "<<detElement->identify());
      } else {
         // get AlignPars for this AlignModule
         DataVector<Trk::AlignPar>* apVec=m_alignModuleTool->getFullAlignPars(alignModule);	
         if (apVec) {
           for (int i=0;i<(int)apVec->size();i++) {
             int param=(*apVec)[i]->paramType();
             (*apVec)[i]->setInitPar(initPars[param],initErrs[param]);
             ATH_MSG_DEBUG("setting initPar "<<initPars[param]<<" to "<<*(*apVec)[i]);
           }
           ATH_MSG_DEBUG("pushing back apVec");
           m_ALineContainerToAlignPar.push_back(apVec);
         }
         else {
           ATH_MSG_DEBUG("pushing back 0");
           m_ALineContainerToAlignPar.push_back(0);
         }
      }
      
      delete [] initPars;      
      delete [] initErrs;      
    }
  }

  //________________________________________________________________________
  void MuonAlignDBTool::createL1LookupTable()
  {
    ATH_MSG_DEBUG("in createL1LookupTable");

    // if no alines from MultiLevelAlignmentDbTool, create empty alines
    if (!m_alines || m_alines->size()<1) {

      ATH_MSG_DEBUG("creating A-lines for L1 with zeros");

      ALineMapContainer* alines=new ALineMapContainer();
      const std::vector<Trk::AlignModule*>* alignModColl=
	m_alignModuleTool->getAlignModuleCollection();
      ATH_MSG_DEBUG("alignModColl size(): "<<alignModColl->size());
      for (int j=0;j<(int)alignModColl->size();j++) {
	ATH_MSG_DEBUG("j="<<j);
	ATH_MSG_DEBUG("name: "<<(*alignModColl)[j]->name());
	ATH_MSG_DEBUG("idHash: "<<(*alignModColl)[j]->identifyHash());
	ATH_MSG_DEBUG("identify: "<<(*alignModColl)[j]->identify());
	ALinePar* newALine = new ALinePar();
	newALine->setAmdbId((*alignModColl)[j]->name(),
			    (*alignModColl)[j]->identifyHash(),
			    (*alignModColl)[j]->identify().get_identifier32().get_compact(),0);
	newALine->setParameters(0.,0.,0.,0.,0.,0.);
	newALine->isNew(true);
	Identifier id((*alignModColl)[j]->identify());	
	alines->insert(std::make_pair(id,(ALinePar*)newALine));
      }
      m_alines=alines;
    }

    const Trk::AlignModuleList* modList=
      m_alignModuleTool->getAlignModuleCollection();

    int ialine(0);
    for (ciALineMap cialine = m_alines->begin(); 
	 cialine != m_alines->end(); ++cialine,ialine++) {
      ATH_MSG_DEBUG("ialine "<<ialine);

      if (ialine>(int)modList->size()-1) {
	ATH_MSG_WARNING("more a-lines in input file than modules created in MuonGeometryManagerTool!");
	continue;
      }

      // get AlignModule from AlignModuleList
      const Trk::AlignModule* mod = (*modList)[ialine];
      ATH_MSG_DEBUG("mod id: "<<mod->identify());

      // get initial alignment parameters
      //Identifier ALineId = (*cialine).first;
      ALinePar* ALine = (*cialine).second;

      float tras,traz,trat,rots,rotz,rott;
      ALine->getParameters(tras,traz,trat,rots,rotz,rott);
			   
      
      ATH_MSG_INFO(std::setw(9)<<std::setprecision(5)<<"global frame tra s/z/t="<<
		   tras<<" "<<traz<<" "<<trat);
      ATH_MSG_INFO(std::setprecision(5)<<"global frame rot s/z/t="<<
		   rots<<" "<<rotz<<" "<<rott);

      const int NPAR=6;
      double* initParsGF=new double[NPAR];
      initParsGF[0]=tras; initParsGF[3]=rots;
      initParsGF[1]=traz; initParsGF[4]=rotz;
      initParsGF[2]=trat; initParsGF[5]=rott;

      double* initPars=new double[NPAR];
      globalToL1AlignFrame(mod,initParsGF,initPars);
      delete [] initParsGF;

      ATH_MSG_INFO(std::setw(10)<<std::setprecision(5)<<"align frame tra s/z/t="<<
		   initPars[0]<<" "<<
		   initPars[1]<<" "<<
		   initPars[2]);
      ATH_MSG_INFO(std::setw(10)<<std::setprecision(5)<<"align frame rot s/z/t="<<
		   initPars[3]<<" "<<
		   initPars[4]<<" "<<
		   initPars[5]);

      float* initErrs=new float[NPAR];
      for (int i=0;i<NPAR;i++) {
	initErrs[i]=0.;
	ATH_MSG_DEBUG("initPar["<<i<<"]="<<initPars[i]);
      }      

      // retrieve align pars from AlignModuleTool
      DataVector<Trk::AlignPar>* alignPars=
	m_alignModuleTool->getFullAlignPars((*modList)[ialine]);
      if (alignPars) {
	for (int i=0;i<(int)alignPars->size();i++) {
	  int param=(*alignPars)[i]->paramType();
	  (*alignPars)[i]->setInitPar(initPars[param],initErrs[param]);
	  ATH_MSG_DEBUG("set initPar["<<param<<"]="<<initPars[param]);
	}
	m_ALineContainerToAlignPar.push_back(alignPars);
      }
      else  {
	ATH_MSG_DEBUG("no alignPars, pushing back 0");
	m_ALineContainerToAlignPar.push_back(0);	
      }
      
      //m_ALineContainerToModId.push_back((long long int)(*modList)[ialine]->identify().get_compact());
      m_ALineContainerToMod.push_back((*modList)[ialine]);
      
      delete [] initPars;      
      delete [] initErrs;
    }
    return;
  }

 //________________________________________________________________________
  void MuonAlignDBTool::createL23LookupTable()
  {
    ATH_MSG_DEBUG("in createL23LookupTable");

    if (m_alines)
      ATH_MSG_DEBUG("have "<<m_alines->size()<<" A-lines");

    // if no alines from MultiLevelAlignmentDbTool, create empty alines
    if (!m_alines || m_alines->size()<1) {

      ATH_MSG_DEBUG("creating A-lines for L23 with zeros");

      ALineMapContainer* alines=new ALineMapContainer();
      const std::vector<Trk::AlignModule*>* alignModColl=
	m_alignModuleTool->getAlignModuleCollection();
      ATH_MSG_DEBUG("alignModColl size(): "<<alignModColl->size());
      for (int j=0;j<(int)alignModColl->size();j++) {
	ATH_MSG_DEBUG("j="<<j);
	ATH_MSG_DEBUG("name: "<<(*alignModColl)[j]->name());
	ATH_MSG_DEBUG("idHash: "<<(*alignModColl)[j]->identifyHash());
	ATH_MSG_DEBUG("identify: "<<(*alignModColl)[j]->identify());
	ALinePar* newALine = new ALinePar();
	newALine->setAmdbId((*alignModColl)[j]->name(),
			    (*alignModColl)[j]->identifyHash(),
			    (*alignModColl)[j]->identify().get_identifier32().get_compact(),0);
	newALine->setParameters(0.,0.,0.,0.,0.,0.);
	newALine->isNew(true);
	Identifier id((*alignModColl)[j]->identify());	
	alines->insert(std::make_pair(id,(ALinePar*)newALine));
      }
      m_alines=alines;
    }


    int ialine(0);
    for (ciALineMap cialine = m_alines->begin(); 
	 cialine != m_alines->end(); ++cialine,ialine++) {
      ATH_MSG_DEBUG("ialine "<<ialine);
      
      Identifier ALineId = (*cialine).first;

      // get TrkDetElementBase for this ALine
      const Trk::TrkDetElementBase* detElement=getDetElement(ALineId);
      if (!detElement) {
	ATH_MSG_DEBUG("no detElement, pushing back 0");
	m_ALineContainerToAlignPar.push_back(0);
        m_ALineContainerToMod.push_back(0);
	continue;
      }
      
      ALinePar* ALine = (*cialine).second;      
      float s,z,t,rots,rotz,rott;
      ALine->getParameters(s,z,t,rots,rotz,rott);
      
      const int NPAR=6;
      float* initPars=new float[NPAR];
      initPars[0]=s;
      initPars[1]=z;
      initPars[2]=t;
      initPars[3]=rots;
      initPars[4]=rotz;
      initPars[5]=rott;
      
      float* initErrs=new float[NPAR];
      for (int i=0;i<NPAR;i++) {
	initErrs[i]=0.; 
	ATH_MSG_DEBUG("initPars["<<i<<"]="<<initPars[i]);
      }
      
      // get AlignModule for this MDT
      const Trk::AlignModule* alignModule=
	m_alignModuleTool->findAlignModule(detElement,Trk::AlignModule::MDT);
      if (!alignModule){ 

        ATH_MSG_WARNING("no AlignModule for MDT");
        m_ALineContainerToMod.push_back(alignModule);

      } else {

         m_ALineContainerToMod.push_back(alignModule);
    
         DataVector<Trk::AlignPar>* apVec2=m_alignModuleTool->getAlignPars(alignModule);	
         ATH_MSG_DEBUG("name: " <<  alignModule->name());
         ATH_MSG_DEBUG("active pars: " << apVec2->size());

         // get AlignPars for this AlignModule
         DataVector<Trk::AlignPar>* apVec=m_alignModuleTool->getFullAlignPars(alignModule);	
         if (apVec) {
           for (int i=0;i<(int)apVec->size();i++) {
             int param=(*apVec)[i]->paramType();
             (*apVec)[i]->setInitPar(initPars[param],initErrs[param]);
             ATH_MSG_DEBUG("setting initPar "<<initPars[param]<<" to "<<*(*apVec)[i]);
           }
           ATH_MSG_DEBUG("pushing back apVec");
           m_ALineContainerToAlignPar.push_back(apVec);
         }
         else {
           ATH_MSG_DEBUG("pushing back 0");
           m_ALineContainerToAlignPar.push_back(0);
         }
      }
      
      delete [] initPars;      
      delete [] initErrs;

    }

    return;
  }



  //________________________________________________________________________
  void MuonAlignDBTool::writeALinesToAsciiFile(const ALineMapContainer* alines, 
					      const std::string& outputfile) const
  {

    std::ofstream alineFile;
    alineFile.open(outputfile.c_str());

    const Trk::AlignModuleList* modList=
      m_alignModuleTool->getAlignModuleCollection();
    
    int ialine(0);
    for (ciALineMap cialine = alines->begin(); 
	 cialine != alines->end(); ++cialine,ialine++) {
      //Identifier ALineId = (*cialine).first;
      ALinePar* ALine = (*cialine).second;
      std::string name; int jff(0),jzz(0),obj(0);
      float tras(0.),traz(0.),trat(0.),rots(0.),rotz(0.),rott(0.);
      ALine->getAmdbId(name,jff,jzz,obj);
      ALine->getParameters(tras,traz,trat,rots,rotz,rott);
      
      ATH_MSG_DEBUG(name<<" "<<jff<<" "<<jzz<<" "<<obj);
      ATH_MSG_INFO(std::setw(10)<<std::setprecision(5)<<
		   "align frame tra s/z/t="<<tras<<" "<<traz<<" "<<trat);
      ATH_MSG_INFO(std::setw(10)<<std::setprecision(5)<<
		   "align frame rot s/z/t="<<rots<<" "<<rotz<<" "<<rott);

      // convert from align frame to global frame

      if (m_alignModuleTool->alignModuleListType()==Trk::L1) {

	const Trk::AlignModule* mod = (*modList)[ialine];
	double* initPars=new double[6];
	initPars[0]=tras; initPars[3]=rots;
	initPars[1]=traz; initPars[4]=rotz;
	initPars[2]=trat; initPars[5]=rott;
		
	double* finalParsGF=new double[6];
	level1AlignToGlobalFrame(mod,initPars,finalParsGF);

	tras=finalParsGF[0]; rots=finalParsGF[3];
	traz=finalParsGF[1]; rotz=finalParsGF[4];
	trat=finalParsGF[2]; rott=finalParsGF[5];

	delete [] finalParsGF;
	delete [] initPars;

	ATH_MSG_INFO(std::setw(9)<<std::setprecision(5)<<
		     "global frame tra s/z/t="<<tras<<" "<<traz<<" "<<trat);
	ATH_MSG_INFO(std::setprecision(5)<<
		     "global frame rot s/z/t="<<rots<<" "<<rotz<<" "<<rott);
	
      }

      alineFile
	<< "A "
	<< name <<" "
	<< jff  <<" "
	<< jzz  <<" "
	<< obj  <<" "
	<< tras <<" "
	<< traz <<" "
	<< trat <<" "
	<< rots <<" "
	<< rotz <<" "
	<< rott <<"\n";      
    }
    alineFile.close();
    return;
  }

  //________________________________________________________________________
  ALineMapContainer* MuonAlignDBTool::asciiToALines(const std::string& inputfile) 
  {
    // this method is used for L3 only!

    ATH_MSG_DEBUG("in asciiToALines, inputfile "<<inputfile);

    ALineMapContainer* alines=new ALineMapContainer();
    std::ifstream alineFile(inputfile.c_str());
    if (!alineFile.is_open())
      ATH_MSG_ERROR("bad ascii file: "<<inputfile);

    char ALineMarker;
    std::string name; int jff(0),jzz(0),obj(0);
    float tras(0.),traz(0.),trat(0.),rots(0.),rotz(0.),rott(0.);

    while (alineFile
	   >> ALineMarker
	   >> name
	   >> jff
	   >> jzz
	   >> obj
	   >> tras
	   >> traz
	   >> trat
	   >> rots
	   >> rotz
	   >> rott
	   ) {

      ATH_MSG_DEBUG(name<<" "<<jff<<" "<<jzz<<" "<<obj<<" "
		    <<tras<<" "<<traz<<" "<<trat<<" "
		    <<rots<<" "<<rotz<<" "<<rott);
      
      bool isValid;
      Identifier id=p_muonMgr->mdtIdHelper()->elementID(name,jzz,jff,
							true,&isValid);
      if (!isValid) continue; 
      
      const MuonGM::MuonReadoutElement* muonRE(0);
      if (p_muonMgr->mdtIdHelper()->is_mdt(id))
	muonRE=p_muonMgr->getMdtReadoutElement(id);
      else if (p_muonMgr->cscIdHelper()->is_csc(id))
	muonRE=p_muonMgr->getCscReadoutElement(id);	
      if (!muonRE) continue;
      
      ATH_MSG_DEBUG("stPhi/stEta="<<muonRE->getStationPhi()<<"/"<<muonRE->getStationEta());
      
      ATH_MSG_DEBUG("found mdt");
      ALinePar* newALine = new ALinePar();
      newALine->setAmdbId(name,jff,jzz,obj);
      newALine->setParameters(tras,traz,trat,rots,rotz,rott);
      alines->insert(std::make_pair(muonRE->identify(),newALine));
    }
    
    alineFile.close();
    
    return alines;    
  }
  
  //________________________________________________________________________
  void MuonAlignDBTool::globalToL1AlignFrame(const Trk::AlignModule* /*mod*/,
					     double* /*globalPars*/, double* /*alignPars*/) const
  {

    ATH_MSG_ERROR("migrate  MuonAlignDBTool::globalToL1AlignFrame to Eigen");
    return;
    /*
    HepGeom::Transform3D parsGlobalFrame(HepGeom::TranslateX3D(globalPars[0])*
					 HepGeom::TranslateY3D(globalPars[1])*
					 HepGeom::TranslateZ3D(globalPars[2])*
					 HepGeom::RotateX3D   (globalPars[3])*
					 HepGeom::RotateY3D   (globalPars[4])*
					 HepGeom::RotateZ3D   (globalPars[5]));
    
    HepGeom::Transform3D initParsAlignFrame = mod->globalFrameToAlignFrame()*parsGlobalFrame*
      mod->globalFrameToAlignFrame().inverse(); 
    decomposeTransform(initParsAlignFrame,alignPars);
    */
    return;
  }

  //________________________________________________________________________
  void MuonAlignDBTool::level1AlignToGlobalFrame(const Trk::AlignModule* /*mod*/,
						 double* /*alignPars*/, double* /*alignParsGF*/) const
  {

    ATH_MSG_ERROR("migrate  MuonAlignDBTool::level1AligntoGlobalFrame to Eigen");
    return;

    /*
    ATH_MSG_DEBUG("in level1AlignToGlobalFrame:");
    ATH_MSG_DEBUG("alignPars tra: "<<alignPars[0]<<" "<<alignPars[1]<<" "<<alignPars[2]);
    ATH_MSG_DEBUG("alignPars rot: "<<alignPars[3]<<" "<<alignPars[4]<<" "<<alignPars[5]);

    HepGeom::Transform3D parsAlignFrame(HepGeom::TranslateX3D(alignPars[0])*
				  HepGeom::TranslateY3D(alignPars[1])*
				  HepGeom::TranslateZ3D(alignPars[2])*
				  HepGeom::RotateX3D   (alignPars[3])*
				  HepGeom::RotateY3D   (alignPars[4])*
				  HepGeom::RotateZ3D   (alignPars[5]));
    
    HepGeom::Transform3D parsGlobalFrame = mod->globalFrameToAlignFrame().inverse()*parsAlignFrame*
      mod->globalFrameToAlignFrame(); 
    decomposeTransform(parsGlobalFrame,alignParsGF);

    ATH_MSG_DEBUG("alignParsGF tra: "<<alignParsGF[0]<<" "<<alignParsGF[1]<<" "<<alignParsGF[2]);
    ATH_MSG_DEBUG("alignParsGF rot: "<<alignParsGF[3]<<" "<<alignParsGF[4]<<" "<<alignParsGF[5]);
    */

    return;
  }

  //________________________________________________________________________
  const MuonGM::MuonReadoutElement* MuonAlignDBTool::getMuonRE(const Trk::TrkDetElementBase* detElement) const 
  {
    const MuonGM::MuonReadoutElement* muonRE=
      dynamic_cast<const MuonGM::MdtReadoutElement*>(detElement);	
    if (!muonRE)
      muonRE=dynamic_cast<const MuonGM::CscReadoutElement*>(detElement);
    if (!muonRE)
      ATH_MSG_WARNING("muonRE not MDT or CSC!");
    return muonRE;
  }
  
} // end namespace
