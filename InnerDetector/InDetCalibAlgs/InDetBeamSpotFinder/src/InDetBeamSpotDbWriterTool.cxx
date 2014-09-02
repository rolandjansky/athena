/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"

//#include "InDetBeamSpotFinder/InDetBeamSpotDbWriterTool.h"
#include "InDetBeamSpotDbWriterTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/ToolHandle.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "BeamSpotID.h"

#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "InDetBeamSpotFinder/BeamSpotStatusCode.h"

using namespace InDet;


InDetBeamSpotDbWriterTool::InDetBeamSpotDbWriterTool(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type,name,parent)
  //  ,m_regsvc("IOVRegistrationSvc",name)
  ,m_regsvc(0)
  ,m_toolSvc("ToolSvc",name)
  ,m_aspec(0),m_root_bs(0)
{
  declareInterface<IInDetBeamSpotWriterTool>(this);

  //declareProperty("IOVRegistrationSvc",m_regsvc);
  declareProperty("ToolSvc", m_toolSvc);
  declareProperty("BeamPosFolder", m_beamposFolder="/Indet/Beampos");
  declareProperty("Tag",m_tagID="");
  
  declareProperty("WriteROOT",m_doRoot = true);
  declareProperty("RootFileName", m_root_filename = "beamspot.root");
  declareProperty("TreeName", m_root_treename = "COOLBeamspot");
  declareProperty("DirName", m_root_dir = "Beamspot");
  

  declareProperty("WriteCOOL",m_doCOOL = false);
  declareProperty("WriteFromTimeStamp",m_doTimeStampWrite = false);      
  declareProperty("WriteFromEvent",m_doEventWrite = false);
  declareProperty("WriteFailed",m_writeFailed = false);
  
  declareProperty("UseLBFromAccepted",m_setLBwithAcceptedEvents=false);
  declareProperty("UseLBFromViewed",  m_setLBwithViewedEvents  =false);

  declareProperty("StoreErrors",  m_storeErrors  =true);
}


StatusCode InDetBeamSpotDbWriterTool::initialize() {
  msg(MSG::DEBUG) << "In initialize" << endreq;

  if ( m_toolSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_toolSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_toolSvc << endreq;
  
  if (m_doCOOL) {
    if (StatusCode::SUCCESS!=service("IOVRegistrationSvc",m_regsvc)) {
      msg(MSG::FATAL) << "IOVRegistrationSvc not found" << endreq;
      return StatusCode::FAILURE;
    }else {
      msg(MSG::DEBUG) << "IOVRegistrationSvc found" << endreq;
    }
    
    /*
      if ( m_regsvc.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_regsvc << endreq;
      return StatusCode::FAILURE;
      } else 
      msg(MSG::INFO) << "Retrieved service " << m_regsvc << endreq;
    */
    
    //create the AttributeListSpecification for the beamspot
    m_aspec =  new coral::AttributeListSpecification();
    m_aspec->extend("status","int");
    m_aspec->extend("posX","float");
    m_aspec->extend("posY","float");
    m_aspec->extend("posZ","float");
    m_aspec->extend("sigmaX","float");
    m_aspec->extend("sigmaY","float");
    m_aspec->extend("sigmaZ","float");
    m_aspec->extend("tiltX","float");
    m_aspec->extend("tiltY","float");
    m_aspec->extend("sigmaXY","float");
    
    if (m_storeErrors) {
      m_aspec->extend("posXErr","float");
      m_aspec->extend("posYErr","float");
      m_aspec->extend("posZErr","float");
      m_aspec->extend("sigmaXErr","float");
      m_aspec->extend("sigmaYErr","float");
      m_aspec->extend("sigmaZErr","float");
      m_aspec->extend("tiltXErr","float");
      m_aspec->extend("tiltYErr","float");
      m_aspec->extend("sigmaXYErr","float");    
    }
    

    if (!m_aspec->size()) msg(MSG::ERROR) << "Attribute list specification is empty!" << endreq;

  }

  if (m_doRoot) {
    // Prepare all the ROOT stuff for writing beamspot data out to a root file
    if ((service("THistSvc", m_thistSvc)).isFailure()){
      msg(MSG::FATAL) << "THistSvc service not found" << endreq;
      return StatusCode::FAILURE;
    }
    const std::string inRootID = "/INDETBEAMSPOTFINDER/" + m_root_dir;
    m_root_bs = new TTree(m_root_treename.c_str(),"Beamspot Position information");
    // insert branches
    m_root_bs->Branch("beamID",&m_beamID,
		      "id/I:runBegin/I:runEnd/I:lumiBegin/I:lumiEnd_plus1:firstEvent:lastEvent:timeStampBegin:timeStampEnd:timeStampOffsetBegin");
    m_root_bs->Branch("beamPos",&m_beamPos,"posX/D:posY:posZ:sigmaX:sigmaY:sigmaZ:tiltX:tiltY:sigmaXY");
    if (m_storeErrors) m_root_bs->Branch("beamPosErr",&m_beamPosErr,"posXErr/D:posYErr:posZErr:sigmaXErr:sigmaYErr:sigmaZErr:tiltXErr:tiltYErr:sigmaXYErr");
    m_root_bs->Branch("beamStatus",&m_beamStatus,"Word/I:Online/I:FitStatus:AlgType:FittedWidth");
    
    if (StatusCode::SUCCESS==m_thistSvc->regTree(inRootID+"/"+m_root_treename,m_root_bs)) {
      msg(MSG::INFO) << "Booked module ntuple " << inRootID <<" "<<m_root_treename << endreq;
    } else {
      msg(MSG::ERROR) << "Unable to register module ntuple " <<inRootID <<" " << m_root_treename <<
        endreq;
      m_doRoot = false;
    }
  } // doRoot

  return StatusCode::SUCCESS;
}


StatusCode InDetBeamSpotDbWriterTool::finalize() {
  msg(MSG::DEBUG) << "In finalize" << endreq;
  //nothing to finalise
  return StatusCode::SUCCESS;
}

StatusCode InDetBeamSpotDbWriterTool::write(std::map<BeamSpot::ID, BeamSpot::BeamSpotDB> & beamspots) {
  
  if (beamspots.size() == 0) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No beamspots are to be stored" << endreq;
    return StatusCode::SUCCESS;
  }

  bool errors = false;
  //start the loop over the stored beamspots
  for (std::map<BeamSpot::ID,  BeamSpot::BeamSpotDB>::const_iterator bit 
	 = beamspots.begin(); bit != beamspots.end(); ++bit ) {
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Attempting to store Beamspot: " 
	  << bit->first << endreq;
     const static unsigned int comp(~0);
     if ( bit->first.lumiStart() == comp && bit->first.lumiEnd() == comp){
       msg(MSG::INFO) << "Found 'default' beamspot of id " << bit->first << ". This will not be recorded to database" << endreq;
       continue;
     }

     if (bit->second.isSuccessfulFit == false) { //beamspot not defined, probably as wasn't solved
       if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "BeamSpot marked as unsuccessful: " << bit->first << " " << endreq;
       //continue;
       if ( m_writeFailed == false) {
	 if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "BeamSpot will be Skipped !!!" << bit->first  << endreq;
	 continue;
       }
     }
     if ( addBeamspot( bit->first, bit->second) ) {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored beamspot " << bit->first << "ok" << endreq;
     }else {
       errors = true;
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Problems storing  beamspot " 
						   << bit->first  << endreq;
     }
  }
  if (errors) {
    if (msgLvl(MSG::ERROR))  msg(MSG::ERROR) << "Problems storing  beamspot(s). " << endreq; 
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;   
}

bool InDetBeamSpotDbWriterTool::addBeamspot(const BeamSpot::ID & id,  const BeamSpot::BeamSpotDB& beamspot) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << id << endreq;
  if (m_doRoot) {
    if (StatusCode::SUCCESS != fillRoot(&id,&beamspot)){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to Write to ROOT file" << endreq;
    }
  }

  if (m_doCOOL) {
    const DataHandle<AthenaAttributeList> adh;
    if (StatusCode::SUCCESS != detStore()->retrieve( adh, m_beamposFolder ) ){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot get BeamPos info. - will attempt to record" << endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Atribute list ptr: " << &(*adh) <<endreq;
    }else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a list with a data handle" <<  endreq;
    }
    
    //try new record method
    AthenaAttributeList *al = const_cast<AthenaAttributeList*>(&(*adh));
    if ( !al ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No AthenaAttributeList - will create new" << endreq;
    al = new AthenaAttributeList(*m_aspec);
    }  
    
    
    //(*al)["posZ"].setValue<float>(posZ);
    (*al)["status"].setValue(beamspot.status);
    (*al)["posX"].setValue(beamspot.posX);
    (*al)["posY"].setValue(beamspot.posY);
    (*al)["posZ"].setValue(beamspot.posZ);
    (*al)["sigmaX"].setValue(beamspot.sigmaX);
    (*al)["sigmaY"].setValue(beamspot.sigmaY);
    (*al)["sigmaZ"].setValue(beamspot.sigmaZ);
    (*al)["tiltX"].setValue(beamspot.tiltX);
    (*al)["tiltY"].setValue(beamspot.tiltY);
    (*al)["sigmaXY"].setValue(beamspot.sigmaXY);
    
    if (m_storeErrors) {
      (*al)["posXErr"].setValue(beamspot.posXErr);
      (*al)["posYErr"].setValue(beamspot.posYErr);
      (*al)["posZErr"].setValue(beamspot.posZErr);
      (*al)["sigmaXErr"].setValue(beamspot.sigmaXErr);
      (*al)["sigmaYErr"].setValue(beamspot.sigmaYErr);
      (*al)["sigmaZErr"].setValue(beamspot.sigmaZErr);
      (*al)["tiltXErr"].setValue(beamspot.tiltXErr);
      (*al)["tiltYErr"].setValue(beamspot.tiltYErr);
      (*al)["sigmaXYErr"].setValue(beamspot.sigmaXYErr);
    }
    
    
    /*
      try {
      (*al)["sigmaXY"].setValue(beamspot.sigmaXY);
      } catch(coral::AttributeListException& e) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
      "No sigmaXY retrieved from conditions DB" << endreq;
      }
    */

    if (StatusCode::SUCCESS==detStore()->record(al, m_beamposFolder, true )) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Recorded Beampos object in TDS " << endreq;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not record Beampos object in TDS" << endreq;
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "If there is a warning message above - that's ok. " << endreq;
    }


    //define the Interval of Validity
    unsigned int runBegin, runEnd, lbBegin, lbEnd;
    runBegin = id.run();
    runEnd   = id.runEnd();
    
    
    if (m_setLBwithAcceptedEvents) {
      lbBegin  = id.firstAcceptedLB();
      lbEnd    = id.lastAcceptedLB() + 1;// IOV is one-past-the-post
    } else if (m_setLBwithViewedEvents) {
      lbBegin  = id.firstLB();
      lbEnd    = id.lastLB() + 1;
    } else {
      lbBegin  = id.lumiStart();
      lbEnd    = id.lumiEnd(); // no +1 is required here (done in method)
    }
    

    unsigned int evtBegin, evtEnd, timeBegin, timeEnd;
    evtBegin  = id.firstEvent();
    evtEnd    = id.lastEvent();
    timeBegin = id.firstTimeStamp();
    timeEnd   = id.lastTimeStamp(); // TBD what about the offset?
    
    /*
      msg(MSG::ERROR) << "In Test Mode !!!!!!!!!!!!!!!!! " << endreq;
      static int test =0;
      lbBegin += test++;
      lbEnd = lbBegin;
    */
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Begin registering of IOV" << endreq;
    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Attempting Db write for Beamspot with IOV range: " << endreq;
      msg(MSG::INFO) << "runBegin: " << runBegin << ", runEnd: " << runEnd
		     << ", lumBegin: " <<lbBegin << ", lumEnd: " << lbEnd << endreq;
    }
    //Register into Database
    if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << "Attempting register with tag: " << m_tagID << endreq;

  
  
    StatusCode sc(StatusCode::FAILURE);
    
    if ( !m_doTimeStampWrite && !m_doEventWrite && 
	 StatusCode::SUCCESS ==  m_regsvc->registerIOV("AthenaAttributeList",
						       m_beamposFolder,m_tagID,
						       runBegin, runEnd,
						       lbBegin, lbEnd)){
      sc = StatusCode::SUCCESS;
    } else if ( m_doEventWrite &&
	      StatusCode::SUCCESS == m_regsvc->registerIOV("AthenaAttributeList",
							   m_beamposFolder,m_tagID,
							   runBegin,runEnd,
							   evtBegin, evtEnd)) {
      sc = StatusCode::SUCCESS;
    }  else if (m_doTimeStampWrite &&  
		StatusCode::SUCCESS == m_regsvc->registerIOV("AthenaAttributeList",
							     m_beamposFolder,m_tagID,
							   runBegin,runEnd,
							   timeBegin, timeEnd)) {
      sc = StatusCode::SUCCESS;
    }
			  
    if (sc != StatusCode::SUCCESS) {
      if (msgLvl(MSG::ERROR))  msg(MSG::ERROR) <<"Could not register in IOV DB for AthenaAttributeList using run and event range " <<endreq;
      return false;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Registered in IOV DB for AthenaAttributeList" <<endreq;
    }
  }

  return true;
}

StatusCode InDetBeamSpotDbWriterTool::fillRoot(const BeamSpot::ID * id ,const BeamSpot::BeamSpotDB* db) {
  if (!m_root_bs || !id || !db) {
    if (msgLvl(MSG::ERROR))
      msg(MSG::ERROR) << " Invalid BS to fill root tree" << endreq;
    return StatusCode::FAILURE;
  }

  m_beamPos[0] = db->posX;
  m_beamPos[1] = db->posY;
  m_beamPos[2] = db->posZ;
  m_beamPos[3] = db->sigmaX;
  m_beamPos[4] = db->sigmaY;
  m_beamPos[5] = db->sigmaZ;
  m_beamPos[6] = db->tiltX;
  m_beamPos[7] = db->tiltY;
  m_beamPos[8] = db->sigmaXY;

  if (m_storeErrors) {
    m_beamPosErr[0] = db->posXErr;
    m_beamPosErr[1] = db->posYErr;
    m_beamPosErr[2] = db->posZErr;
    m_beamPosErr[3] = db->sigmaXErr;
    m_beamPosErr[4] = db->sigmaYErr;
    m_beamPosErr[5] = db->sigmaZErr;
    m_beamPosErr[6] = db->tiltXErr;
    m_beamPosErr[7] = db->tiltYErr;
    m_beamPosErr[8] = db->sigmaXYErr;
  }

  int status = db->status;
  
 
  m_beamStatus[0] = status;
  m_beamStatus[1] = (BeamSpotStatusCode::isOnline(status) ? 1 : 0);
  m_beamStatus[2] = BeamSpotStatusCode::fitStatus(status);
  m_beamStatus[3] = BeamSpotStatusCode::algType(status);
  m_beamStatus[4] = (BeamSpotStatusCode::fitWidth(status) ? 1 :0);

  double lbBegin(0.), lbEnd(0.);
  if (m_setLBwithAcceptedEvents) {
    lbBegin  = id->firstAcceptedLB();
    lbEnd    = id->lastAcceptedLB()+1; // IOV is one-past-the-post
  } else if (m_setLBwithViewedEvents) {
    lbBegin  = id->firstLB();
    lbEnd    = id->lastLB()+1;
  } else {
    lbBegin  = id->lumiStart();
    lbEnd    = id->lumiEnd();
  }


  m_beamID[0] = id->id();
  m_beamID[1] = id->run();
  m_beamID[2] = id->runEnd();
  //  m_beamID[3] = id->lumiStart();
  //  m_beamID[4] = id->lumiEnd();
  m_beamID[3] = lbBegin;
  m_beamID[4] = lbEnd;
  m_beamID[5] = id->firstEvent();
  m_beamID[6] = id->lastEvent();
  m_beamID[7] = id->firstTimeStamp();
  m_beamID[8] = id->lastTimeStamp();
  m_beamID[9] = id->timeStampOffset();

  
  m_root_bs->Fill();
  return StatusCode::SUCCESS;
}
