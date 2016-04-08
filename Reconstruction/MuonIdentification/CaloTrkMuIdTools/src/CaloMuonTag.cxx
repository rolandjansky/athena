/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/CaloMuonTag.h"

#include "StoreGate/StoreGateSvc.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor 
///////////////////////////////////////////////////////////////////////////////
CaloMuonTag::CaloMuonTag( const std::string& type, const std::string& name, const IInterface*  pInterface):
  AthAlgTool(type, name, pInterface),
  m_numTagged(0),
  m_numRejected(0)
{
  declareInterface<ICaloMuonTag>(this);
  
  declareProperty("ptMin",                m_ptMin            = 15000                  );
  declareProperty("ptMax",                m_ptMax            = 35000                  );
  
  declareProperty("emb1Veto",             m_emb1Veto         = Range_t( 200,   600  ) );
  declareProperty("emb2Veto",             m_emb2Veto         = Range_t( 500,   1000 ) );
  declareProperty("emb3Veto",             m_emb3Veto         = Range_t( 300,   400  ) );
  declareProperty("eme1Veto",             m_eme1Veto         = Range_t( 900,   900  ) );
  declareProperty("eme2Veto",             m_eme2Veto         = Range_t( 1900,  1900 ) );
  declareProperty("eme3Veto",             m_eme3Veto         = Range_t( 900,   900  ) );
  declareProperty("tileBar0Sig",          m_tileBar0Sig      = Range_t( 80,   -200  ) );
  declareProperty("tileBar1Sig",          m_tileBar1Sig      = Range_t( 500,  -1000 ) );
  declareProperty("tileBar2Sig",          m_tileBar2Sig      = Range_t( 80,   -200  ) );
  declareProperty("tileExt0Sig",          m_tileExt0Sig      = Range_t( 0,    -300  ) );
  declareProperty("tileExt1Sig",          m_tileExt1Sig      = Range_t( 500,  -400  ) );
  declareProperty("tileExt2Sig",          m_tileExt2Sig      = Range_t( 250,  -1000 ) );
  declareProperty("tileBar0Veto",         m_tileBar0Veto     = Range_t( 2500,  7000 ) );
  declareProperty("tileBar1Veto",         m_tileBar1Veto     = Range_t( 2500,  7000 ) );
  declareProperty("tileBar2Veto",         m_tileBar2Veto     = Range_t( 2500,  7000 ) );
  declareProperty("tileExt0Veto",         m_tileExt0Veto     = Range_t( 2000,  6000 ) );
  declareProperty("tileExt1Veto",         m_tileExt1Veto     = Range_t( 2500,  8000 ) );
  declareProperty("tileExt2Veto",         m_tileExt2Veto     = Range_t( 2500,  8000 ) );
  declareProperty("tileBar0DiffLow",      m_tileBar0DiffLow  = Range_t(-300,  -1000 ) );
  declareProperty("tileBar1DiffLow",      m_tileBar1DiffLow  = Range_t(-550,  -1200 ) );
  declareProperty("tileBar2DiffLow",      m_tileBar2DiffLow  = Range_t(-300,  -1000 ) );
  declareProperty("tileExt0DiffLow",      m_tileExt0DiffLow  = Range_t(-500,  -1000 ) );
  declareProperty("tileExt1DiffLow",      m_tileExt1DiffLow  = Range_t(-500,  -1200 ) );
  declareProperty("tileExt2DiffLow",      m_tileExt2DiffLow  = Range_t(-500,  -1500 ) );
  declareProperty("hec0Veto",             m_hec0Veto         = Range_t(1500,   8000 ) );
  declareProperty("hec1Veto",             m_hec1Veto         = Range_t(2000,   8000 ) );
  declareProperty("hec2Veto",             m_hec2Veto         = Range_t(2000,   8000 ) );
  declareProperty("hec3Veto",             m_hec3Veto         = Range_t(2000,   8000 ) );
  declareProperty("hec0Sig",              m_hec0Sig          = Range_t(0,     -1000 ) );
  declareProperty("hec1Sig",              m_hec1Sig          = Range_t(0,     -1000 ) );
  declareProperty("hec2Sig",              m_hec2Sig          = Range_t(100,   -1000 ) );
  declareProperty("hec3Sig",              m_hec3Sig          = Range_t(250,   -1000 ) );
  
}


///////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////
CaloMuonTag::~CaloMuonTag(){}

///////////////////////////////////////////////////////////////////////////////
// initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonTag::initialize()
{
/*  if (AlgTool::initialize().isFailure()) {
    msg(MSG::ERROR) << "AlgTool couldn't be initialized!" << endreq;
    return StatusCode::FAILURE;
  } */
  
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Initializing CaloMuonTag " << name() << " with configuration (cutName:lowPtCut,highPtCut):"
                      << "\nemb1Veto        : " << m_emb1Veto.first << ", " << m_emb1Veto.second
                      << "\nemb2Veto        : " << m_emb2Veto.first << ", " << m_emb2Veto.second
                      << "\nemb3Veto        : " << m_emb3Veto.first << ", " << m_emb3Veto.second
                      << "\neme1Veto        : " << m_eme1Veto.first << ", " << m_eme1Veto.second
                      << "\neme2Veto        : " << m_eme2Veto.first << ", " << m_eme2Veto.second
                      << "\neme3Veto        : " << m_eme3Veto.first << ", " << m_eme3Veto.second
                      << "\ntileBar0Sig     : " << m_tileBar0Sig.first << ", " << m_tileBar0Sig.second
                      << "\ntileBar1Sig     : " << m_tileBar1Sig.first << ", " << m_tileBar1Sig.second
                      << "\ntileBar2Sig     : " << m_tileBar2Sig.first << ", " << m_tileBar2Sig.second
                      << "\ntileExt0Sig     : " << m_tileExt0Sig.first << ", " << m_tileExt0Sig.second
                      << "\ntileExt1Sig     : " << m_tileExt1Sig.first << ", " << m_tileExt1Sig.second
                      << "\ntileExt2Sig     : " << m_tileExt2Sig.first << ", " << m_tileExt2Sig.second
                      << "\ntileBar0Veto    : " << m_tileBar0Veto.first << ", " << m_tileBar0Veto.second
                      << "\ntileBar1Veto    : " << m_tileBar1Veto.first << ", " << m_tileBar1Veto.second
                      << "\ntileBar2Veto    : " << m_tileBar2Veto.first << ", " << m_tileBar2Veto.second
                      << "\ntileExt0Veto    : " << m_tileExt0Veto.first << ", " << m_tileExt0Veto.second
                      << "\ntileExt1Veto    : " << m_tileExt1Veto.first << ", " << m_tileExt1Veto.second
                      << "\ntileExt2Veto    : " << m_tileExt2Veto.first << ", " << m_tileExt2Veto.second
                      << "\ntileBar0DiffLow : " << m_tileBar0DiffLow.first << ", " << m_tileBar0DiffLow.second
                      << "\ntileBar1DiffLow : " << m_tileBar1DiffLow.first << ", " << m_tileBar1DiffLow.second
                      << "\ntileBar2DiffLow : " << m_tileBar2DiffLow.first << ", " << m_tileBar2DiffLow.second
                      << "\ntileExt0DiffLow : " << m_tileExt0DiffLow.first << ", " << m_tileExt0DiffLow.second
                      << "\ntileExt1DiffLow : " << m_tileExt1DiffLow.first << ", " << m_tileExt1DiffLow.second
                      << "\ntileExt2DiffLow : " << m_tileExt2DiffLow.first << ", " << m_tileExt2DiffLow.second
                      << "\nhec0Veto        : " << m_hec0Veto.first << ", " << m_hec0Veto.second
                      << "\nhec1Veto        : " << m_hec1Veto.first << ", " << m_hec1Veto.second
                      << "\nhec2Veto        : " << m_hec2Veto.first << ", " << m_hec2Veto.second
                      << "\nhec3Veto        : " << m_hec3Veto.first << ", " << m_hec3Veto.second
                      << "\nhec0Sig         : " << m_hec0Sig.first << ", " << m_hec0Sig.second
                      << "\nhec1Sig         : " << m_hec1Sig.first << ", " << m_hec1Sig.second
                      << "\nhec2Sig         : " << m_hec2Sig.first << ", " << m_hec2Sig.second
                      << "\nhec3Sig         : " << m_hec3Sig.first << ", " << m_hec3Sig.second
                      << "\nLow pT threshold  = " << m_ptMin
                      << "\nHigh pT threshold = " << m_ptMax << endreq;
  }

  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// finalize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonTag::finalize()
{
  ATH_MSG_INFO("Number of tracks tagged   : " << m_numTagged);
  ATH_MSG_INFO("Number of tracks rejected : " << m_numRejected);
  
  ATH_MSG_DEBUG("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// caloMuonTag
///////////////////////////////////////////////////////////////////////////////
int CaloMuonTag::caloMuonTag(const std::vector<DepositInCalo>& deposits, double /*eta*/, double pt) const 
{
  std::vector<DepositInCalo>::const_iterator it = deposits.begin();
  int numVetoesTriggered = 0;
  int tag = 4;
  for (;it!=deposits.end(); it++) {
    CaloCell_ID::CaloSample sample = it->subCaloId();
    double dep   = it->energyDeposited();
    double eLoss = it->muonEnergyLoss();
    double diff  = dep - eLoss;
//
// don't use the difference between measured and expected energy per layer 
//
    diff = 0.;

    if (sample==CaloCell_ID::EMB1) {
      if (dep>interpolate(m_emb1Veto, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::EMB2) {
      if (dep>interpolate(m_emb2Veto, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::EMB3) {
      if (dep>interpolate(m_emb3Veto, pt)) {
        numVetoesTriggered++;
      }
    }

    if (sample==CaloCell_ID::EME1) {
      if (dep>interpolate(m_eme1Veto, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::EME2) {
      if (dep>interpolate(m_eme2Veto, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::EME3) {
      if (dep>interpolate(m_eme3Veto, pt)) {
        numVetoesTriggered++;
      }
    }

    if (sample==CaloCell_ID::TileBar0) {
      if (tag>3) tag = 3;
      if (dep<interpolate(m_tileBar0Sig, pt)) {
        numVetoesTriggered++;
      }
      if (dep>interpolate(m_tileBar0Veto, pt)) {
        numVetoesTriggered++;
      }
      if (diff<interpolate(m_tileBar0DiffLow, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::TileBar1) {
      if (tag>2) tag = 2;
      if (dep<interpolate(m_tileBar1Sig, pt)) {
        numVetoesTriggered++;
      }
      if (dep>interpolate(m_tileBar1Veto, pt)) {
        numVetoesTriggered++;
      }
      if (diff<interpolate(m_tileBar1DiffLow, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::TileBar2) {
      if (tag>1) tag = 1;
      if (dep<interpolate(m_tileBar2Sig, pt)) {
        numVetoesTriggered++;
      }
      if (dep>interpolate(m_tileBar2Veto, pt)) {
        numVetoesTriggered++;
      }
      if (diff<interpolate(m_tileBar2DiffLow, pt)) {
        numVetoesTriggered++;
      }
    }


    if (sample==CaloCell_ID::TileExt0) {
      if (tag>3) tag = 3;
      if (dep<interpolate(m_tileExt0Sig, pt)) {
        numVetoesTriggered++;
      }
      if (diff<interpolate(m_tileExt0DiffLow, pt)) {
        numVetoesTriggered++;
      }
      if (dep>interpolate(m_tileExt0Veto, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::TileExt1) {
      if (tag>2) tag = 2;
      if (dep<interpolate(m_tileExt1Sig, pt)) {
        numVetoesTriggered++;
      }
      if (diff<interpolate(m_tileExt1DiffLow, pt)) {
        numVetoesTriggered++;
      }
      if (dep>interpolate(m_tileExt1Veto, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::TileExt2) {
      if (tag>1) tag = 1;
      if (dep<interpolate(m_tileExt2Sig, pt)) {
        numVetoesTriggered++;
      }
      if (diff<interpolate(m_tileExt2DiffLow, pt)) {
        numVetoesTriggered++;
      }
      if (dep>interpolate(m_tileExt2Veto, pt)) {
        numVetoesTriggered++;
      }
    }

    if (sample==CaloCell_ID::HEC0) {
      if (dep>interpolate(m_hec0Veto, pt)) {
        numVetoesTriggered++;
      }
      if (dep<interpolate(m_hec0Sig, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::HEC1) {
      if (tag>3) tag = 3;
      if (dep>interpolate(m_hec1Veto, pt)) {
        numVetoesTriggered++;
      }
      if (dep<interpolate(m_hec1Sig, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::HEC2) {
      if (tag>2) tag = 2;
      if (dep>interpolate(m_hec2Veto, pt)) {
        numVetoesTriggered++;
      }
      if (dep<interpolate(m_hec2Sig, pt)) {
        numVetoesTriggered++;
      }
    }
    if (sample==CaloCell_ID::HEC3) {
      if (tag>1) tag = 1;
      if (dep>interpolate(m_hec3Veto, pt)) {
        numVetoesTriggered++;
      }
      if (dep<interpolate(m_hec3Sig, pt)) {
        numVetoesTriggered++;
      }
    }

    if (numVetoesTriggered>=1) {
      m_numRejected++;
      return 0;
    }

  }
  
  if (tag<3) {
    m_numTagged++;
  }
  else if (tag==4){
    return 0;
  }
  
  return tag;
}

///////////////////////////////////////////////////////////////////////////////
// interpolate
///////////////////////////////////////////////////////////////////////////////
double CaloMuonTag::interpolate(Range_t cuts, double pt) const {
  if (pt<m_ptMin) {
    return cuts.first;
  }
  if (pt>m_ptMax) {
    return cuts.second;
  }
  return cuts.first + (cuts.second-cuts.first)*(pt - m_ptMin)/(m_ptMax-m_ptMin);
}
