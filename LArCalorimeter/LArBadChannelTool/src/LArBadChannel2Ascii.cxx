/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannel2Ascii.h"
#include "LArRecConditions/LArBadFeb.h"

#include "LArIdentifier/LArOnlineID.h"
#include <fstream>

LArBadChannel2Ascii::LArBadChannel2Ascii(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator),
  m_BCKey("LArBadChannel"),
  m_BFKey("LArBadFeb"),
  m_cablingKey("LArOnOffIdMap")
{
  //declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("BCKey",m_BCKey);
  declareProperty("BFKey",m_BFKey);
  declareProperty("LArOnOffIdMapKey",m_cablingKey);
  declareProperty("FileName",m_fileName="");
  declareProperty("WithMissing",m_wMissing=false);
  declareProperty("SkipDisconnected",m_skipDisconnected=false);
  declareProperty("ExecutiveSummaryFile",m_executiveSummaryFile="");
}

LArBadChannel2Ascii::~LArBadChannel2Ascii() {}


StatusCode LArBadChannel2Ascii::initialize() {

  ATH_MSG_INFO ( "initialize()" );

  ATH_CHECK(m_BCKey.initialize());

  if (m_executiveSummaryFile.size()) ATH_CHECK(m_BFKey.initialize());

  if (m_skipDisconnected) ATH_CHECK(m_cablingKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode LArBadChannel2Ascii::stop() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannel2Ascii::finalize() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannel2Ascii::execute() {

  const bool doExecSummary=(m_executiveSummaryFile.size()!=0);

  SG::ReadCondHandle<LArBadChannelCont> bch{m_BCKey};
  const LArBadChannelCont* badChannelCont{*bch};

  const LArOnOffIdMapping* cabling=nullptr;
  if (m_skipDisconnected) {
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    cabling=(*cablingHdl);
  }

  const LArBadFebCont* badFebCont=nullptr;
  if (doExecSummary) {
    SG::ReadCondHandle<LArBadFebCont> badFebHdl{m_BFKey};
    badFebCont=(*badFebHdl);
  }

  const LArOnlineID* larOnlineID;
  ATH_CHECK( detStore()->retrieve(larOnlineID,"LArOnlineID") );
  
  std::ostream *out = &(std::cout); 
  std::ofstream outfile;
  if (m_fileName.size()) {
    outfile.open(m_fileName.c_str(),std::ios::out);
    if (outfile.is_open()) {
      ATH_MSG_INFO ( "Writing to file " << m_fileName );
      out = &outfile;
    }
    else
      ATH_MSG_ERROR ( "Failed to open file " << m_fileName );
  }

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
    //if (m_skipDisconnected && !m_larCablingSvc->isOnlineConnected(chid)) continue;
    if (cabling && !cabling->isOnlineConnected(chid)) continue;
    ++nConnected;

    LArBadChannel bc1 = badChannelCont->status(chid);
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

      (*out) << "  # 0x" << std::hex << chid.get_identifier32().get_compact();
      if (cabling) {
	Identifier offid=cabling->cnvToIdentifier(chid);
	(*out) << " -> 0x" << offid.get_identifier32().get_compact();
      }
      (*out) << std::dec << std::endl;
    } //End if channel is not good (regular printout)
    
    if (doExecSummary) {
      HWIdentifier fid=larOnlineID->feb_Id(chid);
      LArBadFeb bf=badFebCont->status(fid);

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
    ATH_MSG_INFO ( "Found " << count << " entries in the bad-channel database. (Number of connected cells: " << nConnected  );
  else
    ATH_MSG_INFO ( "Found " << count << " entries in the bad-channel database. (Number of cells: " << nConnected  );
  if (m_wMissing)
    ATH_MSG_INFO ( "Inclduing missing FEBs" );
  else
    ATH_MSG_INFO ( "Without missing FEBs" );
  if (outfile.is_open())
    outfile.close();


  if (doExecSummary) {
    std::ofstream exeSum;
    exeSum.open(m_executiveSummaryFile.c_str(),std::ios::out);
    if (!exeSum.is_open()) {
      ATH_MSG_ERROR ( "Failed to open file " << m_executiveSummaryFile );
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ( "Writing Executive Summary to file " << m_executiveSummaryFile );

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

  exeFile << "    EMB: " <<  std::setw(5) << probs[EMB] << " of "  << nEMB << " (" << std::setprecision(3) <<  probs[EMB]*(100./nEMB) << "%)" << std::endl;
  exeFile << "   EMEC: " <<  std::setw(5) <<probs[EMEC] << " of "  << nEMEC << " (" << std::setprecision(3) << probs[EMEC]*(100./nEMEC) << "%)" << std::endl;
  exeFile << " EM tot: " <<  std::setw(5) << probs[EMEC]+probs[EMB]  << " of "  << nEMB+nEMEC 
          << " (" << std::setprecision(3) << (probs[EMEC]+probs[EMB])*(100./(nEMEC+nEMB)) << "%)" << std::endl;

  exeFile << "    HEC: " <<  std::setw(5) <<probs[HEC] << " of "  << nHEC << " (" << std::setprecision(3) << probs[HEC]*(100./nHEC) << "%)" << std::endl;
  exeFile << "   FCAL: " <<  std::setw(5) <<probs[FCAL] << " of "  << nFCAL << " (" << std::setprecision(3) << probs[FCAL]*(100./nFCAL) << "%)" << std::endl;    
  exeFile << "  Total: " <<  std::setw(5) << nTotProb << " of "  << nTot << " (" << std::setprecision(3) << nTotProb*(100./nTot) << "%)" << std::endl;
  exeFile << std::endl;

  return;

}

