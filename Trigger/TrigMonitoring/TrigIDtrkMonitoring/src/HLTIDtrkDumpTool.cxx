/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *  @file HLTIDtrkDumpTool.cxx
 *
 *  Contact:
 *  @author Stefan.Ask (AT cern DOT ch) 
 *  @author Emily Nurse
 *
 *  Comment: see .h file
 * 
 *  To do : (1) Confirm long time solution for the eta value 
 *              from the TRTSegFinder tracks?
 */

#include "GaudiKernel/IJobOptionsSvc.h"
//#include "AthenaMonitoring/AthenaMonMaager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigIDtrkMonitoring/HLTIDtrkDumpTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
//#include "VxVertex/VxContainer.h"
//#include "VxVertex/VxCandidate.h"

#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include <vector>
#include <math.h>
#include <map>

#include "EventInfo/EventInfo.h"


using namespace HLTIDDump;
using namespace CLHEP;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

HLTIDtrkDumpTool::HLTIDtrkDumpTool(const std::string & type, const std::string & name, const IInterface* parent)
  :IHLTMonTool(type, name, parent),
   m_TDT("Trig::TrigDecisionTool/TrigDecisionTool"),
   m_data(true),
   m_useTDT(true),
   m_deltaEta(0.02), // 0.02
   m_deltaEtaTRT(-1), 
   m_deltaPhi(0.01), // 0.01
   m_minPt(1000),
   m_maxAbsEta(2.5),
   m_maxAbsD0(2.0),
   m_maxAbsZ0(200.),
   m_useSiHits(true),
   m_minSiHits(6), // 6
   m_minPixHits(1), // 1
   m_minSctHits(4), // 5
   m_minTrtHits(1), // 1
   m_pMbMon(false),       // CURRENTLY BEING TESTED. More elegant solutions to come.
   m_pMbMinPt(1500.),
   m_pMbMaxAbsEta(2.5),
   m_pMbMaxAbsD0(1.5),
   m_pMbMaxAbsZ0(200.),
   m_pMbTrigChain("InDetMon_FS"), 
   m_pMbCollOFF("TrackParticleCandidate"), // JWH Edit to test combined Muons (TrackParticleCandidate) Reverted
   m_pMbCollEF("InDetTrigParticleCreation_FullScan_EFID")
{

  //+++ Trigger
  declareProperty("UseTrigDecisionTool",    m_useTDT); 
  declareProperty("TrigDecisionTool",       m_TDT, "The tool to access TrigDecision");
  declareProperty("Data",                   m_data); 
  declareProperty("OfflineCollection",      m_offlineCollection = "TrackParticleCandidate");  
  declareProperty("OfflineCollectionStaco", m_offlineCollectionStaco = "Muons"); 
  declareProperty("MCCollection",           m_mcCollection= "");   
  declareProperty("TrackVariables",         m_Variables);

  //+++ Track selection and match
  declareProperty("UseSiHits",         m_useSiHits);
  declareProperty("MinSiHits",         m_minSiHits); 
  declareProperty("MinPixHits",        m_minPixHits); 
  declareProperty("MinSctHits",        m_minSctHits); 
  declareProperty("MinTrtHits",        m_minTrtHits); 
  declareProperty("MinPt",             m_minPt); 
  declareProperty("MaxAbsEta",         m_maxAbsEta); 
  declareProperty("MaxAbsD0",          m_maxAbsD0); 
  declareProperty("MaxAbsZ0",          m_maxAbsZ0); 
  declareProperty("DeltaEta",          m_deltaEta); 
  declareProperty("DeltaEtaTRT",       m_deltaEtaTRT); 
  declareProperty("DeltaPhi",          m_deltaPhi);

  //+++ Trigger slice specific
  declareProperty("CosMon",               m_COSMIC.doMon); 
  declareProperty("CosFS",                m_COSMIC.fullScan); 
  declareProperty("CosROIHalfWidth",      m_COSMIC.ROIHalfWidth); 
  declareProperty("CosTrigSigTracks",     m_COSMIC.sigTracks);
  declareProperty("CosTrigSigSelect",     m_COSMIC.sigSelect);
  declareProperty("CosTrigSigSelectL2",   m_COSMIC.sigSelectL2);
  declareProperty("CosIDSCAN",            m_COSMIC.doIDSCAN);
  declareProperty("CosSiTrack",           m_COSMIC.doSiTrack);
  declareProperty("CosTRTSegF",           m_COSMIC.doTRTSegF);
  declareProperty("CosEF",                m_COSMIC.doEF);
  declareProperty("CosEFtrt",             m_COSMIC.doEFtrt);
  declareProperty("CosIDSCANCollection",  m_COSMIC.IDSCANCollection);   
  declareProperty("CosSiTrackCollection", m_COSMIC.SiTrackCollection);
  declareProperty("CosTRTSegFCollection", m_COSMIC.TRTSegFCollection);
  declareProperty("CosEFCollection",      m_COSMIC.EFCollection);
  declareProperty("CosEFtrtCollection",   m_COSMIC.EFtrtCollection);
  declareProperty("CosName",              m_COSMIC.name);
  declareProperty("CosStaco",             m_COSMIC.staco);
  declareProperty("CosRoi",               m_COSMIC.roiName);
  declareProperty("CosSuffix",            m_COSMIC.suffix);


  declareProperty("eGammaMon",               m_EGAMMA.doMon); 
  declareProperty("eGammaFS",                m_EGAMMA.fullScan); 
  declareProperty("eGammaROIHalfWidth",      m_EGAMMA.ROIHalfWidth); 
  declareProperty("eGammaTrigSigTracks",     m_EGAMMA.sigTracks);
  declareProperty("eGammaTrigSigSelect",     m_EGAMMA.sigSelect);
  declareProperty("eGammaTrigSigSelectL2",   m_EGAMMA.sigSelectL2);
  declareProperty("eGammaIDSCAN",            m_EGAMMA.doIDSCAN);
  declareProperty("eGammaSiTrack",           m_EGAMMA.doSiTrack);
  declareProperty("eGammaL2Star",            m_EGAMMA.doL2Star);
  declareProperty("eGammaTRTSegF",           m_EGAMMA.doTRTSegF);
  declareProperty("eGammaEF",                m_EGAMMA.doEF);
  declareProperty("eGammaEFtrt",             m_EGAMMA.doEFtrt);
  declareProperty("eGammaIDSCANCollection",  m_EGAMMA.IDSCANCollection);
  declareProperty("eGammaSiTrackCollection", m_EGAMMA.SiTrackCollection);
  declareProperty("eGammaL2StarCollection",  m_EGAMMA.L2StarCollection);
  declareProperty("eGammaTRTSegFCollection", m_EGAMMA.TRTSegFCollection);
  declareProperty("eGammaEFCollection",      m_EGAMMA.EFCollection);
  declareProperty("eGammaEFtrtCollection",   m_EGAMMA.EFtrtCollection);
  declareProperty("eGammaStaco",             m_EGAMMA.staco); 
  declareProperty("eGammaName",              m_EGAMMA.name);
  declareProperty("eGammaRoi",               m_EGAMMA.roiName);
  declareProperty("eGammaSuffix",            m_EGAMMA.suffix);

  declareProperty("eGammaCombMon",               m_EGAMMACOMB.doMon); 
  declareProperty("eGammaCombFS",                m_EGAMMACOMB.fullScan); 
  declareProperty("eGammaCombROIHalfWidth",      m_EGAMMACOMB.ROIHalfWidth); 
  declareProperty("eGammaCombTrigSigTracks",     m_EGAMMACOMB.sigTracks);
  declareProperty("eGammaCombTrigSigSelect",     m_EGAMMACOMB.sigSelect);
  declareProperty("eGammaCombTrigSigSelectL2",   m_EGAMMACOMB.sigSelectL2);
  declareProperty("eGammaCombIDSCAN",            m_EGAMMACOMB.doIDSCAN);
  declareProperty("eGammaCombSiTrack",           m_EGAMMACOMB.doSiTrack);
  declareProperty("eGammaCombL2Star",            m_EGAMMACOMB.doL2Star);
  declareProperty("eGammaCombTRTSegF",           m_EGAMMACOMB.doTRTSegF);
  declareProperty("eGammaCombEF",                m_EGAMMACOMB.doEF);
  declareProperty("eGammaCombEFtrt",             m_EGAMMACOMB.doEFtrt);
  declareProperty("eGammaCombIDSCANCollection",  m_EGAMMACOMB.IDSCANCollection);
  declareProperty("eGammaCombSiTrackCollection", m_EGAMMACOMB.SiTrackCollection);
  declareProperty("eGammaCombL2StarCollection",  m_EGAMMACOMB.L2StarCollection);
  declareProperty("eGammaCombTRTSegFCollection", m_EGAMMACOMB.TRTSegFCollection);
  declareProperty("eGammaCombEFCollection",      m_EGAMMACOMB.EFCollection);
  declareProperty("eGammaCombEFtrtCollection",   m_EGAMMACOMB.EFtrtCollection);
  declareProperty("eGammaCombStaco",             m_EGAMMACOMB.staco); 
  declareProperty("eGammaCombName",              m_EGAMMACOMB.name);
  declareProperty("eGammaCombRoi",               m_EGAMMACOMB.roiName);
  declareProperty("eGammaCombSuffix",            m_EGAMMACOMB.suffix);

  declareProperty("TauMon",               m_TAU.doMon);
  declareProperty("TauFS",                m_TAU.fullScan);
  declareProperty("TauROIHalfWidth",      m_TAU.ROIHalfWidth); 
  declareProperty("TauTrigSigTracks",     m_TAU.sigTracks);
  declareProperty("TauTrigSigSelect",     m_TAU.sigSelect);
  declareProperty("TauTrigSigSelectL2",   m_TAU.sigSelectL2);
  declareProperty("TauIDSCAN",            m_TAU.doIDSCAN);
  declareProperty("TauSiTrack",           m_TAU.doSiTrack);
  declareProperty("TauL2Star",            m_TAU.doL2Star);
  declareProperty("TauTRTSegF",           m_TAU.doTRTSegF);
  declareProperty("TauEF",                m_TAU.doEF);
  declareProperty("TauEFtrt",             m_TAU.doEFtrt);
  declareProperty("TauIDSCANCollection",  m_TAU.IDSCANCollection);
  declareProperty("TauSiTrackCollection", m_TAU.SiTrackCollection);
  declareProperty("TauL2StarCollection",  m_TAU.L2StarCollection);
  declareProperty("TauTRTSegFCollection", m_TAU.TRTSegFCollection);
  declareProperty("TauEFCollection",      m_TAU.EFCollection);
  declareProperty("TauEFtrtCollection",   m_TAU.EFtrtCollection);
  declareProperty("TauStaco",             m_TAU.staco); 
  declareProperty("TauName",              m_TAU.name);
  declareProperty("TauRoi",               m_TAU.roiName);
  declareProperty("TauSuffix",            m_TAU.suffix);

// TJN 14/4/2011 // New TAU SLICE
  declareProperty("NewTauMon",               m_NEWTAU.doMon);
  declareProperty("NewTauFS",                m_NEWTAU.fullScan);
  declareProperty("NewTauROIHalfWidth",      m_NEWTAU.ROIHalfWidth); 
  declareProperty("NewTauTrigSigTracks",     m_NEWTAU.sigTracks);
  declareProperty("NewTauTrigSigSelect",     m_NEWTAU.sigSelect);
  declareProperty("NewTauTrigSigSelectL2",   m_NEWTAU.sigSelectL2);
  declareProperty("NewTauIDSCAN",            m_NEWTAU.doIDSCAN);
  declareProperty("NewTauSiTrack",           m_NEWTAU.doSiTrack);
  declareProperty("NewTauL2Star",            m_NEWTAU.doL2Star);
  declareProperty("NewTauTRTSegF",           m_NEWTAU.doTRTSegF);
  declareProperty("NewTauEF",                m_NEWTAU.doEF);
  declareProperty("NewTauEFtrt",             m_NEWTAU.doEFtrt);
  declareProperty("NewTauIDSCANCollection",  m_NEWTAU.IDSCANCollection);
  declareProperty("NewTauSiTrackCollection", m_NEWTAU.SiTrackCollection);
  declareProperty("NewTauL2StarCollection",  m_NEWTAU.L2StarCollection);
  declareProperty("NewTauTRTSegFCollection", m_NEWTAU.TRTSegFCollection);
  declareProperty("NewTauEFCollection",      m_NEWTAU.EFCollection);
  declareProperty("NewTauEFtrtCollection",   m_NEWTAU.EFtrtCollection);
  declareProperty("NewTauStaco",             m_NEWTAU.staco); 
  declareProperty("NewTauName",              m_NEWTAU.name);
  declareProperty("NewTauRoi",               m_NEWTAU.roiName);
  declareProperty("NewTauSuffix",            m_NEWTAU.suffix);

  declareProperty("MuMon",                m_MUON.doMon); 
  declareProperty("MuFS",                 m_MUON.fullScan); 
  declareProperty("MuROIHalfWidth",       m_MUON.ROIHalfWidth); 
  declareProperty("MuTrigSigTracks",      m_MUON.sigTracks);
  declareProperty("MuTrigSigSelect",      m_MUON.sigSelect);
  declareProperty("MuTrigSigSelectL2",      m_MUON.sigSelectL2);
  declareProperty("MuIDSCAN",             m_MUON.doIDSCAN);
  declareProperty("MuSiTrack",            m_MUON.doSiTrack);
  declareProperty("MuL2Star",             m_MUON.doL2Star);
  declareProperty("MuTRTSegF",            m_MUON.doTRTSegF);
  declareProperty("MuEF",                 m_MUON.doEF);
  declareProperty("MuEFtrt",              m_MUON.doEFtrt);
  declareProperty("MuIDSCANCollection",   m_MUON.IDSCANCollection);
  declareProperty("MuSiTrackCollection",  m_MUON.SiTrackCollection);
  declareProperty("MuL2StarCollection",   m_MUON.L2StarCollection);
  declareProperty("MuTRTSegFCollection",  m_MUON.TRTSegFCollection);
  declareProperty("MuEFCollection",       m_MUON.EFCollection);
  declareProperty("MuEFtrtCollection",    m_MUON.EFtrtCollection);
  declareProperty("MuStaco",              m_MUON.staco); //JWH
  declareProperty("MuName",               m_MUON.name); // JWH
  declareProperty("MuRoi",                m_MUON.roiName);
  declareProperty("MuSuffix",             m_MUON.suffix);

  declareProperty("MuCombMon",                m_MUONCOMB.doMon); // JWH 
  declareProperty("MuCombFS",                 m_MUONCOMB.fullScan); 
  declareProperty("MuCombROIHalfWidth",       m_MUONCOMB.ROIHalfWidth); 
  declareProperty("MuCombTrigSigTracks",      m_MUONCOMB.sigTracks);
  declareProperty("MuCombTrigSigSelect",      m_MUONCOMB.sigSelect);
  declareProperty("MuCombTrigSigSelectL2",    m_MUONCOMB.sigSelectL2);
  declareProperty("MuCombIDSCAN",             m_MUONCOMB.doIDSCAN);
  declareProperty("MuCombSiTrack",            m_MUONCOMB.doSiTrack);
  declareProperty("MuCombL2Star",             m_MUONCOMB.doL2Star);
  declareProperty("MuCombTRTSegF",            m_MUONCOMB.doTRTSegF);
  declareProperty("MuCombEF",                 m_MUONCOMB.doEF);
  declareProperty("MuCombEFtrt",              m_MUONCOMB.doEFtrt);
  declareProperty("MuCombIDSCANCollection",   m_MUONCOMB.IDSCANCollection);
  declareProperty("MuCombSiTrackCollection",  m_MUONCOMB.SiTrackCollection);
  declareProperty("MuCombL2StarCollection",   m_MUONCOMB.L2StarCollection);
  declareProperty("MuCombTRTSegFCollection",  m_MUONCOMB.TRTSegFCollection);
  declareProperty("MuCombEFCollection",       m_MUONCOMB.EFCollection);
  declareProperty("MuCombEFtrtCollection",    m_MUONCOMB.EFtrtCollection);
  declareProperty("MuCombStaco",              m_MUONCOMB.staco); // JWH
  declareProperty("MuCombName",               m_MUONCOMB.name); // JWH
  declareProperty("MuCombRoi",                m_MUONCOMB.roiName);

  declareProperty("BMon",                m_BJET.doMon); 
  declareProperty("BFS",                 m_BJET.fullScan); 
  declareProperty("BROIHalfWidth",       m_BJET.ROIHalfWidth); 
  declareProperty("BTrigSigTracks",      m_BJET.sigTracks);
  declareProperty("BTrigSigSelect",      m_BJET.sigSelect);
  declareProperty("BTrigSigSelectL2",    m_BJET.sigSelectL2);
  declareProperty("BIDSCAN",             m_BJET.doIDSCAN);
  declareProperty("BSiTrack",            m_BJET.doSiTrack);
  declareProperty("BL2Star",             m_BJET.doL2Star);
  declareProperty("BTRTSegF",            m_BJET.doTRTSegF);
  declareProperty("BEF",                 m_BJET.doEF);
  declareProperty("BEFtrt",              m_BJET.doEFtrt);
  declareProperty("BIDSCANCollection",   m_BJET.IDSCANCollection);
  declareProperty("BSiTrackCollection",  m_BJET.SiTrackCollection);
  declareProperty("BL2StarCollection",   m_BJET.L2StarCollection);
  declareProperty("BTRTSegFCollection",  m_BJET.TRTSegFCollection);
  declareProperty("BEFCollection",       m_BJET.EFCollection);
  declareProperty("BEFtrtCollection",    m_BJET.EFtrtCollection);
  declareProperty("BName",               m_BJET.name);
  declareProperty("BStaco",              m_BJET.staco);
  declareProperty("BRoi",                m_BJET.roiName);
  declareProperty("BSuffix",             m_BJET.suffix);

  declareProperty("FSMon",                m_FULLSCAN.doMon); 
  declareProperty("FSFS",                 m_FULLSCAN.fullScan); 
  declareProperty("FSROIHalfWidth",       m_FULLSCAN.ROIHalfWidth); 
  declareProperty("FSTrigSigTracks",      m_FULLSCAN.sigTracks);
  declareProperty("FSTrigSigSelect",      m_FULLSCAN.sigSelect);
  declareProperty("FSTrigSigSelectL2",    m_FULLSCAN.sigSelectL2);
  declareProperty("FSIDSCAN",             m_FULLSCAN.doIDSCAN);
  declareProperty("FSSiTrack",            m_FULLSCAN.doSiTrack);
  declareProperty("FSL2Star",             m_FULLSCAN.doL2Star);
  declareProperty("FSTRTSegF",            m_FULLSCAN.doTRTSegF);
  declareProperty("FSEF",                 m_FULLSCAN.doEF);
  declareProperty("FSEFtrt",              m_FULLSCAN.doEFtrt);
  declareProperty("FSIDSCANCollection",   m_FULLSCAN.IDSCANCollection);
  declareProperty("FSSiTrackCollection",  m_FULLSCAN.SiTrackCollection);
  declareProperty("FSL2StarCollection",   m_FULLSCAN.L2StarCollection);
  declareProperty("FSTRTSegFCollection",  m_FULLSCAN.TRTSegFCollection);
  declareProperty("FSEFCollection",       m_FULLSCAN.EFCollection);
  declareProperty("FSEFtrtCollection",    m_FULLSCAN.EFtrtCollection);
  declareProperty("FSStaco",              m_FULLSCAN.staco); //JWH
  declareProperty("FSName",               m_FULLSCAN.name);
  declareProperty("FSRoi",                m_FULLSCAN.roiName);
  declareProperty("FSSuffix",             m_FULLSCAN.suffix);

  declareProperty("MBMon",                m_MINBIAS.doMon); 
  declareProperty("MBFS",                 m_MINBIAS.fullScan); 
  declareProperty("MBROIHalfWidth",       m_MINBIAS.ROIHalfWidth); 
  declareProperty("MBTrigSigTracks",      m_MINBIAS.sigTracks);
  declareProperty("MBTrigSigSelect",      m_MINBIAS.sigSelect);
  declareProperty("MBTrigSigSelectL2",    m_MINBIAS.sigSelectL2);
  declareProperty("MBIDSCAN",             m_MINBIAS.doIDSCAN);
  declareProperty("MBSiTrack",            m_MINBIAS.doSiTrack);
  declareProperty("MBL2Star",             m_MINBIAS.doL2Star);
  declareProperty("MBTRTSegF",            m_MINBIAS.doTRTSegF);
  declareProperty("MBEF",                 m_MINBIAS.doEF);
  declareProperty("MBEFtrt",              m_MINBIAS.doEFtrt);
  declareProperty("MBIDSCANCollection",   m_MINBIAS.IDSCANCollection);
  declareProperty("MBSiTrackCollection",  m_MINBIAS.SiTrackCollection);
  declareProperty("MBL2StarCollection",   m_MINBIAS.L2StarCollection);
  declareProperty("MBTRTSegFCollection",  m_MINBIAS.TRTSegFCollection);
  declareProperty("MBEFCollection",       m_MINBIAS.EFCollection);
  declareProperty("MBEFtrtCollection",    m_MINBIAS.EFtrtCollection);
  declareProperty("MBStaco",              m_MINBIAS.staco); //JWH
  declareProperty("MBName",               m_MINBIAS.name);
  declareProperty("MBRoi",                m_MINBIAS.roiName);
  declareProperty("MBSuffix",             m_MINBIAS.suffix);

  //+++ Physics Monitoring config.
  declareProperty("PhysMbMon",               m_pMbMon);       // MB histograms
  declareProperty("PhysMbMinPt",             m_pMbMinPt);     // MB histograms
  declareProperty("PhysMbMaxAbsEta",         m_pMbMaxAbsEta); // MB histograms
  declareProperty("PhysMbMaxAbsD0",          m_pMbMaxAbsD0);  // MB histograms
  declareProperty("PhysMbMaxAbsZ0",          m_pMbMaxAbsZ0);  // MB histograms
  declareProperty("PhysMbTrigChain",         m_pMbTrigChain); // MB histograms
  declareProperty("PhysMbCollOFF",           m_pMbCollOFF);   // MB histograms
  declareProperty("PhysMbCollEF",            m_pMbCollEF);    // MB histograms

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

HLTIDtrkDumpTool::~HLTIDtrkDumpTool() {
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode HLTIDtrkDumpTool::init() {

  if (m_BJET.staco)
    m_BJET.staco = 0;
  
  if (m_useTDT) {
    StatusCode sc = m_TDT.retrieve();
    if ( sc.isFailure()   ) {  
      ATH_MSG_FATAL("HLTIDtrkDumpTool FATAL: Could not retrieve TrigDecisionTool!");
      return sc;
    }
  }

  //+++ Cross check signatures and tracking alg. vectors
  if (m_COSMIC.doMon && !checkSize(m_COSMIC)) {
    ATH_MSG_FATAL(" Inconsistent settings for Cosmics Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_EGAMMA.doMon && !checkSize(m_EGAMMA)) {
    ATH_MSG_FATAL(" Inconsistent settings for eGamma Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_EGAMMACOMB.doMon && !checkSize(m_EGAMMACOMB)) {
    ATH_MSG_FATAL(" Inconsistent settings for eGamma combined Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_TAU.doMon && !checkSize(m_TAU)) {
    ATH_MSG_FATAL(" Inconsistent settings for Tau Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_NEWTAU.doMon && !checkSize(m_NEWTAU)) { //TJN
    ATH_MSG_FATAL(" Inconsistent settings for NewTau Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_MUON.doMon && !checkSize(m_MUON)) {
    ATH_MSG_FATAL(" Inconsistent settings for Muon Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_MUONCOMB.doMon && !checkSize(m_MUONCOMB)) {
    ATH_MSG_FATAL(" Inconsistent settings for Muon Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_BJET.doMon && !checkSize(m_BJET)) {
    ATH_MSG_FATAL(" Inconsistent settings for BJet Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_FULLSCAN.doMon && !checkSize(m_FULLSCAN)) {
    ATH_MSG_FATAL(" Inconsistent settings for FullScan Basic Plots! ");
    return StatusCode::FAILURE;
  }
  if (m_MINBIAS.doMon && !checkSize(m_MINBIAS)) {
    ATH_MSG_FATAL(" Inconsistent settings for MinBias Basic Plots! ");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTIDtrkDumpTool::book() {
#else
StatusCode HLTIDtrkDumpTool::book(bool newEventsBlock, bool newLumiBlock, bool newRun) {
#endif

  ATH_MSG_VERBOSE(" ====== Begin book() ====== ");

  //+++ Standard histograms
  MonGroup * monGroup = new MonGroup(this, "HLT/IDTrkDump", run);
  addMonGroup(monGroup);
  //+++ Efficiency MonGroup, in order to merge them properly
  MonGroup * effMonGroup = new MonGroup(this, "HLT/IDTrkDumpEff", run);
  addMonGroup(effMonGroup);

  //+++ Debugging histograms histograms, contains the numerators and denominators for efficeincy plots
  MonGroup * effDebugMonGroup = new MonGroup(this, "HLT/IDTrkDumpEffDebug", run);
  addMonGroup(effDebugMonGroup);

  //+++ LowStat MonGroups
  MonGroup * lowMonGroup = new MonGroup(this, "HLT/IDTrkDumpLS", ManagedMonitorToolBase::lowStat);
  addMonGroup(lowMonGroup);
  MonGroup * lowEffMonGroup = new MonGroup(this, "HLT/IDTrkDumpEffLS", ManagedMonitorToolBase::lowStat);
  addMonGroup(lowEffMonGroup);

  //+++ Physics Monitoring Histograms
  MonGroup * tmpGroup = new MonGroup(this, "HLT/IDTrkPhys", run);
  addMonGroup(tmpGroup);

  //  if (newRun || newLowStatInterval) {
  if (newRun) {

    //+++ Diagnostic Histograms
    addHistogram(new TH1F("L2Result_Checker", "Checks events that failed due to L2 truncated result", 10, 0, 10.), "HLT/IDTrkDump");
    addHistogram(new TH1F("EFResult_Checker", "Checks events that failed due to EF truncated result", 10, 0, 10.), "HLT/IDTrkDump");

    //+++ Cosmic signature histograms
    if (m_COSMIC.doMon)  bookMonHist( m_Variables, 
				      m_COSMIC,
				      "cos",
				      newRun);
    //+++ eGamma signature histograms
    if (m_EGAMMA.doMon)  bookMonHist( m_Variables, 
				      m_EGAMMA,
				      "eg",
				      newRun);
    //+++ eGamma signature histograms
    if (m_EGAMMACOMB.doMon) bookMonHist( m_Variables, 
				      m_EGAMMACOMB,
				      "egcomb",
				      newRun);
    //+++ Tau signature histograms
    if (m_TAU.doMon)     bookMonHist( m_Variables, 
				      m_TAU,
				      "tau",
				      newRun);

    //+++ NewTau signature histograms //TJN
    if (m_NEWTAU.doMon)     bookMonHist( m_Variables, 
				      m_NEWTAU,
				      "newtau",
				      newRun);

    //+++ Mu signature histograms
    if (m_MUON.doMon)    bookMonHist( m_Variables, 
				      m_MUON,
				      "mu",
				      newRun);

    //+++ Mu signature histograms
    if (m_MUONCOMB.doMon)
      { bookMonHist( m_Variables, m_MUONCOMB, "mucomb", newRun);
      }

    //+++ B signature histograms
    if (m_BJET.doMon)    bookMonHist( m_Variables, 
				      m_BJET,
				      "b",
				      newRun);

    //+++ FullScan signature histograms
    if (m_FULLSCAN.doMon)    bookMonHist( m_Variables, 
					  m_FULLSCAN,
					  "fs",
					  newRun);
    //+++ Physics min bias histstograms
    if (m_pMbMon && newRun) {
      addHistogram(new TH1F("hnTrk1_EF", "hnTrk1_EF", 60, 0., 60.), "HLT/IDTrkPhys");     // |eta| < maxAbsEta
      addHistogram(new TH1F("hpT1_EF",   "hpT1_EF",  100, 0., 100.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("heta1_EF",  "heta1_EF",  60, -3., 3.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hsumE1_EF", "hsumE1_EF", 50, 0., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPt1_EF", "hvsPt1_EF", 40, 0., 200.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPz1_EF", "hvsPz1_EF", 50, -500., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hnTrk2_EF", "hnTrk2_EF", 60, 0., 60.), "HLT/IDTrkPhys");     // |eta| < 1.0 (Barrel)
      addHistogram(new TH1F("hpT2_EF",   "hpT2_EF",  100, 0., 100.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("heta2_EF",  "heta2_EF",  60, -3., 3.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hsumE2_EF", "hsumE2_EF", 50, 0., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPt2_EF", "hvsPt2_EF", 40, 0., 200.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPz2_EF", "hvsPz2_EF", 50, -500., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hnTrk1_OFF", "hnTrk1_OFF", 60, 0., 60.), "HLT/IDTrkPhys");     // |eta| < maxAbsEta
      addHistogram(new TH1F("hpT1_OFF",   "hpT1_OFF",  100, 0., 100.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("heta1_OFF",  "heta1_OFF",  60, -3., 3.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hsumE1_OFF", "hsumE1_OFF", 50, 0., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPt1_OFF", "hvsPt1_OFF", 40, 0., 200.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPz1_OFF", "hvsPz1_OFF", 50, -500., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hnTrk2_OFF", "hnTrk2_OFF", 60, 0., 60.), "HLT/IDTrkPhys");     // |eta| < 1.0 (Barrel)
      addHistogram(new TH1F("hpT2_OFF",   "hpT2_OFF",  100, 0., 100.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("heta2_OFF",  "heta2_OFF",  60, -3., 3.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hsumE2_OFF", "hsumE2_OFF", 50, 0., 500.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPt2_OFF", "hvsPt2_OFF", 40, 0., 200.), "HLT/IDTrkPhys");
      addHistogram(new TH1F("hvsPz2_OFF", "hvsPz2_OFF", 50, -500., 500.), "HLT/IDTrkPhys");
    }

    //+++ FullScan signature histograms
    if (m_MINBIAS.doMon)    bookMonHist( m_Variables, 
					 m_MINBIAS,
					 "mb",
					 newRun);
    

    //+++ MinBias signature histograms

  } else if ( newEventsBlock || newLumiBlock ) {
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_VERBOSE(" ====== End book() ====== ");
  
  return StatusCode::SUCCESS;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode HLTIDtrkDumpTool::fill( ) {

  ATH_MSG_VERBOSE(" ====== Begin fill() ====== ");
  
  StatusCode sc = StatusCode::SUCCESS;

  /// Code to remove events with Truncated L2 results
  
  const DataHandle<TrigDec::TrigDecision> td;
  StatusCode sc2 = StatusCode::SUCCESS;
  sc2 = evtStore()->retrieve(td);
  if (sc2.isFailure()) {
  }


  /*    const DataHandle<EventInfo> eventInfo;

  StatusCode sc3 = evtStore()->retrieve(eventInfo);
  if(sc3.isFailure()) std::cout << "Failed to get eventInfo" << std::endl;
  unsigned int eventNumber = (eventInfo->event_ID())->event_number();
  */
  /*  const EventInfo* pEventInfo;
  int run_number        = 0;
  unsigned event_number      = 0;
	
  unsigned lumi_block        = 0;

	
  if (evtStore()->retrieve(pEventInfo).isFailure() ) {
  } else {
    run_number        = pEventInfo->event_ID()->run_number();
    //event_number      = pEventInfo->event_ID()->event_number();
    //lumi_block        = pEventInfo->event_ID()->lumi_block();
  }
  */
  
  
  bool Truncated_Result = 0;
  bool Truncated_Result_EF = 0;
  
  const HLT::HLTResult* r = (&(td->getL2Result()));
  if (r->isHLTResultTruncated()){
    hist("L2Result_Checker","HLT/IDTrkDump")->Fill(1.0);
    Truncated_Result = 1;
  }
  else hist("L2Result_Checker","HLT/IDTrkDump")->Fill(0.0);

  const HLT::HLTResult* rd = (&(td->getEFResult()));
  if (rd->isHLTResultTruncated()){
    hist("EFResult_Checker","HLT/IDTrkDump")->Fill(1.0);
    Truncated_Result_EF = 1;
  }
  else hist("EFResult_Checker","HLT/IDTrkDump")->Fill(0.0);

  if (!Truncated_Result && !Truncated_Result_EF){

    //+++ Cosmics
    if (m_COSMIC.doMon) sc = cosMon();
    
    //+++ eGamma
    if (m_EGAMMA.doMon) sc = egMon();

    //+++ eGammaCOMB
    if (m_EGAMMACOMB.doMon) sc = egcombMon();
    
    //+++ Tau
    if (m_TAU.doMon)    sc = tauMon();
    
    //+++ NewTau //TJN
    if (m_NEWTAU.doMon)    sc = newtauMon();

    //+++ Muon
    if (m_MUON.doMon)   sc = muMon();
    
    //+++ MuonCOMB
    if (m_MUONCOMB.doMon)   sc = mucombMon();
    
    //+++ BJet
    if (m_BJET.doMon)   sc = bMon();
    
    //+++ FullScan
    if (m_FULLSCAN.doMon) sc = fsMon();
    
    //+++ FullScan
    if (m_MINBIAS.doMon) sc = mbMon();
  }

  ATH_MSG_VERBOSE(" ====== End fill() ====== ");

  return sc;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#ifdef ManagedMonitorToolBase_Uses_API_201401
 StatusCode  HLTIDtrkDumpTool::proc() {
#else
StatusCode  HLTIDtrkDumpTool::proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun ) {
#endif
  
  ATH_MSG_VERBOSE(" ====== Begin proc() ====== ");

  if (endOfRun) {

    // Not used right now

  }

  ATH_MSG_VERBOSE(" ====== End proc() ====== ");

  return StatusCode::SUCCESS;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::cosMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin cosMon() ====== ");
  
  StatusCode sc_ideftrk = fillBasicHist(m_COSMIC);

  ATH_MSG_VERBOSE(" ====== End cosMon() ====== ");
  
  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::egMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin egMon() ====== ");

  StatusCode sc_ideftrk = fillBasicHist(m_EGAMMA);

  ATH_MSG_VERBOSE(" ====== End egMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::egcombMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin egcombMon() ====== ");

  StatusCode sc_ideftrk = fillBasicHist(m_EGAMMACOMB);

  ATH_MSG_VERBOSE(" ====== End egcombMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::tauMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin tauMon() ====== ");
  
  StatusCode sc_ideftrk = fillBasicHist(m_TAU);

  ATH_MSG_VERBOSE(" ====== End tauMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::newtauMon( ) { //TJN

  ATH_MSG_VERBOSE(" ====== Begin newtauMon() ====== ");
  
  StatusCode sc_ideftrk = fillBasicHist(m_NEWTAU);

  ATH_MSG_VERBOSE(" ====== End newtauMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::muMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin muMon() ====== ");

  StatusCode sc_ideftrk = fillBasicHist(m_MUON);
 
  ATH_MSG_VERBOSE(" ====== End muMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::mucombMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin mucombMon() ====== ");

  StatusCode sc_ideftrk = fillBasicHist(m_MUONCOMB);
 
  ATH_MSG_VERBOSE(" ====== End mucombMon() ====== ");
  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::bMon( ) {                                 

   ATH_MSG_DEBUG(" ====== Begin bMon() ====== ");
  
  StatusCode sc_ideftrk = fillBasicHist(m_BJET);

  ATH_MSG_DEBUG(" ====== End bMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::fsMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin fsMon() ====== ");

  StatusCode sc_ideftrk = fillBasicHist(m_FULLSCAN);
 
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Additional fullscan plots
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  //+++ Mb histograms
  if (!m_pMbMon) return sc_ideftrk;

  //+++ Passed trigger 
  std::string EFChainName = "EF_" + m_pMbTrigChain;
  if (!m_TDT->isPassed(EFChainName)) return sc_ideftrk; 

  //+++ Get Offline and EF collections
  std::vector< simpleTrack > allOFFTracks, allEFTracks;  
  const DataHandle<Rec::TrackParticleContainer> trackPartColl, trackPartCollLast;
  sc_ideftrk = m_storeGate->retrieve(trackPartColl,trackPartCollLast);
  if (sc_ideftrk.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve tracks! ");
    allOFFTracks.clear();
    allEFTracks.clear();
  } else {
    for ( ; trackPartColl != trackPartCollLast;++trackPartColl) {
      std::string collectionKey = trackPartColl.key();
      if( collectionKey.find(m_pMbCollOFF, 0) != std::string::npos ){ 
	allOFFTracks = GetTrackPartVectors(trackPartColl);
      } else if ( collectionKey.find(m_pMbCollEF, 0) != std::string::npos ) {
	allEFTracks = GetTrackPartVectors(trackPartColl);
      }
    }
  }

  fillPhysMbHist( allOFFTracks, "OFF"); // Offline plots
  fillPhysMbHist( allEFTracks, "EF");   // EF plots

  ATH_MSG_VERBOSE(" ====== End fsMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::mbMon( ) {

  ATH_MSG_VERBOSE(" ====== Begin mbMon() ====== ");
  
  StatusCode sc_ideftrk = fillBasicHist(m_MINBIAS);

  ATH_MSG_VERBOSE(" ====== End mbMon() ====== ");

  return sc_ideftrk;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

StatusCode  HLTIDtrkDumpTool::fillBasicHist( HLTIDDump::SliceVariables & SLICE) {
  
  ATH_MSG_VERBOSE(" ====== Begin fillBasicHist() ====== ");

  StatusCode sc_ideftrk = StatusCode::SUCCESS;

  //+++ Get offline trackparticles 
  std::vector< simpleTrack > EVENTallOFFTracks, EVENTselSiOFFTracks, EVENTselTrtOFFTracks;
  std::vector< simpleTrack > TAUtracks, EGAMMAtracks;  
  const DataHandle<Rec::TrackParticleContainer> trackCollectionOFF;

  if (SLICE.staco){ // JWH
     sc_ideftrk = m_storeGate->retrieve(trackCollectionOFF,m_offlineCollectionStaco);
     ATH_MSG_DEBUG(" Using Staco tracks ");
  }
  else{
    sc_ideftrk = m_storeGate->retrieve(trackCollectionOFF,m_offlineCollection);
  }
  if(SLICE.name == "NewTau"){
    TAUtracks = GetTauTracks();
    if (TAUtracks.size() == 0 ) sc_ideftrk = StatusCode::FAILURE;
  }
  if(SLICE.name == "EGammaComb"){
    EGAMMAtracks = GetElectronTracks();
    if (EGAMMAtracks.size() == 0 ) sc_ideftrk = StatusCode::FAILURE;
  }

  if (sc_ideftrk.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve offline tracks! JWH");
    EVENTallOFFTracks.clear();
    EVENTselSiOFFTracks.clear();
    EVENTselTrtOFFTracks.clear();
  } else {
    if (SLICE.name == "EGammaComb") EVENTallOFFTracks = GetTrackPartVectors(EGAMMAtracks, EVENTselSiOFFTracks, EVENTselTrtOFFTracks, true );
    else if (SLICE.name == "NewTau") EVENTallOFFTracks = GetTrackPartVectors(TAUtracks, EVENTselSiOFFTracks, EVENTselTrtOFFTracks, true ); 
    else EVENTallOFFTracks = GetTrackPartVectors(trackCollectionOFF, EVENTselSiOFFTracks, EVENTselTrtOFFTracks, true);
  }
  
  //+++ set the return value back to SUCCESS as we can go on with these errors
  sc_ideftrk = StatusCode::SUCCESS;

  //+++ Loop over signatures
  for (unsigned int i =0; i < SLICE.sigSelect.size(); ++i) {

    std::vector< simpleTrack > allOFFTracks;
    std::vector< simpleTrack > selSiOFFTracks;
    std::vector< simpleTrack > selTrtOFFTracks;
    std::vector< simpleTrack > allOFFTracks_EF;
    std::vector< simpleTrack > selSiOFFTracks_EF;
    std::vector< simpleTrack > selTrtOFFTracks_EF;

    std::vector< simpleTrack > allIDSTracks;
    std::vector< simpleTrack > allSITTracks;
    std::vector< simpleTrack > allL2STracks;
    std::vector< simpleTrack > allL2STracks_A;
    std::vector< simpleTrack > allL2STracks_B;
    std::vector< simpleTrack > allL2STracks_C;
    std::vector< simpleTrack > allTSFTracks;
    std::vector< simpleTrack > allEFTracks;
    std::vector< simpleTrack > allEFtrtTracks;

    // Special case : If signature is "noTDT", don't use the Trigger Decision Tool and get all tracks from StoreGate
    if (SLICE.sigSelect[i].find("NoTDT") != std::string::npos) {

      allOFFTracks    = EVENTallOFFTracks;
      selSiOFFTracks  = EVENTselSiOFFTracks;
      selTrtOFFTracks = EVENTselTrtOFFTracks;

      if (SLICE.doIDSCAN[i]) {
	const DataHandle<TrigInDetTrackCollection> trackCollectionIDS;
	sc_ideftrk = m_storeGate->retrieve(trackCollectionIDS, SLICE.IDSCANCollection[i]);
	if (sc_ideftrk.isFailure()) {
    ATH_MSG_VERBOSE(" Failed to retrieve IDS tracks! JWH");
	} 
	else {
	  allIDSTracks = GetTrigInDetTrackVectors(trackCollectionIDS, false);
	}
      }

      if (SLICE.doSiTrack[i]) {
	//+++ Get SiTrack tracks (TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL)
	const DataHandle<TrigInDetTrackCollection> trackCollectionSIT;
	sc_ideftrk = m_storeGate->retrieve(trackCollectionSIT, SLICE.SiTrackCollection[i]);
	if (sc_ideftrk.isFailure()) {
	  ATH_MSG_VERBOSE(" Failed to retrieve SIT tracks! JWH");
	} else {
	  allSITTracks = GetTrigInDetTrackVectors(trackCollectionSIT, false);
	}
      }

    if (SLICE.doL2Star[i]) {
	//+++ Get L2Star tracks 
	const DataHandle<TrigInDetTrackCollection> trackCollectionL2S;
	std::vector< simpleTrack > trackCollectionL2S_A;
	std::vector< simpleTrack > trackCollectionL2S_B;
	std::vector< simpleTrack > trackCollectionL2S_C;
	sc_ideftrk = m_storeGate->retrieve(trackCollectionL2S, SLICE.L2StarCollection[i]);
	if (sc_ideftrk.isFailure()) {
	  //std::cout << "Failed to get tracks" << std::endl;
	  ATH_MSG_VERBOSE(" Failed to retrieve L2S tracks! JWH");
	} else {
	  allL2STracks = GetTrigInDetTrackVectors(trackCollectionL2S, allL2STracks_A, allL2STracks_B, allL2STracks_C, false);
	}
      }

      if (SLICE.doTRTSegF[i]) {
	//+++ Get TRTSegFinder tracks (TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL)
	const DataHandle<TrigInDetTrackCollection> trackCollectionTSF;
	sc_ideftrk = m_storeGate->retrieve(trackCollectionTSF, SLICE.TRTSegFCollection[i]);
	if (sc_ideftrk.isFailure()) {
	  ATH_MSG_VERBOSE(" Failed to retrieve TSF tracks! JWH");
	} else {
	  allTSFTracks = GetTrigInDetTrackVectors(trackCollectionTSF, false);
	}
      }

      if (SLICE.doEF[i]) { 
	//+++ Get EF trackparticles (TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL)
	const DataHandle<Rec::TrackParticleContainer> trackCollectionEF;
	sc_ideftrk = m_storeGate->retrieve(trackCollectionEF, SLICE.EFCollection[i]);
	if (sc_ideftrk.isFailure()) {
	    ATH_MSG_VERBOSE(" Failed to retrieve EF tracks! ");
	} else {
	  allEFTracks = GetTrackPartVectors(trackCollectionEF);
	}
      }

      if (SLICE.doEFtrt[i]) { 
	//+++ Get EFtrt trackparticles (TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL)
	const DataHandle<Rec::TrackParticleContainer> trackCollectionEFtrt;
	sc_ideftrk = m_storeGate->retrieve(trackCollectionEFtrt, SLICE.EFtrtCollection[i]);
	if (sc_ideftrk.isFailure()) {
	  ATH_MSG_VERBOSE(" Failed to retrieve EF tracks! JWH");
	} else {
	  allEFtrtTracks = GetTrackPartVectors(trackCollectionEFtrt);
	}
      }


      sc_ideftrk = StatusCode::SUCCESS;


    //+++++ All other cases: Use TDT where a real signature name is supplied +++++
    } else {

      //+++ Get RoIs 
      std::string EFChainName = "EF_" +  SLICE.sigSelect[i];

      //+++ Get RoIs 
      std::string L2ChainName = "L2_" + SLICE.sigTracks[i];
      Trig::FeatureContainer featCont = m_TDT->features(SLICE.sigSelectL2[i]); // Added to deal with the different names for the bjet triggers at L2 and EF
      Trig::FeatureContainer featCont_EF = m_TDT->features(EFChainName); // JWH

      std::vector< Trig::Feature<TrigRoiDescriptor> > initRois;
      std::vector< Trig::Feature<TrigRoiDescriptor> > initRois_EF;

      if (SLICE.name == "Mu" || SLICE.name == "MuComb"){
	initRois = featCont.get<TrigRoiDescriptor>("forID");
	initRois_EF = featCont_EF.get<TrigRoiDescriptor>("forID");
      }
      else {
	initRois = featCont.get<TrigRoiDescriptor>("initialRoI");
	initRois_EF = featCont.get<TrigRoiDescriptor>("initialRoI");
      }

      bool verbose_output = false;
      if (SLICE.name == "Mu") verbose_output = false;

      Trig::FeatureContainer featContEF_debug = m_TDT->features(EFChainName);
      std::vector< Trig::Feature<Rec::TrackParticleContainer> > featureEF_debug = featContEF_debug.get<Rec::TrackParticleContainer>(SLICE.EFCollection[i]);

      if (!m_TDT->isPassed(SLICE.sigSelectL2[i]) && initRois.size()>0){
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_debug;
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_debug_idscan;
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature_debug_l2s;
	feature_debug = featCont.get<TrigInDetTrackCollection>(SLICE.SiTrackCollection[i]);
	feature_debug_idscan = featCont.get<TrigInDetTrackCollection>(SLICE.SiTrackCollection[i]);
	feature_debug_l2s = featCont.get<TrigInDetTrackCollection>(SLICE.SiTrackCollection[i]);
      }

      if (initRois.empty() && !SLICE.fullScan) continue; 

      //if (m_TDT->isPassed(SLICE.sigSelectL2[i])){ // JWH Run L2 Chains without caring if the chain passes the EF
      if (m_TDT->isPassed(EFChainName)){ // JWH Run L2 Chains without caring if the chain passes the EF

	//+++ Select tracks inside RoI (mainly for offline and truth)
	if (!SLICE.fullScan) {
	  allOFFTracks    = inSideRoi( EVENTallOFFTracks, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  selSiOFFTracks  = inSideRoi( EVENTselSiOFFTracks, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  selTrtOFFTracks = inSideRoi( EVENTselTrtOFFTracks, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  //allOFFTracks_EF    = inSideRoi( EVENTallOFFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  //selSiOFFTracks_EF  = inSideRoi( EVENTselSiOFFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  //selTrtOFFTracks_EF = inSideRoi( EVENTselTrtOFFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	} else {  
	  allOFFTracks    = EVENTallOFFTracks;
	  selSiOFFTracks  = EVENTselSiOFFTracks;
	  selTrtOFFTracks = EVENTselTrtOFFTracks;
	  //allOFFTracks_EF    = EVENTallOFFTracks;
          //selSiOFFTracks_EF  = EVENTselSiOFFTracks;
          //selTrtOFFTracks_EF = EVENTselTrtOFFTracks;
	}
	
	std::vector< Trig::Feature<TrigInDetTrackCollection> > feature;
		
	if (SLICE.doIDSCAN[i]) {
	  //+++ Get a vector of TrigInDetTrackCollections for the track type for this L2 trigger
	  
	  feature = featCont.get<TrigInDetTrackCollection>(SLICE.IDSCANCollection[i]); 
	  allIDSTracks = GetTrigInDetTrackVectors(feature, false, verbose_output);
	  if (!SLICE.fullScan) allIDSTracks = inSideRoi( allIDSTracks, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, "IDS");
	}

	if (SLICE.doSiTrack[i]) {
	  //+++ Get a vector of TrigInDetTrackCollections for the track type for this L2 trigger
	  feature = featCont.get<TrigInDetTrackCollection>(SLICE.SiTrackCollection[i]);
	  allSITTracks = GetTrigInDetTrackVectors(feature, false, false);
	  if (!SLICE.fullScan) allSITTracks = inSideRoi( allSITTracks, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, "SIT"); 
	}

	if (SLICE.doL2Star[i]) {
	  //+++ Get L2Star tracks
	  feature = featCont.get<TrigInDetTrackCollection>(SLICE.L2StarCollection[i], TrigDefs::alsoDeactivateTEs);
	  allL2STracks_A = GetTrigInDetTrackVectors(feature, false, false, 0, SLICE.L2StarCollection[i]);
	  allL2STracks_B = GetTrigInDetTrackVectors(feature, false, false, 1, SLICE.L2StarCollection[i]);
	  allL2STracks_C = GetTrigInDetTrackVectors(feature, false, false, 2, SLICE.L2StarCollection[i]);
	  if (!SLICE.fullScan){
	    allL2STracks_A = inSideRoi( allL2STracks_A, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, "L2S_A");
	    allL2STracks_B = inSideRoi( allL2STracks_B, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, "L2S_B");
	    allL2STracks_C = inSideRoi( allL2STracks_C, initRois, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, "L2S_C");
	  }
	}
      
	if (SLICE.doTRTSegF[i]) {
	  //+++ Get a vector of TrigInDetTrackCollections for the track type for this L2 trigger
	  feature = featCont.get<TrigInDetTrackCollection>(SLICE.TRTSegFCollection[i]);
	  allTSFTracks = GetTrigInDetTrackVectors(feature, false, false);
	  if (!SLICE.fullScan) allTSFTracks = inSideRoi( allTSFTracks, initRois, -1.0, SLICE.ROIHalfWidth, "");
	}
      }
      
      if (m_TDT->isPassed(EFChainName)){ 
	Trig::FeatureContainer featContEF = m_TDT->features(EFChainName);

	if (initRois_EF.empty() && !SLICE.fullScan) continue; 

	//+++ Select tracks inside RoI (mainly for offline and truth)
	if (!SLICE.fullScan) {
	  allOFFTracks_EF    = inSideRoi( EVENTallOFFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  selSiOFFTracks_EF  = inSideRoi( EVENTselSiOFFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	  selTrtOFFTracks_EF = inSideRoi( EVENTselTrtOFFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth, ""); 
	} else {  
	  allOFFTracks_EF    = EVENTallOFFTracks;
          selSiOFFTracks_EF  = EVENTselSiOFFTracks;
          selTrtOFFTracks_EF = EVENTselTrtOFFTracks;
	}

	if (SLICE.doEF[i]) { 
	  //+++ Get a vector of TrigInDetTrackCollections for the track type for this EF trigger	  
	  std::vector< Trig::Feature<Rec::TrackParticleContainer> > featureEF = featContEF.get<Rec::TrackParticleContainer>(SLICE.EFCollection[i]);
	  allEFTracks = GetTrigInDetTrackVectors(featureEF, false, verbose_output);
	  if (!SLICE.fullScan) allEFTracks = inSideRoi( allEFTracks, initRois_EF, SLICE.ROIHalfWidth, SLICE.ROIHalfWidth,"EF");
	}
	
	if (SLICE.doEFtrt[i]) { 
	  //+++ Get a vector of TrigInDetTrackCollections for the track type for this EF trigger
	  std::vector< Trig::Feature<Rec::TrackParticleContainer> > featureEF = featContEF.get<Rec::TrackParticleContainer>(SLICE.EFtrtCollection[i]);
	  allEFtrtTracks = GetTrigInDetTrackVectors(featureEF, false, false);
	  if (!SLICE.fullScan) allEFtrtTracks = inSideRoi( allEFtrtTracks, initRois_EF, -1.0, SLICE.ROIHalfWidth,""); 
	}
      }
    }
    
    //+++ Fill histograms ++++

    //+++ Offline and truth track parameters

    fillParameterHist(selSiOFFTracks, SLICE.sigTracks[i], SLICE.sigSelect[i], "OFF_par", SLICE);
    bool mu_verbose = false;
    if (SLICE.name == "Mu") mu_verbose = false;
    if (SLICE.doIDSCAN[i]) {
      //+++ Track matching
      //+++ Currently using selected offline tracks and all trigger tracks for efficiencies and residuals
      std::map<int, std::vector<int> > offline2IDS = MatchTracks(selSiOFFTracks, allIDSTracks, m_deltaEta, m_deltaPhi, mu_verbose);
      std::map<int, std::vector<int> > IDS2offline = MatchTracks(allIDSTracks, allOFFTracks, m_deltaEta, m_deltaPhi, false);
          
      if (!SLICE.staco){
	//+++ Basic histograms
	fillParameterHist (allIDSTracks,   SLICE.sigTracks[i],  SLICE.sigSelect[i], "IDS_par", SLICE);
	fillResidualHist  (allIDSTracks,   selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "IDS_res", offline2IDS, SLICE);
	fillEfficiencyHist(selSiOFFTracks, SLICE.sigTracks[i],  SLICE.sigSelect[i], "IDS", offline2IDS, SLICE); 
	fillFakeHist      (allIDSTracks,   SLICE.sigTracks[i],  SLICE.sigSelect[i], "IDS", IDS2offline, SLICE);       
      }
      else {
	fillParameterHist (allIDSTracks,   SLICE.sigTracks[i],  SLICE.sigSelect[i], "IDS_par", SLICE);
	fillResidualHist  (allIDSTracks,   selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "IDS_res_comb", offline2IDS, SLICE);
	fillEfficiencyHist(selSiOFFTracks, SLICE.sigTracks[i],  SLICE.sigSelect[i], "IDS", offline2IDS, SLICE); 
	fillFakeHist      (allIDSTracks,   SLICE.sigTracks[i],  SLICE.sigSelect[i], "IDS", IDS2offline, SLICE);
      }
    }
    
    //+++ SiTrack
    if (SLICE.doSiTrack[i]) {
      //+++ Track matching
      std::map<int, std::vector<int> > offline2SIT = MatchTracks(selSiOFFTracks, allSITTracks,  m_deltaEta, m_deltaPhi, false);
      std::map<int, std::vector<int> > SIT2offline = MatchTracks(allSITTracks, allOFFTracks, m_deltaEta, m_deltaPhi, false);
      
      if (!SLICE.staco){
	//+++ Basic histograms
	fillParameterHist (allSITTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "SIT_par", SLICE);
	fillResidualHist  (allSITTracks,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "SIT_res", offline2SIT, SLICE);
	fillEfficiencyHist(selSiOFFTracks,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "SIT", offline2SIT, SLICE); 
	fillFakeHist      (allSITTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "SIT", SIT2offline, SLICE);    
      }
      else {
	fillParameterHist (allSITTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "SIT_par", SLICE);
	fillResidualHist  (allSITTracks,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "SIT_res_comb", offline2SIT, SLICE);
	fillEfficiencyHist(selSiOFFTracks,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "SIT", offline2SIT, SLICE); 
	fillFakeHist      (allSITTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "SIT", SIT2offline, SLICE);    
      }
    } 
    if (SLICE.doL2Star[i]) {
      //+++ Track matching
      std::map<int, std::vector<int> > offline2L2S_A;
      offline2L2S_A = MatchTracks(selSiOFFTracks, allL2STracks_A,  m_deltaEta, m_deltaPhi, false);
      std::map<int, std::vector<int> > L2S2offline_A;
      L2S2offline_A = MatchTracks(allL2STracks_A, allOFFTracks,    m_deltaEta, m_deltaPhi, false);
      std::map<int, std::vector<int> > offline2L2S_B;
      offline2L2S_B = MatchTracks(selSiOFFTracks, allL2STracks_B,  m_deltaEta, m_deltaPhi, false);
      std::map<int, std::vector<int> > L2S2offline_B;
      L2S2offline_B = MatchTracks(allL2STracks_B, allOFFTracks,    m_deltaEta, m_deltaPhi, false);
      std::map<int, std::vector<int> > offline2L2S_C;
      offline2L2S_C = MatchTracks(selSiOFFTracks, allL2STracks_C,  m_deltaEta, m_deltaPhi, false);
      std::map<int, std::vector<int> > L2S2offline_C;
      L2S2offline_C = MatchTracks(allL2STracks_C, allOFFTracks,    m_deltaEta, m_deltaPhi, false);
      
      if (!SLICE.staco){
	//+++ Basic histograms
	if (allL2STracks_A.size()>0){	
	  fillParameterHist (allL2STracks_A,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_A_par", SLICE);
	  fillResidualHist  (allL2STracks_A,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "L2S_A_res", offline2L2S_A, SLICE);
	  fillEfficiencyHist(selSiOFFTracks,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_A", offline2L2S_A, SLICE); 
	  fillFakeHist      (allL2STracks_A,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_A", L2S2offline_A, SLICE);    
	}
	if (allL2STracks_B.size()>0){
	  fillParameterHist (allL2STracks_B,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_B_par", SLICE);
	  fillResidualHist  (allL2STracks_B,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "L2S_B_res", offline2L2S_B, SLICE);
	  fillEfficiencyHist(selSiOFFTracks,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_B", offline2L2S_B, SLICE); 
	  fillFakeHist      (allL2STracks_B,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_B", L2S2offline_B, SLICE);  
	}
	if (allL2STracks_C.size()>0){
	  fillParameterHist (allL2STracks_C,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_C_par", SLICE);
	  fillResidualHist  (allL2STracks_C,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "L2S_C_res", offline2L2S_C, SLICE);
	  fillEfficiencyHist(selSiOFFTracks,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_C", offline2L2S_C, SLICE); 
	  fillFakeHist      (allL2STracks_C,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_C", L2S2offline_C, SLICE);  
	}
      }
      else {
	if (allL2STracks_A.size()>0){
	  fillParameterHist (allL2STracks_A,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_A_par", SLICE);
	  fillResidualHist  (allL2STracks_A,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "L2S_A_res_comb", offline2L2S_A, SLICE);
	  fillEfficiencyHist(selSiOFFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_A", offline2L2S_A, SLICE); 
	  fillFakeHist      (allL2STracks_A,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_A", L2S2offline_A, SLICE);
	}
	if (allL2STracks_B.size()>0){
	      fillParameterHist (allL2STracks_B,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_B_par", SLICE);
	      fillResidualHist  (allL2STracks_B,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "L2S_B_res_comb", offline2L2S_B, SLICE);
	      fillEfficiencyHist(selSiOFFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_B", offline2L2S_B, SLICE); 
	      fillFakeHist      (allL2STracks_B,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_B", L2S2offline_B, SLICE);
	}
	if (allL2STracks_C.size()>0){
	  fillParameterHist (allL2STracks_C,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_C_par", SLICE);
	  fillResidualHist  (allL2STracks_C,    selSiOFFTracks,  SLICE.sigTracks[i], SLICE.sigSelect[i], "L2S_C_res_comb", offline2L2S_C, SLICE);
	  fillEfficiencyHist(selSiOFFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_C", offline2L2S_C, SLICE); 
	  fillFakeHist      (allL2STracks_C,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "L2S_C", L2S2offline_C, SLICE);    
	}
      }
    }
    //+++ TRTSegFinder
    if (SLICE.doTRTSegF[i]) {
      //+++ Track matching
      std::map<int, std::vector<int> > offline2TSF = MatchTracks(selTrtOFFTracks, allTSFTracks, m_deltaEtaTRT, m_deltaPhi, false);
      std::map<int, std::vector<int> > TSF2offline = MatchTracks(allTSFTracks,    allOFFTracks, m_deltaEtaTRT, m_deltaPhi, false);

      if (!SLICE.staco){
	//+++ Basic histograms
	fillParameterHist (allTSFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "TSF_par", SLICE);
	fillResidualHist  (allTSFTracks,    selTrtOFFTracks, SLICE.sigTracks[i], SLICE.sigSelect[i], "TSF_res", offline2TSF, SLICE);
	fillEfficiencyHist(selTrtOFFTracks, SLICE.sigTracks[i],  SLICE.sigSelect[i], "TSF", offline2TSF, SLICE); 
	fillFakeHist      (allTSFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "TSF", TSF2offline, SLICE);         
      }
      else {
	fillParameterHist (allTSFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "TSF_par", SLICE);
	fillResidualHist  (allTSFTracks,    selTrtOFFTracks, SLICE.sigTracks[i], SLICE.sigSelect[i], "TSF_res_comb", offline2TSF, SLICE);
	fillEfficiencyHist(selTrtOFFTracks, SLICE.sigTracks[i],  SLICE.sigSelect[i], "TSF", offline2TSF, SLICE); 
	fillFakeHist      (allTSFTracks,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "TSF", TSF2offline, SLICE);    	
      }
    }


    if (SLICE.doEF[i]) {
      //+++ Track matching
      std::map<int, std::vector<int> > offline2EF = MatchTracks(selSiOFFTracks_EF, allEFTracks,  m_deltaEta, m_deltaPhi, mu_verbose);
      std::map<int, std::vector<int> > EF2offline = MatchTracks(allEFTracks,    allOFFTracks_EF, m_deltaEta, m_deltaPhi, false);

      if (!SLICE.staco) {
	//+++ Basic histograms
	fillParameterHist (allEFTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EF_par", SLICE);
	fillResidualHist  (allEFTracks,     selSiOFFTracks_EF,  SLICE.sigTracks[i], SLICE.sigSelect[i], "EF_res", offline2EF, SLICE);
	fillEfficiencyHist(selSiOFFTracks_EF,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "EF", offline2EF, SLICE);
	fillFakeHist      (allEFTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EF", EF2offline, SLICE);
      }
      else {
	fillParameterHist (allEFTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EF_par", SLICE);
	fillResidualHist  (allEFTracks,     selSiOFFTracks_EF,  SLICE.sigTracks[i], SLICE.sigSelect[i], "EF_res_comb", offline2EF, SLICE);
	fillEfficiencyHist(selSiOFFTracks_EF,  SLICE.sigTracks[i],  SLICE.sigSelect[i], "EF", offline2EF, SLICE);
	fillFakeHist      (allEFTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EF", EF2offline, SLICE);
      }
    }

    if (SLICE.doEFtrt[i]) {
      //+++ Track matching
      std::map<int, std::vector<int> > offline2EFtrt = MatchTracks(selTrtOFFTracks_EF, allEFtrtTracks,  m_deltaEtaTRT, m_deltaPhi, false);
      std::map<int, std::vector<int> > EFtrt2offline = MatchTracks(allEFtrtTracks, allOFFTracks_EF,     m_deltaEtaTRT, m_deltaPhi, false);

      if (!SLICE.staco) {
	//+++ Basic histograms
	fillParameterHist (allEFtrtTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EFT_par", SLICE);
	fillResidualHist  (allEFtrtTracks,     selTrtOFFTracks_EF,  SLICE.sigTracks[i], SLICE.sigSelect[i], "EFT_res", offline2EFtrt, SLICE);
	fillEfficiencyHist(selTrtOFFTracks_EF,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "EFT", offline2EFtrt, SLICE);
	fillFakeHist      (allEFtrtTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EFT", EFtrt2offline, SLICE);
      }
      else {
	fillParameterHist (allEFtrtTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EFT_par", SLICE);
	fillResidualHist  (allEFtrtTracks,     selTrtOFFTracks_EF,  SLICE.sigTracks[i], SLICE.sigSelect[i], "EFT_res_comb", offline2EFtrt, SLICE);
	fillEfficiencyHist(selTrtOFFTracks_EF,    SLICE.sigTracks[i],  SLICE.sigSelect[i], "EFT", offline2EFtrt, SLICE);
	fillFakeHist      (allEFtrtTracks,     SLICE.sigTracks[i],  SLICE.sigSelect[i], "EFT", EFtrt2offline, SLICE);
      }
    }
  }
 
  ATH_MSG_VERBOSE(" ====== End fillBasicHist() ====== ");
  return sc_ideftrk;
  }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MB physics related plots for monitoring.

void  HLTIDtrkDumpTool::fillPhysMbHist( std::vector< simpleTrack > & tracks, 
					std::string trkType ) {

  ATH_MSG_VERBOSE(" ====== Begin fillPhysMbHist() ====== ");
  //std::cout << "JWH fill minBias Hist ****" << std::endl;

  if ((trkType != "EF") && (trkType != "OFF")) return;

  std::string pTN   = "hpT";
  std::string etaN  = "heta";
  std::string nTrkN = "hnTrk";
  std::string sumEN = "hsumE";
  std::string vsPtN = "hvsPt";
  std::string vsPzN = "hvsPz";

  // Local stuff
  int tmpCount1 = 0;
  double tmpSumE1 = 0;
  TLorentzVector tmpVec1(0.,0.,0.,0.);
  TLorentzVector *tmpvp1;
  
  int tmpCount2 = 0;
  double tmpSumE2 = 0;
  TLorentzVector tmpVec2(0.,0.,0.,0.);
  TLorentzVector *tmpvp2;
  
  // Track loop and selection
  for (unsigned int k = 0; k < tracks.size(); ++k) {
    if (tracks[k].Pt() < m_pMbMinPt) continue;
    if (fabs(tracks[k].Eta()) > m_pMbMaxAbsEta) continue;
    if (fabs(tracks[k].d0) > m_pMbMaxAbsD0) continue;
    if (fabs(tracks[k].z0) > m_pMbMaxAbsZ0) continue;
    
    // B+EC histograms
    hist(pTN+"1_"+trkType,"HLT/IDTrkPhys")->Fill(tracks[k].Pt()/1000.);
    hist(etaN+"1_"+trkType,"HLT/IDTrkPhys")->Fill(tracks[k].Eta());
    tmpSumE1 += tracks[k].E();
    ++tmpCount1;
    tmpvp1 = (TLorentzVector *) &tracks[k];
    tmpVec1 += *tmpvp1;
    
    // Barrel only histograms
    if (m_pMbMaxAbsEta < 1.0) continue;
    if (fabs(tracks[k].Eta())<1.0) {
      hist(pTN+"2_"+trkType,"HLT/IDTrkPhys")->Fill(tracks[k].Pt()/1000.);
      hist(etaN+"2_"+trkType,"HLT/IDTrkPhys")->Fill(tracks[k].Eta());
      tmpSumE2 += tracks[k].E();
      ++tmpCount2;
      tmpvp2 = (TLorentzVector *) &tracks[k];
      tmpVec2 += *tmpvp2;
    }
  }
  
  // Fill event quantities
  hist(nTrkN+"1_"+trkType,"HLT/IDTrkPhys")->Fill(tmpCount1);
  hist(sumEN+"1_"+trkType,"HLT/IDTrkPhys")->Fill(tmpSumE1/1000.);
  hist(vsPtN+"1_"+trkType,"HLT/IDTrkPhys")->Fill(tmpVec1.Pt()/1000.);
  hist(vsPzN+"1_"+trkType,"HLT/IDTrkPhys")->Fill(tmpVec1.Pz()/1000.);
  hist(nTrkN+"2_"+trkType,"HLT/IDTrkPhys")->Fill(tmpCount2);
  hist(sumEN+"2_"+trkType,"HLT/IDTrkPhys")->Fill(tmpSumE2/1000.);
  hist(vsPtN+"2_"+trkType,"HLT/IDTrkPhys")->Fill(tmpVec2.Pt()/1000.);
  hist(vsPzN+"2_"+trkType,"HLT/IDTrkPhys")->Fill(tmpVec2.Pz()/1000.);

  ATH_MSG_VERBOSE(" ====== End fillPhysMbHist() ====== ");

  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void  HLTIDtrkDumpTool::bookMonHist( std::vector< std::string > & variables, 
				     HLTIDDump::SliceVariables & SLICE,
				     std::string sName,
				     bool newRun ) { 
  
  if (newRun) {
    std::string tmp_histName;
    
    //+++ Fill histogram name vectors
    std::vector< std::string >::const_iterator itrTrigVar     = variables.begin();
    std::vector< std::string >::const_iterator itrTrigVarLast = variables.end();
    for (; itrTrigVar != itrTrigVarLast; ++itrTrigVar) {   
      std::vector< std::string >::const_iterator itrSig     = SLICE.sigTracks.begin();
      std::vector< std::string >::const_iterator itrSigLast = SLICE.sigTracks.end();
      for (int index = 0; itrSig != itrSigLast; ++itrSig, ++index) {
	
	if (SLICE.doIDSCAN[index]) {
	  //std::cout << "JWH dO IDSCAN = true for SLICE " << SLICE.name << std::endl;
	  SLICE.histKeeper.par_IDS.push_back(makeHistName( *itrSig, SLICE.sigSelect[index], *itrTrigVar,"IDS", SLICE));   
	}
	
	if (SLICE.doSiTrack[index]) {
	  //std::cout << "JWH dO SITRACK = true for SLICE " << SLICE.name << std::endl;
	  SLICE.histKeeper.par_SIT.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"SIT", SLICE)); 
	}

	if (SLICE.doL2Star[index]) {
	  //	  std::cout << "JWH dO L2STAR = true for SLICE " << SLICE.name << std::endl;
	  //	  std::cout << "Hist Name = " << makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"L2S_A", SLICE) << std::endl;
	  SLICE.histKeeper.par_L2S.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"L2S_A", SLICE));
	  SLICE.histKeeper.par_L2S.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"L2S_B", SLICE));
	  SLICE.histKeeper.par_L2S.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"L2S_C", SLICE));
	}
	
	if (SLICE.doTRTSegF[index]) {
	  //std::cout << "JWH dO TSF = true for SLICE " << SLICE.name << std::endl;
	  SLICE.histKeeper.par_TSF.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"TSF", SLICE));   
	}
	
	if (SLICE.doEF[index]) {
	  //std::cout << "JWH dO EF = true for SLICE " << SLICE.name << std::endl;
	  SLICE.histKeeper.par_EF.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"EF", SLICE));  
	}
	if (SLICE.doEFtrt[index]) {
	  //std::cout << "JWH dO EFT = true for SLICE " << SLICE.name << std::endl;
	  SLICE.histKeeper.par_EFT.push_back(makeHistName( *itrSig, SLICE.sigSelect[index],*itrTrigVar,"EFT", SLICE));  
	}
	
	tmp_histName = makeHistName( *itrSig, SLICE.sigSelect[index], *itrTrigVar, "OFF_par", SLICE); 
	SLICE.histKeeper.par_OFF.push_back(tmp_histName); 
	if (!m_data) {
	  tmp_histName = makeHistName(*itrSig, SLICE.sigSelect[index],*itrTrigVar, "TRU_par", SLICE); 
	  SLICE.histKeeper.par_TRU.push_back(tmp_histName); 
	}
      }
    }
  }
  //+++ pT ranges
  double tmp_maxPt    = 50000.;
  double tmp_absResPt = 0.0005;
  if (sName == "cos") tmp_absResPt = 0.005;
  //+++ Eta ranges
  double tmp_maxEta    = 3.;
  double tmp_absResEta = 0.05; // 0.0005;
  if (sName == "cos") tmp_absResEta = 0.05;
  //+++ Phi ranges
  double tmp_maxPhi    = 3.15;
  double tmp_absResPhi = 0.01; // 0.0001;
  if (sName == "cos") tmp_absResPhi = 0.01;
  //+++ D0 ranges
  //  double tmp_maxD0    = 6.0;
  //  double tmp_absResD0 = 0.01; // 0.0001;
  //  if (sName == "cos") tmp_absResD0 = 0.01;
  //+++ Z0 ranges
  //  double tmp_maxZ0    = 100.0;
  //  double tmp_absResZ0 = 0.01; // 0.0001;
  //  if (sName == "cos") tmp_absResZ0 = 0.01;

  const int pTBins        = 50;
  const int pTLsEffBins   = 20;
  const int pTResBins     = 200;
  const int etaBins       = 30;
  const int etaLsEffBins  = 12;
  const int etaResBins    = 300;
  const int phiBins       = 34;
  const int phiLsEffBins  = 13;
  const int phiResBins    = 100;
  /*const int d0Bins       = 50;
  const int d0LsEffBins  = 10;
  const int d0ResBins    = 100;
  const int z0Bins       = 50;
  const int z0LsEffBins  = 10;
  const int z0ResBins    = 100;*/
  const double tmp_effMin = 0.;
  const double tmp_effMax = 110.; 
  const int roiBins       = 100;
  const double roidMax     = 0.5;

  //+++ Book histograms

  // loop round hist.par_IDS and make par, res, eff, fake histo names from that
  // Loop around all the hist.par vectors...
  std::vector <std::string> allNames;
  appendVector(SLICE.histKeeper,allNames);

  std::string stacoName = "";
  if (SLICE.staco){
    stacoName = "_comb";
  }

  for (unsigned int i=0; i < allNames.size(); ++i) {
    
    if (allNames[i].find("_pT_") != std::string::npos) {
      //      std::cout << "Name = " << allNames[i] << std::endl;
      if (newRun) {
	this->setCurrentMonGroup("HLT/IDTrkDump");
	addHistogram(new TH1F((allNames[i] + "_par" + stacoName).c_str(),      (allNames[i] + "_par" + stacoName).c_str(),       pTBins, 0., tmp_maxPt)); // JWH
	
	addHistogram(new TH1F((allNames[i] + "_res" + stacoName).c_str(),      (allNames[i] + "_res" + stacoName).c_str(),       pTResBins, -tmp_absResPt, tmp_absResPt)); // JWH
	this->setCurrentMonGroup("HLT/IDTrkDumpEff");
	addHistogram(new TProfile((allNames[i] + "_eff" + stacoName).c_str(),      (allNames[i] + "_eff" + stacoName).c_str(),       pTBins, 0., tmp_maxPt, tmp_effMin, tmp_effMax));
	addHistogram(new TProfile((allNames[i] + "_fake" + stacoName).c_str(),     (allNames[i] + "_fake" + stacoName).c_str(),      pTBins, 0., tmp_maxPt, tmp_effMin, tmp_effMax));
      }
      this->setCurrentMonGroup("HLT/IDTrkDumpLS");
      addHistogram(new TH1F((allNames[i] + "_par" + stacoName).c_str(),      (allNames[i] + "_par" + stacoName).c_str(),       pTBins, 0., tmp_maxPt));
      addHistogram(new TH1F((allNames[i] + "_res" + stacoName).c_str(),      (allNames[i] + "_res" + stacoName).c_str(),       pTResBins, -tmp_absResPt, tmp_absResPt));
      this->setCurrentMonGroup("HLT/IDTrkDumpEffLS");
      addHistogram(new TProfile((allNames[i] + "_eff" + stacoName).c_str(),      (allNames[i] + "_eff" + stacoName).c_str(),       pTLsEffBins, 0., tmp_maxPt, tmp_effMin, tmp_effMax));
      addHistogram(new TProfile((allNames[i] + "_fake" + stacoName).c_str(),     (allNames[i] + "_fake" + stacoName).c_str(),      pTLsEffBins, 0., tmp_maxPt, tmp_effMin, tmp_effMax));
      if (newRun) {
	this->setCurrentMonGroup("HLT/IDTrkDumpEffDebug");
	addHistogram(new TH1F((allNames[i] + "_effnum" + stacoName).c_str(),   (allNames[i] + "_effnum" + stacoName).c_str(),    pTBins, 0., tmp_maxPt));
	addHistogram(new TH1F((allNames[i] + "_effdenom" + stacoName).c_str(), (allNames[i] + "_effdenom" + stacoName).c_str(),  pTBins, 0., tmp_maxPt));
	addHistogram(new TH1F((allNames[i] + "_fakenum" + stacoName).c_str(),  (allNames[i] + "_fakenum" + stacoName).c_str(),   pTBins, 0., tmp_maxPt));
	addHistogram(new TH1F((allNames[i] + "_fakedenom" + stacoName).c_str(),(allNames[i] + "_fakedenom" + stacoName).c_str(), pTBins, 0., tmp_maxPt));
      }
    } else if (allNames[i].find("_eta_") != std::string::npos) {
      if (newRun) {
	this->setCurrentMonGroup("HLT/IDTrkDump");
	addHistogram(new TH1F((allNames[i] + "_par" + stacoName).c_str(),      (allNames[i] + "_par" + stacoName).c_str(),       etaBins,    -tmp_maxEta,    tmp_maxEta));
	addHistogram(new TH1F((allNames[i] + "_res" + stacoName).c_str(),      (allNames[i] + "_res" + stacoName).c_str(),       etaResBins, -tmp_absResEta, tmp_absResEta));
	addHistogram(new TH1F((allNames[i] + "_roi" + stacoName).c_str(),      (allNames[i] + "_roi" + stacoName).c_str(),       etaBins,    -tmp_maxEta,    tmp_maxEta));
	addHistogram(new TH1F((allNames[i] + "_roid" + stacoName).c_str(),     (allNames[i] + "_roid" + stacoName).c_str(),      roiBins,    -roidMax,       roidMax));
	this->setCurrentMonGroup("HLT/IDTrkDumpEff");
	addHistogram(new TProfile((allNames[i] + "_eff" + stacoName).c_str(),      (allNames[i] + "_eff" + stacoName).c_str(),       etaBins, -tmp_maxEta, tmp_maxEta, tmp_effMin, tmp_effMax));
	addHistogram(new TProfile((allNames[i] + "_fake" + stacoName).c_str(),     (allNames[i] + "_fake" + stacoName).c_str(),      etaBins, -tmp_maxEta, tmp_maxEta, tmp_effMin, tmp_effMax));
      }
      this->setCurrentMonGroup("HLT/IDTrkDumpLS");
      addHistogram(new TH1F((allNames[i] + "_par" + stacoName).c_str(),      (allNames[i] + "_par" + stacoName).c_str(),       etaBins,    -tmp_maxEta,    tmp_maxEta));
      addHistogram(new TH1F((allNames[i] + "_res" + stacoName).c_str(),      (allNames[i] + "_res" + stacoName).c_str(),       etaResBins, -tmp_absResEta, tmp_absResEta));
      addHistogram(new TH1F((allNames[i] + "_roi" + stacoName).c_str(),      (allNames[i] + "_roi" + stacoName).c_str(),       etaBins,    -tmp_maxEta,    tmp_maxEta));
      addHistogram(new TH1F((allNames[i] + "_roid" + stacoName).c_str(),     (allNames[i] + "_roid" + stacoName).c_str(),      roiBins,    -roidMax,       roidMax));
      this->setCurrentMonGroup("HLT/IDTrkDumpEffLS");
      addHistogram(new TProfile((allNames[i] + "_eff" + stacoName).c_str(),      (allNames[i] + "_eff" + stacoName).c_str(),       etaLsEffBins, -tmp_maxEta, tmp_maxEta, tmp_effMin, tmp_effMax));
      addHistogram(new TProfile((allNames[i] + "_fake" + stacoName).c_str(),     (allNames[i] + "_fake" + stacoName).c_str(),      etaLsEffBins, -tmp_maxEta, tmp_maxEta, tmp_effMin, tmp_effMax));
      if (newRun) {
	this->setCurrentMonGroup("HLT/IDTrkDumpEffDebug");
	addHistogram(new TH1F((allNames[i] + "_effnum" + stacoName).c_str(),   (allNames[i] + "_effnum" + stacoName).c_str(),    etaBins, -tmp_maxEta, tmp_maxEta));
	addHistogram(new TH1F((allNames[i] + "_effdenom" + stacoName).c_str(), (allNames[i] + "_effdenom" + stacoName).c_str(),  etaBins, -tmp_maxEta, tmp_maxEta));
	addHistogram(new TH1F((allNames[i] + "_fakenum" + stacoName).c_str(),  (allNames[i] + "_fakenum" + stacoName).c_str(),   etaBins, -tmp_maxEta, tmp_maxEta));
	addHistogram(new TH1F((allNames[i] + "_fakedenom" + stacoName).c_str(),(allNames[i] + "_fakedenom" + stacoName).c_str(), etaBins, -tmp_maxEta, tmp_maxEta));
      }
    } else if (allNames[i].find("_phi_") != std::string::npos) {
      if (newRun) {
	this->setCurrentMonGroup("HLT/IDTrkDump");
	addHistogram(new TH1F((allNames[i] + "_par" + stacoName).c_str(),      (allNames[i] + "_par" + stacoName).c_str(),       phiBins,    -tmp_maxPhi,     tmp_maxPhi));
	addHistogram(new TH1F((allNames[i] + "_res" + stacoName).c_str(),      (allNames[i] + "_res" + stacoName).c_str(),       phiResBins, -tmp_absResPhi,  tmp_absResPhi));
	addHistogram(new TH1F((allNames[i] + "_roi" + stacoName).c_str(),      (allNames[i] + "_roi" + stacoName).c_str(),       phiBins,    -tmp_maxPhi,     tmp_maxPhi));
	addHistogram(new TH1F((allNames[i] + "_roid" + stacoName).c_str(),     (allNames[i] + "_roid" + stacoName).c_str(),      roiBins,    -roidMax,        roidMax));
	this->setCurrentMonGroup("HLT/IDTrkDumpEff");
	addHistogram(new TProfile((allNames[i] + "_eff" + stacoName).c_str(),      (allNames[i] + "_eff" + stacoName).c_str(),       phiBins, -tmp_maxPhi, tmp_maxPhi, tmp_effMin, tmp_effMax));
	addHistogram(new TProfile((allNames[i] + "_fake" + stacoName).c_str(),     (allNames[i] + "_fake" + stacoName).c_str(),      phiBins, -tmp_maxPhi, tmp_maxPhi, tmp_effMin, tmp_effMax));
      }
      this->setCurrentMonGroup("HLT/IDTrkDumpLS");
      addHistogram(new TH1F((allNames[i] + "_par" + stacoName).c_str(),      (allNames[i] + "_par" + stacoName).c_str(),       phiBins,    -tmp_maxPhi,     tmp_maxPhi));
      addHistogram(new TH1F((allNames[i] + "_res" + stacoName).c_str(),      (allNames[i] + "_res" + stacoName).c_str(),       phiResBins, -tmp_absResPhi,  tmp_absResPhi));
      addHistogram(new TH1F((allNames[i] + "_roi" + stacoName).c_str(),      (allNames[i] + "_roi" + stacoName).c_str(),       phiBins,    -tmp_maxPhi,     tmp_maxPhi));
      addHistogram(new TH1F((allNames[i] + "_roid" + stacoName).c_str(),     (allNames[i] + "_roid" + stacoName).c_str(),      roiBins,    -roidMax,        roidMax));
      this->setCurrentMonGroup("HLT/IDTrkDumpEffLS");
      addHistogram(new TProfile((allNames[i] + "_eff" + stacoName).c_str(),      (allNames[i] + "_eff" + stacoName).c_str(),       phiLsEffBins, -tmp_maxPhi, tmp_maxPhi, tmp_effMin, tmp_effMax));
      addHistogram(new TProfile((allNames[i] + "_fake" + stacoName).c_str(),     (allNames[i] + "_fake" + stacoName).c_str(),      phiLsEffBins, -tmp_maxPhi, tmp_maxPhi, tmp_effMin, tmp_effMax));
      if (newRun) {
	this->setCurrentMonGroup("HLT/IDTrkDumpEffDebug");
	addHistogram(new TH1F((allNames[i] + "_effnum" + stacoName).c_str(),   (allNames[i] + "_effnum" + stacoName).c_str(),    phiBins, -tmp_maxPhi, tmp_maxPhi));
	addHistogram(new TH1F((allNames[i] + "_effdenom" + stacoName).c_str(), (allNames[i] + "_effdenom" + stacoName).c_str(),  phiBins, -tmp_maxPhi, tmp_maxPhi));
	addHistogram(new TH1F((allNames[i] + "_fakenum" + stacoName).c_str(),  (allNames[i] + "_fakenum" + stacoName).c_str(),   phiBins, -tmp_maxPhi, tmp_maxPhi));
	addHistogram(new TH1F((allNames[i] + "_fakedenom" + stacoName).c_str(),(allNames[i] + "_fakedenom" + stacoName).c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi));
      }
    }
  }


  if (!SLICE.staco){ 
    for (unsigned int i=0; i < SLICE.histKeeper.par_OFF.size(); ++i) {
      if (SLICE.histKeeper.par_OFF[i].find("pT_OFF_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F(SLICE.histKeeper.par_OFF[i].c_str(), SLICE.histKeeper.par_OFF[i].c_str(), pTBins, 0., tmp_maxPt),"HLT/IDTrkDump");
	addHistogram(new TH1F(SLICE.histKeeper.par_OFF[i].c_str(), SLICE.histKeeper.par_OFF[i].c_str(), pTBins, 0., tmp_maxPt),"HLT/IDTrkDumpLS");
      } else if (SLICE.histKeeper.par_OFF[i].find("eta_OFF_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F(SLICE.histKeeper.par_OFF[i].c_str(), SLICE.histKeeper.par_OFF[i].c_str(), etaBins, -tmp_maxEta, tmp_maxEta),"HLT/IDTrkDump");
	addHistogram(new TH1F(SLICE.histKeeper.par_OFF[i].c_str(), SLICE.histKeeper.par_OFF[i].c_str(), etaBins, -tmp_maxEta, tmp_maxEta),"HLT/IDTrkDumpLS");
      } else if (SLICE.histKeeper.par_OFF[i].find("phi_OFF_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F(SLICE.histKeeper.par_OFF[i].c_str(), SLICE.histKeeper.par_OFF[i].c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi),"HLT/IDTrkDump");
	addHistogram(new TH1F(SLICE.histKeeper.par_OFF[i].c_str(), SLICE.histKeeper.par_OFF[i].c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi),"HLT/IDTrkDumpLS");
      } 
    }
  }

  if (SLICE.staco){ 
    for (unsigned int i=0; i < SLICE.histKeeper.par_OFF.size(); ++i) {
      if (SLICE.histKeeper.par_OFF[i].find("pT_OFF_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F((SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), (SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), pTBins, 0., tmp_maxPt),"HLT/IDTrkDump");
	addHistogram(new TH1F((SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), (SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), pTBins, 0., tmp_maxPt),"HLT/IDTrkDumpLS");
      } else if (SLICE.histKeeper.par_OFF[i].find("eta_OFF_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F((SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), (SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), etaBins, -tmp_maxEta, tmp_maxEta),"HLT/IDTrkDump");
	addHistogram(new TH1F((SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), (SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), etaBins, -tmp_maxEta, tmp_maxEta),"HLT/IDTrkDumpLS");
      } else if (SLICE.histKeeper.par_OFF[i].find("phi_OFF_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F((SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), (SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi),"HLT/IDTrkDump");
	addHistogram(new TH1F((SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), (SLICE.histKeeper.par_OFF[i] + "_comb").c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi),"HLT/IDTrkDumpLS");
      } 
    }
  }
  

  
  if (!m_data) {
    for (unsigned int i=0; i < SLICE.histKeeper.par_TRU.size(); ++i) {
      if (SLICE.histKeeper.par_TRU[i].find("pT_TRU_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F(SLICE.histKeeper.par_TRU[i].c_str(), SLICE.histKeeper.par_TRU[i].c_str(), pTBins, 0., tmp_maxPt),"HLT/IDTrkDump");
	addHistogram(new TH1F(SLICE.histKeeper.par_TRU[i].c_str(), SLICE.histKeeper.par_TRU[i].c_str(), pTBins, 0., tmp_maxPt),"HLT/IDTrkDumpLS");
      } else if (SLICE.histKeeper.par_TRU[i].find("eta_TRU_par") != std::string::npos) {
	if (newRun) addHistogram(new TH1F(SLICE.histKeeper.par_TRU[i].c_str(), SLICE.histKeeper.par_TRU[i].c_str(), etaBins, -tmp_maxEta, tmp_maxEta),"HLT/IDTrkDump");
	addHistogram(new TH1F(SLICE.histKeeper.par_TRU[i].c_str(), SLICE.histKeeper.par_TRU[i].c_str(), etaBins, -tmp_maxEta, tmp_maxEta),"HLT/IDTrkDumpLS");
      } else if (SLICE.histKeeper.par_TRU[i].find("phi_TRU_par") != std::string::npos) { 
	if (newRun) addHistogram(new TH1F(SLICE.histKeeper.par_TRU[i].c_str(), SLICE.histKeeper.par_TRU[i].c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi),"HLT/IDTrkDump");
	addHistogram(new TH1F(SLICE.histKeeper.par_TRU[i].c_str(), SLICE.histKeeper.par_TRU[i].c_str(), phiBins, -tmp_maxPhi, tmp_maxPhi),"HLT/IDTrkDumpLS");
      }
    }
  }
  

  return;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

void  HLTIDtrkDumpTool::fillParameterHist( std::vector< simpleTrack > & tracks, 
					   const std::string & sigTracks,
					   const std::string & sigSelect,
					   const std::string & trkType, 
                                           HLTIDDump::SliceVariables & SLICE) {
 
  //  std::cout << "JWH fill Paramter Hist" << std::endl;
  // loop around parameters we want to plot

  

  for (unsigned int j = 0; j<m_Variables.size(); ++j) {
 
    // make histogram name from signature, parameter and trkType

    const std::string histName = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType, SLICE);// JWH
    const std::string histName2 = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_comb", SLICE);
    std::string roiPf = trkType; 
    std::string roiPfcomb = trkType;
    std::string roiPfcomb2 = trkType;
    const std::string roiName  = makeHistName(sigTracks, sigSelect, m_Variables[j] ,roiPf.replace(roiPf.length() - 3, 3, "roi"), SLICE); //JWH
    const std::string roidName = makeHistName(sigTracks, sigSelect, m_Variables[j] ,roiPf+"d", SLICE); // JWH
    const std::string roiNameComb  = makeHistName(sigTracks, sigSelect, m_Variables[j] , roiPfcomb.replace(roiPfcomb.length() -3, 3 , "roi_comb"), SLICE); //JWH
    const std::string roidNameComb = makeHistName(sigTracks, sigSelect, m_Variables[j] , roiPfcomb2.replace(roiPfcomb2.length() -3, 3, "roid_comb"), SLICE); // JWH
    
    // std::cout << "JWH tracks size = " << tracks.size() << std::endl;

    for (unsigned int k = 0; k < tracks.size(); ++k) {
      // the hist function throws an exception if the histogram does not exist
      if (!SLICE.staco){
	//std::cout << " Block1 start" << std::endl;
	if (m_Variables[j] == "pT")  hist(histName,"HLT/IDTrkDump")->Fill(tracks[k].Pt());
	if (m_Variables[j] == "eta") hist(histName,"HLT/IDTrkDump")->Fill(tracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histName,"HLT/IDTrkDump")->Fill(tracks[k].Phi());
	if (m_Variables[j] == "pT")  hist(histName,"HLT/IDTrkDumpLS")->Fill(tracks[k].Pt());
	if (m_Variables[j] == "eta") hist(histName,"HLT/IDTrkDumpLS")->Fill(tracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histName,"HLT/IDTrkDumpLS")->Fill(tracks[k].Phi());
	//	std::cout << " Block1 end" << std::endl;
      }
      else {
	//	std::cout << " Block2 start" << std::endl;
	if (m_Variables[j] == "pT")  hist(histName2,"HLT/IDTrkDump")->Fill(tracks[k].Pt());
	if (m_Variables[j] == "eta") hist(histName2,"HLT/IDTrkDump")->Fill(tracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histName2,"HLT/IDTrkDump")->Fill(tracks[k].Phi());
	if (m_Variables[j] == "pT")  hist(histName2,"HLT/IDTrkDumpLS")->Fill(tracks[k].Pt());
	if (m_Variables[j] == "eta") hist(histName2,"HLT/IDTrkDumpLS")->Fill(tracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histName2,"HLT/IDTrkDumpLS")->Fill(tracks[k].Phi());
	//	std::cout << " Block2 end" << std::endl;
      }
            
      //+++ RoI plots (only for trigger tracks).
      if (trkType != "OFF_par") {
	if (!(SLICE.staco)) { //JWH
	  //	std::cout << " Block3 start" << std::endl;
	  if (m_Variables[j] == "eta") hist(roiName,"HLT/IDTrkDump")->Fill(tracks[k].etaR);
	  if (m_Variables[j] == "eta") hist(roidName,"HLT/IDTrkDump")->Fill(tracks[k].etaR - tracks[k].Eta());
	  if (m_Variables[j] == "phi") hist(roiName,"HLT/IDTrkDump")->Fill(tracks[k].phiR);
	  if (m_Variables[j] == "phi") hist(roidName,"HLT/IDTrkDump")->Fill(tracks[k].phiR - tracks[k].Phi());
	  //	std::cout << " Block3 end" << std::endl;
	}
	else {
	  //	std::cout << " Block4 start" << std::endl;
	  if (m_Variables[j] == "eta") hist(roiNameComb,"HLT/IDTrkDump")->Fill(tracks[k].etaR);
	  if (m_Variables[j] == "eta") hist(roidNameComb,"HLT/IDTrkDump")->Fill(tracks[k].etaR - tracks[k].Eta());
	  if (m_Variables[j] == "phi") hist(roiNameComb,"HLT/IDTrkDump")->Fill(tracks[k].phiR);
	  if (m_Variables[j] == "phi") hist(roidNameComb,"HLT/IDTrkDump")->Fill(tracks[k].phiR - tracks[k].Phi());
	  //	std::cout << " Block4 end" << std::endl;
	} // end of else loop
      } // end of trk != OFF_par loop
    } // end of track loop
  } // end of variable loop
  //std::cout << "JWH END fill Paramter Hist ****" << std::endl;
  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void  HLTIDtrkDumpTool::fillResidualHist( std::vector< simpleTrack > & recoTracks, 
					  std::vector< simpleTrack > & truthTracks,
					  const std::string & sigTracks,
					  const std::string & sigSelect,
					  const std::string & trkType,
					  std::map<int, std::vector<int> > & truth2reco,
					  HLTIDDump::SliceVariables & SLICE) { 
  
  //  std::cout << "JWH fill Res Hist" << std::endl;
  // loop around parameters we want to plot
  for (unsigned int j = 0; j<m_Variables.size(); ++j) { 
    
    // make histogram name from signature, parameter and trkType
    const std::string histName = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType, SLICE); //JWH
    
     for (unsigned int k = 0; k < truthTracks.size(); ++k) {
      
      //+++ Check match
      int irec = -1;
      const std::map<int,std::vector<int> >::iterator iter = truth2reco.find(k);
      if (iter!=truth2reco.end()){
	if (iter->second.size() > 0) irec = iter->second.at(0);
      } 
    
      if ( irec >= 0 ) {
	if (m_Variables[j] == "pT")  hist(histName,"HLT/IDTrkDump")->Fill(1.0/truthTracks[k].Pt()  - 1.0/recoTracks[irec].Pt()); 
	if (m_Variables[j] == "eta") hist(histName,"HLT/IDTrkDump")->Fill(truthTracks[k].Eta() - recoTracks[irec].Eta());
	if (m_Variables[j] == "phi") hist(histName,"HLT/IDTrkDump")->Fill(truthTracks[k].Phi() - recoTracks[irec].Phi());
	if (m_Variables[j] == "pT")  hist(histName,"HLT/IDTrkDumpLS")->Fill(1.0/truthTracks[k].Pt()  - 1.0/recoTracks[irec].Pt()); 
	if (m_Variables[j] == "eta") hist(histName,"HLT/IDTrkDumpLS")->Fill(truthTracks[k].Eta() - recoTracks[irec].Eta());
	if (m_Variables[j] == "phi") hist(histName,"HLT/IDTrkDumpLS")->Fill(truthTracks[k].Phi() - recoTracks[irec].Phi());
      }
    }
  }

  //std::cout << "JWH end fill Res Hist ****" << std::endl;
  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

void  HLTIDtrkDumpTool::fillEfficiencyHist( std::vector< simpleTrack > & truthTracks,
					    const std::string & sigTracks,
					    const std::string & sigSelect,
					    const std::string & trkType,
					    std::map<int, std::vector<int> > & truth2reco, 
					    HLTIDDump::SliceVariables & SLICE) { 
  
  this->setCurrentMonGroup("HLT/IDTrkDumpEffDebug");

  //  std::cout << "JWH fill Eff Hist" << std::endl;
  //  std::cout << "JWH Track Type = " << trkType << std::endl;
  // loop around parameters we want to plot
  for (unsigned int j = 0; j<m_Variables.size(); ++j) { 

    std::string histNameNom;
    std::string histNameDenom;
    std::string histNameEff;
    
    if (!SLICE.staco){
      // make histogram names from signature, parameter and trkType
      histNameNom =   makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_effnum", SLICE); 
      histNameDenom = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_effdenom", SLICE);
      histNameEff = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_eff", SLICE); 
    }
    else {
      histNameNom =   makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_effnum_comb", SLICE); 
      histNameDenom = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_effdenom_comb", SLICE);
      histNameEff = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_eff_comb", SLICE); 
    }
    //    if (truthTracks.size()>0) std::cout << "JWH Truth tracks size = " << truthTracks.size() << std::endl;
    for (unsigned int k = 0; k < truthTracks.size(); ++k) {

      if (m_Variables[j] == "pT")  hist(histNameDenom)->Fill(truthTracks[k].Pt()); 
      if (m_Variables[j] == "eta") hist(histNameDenom)->Fill(truthTracks[k].Eta());
      if (m_Variables[j] == "phi") hist(histNameDenom)->Fill(truthTracks[k].Phi());
      
      //+++ Check match
      bool tmpPassed = false;
      const std::map<int,std::vector<int> >::iterator iter = truth2reco.find(k);
      if (iter!=truth2reco.end()){
	if (iter->second.size() > 0) tmpPassed = true ;
      } 
      
      if ( tmpPassed ) {
	//if (truthTracks.size()>0 && trkType == "L2S_A") {
	  //	  std::cout << "EFFICIENT! " << std::endl;
	  //	  std::cout << "Truth track = " << truthTracks[k].Pt() << std::endl;}
	if (m_Variables[j] == "pT")  hist(histNameNom)->Fill(truthTracks[k].Pt()); 
	if (m_Variables[j] == "eta") hist(histNameNom)->Fill(truthTracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histNameNom)->Fill(truthTracks[k].Phi());
	
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEff"))->Fill(truthTracks[k].Pt(),100.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEff"))->Fill(truthTracks[k].Eta(),100.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEff"))->Fill(truthTracks[k].Phi(),100.,1.);
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEffLS"))->Fill(truthTracks[k].Pt(),100.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEffLS"))->Fill(truthTracks[k].Eta(),100.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEffLS"))->Fill(truthTracks[k].Phi(),100.,1.);
      } else {
	//if (truthTracks.size()>0  && trkType == "L2S_A"){
	//	  std::cout << "INEFFICIENT! " << std::endl;
	//	  std::cout << "Truth track = " << truthTracks[k].Pt() << std::endl;
	//}
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEff"))->Fill(truthTracks[k].Pt(),0.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEff"))->Fill(truthTracks[k].Eta(),0.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEff"))->Fill(truthTracks[k].Phi(),0.,1.);
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEffLS"))->Fill(truthTracks[k].Pt(),0.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEffLS"))->Fill(truthTracks[k].Eta(),0.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameEff,"HLT/IDTrkDumpEffLS"))->Fill(truthTracks[k].Phi(),0.,1.);
      }
      
    }
  }
  
  //std::cout << "JWH end fill Eff Hist ****" << std::endl;
  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

void  HLTIDtrkDumpTool::fillFakeHist( std::vector< simpleTrack > & recoTracks, 
				      const std::string & sigTracks,
				      const std::string & sigSelect,
				      const std::string & trkType,
				      std::map<int, std::vector<int> > & reco2truth,
				      HLTIDDump::SliceVariables & SLICE ) { 

  //  std::cout << "JWH fill Fake Hist" << std::endl;
  this->setCurrentMonGroup("HLT/IDTrkDumpEffDebug");

  //+++ Loop around parameters we want to plot
  for (unsigned int j = 0; j<m_Variables.size(); ++j) { 

    //+++ Make histogram names from signature, parameter and trkType
   
    const std::string histNameNom =   makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_fakenum", SLICE); //JWH
    const std::string histNameDenom = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_fakedenom", SLICE); //JWH
    const std::string histNameFake = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_fake", SLICE); //JWH
    const std::string histNameNomComb =   makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_fakenum_comb", SLICE); //JWH
    const std::string histNameDenomComb = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_fakedenom_comb", SLICE); //JWH
    const std::string histNameFakeComb = makeHistName(sigTracks, sigSelect, m_Variables[j] ,trkType + "_fake_comb", SLICE); //JWH
    
 
    for (unsigned int k = 0; k < recoTracks.size(); ++k) {
      //      std::cout << " Block1 start" << std::endl;
      if (m_Variables[j] == "pT")  hist(histNameDenom)->Fill(recoTracks[k].Pt()); 
      if (m_Variables[j] == "eta") hist(histNameDenom)->Fill(recoTracks[k].Eta());
      if (m_Variables[j] == "phi") hist(histNameDenom)->Fill(recoTracks[k].Phi());
      //      std::cout << " Block1 end" << std::endl;
      //+++ Check match
      bool tmpPassed = false;
      const std::map<int,std::vector<int> >::iterator iter = reco2truth.find(k);
      if (iter!=reco2truth.end()){
	if (iter->second.size() > 0) tmpPassed = true ;
      } 
      
      if ( !tmpPassed && !SLICE.staco ) {
	//	std::cout << " Block2 start" << std::endl;
	if (m_Variables[j] == "pT")  hist(histNameNom)->Fill(recoTracks[k].Pt()); 
	if (m_Variables[j] == "eta") hist(histNameNom)->Fill(recoTracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histNameNom)->Fill(recoTracks[k].Phi());

	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Pt(),100.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Eta(),100.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Phi(),100.,1.);
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Pt(),100.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Eta(),100.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Phi(),100.,1.);
	//	std::cout << " Block2 end" << std::endl;
      } 
      if (tmpPassed && !SLICE.staco) {
	//std::cout << " Block3 start" << std::endl;
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Pt(),0.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Eta(),0.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Phi(),0.,1.);
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Pt(),0.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Eta(),0.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFake,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Phi(),0.,1.);
	//std::cout << " Block3 end" << std::endl;
      }
      if ( !tmpPassed && SLICE.staco ) {
	//	std::cout << " Block4 start" << std::endl;
	if (m_Variables[j] == "pT")  hist(histNameNomComb)->Fill(recoTracks[k].Pt()); 
	if (m_Variables[j] == "eta") hist(histNameNomComb)->Fill(recoTracks[k].Eta());
	if (m_Variables[j] == "phi") hist(histNameNomComb)->Fill(recoTracks[k].Phi());

	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Pt(),100.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Eta(),100.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Phi(),100.,1.);
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Pt(),100.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Eta(),100.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Phi(),100.,1.);
	//std::cout << " Block4 end" << std::endl;
      } 
      if (tmpPassed && SLICE.staco) {
	//std::cout << " Block5 start" << std::endl;
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Pt(),0.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Eta(),0.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEff"))->Fill(recoTracks[k].Phi(),0.,1.);
	if (m_Variables[j] == "pT")  ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Pt(),0.,1.); 
	if (m_Variables[j] == "eta") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Eta(),0.,1.);
	if (m_Variables[j] == "phi") ((TProfile *) hist(histNameFakeComb,"HLT/IDTrkDumpEffLS"))->Fill(recoTracks[k].Phi(),0.,1.);
	//std::cout << " Block5 end" << std::endl;
      }
    }
  }
  //std::cout << "JWH end fill Fake Hist ****" << std::endl;
  return;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL (?)

std::vector< simpleTrack >  HLTIDtrkDumpTool::inSideRoi( std::vector< simpleTrack > & trackVector, 
							 std::vector< Trig::Feature<TrigRoiDescriptor> > & vecRoi, 
							 float etaRoiHalf, float phiRoiHalf, std::string alg) { 
  std::vector< simpleTrack > outputVector;

  if(alg == "Mu2") std::cout << "Incorrect Slice name" << std::endl; 

  //  std::vector< Trig::Feature<TrigRoiDescriptor> > tempvecRoi;
  //  tempvecRoi.push_back(vecRoi[0]);

  for (unsigned int i = 0; i<trackVector.size(); ++i) {   
    int counter = 0;
    bool matched = false;

    std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiItr = vecRoi.begin();
    std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roiLast = vecRoi.end();
    
    for ( ; roiItr != roiLast && !matched; ++roiItr) {
      const TrigRoiDescriptor* roi = (*roiItr).cptr();
      
      //etaRoiHalf = roi->etaHalfWidth();
      //phiRoiHalf = roi->phiHalfWidth();
      
      bool passeta = false;
      if (etaRoiHalf > 0.0F) {
	passeta = (fabs(trackVector[i].Eta() - roi->eta()) < etaRoiHalf);
      } else {
        passeta = (trackVector[i].Eta()*roi->eta() > 0.0);
      }
      float tmp_dphi = fabs(trackVector[i].Phi() - roi->phi());

      if (tmp_dphi > M_PI) tmp_dphi = 2 * M_PI - tmp_dphi;
      if (passeta && (tmp_dphi <= phiRoiHalf)) {
	trackVector[i].etaR = roi->eta();
	trackVector[i].phiR = roi->phi();
	outputVector.push_back(trackVector[i]);
	//+++ break out of the loop so that the same track doesn't get added again
	matched = true;
      }
      if (!(passeta && (tmp_dphi <= phiRoiHalf))){
	++counter;
      }
    }
    //hist("Track_Roi_position","HLT/IDTrkDump")->Fill(counter);
  }
    
  return outputVector;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYS TOOL

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrackPartVectors( const DataHandle<Rec::TrackParticleContainer> trackPartCont) {

  std::vector< simpleTrack > dummyA, dummyB;
  return GetTrackPartVectors(trackPartCont,dummyA, dummyB, false);

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrackPartVectors( const DataHandle<Rec::TrackParticleContainer> trackPartCont,
								     std::vector< simpleTrack > & siSelected, std::vector< simpleTrack > & trtSelected, 
								     bool selection /* = true */) {

 

  std::vector< simpleTrack > outputVector;
  Rec::TrackParticleContainer::const_iterator trackParticle = trackPartCont->begin();
  Rec::TrackParticleContainer::const_iterator lastTrkPart   = trackPartCont->end();
  double vtx_x0 = 0;
  double vtx_y0 = 0;
  double vtx_z0 = 0;
  double Trk_Vx_z0 = 0; 
  double Trk_Vx_d0 = 0; // JWH more realistic d0 value

  //+++ Retrieve offline primary vertex candidate from storegate
  if (selection){
    StatusCode sc_offvtx = StatusCode::SUCCESS;
    
    std::string VxPrimaryCandidate = "VxPrimaryCandidate";
    const VxContainer* VertexCollection;
    sc_offvtx = m_storeGate->retrieve(VertexCollection, VxPrimaryCandidate);
      
    if (sc_offvtx.isFailure()) {
      ATH_MSG_VERBOSE(" Failed to retrieve vertexes! ");
    }
    else {
      vtx_x0 = (VertexCollection->at(0))->recVertex().position().x();
      vtx_y0 = (VertexCollection->at(0))->recVertex().position().y();
      vtx_z0 = (VertexCollection->at(0))->recVertex().position().z();
    }
  }
  
  for (; trackParticle != lastTrkPart; ++trackParticle) {
    
    //+++ Retrieve track parameters from input track object
    const Trk::TrackSummary* summary = (*trackParticle)->trackSummary();
    const Trk::Perigee* measPer = (*trackParticle)->measuredPerigee();
    if (summary == 0 || measPer==0) {
	    ATH_MSG_DEBUG("No measured perigee or track summary parameters assigned to the track");
      continue;
    }
    
    //if (summary == 0 || measPer==0){
    //    HepVector perigeeParams = measPer->parameters();
    float pt  = measPer->pT(); 
    float eta = measPer->eta();
    float phi = (*trackParticle)->measuredPerigee()->parameters()[Trk::phi0];
    float z0  = (*trackParticle)->measuredPerigee()->parameters()[Trk::z0];
    float d0  = (*trackParticle)->measuredPerigee()->parameters()[Trk::d0];
    float nPixHits = summary->get(Trk::numberOfPixelHits);
    float nSctHits = summary->get(Trk::numberOfSCTHits);
    float nTrtHits = summary->get(Trk::numberOfTRTHits);
    float etaR = -999;
    float phiR = -999;

    //+++ Return tracks in generic format
    simpleTrack temp;
    temp.SetPtEtaPhiM(pt,eta,phi,0.);
    temp.z0 = z0;
    temp.d0 = d0;
    temp.nPixHits = nPixHits;
    temp.nSctHits = nSctHits;
    temp.nTrtHits = nTrtHits;
    temp.etaR = etaR;
    temp.phiR = phiR;
    outputVector.push_back(temp);

    //+++ Determine the x0 and y0 of track JWH
    double x0 = 0;
    double y0 = 0;
    
    if ( phi >=0){ 
      if ( phi > (TMath::PiOver2() )){ // Upper left quadrant
	y0 = TMath::Sin(TMath::Pi()-phi)*d0;
	x0 = -TMath::Cos(TMath::Pi()-phi)*d0;
      }
      else { // upper right quadrant
	y0 = TMath::Sin(phi)*d0;
	x0 = TMath::Cos(phi)*d0;
      }
    }

    if (phi < 0){
      if ((phi < -TMath::PiOver2() )){ // lower left quadrant
      	y0 = -TMath::Sin(TMath::Pi()-(+phi))*d0;
	x0 = -TMath::Cos(TMath::Pi()-(+phi))*d0;
       }
      else { // lower right quadrant
	y0 = -TMath::Sin(+phi)*d0; //vtx_yo
	x0 = TMath::Cos(+phi)*d0;
      }
    }
    double theta = TMath::ATan2(y0,x0);

    Trk_Vx_d0 = sqrt( pow((x0-vtx_x0),2) + pow((y0-vtx_y0), 2) );
    Trk_Vx_z0 = fabs( (z0 - vtx_z0)*sin(theta));

    //+++ Si and TRT track selections
    if (selection) {
      if (pt < m_minPt) continue;
      if (fabs(eta) > m_maxAbsEta) continue;

      // TRT track selection
      if( nTrtHits >= m_minTrtHits) trtSelected.push_back(temp);

      // Si track selection
      if (fabs(Trk_Vx_d0) > m_maxAbsD0) continue;
      if (Trk_Vx_z0 > 1.5) continue;
      //else if (fabs(z0) > m_maxAbsZ0) continue;  // do wrt Vtx
      if (m_useSiHits) {
	if( 2 * nPixHits + nSctHits >= m_minSiHits ) siSelected.push_back(temp);
      } else {
	if (nPixHits < m_minPixHits) continue;
	if (nSctHits >= m_minSctHits ) siSelected.push_back(temp);
      }
    }
  }

  return outputVector;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrackPartVectors( std::vector< simpleTrack > trackPartCont,
								     std::vector< simpleTrack > & siSelected, std::vector< simpleTrack > & trtSelected, 
								     bool selection /* = true */) {

 

  std::vector< simpleTrack > outputVector(trackPartCont);
  double vtx_x0 = 0;
  double vtx_y0 = 0;
  double Trk_Vx_d0 = 0; // JWH more realistic d0 value

  //+++ Retrieve offline primary vertex candidate from storegate
  if (selection){
    StatusCode sc_offvtx = StatusCode::SUCCESS;
    
    //const DataHandle<Trk::VxCandidate> VertexCollection;
    std::string VxPrimaryCandidate = "VxPrimaryCandidate";
    const VxContainer* VertexCollection;
    sc_offvtx = m_storeGate->retrieve(VertexCollection, VxPrimaryCandidate);
      
    if (sc_offvtx.isFailure()) {
      ATH_MSG_VERBOSE(" Failed to retrieve vertexes! ");
    }
    else {
      vtx_x0 = (VertexCollection->at(0))->recVertex().position().x();
      vtx_y0 = (VertexCollection->at(0))->recVertex().position().y();
    }
  }
  
  for (unsigned int i=0; i<trackPartCont.size();i++) {
    
    //+++ Retrieve track parameters from input track object
    simpleTrack temp(trackPartCont.at(i));
    
    //if (summary == 0 || measPer==0){
    float pt  = temp.Pt(); 
    float eta = temp.Eta();
    float phi = temp.Phi();
    float z0  = temp.z0;
    float d0  = temp.d0;
    float nPixHits = temp.nPixHits;
    float nSctHits = temp.nSctHits;
    float nTrtHits = temp.nTrtHits;
    //float etaR = -999;
    //float phiR = -999;
    
    
    //+++ Determine the x0 and y0 of track JWH
    double x0 = 0;
    double y0 = 0;
    
    if ( phi >=0){ 
      if ( phi > (TMath::PiOver2() )){ // Upper left quadrant
	y0 = TMath::Sin(TMath::Pi()-phi)*d0;
	x0 = -TMath::Cos(TMath::Pi()-phi)*d0;
      }
      else { // upper right quadrant
	y0 = TMath::Sin(phi)*d0;
	x0 = TMath::Cos(phi)*d0;
      }
    }

    if (phi < 0){
      if ((phi < -TMath::PiOver2() )){ // lower left quadrant
      	y0 = -TMath::Sin(TMath::Pi()-(+phi))*d0;
	x0 = -TMath::Cos(TMath::Pi()-(+phi))*d0;
       }
      else { // lower right quadrant
	y0 = -TMath::Sin(+phi)*d0;
	x0 = TMath::Cos(+phi)*d0;
      }
    }

    Trk_Vx_d0 = sqrt( pow((x0-vtx_x0),2) + pow((y0-vtx_y0), 2) );

    //+++ Si and TRT track selections
    if (selection) {
      if (pt < m_minPt) continue;
      if (fabs(eta) > m_maxAbsEta) continue;
      // TRT track selection
      if( nTrtHits >= m_minTrtHits) trtSelected.push_back(temp);
      // Si track selection

      //      if (fabs(d0) > m_maxAbsD0) continue;
      if (fabs(Trk_Vx_d0) > m_maxAbsD0) continue;
      if (fabs(z0) > m_maxAbsZ0) continue;
      if (m_useSiHits) {
	if( 2 * nPixHits + nSctHits >= m_minSiHits ) siSelected.push_back(temp);
      } else {
	if (nPixHits < m_minPixHits) continue;
	if (nSctHits >= m_minSctHits ) siSelected.push_back(temp);
      }
    }
  }
  //std::cout << "trtSelected.size() = " << trtSelected.size() << std::endl;

  return outputVector;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrigInDetTrackVectors( const DataHandle<TrigInDetTrackCollection> trigInDetTrkCont, 
								       bool selection) {


  std::vector< simpleTrack > outputVector;
  TrigInDetTrackCollection::const_iterator track     = trigInDetTrkCont->begin();
  TrigInDetTrackCollection::const_iterator lastTrack = trigInDetTrkCont->end();

  for (; track != lastTrack; ++track) {
    
    float pt  = (*track)->param()->pT();
    float eta = (*track)->param()->eta();
    float phi = (*track)->param()->phi0();
    float z0  = (*track)->param()->z0();
    float d0  = (*track)->param()->a0();
    float nPixHits = 0; // Not used
    float nSctHits = 0; // Not used
    float nTrtHits = 0; // Not used
    float etaR = -999;
    float phiR = -999;

    //+++ Apply selection (normally NOT, only for offline tracks).
    if (selection) {
      if (m_useSiHits) {
	int tmpSiHits = 2 * ( (*track)->NPixelSpacePoints() + (*track)->NSCT_SpacePoints());  // Correct ??
	if (tmpSiHits < m_minSiHits) continue;
      } else {
	if ((*track)->NPixelSpacePoints() < m_minPixHits) continue;
	if (2 * (*track)->NSCT_SpacePoints() < m_minSctHits) continue;
      }
      if (pt < m_minPt) continue;
      if (fabs(eta) > m_maxAbsEta) continue;
    }

    //+++ Return tracks in generic format
    simpleTrack temp;
    temp.SetPtEtaPhiM(pt,eta,phi,0.);
    temp.z0 = z0;
    temp.d0 = d0;
    temp.nPixHits = nPixHits;
    temp.nSctHits = nSctHits;
    temp.nTrtHits = nTrtHits;
    temp.etaR = etaR;
    temp.phiR = phiR;
    outputVector.push_back(temp);
        
  }

  return outputVector;
}

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrigInDetTrackVectors( const DataHandle<TrigInDetTrackCollection> trigInDetTrkCont, std::vector< simpleTrack > trackCollection_A, std::vector< simpleTrack> trackCollection_B, std::vector< simpleTrack > trackCollection_C,
								       bool selection) {
  std::vector< simpleTrack > outputVector;
  TrigInDetTrackCollection::const_iterator track     = trigInDetTrkCont->begin();
  TrigInDetTrackCollection::const_iterator lastTrack = trigInDetTrkCont->end();
  
  for (; track != lastTrack; ++track) {
    
    float pt  = (*track)->param()->pT();
    float eta = (*track)->param()->eta();
    float phi = (*track)->param()->phi0();
    float z0  = (*track)->param()->z0();
    float d0  = (*track)->param()->a0();
    float nPixHits = 0; // Not used
    float nSctHits = 0; // Not used
    float nTrtHits = 0; // Not used
    float etaR = -999;
    float phiR = -999;

    //+++ Apply selection (normally NOT, only for offline tracks).
    if (selection) {
      if (m_useSiHits) {
	int tmpSiHits = 2 * ( (*track)->NPixelSpacePoints() + (*track)->NSCT_SpacePoints());  // Correct ??
	if (tmpSiHits < m_minSiHits) continue;
      } else {
	if ((*track)->NPixelSpacePoints() < m_minPixHits) continue;
	if (2 * (*track)->NSCT_SpacePoints() < m_minSctHits) continue;
      }
      if (pt < m_minPt) continue;
      if (fabs(eta) > m_maxAbsEta) continue;
    }

    //+++ Return tracks in generic format
    simpleTrack temp;
    temp.SetPtEtaPhiM(pt,eta,phi,0.);
    temp.z0 = z0;
    temp.d0 = d0;
    temp.nPixHits = nPixHits;
    temp.nSctHits = nSctHits;
    temp.nTrtHits = nTrtHits;
    temp.etaR = etaR;
    temp.phiR = phiR;
    outputVector.push_back(temp);
    if ((*track)->algorithmId()==5) trackCollection_A.push_back(temp);
    if ((*track)->algorithmId()==6) trackCollection_B.push_back(temp);
    if ((*track)->algorithmId()==7) trackCollection_C.push_back(temp);       
  }

  return outputVector;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrigInDetTrackVectors( const std::vector< Trig::Feature<TrigInDetTrackCollection> >&  feature, 
								       bool selection, bool verbose) {
  std::vector< simpleTrack > outputVector;
  if (verbose){
        std::cout << "(JWH) - - - - - - - - - - - - - - - GetTrigInDetTrackVectors - - - - - - - - - - - - - - -" << std::endl;
        std::cout << "(JWH) Selection cuts = " << selection << std::endl;
  }

  for (unsigned int count = 0 ; count < feature.size() ; count++) {

    const TrigInDetTrackCollection* trigInDetTrkCont = feature[count].cptr();

    TrigInDetTrackCollection::const_iterator track     = trigInDetTrkCont->begin();
    TrigInDetTrackCollection::const_iterator lastTrack = trigInDetTrkCont->end();
    int counter = 0;
    for (; track != lastTrack; ++track) {
      ++counter;
      float pt  = (*track)->param()->pT();
      float eta = (*track)->param()->eta();
      float phi = (*track)->param()->phi0();
      float z0  = (*track)->param()->z0();
      float d0  = (*track)->param()->a0();
      float nPixHits = 0; // Not used
      float nSctHits = 0; // Not used
      float nTrtHits = 0; // Not used
      float etaR = -999;
      float phiR = -999;

    
      //+++ Apply selection (normally NOT, only for offline tracks).
      if (selection) {
	if (m_useSiHits) {
	  int tmpSiHits = 2 * ( (*track)->NPixelSpacePoints() + (*track)->NSCT_SpacePoints());  // Correct ??
	  if (tmpSiHits < m_minSiHits) continue;
	} else {
	  if ((*track)->NPixelSpacePoints() < m_minPixHits) continue;
	  if (2 * (*track)->NSCT_SpacePoints() < m_minSctHits) continue;
	}
	if (pt < m_minPt) continue;
	if (fabs(eta) > m_maxAbsEta) continue;
      }

      //+++ Return tracks in generic format
      simpleTrack temp;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      temp.etaR = etaR;
      temp.phiR = phiR;
      outputVector.push_back(temp);
    }
  }

  return outputVector;
}

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrigInDetTrackVectors( const std::vector< Trig::Feature<TrigInDetTrackCollection> >&  feature, 
								       bool selection, bool verbose, int alg, std::string collection) {

  // Overloaded function to deal with the L2Star multiple methods issue.

  std::vector< simpleTrack > outputVector; // L2Star Collection A
  std::vector< simpleTrack > outputVector_A; // L2Star Collection A
  std::vector< simpleTrack > outputVector_B; // L2Star Collection A
  std::vector< simpleTrack > outputVector_C; // L2Star Collection A

  if (alg < 0 || alg > 2){
    ATH_MSG_WARNING("Non sensible value passed for alg parameter, alg = " << alg);
    return outputVector;
  }

  if (verbose){
    std::cout << "(JWH) - - - - - - - - - - - - - - - GetTrigInDetTrackVectors - - - - - - - - - - - - - - -" << std::endl;
    std::cout << "(JWH) Selection cuts = " << selection << std::endl;
  }

  Trig::ExpertMethods* em = m_TDT->ExperimentalAndExpertMethods();
  em->enable();
  const HLT::NavigationCore* nc = em->getNavigation();

  for (unsigned int count = 0 ; count < feature.size() ; count++) {

    //const TrigInDetTrackCollection* trigInDetTrkCont = feature[count].cptr();

    const HLT::TriggerElement* te = feature[count].te();

    std::vector< const TrigInDetTrackCollection*> collectionVector;
    //    if ( !const_cast<HLT::NavigationCore *>(nc)->getFeatures( te, collectionVector, "TrigL2SiTrackFinder") ){
    if( !const_cast<HLT::NavigationCore *>(nc)->getFeatures( te, collectionVector, collection) ){
      ATH_MSG_WARNING("Did something that TIDA would have returned empty for");
    }

    //std::cout << " IN GetTrigInDetTrackVector, collectionVector size = " << collectionVector.size() << std::endl;

    double size = collectionVector.size();
    // Apparently the following line spits out warnings for every event, so disable for now...
    //if (size > 3) ATH_MSG_WARNING("More than 3 collections found in L2Star");
    unsigned int vec = 0;

    for (; vec < size; ++vec) {
      TrigInDetTrackCollection::const_iterator track     = collectionVector[vec]->begin();
      TrigInDetTrackCollection::const_iterator lastTrack = collectionVector[vec]->end();     
      
      for (; track != lastTrack; ++track) {
	float pt  = (*track)->param()->pT();
	float eta = (*track)->param()->eta();
	float phi = (*track)->param()->phi0();
	float z0  = (*track)->param()->z0();
	float d0  = (*track)->param()->a0();
	float nPixHits = 0; // Not used
	float nSctHits = 0; // Not used
	float nTrtHits = 0; // Not used
	float etaR = -999;
	float phiR = -999;
	
	
	//+++ Apply selection (normally NOT, only for offline tracks).
	if (selection) {
	  if (m_useSiHits) {
	    int tmpSiHits = 2 * ( (*track)->NPixelSpacePoints() + (*track)->NSCT_SpacePoints());  // Correct ??
	    if (tmpSiHits < m_minSiHits) continue;
	  } else {
	    if ((*track)->NPixelSpacePoints() < m_minPixHits) continue;
	    if (2 * (*track)->NSCT_SpacePoints() < m_minSctHits) continue;
	  }
	  if (pt < m_minPt) continue;
	  if (fabs(eta) > m_maxAbsEta) continue;
	}
	
       //+++ Return tracks in generic format
	simpleTrack temp;
	temp.SetPtEtaPhiM(pt,eta,phi,0.);
	temp.z0 = z0;
	temp.d0 = d0;
	temp.nPixHits = nPixHits;
	temp.nSctHits = nSctHits;
	temp.nTrtHits = nTrtHits;
	temp.etaR = etaR;
	temp.phiR = phiR;
	
	if ((*track)->algorithmId() == 5){
	  outputVector_A.push_back(temp);
	}
	if ((*track)->algorithmId() == 6){
	  outputVector_B.push_back(temp);
	}
	if ((*track)->algorithmId() == 7) {
	  outputVector_C.push_back(temp);
	}
	
      }
    }
  }
  if (alg==0) return outputVector_A;
  else if (alg==1) return outputVector_B;
  else if (alg==2) return outputVector_C;
  // Should never get past this point
  else return outputVector;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

std::vector< simpleTrack > HLTIDtrkDumpTool::GetTrigInDetTrackVectors( const std::vector< Trig::Feature<Rec::TrackParticleContainer> >&  feature, 
								       bool selection, bool verbose) {
  
  std::vector< simpleTrack > outputVector;

  if (verbose){
        std::cout << "(JWH) - - - - - - - - - - - - - - - GetTrigInDetTrackVectors - - - - - - - - - - - - - - -" << std::endl;
        std::cout << "(JWH) Selection cuts = " << selection << std::endl;
  }

  for (unsigned int count = 0 ; count < feature.size() ; count++) {

    const Rec::TrackParticleContainer* trigInDetTrkCont = feature[count].cptr();

    Rec::TrackParticleContainer::const_iterator track     = trigInDetTrkCont->begin();
    Rec::TrackParticleContainer::const_iterator lastTrack = trigInDetTrkCont->end();
    int counter = 0;
    for (; track != lastTrack; ++track) {
      ++counter;
      const Trk::TrackSummary* summary = (*track)->trackSummary();
      const Trk::Perigee* measPer = (*track)->measuredPerigee();
      if (summary == 0 || measPer==0) {
	      ATH_MSG_DEBUG("No measured perigee or track summary parameters assigned to the track");
	      continue;
      }    

      //      HepVector perigeeParams = measPer->parameters();    
      float pt  = measPer->pT(); 
      float eta = measPer->eta();
      float phi= (*track)->measuredPerigee()->parameters()[Trk::phi0];
      float z0  = (*track)->measuredPerigee()->parameters()[Trk::z0];
      float d0  = (*track)->measuredPerigee()->parameters()[Trk::d0];
      float nPixHits = summary->get(Trk::numberOfPixelHits);
      float nSctHits = summary->get(Trk::numberOfSCTHits);
      float nTrtHits = summary->get(Trk::numberOfTRTHits);
      float etaR = -999;
      float phiR = -999;

     

      //+++ Si and TRT track selections
      if (selection) {
	if (m_useSiHits) {
	  if (2 * nPixHits + nSctHits < m_minSiHits) continue; 
	} else {
	  if (nPixHits < m_minPixHits) continue;
	  if (nSctHits < m_minSctHits) continue;
	}
	if (pt < m_minPt) continue;
	if (fabs(eta) > m_maxAbsEta) continue;
      }

      //+++ Return tracks in generic format
      simpleTrack temp;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      temp.etaR = etaR;
      temp.phiR = phiR;
      outputVector.push_back(temp);
    }
  }

  return outputVector;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// TO BE REPLACED BY COMMON HLT TRACK ANALYSIS TOOL

std::map<int, std::vector<int> > HLTIDtrkDumpTool::MatchTracks( const std::vector< simpleTrack > & precisetracks, 
								const std::vector< simpleTrack > & coarsetracks, 
								double deltaEta, double deltaPhi, bool verbose ) {

  std::map< int, std::vector<int> > precise2coarse;
  double deltaPt = 1000; // JWH
  verbose = false;
   if (verbose) std::cout << "(JWH) - - - - - - - - - - Match Tracks - - - - - - - - - - - - -" << std::endl;
  //+++ Loop over the precise tracks
  for (unsigned int ipt=0; ipt<precisetracks.size(); ++ipt) {

    simpleTrack prTrk = precisetracks[ipt];

    double precisePt = prTrk.Pt(); // JWH
    std::vector<int> matchVector(0);

    for (unsigned int ict=0; ict<coarsetracks.size(); ++ict) {

      double delr = prTrk.DeltaR( coarsetracks[ict] );
      double delphi = prTrk.DeltaPhi( coarsetracks[ict] );
      double delPt = fabs(precisePt - coarsetracks[ict].Pt()); // JWH

      bool passPt = delPt <= deltaPt; // JWH
      passPt = true; // Force to true until we understand this better.

      //+++ If deltaEta is a negetive value just require the tracks to be on the same side of the detector 
      //+++ e.g. for TRT matching
      bool passEta = false;
      if (deltaEta > 0.0) passEta = fabs(prTrk.Eta() -  coarsetracks[ict].Eta()) < deltaEta;
      else if (deltaEta > -10.0) passEta = prTrk.Eta()*coarsetracks[ict].Eta() > 0.0;
      else passEta = true;

      if ( delphi < deltaPhi && passEta && passPt) { // JWH

	bool inserted(false);

	//+++ try to insert the coarse track in order of minimum deltaR
	for( std::vector<int>::iterator trkItr = matchVector.begin();trkItr != matchVector.end(); ++trkItr )
	  if ( delr < prTrk.DeltaR( coarsetracks[*trkItr] ) ) {
	    matchVector.insert( trkItr, ict );
	    inserted = true;
	    break; }

	if ( !inserted )
	  matchVector.push_back(ict);

      }

    } //+++ end of loop over coarse tracks

    //+++ if no tracks are matched, move on to next precise track. No map entry for that precise track.
    if ( matchVector.empty() ) continue;

    precise2coarse.insert( std::pair<int, std::vector<int> >(ipt, matchVector) );

  }

  bool updated = true;
  while ( updated ) {

    updated = false;

    for(std::map<int, std::vector<int> >::iterator mapItr1 = precise2coarse.begin();
	mapItr1 != precise2coarse.end(); ++mapItr1) {

      //+++ When no match entries are found in the map, remove them
      std::vector<int> matchVect1 = mapItr1->second;
      if ( matchVect1.empty() ) {
	updated = true;
	precise2coarse.erase( mapItr1 );
	break; }

      //+++ Loop over the map to see if there is any other precise track
      //+++   that was matched to the same coarse track
      for(std::map<int, std::vector<int> >::iterator mapItr2 = precise2coarse.begin();
	  mapItr2 != precise2coarse.end(); ++mapItr2) {

	if ( mapItr1->first <= mapItr2->first ) {
	  continue;
	}

	std::vector<int> matchVect2 = mapItr2->second;

	//+++ Fix SA 15/01/10
	if ( matchVect2.empty() ) continue;
	if ( matchVect1.front() != matchVect2.front() ) continue;

	double delr1 =
	  precisetracks[mapItr1->first].DeltaR(coarsetracks[matchVect1[0]]);
	double delr2 =
	  precisetracks[mapItr2->first].DeltaR(coarsetracks[matchVect2[0]]);

	if ( delr2 > delr1 ){
	  //+++ Fix JL 9/10/08
	  if(! mapItr2->second.empty())  (mapItr2->second).erase((mapItr2->second).begin());
	  else ATH_MSG_DEBUG("Warning, vector is empty");
	}
	else{
	  //+++ Fix JL 9/10/08
	  if(! mapItr1->second.empty())  (mapItr1->second).erase((mapItr1->second).begin());
	  else ATH_MSG_DEBUG("Warning, vector is empty");
	}

	updated = true;
	break;

      } //+++ end of internal loop over the match map
    } //+++ end of loop over the match map
  } //+++ end of while loop

  return(precise2coarse);

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

std::string HLTIDtrkDumpTool::makeHistName( const std::string & sigTracks,
					    const std::string & sigSelect,
					    const std::string & par,
					    const std::string & trkType, 
					    HLTIDDump::SliceVariables & SLICE) const {  // JWH
 
  if( SLICE.staco)
    return sigTracks + "_" + sigSelect + "_" + par + "_" + trkType; 
  else 
    return sigTracks + "_" + sigSelect + "_" + par + "_" + trkType; 
  
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void  HLTIDtrkDumpTool::appendVector(const HLTIDDump::HistogramKeeper & hist,  std::vector <std::string> & allNames) {

  // appends all the names stored in vectors of strings into one vector

  allNames.insert(allNames.end(), hist.par_IDS.begin(), hist.par_IDS.end()); 
  allNames.insert(allNames.end(), hist.par_SIT.begin(), hist.par_SIT.end());
  allNames.insert(allNames.end(), hist.par_L2S.begin(), hist.par_L2S.end());
  allNames.insert(allNames.end(), hist.par_TSF.begin(), hist.par_TSF.end()); 
  allNames.insert(allNames.end(), hist.par_EF.begin(),  hist.par_EF.end()); 
  allNames.insert(allNames.end(), hist.par_EFT.begin(), hist.par_EFT.end()); 

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool HLTIDtrkDumpTool::checkSize(HLTIDDump::SliceVariables & SLICE) {

  unsigned int tmp_sigSize = SLICE.sigTracks.size();

  if (SLICE.sigSelect.size()         != tmp_sigSize) return (false);
  if (SLICE.doIDSCAN.size()          != tmp_sigSize) return (false);
  if (SLICE.doSiTrack.size()         != tmp_sigSize) return (false);
  if (SLICE.doTRTSegF.size()         != tmp_sigSize) return (false);
  if (SLICE.doEF.size()              != tmp_sigSize) return (false);
  if (SLICE.doEFtrt.size()           != tmp_sigSize) return (false);
  if (SLICE.IDSCANCollection.size()  != tmp_sigSize) return (false);
  if (SLICE.SiTrackCollection.size() != tmp_sigSize) return (false);
  if (SLICE.TRTSegFCollection.size() != tmp_sigSize) return (false);
  if (SLICE.EFCollection.size()      != tmp_sigSize) return (false);
  if (SLICE.EFtrtCollection.size()   != tmp_sigSize) return (false);

  return true;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::vector< simpleTrack > HLTIDtrkDumpTool::GetTauTracks() {

  std::vector< simpleTrack > TAUtracks;
  const Analysis::TauJetContainer* TauCollection;
  StatusCode sc_newtau = StatusCode::SUCCESS;
  sc_newtau =  m_storeGate->retrieve(TauCollection,"TauRecContainer");
  
  if(!(sc_newtau.isFailure())){
  //  else{
    //hist("Tau_Size","HLT/IDTrkDump")->Fill(TauCollection->size());
    int number_of_taus = TauCollection->size();
    
    for(int i=0;i<number_of_taus; i++){
      //if ( (TauCollection->at(i)->nProng() ==1 && TauCollection->at(i)->tauID()->tauCutSafeMedium() ) || ( TauCollection->at(i)->nProng() ==3  && TauCollection->at(i)->tauID()->tauCutSafeTight() )){
      if ( (TauCollection->at(i)->nProng() ==1 && TauCollection->at(i)->tauID()->isTau(TauJetParameters::TauCutMedium) ) || ( TauCollection->at(i)->nProng() ==3  && TauCollection->at(i)->tauID()->isTau(TauJetParameters::TauCutTight) )){

	//hist("Tau_Et","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->et());
	//hist("Tau_Eta","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->eta());
	//hist("Tau_Phi","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->phi());

	int tau_calo_tracks = TauCollection->at(i)->seedCalo_numTrack();

	//hist("Tau_NumCaloTracks","HLT/IDTrkDump") ->Fill(tau_calo_tracks);
	
	if (tau_calo_tracks <=3 ){
	  for(int j=0; j<tau_calo_tracks; j++){

	    //hist("TauCaloTrack_Pt","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->seedCalo_track(j)->pt());
	    //hist("TauCaloTrack_Eta","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->seedCalo_track(j)->eta());
	    //hist("TauCaloTrack_Phi","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->seedCalo_track(j)->phi());
	    //hist("Tau_Calo_Pt_Et_diff","HLT/IDTrkDump") ->Fill(TauCollection->at(i)->et()-TauCollection->at(i)->seedCalo_track(j)->pt());
	    
	    const Trk::TrackSummary* summary = TauCollection->at(i)->seedCalo_track(j)->trackSummary();
	    //	    HepVector perigeeParams = TauCollection->at(i)->seedCalo_track(j)->measuredPerigee()->parameters();
	    float pt  = TauCollection->at(i)->seedCalo_track(j)->measuredPerigee()->pT(); 
	    float eta = TauCollection->at(i)->seedCalo_track(j)->measuredPerigee()->eta();
	    float phi = TauCollection->at(i)->seedCalo_track(j)->measuredPerigee()->parameters()[Trk::phi0];
	    float z0  = TauCollection->at(i)->seedCalo_track(j)->measuredPerigee()->parameters()[Trk::z0];
	    float d0  = TauCollection->at(i)->seedCalo_track(j)->measuredPerigee()->parameters()[Trk::d0];
	    float nPixHits = summary->get(Trk::numberOfPixelHits);
	    float nSctHits = summary->get(Trk::numberOfSCTHits);
	    float nTrtHits = summary->get(Trk::numberOfTRTHits);
	    float etaR = -999;
	    float phiR = -999;
	    
	    //+++ Return tracks in generic format
	    simpleTrack temp;
	    temp.SetPtEtaPhiM(pt,eta,phi,0.);
	    temp.z0 = z0;
	    temp.d0 = d0;
	    temp.nPixHits = nPixHits;
	    temp.nSctHits = nSctHits;
	    temp.nTrtHits = nTrtHits;
	    temp.etaR = etaR;
	    temp.phiR = phiR;
	    TAUtracks.push_back(temp);
	  }
	}
      }
    }
  }
  return TAUtracks;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::vector< simpleTrack > HLTIDtrkDumpTool::GetElectronTracks() {

  std::vector< simpleTrack > EGAMMAtracks;
  ElectronContainer* ElectronCollection;
  std::string m_ElectronCollection = "ElectronAODCollection";
  StatusCode sc=m_storeGate->retrieve( ElectronCollection, m_ElectronCollection);
  if (sc.isFailure()){
    ATH_MSG_WARNING("No Electrons of type " << m_ElectronCollection << " found in storeGate");
    ElectronCollection = 0;
  }
  else  ATH_MSG_DEBUG("Number of electrons = " << ElectronCollection->size());


  /// iterators over the container 
  //std::vector< Analysis::Electron* > m_TightElectrons; // Tight electrons are constructed using both cluster and ID track information.
  std::vector< const Analysis::Electron* > TightElectrons; // Tight electrons are constructed using both cluster and ID track information.
  //std::vector< Electron* > m_LooseElectrons; // Loose electrons are constructed using only loose claorimeter cluster information ... hopefully.

  ATH_MSG_DEBUG("Tight electrons size before loop = " << TightElectrons.size());
  if (ElectronCollection && ElectronCollection->size() != 0){
    for (ElectronContainer::const_iterator elecItr  = ElectronCollection->begin(); elecItr != ElectronCollection->end(); ++elecItr) { 
      //Analysis::Electron* temp = *elecItr;
      const Analysis::Electron* temp = *elecItr;
      //TightElectrons.push_back(temp);

      //if( (*elecItr)->isem(egammaPID::ElectronMedium)==0) m_TightElectrons.push_back(temp);
      if( (*elecItr)->isem(egammaPID::ElectronMedium)==0) TightElectrons.push_back(temp);
      //if( (*elecItr)->isem(egammaPID::ElectronLoose)==0) m_LooseElectrons.push_back(temp);
    } 
  }
  ATH_MSG_DEBUG("Tight electrons size after loop = " << TightElectrons.size());


  //for( std::vector< Analysis::Electron* >::const_iterator it = m_TightElectrons.begin(); it != m_TightElectrons.end(); ++it){
  for( std::vector< const Analysis::Electron* >::const_iterator it = TightElectrons.begin(); it != TightElectrons.end(); ++it){
    if ((*it)->trackParticle() && !(*it)->conversion()) {// blah.selectTrack( (*itr)->trackParticle());
      simpleTrack temp;
      const Trk::TrackSummary* summary = (*it)->trackParticle()->trackSummary();
      //   HepVector perigeeParams = (*it)->trackParticle()->measuredPerigee()->parameters();
      float pt  = (*it)->trackParticle()->measuredPerigee()->pT(); 
      float eta = (*it)->trackParticle()->measuredPerigee()->eta();
      float phi = (*it)->trackParticle()->measuredPerigee()->parameters()[Trk::phi0];
      float z0  = (*it)->trackParticle()->measuredPerigee()->parameters()[Trk::z0];
      float d0  = (*it)->trackParticle()->measuredPerigee()->parameters()[Trk::d0];
      float nPixHits = summary->get(Trk::numberOfPixelHits);
      float nSctHits = summary->get(Trk::numberOfSCTHits);
      float nTrtHits = summary->get(Trk::numberOfTRTHits);
      float etaR = -999;
      float phiR = -999;
      temp.SetPtEtaPhiM(pt,eta,phi,0.);
      temp.z0 = z0;
      temp.d0 = d0;
      temp.nPixHits = nPixHits;
      temp.nSctHits = nSctHits;
      temp.nTrtHits = nTrtHits;
      temp.etaR = etaR;
      temp.phiR = phiR;
      
      EGAMMAtracks.push_back(temp);
    }
  }

  return EGAMMAtracks;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
