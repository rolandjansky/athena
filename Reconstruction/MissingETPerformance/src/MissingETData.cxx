/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/MissingETData.h"
#include "xAODEventInfo/EventInfo.h"

// MB: reference to MET Goodies map for storing derived quantities
MET::Goodies& MissingETData::goodies(MET::Goodies::instance());


MissingETData::MissingETData( const std::string& type,
    const std::string& name,
    const IInterface* parent ) : AthAlgTool( type, name, parent ),
				 m_useNoiseTool(false),
				 m_useBadChannelTool(false),
				 m_useBadChannelMasker(false)
{
  declareInterface<MissingETData>( this );

  declareProperty("METL1ROIKey",              _HLTL1ROIKey             = "LVL1_ROI");
  declareProperty("METL2Key",                 _HLTMETL2Key             = "HLT_T2MissingET");
  declareProperty("METEF_FEBHeaderKey",       _HLTMETEF_FEBHeaderKey   = "HLT_TrigEFMissingET_FEB");
  declareProperty("METEF_noiseSuppKey",       _HLTMETEF_noiseSuppKey   = "HLT_TrigEFMissingET_noiseSupp");
  declareProperty("METEF_defaultKey",         _HLTMETEF_defaultKey     = "HLT_TrigEFMissingET");

  declareProperty("METTruthKey",              _METTruthKey             = "MET_Truth");

  declareProperty("METCorrTopoKey",           _METCorrTopoKey          = "MET_CorrTopo");
  declareProperty("METCalibKey",              _METCalibKey             = "MET_Calib");
  declareProperty("METCryoConeKey",           _METCryoConeKey          = "MET_CryoCone");
  declareProperty("METBaseKey",               _METBaseKey              = "MET_Base");
  declareProperty("METCryoKey",               _METCryoKey              = "MET_Cryo");
  declareProperty("METDM_AllKey",             _METDM_AllKey            = "MET_DM_All");
  declareProperty("METDM_Crack1Key",          _METDM_Crack1Key         = "MET_DM_Crack1");
  declareProperty("METDM_Crack2Key",          _METDM_Crack2Key         = "MET_DM_Crack2");
  declareProperty("METDM_CryoKey",            _METDM_CryoKey           = "MET_DM_Cryo");
  declareProperty("METFinalKey",              _METFinalKey             = "MET_Final");
  declareProperty("METLocHadTopoObjKey",      _METLocHadTopoObjKey     = "MET_LocHadTopoObj");
  declareProperty("METTopoObjKey",            _METTopoObjKey           = "MET_TopoObj");
  declareProperty("METMuonBoy_SpectroKey",    _METMuonBoy_SpectroKey   = "MET_MuonBoy_Spectro");
  declareProperty("METMuonBoy_TrackKey",      _METMuonBoy_TrackKey     = "MET_MuonBoy_Track");
  declareProperty("METMuonKey",               _METMuonKey              = "MET_Muon");
  declareProperty("METMuonMuonsKey",          _METMuonMuonsKey         = "MET_MuonMuons");
  declareProperty("METBase0Key",              _METBase0Key             = "MET_Base0");
  declareProperty("METTopoKey",               _METTopoKey              = "MET_Topo");
  declareProperty("METLocHadTopoKey",         _METLocHadTopoKey        = "MET_LocHadTopo");
  declareProperty("METRefFinalKey",           _METRefFinalKey          = "MET_RefFinal");
  declareProperty("METRefEleKey",             _METRefEleKey            = "MET_RefEle");
  declareProperty("METRefGammaKey",           _METRefGammaKey          = "MET_RefGamma");
  declareProperty("METRefJetKey",             _METRefJetKey            = "MET_RefJet");
  declareProperty("METSoftJetsKey",           _METSoftJetsKey          = "MET_SoftJets");
  declareProperty("METRefMuonKey",            _METRefMuonKey           = "MET_RefMuon");
  declareProperty("METRefMuon_TrackKey",      _METRefMuon_TrackKey     = "MET_RefMuon_Track");
  declareProperty("METRefTauKey",             _METRefTauKey            = "MET_RefTau");
  declareProperty("METMuonBoyKey",            _METMuonBoyKey           = "MET_MuonBoy");
  declareProperty("METMuIDKey",               _METMuIDKey              = "MET_MuID");
  declareProperty("METCellOutKey",            _METCellOutKey           = "MET_CellOut");
  declareProperty("METCellOutEFlowKey",       _METCellOutEFlowKey      = "MET_CellOut_EFlow");

  declareProperty("METCompositionKey",        _metCompositionKey       = "MET_RefComposition");

  declareProperty("IgnoreMissingContainers",  _ignoreMissingContainers = false);

  declareProperty("AllCaloKey",               _allCaloCellsKey         = "AllCalo");
  declareProperty("getCaloCells",             _getCaloCells            = false);

  // Use NoiseTool?
  declareProperty("UseCaloNoiseTool", m_useNoiseTool);
  // NoiseTool
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  //N sigma cut for noise tool
  declareProperty("NSigmaCut", n_sigma=2.0);

  //Use BadChannel Tool?
  declareProperty("UseBadChannelTool", m_useBadChannelTool);
  //BadChannelTool
  declareProperty("BadChannelTool",m_badChannelTool,"Tool Handle for BadChannelTool");

  //Use BadChannelMasker?
  declareProperty("UseBadChannelMasker", m_useBadChannelMasker);
  //BadChannelMasker
  declareProperty("BadChannelMasker",m_badChannelMasker,"Tool Handle for BadChannelMasker");

  _met_zero = new MissingEtCalo;
  _met_truth_int = new MissingET;
  _met_truth_nonInt = new MissingET;
  _met_truth_muons = new MissingET;
  _hlt_met_l1roi_zero = new LVL1_ROI;
  _hlt_met_cont_zero = new TrigMissingETContainer;
  _hlt_met_cont_zero->push_back(new TrigMissingET(25));

  _met_composition_zero = new MissingETComposition;
}

MissingETData::~MissingETData() {

  if (_met_zero) delete _met_zero;
  if (_met_truth_int) delete _met_truth_int;
  if (_met_truth_nonInt) delete _met_truth_nonInt;
  if (_met_truth_muons) delete _met_truth_muons;
  if (_hlt_met_l1roi_zero) delete _hlt_met_l1roi_zero;
  if (_hlt_met_cont_zero) {
    TrigMissingETContainer::const_iterator _hltFirst  = _hlt_met_cont_zero->begin();
    TrigMissingETContainer::const_iterator _hltLast = _hlt_met_cont_zero->end();
    for ( ; _hltFirst != _hltLast; _hltFirst++ ) {
      const TrigMissingET *_hltzero = *_hltFirst;
      if(_hltzero) delete _hltzero;
    }
  }
}

StatusCode MissingETData::initialize() {
  msg() << MSG::DEBUG << "MissingETData Tool initialize() has been called" << endreq;

  _met_zero->setEx(0.);
  _met_zero->setEy(0.);
  _met_zero->setEtSum(0.);
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
      msg() << MSG::ERROR << "Unable to find bad channel tool" << endreq;
      //      return sc;
    }
  }//if use bad channel tool

  // retrieve the bad channel masker
  if (m_useBadChannelMasker) {

    StatusCode sc = m_badChannelMasker.retrieve();

    if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to find bad channel masker" << endreq;
      //      return sc;
    }
  }//if use bad channel masker

  // retrieve the noisetool 
  if (m_useNoiseTool) {

    StatusCode sc = m_noiseTool.retrieve();

    if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to find noise tool" << endreq;
      //      return sc;
    }
  }//if use noise tool

  return StatusCode::SUCCESS;
}

StatusCode MissingETData::finalize() {
  msg() << MSG::DEBUG << "MissingETData Tool finalize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode MissingETData::retrieveContainers() {

  msg() << MSG::DEBUG << "in MissingETData::retrieveContainers() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  //get calo cells - this is both liquid argon and tile cells
  if (_getCaloCells) {
    const CaloCellContainer *allCaloCells;
    sc = evtStore()->retrieve( allCaloCells, _allCaloCellsKey );

    if ( sc.isFailure() ) {
      msg() << MSG::WARNING << "No CaloCellContainer found: key = " << _allCaloCellsKey << endreq;
      return sc;
    }
    setAllCaloCells(allCaloCells);
    setCaloCellsExist(true);
    fillEtaRings();
  }//getcalocells

  // get L1 ROI 
  if (!evtStore()->contains<LVL1_ROI>(_HLTL1ROIKey) || _HLTL1ROIKey == "" ) {
    setL1ROI(_hlt_met_l1roi_zero);
  } else {
    const LVL1_ROI *m_L1EsumROI;
    sc = evtStore()->retrieve(m_L1EsumROI, _HLTL1ROIKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << _HLTL1ROIKey << endreq;
      return sc;
    }
    setL1ROI(m_L1EsumROI);
  }

  // get L2 MET
  if (!evtStore()->contains<TrigMissingETContainer>(_HLTMETL2Key) || _HLTMETL2Key == "") {
    setL2MET(_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *m_L2MET;
    sc = evtStore()->retrieve(m_L2MET, _HLTMETL2Key);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << _HLTMETL2Key << endreq;
      return sc;
    }
    setL2MET(m_L2MET);
  }

  // get EF MET - computed from FEB Header
  if (!evtStore()->contains<TrigMissingETContainer>(_HLTMETEF_FEBHeaderKey) || _HLTMETEF_FEBHeaderKey == "") {
    setEFMET_FEBHeader(_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *m_EFMET_FEBHeader;
    sc = evtStore()->retrieve(m_EFMET_FEBHeader, _HLTMETEF_FEBHeaderKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << _HLTMETEF_FEBHeaderKey << endreq;
      return sc;
    }
    setEFMET_FEBHeader(m_EFMET_FEBHeader);
  }

  // get EF MET - computed from FEB Header
  if (!evtStore()->contains<TrigMissingETContainer>(_HLTMETEF_noiseSuppKey) || _HLTMETEF_noiseSuppKey == "") {
    setEFMET_noiseSupp(_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *m_EFMET_noiseSupp;
    sc = evtStore()->retrieve(m_EFMET_noiseSupp, _HLTMETEF_noiseSuppKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << _HLTMETEF_noiseSuppKey << endreq;
      return sc;
    }
    setEFMET_noiseSupp(m_EFMET_noiseSupp);
  }

  // get EF MET - default
  if (!evtStore()->contains<TrigMissingETContainer>(_HLTMETEF_defaultKey) || _HLTMETEF_defaultKey == "" ) {
    setEFMET_default(_hlt_met_cont_zero);
  } else {
    const TrigMissingETContainer *m_EFMET_default;
    sc = evtStore()->retrieve(m_EFMET_default, _HLTMETEF_defaultKey);
    if( sc.isFailure() ) {
      msg() << MSG::WARNING << "No ESD/AOD/DPD TrigMissingETContainer found: key = "
        << _HLTMETEF_defaultKey << endreq;
      return sc;
    }
    setEFMET_default(m_EFMET_default);
  }

  if (_metCompositionKey == "") {setMETComposition(_met_composition_zero); } else {
    const MissingETComposition *metcomp;
    if (evtStore()->contains<MissingETComposition>(_metCompositionKey)) {
      sc = evtStore()->retrieve( metcomp, _metCompositionKey);
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {metcomp = _met_composition_zero; } else {
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _metCompositionKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMETComposition(metcomp);
  }

  if (_METBaseKey == "") {setBase(_met_zero); } else {
    const MissingEtCalo *MET_base;
    if (evtStore()->contains<MissingEtCalo>(_METBaseKey)) {
      sc = evtStore()->retrieve( MET_base, _METBaseKey );
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {MET_base = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METBaseKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setBase(MET_base);
  }

  if (_METRefFinalKey == "") {setRefFinal(_met_zero); } else {
    const MissingET *MET_refFinal;
    if (evtStore()->contains<MissingET>(_METRefFinalKey)) {
      sc = evtStore()->retrieve( MET_refFinal, _METRefFinalKey );
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {MET_refFinal = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefFinalKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefFinal(MET_refFinal);
  }

  if (_METCorrTopoKey == "") {setCorrTopo(_met_zero); } else {
    const MissingEtCalo *MET_corrTopo;
    if (evtStore()->contains<MissingEtCalo>(_METCorrTopoKey)) {
      sc = evtStore()->retrieve( MET_corrTopo, _METCorrTopoKey );
      if ( sc.isFailure() ) { return sc;} 
    }
    else {
      if (ignoreMissingContainers()) {MET_corrTopo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCorrTopoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCorrTopo(MET_corrTopo);
  }

  if (_METCalibKey == "") {setCalib(_met_zero); } else {
    const MissingEtCalo *MET_calib;
    if (evtStore()->contains<MissingEtCalo>(_METCalibKey)) {
      sc = evtStore()->retrieve( MET_calib, _METCalibKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_calib = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCalibKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCalib(MET_calib);
  }

  if (_METCryoConeKey == "") {setCryoCone(_met_zero); } else {
    const MissingET *MET_cryoCone;
    if (evtStore()->contains<MissingET>(_METCryoConeKey)) {
      sc = evtStore()->retrieve( MET_cryoCone, _METCryoConeKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cryoCone = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCryoConeKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCryoCone(MET_cryoCone);
  }

  if (_METCellOutKey == "") {setCellOut(_met_zero); } else {
    const MissingET *MET_cellOut;
    if (evtStore()->contains<MissingET>(_METCellOutKey)) {
      sc = evtStore()->retrieve( MET_cellOut, _METCellOutKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOut = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCellOutKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCellOut(MET_cellOut);
  }

  if (_METCellOutEFlowKey == "") {setCellOutEFlow(_met_zero); } else {
    const MissingET *MET_cellOutEFlow;
    if (evtStore()->contains<MissingET>(_METCellOutEFlowKey)) {
      sc = evtStore()->retrieve( MET_cellOutEFlow, _METCellOutEFlowKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOutEFlow = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCellOutEFlowKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCellOutEFlow(MET_cellOutEFlow);
  }

  if (_METRefEleKey == "") {setRefEle(_met_zero); } else {
    const MissingET *MET_refEle;
    if (evtStore()->contains<MissingET>(_METRefEleKey)) {
      sc = evtStore()->retrieve( MET_refEle, _METRefEleKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refEle = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefEleKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefEle(MET_refEle);
  }

  if (_METRefGammaKey == "") {setRefGamma(_met_zero); } else {
    const MissingET *MET_refGamma;
    if (evtStore()->contains<MissingET>(_METRefGammaKey)) {
      sc = evtStore()->retrieve( MET_refGamma, _METRefGammaKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refGamma = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefGammaKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefGamma(MET_refGamma);
  }

  if (_METRefJetKey == "") {setRefJet(_met_zero); } else {
    const MissingET *MET_refJet;
    if (evtStore()->contains<MissingET>(_METRefJetKey)) {
      sc = evtStore()->retrieve( MET_refJet, _METRefJetKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refJet = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefJetKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefJet(MET_refJet);
  }

  if (_METSoftJetsKey == "") {setSoftJets(_met_zero); } else {
    const MissingET *MET_softJets;
    if (evtStore()->contains<MissingET>(_METSoftJetsKey)) {
      sc = evtStore()->retrieve( MET_softJets, _METSoftJetsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_softJets = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METSoftJetsKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setSoftJets(MET_softJets);
  }

  if (_METMuonBoyKey == "") {setMuonBoy(_met_zero); } else {
    const MissingET *MET_MuonBoy;
    if (evtStore()->contains<MissingET>(_METMuonBoyKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy, _METMuonBoyKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonBoyKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy(MET_MuonBoy);
  }

  if (_METMuIDKey == "") {setMuID(_met_zero); } else {
    const MissingET *MET_MuID;
    if (evtStore()->contains<MissingET>(_METMuIDKey)) {
      sc = evtStore()->retrieve( MET_MuID, _METMuIDKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuID = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuIDKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuID(MET_MuID);
  }

  if (_METRefMuonKey == "") {setRefMuon(_met_zero); } else {
    const MissingET *MET_refMuon;
    if (evtStore()->contains<MissingET>(_METRefMuonKey)) {
      sc = evtStore()->retrieve( MET_refMuon, _METRefMuonKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refMuon = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefMuonKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefMuon(MET_refMuon);
  }

  if (_METRefMuon_TrackKey == "") {setRefMuon_Track(_met_zero); } else {
    const MissingET *MET_refMuon_Track;
    if (evtStore()->contains<MissingET>(_METRefMuon_TrackKey)) {
      sc = evtStore()->retrieve( MET_refMuon_Track, _METRefMuon_TrackKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refMuon_Track = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefMuon_TrackKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefMuon_Track(MET_refMuon_Track);
  }

  if (_METRefTauKey == "") {setRefTau(_met_zero); } else {
    const MissingET *MET_refTau;
    if (evtStore()->contains<MissingET>(_METRefTauKey)) {
      sc = evtStore()->retrieve( MET_refTau, _METRefTauKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refTau = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefTauKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefTau(MET_refTau);
  }

  if (_METCryoKey == "") {setCryo(_met_zero); } else {
    const MissingET *MET_Cryo;
    if (evtStore()->contains<MissingET>(_METCryoKey)) {
      sc = evtStore()->retrieve( MET_Cryo, _METCryoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Cryo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCryoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCryo(MET_Cryo);
  }

  if (_METDM_AllKey == "") {setDM_All(_met_zero); } else {
    const MissingET *MET_DM_All;
    if (evtStore()->contains<MissingET>(_METDM_AllKey)) {
      sc = evtStore()->retrieve( MET_DM_All, _METDM_AllKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_All = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METDM_AllKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setDM_All(MET_DM_All);
  }

  if (_METDM_Crack1Key == "") {setDM_Crack1(_met_zero); } else {
    const MissingET *MET_DM_Crack1;
    if (evtStore()->contains<MissingET>(_METDM_Crack1Key)) {
      sc = evtStore()->retrieve( MET_DM_Crack1, _METDM_Crack1Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_Crack1 = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METDM_Crack1Key << endreq;
        return StatusCode::FAILURE;
      }
    }
    setDM_Crack1(MET_DM_Crack1);
  }

  if (_METDM_Crack2Key == "") {setDM_Crack2(_met_zero); } else {
    const MissingET *MET_DM_Crack2;
    if (evtStore()->contains<MissingET>(_METDM_Crack2Key)) {
      sc = evtStore()->retrieve( MET_DM_Crack2, _METDM_Crack2Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_Crack2 = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METDM_Crack2Key << endreq;
        return StatusCode::FAILURE;
      }
    }
    setDM_Crack2(MET_DM_Crack2);
  }

  if (_METDM_CryoKey == "") {setDM_Cryo(_met_zero); } else {
    const MissingET *MET_DM_Cryo;
    if (evtStore()->contains<MissingET>(_METDM_CryoKey)) {
      sc = evtStore()->retrieve( MET_DM_Cryo, _METDM_CryoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_DM_Cryo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METDM_CryoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setDM_Cryo(MET_DM_Cryo);
  }

  if (_METFinalKey == "") {setFinal(_met_zero); } else {
    const MissingET *MET_Final;
    if (evtStore()->contains<MissingET>(_METFinalKey)) {
      sc = evtStore()->retrieve( MET_Final, _METFinalKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Final = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METFinalKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setFinal(MET_Final);
  }

  if (_METLocHadTopoObjKey == "") {setLocHadTopoObj(_met_zero); } else {
    const MissingET *MET_LocHadTopoObj;
    if (evtStore()->contains<MissingET>(_METLocHadTopoObjKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopoObj, _METLocHadTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopoObj = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METLocHadTopoObjKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopoObj(MET_LocHadTopoObj);
  }

  if (_METTopoObjKey == "") {setTopoObj(_met_zero); } else {
    const MissingET *MET_TopoObj;
    if (evtStore()->contains<MissingET>(_METTopoObjKey)) {
      sc = evtStore()->retrieve( MET_TopoObj, _METTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_TopoObj = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METTopoObjKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setTopoObj(MET_TopoObj);
  }

  if (_METMuonBoy_SpectroKey == "") {setMuonBoy_Spectro(_met_zero); } else {
    const MissingET *MET_MuonBoy_Spectro;
    if (evtStore()->contains<MissingET>(_METMuonBoy_SpectroKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Spectro, _METMuonBoy_SpectroKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Spectro = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonBoy_SpectroKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Spectro(MET_MuonBoy_Spectro);
  }

  if (_METMuonBoy_TrackKey == "") {setMuonBoy_Track(_met_zero); } else {
    const MissingET *MET_MuonBoy_Track;
    if (evtStore()->contains<MissingET>(_METMuonBoy_TrackKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Track, _METMuonBoy_TrackKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Track = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonBoy_TrackKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Track(MET_MuonBoy_Track);
  }

  if (_METMuonKey == "") {setMuon(_met_zero); } else {
    const MissingET *MET_Muon;
    if (evtStore()->contains<MissingET>(_METMuonKey)) {
      sc = evtStore()->retrieve( MET_Muon, _METMuonKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Muon = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuon(MET_Muon);
  }

  if (_METMuonMuonsKey == "") {setMuonMuons(_met_zero); } else {
    const MissingET *MET_MuonMuons;
    if (evtStore()->contains<MissingET>(_METMuonMuonsKey)) {
      sc = evtStore()->retrieve( MET_MuonMuons, _METMuonMuonsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonMuons = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonMuonsKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonMuons(MET_MuonMuons);
  }

  if (_METBase0Key == "") {setBase0(_met_zero); } else {
    const MissingEtCalo *MET_Base0;
    if (evtStore()->contains<MissingEtCalo>(_METBase0Key)) {
      sc = evtStore()->retrieve( MET_Base0, _METBase0Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Base0 = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METBase0Key << endreq;
        return StatusCode::FAILURE;
      }
    }
    setBase0(MET_Base0);
  }

  if (_METTopoKey == "") {setTopo(_met_zero); } else {
    const MissingEtCalo *MET_Topo;
    if (evtStore()->contains<MissingEtCalo>(_METTopoKey)) {
      sc = evtStore()->retrieve( MET_Topo, _METTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Topo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METTopoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setTopo(MET_Topo);
  }

  if (_METLocHadTopoKey == "") {setLocHadTopo(_met_zero); } else {
    const MissingEtCalo *MET_LocHadTopo;
    if (evtStore()->contains<MissingEtCalo>(_METLocHadTopoKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopo, _METLocHadTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METLocHadTopoKey << endreq;
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

  if (_METTruthKey == "") {
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
      msg() << MSG::ERROR << "Could not retrieve iterators for MissingET objects" << endreq;
      return sc; 
    }
    for ( ; firstMET != lastMET; firstMET++ ) {
      const MissingET* theObject = firstMET;

      //////////////////////////////////////////////////////
      // Truth objects
      //////////////////////////////////////////////////////
      if ( theObject->getSource() == MissingET::Truth ) {
        const MissingEtTruth* truthObject;
        if ( (evtStore()->retrieve(truthObject, _METTruthKey)).isFailure() ) {
          msg() << MSG::ERROR << "Could not retrieve Truth term" << endreq;
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

  msg() << MSG::DEBUG << "in MissingETData::retrieveLevel1Containers() " << endreq;
  
  StatusCode sc = StatusCode::SUCCESS;


 if (_METCellOutKey == "") {setCellOut(_met_zero); } else {
    const MissingET *MET_cellOut;
    if (evtStore()->contains<MissingET>(_METCellOutKey)) {
      sc = evtStore()->retrieve( MET_cellOut, _METCellOutKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOut = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCellOutKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCellOut(MET_cellOut);
  }

 if (_METCellOutEFlowKey == "") {setCellOutEFlow(_met_zero); } else {
    const MissingET *MET_cellOutEFlow;
    if (evtStore()->contains<MissingET>(_METCellOutEFlowKey)) {
      sc = evtStore()->retrieve( MET_cellOutEFlow, _METCellOutEFlowKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cellOutEFlow = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCellOutEFlowKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCellOutEFlow(MET_cellOutEFlow);
  }

  if (_METRefEleKey == "") {setRefEle(_met_zero); } else {
    const MissingET *MET_refEle;
    if (evtStore()->contains<MissingET>(_METRefEleKey)) {
      sc = evtStore()->retrieve( MET_refEle, _METRefEleKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refEle = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefEleKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefEle(MET_refEle);
  }

  if (_METRefGammaKey == "") {setRefGamma(_met_zero); } else {
    const MissingET *MET_refGamma;
    if (evtStore()->contains<MissingET>(_METRefGammaKey)) {
      sc = evtStore()->retrieve( MET_refGamma, _METRefGammaKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refGamma = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefGammaKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefGamma(MET_refGamma);
  }

  if (_METRefJetKey == "") {setRefJet(_met_zero); } else {
    const MissingET *MET_refJet;
    if (evtStore()->contains<MissingET>(_METRefJetKey)) {
      sc = evtStore()->retrieve( MET_refJet, _METRefJetKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refJet = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefJetKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefJet(MET_refJet);
  }

  if (_METSoftJetsKey == "") {setSoftJets(_met_zero); } else {
    const MissingET *MET_softJets;
    if (evtStore()->contains<MissingET>(_METSoftJetsKey)) {
      sc = evtStore()->retrieve( MET_softJets, _METSoftJetsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_softJets = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METSoftJetsKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setSoftJets(MET_softJets);
  }

 if (_METCryoKey == "") {setCryo(_met_zero); } else {
    const MissingET *MET_Cryo;
    if (evtStore()->contains<MissingET>(_METCryoKey)) {
      sc = evtStore()->retrieve( MET_Cryo, _METCryoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Cryo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCryoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCryo(MET_Cryo);
  }



  if (_METRefTauKey == "") {setRefTau(_met_zero); } else {
    const MissingET *MET_refTau;
    if (evtStore()->contains<MissingET>(_METRefTauKey)) {
      sc = evtStore()->retrieve( MET_refTau, _METRefTauKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_refTau = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METRefTauKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setRefTau(MET_refTau);
  }

  return sc;
}//end of level 1 container retrieval

StatusCode MissingETData::retrieveLevel2Containers() {

  msg() << MSG::DEBUG << "in MissingETData::retrieveLevel2Containers() " << endreq;
  
  StatusCode sc = StatusCode::SUCCESS;

  if (_METFinalKey == "") {setFinal(_met_zero); } else {
    const MissingET *MET_Final;
    if (evtStore()->contains<MissingET>(_METFinalKey)) {
      sc = evtStore()->retrieve( MET_Final, _METFinalKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Final = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METFinalKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setFinal(MET_Final);
  }


 if (_METMuonBoy_SpectroKey == "") {setMuonBoy_Spectro(_met_zero); } else {
    const MissingET *MET_MuonBoy_Spectro;
    if (evtStore()->contains<MissingET>(_METMuonBoy_SpectroKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Spectro, _METMuonBoy_SpectroKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Spectro = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonBoy_SpectroKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Spectro(MET_MuonBoy_Spectro);
  }

  if (_METMuonBoy_TrackKey == "") {setMuonBoy_Track(_met_zero); } else {
    const MissingET *MET_MuonBoy_Track;
    if (evtStore()->contains<MissingET>(_METMuonBoy_TrackKey)) {
      sc = evtStore()->retrieve( MET_MuonBoy_Track, _METMuonBoy_TrackKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonBoy_Track = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonBoy_TrackKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonBoy_Track(MET_MuonBoy_Track);
  }

  if (_METMuonKey == "") {setMuon(_met_zero); } else {
    const MissingET *MET_Muon;
    if (evtStore()->contains<MissingET>(_METMuonKey)) {
      sc = evtStore()->retrieve( MET_Muon, _METMuonKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Muon = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuon(MET_Muon);
  }

  if (_METMuonMuonsKey == "") {setMuonMuons(_met_zero); } else {
    const MissingET *MET_MuonMuons;
    if (evtStore()->contains<MissingET>(_METMuonMuonsKey)) {
      sc = evtStore()->retrieve( MET_MuonMuons, _METMuonMuonsKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_MuonMuons = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METMuonMuonsKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setMuonMuons(MET_MuonMuons);
  }

 if (_METCryoConeKey == "") {setCryoCone(_met_zero); } else {
    const MissingET *MET_cryoCone;
    if (evtStore()->contains<MissingET>(_METCryoConeKey)) {
      sc = evtStore()->retrieve( MET_cryoCone, _METCryoConeKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_cryoCone = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCryoConeKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCryoCone(MET_cryoCone);
  }

  return sc;
}//end of level 2 container retrieval


StatusCode MissingETData::retrieveLevel3Containers() {
  
  msg() << MSG::DEBUG << "in MissingETData::retrieveLevel3Containers() " << endreq;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if (_METBase0Key == "") {setBase0(_met_zero); } else {
    const MissingEtCalo *MET_Base0;
    if (evtStore()->contains<MissingEtCalo>(_METBase0Key)) {
      sc = evtStore()->retrieve( MET_Base0, _METBase0Key );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Base0 = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METBase0Key << endreq;
        return StatusCode::FAILURE;
      }
    }
    setBase0(MET_Base0);
  }

  if (_METTopoKey == "") {setTopo(_met_zero); } else {
    const MissingEtCalo *MET_Topo;
    if (evtStore()->contains<MissingEtCalo>(_METTopoKey)) {
      sc = evtStore()->retrieve( MET_Topo, _METTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_Topo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METTopoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setTopo(MET_Topo);
  }

  if (_METLocHadTopoKey == "") {setLocHadTopo(_met_zero); } else {
    const MissingEtCalo *MET_LocHadTopo;
    if (evtStore()->contains<MissingEtCalo>(_METLocHadTopoKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopo, _METLocHadTopoKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METLocHadTopoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopo(MET_LocHadTopo);
  }

 if (_METCorrTopoKey == "") {setCorrTopo(_met_zero); } else {
    const MissingEtCalo *MET_corrTopo;
    if (evtStore()->contains<MissingEtCalo>(_METCorrTopoKey)) {
      sc = evtStore()->retrieve( MET_corrTopo, _METCorrTopoKey );
      if ( sc.isFailure() ) { return sc;} 
    }
    else {
      if (ignoreMissingContainers()) {MET_corrTopo = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCorrTopoKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCorrTopo(MET_corrTopo);
  }

  if (_METCalibKey == "") {setCalib(_met_zero); } else {
    const MissingEtCalo *MET_calib;
    if (evtStore()->contains<MissingEtCalo>(_METCalibKey)) {
      sc = evtStore()->retrieve( MET_calib, _METCalibKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_calib = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METCalibKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setCalib(MET_calib);
  }

 if (_METBaseKey == "") {setBase(_met_zero); } else {
    const MissingEtCalo *MET_base;
    if (evtStore()->contains<MissingEtCalo>(_METBaseKey)) {
      sc = evtStore()->retrieve( MET_base, _METBaseKey );
      if ( sc.isFailure() ) { return sc; }
    }
    else {
      if (ignoreMissingContainers()) {MET_base = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METBaseKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setBase(MET_base);
  }

  if (_METLocHadTopoObjKey == "") {setLocHadTopoObj(_met_zero); } else {
    const MissingET *MET_LocHadTopoObj;
    if (evtStore()->contains<MissingET>(_METLocHadTopoObjKey)) {
      sc = evtStore()->retrieve( MET_LocHadTopoObj, _METLocHadTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_LocHadTopoObj = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METLocHadTopoObjKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setLocHadTopoObj(MET_LocHadTopoObj);
  }

  if (_METTopoObjKey == "") {setTopoObj(_met_zero); } else {
    const MissingET *MET_TopoObj;
    if (evtStore()->contains<MissingET>(_METTopoObjKey)) {
      sc = evtStore()->retrieve( MET_TopoObj, _METTopoObjKey );
      if ( sc.isFailure() ) { return sc;}
    }
    else {
      if (ignoreMissingContainers()) {MET_TopoObj = _met_zero; } else { 
        msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << _METTopoObjKey << endreq;
        return StatusCode::FAILURE;
      }
    }
    setTopoObj(MET_TopoObj);
  }

  return sc;
}//end of level 3 container retrieval


void MissingETData::fillEtaRings() {

  const CaloCellContainer *cells = allCaloCells();
  met_cells_x=0.;
  met_cells_y=0.;
  met_cells_x_nsigma=0.;
  met_cells_y_nsigma=0.;

  for (unsigned int i = 0; i < 24; ++i) {
    met_cells_x_by_layer[i] = 0.;
    met_cells_y_by_layer[i] = 0.;
    met_cells_x_by_layer_nsigma[i] = 0.;
    met_cells_y_by_layer_nsigma[i] = 0.;
  }

  for (unsigned int j= 0;j<24;++j) {
    for (unsigned int i= 0;i<100;++i) {

      _eta_rings_by_sample[j][i].setEx(0.);
      _eta_rings_by_sample[j][i].setEy(0.);
      _eta_rings_by_sample[j][i].setEtSum(0.);

      _eta_rings_all_samples[i].setEx(0.);
      _eta_rings_all_samples[i].setEy(0.);
      _eta_rings_all_samples[i].setEtSum(0.);

      _eta_rings_by_sample_nsigma[j][i].setEx(0.);
      _eta_rings_by_sample_nsigma[j][i].setEy(0.);
      _eta_rings_by_sample_nsigma[j][i].setEtSum(0.);

      _eta_rings_all_samples_nsigma[i].setEx(0.);
      _eta_rings_all_samples_nsigma[i].setEy(0.);
      _eta_rings_all_samples_nsigma[i].setEtSum(0.);
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
      msg() << MSG::ERROR << "EtaRingsTool, cell sample > 23, sample=" << sample << endreq; 
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

    met_cells_x -= ex;
    met_cells_y -= ey;

    met_cells_x_by_layer[sample] -= ex;
    met_cells_y_by_layer[sample] -= ey;

    float noiseSigma = 0.;
    if ( m_useNoiseTool ) {
      noiseSigma = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);

      if (std::abs(cell->energy()) >= n_sigma*noiseSigma) {
	met_cells_x_nsigma -= ex;
	met_cells_y_nsigma -= ey;

	met_cells_x_by_layer_nsigma[sample] -= ex;
	met_cells_y_by_layer_nsigma[sample] -= ey;
      }
    }//use noisetool

    if (fabs(eta_cell) < 5.0) {
      int i = (int)((5.0 + eta_cell)*10.0);

      _eta_rings_by_sample[sample][i].setEx(_eta_rings_by_sample[sample][i].etx() - ex);
      _eta_rings_by_sample[sample][i].setEy(_eta_rings_by_sample[sample][i].ety() - ey);
      _eta_rings_by_sample[sample][i].setEtSum(_eta_rings_by_sample[sample][i].sumet() + et_cell);

      _eta_rings_all_samples[i].setEx(_eta_rings_all_samples[i].etx() - ex);
      _eta_rings_all_samples[i].setEy(_eta_rings_all_samples[i].ety() - ey);
      _eta_rings_all_samples[i].setEtSum(_eta_rings_all_samples[i].sumet() - et_cell);

      if ( m_useNoiseTool && (std::abs(cell->energy()) >= n_sigma*noiseSigma) ) {
	_eta_rings_by_sample_nsigma[sample][i].setEx(_eta_rings_by_sample_nsigma[sample][i].etx() - ex);
	_eta_rings_by_sample_nsigma[sample][i].setEy(_eta_rings_by_sample_nsigma[sample][i].ety() - ey);
	_eta_rings_by_sample_nsigma[sample][i].setEtSum(_eta_rings_by_sample_nsigma[sample][i].sumet() + et_cell);

	_eta_rings_all_samples_nsigma[i].setEx(_eta_rings_all_samples_nsigma[i].etx() - ex);
	_eta_rings_all_samples_nsigma[i].setEy(_eta_rings_all_samples_nsigma[i].ety() - ey);
	_eta_rings_all_samples_nsigma[i].setEtSum(_eta_rings_all_samples_nsigma[i].sumet() - et_cell);

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

const MissingET* MissingETData::getMissingETbyName( const std::string& m_contName ) {

  const MissingET *m_met_ret = _met_zero;

  if (evtStore()->contains<MissingET>(m_contName)) {
    StatusCode sc = evtStore()->retrieve( m_met_ret, m_contName );
    if ( sc.isFailure() ) { return m_met_ret;}
  }
  else {
    if (ignoreMissingContainers()) {m_met_ret = _met_zero; } else { 
      msg() << MSG::WARNING << "No ESD/AOD/DPD container found: key = " << m_contName << endreq;
      return m_met_ret;
    }
  }
  return m_met_ret;
}
