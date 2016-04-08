/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileSimUtils/TileCalibHitCntNtup.h"

// Atlas includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "AthenaKernel/errorcheck.h"

// ROOT includes
#include "TTree.h"

static const InterfaceID IID_ITileCalibHitCntNtup("TileCalibHitCntNtup", 1, 0);

const InterfaceID&
TileCalibHitCntNtup::interfaceID() {
  return IID_ITileCalibHitCntNtup;
}

/**
 * Constructor
 */
TileCalibHitCntNtup::TileCalibHitCntNtup(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_thistSvc("THistSvc", name)
    , m_streamName("AANT")
    , m_ntupleID("TileCalibHitCnt")
    , m_treeSize(16000000000LL)
    , m_ntuplePtr(0),
    m_act_nchan(0),
    m_inact_nchan(0),
    m_dm_nchan(0),
    m_act_totE(0),
    m_act_visE(0),
    m_inact_totE(0),
    m_inact_visE(0),
    m_DM_totE(0),
    m_DM_visE(0),
    m_barr_totE(0),
    m_barr_Em(0),
    m_barr_NonEm(0),
    m_barr_Invisible(0),
    m_barr_Escaped(0),
    m_inact_barr_totE(0),
    m_inact_barr_Em(0),
    m_inact_barr_NonEm(0),
    m_inact_barr_Invisible(0),
    m_inact_barr_Escaped(0),
    m_ext_totE(0),
    m_ext_Em(0),
    m_ext_NonEm(0),
    m_ext_Invisible(0),
    m_ext_Escaped(0),
    m_inact_ext_totE(0),
    m_inact_ext_Em(0),
    m_inact_ext_NonEm(0),
    m_inact_ext_Invisible(0),
    m_inact_ext_Escaped(0),
    m_itc_totE(0),
    m_itc_Em(0),
    m_itc_NonEm(0),
    m_itc_Invisible(0),
    m_itc_Escaped(0),
    m_inact_itc_totE(0),
    m_inact_itc_Em(0),
    m_inact_itc_NonEm(0),
    m_inact_itc_Invisible(0),
    m_inact_itc_Escaped(0),
    m_gscin_totE(0),
    m_gscin_Em(0),
    m_gscin_NonEm(0),
    m_gscin_Invisible(0),
    m_gscin_Escaped(0),
    m_inact_gscin_totE(0),
    m_inact_gscin_Em(0),
    m_inact_gscin_NonEm(0),
    m_inact_gscin_Invisible(0),
    m_inact_gscin_Escaped(0),
    _m_Total(0),
    _m_Em(0),
    _m_NonEm(0),
    _m_Invisible(0),
    _m_Escaped(0),
    _m_subCalo(0),
    _m_section(0),
    _m_side(0),
    _m_module(0),
    _m_tower(0),
    _m_sample(0),
    _m_inact_Total(0),
    _m_inact_Em(0),
    _m_inact_NonEm(0),
    _m_inact_Invisible(0),
    _m_inact_Escaped(0),
    _m_inact_subCalo(0),
    _m_inact_section(0),
    _m_inact_side(0),
    _m_inact_module(0),
    _m_inact_tower(0),
    _m_inact_sample(0),
    _m_dm_ene(0),
    _m_dm_vis(0),
    _m_dm_subDet(0),
    _m_dm_type(0),
    _m_dm_sampling(0),
    _m_dm_region(0),
    _m_dm_eta(0),
    _m_dm_phi(0),
    m_storeGate(0),
    m_detStore(0),
    m_caloCell_ID(0),
    m_caloDM_ID(0),
    m_max_chan(4999)
{
  declareInterface < TileCalibHitCntNtup > (this);

  declareProperty("THistSvc",   m_thistSvc);
  declareProperty("StreamName", m_streamName);
  declareProperty("NTupleID",   m_ntupleID);
  declareProperty("TreeSize",   m_treeSize);

  m_ActiveHitContainer=   "TileCalibHitActiveCell";
  m_InactiveHitContainer= "TileCalibHitInactiveCell";
  m_dmHitContainer=       "TileCalibHitDeadMaterial";

  declareProperty("ActiveHitCnt",   m_ActiveHitContainer);
  declareProperty("InactiveHitCnt", m_InactiveHitContainer);
  declareProperty("DMCalibHitCnt",  m_dmHitContainer);

}

/**
 * Destructor
 */
TileCalibHitCntNtup::~TileCalibHitCntNtup() {
}

/**
 * Initializer
 */
StatusCode TileCalibHitCntNtup::initialize() {

  ATH_MSG_INFO("in initialize()");

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Unable to get pointer to StoreGate Service");
    return sc;
  }

  sc = service("DetectorStore", m_detStore);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Unable to get pointer to DetectorStore Service");
    return sc;
  }

  // retrieve ID helpers from det store
  sc = m_detStore->retrieve(m_caloCell_ID);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CaloCalibrationID helper from DetectorStore");
    return sc;
  }

  sc = m_detStore->retrieve(m_caloDM_ID);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CaloCalibrationID helper from DetectorStore");
    return sc;
  }

  CHECK(m_thistSvc.retrieve());

  m_ntuplePtr=new TTree(m_ntupleID.c_str(),"TileCalibHitCntNtuple");
  m_ntuplePtr->SetMaxTreeSize(m_treeSize);

  // The fields characterizing the Event
  m_ntuplePtr->Branch("Tile_Nhit_Active", &m_act_nchan,   "Tile_Nhit_Active/I");
  m_ntuplePtr->Branch("Tile_Nhit_Inact",  &m_inact_nchan, "Tile_Nhit_Inact/I");
  m_ntuplePtr->Branch("Tile_Nhit_DM",     &m_dm_nchan,    "Tile_Nhit_DM/I");

  m_ntuplePtr->Branch("Tile_Active_Total_Ene", &m_act_totE,   "Tile_Active_Total_Ene/F");
  m_ntuplePtr->Branch("Tile_Active_Vis_Ene",   &m_act_visE,   "Tile_Active_Vis_Ene/F");
  m_ntuplePtr->Branch("Tile_Inact_Total_Ene",  &m_inact_totE, "Tile_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("Tile_Inact_Vis_Ene",    &m_inact_visE, "Tile_Inact_Vis_Ene/F");
  m_ntuplePtr->Branch("Tile_DM_Total_Ene",     &m_DM_totE,    "Tile_DM_Total_Ene/F");
  m_ntuplePtr->Branch("Tile_DM_Vis_Ene",       &m_DM_visE,    "Tile_DM_Vis_Ene/F");

  m_ntuplePtr->Branch("Barr_Active_Total_Ene",     &m_barr_totE,      "Barr_Active_Total_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_Em_Ene",        &m_barr_Em,        "Barr_Active_Em_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_NonEm_Ene",     &m_barr_NonEm,     "Barr_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_Invisible_Ene", &m_barr_Invisible, "Barr_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_Escaped_Ene",   &m_barr_Escaped,   "Barr_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("Barr_Inact_Total_Ene",      &m_inact_barr_totE,      "Barr_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_Em_Ene",         &m_inact_barr_Em,        "Barr_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_NonEm_Ene",      &m_inact_barr_NonEm,     "Barr_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_Invisible_Ene",  &m_inact_barr_Invisible, "Barr_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_Escaped_Ene",    &m_inact_barr_Escaped,   "Barr_Inact_Escaped_Ene/F");

  m_ntuplePtr->Branch("Ext_Active_Total_Ene",     &m_ext_totE,       "Ext_Active_Total_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_Em_Ene",        &m_ext_Em,         "Ext_Active_Em_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_NonEm_Ene",     &m_ext_NonEm,      "Ext_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_Invisible_Ene", &m_ext_Invisible,  "Ext_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_Escaped_Ene",   &m_ext_Escaped,    "Ext_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("Ext_Inact_Total_Ene",      &m_inact_ext_totE,       "Ext_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_Em_Ene",         &m_inact_ext_Em,         "Ext_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_NonEm_Ene",      &m_inact_ext_NonEm,      "Ext_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_Invisible_Ene",  &m_inact_ext_Invisible,  "Ext_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_Escaped_Ene",    &m_inact_ext_Escaped,    "Ext_Inact_Escaped_Ene/F");

  m_ntuplePtr->Branch("ITC_Active_Total_Ene",     &m_itc_totE,       "ITC_Active_Total_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_Em_Ene",        &m_itc_Em,         "ITC_Active_Em_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_NonEm_Ene",     &m_itc_NonEm,      "ITC_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_Invisible_Ene", &m_itc_Invisible,  "ITC_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_Escaped_Ene",   &m_itc_Escaped,    "ITC_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("ITC_Inact_Total_Ene",      &m_inact_itc_totE,       "ITC_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_Em_Ene",         &m_inact_itc_Em,         "ITC_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_NonEm_Ene",      &m_inact_itc_NonEm,      "ITC_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_Invisible_Ene",  &m_inact_itc_Invisible,  "ITC_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_Escaped_Ene",    &m_inact_itc_Escaped,    "ITC_Inact_Escaped_Ene/F");

  m_ntuplePtr->Branch("GapScin_Active_Total_Ene",     &m_gscin_totE,      "GapScin_Active_Total_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_Em_Ene",        &m_gscin_Em,        "GapScin_Active_Em_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_NonEm_Ene",     &m_gscin_NonEm,     "GapScin_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_Invisible_Ene", &m_gscin_Invisible, "GapScin_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_Escaped_Ene",   &m_gscin_Escaped,   "GapScin_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("GapScin_Inact_Total_Ene",      &m_inact_gscin_totE,      "GapScin_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_Em_Ene",         &m_inact_gscin_Em,        "GapScin_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_NonEm_Ene",      &m_inact_gscin_NonEm,     "GapScin_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_Invisible_Ene",  &m_inact_gscin_Invisible, "GapScin_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_Escaped_Ene",    &m_inact_gscin_Escaped,   "GapScin_Inact_Escaped_Ene/F");

  // The fields characterizing the Hit
  m_ntuplePtr->Branch("_Tile_Active_Total_Ene",     &_m_Total);
  m_ntuplePtr->Branch("_Tile_Active_Em_Ene",        &_m_Em);
  m_ntuplePtr->Branch("_Tile_Active_NonEm_Ene",     &_m_NonEm);
  m_ntuplePtr->Branch("_Tile_Active_Invisible_Ene", &_m_Invisible);
  m_ntuplePtr->Branch("_Tile_Active_Escaped_Ene",   &_m_Escaped);
  m_ntuplePtr->Branch("_Tile_Active_subCalo",       &_m_subCalo);
  m_ntuplePtr->Branch("_Tile_Active_section",       &_m_section);
  m_ntuplePtr->Branch("_Tile_Active_side",          &_m_side);
  m_ntuplePtr->Branch("_Tile_Active_module",        &_m_module);
  m_ntuplePtr->Branch("_Tile_Active_tower",         &_m_tower);
  m_ntuplePtr->Branch("_Tile_Active_sample",        &_m_sample);

  m_ntuplePtr->Branch("_Tile_Inact_Total_Ene",     &_m_inact_Total);
  m_ntuplePtr->Branch("_Tile_Inact_Em_Ene",        &_m_inact_Em);
  m_ntuplePtr->Branch("_Tile_Inact_NonEm_Ene",     &_m_inact_NonEm);
  m_ntuplePtr->Branch("_Tile_Inact_Invisible_Ene", &_m_inact_Invisible);
  m_ntuplePtr->Branch("_Tile_Inact_Escaped_Ene",   &_m_inact_Escaped);
  m_ntuplePtr->Branch("_Tile_Inact_subCalo",       &_m_inact_subCalo);
  m_ntuplePtr->Branch("_Tile_Inact_section",       &_m_inact_section);
  m_ntuplePtr->Branch("_Tile_Inact_side",          &_m_inact_side);
  m_ntuplePtr->Branch("_Tile_Inact_module",        &_m_inact_module);
  m_ntuplePtr->Branch("_Tile_Inact_tower",         &_m_inact_tower);
  m_ntuplePtr->Branch("_Tile_Inact_sample",        &_m_inact_sample);

  m_ntuplePtr->Branch("_Tile_DM_Ene",         &_m_dm_ene);
  m_ntuplePtr->Branch("_Tile_DM_Vis",         &_m_dm_vis);
  m_ntuplePtr->Branch("_Tile_DM_subDetector", &_m_dm_subDet);
  m_ntuplePtr->Branch("_Tile_DM_type",        &_m_dm_type);
  m_ntuplePtr->Branch("_Tile_DM_sampling",    &_m_dm_sampling);
  m_ntuplePtr->Branch("_Tile_DM_region",      &_m_dm_region);
  m_ntuplePtr->Branch("_Tile_DM_eta",         &_m_dm_eta);
  m_ntuplePtr->Branch("_Tile_DM_phi",         &_m_dm_phi);

  sc=m_thistSvc->regTree("/"+m_streamName+"/"+m_ntupleID,m_ntuplePtr);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Problem registering TileCalibHitCnt Tree");
  }

  return sc;
}

/**
 * Executer
 */
StatusCode TileCalibHitCntNtup::StoreCNT( const CaloCalibrationHitContainer* ActiveHitCnt, 
                                          const CaloCalibrationHitContainer* InactiveHitCnt, 
                                          const CaloCalibrationHitContainer* dmHitCnt ) 
{
  StatusCode sc;

  m_act_nchan = m_inact_nchan = m_dm_nchan = 0;

  m_act_totE = m_act_visE = m_inact_totE = m_inact_visE = m_DM_totE = m_DM_visE = 0.;

  m_barr_totE      = 0.;   m_ext_totE      = 0.;   m_itc_totE      = 0.;   m_gscin_totE      = 0.;
  m_barr_Em        = 0.;   m_ext_Em        = 0.;   m_itc_Em        = 0.;   m_gscin_Em        = 0.;
  m_barr_NonEm     = 0.;   m_ext_NonEm     = 0.;   m_itc_NonEm     = 0.;   m_gscin_NonEm     = 0.;
  m_barr_Invisible = 0.;   m_ext_Invisible = 0.;   m_itc_Invisible = 0.;   m_gscin_Invisible = 0.;
  m_barr_Escaped   = 0.;   m_ext_Escaped   = 0.;   m_itc_Escaped   = 0.;   m_gscin_Escaped   = 0.;

  m_inact_barr_totE      = 0.;   m_inact_ext_totE      = 0.;   m_inact_itc_totE      = 0.;   m_inact_gscin_totE      = 0.;
  m_inact_barr_Em        = 0.;   m_inact_ext_Em        = 0.;   m_inact_itc_Em        = 0.;   m_inact_gscin_Em        = 0.;
  m_inact_barr_NonEm     = 0.;   m_inact_ext_NonEm     = 0.;   m_inact_itc_NonEm     = 0.;   m_inact_gscin_NonEm     = 0.;
  m_inact_barr_Invisible = 0.;   m_inact_ext_Invisible = 0.;   m_inact_itc_Invisible = 0.;   m_inact_gscin_Invisible = 0.;
  m_inact_barr_Escaped   = 0.;   m_inact_ext_Escaped   = 0.;   m_inact_itc_Escaped   = 0.;   m_inact_gscin_Escaped   = 0.;

  _m_Total->clear();
  _m_Em->clear();
  _m_NonEm->clear();
  _m_Invisible->clear();
  _m_Escaped->clear();

  _m_subCalo->clear();
  _m_section->clear();
  _m_side->clear();
  _m_module->clear();
  _m_tower->clear();
  _m_sample->clear();

  _m_inact_Total->clear();
  _m_inact_Em->clear();
  _m_inact_NonEm->clear();
  _m_inact_Invisible->clear();
  _m_inact_Escaped->clear();

  _m_inact_subCalo->clear();
  _m_inact_section->clear();
  _m_inact_side->clear();
  _m_inact_module->clear();
  _m_inact_tower->clear();
  _m_inact_sample->clear();

  _m_dm_ene->clear();
  _m_dm_vis->clear();
  _m_dm_subDet->clear();
  _m_dm_type->clear();
  _m_dm_sampling->clear();
  _m_dm_region->clear();
  _m_dm_eta->clear();
  _m_dm_phi->clear();

  //------------ Go inside the CalibHit containers ------------
  int  nhit;
  char text_info[100];
  if(ActiveHitCnt) {
    nhit = ActiveHitCnt->Size();
    sprintf(text_info,"Number of CalibHits in Tile ActiveCell container   = %d",nhit);
    ATH_MSG_INFO(text_info);

    if( nhit != 0 )  {
      CaloCalibrationHitContainer::const_iterator it = ActiveHitCnt->begin();
      CaloCalibrationHitContainer::const_iterator end = ActiveHitCnt->end();

      for(; it != end; it++) {
        m_act_totE += (*it)->energyTotal();
        m_act_visE += (*it)->energyEM() + (*it)->energyNonEM() ;

        _m_Total->push_back( (*it)->energyTotal() );
        _m_Em->push_back( (*it)->energyEM() );
        _m_NonEm->push_back( (*it)->energyNonEM() );
        _m_Invisible->push_back( (*it)->energyInvisible() );
        _m_Escaped->push_back( (*it)->energyEscaped() );

        Identifier id=(*it)->cellID();

        _m_subCalo->push_back( m_caloCell_ID->sub_calo(id) );
        _m_section->push_back( m_caloCell_ID->section(id) );
        _m_side->push_back( m_caloCell_ID->side(id) );
        _m_module->push_back( m_caloCell_ID->module(id) );
        _m_tower->push_back( m_caloCell_ID->tower(id) );
        _m_sample->push_back( m_caloCell_ID->sample(id) );

        if(m_caloCell_ID->is_tile_barrel(id)) {
          m_barr_totE       += (*it)->energyTotal();
          m_barr_Em         += (*it)->energyEM();
          m_barr_NonEm      += (*it)->energyNonEM();
          m_barr_Invisible  += (*it)->energyInvisible();
          m_barr_Escaped    += (*it)->energyEscaped();
        } else if(m_caloCell_ID->is_tile_extbarrel(id)) {
          m_ext_totE        += (*it)->energyTotal();
          m_ext_Em          += (*it)->energyEM();
          m_ext_NonEm       += (*it)->energyNonEM();
          m_ext_Invisible   += (*it)->energyInvisible();
          m_ext_Escaped     += (*it)->energyEscaped();
        } else if(m_caloCell_ID->is_tile_gapscin(id)) {
          m_gscin_totE      += (*it)->energyTotal();
          m_gscin_Em        += (*it)->energyEM();
          m_gscin_NonEm     += (*it)->energyNonEM();
          m_gscin_Invisible += (*it)->energyInvisible();
          m_gscin_Escaped   += (*it)->energyEscaped();
        } else if(m_caloCell_ID->is_tile_gap(id)) {
          m_itc_totE      += (*it)->energyTotal();
          m_itc_Em        += (*it)->energyEM();
          m_itc_NonEm     += (*it)->energyNonEM();
          m_itc_Invisible += (*it)->energyInvisible();
          m_itc_Escaped   += (*it)->energyEscaped();
        } else {
          ATH_MSG_WARNING("CalibHit in TileCalibHitActiveCell container hasn't Tile Identifier");
        }
        ++m_act_nchan;
        if (m_act_nchan >= m_max_chan) break;
      } // loop over container's hit
    }
  } else {
    ATH_MSG_WARNING("TileCalibHitActiveCell not exists for this event");
  }

  if(InactiveHitCnt) {
    nhit = InactiveHitCnt->Size();
    sprintf(text_info,"Number of CalibHits in Tile InactiveCell container = %d",nhit);
    ATH_MSG_INFO(text_info);

    if( nhit != 0 )  {
      CaloCalibrationHitContainer::const_iterator it = InactiveHitCnt->begin();
      CaloCalibrationHitContainer::const_iterator end = InactiveHitCnt->end();

      for(; it != end; it++) {
        m_inact_totE += (*it)->energyTotal();
        m_inact_visE += (*it)->energyEM() + (*it)->energyNonEM() ;

        _m_inact_Total->push_back( (*it)->energyTotal() );
        _m_inact_Em->push_back( (*it)->energyEM() );
        _m_inact_NonEm->push_back( (*it)->energyNonEM() );
        _m_inact_Invisible->push_back( (*it)->energyInvisible() );
        _m_inact_Escaped->push_back( (*it)->energyEscaped() );

        Identifier id=(*it)->cellID();

        _m_inact_subCalo->push_back( m_caloCell_ID->sub_calo(id) );
        _m_inact_section->push_back( m_caloCell_ID->section(id) );
        _m_inact_side->push_back( m_caloCell_ID->side(id) );
        _m_inact_module->push_back( m_caloCell_ID->module(id) );
        _m_inact_tower->push_back( m_caloCell_ID->tower(id) );
        _m_inact_sample->push_back( m_caloCell_ID->sample(id) );

        if(m_caloCell_ID->is_tile_barrel(id)) {
          m_inact_barr_totE       += (*it)->energyTotal();
          m_inact_barr_Em         += (*it)->energyEM();
          m_inact_barr_NonEm      += (*it)->energyNonEM();
          m_inact_barr_Invisible  += (*it)->energyInvisible();
          m_inact_barr_Escaped    += (*it)->energyEscaped();
        } else if(m_caloCell_ID->is_tile_extbarrel(id)) {
          m_inact_ext_totE        += (*it)->energyTotal();
          m_inact_ext_Em          += (*it)->energyEM();
          m_inact_ext_NonEm       += (*it)->energyNonEM();
          m_inact_ext_Invisible   += (*it)->energyInvisible();
          m_inact_ext_Escaped     += (*it)->energyEscaped();
        } else if(m_caloCell_ID->is_tile_gapscin(id)) {
          m_inact_gscin_totE      += (*it)->energyTotal();
          m_inact_gscin_Em        += (*it)->energyEM();
          m_inact_gscin_NonEm     += (*it)->energyNonEM();
          m_inact_gscin_Invisible += (*it)->energyInvisible();
          m_inact_gscin_Escaped   += (*it)->energyEscaped();
        } else if(m_caloCell_ID->is_tile_gap(id)) {
          m_inact_itc_totE      += (*it)->energyTotal();
          m_inact_itc_Em        += (*it)->energyEM();
          m_inact_itc_NonEm	  += (*it)->energyNonEM();
          m_inact_itc_Invisible += (*it)->energyInvisible();
          m_inact_itc_Escaped   += (*it)->energyEscaped();
        } else {
          ATH_MSG_WARNING("CalibHit in TileCalibHitInactiveCell container hasn't Tile Identifier");
        }
        ++m_inact_nchan;
        if (m_inact_nchan >= m_max_chan) break;
      } // loop over container's hit
    }
  } else {
    ATH_MSG_WARNING("TileCalibHitInactiveCell not exists for this event");
  }

  if(dmHitCnt) {
    nhit = dmHitCnt->Size();
    sprintf(text_info,"Number of CalibHits in Tile DeadMaterial container = %d",nhit);
    ATH_MSG_INFO(text_info);

    if( nhit != 0  ) {
      CaloCalibrationHitContainer::const_iterator it = dmHitCnt->begin();
      CaloCalibrationHitContainer::const_iterator end = dmHitCnt->end();

      for(; it != end; it++) {
        m_DM_totE += (*it)->energyTotal();
        m_DM_visE += (*it)->energyEM() + (*it)->energyNonEM();

        Identifier id=(*it)->cellID();

        _m_dm_ene->push_back( (*it)->energyTotal() );
        _m_dm_vis->push_back( (*it)->energyEM() + (*it)->energyNonEM() );
        _m_dm_subDet->push_back( m_caloDM_ID->pos_neg_z(id) );
        _m_dm_type->push_back( m_caloDM_ID->dmat(id) );
        _m_dm_sampling->push_back( m_caloDM_ID->sampling(id) );
        _m_dm_region->push_back( m_caloDM_ID->region(id) );
        _m_dm_eta->push_back( m_caloDM_ID->eta(id) );
        _m_dm_phi->push_back( m_caloDM_ID->phi(id) );

        ++m_dm_nchan;
        if (m_dm_nchan >= m_max_chan) break;
      }
    }
  } else {
    ATH_MSG_WARNING("TileCalibHitDeadMaterial not exists for this event");
  }

  sc = m_ntuplePtr->Fill();
  if ( !sc ) {
    ATH_MSG_ERROR("StoreCNT(): Unable to Fill Cnt Ntuple");
  } else {
    ATH_MSG_INFO("StoreCNT() completed successfully");
  }
  return sc;
}

/**
 * Finalizer
 */
StatusCode TileCalibHitCntNtup::finalize() {
  ATH_MSG_INFO("Finalizing");
  return StatusCode::SUCCESS;
}

