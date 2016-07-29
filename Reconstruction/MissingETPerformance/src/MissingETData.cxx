/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/MissingETData.h"
#include "xAODEventInfo/EventInfo.h"

// MB: reference to MET Goodies map for storing derived quantities
MET::Goodies& MissingETData::s_goodies(MET::Goodies::instance());


MissingETData::MissingETData( const std::string& type,
    const std::string& name,
    const IInterface* parent ) : AthAlgTool( type, name, parent ),
				 m_useNoiseTool(false),
				 m_useBadChannelTool(false),
				 m_useBadChannelMasker(false)
{
  declareInterface<MissingETData>( this );

  declareProperty("METL1ROIKey",              m_HLTL1ROIKey             = "LVL1_ROI");
  declareProperty("METL2Key",                 m_HLTMETL2Key             = "HLT_T2MissingET");
  declareProperty("METEF_FEBHeaderKey",       m_HLTMETEF_FEBHeaderKey   = "HLT_TrigEFMissingET_FEB");
  declareProperty("METEF_noiseSuppKey",       m_HLTMETEF_noiseSuppKey   = "HLT_TrigEFMissingET_noiseSupp");
  declareProperty("METEF_defaultKey",         m_HLTMETEF_defaultKey     = "HLT_TrigEFMissingET");

  declareProperty("METTruthKey",              m_METTruthKey             = "MET_Truth");

  declareProperty("METCorrTopoKey",           m_METCorrTopoKey          = "MET_CorrTopo");
  declareProperty("METCalibKey",              m_METCalibKey             = "MET_Calib");
  declareProperty("METCryoConeKey",           m_METCryoConeKey          = "MET_CryoCone");
  declareProperty("METBaseKey",               m_METBaseKey              = "MET_Base");
  declareProperty("METCryoKey",               m_METCryoKey              = "MET_Cryo");
  declareProperty("METDM_AllKey",             m_METDM_AllKey            = "MET_DM_All");
  declareProperty("METDM_Crack1Key",          m_METDM_Crack1Key         = "MET_DM_Crack1");
  declareProperty("METDM_Crack2Key",          m_METDM_Crack2Key         = "MET_DM_Crack2");
  declareProperty("METDM_CryoKey",            m_METDM_CryoKey           = "MET_DM_Cryo");
  declareProperty("METFinalKey",              m_METFinalKey             = "MET_Final");
  declareProperty("METLocHadTopoObjKey",      m_METLocHadTopoObjKey     = "MET_LocHadTopoObj");
  declareProperty("METTopoObjKey",            m_METTopoObjKey           = "MET_TopoObj");
  declareProperty("METMuonBoy_SpectroKey",    m_METMuonBoy_SpectroKey   = "MET_MuonBoy_Spectro");
  declareProperty("METMuonBoy_TrackKey",      m_METMuonBoy_TrackKey     = "MET_MuonBoy_Track");
  declareProperty("METMuonKey",               m_METMuonKey              = "MET_Muon");
  declareProperty("METMuonMuonsKey",          m_METMuonMuonsKey         = "MET_MuonMuons");
  declareProperty("METBase0Key",              m_METBase0Key             = "MET_Base0");
  declareProperty("METTopoKey",               m_METTopoKey              = "MET_Topo");
  declareProperty("METLocHadTopoKey",         m_METLocHadTopoKey        = "MET_LocHadTopo");
  declareProperty("METRefFinalKey",           m_METRefFinalKey          = "MET_RefFinal");
  declareProperty("METRefEleKey",             m_METRefEleKey            = "MET_RefEle");
  declareProperty("METRefGammaKey",           m_METRefGammaKey          = "MET_RefGamma");
  declareProperty("METRefJetKey",             m_METRefJetKey            = "MET_RefJet");
  declareProperty("METSoftJetsKey",           m_METSoftJetsKey          = "MET_SoftJets");
  declareProperty("METRefMuonKey",            m_METRefMuonKey           = "MET_RefMuon");
  declareProperty("METRefMuon_TrackKey",      m_METRefMuon_TrackKey     = "MET_RefMuon_Track");
  declareProperty("METRefTauKey",             m_METRefTauKey            = "MET_RefTau");
  declareProperty("METMuonBoyKey",            m_METMuonBoyKey           = "MET_MuonBoy");
  declareProperty("METMuIDKey",               m_METMuIDKey              = "MET_MuID");
  declareProperty("METCellOutKey",            m_METCellOutKey           = "MET_CellOut");
  declareProperty("METCellOutEFlowKey",       m_METCellOutEFlowKey      = "MET_CellOut_EFlow");

  declareProperty("METCompositionKey",        m_metCompositionKey       = "MET_RefComposition");

  declareProperty("IgnoreMissingContainers",  m_ignoreMissingContainers = false);

  declareProperty("AllCaloKey",               m_allCaloCellsKey         = "AllCalo");
  declareProperty("getCaloCells",             m_getCaloCells            = false);

  // Use NoiseTool?
  declareProperty("UseCaloNoiseTool", m_useNoiseTool);
  // NoiseTool
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  //N sigma cut for noise tool
  declareProperty("NSigmaCut", m_n_sigma=2.0);

  //Use BadChannel Tool?
  declareProperty("UseBadChannelTool", m_useBadChannelTool);
  //BadChannelTool
  declareProperty("BadChannelTool",m_badChannelTool,"Tool Handle for BadChannelTool");

  //Use BadChannelMasker?
  declareProperty("UseBadChannelMasker", m_useBadChannelMasker);
  //BadChannelMasker
  declareProperty("BadChannelMasker",m_badChannelMasker,"Tool Handle for BadChannelMasker");

  m_met_zero = new MissingEtCalo;
  m_met_truth_int = new MissingET;
  m_met_truth_nonInt = new MissingET;
  m_met_truth_muons = new MissingET;
  m_hlt_met_l1roi_zero = new LVL1_ROI;
  m_hlt_met_cont_zero = new TrigMissingETContainer;
  m_hlt_met_cont_zero->push_back(new TrigMissingET(25));

  m_met_composition_zero = new MissingETComposition;
}

MissingETData::~MissingETData() {

  if (m_met_zero) delete m_met_zero;
  if (m_met_truth_int) delete m_met_truth_int;
  if (m_met_truth_nonInt) delete m_met_truth_nonInt;
  if (m_met_truth_muons) delete m_met_truth_muons;
  if (m_hlt_met_l1roi_zero) delete m_hlt_met_l1roi_zero;
  if (m_hlt_met_cont_zero) {
    TrigMissingETContainer::const_iterator hltFirst  = m_hlt_met_cont_zero->begin();
    TrigMissingETContainer::const_iterator hltLast = m_hlt_met_cont_zero->end();
    for ( ; hltFirst != hltLast; hltFirst++ ) {
      const TrigMissingET *hltzero = *hltFirst;
      if(hltzero) delete hltzero;
    }
  }
}

StatusCode MissingETData::initialize() {
  msg() << MSG::DEBUG << "MissingETData Tool initialize() has been called" << endmsg;

  m_met_zero->setEx(0.);
  m_met_zero->setEy(0.);
  m_met_zero->setEtSum(0.);
  truth_int()->setEx(0.);
  truth_int()->setEy(0.);
  truth_int()->setEtSum(0.);
  truth_nonInt()->setEx(0.);
  truth_nonInt()->setEy(0.);
  truth_nonInt()->setEtSum(0.);
  truth_muons()->setEx(0.);
  truth_muons()->setEy(0.);
  truth_muons()->setEtSum(0.);

  m_truthIndices.push_back(MissingEtTruth::Int);             //   0
  m_truthIndices.push_back(MissingEtTruth::NonInt);          //   1
  m_truthIndices.push_back(MissingEtTruth::IntCentral);      //   2
  m_truthIndices.push_back(MissingEtTruth::IntFwd);          //   3
  m_truthIndices.push_back(MissingEtTruth::IntOutCover);     //   4
  m_truthIndices.push_back(MissingEtTruth::Muons);           //   5

  // retrieve the bad channel tool
  if (m_useBadChannelTool) {

    StatusCode sc = m_badChannelTool.retrieve();

    if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to find bad channel tool" << endmsg;
      //      return sc;
    }
  }//if use bad channel tool

  // retrieve the bad channel masker
  if (m_useBadChannelMasker) {

    StatusCode sc = m_badChannelMasker.retrieve();

    if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to find bad channel masker" << endmsg;
      //      return sc;
    }
  }//if use bad channel masker

  // retrieve the noisetool 
  if (m_useNoiseTool) {

    StatusCode sc = m_noiseTool.retrieve();

    if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to find noise tool" << endmsg;
      //      return sc;
    }
  }//if use noise tool

  return StatusCode::SUCCESS;
}

StatusCode MissingETData::finalize() {
  msg() << MSG::DEBUG << "MissingETData Tool finalize() has been called" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode MissingETData::retrieveContainers() {

  msg() << MSG::DEBUG << "in MissingETData::retrieveContainers() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  //get calo cells - this is both liquid argon and tile cells
  if (m_getCaloCells) {
    const CaloCellContainer *allCaloCells;
    sc = evtStore()->retrieve( allCaloCells, m_allCaloCellsKey );

    if ( sc.isFailure() ) {
      msg() << MSG::WARNING << "No CaloCellContainer found: key = " << m_allCaloCellsKey << endmsg;
      return sc;
    }
    setAllCaloCells(allCaloCells);
    setCaloCellsExist(true);
    fillEtaRings();
  }//getcalocells

  // get L1 ROI 
  if (!evtStore()->contains<LVL1_ROI>(m_HLTL1ROIKey) || m_HLTL1ROIKey == "" ) {
    setL1ROI(m_hlt_met_l1roi_zero);
  } else {
    const LVL1_ROI *L1EsumROI;
    sc = evtStore()->retrieve(L1EsumROI, m_HLTL1ROIKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << m_HLTL1ROIKey << endmsg;
      return sc;
    }
    setL1ROI(L1EsumROI);
  }

  // get L2 MET
  if (!evtStore()->contains<TrigMissingETContainer>(m_HLTMETL2Key) || m_HLTMETL2Key == "") {
    setL2MET(m_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *L2MET;
    sc = evtStore()->retrieve(L2MET, m_HLTMETL2Key);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << m_HLTMETL2Key << endmsg;
      return sc;
    }
    setL2MET(L2MET);
  }

  // get EF MET - computed from FEB Header
  if (!evtStore()->contains<TrigMissingETContainer>(m_HLTMETEF_FEBHeaderKey) || m_HLTMETEF_FEBHeaderKey == "") {
    setEFMET_FEBHeader(m_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *EFMET_FEBHeader;
    sc = evtStore()->retrieve(EFMET_FEBHeader, m_HLTMETEF_FEBHeaderKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << m_HLTMETEF_FEBHeaderKey << endmsg;
      return sc;
    }
    setEFMET_FEBHeader(EFMET_FEBHeader);
  }

  // get EF MET - computed from FEB Header
  if (!evtStore()->contains<TrigMissingETContainer>(m_HLTMETEF_noiseSuppKey) || m_HLTMETEF_noiseSuppKey == "") {
    setEFMET_noiseSupp(m_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *EFMET_noiseSupp;
    sc = evtStore()->retrieve(EFMET_noiseSupp, m_HLTMETEF_noiseSuppKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << m_HLTMETEF_noiseSuppKey << endmsg;
      return sc;
    }
    setEFMET_noiseSupp(EFMET_noiseSupp);
  }

  // get EF MET - default
  if (!evtStore()->contains<TrigMissingETContainer>(m_HLTMETEF_defaultKey) || m_HLTMETEF_defaultKey == "" ) {
    setEFMET_default(m_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *EFMET_default;
    sc = evtStore()->retrieve(EFMET_default, m_HLTMETEF_defaultKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << m_HLTMETEF_defaultKey << endmsg;
      return sc;
    }
    setEFMET_default(EFMET_default);
  }

  if (m_metCompositionKey == "") {setMETComposition(m_met_composition_zero); } else {
    const MissingETComposition *metcomp;
    if (evtStore()->contains<MissingETComposition>(m_metCompositionKey)) {
      sc = evtStore()->retrieve( metcomp, m_metCompositionKey);
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {metcomp = m_met_composition_zero; } else {
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_metCompositionKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMETComposition(metcomp);
  }

  if (m_METBaseKey == "") {setBase(m_met_zero); } else {
    const MissingEtCalo *MET_base;
    if (evtStore()->contains<MissingEtCalo>(m_METBaseKey)) {
      sc = evtStore()->retrieve( MET_base, m_METBaseKey );
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {MET_base = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METBaseKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setBase(MET_base);
  }

  if (m_METRefFinalKey == "") {setRefFinal(m_met_zero); } else {
    const MissingET *MET_refFinal;
    if (evtStore()->contains<MissingET>(m_METRefFinalKey)) {
      sc = evtStore()->retrieve( MET_refFinal, m_METRefFinalKey );
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {MET_refFinal = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefFinalKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefFinal(MET_refFinal);
  }

  if (m_METCorrTopoKey == "") {setCorrTopo(m_met_zero); } else {
    const MissingEtCalo *MET_corrTopo;
    if (evtStore()->contains<MissingEtCalo>(m_METCorrTopoKey)) {
      sc = evtStore()->retrieve( MET_corrTopo, m_METCorrTopoKey );
      if ( sc.isFailure() ) { return sc;} 
    }
    else {
      if (ignoreMissingContainers()) {MET_corrTopo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCorrTopoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCorrTopo(MET_corrTopo);
  }

  if (m_METCalibKey == "") {setCalib(m_met_zero); } else {
    const MissingEtCalo *MET_calib;
    if (evtStore()->contains<MissingEtCalo>(m_METCalibKey)) {
      sc = evtStore()->retrieve( MET_calib, m_METCalibKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_calib = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCalibKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCalib(MET_calib);
  }

  if (m_METCryoConeKey == "") {setCryoCone(m_met_zero); } else {
    const MissingET *MET_cryoCone;
    if (evtStore()->contains<MissingET>(m_METCryoConeKey)) {
      sc = evtStore()->retrieve( MET_cryoCone, m_METCryoConeKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cryoCone = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCryoConeKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCryoCone(MET_cryoCone);
  }

  if (m_METCellOutKey == "") {setCellOut(m_met_zero); } else {
    const MissingET *MET_cellOut;
    if (evtStore()->contains<MissingET>(m_METCellOutKey)) {
      sc = evtStore()->retrieve( MET_cellOut, m_METCellOutKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOut = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCellOutKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCellOut(MET_cellOut);
  }

  if (m_METCellOutEFlowKey == "") {setCellOutEFlow(m_met_zero); } else {
    const MissingET *MET_cellOutEFlow;
    if (evtStore()->contains<MissingET>(m_METCellOutEFlowKey)) {
      sc = evtStore()->retrieve( MET_cellOutEFlow, m_METCellOutEFlowKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOutEFlow = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCellOutEFlowKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCellOutEFlow(MET_cellOutEFlow);
  }

  if (m_METRefEleKey == "") {setRefEle(m_met_zero); } else {
    const MissingET *MET_refEle;
    if (evtStore()->contains<MissingET>(m_METRefEleKey)) {
      sc = evtStore()->retrieve( MET_refEle, m_METRefEleKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refEle = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefEleKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefEle(MET_refEle);
  }

  if (m_METRefGammaKey == "") {setRefGamma(m_met_zero); } else {
    const MissingET *MET_refGamma;
    if (evtStore()->contains<MissingET>(m_METRefGammaKey)) {
      sc = evtStore()->retrieve( MET_refGamma, m_METRefGammaKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refGamma = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefGammaKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefGamma(MET_refGamma);
  }

  if (m_METRefJetKey == "") {setRefJet(m_met_zero); } else {
    const MissingET *MET_refJet;
    if (evtStore()->contains<MissingET>(m_METRefJetKey)) {
      sc = evtStore()->retrieve( MET_refJet, m_METRefJetKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refJet = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefJetKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefJet(MET_refJet);
  }

  if (m_METSoftJetsKey == "") {setSoftJets(m_met_zero); } else {
    const MissingET *MET_softJets;
    if (evtStore()->contains<MissingET>(m_METSoftJetsKey)) {
      sc = evtStore()->retrieve( MET_softJets, m_METSoftJetsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_softJets = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METSoftJetsKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setSoftJets(MET_softJets);
  }

  if (m_METMuonBoyKey == "") {setMuonBoy(m_met_zero); } else {
    const MissingET *MET_MuonBoy;
    if (evtStore()->contains<MissingET>(m_METMuonBoyKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy, m_METMuonBoyKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonBoyKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy(MET_MuonBoy);
  }

  if (m_METMuIDKey == "") {setMuID(m_met_zero); } else {
    const MissingET *MET_MuID;
    if (evtStore()->contains<MissingET>(m_METMuIDKey)) {
      sc = evtStore()->retrieve( MET_MuID, m_METMuIDKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuID = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuIDKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuID(MET_MuID);
  }

  if (m_METRefMuonKey == "") {setRefMuon(m_met_zero); } else {
    const MissingET *MET_refMuon;
    if (evtStore()->contains<MissingET>(m_METRefMuonKey)) {
      sc = evtStore()->retrieve( MET_refMuon, m_METRefMuonKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refMuon = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefMuonKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefMuon(MET_refMuon);
  }

  if (m_METRefMuon_TrackKey == "") {setRefMuon_Track(m_met_zero); } else {
    const MissingET *MET_refMuon_Track;
    if (evtStore()->contains<MissingET>(m_METRefMuon_TrackKey)) {
      sc = evtStore()->retrieve( MET_refMuon_Track, m_METRefMuon_TrackKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refMuon_Track = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefMuon_TrackKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefMuon_Track(MET_refMuon_Track);
  }

  if (m_METRefTauKey == "") {setRefTau(m_met_zero); } else {
    const MissingET *MET_refTau;
    if (evtStore()->contains<MissingET>(m_METRefTauKey)) {
      sc = evtStore()->retrieve( MET_refTau, m_METRefTauKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refTau = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefTauKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefTau(MET_refTau);
  }

  if (m_METCryoKey == "") {setCryo(m_met_zero); } else {
    const MissingET *MET_Cryo;
    if (evtStore()->contains<MissingET>(m_METCryoKey)) {
      sc = evtStore()->retrieve( MET_Cryo, m_METCryoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Cryo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCryoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCryo(MET_Cryo);
  }

  if (m_METDM_AllKey == "") {setDM_All(m_met_zero); } else {
    const MissingET *MET_DM_All;
    if (evtStore()->contains<MissingET>(m_METDM_AllKey)) {
      sc = evtStore()->retrieve( MET_DM_All, m_METDM_AllKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_All = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METDM_AllKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setDM_All(MET_DM_All);
  }

  if (m_METDM_Crack1Key == "") {setDM_Crack1(m_met_zero); } else {
    const MissingET *MET_DM_Crack1;
    if (evtStore()->contains<MissingET>(m_METDM_Crack1Key)) {
      sc = evtStore()->retrieve( MET_DM_Crack1, m_METDM_Crack1Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_Crack1 = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METDM_Crack1Key << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setDM_Crack1(MET_DM_Crack1);
  }

  if (m_METDM_Crack2Key == "") {setDM_Crack2(m_met_zero); } else {
    const MissingET *MET_DM_Crack2;
    if (evtStore()->contains<MissingET>(m_METDM_Crack2Key)) {
      sc = evtStore()->retrieve( MET_DM_Crack2, m_METDM_Crack2Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_Crack2 = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METDM_Crack2Key << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setDM_Crack2(MET_DM_Crack2);
  }

  if (m_METDM_CryoKey == "") {setDM_Cryo(m_met_zero); } else {
    const MissingET *MET_DM_Cryo;
    if (evtStore()->contains<MissingET>(m_METDM_CryoKey)) {
      sc = evtStore()->retrieve( MET_DM_Cryo, m_METDM_CryoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_Cryo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METDM_CryoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setDM_Cryo(MET_DM_Cryo);
  }

  if (m_METFinalKey == "") {setFinal(m_met_zero); } else {
    const MissingET *MET_Final;
    if (evtStore()->contains<MissingET>(m_METFinalKey)) {
      sc = evtStore()->retrieve( MET_Final, m_METFinalKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Final = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METFinalKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setFinal(MET_Final);
  }

  if (m_METLocHadTopoObjKey == "") {setLocHadTopoObj(m_met_zero); } else {
    const MissingET *MET_LocHadTopoObj;
    if (evtStore()->contains<MissingET>(m_METLocHadTopoObjKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopoObj, m_METLocHadTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopoObj = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METLocHadTopoObjKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopoObj(MET_LocHadTopoObj);
  }

  if (m_METTopoObjKey == "") {setTopoObj(m_met_zero); } else {
    const MissingET *MET_TopoObj;
    if (evtStore()->contains<MissingET>(m_METTopoObjKey)) {
      sc = evtStore()->retrieve( MET_TopoObj, m_METTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_TopoObj = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METTopoObjKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setTopoObj(MET_TopoObj);
  }

  if (m_METMuonBoy_SpectroKey == "") {setMuonBoy_Spectro(m_met_zero); } else {
    const MissingET *MET_MuonBoy_Spectro;
    if (evtStore()->contains<MissingET>(m_METMuonBoy_SpectroKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Spectro, m_METMuonBoy_SpectroKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Spectro = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonBoy_SpectroKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Spectro(MET_MuonBoy_Spectro);
  }

  if (m_METMuonBoy_TrackKey == "") {setMuonBoy_Track(m_met_zero); } else {
    const MissingET *MET_MuonBoy_Track;
    if (evtStore()->contains<MissingET>(m_METMuonBoy_TrackKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Track, m_METMuonBoy_TrackKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Track = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonBoy_TrackKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Track(MET_MuonBoy_Track);
  }

  if (m_METMuonKey == "") {setMuon(m_met_zero); } else {
    const MissingET *MET_Muon;
    if (evtStore()->contains<MissingET>(m_METMuonKey)) {
      sc = evtStore()->retrieve( MET_Muon, m_METMuonKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Muon = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuon(MET_Muon);
  }

  if (m_METMuonMuonsKey == "") {setMuonMuons(m_met_zero); } else {
    const MissingET *MET_MuonMuons;
    if (evtStore()->contains<MissingET>(m_METMuonMuonsKey)) {
      sc = evtStore()->retrieve( MET_MuonMuons, m_METMuonMuonsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonMuons = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonMuonsKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonMuons(MET_MuonMuons);
  }

  if (m_METBase0Key == "") {setBase0(m_met_zero); } else {
    const MissingEtCalo *MET_Base0;
    if (evtStore()->contains<MissingEtCalo>(m_METBase0Key)) {
      sc = evtStore()->retrieve( MET_Base0, m_METBase0Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Base0 = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METBase0Key << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setBase0(MET_Base0);
  }

  if (m_METTopoKey == "") {setTopo(m_met_zero); } else {
    const MissingEtCalo *MET_Topo;
    if (evtStore()->contains<MissingEtCalo>(m_METTopoKey)) {
      sc = evtStore()->retrieve( MET_Topo, m_METTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Topo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METTopoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setTopo(MET_Topo);
  }

  if (m_METLocHadTopoKey == "") {setLocHadTopo(m_met_zero); } else {
    const MissingEtCalo *MET_LocHadTopo;
    if (evtStore()->contains<MissingEtCalo>(m_METLocHadTopoKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopo, m_METLocHadTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METLocHadTopoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopo(MET_LocHadTopo);
  }

  const DataHandle<xAOD::EventInfo> evt = 0;  
  sc = evtStore()->retrieve<xAOD::EventInfo>( evt );      
  if ( sc.isFailure() ) { return sc;}
  if(evt){
    setBCID (evt->bcid());
    setMU (evt->averageInteractionsPerCrossing());
  }

  if (m_METTruthKey == "") {
    truth_int()->setEx(0.);
    truth_int()->setEy(0.);
    truth_int()->setEtSum(0.);
    truth_nonInt()->setEx(0.);
    truth_nonInt()->setEy(0.);
    truth_nonInt()->setEtSum(0.);
    truth_muons()->setEx(0.);
    truth_muons()->setEy(0.);
    truth_muons()->setEtSum(0.);
  } else {

    const DataHandle<MissingET> firstMET;
    const DataHandle<MissingET> lastMET;

    if ( (evtStore()->retrieve(firstMET,lastMET)).isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve iterators for MissingET objects" << endmsg;
      return sc; 
    }
    for ( ; firstMET != lastMET; firstMET++ ) {
      const MissingET* theObject = firstMET;

      //////////////////////////////////////////////////////
      // Truth objects
      //////////////////////////////////////////////////////
      if ( theObject->getSource() == MissingET::Truth ) {
        const MissingEtTruth* truthObject;
        if ( (evtStore()->retrieve(truthObject, m_METTruthKey)).isFailure() ) {
          msg() << MSG::ERROR << "Could not retrieve Truth term" << endmsg;
        } else {
          for ( unsigned int i=0; i<m_truthIndices.size(); i++ ) {
            switch ( m_truthIndices[i] ) {
              case MissingEtTruth::Int:                                                     
                truth_int()->setEx(truthObject->exTruth(m_truthIndices[i]));
                truth_int()->setEy(truthObject->eyTruth(m_truthIndices[i]));
                truth_int()->setEtSum(truthObject->etSumTruth(m_truthIndices[i]));
                break;                                                                              
              case MissingEtTruth::NonInt:                                                          
                truth_nonInt()->setEx(truthObject->exTruth(m_truthIndices[i]));
                truth_nonInt()->setEy(truthObject->eyTruth(m_truthIndices[i]));
                truth_nonInt()->setEtSum(truthObject->etSumTruth(m_truthIndices[i]));
                break;                                                                              
              case MissingEtTruth::Muons:                                                           
                truth_muons()->setEx(truthObject->exTruth(m_truthIndices[i]));
                truth_muons()->setEy(truthObject->eyTruth(m_truthIndices[i]));
                truth_muons()->setEtSum(truthObject->etSumTruth(m_truthIndices[i]));
                break;    
              default:
                break;                                                                              
            }//switch
          }//for loop over truthIndices
        }//else
      }//if truth
    }//for loop over MissingET
  }//if key is given

  return sc;
}//end retrieveContainers()

StatusCode MissingETData::retrieveLevel1Containers() {

  msg() << MSG::DEBUG << "in MissingETData::retrieveLevel1Containers() " << endmsg;
  
  StatusCode sc = StatusCode::SUCCESS;


 if (m_METCellOutKey == "") {setCellOut(m_met_zero); } else {
    const MissingET *MET_cellOut;
    if (evtStore()->contains<MissingET>(m_METCellOutKey)) {
      sc = evtStore()->retrieve( MET_cellOut, m_METCellOutKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOut = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCellOutKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCellOut(MET_cellOut);
  }

 if (m_METCellOutEFlowKey == "") {setCellOutEFlow(m_met_zero); } else {
    const MissingET *MET_cellOutEFlow;
    if (evtStore()->contains<MissingET>(m_METCellOutEFlowKey)) {
      sc = evtStore()->retrieve( MET_cellOutEFlow, m_METCellOutEFlowKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOutEFlow = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCellOutEFlowKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCellOutEFlow(MET_cellOutEFlow);
  }

  if (m_METRefEleKey == "") {setRefEle(m_met_zero); } else {
    const MissingET *MET_refEle;
    if (evtStore()->contains<MissingET>(m_METRefEleKey)) {
      sc = evtStore()->retrieve( MET_refEle, m_METRefEleKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refEle = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefEleKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefEle(MET_refEle);
  }

  if (m_METRefGammaKey == "") {setRefGamma(m_met_zero); } else {
    const MissingET *MET_refGamma;
    if (evtStore()->contains<MissingET>(m_METRefGammaKey)) {
      sc = evtStore()->retrieve( MET_refGamma, m_METRefGammaKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refGamma = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefGammaKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefGamma(MET_refGamma);
  }

  if (m_METRefJetKey == "") {setRefJet(m_met_zero); } else {
    const MissingET *MET_refJet;
    if (evtStore()->contains<MissingET>(m_METRefJetKey)) {
      sc = evtStore()->retrieve( MET_refJet, m_METRefJetKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refJet = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefJetKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefJet(MET_refJet);
  }

  if (m_METSoftJetsKey == "") {setSoftJets(m_met_zero); } else {
    const MissingET *MET_softJets;
    if (evtStore()->contains<MissingET>(m_METSoftJetsKey)) {
      sc = evtStore()->retrieve( MET_softJets, m_METSoftJetsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_softJets = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METSoftJetsKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setSoftJets(MET_softJets);
  }

 if (m_METCryoKey == "") {setCryo(m_met_zero); } else {
    const MissingET *MET_Cryo;
    if (evtStore()->contains<MissingET>(m_METCryoKey)) {
      sc = evtStore()->retrieve( MET_Cryo, m_METCryoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Cryo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCryoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCryo(MET_Cryo);
  }



  if (m_METRefTauKey == "") {setRefTau(m_met_zero); } else {
    const MissingET *MET_refTau;
    if (evtStore()->contains<MissingET>(m_METRefTauKey)) {
      sc = evtStore()->retrieve( MET_refTau, m_METRefTauKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refTau = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METRefTauKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setRefTau(MET_refTau);
  }

  return sc;
}//end of level 1 container retrieval

StatusCode MissingETData::retrieveLevel2Containers() {

  msg() << MSG::DEBUG << "in MissingETData::retrieveLevel2Containers() " << endmsg;
  
  StatusCode sc = StatusCode::SUCCESS;

  if (m_METFinalKey == "") {setFinal(m_met_zero); } else {
    const MissingET *MET_Final;
    if (evtStore()->contains<MissingET>(m_METFinalKey)) {
      sc = evtStore()->retrieve( MET_Final, m_METFinalKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Final = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METFinalKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setFinal(MET_Final);
  }


 if (m_METMuonBoy_SpectroKey == "") {setMuonBoy_Spectro(m_met_zero); } else {
    const MissingET *MET_MuonBoy_Spectro;
    if (evtStore()->contains<MissingET>(m_METMuonBoy_SpectroKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Spectro, m_METMuonBoy_SpectroKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Spectro = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonBoy_SpectroKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Spectro(MET_MuonBoy_Spectro);
  }

  if (m_METMuonBoy_TrackKey == "") {setMuonBoy_Track(m_met_zero); } else {
    const MissingET *MET_MuonBoy_Track;
    if (evtStore()->contains<MissingET>(m_METMuonBoy_TrackKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Track, m_METMuonBoy_TrackKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Track = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonBoy_TrackKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Track(MET_MuonBoy_Track);
  }

  if (m_METMuonKey == "") {setMuon(m_met_zero); } else {
    const MissingET *MET_Muon;
    if (evtStore()->contains<MissingET>(m_METMuonKey)) {
      sc = evtStore()->retrieve( MET_Muon, m_METMuonKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Muon = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuon(MET_Muon);
  }

  if (m_METMuonMuonsKey == "") {setMuonMuons(m_met_zero); } else {
    const MissingET *MET_MuonMuons;
    if (evtStore()->contains<MissingET>(m_METMuonMuonsKey)) {
      sc = evtStore()->retrieve( MET_MuonMuons, m_METMuonMuonsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonMuons = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METMuonMuonsKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setMuonMuons(MET_MuonMuons);
  }

 if (m_METCryoConeKey == "") {setCryoCone(m_met_zero); } else {
    const MissingET *MET_cryoCone;
    if (evtStore()->contains<MissingET>(m_METCryoConeKey)) {
      sc = evtStore()->retrieve( MET_cryoCone, m_METCryoConeKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cryoCone = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCryoConeKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCryoCone(MET_cryoCone);
  }

  return sc;
}//end of level 2 container retrieval


StatusCode MissingETData::retrieveLevel3Containers() {
  
  msg() << MSG::DEBUG << "in MissingETData::retrieveLevel3Containers() " << endmsg;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if (m_METBase0Key == "") {setBase0(m_met_zero); } else {
    const MissingEtCalo *MET_Base0;
    if (evtStore()->contains<MissingEtCalo>(m_METBase0Key)) {
      sc = evtStore()->retrieve( MET_Base0, m_METBase0Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Base0 = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METBase0Key << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setBase0(MET_Base0);
  }

  if (m_METTopoKey == "") {setTopo(m_met_zero); } else {
    const MissingEtCalo *MET_Topo;
    if (evtStore()->contains<MissingEtCalo>(m_METTopoKey)) {
      sc = evtStore()->retrieve( MET_Topo, m_METTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Topo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METTopoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setTopo(MET_Topo);
  }

  if (m_METLocHadTopoKey == "") {setLocHadTopo(m_met_zero); } else {
    const MissingEtCalo *MET_LocHadTopo;
    if (evtStore()->contains<MissingEtCalo>(m_METLocHadTopoKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopo, m_METLocHadTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METLocHadTopoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopo(MET_LocHadTopo);
  }

 if (m_METCorrTopoKey == "") {setCorrTopo(m_met_zero); } else {
    const MissingEtCalo *MET_corrTopo;
    if (evtStore()->contains<MissingEtCalo>(m_METCorrTopoKey)) {
      sc = evtStore()->retrieve( MET_corrTopo, m_METCorrTopoKey );
      if ( sc.isFailure() ) { return sc;} 
    }
    else {
      if (ignoreMissingContainers()) {MET_corrTopo = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCorrTopoKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCorrTopo(MET_corrTopo);
  }

  if (m_METCalibKey == "") {setCalib(m_met_zero); } else {
    const MissingEtCalo *MET_calib;
    if (evtStore()->contains<MissingEtCalo>(m_METCalibKey)) {
      sc = evtStore()->retrieve( MET_calib, m_METCalibKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_calib = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METCalibKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setCalib(MET_calib);
  }

 if (m_METBaseKey == "") {setBase(m_met_zero); } else {
    const MissingEtCalo *MET_base;
    if (evtStore()->contains<MissingEtCalo>(m_METBaseKey)) {
      sc = evtStore()->retrieve( MET_base, m_METBaseKey );
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {MET_base = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METBaseKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setBase(MET_base);
  }

  if (m_METLocHadTopoObjKey == "") {setLocHadTopoObj(m_met_zero); } else {
    const MissingET *MET_LocHadTopoObj;
    if (evtStore()->contains<MissingET>(m_METLocHadTopoObjKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopoObj, m_METLocHadTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopoObj = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METLocHadTopoObjKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopoObj(MET_LocHadTopoObj);
  }

  if (m_METTopoObjKey == "") {setTopoObj(m_met_zero); } else {
    const MissingET *MET_TopoObj;
    if (evtStore()->contains<MissingET>(m_METTopoObjKey)) {
      sc = evtStore()->retrieve( MET_TopoObj, m_METTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_TopoObj = m_met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_METTopoObjKey << endmsg;
        return StatusCode::FAILURE;
      }
    }
    setTopoObj(MET_TopoObj);
  }

  return sc;
}//end of level 3 container retrieval


void MissingETData::fillEtaRings() {

  const CaloCellContainer *cells = allCaloCells();
  m_met_cells_x=0.;
  m_met_cells_y=0.;
  m_met_cells_x_nsigma=0.;
  m_met_cells_y_nsigma=0.;

  for (unsigned int i = 0; i < 24; ++i) {
    m_met_cells_x_by_layer[i] = 0.;
    m_met_cells_y_by_layer[i] = 0.;
    m_met_cells_x_by_layer_nsigma[i] = 0.;
    m_met_cells_y_by_layer_nsigma[i] = 0.;
  }

  for (unsigned int j= 0;j<24;++j) {
    for (unsigned int i= 0;i<100;++i) {

      m_eta_rings_by_sample[j][i].setEx(0.);
      m_eta_rings_by_sample[j][i].setEy(0.);
      m_eta_rings_by_sample[j][i].setEtSum(0.);

      m_eta_rings_all_samples[i].setEx(0.);
      m_eta_rings_all_samples[i].setEy(0.);
      m_eta_rings_all_samples[i].setEtSum(0.);

      m_eta_rings_by_sample_nsigma[j][i].setEx(0.);
      m_eta_rings_by_sample_nsigma[j][i].setEy(0.);
      m_eta_rings_by_sample_nsigma[j][i].setEtSum(0.);

      m_eta_rings_all_samples_nsigma[i].setEx(0.);
      m_eta_rings_all_samples_nsigma[i].setEy(0.);
      m_eta_rings_all_samples_nsigma[i].setEtSum(0.);
    }
  }

  //loop over all calo cells
  for (const CaloCell* cell : *cells) {

    double eta_cell = cell->eta();
    double et_cell = cell->et();
    double phi_cell = cell->phi();
    double ex = et_cell * cos(phi_cell);
    double ey = et_cell * sin(phi_cell);

    Identifier id = cell->ID();

    const CaloDetDescrElement *dde = cell->caloDDE();
    CaloCell_ID::CaloSample sample = dde->getSampling();

    //    enum CaloSample {
    //       PreSamplerB=0, EMB1, EMB2, EMB3,       // LAr barrel
    //       PreSamplerE, EME1, EME2, EME3,       // LAr EM endcap 
    //       HEC0, HEC1, HEC2, HEC3,            // Hadronic end cap cal.
    //       TileBar0, TileBar1, TileBar2,      // Tile barrel
    //       TileGap1, TileGap2, TileGap3,      // Tile gap (ITC & scint)
    //       TileExt0, TileExt1, TileExt2,      // Tile extended barrel
    //       FCAL0, FCAL1, FCAL2,               // Forward EM endcap
    //       Unknown
    //0->23, 24 layers

    if (sample > 23) { 
      msg() << MSG::ERROR << "EtaRingsTool, cell sample > 23, sample=" << sample << endmsg; 
      continue;
    }

    if ( m_useBadChannelTool ) {
      CaloBadChannel status = m_badChannelTool->caloStatus(id); 
      if (status.packedData() != 0) {
	continue;
      }
    }//bad channel tool

    if ( m_useBadChannelMasker ) {
      if ( sample <= CaloCell_ID::HEC3 || 
	   sample == CaloCell_ID::FCAL0 || sample == CaloCell_ID::FCAL1 || sample == CaloCell_ID::FCAL2 ) {

	CaloGain::CaloGain gain=cell->gain();

	if( m_badChannelMasker->cellShouldBeMasked(id,gain) ) {
	  continue;
	}
      }//..if its a lar cell
    }//use bad channel masker

    m_met_cells_x -= ex;
    m_met_cells_y -= ey;

    m_met_cells_x_by_layer[sample] -= ex;
    m_met_cells_y_by_layer[sample] -= ey;

    float noiseSigma = 0.;
    if ( m_useNoiseTool ) {
      noiseSigma = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);

      if (std::abs(cell->energy()) >= m_n_sigma*noiseSigma) {
	m_met_cells_x_nsigma -= ex;
	m_met_cells_y_nsigma -= ey;

	m_met_cells_x_by_layer_nsigma[sample] -= ex;
	m_met_cells_y_by_layer_nsigma[sample] -= ey;
      }
    }//use noisetool

    if (fabs(eta_cell) < 5.0) {
      int i = (int)((5.0 + eta_cell)*10.0);

      m_eta_rings_by_sample[sample][i].setEx(m_eta_rings_by_sample[sample][i].etx() - ex);
      m_eta_rings_by_sample[sample][i].setEy(m_eta_rings_by_sample[sample][i].ety() - ey);
      m_eta_rings_by_sample[sample][i].setEtSum(m_eta_rings_by_sample[sample][i].sumet() + et_cell);

      m_eta_rings_all_samples[i].setEx(m_eta_rings_all_samples[i].etx() - ex);
      m_eta_rings_all_samples[i].setEy(m_eta_rings_all_samples[i].ety() - ey);
      m_eta_rings_all_samples[i].setEtSum(m_eta_rings_all_samples[i].sumet() - et_cell);

      if ( m_useNoiseTool && (std::abs(cell->energy()) >= m_n_sigma*noiseSigma) ) {
	m_eta_rings_by_sample_nsigma[sample][i].setEx(m_eta_rings_by_sample_nsigma[sample][i].etx() - ex);
	m_eta_rings_by_sample_nsigma[sample][i].setEy(m_eta_rings_by_sample_nsigma[sample][i].ety() - ey);
	m_eta_rings_by_sample_nsigma[sample][i].setEtSum(m_eta_rings_by_sample_nsigma[sample][i].sumet() + et_cell);

	m_eta_rings_all_samples_nsigma[i].setEx(m_eta_rings_all_samples_nsigma[i].etx() - ex);
	m_eta_rings_all_samples_nsigma[i].setEy(m_eta_rings_all_samples_nsigma[i].ety() - ey);
	m_eta_rings_all_samples_nsigma[i].setEtSum(m_eta_rings_all_samples_nsigma[i].sumet() - et_cell);

      }//use noisetool
    }//eta up to 5
  }//end loop over calo cells
}//end fillEtaRings()

float MissingETData::sumet_MissingET_Base_EMfraction() {

    //sumet
    float etsum = base()->sumet();

    //sumet in all em calo
    float etsumEMB  = base()->etSumCalo(MissingEtCalo::EMB); 
    float etsumPEMB = base()->etSumCalo(MissingEtCalo::PEMB); 
    float etsumEMEC = base()->etSumCalo(MissingEtCalo::EME);
    float etsumPEMEC= base()->etSumCalo(MissingEtCalo::PEMEC);

    //EM fraction of SUMET
    float SUMET_BASE_EMfrac = (etsumEMB+etsumPEMB+etsumEMEC+etsumPEMEC)/etsum;

    return SUMET_BASE_EMfrac;
}

const MissingET* MissingETData::getMissingETbyName( const std::string& contName ) {

  const MissingET *met_ret = m_met_zero;

  if (evtStore()->contains<MissingET>(contName)) {
    StatusCode sc = evtStore()->retrieve( met_ret, contName );
    if ( sc.isFailure() ) { return met_ret;}
  }
  else {
    if (ignoreMissingContainers()) {met_ret = m_met_zero; } else { 
      msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << contName << endmsg;
      return met_ret;
    }
  }
  return met_ret;
}
