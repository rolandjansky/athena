/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKTrackFitterAlgo.h"
#include "TrigFTKSim/TrackFitter711.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRoadInput.h"
#include "TrigFTKSim/FTKTrackFileOutput.h"



#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <iostream>
#include <fstream>


/////////////////////////////////////////////////////////////////////////////
FTKTrackFitterAlgo::FTKTrackFitterAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tfpobj(0x0),
  m_roadMarketTool("FTK_RoadMarketTool/FTK_RoadMarketTool"),
  m_trackOutputTool("FTK_SGTrackOutput/FTK_SGTrackOutput"),
  m_SecondStageFit(false),
  m_IBLMode(0), m_fixEndcapL0(false),
  m_ITkMode(false),
  m_nbanks(0), m_nsubregions(1),
  m_verbosity(0),
  m_chi2cut(17),
  m_chi2cut_maj(14),
  m_chi2cut_vetmaj(-1),
  m_chi2dofcut(4),
  m_doAuxFW(false),
  m_HitWarrior(2),
  m_KeepRejected(0), 
  m_FitRemoved(0),
  m_DoMajority(1),
  m_OnePerRoad(0),
  m_HWNDiff(3),
  m_MaxNcomb(10000),
  m_MaxNhitsPerPlane(-1),
  m_MaxTrkout(2000000),
  m_NoRecoveryNHits(-1),
  m_SCTTrkMode(false),
  m_loadHWConf_path(""),
  m_SSF_TFMode(1),
  m_SSF_multiconn(0),
  m_SSF_maxnconn(999),
  m_SSF_allow_extramiss(0),
  m_SSF_check_TR_by(1),
  m_SSF_TR_min_eta(1.0),
  m_SSF_TR_max_eta(1.4),
  m_save_1stStageTrks(false),
  m_doTrackFile(false),
  m_addRoads(false),
  m_trackfilepath("./"), 
  m_trackfilename("ftktracks.root"),
  m_pmap(0x0),m_pmap_path(),m_pmap_complete(0x0),m_pmapcomplete_path(),m_pmap_unused(0x0),m_pmapunused_path(),
  m_rmap(0x0), m_rmap_unused(0x0),m_modulelut2nd_path(), 
  m_ssmap(0x0), m_ssmap_unused(0x0), 
  m_rmap_path(), m_ssmap_path(), m_ssmapunused_path(),
  m_AutoDisable(false)
{
  declareProperty("SecondStageFit",m_SecondStageFit,"Enable the second-stage fitter code");
  declareProperty("IBLMode",m_IBLMode,"Switch on the IBL layer");
  declareProperty("FixEndCapL0",m_fixEndcapL0, "Fix endcap L0 clustering");
  declareProperty("ITkMode",m_ITkMode,"Use ITk geometry, for Phase-II studies");
  declareProperty("NBanks",m_nbanks); // number of banks
  declareProperty("NSubRegions",m_nsubregions);
  declareProperty("Chi2Cut",m_chi2cut);
  declareProperty("Chi2Cut_Maj",m_chi2cut_maj);
  declareProperty("Chi2Cut_VetoMaj",m_chi2cut_vetmaj);
  declareProperty("Chi2DofCut",m_chi2dofcut);
  declareProperty("doAuxFW", m_doAuxFW);
  declareProperty("HitWarrior", m_HitWarrior);
  declareProperty("KeepRejected", m_KeepRejected);
  declareProperty("FitRemoved", m_FitRemoved);
  declareProperty("DoMajority",m_DoMajority);
  declareProperty("OnePerRoad",m_OnePerRoad);
  declareProperty("HWNDiff", m_HWNDiff);
  declareProperty("MaxNcomb", m_MaxNcomb);
  declareProperty("MaxNhitsPerPlane", m_MaxNhitsPerPlane);
  declareProperty("MaxTrkout", m_MaxTrkout);
  declareProperty("NoRecoveryNHits",m_NoRecoveryNHits);
  declareProperty("SCTTrkMode",m_SCTTrkMode);
  declareProperty("loadHWConf_path", m_loadHWConf_path);
  declareProperty("pmap_path",m_pmap_path);
  declareProperty("pmapcomplete_path",m_pmapcomplete_path);
  declareProperty("pmapunused_path",m_pmapunused_path);
  declareProperty("ModuleLUTPath2nd",m_modulelut2nd_path);
  declareProperty("fitconstantspath",m_fitconstantspath,"Array with bank paths");
  declareProperty("fit711constantspath",m_fit711constantspath,"Array with bank paths for 11L");
  declareProperty("sectorpath", m_sectorpath,"Array with sector paths for 11L");
  declareProperty("bankregion",m_bankregion,"Array of regions");
  declareProperty("banksubregion",m_banksubregion,"Array of subregions");
  declareProperty("doTrackFile",m_doTrackFile,"If true the old format file is generated");
  declareProperty("addRoads",m_addRoads);
  declareProperty("TrackFileName",m_trackfilename,"Name of the track output file, old format");
  declareProperty("OutDir",m_trackfilepath,"Path of the track output file, old format");
  declareProperty("rmap_path",m_rmap_path);
  declareProperty("ssmap_path",m_ssmap_path);
  declareProperty("ssmapunused_path",m_ssmapunused_path);
  declareProperty("TRACKFITTER_MODE",m_SSF_TFMode);
  declareProperty("Save1stStageTrks",m_save_1stStageTrks);
  declareProperty("SSFMultiConnection",m_SSF_multiconn);
  declareProperty("SSFNConnections",m_SSF_maxnconn);
  declareProperty("SSFAllowExtraMiss",m_SSF_allow_extramiss);
  declareProperty("SSFTRDefn",m_SSF_check_TR_by);
  declareProperty("SSFTRMinEta",m_SSF_TR_min_eta);
  declareProperty("SSFTRMaxEta",m_SSF_TR_max_eta);

}

FTKTrackFitterAlgo::~FTKTrackFitterAlgo()
{
  // destroy configuration
  if (m_ssmap) delete m_ssmap;
  if (m_ssmap_unused) delete m_ssmap_unused;
  if (m_rmap) delete m_rmap;
  if (m_rmap_unused) delete m_rmap_unused;
  if (m_pmap) delete m_pmap;
  if (m_pmap_unused) delete m_pmap_unused;
  if (m_pmap_complete) delete m_pmap_complete;
  if (m_tfpobj) delete m_tfpobj;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKTrackFitterAlgo::initialize(){
  MsgStream log(msgSvc(), name());
  FTKSetup::getFTKSetup().setMsgStream(&log);
  log << MSG::INFO << "FTKTrackFitterAlgo::initialize()" << endreq;

  log << MSG::INFO << "IBLMode value: " << m_IBLMode << endreq;
  FTKSetup::getFTKSetup().setIBLMode(m_IBLMode);

  log << MSG::INFO << "Fix EndcapL0 value: " << m_fixEndcapL0 << endreq;
  FTKSetup::getFTKSetup().setfixEndcapL0(m_fixEndcapL0);

  log << MSG::INFO << "ITkMode value: " << m_ITkMode << endreq;
  FTKSetup::getFTKSetup().setITkMode(m_ITkMode);
  
   if (m_SecondStageFit) 
    m_tfpobj = new TrackFitter711;
  else 
    m_tfpobj = new TrackFitter;

  // Look for the main plane-map and set plane map
  log << MSG::INFO << "Read the logical layer definitions" << endreq;
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "Main plane map definition missing" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Look for the unused plane-map 
  
  if(m_SecondStageFit){

    if (m_pmapunused_path.empty()) {
      log << MSG::FATAL << "Unused plane map definition missing" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      m_pmap_unused = new FTKPlaneMap(m_pmapunused_path.c_str());
      if (!(*m_pmap_unused)) {
        log << MSG::FATAL << "Error using plane map: " << m_pmapunused_path << endreq;
        return StatusCode::FAILURE;
      }
    }


  }


  log << MSG::INFO << "Read the pattern matching PMAP file: " << m_pmap_path << endreq;
  log << MSG::INFO << "Read the unused PMAP file: " << m_pmapunused_path << endreq;
  log << MSG::INFO << "Read the final PMAP file: " << m_pmapcomplete_path << endreq;

  log << MSG::INFO << "Read the tower-map file: " << m_rmap_path << endreq;
  log << MSG::INFO << "Read the SS definition file  : " << m_ssmap_path << endreq;
  log << MSG::INFO << "Read the extrapolation window definition: " << m_ssmapunused_path << endreq;




  if (m_SecondStageFit){
    log << MSG::INFO << "Read the 11lL pmap  file" << endreq;
    if (m_pmapcomplete_path.empty()) {
      log << MSG::FATAL << "11L plane map definition missing" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      m_pmap_complete = new FTKPlaneMap(m_pmapcomplete_path.c_str());
      if (!(*m_pmap_complete)) {
	log << MSG::FATAL << "Error using 11L plane map: " << m_pmapcomplete_path << endreq;
	return StatusCode::FAILURE;
      }
    }

    // Create the region map objects
    if (m_rmap_path.empty()) {
      log << MSG::FATAL << "Region map file needed" << endreq;
      return StatusCode::FAILURE;
    }
  
    log << MSG::INFO << "Creating region map" << endreq;
    m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
    if (!(*m_rmap)) {
      log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endreq;
      return StatusCode::FAILURE;
    }


    if (m_pmap_complete) {
      log << MSG::INFO << "Creating region map for the unused layers" << endreq;
      m_rmap_unused = new FTKRegionMap(m_pmap_complete, m_rmap_path.c_str());
      if( FTKSetup::getFTKSetup().getHWModeSS()==2) {
	if (m_modulelut2nd_path.empty()) {
	  log << MSG::FATAL << "A module LUT is required when HW SS calculation is required" << m_modulelut2nd_path << endreq;
	  return StatusCode::FAILURE;
	}
	else {
	  log << MSG::INFO << "Loading module map from: " << m_modulelut2nd_path << endreq;
	  m_rmap_unused->loadModuleIDLUT(m_modulelut2nd_path.c_str()); //"/afs/cern.ch/work/v/vcavalie/HWMODE2/InstallArea/share/ftk_configuration/map_files/raw_12LiblHW_32.moduleidmap");
	}
      }
    }


    log << MSG::INFO << "Read SS configurations" << endreq;
    log << MSG::INFO << "Read AM SS configuration" << endreq;
    m_ssmap = new FTKSSMap(m_rmap, m_ssmap_path.c_str(), false);
    if (!m_ssmapunused_path.empty()) {
      log << MSG::INFO << "Read SS configuration for layer not used in AM" << endreq;
      m_ssmap_unused = new FTKSSMap(m_rmap_unused, m_ssmapunused_path.c_str(), false);
  }

    dynamic_cast<TrackFitter711*>(m_tfpobj)->setPlaneMapIncomplete(m_pmap);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setPlaneMap(m_pmap_complete);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setSSMapIncomplete(m_ssmap);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setSSMapComplete(m_ssmap_unused);
  }
  else{
    m_tfpobj->setPlaneMap(m_pmap); 
  }



  // set Road Input Module
  //-- case 1 : input from I/O --
  FTKRoadInput *ftkinputmodule = m_roadMarketTool->inputReference();
  m_roadMarketTool->setTrackFitterReference(m_tfpobj);
  m_tfpobj->setRoadInputModule(ftkinputmodule);

  // set Track Output Module
  if (m_doTrackFile) {
    log << MSG::INFO << "Write the TF simulation results in an external file: " << m_trackfilename.c_str() << endreq;
    log << MSG::INFO << "Output Directory :" << m_trackfilepath.c_str() << endreq; // should be impremented -> each region output 0/xxx0.root ,1/xxx1.root,2,3...
    FTKTrackFileOutput *ftkouttrackmodule = new FTKTrackFileOutput();
    ftkouttrackmodule->setMultiOut(false);
    ftkouttrackmodule->setOutDir(m_trackfilepath.c_str());
    ftkouttrackmodule->setFileName(m_trackfilename.c_str());
    ftkouttrackmodule->setBranchFormat("FTKTracksStream%d.");
    m_tfpobj->setTrackOutputModule(ftkouttrackmodule);
  }
  else {
    StatusCode sc = m_trackOutputTool.retrieve();
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not retrieve FTK_SGTrackOutput tool" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Setting FTK_SGTrackOutput tool" << endreq;
    }

    m_tfpobj->setTrackOutputModule(m_trackOutputTool->reference());
  }
  


  // set path Hit Warrior configulation 
  log << MSG::INFO << "Load Hit Warrior Configulation " << endreq;
  if (m_loadHWConf_path.empty()) {
    log << MSG::FATAL << "HW Conf definition missing" << endreq;
    return StatusCode::FAILURE;
  }
  else {

    if (m_SecondStageFit){
      dynamic_cast<TrackFitter711*>(m_tfpobj)->setNCoordsPlanes(m_pmap->getTotalDim(),m_pmap->getNPlanes(),m_pmap_complete->getTotalDim(),m_pmap_complete->getNPlanes()); 
    }else{
      (m_tfpobj)->setNCoordsPlanes(m_pmap->getTotalDim(),m_pmap->getNPlanes());
    }
    log << MSG::INFO << "Load Hit Warrior Configulation " << endreq; 
    m_tfpobj->loadHWConf(m_loadHWConf_path.c_str());
  }

  
  
  
  // set parameter object to TrackFitter
  m_tfpobj->setChi2Cut(m_chi2cut);
  m_tfpobj->setHitWarrior(m_HitWarrior);
  m_tfpobj->setChi2Cut_maj(m_chi2cut_maj);
  m_tfpobj->setChi2Cut_vetomaj(m_chi2cut_vetmaj);
  m_tfpobj->setChi2DofCut(m_chi2dofcut);
  m_tfpobj->setKeepRejected(m_KeepRejected); 
  m_tfpobj->setFitRemoved(m_FitRemoved);
  m_tfpobj->setHWNDiff(m_HWNDiff);
  m_tfpobj->setMaxNcomb(m_MaxNcomb);
  m_tfpobj->setMaxNhitsPerPlane(m_MaxNhitsPerPlane);
  m_tfpobj->setMaxTrkout(m_MaxTrkout);

  m_tfpobj->setRequireFirst(0);
  m_tfpobj->setOnePerRoad(m_OnePerRoad);

  //std::cout << "chi2cut "        << m_tfpobj->getChi2Cut()         << std::endl;
  //std::cout << "hitwarr "        << m_tfpobj->getHitWarrior()      << std::endl;
  //std::cout << "chi2cutmaj "     << m_tfpobj->getChi2Cut_maj()     << std::endl;
  //std::cout << "chi2cutvetomaj " << m_tfpobj->getChi2Cut_vetomaj() << std::endl;
  //std::cout << "chi2dofcut "     << m_tfpobj->getChi2DofCut()      << std::endl;
  //std::cout << "hwndiff "        << m_tfpobj->getHWNDiff()         << std::endl;
  //std::cout << "MaxNComb "       << m_MaxNcomb                     << std::endl;
  //std::cout << "MaxTrkout "      << m_MaxTrkout                    << std::endl;
  //std::cout << "fitremoved "     << m_tfpobj->getFitRemoved()      << std::endl;

  //std::cout << "chi2cut "        << m_tfpobj->getChi2Cut()         << std::endl;
  //std::cout << "hitwarr "        << m_tfpobj->getHitWarrior()      << std::endl;
  //std::cout << "chi2cutmaj "     << m_tfpobj->getChi2Cut_maj()     << std::endl;
  //std::cout << "chi2cutvetomaj " << m_tfpobj->getChi2Cut_vetomaj() << std::endl;
  //std::cout << "chi2dofcut "     << m_tfpobj->getChi2DofCut()      << std::endl;
  //std::cout << "hwndiff "        << m_HWNDiff                      << std::endl;
  //std::cout << "MaxNComb "       << m_MaxNcomb                     << std::endl;
  //std::cout << "MaxTrkout "      << m_MaxTrkout                    << std::endl;
  //std::cout << "fitremoved "     << m_tfpobj->getFitRemoved()      << std::endl;


  
  //--- should add below options ---//
  //m_tfpobj->setNorecoveryNhits(0); 
  //m_tfpobj->setOnePerRoad(0);
  //m_tfpobj->setDoMajority(0);
  //m_tfpobj->setRequireFirst(0);

  m_tfpobj->setNorecoveryNhits(m_NoRecoveryNHits);

  if(m_SecondStageFit ){

    //    TrackFitter711 *realTF = dynamic_cast<TrackFitter711*>(m_tfpobj);

    if (m_SSF_TFMode == 3)
      dynamic_cast<TrackFitter711*>(m_tfpobj)->setSuperExtrapolateMode(true);
    else
      dynamic_cast<TrackFitter711*>(m_tfpobj)->setSuperExtrapolateMode(false);
    if (m_save_1stStageTrks)
      dynamic_cast<TrackFitter711*>(m_tfpobj)->setSaveIncompleteTracks(true);
    else
      dynamic_cast<TrackFitter711*>(m_tfpobj)->setSaveIncompleteTracks(false);

    dynamic_cast<TrackFitter711*>(m_tfpobj)->setUseSectorDB(true);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setUseMultipleConn(m_SSF_multiconn);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setUseNConn(m_SSF_maxnconn);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setAllowExtraMissInSuperExp(m_SSF_allow_extramiss);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setCheckTRby(m_SSF_check_TR_by);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setTransitionRegionMinEta(m_SSF_TR_min_eta);
    dynamic_cast<TrackFitter711*>(m_tfpobj)->setTransitionRegionMaxEta(m_SSF_TR_max_eta);

    log << MSG::INFO << "SSF allow miss = " << m_SSF_allow_extramiss << endreq;
    log << MSG::INFO << "SSF check TR by = " << m_SSF_check_TR_by << endreq;
    log << MSG::INFO << "SSF TR min eta = " << m_SSF_TR_min_eta << endreq;
    log << MSG::INFO << "SSF TR max eta = " << m_SSF_TR_max_eta << endreq;

    //std::cout << "m_SSF_multiconn: " << m_SSF_multiconn << std::endl;
    //std::cout << "m_SSF_maxconn: " << m_SSF_maxnconn << std::endl;
  }
  // set banks in case of region selection//
  m_tfpobj->prepareBanks(m_nbanks,m_nsubregions); //Set the below function ... to set the region Number

  if(m_fitconstantspath.size()==0){
    log << MSG::FATAL << "No Bank for input" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_SecondStageFit){
    if(m_fit711constantspath.size()==0){
      log << MSG::FATAL << "No 11 L Bank for input" << endreq;
      return StatusCode::FAILURE;
    }

  }

  if (m_nbanks!=m_tfpobj->getNumRegions() || m_nsubregions!=m_tfpobj->getNumSubRegions() ) {
    log << MSG::FATAL << "Wrong set Region or Subregion in TrackFitter" << endreq;
    return StatusCode::FAILURE;
  }


  unsigned int nbanks = m_fitconstantspath.size(); 

  if(nbanks!=m_bankregion.size() || nbanks!=m_banksubregion.size()){
    log << MSG::FATAL << "Different array property length in bank definitions" << endreq;
    return StatusCode::FAILURE;
  }



  for(unsigned int ibank=0;ibank!=nbanks;++ibank){
    
    int ir = m_bankregion[ibank];
    int is = m_banksubregion[ibank];
    const string &bankpath8L = m_fitconstantspath[ibank];
    
    log << MSG::INFO << "*** set bank *** region:" << ir << "\t subregions:" << is << "\t bank path:" << bankpath8L.c_str() << endreq;
    if(m_SecondStageFit){
      const string &bankpath11L = m_fit711constantspath[ibank];
      const string &sectorpath_s = m_sectorpath[ibank];
      log << MSG::INFO << "*** set bank *** region:" << ir << "\t subregions:" << is << "\t bank path:" << bankpath11L.c_str() << endreq;

      FTKConstantBank* bank8 = new FTKConstantBank(dynamic_cast<TrackFitter711*>(m_tfpobj)->getNCoordsIncomplete(),bankpath8L.c_str());
      bank8->doAuxFW(m_doAuxFW);

      FTKConstantBank* bank11 = new FTKConstantBank(dynamic_cast<TrackFitter711*>(m_tfpobj)->getNCoordsComplete(),bankpath11L.c_str());
      FTKSector711DB * sector= new FTKSector711DB(bank8->getNSectors(),m_pmap_complete->getNPlanes()-m_pmap->getNPlanes(),sectorpath_s.c_str());
      dynamic_cast<TrackFitter711*>(m_tfpobj)->setBank(ir,is,bank11,bank8, sector);
    }
    else{
      FTKConstantBank* bank8 = new FTKConstantBank(m_tfpobj->getNCoords(),bankpath8L.c_str());
      bank8->doAuxFW(m_doAuxFW);
      m_tfpobj->setBank(ir,is,bank8);

    }
       
  }
     

  m_tfpobj->init(); 

  /* if combined file with tracks and roads has been requested the roads
     can be connected at this time */
  if (m_doTrackFile && m_addRoads) {
    // produce a combined outptu file with both roads and tracks in different branches
    log << MSG::INFO << "The FTK roads will be added" << endreq;
    FTKTrackFileOutput *ftkouttrackmodule = dynamic_cast<FTKTrackFileOutput*>(m_tfpobj->getTrackOutputModule());    
    m_roadMarketTool->ConnectRoads(ftkouttrackmodule->getTree(),"FTKRoadsStream%d.",m_bankregion);
    
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKTrackFitterAlgo::execute() {
   MsgStream log(msgSvc(), name());
   FTKSetup::getFTKSetup().setMsgStream(&log);

   log << MSG::INFO << "FTKTrackFitterAlgo::execute() start" << endreq;

  if (m_AutoDisable) {
    // if AutoDisable feature is found all the execution driven by road-finder
    log << MSG::INFO << "FTKRoadFinderAlgo auto-disable mode, nothing to do" << endreq;
  }
  else {
    // elaborate the next event
   int res = m_tfpobj->nextEvent();
   
   if (res<0) {
     // make transparent the current algorithm
     m_AutoDisable = true;
   }
  }

  log << MSG::INFO << "FTKTrackFitterAlgo::execute() end" << endreq;
  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKTrackFitterAlgo::finalize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endreq;
    FTKSetup::getFTKSetup().setMsgStream(&log);

    // if the external custom track file is created this has to deleted
    m_tfpobj->getTrackOutputModule()->endFile();

    return StatusCode::SUCCESS;
}
