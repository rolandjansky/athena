/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannel2Ascii.h"

#include "StoreGate/StoreGate.h"

//#include "LArRecConditions/ILArBadChanTool.h"
#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include <fstream>

LArBadChannel2Ascii::LArBadChannel2Ascii(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm( name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_larCablingSvc("LArCablingService"),
  m_detStore(0)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("FileName",m_fileName="");
  declareProperty("WithMissing",m_wMissing=false);
  declareProperty("SkipDisconnected",m_skipDisconnected=false);
  declareProperty("ExecutiveSummaryFile",m_executiveSummaryFile="");
}

LArBadChannel2Ascii::~LArBadChannel2Ascii() {}


StatusCode LArBadChannel2Ascii::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  if ( m_BadChanTool.retrieve().isFailure() ) {
    log << MSG::FATAL  << "Could not retrieve bad channel tool" << endreq;
    return StatusCode::FAILURE;
  }

  // locate the conditions store ptr to it.
  if (service("DetectorStore", m_detStore).isFailure()) {
    log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }


  if (m_skipDisconnected) {
    if(m_larCablingSvc.retrieve().isFailure()) {
      log << MSG::FATAL  << "Could not retrieve LAr Cabling tool" << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode LArBadChannel2Ascii::execute() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannel2Ascii::finalize() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannel2Ascii::stop() {
  MsgStream log(msgSvc(), name());

  const LArOnlineID* larOnlineID;
  StatusCode sc = m_detStore->retrieve(larOnlineID,"LArOnlineID");
  if (sc.isFailure()) {
    log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore" 
	 << endreq;
    return StatusCode::FAILURE;
  }

  
  std::ostream *out = &(std::cout); 
  std::ofstream outfile;
  if (m_fileName.size()) {
    outfile.open(m_fileName.c_str(),std::ios::out);
    if (outfile.is_open()) {
      log << MSG::INFO << "Writing to file " << m_fileName << endreq;
      out = &outfile;
    }
    else
      log << MSG::ERROR << "Failed to open file " << m_fileName << endreq;
  }

  const bool doExecSummary=(m_executiveSummaryFile.size()!=0);

  const LArBadChanBitPacking packing;

  //std::vector<std::vector<unsigned> > problemMatrix(nParts,std::vector<unsigned>(nProblemTypes) );
  std::vector<std::vector<unsigned> > problemMatrix(nProblemTypes,std::vector<unsigned>(nParts) );
  
  
  const LArBadChannelEnum::BitWord missingFEBMask= ~(0x1<<LArBadChannelEnum::missingFEBBit);
  //std::cout << std::hex << "Mask=" << missingFEBMask << std::dec << std::endl;
    

  std::vector<HWIdentifier>::const_iterator it  = larOnlineID->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e= larOnlineID->channel_end();
  unsigned count=0,nConnected=0;
  for(;it!=it_e;++it) {  
    const HWIdentifier chid=*it;
    if (m_skipDisconnected && !m_larCablingSvc->isOnlineConnected(chid)) continue;
    ++nConnected;

    LArBadChannel bc1 = m_BadChanTool->status(chid);
    LArBadChannelEnum::BitWord bcw=bc1.packedData();
    if (!m_wMissing) bcw&=missingFEBMask; //Supress missingFEB bit
    LArBadChannel bc(bcw);
    
    if (!bc.good()) {
      ++count;
      (*out) << larOnlineID->barrel_ec(chid) << " " 
	     << larOnlineID->pos_neg(chid) << " "
	     << larOnlineID->feedthrough(chid) << " "
	     << larOnlineID->slot(chid) << " "
	     << larOnlineID->channel(chid) << " "
	     << "0 "; //Dummy 0 for calib-line
      (*out) << packing.stringStatus(bc);

      (*out) << "  # 0x" << std::hex << chid.get_identifier32().get_compact() << std::dec << std::endl;
    } //End if channel is good (regular printout)
    
    if (doExecSummary) {
      HWIdentifier fid=larOnlineID->feb_Id(chid);
      LArBadFeb bf= m_BadChanTool->febStatus(fid);

      DetPart dp=EMB;
      if (larOnlineID->isEMECchannel(chid)) 
	dp=EMEC;
      else if (larOnlineID->isHECchannel(chid))
	dp=HEC;
      else if (larOnlineID->isFCALchannel(chid))
	dp=FCAL;
      
      if (bc.deadReadout()) 
	++problemMatrix[DeadReadout][dp];
      if (bc.deadPhys())
	++problemMatrix[DeadPhys][dp];
      if (bc.deadCalib()) 
	++problemMatrix[DeadCalib][dp];
      if (bc.reallyNoisy()) 
	++problemMatrix[Noisy][dp];
      if (bc.sporadicBurstNoise())
	++problemMatrix[Sporadic][dp];
	

      if (bf.deadAll() || bf.deadReadout() || bf.deactivatedInOKS())
	++problemMatrix[DeadFEB][dp];
	
      if (bf.deadAll() || bf.deadReadout() || bf.deactivatedInOKS() || bc.deadReadout() || bc.deadPhys() || bc.reallyNoisy())
	++problemMatrix[GrandTotalDead][dp];
	
    }//end if executive Summary

  }//end loop over channels;
  if (m_skipDisconnected)
    log << MSG::INFO << "Found " << count << " entries in the bad-channel database. (Number of connected cells: " << nConnected  << endreq;
  else
    log << MSG::INFO << "Found " << count << " entries in the bad-channel database. (Number of cells: " << nConnected  << endreq;
  if (m_wMissing)
    log << MSG::INFO << "Inclduing missing FEBs" << endreq;
  else
    log << MSG::INFO << "Without missing FEBs" << endreq;
  if (outfile.is_open())
    outfile.close();


  if (doExecSummary) {
    std::ofstream exeSum;
    exeSum.open(m_executiveSummaryFile.c_str(),std::ios::out);
    if (!exeSum.is_open()) {
      log << MSG::ERROR << "Failed to open file " << m_executiveSummaryFile << endreq;
      return StatusCode::FAILURE;
    }

    log << MSG::INFO << "Writing Executive Summary to file " << m_executiveSummaryFile << endreq;    

    exeSum << "LAr dead readout channels:" << std::endl;
    writeSum(exeSum,problemMatrix[DeadReadout]);

    exeSum << "LAr permanently dead channels inside detector:" << std::endl;
    writeSum(exeSum,problemMatrix[DeadPhys]);

    exeSum << "LAr noisy readout channels (more than 10 sigma above phi average or unstable):" << std::endl;
    writeSum(exeSum,problemMatrix[Noisy]);
    
    exeSum << "LAr readout channels showing sporadic noise bursts:" << std::endl;
    writeSum(exeSum,problemMatrix[Sporadic]);

    exeSum << "LAr readout channels w/o calibration (constants from phi average of eta neighbours):" << std::endl;
    writeSum(exeSum,problemMatrix[DeadCalib]);

    exeSum << "LAr readout channels connected to inactive Front End Boards:" << std::endl;
    writeSum(exeSum,problemMatrix[DeadFEB]);

    exeSum << "LAr readout channels not usable:" << std::endl;
    
    writeSum(exeSum,problemMatrix[GrandTotalDead]);

    exeSum.close();
  }

  return StatusCode::SUCCESS;
}


void LArBadChannel2Ascii::writeSum(std::ofstream& exeFile, std::vector<unsigned> probs) const {
  const unsigned nEMB=109568, nEMEC=63744, nHEC=5632, nFCAL=3524;
  const unsigned nTot=nEMB+nEMEC+nHEC+nFCAL;
  unsigned nTotProb=0;
  for(size_t i=0;i<probs.size();++i) 
    nTotProb+=probs[i];

  exeFile << "    EMB: " <<  std::setw(5) << probs[EMB] << " of "  << nEMB << " (" << std::setprecision(3) <<  (100.0*probs[EMB])/nEMB << "%)" << std::endl;
  exeFile << "   EMEC: " <<  std::setw(5) <<probs[EMEC] << " of "  << nEMEC << " (" << std::setprecision(3) << (100.0*probs[EMEC])/nEMEC << "%)" << std::endl;
  exeFile << " EM tot: " <<  std::setw(5) << probs[EMEC]+probs[EMB]  << " of "  << nEMB+nEMEC 
	   << " (" << std::setprecision(3) << (100.0*(probs[EMEC]+probs[EMB]))/(nEMEC+nEMB) << "%)" << std::endl;

  exeFile << "    HEC: " <<  std::setw(5) <<probs[HEC] << " of "  << nHEC << " (" << std::setprecision(3) << (100.0*probs[HEC])/nHEC << "%)" << std::endl;
  exeFile << "   FCAL: " <<  std::setw(5) <<probs[FCAL] << " of "  << nFCAL << " (" << std::setprecision(3) << (100.0*probs[FCAL])/nFCAL << "%)" << std::endl;    
  exeFile << "  Total: " <<  std::setw(5) << nTotProb << " of "  << nTot << " (" << std::setprecision(3) << (100.0*nTotProb)/nTot << "%)" << std::endl;
  exeFile << std::endl;

  return;

}

