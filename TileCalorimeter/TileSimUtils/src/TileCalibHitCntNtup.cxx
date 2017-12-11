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
TileCalibHitCntNtup::TileCalibHitCntNtup(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_thistSvc("THistSvc", name),
      m_streamName("AANT"),
      m_ntupleID("TileCalibHitCnt"),
      m_treeSize(16000000000LL),
      m_ntuplePtr(0),
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
      m_Total(0),
      m_Em(0),
      m_NonEm(0),
      m_Invisible(0),
      m_Escaped(0),
      m_subCalo(0),
      m_section(0),
      m_side(0),
      m_module(0),
      m_tower(0),
      m_sample(0),
      m_inact_Total(0),
      m_inact_Em(0),
      m_inact_NonEm(0),
      m_inact_Invisible(0),
      m_inact_Escaped(0),
      m_inact_subCalo(0),
      m_inact_section(0),
      m_inact_side(0),
      m_inact_module(0),
      m_inact_tower(0),
      m_inact_sample(0),
      m_dm_ene(0),
      m_dm_vis(0),
      m_dm_subDet(0),
      m_dm_type(0),
      m_dm_sampling(0),
      m_dm_region(0),
      m_dm_eta(0),
      m_dm_phi(0),
      m_caloCell_ID(0),
      m_caloDM_ID(0),
      m_max_chan(4999)
{
  declareInterface<TileCalibHitCntNtup>(this);

  declareProperty("THistSvc", m_thistSvc);
  declareProperty("StreamName", m_streamName);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("TreeSize", m_treeSize);

  m_ActiveHitContainer = "TileCalibHitActiveCell";
  m_InactiveHitContainer = "TileCalibHitInactiveCell";
  m_dmHitContainer = "TileCalibHitDeadMaterial";

  declareProperty("ActiveHitCnt", m_ActiveHitContainer);
  declareProperty("InactiveHitCnt", m_InactiveHitContainer);
  declareProperty("DMCalibHitCnt", m_dmHitContainer);

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

  // retrieve ID helpers from det store
  CHECK(detStore()->retrieve(m_caloCell_ID));

  CHECK(detStore()->retrieve(m_caloDM_ID));

  CHECK(m_thistSvc.retrieve());

  m_ntuplePtr = new TTree(m_ntupleID.c_str(), "TileCalibHitCntNtuple");
  m_ntuplePtr->SetMaxTreeSize(m_treeSize);

  // The fields characterizing the Event
  m_ntuplePtr->Branch("Tile_Nhit_Active", &m_act_nchan, "Tile_Nhit_Active/I");
  m_ntuplePtr->Branch("Tile_Nhit_Inact", &m_inact_nchan, "Tile_Nhit_Inact/I");
  m_ntuplePtr->Branch("Tile_Nhit_DM", &m_dm_nchan, "Tile_Nhit_DM/I");

  m_ntuplePtr->Branch("Tile_Active_Total_Ene", &m_act_totE, "Tile_Active_Total_Ene/F");
  m_ntuplePtr->Branch("Tile_Active_Vis_Ene", &m_act_visE, "Tile_Active_Vis_Ene/F");
  m_ntuplePtr->Branch("Tile_Inact_Total_Ene", &m_inact_totE, "Tile_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("Tile_Inact_Vis_Ene", &m_inact_visE, "Tile_Inact_Vis_Ene/F");
  m_ntuplePtr->Branch("Tile_DM_Total_Ene", &m_DM_totE, "Tile_DM_Total_Ene/F");
  m_ntuplePtr->Branch("Tile_DM_Vis_Ene", &m_DM_visE, "Tile_DM_Vis_Ene/F");

  m_ntuplePtr->Branch("Barr_Active_Total_Ene", &m_barr_totE, "Barr_Active_Total_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_Em_Ene", &m_barr_Em, "Barr_Active_Em_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_NonEm_Ene", &m_barr_NonEm, "Barr_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_Invisible_Ene", &m_barr_Invisible, "Barr_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("Barr_Active_Escaped_Ene", &m_barr_Escaped, "Barr_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("Barr_Inact_Total_Ene", &m_inact_barr_totE, "Barr_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_Em_Ene", &m_inact_barr_Em, "Barr_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_NonEm_Ene", &m_inact_barr_NonEm, "Barr_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_Invisible_Ene", &m_inact_barr_Invisible, "Barr_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("Barr_Inact_Escaped_Ene", &m_inact_barr_Escaped, "Barr_Inact_Escaped_Ene/F");

  m_ntuplePtr->Branch("Ext_Active_Total_Ene", &m_ext_totE, "Ext_Active_Total_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_Em_Ene", &m_ext_Em, "Ext_Active_Em_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_NonEm_Ene", &m_ext_NonEm, "Ext_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_Invisible_Ene", &m_ext_Invisible, "Ext_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("Ext_Active_Escaped_Ene", &m_ext_Escaped, "Ext_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("Ext_Inact_Total_Ene", &m_inact_ext_totE, "Ext_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_Em_Ene", &m_inact_ext_Em, "Ext_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_NonEm_Ene", &m_inact_ext_NonEm, "Ext_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_Invisible_Ene", &m_inact_ext_Invisible, "Ext_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("Ext_Inact_Escaped_Ene", &m_inact_ext_Escaped, "Ext_Inact_Escaped_Ene/F");

  m_ntuplePtr->Branch("ITC_Active_Total_Ene", &m_itc_totE, "ITC_Active_Total_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_Em_Ene", &m_itc_Em, "ITC_Active_Em_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_NonEm_Ene", &m_itc_NonEm, "ITC_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_Invisible_Ene", &m_itc_Invisible, "ITC_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("ITC_Active_Escaped_Ene", &m_itc_Escaped, "ITC_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("ITC_Inact_Total_Ene", &m_inact_itc_totE, "ITC_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_Em_Ene", &m_inact_itc_Em, "ITC_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_NonEm_Ene", &m_inact_itc_NonEm, "ITC_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_Invisible_Ene", &m_inact_itc_Invisible, "ITC_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("ITC_Inact_Escaped_Ene", &m_inact_itc_Escaped, "ITC_Inact_Escaped_Ene/F");

  m_ntuplePtr->Branch("GapScin_Active_Total_Ene", &m_gscin_totE, "GapScin_Active_Total_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_Em_Ene", &m_gscin_Em, "GapScin_Active_Em_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_NonEm_Ene", &m_gscin_NonEm, "GapScin_Active_NonEm_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_Invisible_Ene", &m_gscin_Invisible, "GapScin_Active_Invisible_Ene/F");
  m_ntuplePtr->Branch("GapScin_Active_Escaped_Ene", &m_gscin_Escaped, "GapScin_Active_Escaped_Ene/F");

  m_ntuplePtr->Branch("GapScin_Inact_Total_Ene", &m_inact_gscin_totE, "GapScin_Inact_Total_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_Em_Ene", &m_inact_gscin_Em, "GapScin_Inact_Em_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_NonEm_Ene", &m_inact_gscin_NonEm, "GapScin_Inact_NonEm_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_Invisible_Ene", &m_inact_gscin_Invisible, "GapScin_Inact_Invisible_Ene/F");
  m_ntuplePtr->Branch("GapScin_Inact_Escaped_Ene", &m_inact_gscin_Escaped, "GapScin_Inact_Escaped_Ene/F");

  // The fields characterizing the Hit
  m_ntuplePtr->Branch("Tile_Active_Total_Ene", &m_Total);
  m_ntuplePtr->Branch("Tile_Active_Em_Ene", &m_Em);
  m_ntuplePtr->Branch("Tile_Active_NonEm_Ene", &m_NonEm);
  m_ntuplePtr->Branch("Tile_Active_Invisible_Ene", &m_Invisible);
  m_ntuplePtr->Branch("Tile_Active_Escaped_Ene", &m_Escaped);
  m_ntuplePtr->Branch("Tile_Active_subCalo", &m_subCalo);
  m_ntuplePtr->Branch("Tile_Active_section", &m_section);
  m_ntuplePtr->Branch("Tile_Active_side", &m_side);
  m_ntuplePtr->Branch("Tile_Active_module", &m_module);
  m_ntuplePtr->Branch("Tile_Active_tower", &m_tower);
  m_ntuplePtr->Branch("Tile_Active_sample", &m_sample);

  m_ntuplePtr->Branch("Tile_Inact_Total_Ene", &m_inact_Total);
  m_ntuplePtr->Branch("Tile_Inact_Em_Ene", &m_inact_Em);
  m_ntuplePtr->Branch("Tile_Inact_NonEm_Ene", &m_inact_NonEm);
  m_ntuplePtr->Branch("Tile_Inact_Invisible_Ene", &m_inact_Invisible);
  m_ntuplePtr->Branch("Tile_Inact_Escaped_Ene", &m_inact_Escaped);
  m_ntuplePtr->Branch("Tile_Inact_subCalo", &m_inact_subCalo);
  m_ntuplePtr->Branch("Tile_Inact_section", &m_inact_section);
  m_ntuplePtr->Branch("Tile_Inact_side", &m_inact_side);
  m_ntuplePtr->Branch("Tile_Inact_module", &m_inact_module);
  m_ntuplePtr->Branch("Tile_Inact_tower", &m_inact_tower);
  m_ntuplePtr->Branch("Tile_Inact_sample", &m_inact_sample);

  m_ntuplePtr->Branch("Tile_DM_Ene", &m_dm_ene);
  m_ntuplePtr->Branch("Tile_DM_Vis", &m_dm_vis);
  m_ntuplePtr->Branch("Tile_DM_subDetector", &m_dm_subDet);
  m_ntuplePtr->Branch("Tile_DM_type", &m_dm_type);
  m_ntuplePtr->Branch("Tile_DM_sampling", &m_dm_sampling);
  m_ntuplePtr->Branch("Tile_DM_region", &m_dm_region);
  m_ntuplePtr->Branch("Tile_DM_eta", &m_dm_eta);
  m_ntuplePtr->Branch("Tile_DM_phi", &m_dm_phi);

  CHECK(m_thistSvc->regTree("/" + m_streamName + "/" + m_ntupleID, m_ntuplePtr));

  return StatusCode::SUCCESS;
}

/**
 * Executer
 */
StatusCode TileCalibHitCntNtup::StoreCNT(const CaloCalibrationHitContainer* ActiveHitCnt,
                                         const CaloCalibrationHitContainer* InactiveHitCnt,
                                         const CaloCalibrationHitContainer* dmHitCnt) {

  m_act_nchan = m_inact_nchan = m_dm_nchan = 0;

  m_act_totE = m_act_visE = m_inact_totE = m_inact_visE = m_DM_totE = m_DM_visE = 0.;

  m_barr_totE = 0.;
  m_ext_totE = 0.;
  m_itc_totE = 0.;
  m_gscin_totE = 0.;
  m_barr_Em = 0.;
  m_ext_Em = 0.;
  m_itc_Em = 0.;
  m_gscin_Em = 0.;
  m_barr_NonEm = 0.;
  m_ext_NonEm = 0.;
  m_itc_NonEm = 0.;
  m_gscin_NonEm = 0.;
  m_barr_Invisible = 0.;
  m_ext_Invisible = 0.;
  m_itc_Invisible = 0.;
  m_gscin_Invisible = 0.;
  m_barr_Escaped = 0.;
  m_ext_Escaped = 0.;
  m_itc_Escaped = 0.;
  m_gscin_Escaped = 0.;

  m_inact_barr_totE = 0.;
  m_inact_ext_totE = 0.;
  m_inact_itc_totE = 0.;
  m_inact_gscin_totE = 0.;
  m_inact_barr_Em = 0.;
  m_inact_ext_Em = 0.;
  m_inact_itc_Em = 0.;
  m_inact_gscin_Em = 0.;
  m_inact_barr_NonEm = 0.;
  m_inact_ext_NonEm = 0.;
  m_inact_itc_NonEm = 0.;
  m_inact_gscin_NonEm = 0.;
  m_inact_barr_Invisible = 0.;
  m_inact_ext_Invisible = 0.;
  m_inact_itc_Invisible = 0.;
  m_inact_gscin_Invisible = 0.;
  m_inact_barr_Escaped = 0.;
  m_inact_ext_Escaped = 0.;
  m_inact_itc_Escaped = 0.;
  m_inact_gscin_Escaped = 0.;

  m_Total->clear();
  m_Em->clear();
  m_NonEm->clear();
  m_Invisible->clear();
  m_Escaped->clear();

  m_subCalo->clear();
  m_section->clear();
  m_side->clear();
  m_module->clear();
  m_tower->clear();
  m_sample->clear();

  m_inact_Total->clear();
  m_inact_Em->clear();
  m_inact_NonEm->clear();
  m_inact_Invisible->clear();
  m_inact_Escaped->clear();

  m_inact_subCalo->clear();
  m_inact_section->clear();
  m_inact_side->clear();
  m_inact_module->clear();
  m_inact_tower->clear();
  m_inact_sample->clear();

  m_dm_ene->clear();
  m_dm_vis->clear();
  m_dm_subDet->clear();
  m_dm_type->clear();
  m_dm_sampling->clear();
  m_dm_region->clear();
  m_dm_eta->clear();
  m_dm_phi->clear();

  //------------ Go inside the CalibHit containers ------------
  int nhit;
  char text_info[100];
  if (ActiveHitCnt) {
    nhit = ActiveHitCnt->Size();
    sprintf(text_info, "Number of CalibHits in Tile ActiveCell container   = %d", nhit);
    ATH_MSG_INFO(text_info);

    if (nhit != 0) {
      CaloCalibrationHitContainer::const_iterator it = ActiveHitCnt->begin();
      CaloCalibrationHitContainer::const_iterator end = ActiveHitCnt->end();

      for (; it != end; it++) {
        m_act_totE += (*it)->energyTotal();
        m_act_visE += (*it)->energyEM() + (*it)->energyNonEM();

        m_Total->push_back( (*it)->energyTotal());
        m_Em->push_back( (*it)->energyEM());
        m_NonEm->push_back( (*it)->energyNonEM());
        m_Invisible->push_back( (*it)->energyInvisible());
        m_Escaped->push_back( (*it)->energyEscaped());

        Identifier id = (*it)->cellID();

        m_subCalo->push_back(m_caloCell_ID->sub_calo(id));
        m_section->push_back(m_caloCell_ID->section(id));
        m_side->push_back(m_caloCell_ID->side(id));
        m_module->push_back(m_caloCell_ID->module(id));
        m_tower->push_back(m_caloCell_ID->tower(id));
        m_sample->push_back(m_caloCell_ID->sample(id));

        if (m_caloCell_ID->is_tile_barrel(id)) {
          m_barr_totE += (*it)->energyTotal();
          m_barr_Em += (*it)->energyEM();
          m_barr_NonEm += (*it)->energyNonEM();
          m_barr_Invisible += (*it)->energyInvisible();
          m_barr_Escaped += (*it)->energyEscaped();
        } else if (m_caloCell_ID->is_tile_extbarrel(id)) {
          m_ext_totE += (*it)->energyTotal();
          m_ext_Em += (*it)->energyEM();
          m_ext_NonEm += (*it)->energyNonEM();
          m_ext_Invisible += (*it)->energyInvisible();
          m_ext_Escaped += (*it)->energyEscaped();
        } else if (m_caloCell_ID->is_tile_gapscin(id)) {
          m_gscin_totE += (*it)->energyTotal();
          m_gscin_Em += (*it)->energyEM();
          m_gscin_NonEm += (*it)->energyNonEM();
          m_gscin_Invisible += (*it)->energyInvisible();
          m_gscin_Escaped += (*it)->energyEscaped();
        } else if (m_caloCell_ID->is_tile_gap(id)) {
          m_itc_totE += (*it)->energyTotal();
          m_itc_Em += (*it)->energyEM();
          m_itc_NonEm += (*it)->energyNonEM();
          m_itc_Invisible += (*it)->energyInvisible();
          m_itc_Escaped += (*it)->energyEscaped();
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

  if (InactiveHitCnt) {
    nhit = InactiveHitCnt->Size();
    sprintf(text_info, "Number of CalibHits in Tile InactiveCell container = %d", nhit);
    ATH_MSG_INFO(text_info);

    if (nhit != 0) {
      CaloCalibrationHitContainer::const_iterator it = InactiveHitCnt->begin();
      CaloCalibrationHitContainer::const_iterator end = InactiveHitCnt->end();

      for (; it != end; it++) {
        m_inact_totE += (*it)->energyTotal();
        m_inact_visE += (*it)->energyEM() + (*it)->energyNonEM();

        m_inact_Total->push_back( (*it)->energyTotal());
        m_inact_Em->push_back( (*it)->energyEM());
        m_inact_NonEm->push_back( (*it)->energyNonEM());
        m_inact_Invisible->push_back( (*it)->energyInvisible());
        m_inact_Escaped->push_back( (*it)->energyEscaped());

        Identifier id = (*it)->cellID();

        m_inact_subCalo->push_back(m_caloCell_ID->sub_calo(id));
        m_inact_section->push_back(m_caloCell_ID->section(id));
        m_inact_side->push_back(m_caloCell_ID->side(id));
        m_inact_module->push_back(m_caloCell_ID->module(id));
        m_inact_tower->push_back(m_caloCell_ID->tower(id));
        m_inact_sample->push_back(m_caloCell_ID->sample(id));

        if (m_caloCell_ID->is_tile_barrel(id)) {
          m_inact_barr_totE += (*it)->energyTotal();
          m_inact_barr_Em += (*it)->energyEM();
          m_inact_barr_NonEm += (*it)->energyNonEM();
          m_inact_barr_Invisible += (*it)->energyInvisible();
          m_inact_barr_Escaped += (*it)->energyEscaped();
        } else if (m_caloCell_ID->is_tile_extbarrel(id)) {
          m_inact_ext_totE += (*it)->energyTotal();
          m_inact_ext_Em += (*it)->energyEM();
          m_inact_ext_NonEm += (*it)->energyNonEM();
          m_inact_ext_Invisible += (*it)->energyInvisible();
          m_inact_ext_Escaped += (*it)->energyEscaped();
        } else if (m_caloCell_ID->is_tile_gapscin(id)) {
          m_inact_gscin_totE += (*it)->energyTotal();
          m_inact_gscin_Em += (*it)->energyEM();
          m_inact_gscin_NonEm += (*it)->energyNonEM();
          m_inact_gscin_Invisible += (*it)->energyInvisible();
          m_inact_gscin_Escaped += (*it)->energyEscaped();
        } else if (m_caloCell_ID->is_tile_gap(id)) {
          m_inact_itc_totE += (*it)->energyTotal();
          m_inact_itc_Em += (*it)->energyEM();
          m_inact_itc_NonEm += (*it)->energyNonEM();
          m_inact_itc_Invisible += (*it)->energyInvisible();
          m_inact_itc_Escaped += (*it)->energyEscaped();
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

  if (dmHitCnt) {
    nhit = dmHitCnt->Size();
    sprintf(text_info, "Number of CalibHits in Tile DeadMaterial container = %d", nhit);
    ATH_MSG_INFO(text_info);

    if (nhit != 0) {
      CaloCalibrationHitContainer::const_iterator it = dmHitCnt->begin();
      CaloCalibrationHitContainer::const_iterator end = dmHitCnt->end();

      for (; it != end; it++) {
        m_DM_totE += (*it)->energyTotal();
        m_DM_visE += (*it)->energyEM() + (*it)->energyNonEM();

        Identifier id = (*it)->cellID();

        m_dm_ene->push_back( (*it)->energyTotal());
        m_dm_vis->push_back( (*it)->energyEM() + (*it)->energyNonEM());
        m_dm_subDet->push_back(m_caloDM_ID->pos_neg_z(id));
        m_dm_type->push_back(m_caloDM_ID->dmat(id));
        m_dm_sampling->push_back(m_caloDM_ID->sampling(id));
        m_dm_region->push_back(m_caloDM_ID->region(id));
        m_dm_eta->push_back(m_caloDM_ID->eta(id));
        m_dm_phi->push_back(m_caloDM_ID->phi(id));

        ++m_dm_nchan;
        if (m_dm_nchan >= m_max_chan) break;
      }
    }
  } else {
    ATH_MSG_WARNING("TileCalibHitDeadMaterial not exists for this event");
  }

  if (m_ntuplePtr->Fill()==0) {
    ATH_MSG_ERROR("StoreCNT(): Unable to Fill Cnt Ntuple");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("StoreCNT() completed successfully");
    return StatusCode::SUCCESS;
  }
}

/**
 * Finalizer
 */
StatusCode TileCalibHitCntNtup::finalize() {
  ATH_MSG_INFO("Finalizing");
  return StatusCode::SUCCESS;
}

