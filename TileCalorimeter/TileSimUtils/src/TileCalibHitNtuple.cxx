/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileSimUtils/TileCalibHitNtuple.h"

// Atlas includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "AthenaKernel/errorcheck.h"

// ROOT includes
#include "TTree.h"

static const InterfaceID IID_ITileCalibHitNtuple("TileCalibHitNtuple", 1, 0);

const InterfaceID&
TileCalibHitNtuple::interfaceID() {
  return IID_ITileCalibHitNtuple;
}

/**
 * Constructor
 */
TileCalibHitNtuple::TileCalibHitNtuple(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_thistSvc("THistSvc", name)
    , m_streamName("AANT")
    , m_ntupleID("TileCalibHit")
    , m_treeSize(16000000000LL)
    , m_ntuplePtr(0)
{
  m_det=m_mod=m_tow=m_samp=m_reg=m_calc=m_evt=0;
  m_xPos=m_yPos=m_zPos=m_ener=0.0;

  declareInterface < TileCalibHitNtuple > (this);

  declareProperty("THistSvc",   m_thistSvc);
  declareProperty("StreamName", m_streamName);
  declareProperty("NTupleID",   m_ntupleID);
  declareProperty("TreeSize",   m_treeSize);

}

/**
 * Destructor
 */
TileCalibHitNtuple::~TileCalibHitNtuple() {
}

/**
 * Initializer
 */
StatusCode TileCalibHitNtuple::initialize() {

  ATH_MSG_INFO("in initialize()");

  CHECK(m_thistSvc.retrieve());

  m_ntuplePtr=new TTree(m_ntupleID.c_str(),"TileCalibHit-Ntuple");
  m_ntuplePtr->SetMaxTreeSize(m_treeSize);

  m_ntuplePtr->Branch("detector", &m_det,     "detector/S");
  m_ntuplePtr->Branch("module",   &m_mod,     "module/S");
  m_ntuplePtr->Branch("tower",    &m_tow,     "tower/S");
  m_ntuplePtr->Branch("sample",   &m_samp,    "sample/S");
  m_ntuplePtr->Branch("region",   &m_reg,     "region/S");
  m_ntuplePtr->Branch("IDcalc",   &m_calc,    "IDcalc/S");
  m_ntuplePtr->Branch("event",    &m_evt,     "event/S");
  m_ntuplePtr->Branch("xPos",     &m_xPos,    "xPos/F");
  m_ntuplePtr->Branch("yPos",     &m_yPos,    "yPos/F");
  m_ntuplePtr->Branch("zPos",     &m_zPos,    "zPos/F");
  m_ntuplePtr->Branch("energy",   &m_ener,    "energy/F");

  StatusCode sc=m_thistSvc->regTree("/"+m_streamName+"/"+m_ntupleID,m_ntuplePtr);

  if(sc.isFailure()) {
    ATH_MSG_ERROR("Problem registering TileCalibHit Tree");
  }

  return sc;
}

/**
 * Finalizer
 */
StatusCode TileCalibHitNtuple::finalize() {

  ATH_MSG_INFO("Finalizing");

  return StatusCode::SUCCESS;
}

void TileCalibHitNtuple::storeHit(int det, int mod, int tow, int samp, int reg,
                                  int calc, int evt, float x, float y, float z, float e)
{
  m_det  = det;
  m_mod  = mod;
  m_tow  = tow;
  m_samp = samp;
  m_reg  = reg;
  m_calc = calc;
  m_evt  = evt;
  m_xPos = x;
  m_yPos = y;
  m_zPos = z;
  m_ener = e;
  
  m_ntuplePtr->Fill();
}
