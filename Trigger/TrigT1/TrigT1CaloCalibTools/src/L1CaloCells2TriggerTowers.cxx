/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"

namespace LVL1{
  L1CaloCells2TriggerTowers::L1CaloCells2TriggerTowers(const std::string& name) : 
    asg::AsgTool( name ),
    m_caloMgr(nullptr),
    m_lvl1Helper(nullptr),
    m_tileID(nullptr),
    m_caloCellHelper(nullptr),
    m_larCablingSvc(nullptr),
    m_tileCablingService(nullptr),
    m_ttSvc(nullptr),
    m_bInitialized(false),
    m_bLArDigitsInitialized(false),
    m_bTileDigitsInitialized(false)
  {
  }

  // Return the Calo Cells associated with a TriggerTower
  std::vector<const CaloCell*> L1CaloCells2TriggerTowers::caloCells(const Identifier& ttId) const {

    std::map<unsigned int, std::vector<const CaloCell*> >::const_iterator it = m_mTTCaloCells.find(ttId.get_identifier32().get_compact());
    if(it!=m_mTTCaloCells.end()) {
      return it->second;

    } else {
      return std::vector<const CaloCell*>();
    }
  }

  // Return the Calo Cells associated with a TriggerTower by layers
  std::vector<std::vector<const CaloCell*> > L1CaloCells2TriggerTowers::caloCellsByLayer(const Identifier& ttId) const {
    const std::vector<const CaloCell*> vCells(this->caloCells(ttId));
    std::vector<std::vector<const CaloCell*> > ccByLayer;
    std::vector<const CaloCell*> cc;
    int caloSampleInUse(-10); // start this varibale with unrealistic value
    bool firstCell(true);
    if(vCells.size()!=0) {
      std::vector<const CaloCell*>::const_iterator itCells = vCells.begin();
      for(;itCells!=vCells.end();++itCells) {
        Identifier Id((*itCells)->ID());

        if(firstCell == true){caloSampleInUse = m_caloCellHelper->calo_sample(Id);firstCell = false;}
        int currentCaloSample = m_caloCellHelper->calo_sample(Id);
        if( caloSampleInUse != currentCaloSample ){
          ccByLayer.push_back(cc);
          cc.clear();
          caloSampleInUse = currentCaloSample;
        }
        cc.push_back(*itCells);
      }
      ccByLayer.push_back(cc);
    }

    return ccByLayer;
  }

  // Return the Calo Cell layer enum
  std::vector<int> L1CaloCells2TriggerTowers::layerNames(const Identifier& ttId) const {
    const std::vector<const CaloCell*> vCells(this->caloCells(ttId));
    std::vector<int> layers;
    int caloSampleInUse(-10); // start this varibale with unrealistic value
    if(vCells.size()!=0) {
      std::vector<const CaloCell*>::const_iterator itCells = vCells.begin();
      for(;itCells!=vCells.end();++itCells) {
        Identifier Id((*itCells)->ID());

        int currentCaloSample = m_caloCellHelper->calo_sample(Id);
        if( caloSampleInUse != currentCaloSample ){
          layers.push_back(currentCaloSample);
          caloSampleInUse = currentCaloSample;
        }
      }
    }
    return layers;
  }

  int L1CaloCells2TriggerTowers::layerNames(const CaloCell* cell) const{
    return m_caloCellHelper->calo_sample( cell->ID() );
  }

  // Return the energy sum from a vector of calo cells
  float L1CaloCells2TriggerTowers::energy(const std::vector<const CaloCell*> &cells) const {
    // mode : 0 == Energy, 1 == Et
    unsigned int mode(0);
    return this->calcEnergyOrEt(cells,mode);
  }

  // Return the Et sum from a vector of calo cells
  float L1CaloCells2TriggerTowers::et(const std::vector<const CaloCell*> &cells) const {
    // mode : 0 == Energy, 1 == Et
    unsigned int mode(1);
    return this->calcEnergyOrEt(cells,mode);
  }

  float L1CaloCells2TriggerTowers::energy(const Identifier& ttId) const {
    return this->energy( this->caloCells(ttId) );
  }
  
  float L1CaloCells2TriggerTowers::et(const Identifier& ttId) const {
    return this->et( this->caloCells(ttId) );
  }

  // Calculate Energy or Et
  float L1CaloCells2TriggerTowers::calcEnergyOrEt(const std::vector<const CaloCell*> &cells,const unsigned int mode) const{
    float energy(0.),et(0.),GeV(1000.0);
    if(cells.size()!=0) {
      typedef std::vector<const CaloCell*>::const_iterator Itr;

      for(Itr i=cells.begin();i!=cells.end();++i) {
        Identifier Id((*i)->ID());
        float cellEnergy(0.0);
        // Tile cells
        if(m_caloCellHelper->is_tile(Id)) {
          int section = m_caloCellHelper->section(Id);
          int sample = m_caloCellHelper->sample(Id);

          if( (section==1 || section==2) && sample==2) { //D-cells, except D4
            cellEnergy+=(*i)->energy()*0.5;

          } else {
            cellEnergy+=(*i)->energy();
          }

        // LArg cells
        } else {
          cellEnergy+=(*i)->energy(); // sampling weights between layers in TBB ?
        }
        if(mode == 0){energy += cellEnergy;}
        if(mode == 1){et += cellEnergy/cosh((*i)->eta());}
      }
    }
    if(mode == 0){return energy / GeV;}
    if(mode == 1){return et / GeV;}
    return energy / GeV;
  }

  // The rest of this file is initialize or finalise or legacy


  StatusCode L1CaloCells2TriggerTowers::initialize() {
    ATH_MSG_INFO("Initialize LVL1::L1CaloCells2TriggerTowers");

    StatusCode sc;

    sc = detStore()->retrieve(m_caloMgr);
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load CaloMgr");return sc;}

    m_caloCellHelper = m_caloMgr->getCaloCell_ID();
    if (!m_caloCellHelper) {
      ATH_MSG_ERROR("Could not access CaloCell_ID helper");
      return StatusCode::FAILURE;
    }

      // Get LVL1 idhelper from detector store and add to mgr
      const CaloLVL1_ID* lvl1_id = 0;
      sc = detStore()->retrieve(lvl1_id, "CaloLVL1_ID");
      if(sc.isFailure()){ATH_MSG_ERROR("Failed to get CaloLVL1_ID");return sc;}
      m_lvl1Helper = (CaloLVL1_ID*) lvl1_id;

      sc = detStore()->retrieve(m_tileID, "TileID") ;
      if(sc.isFailure()){ATH_MSG_ERROR("Failed to get Tile_ID");return sc;}

      //Retrieve cabling & tt services
      ISvcLocator* svcLoc = Gaudi::svcLocator( );
      IToolSvc* toolSvc;

      sc = svcLoc->service( "ToolSvc",toolSvc  );
      if(sc.isSuccess()) {
        sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc);
        if(sc.isFailure()){ATH_MSG_ERROR("Could not retrieve CaloTriggerTowerService Tool");return sc;}
        
        sc = toolSvc->retrieveTool("LArCablingLegacyService", m_larCablingSvc);
        if(sc.isFailure()){ATH_MSG_ERROR("Could not retrieve LArCablingService");return sc;}

      }
      else{ATH_MSG_ERROR("Could not retrieve ToolSvc");return sc;}

      m_tileCablingService = TileCablingService::getInstance();

      return sc;
  }

  StatusCode L1CaloCells2TriggerTowers::finalize() {
      return StatusCode::SUCCESS;
  }

  bool L1CaloCells2TriggerTowers::initCaloCellsTriggerTowers(const CaloCellContainer& cellContainer) {

	  this->resetCaloCells();

	  std::vector<Identifier>::const_iterator it_towerId  = m_lvl1Helper->tower_begin();
	  std::vector<Identifier>::const_iterator it_towerEnd = m_lvl1Helper->tower_end();

	  // loop on online channels
	  for(; it_towerId!=it_towerEnd;++it_towerId){
		  m_mTTCaloCells[(*it_towerId).get_identifier32().get_compact()].reserve(60);
	  }

	  CaloCellContainer::const_iterator itCell = cellContainer.begin();
	  for (;itCell!=cellContainer.end();++itCell){

		  const CaloCell * caloCell= *itCell;
		  const Identifier invalidId(0);
		  Identifier ttId1;
		  Identifier ttId2;

		  this->matchCell2Tower(caloCell, ttId1, ttId2);

		  if (ttId1 != invalidId) {
			  m_mTTCaloCells[ttId1.get_identifier32().get_compact()].push_back(caloCell);
		  }
		  if (ttId2 != invalidId) { // Tile D cells match to two towers
			  m_mTTCaloCells[ttId2.get_identifier32().get_compact()].push_back(caloCell);
		  }

	  } // end loop on calo cells

	  m_bInitialized = true;
	  return m_bInitialized;
  }

  void L1CaloCells2TriggerTowers::matchCell2Tower(const CaloCell* caloCell, Identifier& ttId1, Identifier& ttId2) const {

	  const Identifier invalidId(0);
	  ttId1 = invalidId;
	  ttId2 = invalidId;
	  const Identifier cellId(caloCell->ID());

	  // Tile cells
	  if (m_caloCellHelper->is_tile(cellId)) {

		  int section = m_caloCellHelper->section(cellId);
		  int sample = m_caloCellHelper->sample(cellId);

		  // gap detector - D4 & C10 cells
		  // both pmt of D4 belong to the same TT
		  if(section==3 && (sample==1 || sample==2) ) {
			  ttId1 = m_tileCablingService->cell2tt_id(cellId);

		  // barrel & ext. barrel - D cells
		  } else if( (section==1 || section==2) && sample==2) {

			  // special treatment of D-cells
			  // each pmt is affected to a different trigger tower
			  // each D-CaloCell is then added to 2 TT. This must ne taken into account when computing the TT energy
			  // from the CaloCells by dividing the D-cell energy by 2.
			  ttId1 = m_tileCablingService->pmt2tt_id(m_tileID->pmt_id(cellId, 0));
			  ttId2 = m_tileCablingService->pmt2tt_id(m_tileID->pmt_id(cellId, 1));

		  // barrel & ext. barrel - A & BC cells
		  } else if( (section==1 || section==2) && (sample==0 || sample==1) ) {
			  ttId1 = m_tileCablingService->cell2tt_id(cellId);
		  }

	  //LArg cells
	  } else {
		  //whichTTID ttId returns a layer_id, not a tower_id !
		  const Identifier layerId(m_ttSvc->whichTTID(cellId));

		  // We are not calling here m_ttSvc->is_in_lvl1(cellId) but use directly its code for a performance reason
		  // By doing so we save a redundant and costly call to m_ttSvc->whichTTID()
		  bool lvl1(true);
		  if(m_caloCellHelper->is_em_barrel(cellId)) {
			  int samp=m_caloCellHelper->sampling(cellId);
			  if(samp==0) {
				  int eta=m_caloCellHelper->eta(cellId);
				  if(eta==60) {
				  lvl1 = false ;
			  }
		    }
		  } else {
			  if(m_lvl1Helper->is_hec(layerId)) {
				  int layer(m_lvl1Helper->layer(layerId));
				  if(layer == 3){
					  lvl1 = false ;
				  }
			  }
		  }

		  if(lvl1) {
			  // check if the channel is connected
			  HWIdentifier channelID = m_larCablingSvc->createSignalChannelID(cellId);
			  if(m_larCablingSvc->isOnlineConnected(channelID)) {

				  // As we don't want the layer information embedded in the identifier, we recreate a tower Id
				  ttId1 = m_lvl1Helper->tower_id( m_lvl1Helper->pos_neg_z(layerId), m_lvl1Helper->sampling(layerId), m_lvl1Helper->region(layerId), m_lvl1Helper->eta(layerId), m_lvl1Helper->phi(layerId));
			  }
		  }
	  }

  }

  bool L1CaloCells2TriggerTowers::initLArDigitsTriggerTowers(const LArDigitContainer& larDigitContainer) {

	  this->resetLArDigits();

      std::vector<Identifier>::const_iterator it_towerId    = m_lvl1Helper->tower_begin();
	  std::vector<Identifier>::const_iterator it_towerEnd = m_lvl1Helper->tower_end();

      // loop on online channels
      for(; it_towerId!=it_towerEnd;++it_towerId){
	  m_mTTLArDigits[(*it_towerId).get_identifier32().get_compact()].reserve(60);
      }

	  LArDigitContainer::const_iterator itLArDigit = larDigitContainer.begin();
	  for (;itLArDigit!=larDigitContainer.end();++itLArDigit){

		  const LArDigit * larDigit= *itLArDigit;
		  const HWIdentifier larDigitChannelID = larDigit->channelID();

		  if(m_larCablingSvc->isOnlineConnected(larDigitChannelID)) {
			  // convert HWID to ID
			  Identifier larDigitID = m_larCablingSvc->cnvToIdentifier(larDigitChannelID);

			  //whichTTID ttId returns a layer_id, not a tower_id !
			  const Identifier layerId(m_ttSvc->whichTTID(larDigitID));

			  // We are not calling here m_ttSvc->is_in_lvl1(cellId) but use directly its code for a performance reason
			  // By doing so we save a redundant and costly call to m_ttSvc->whichTTID()
			  bool lvl1(true);
			  if(m_caloCellHelper->is_em_barrel(larDigitID)) {
				  int samp=m_caloCellHelper->sampling(larDigitID);
				  if(samp==0) {
					  int eta=m_caloCellHelper->eta(larDigitID);
					  if(eta==60) {
					  lvl1 = false ;
				  }
			    }
			  } else {
				  if(m_lvl1Helper->is_hec(layerId)) {
					  int layer(m_lvl1Helper->layer(layerId));
					  if(layer == 3){
						  lvl1 = false ;
					  }
				  }
			  }

			  if(lvl1) { //there are 1216 LArg Cells that are not part of lvl1
				  //whichTTID ttId returns a layer_id, not a tower_id !
				  // As we don't want the layer information embedded in the identifier, we recreate a tower Id
				  const Identifier ttId = m_lvl1Helper->tower_id( m_lvl1Helper->pos_neg_z(layerId), m_lvl1Helper->sampling(layerId), m_lvl1Helper->region(layerId), m_lvl1Helper->eta(layerId), m_lvl1Helper->phi(layerId));
				  m_mTTLArDigits[ttId.get_identifier32().get_compact()].push_back(larDigit);
			  }
		  }
	  } // end loop on calo cells

	  m_bLArDigitsInitialized = true;
	  return m_bLArDigitsInitialized;
  }

  bool L1CaloCells2TriggerTowers::initTileDigitsTriggerTowers(const TileDigitsContainer& tileDigitsContainer) {

	  this->resetTileDigits();


      std::vector<Identifier>::const_iterator it_towerId    = m_lvl1Helper->tower_begin();
	  std::vector<Identifier>::const_iterator it_towerEnd = m_lvl1Helper->tower_end();

      // loop on online channels
      for(; it_towerId!=it_towerEnd;++it_towerId){
	  m_mTTTileDigits[(*it_towerId).get_identifier32().get_compact()].reserve(8);
      }

	  TileDigitsContainer::const_iterator collItr = tileDigitsContainer.begin();
	  TileDigitsContainer::const_iterator lastColl = tileDigitsContainer.end();

	  for(; collItr!=lastColl; ++collItr) {

		  TileDigitsCollection::const_iterator digitsItr=(*collItr)->begin();
		  TileDigitsCollection::const_iterator lastDigits=(*collItr)->end();

		  if (digitsItr!=lastDigits) {
			  for(; digitsItr!=lastDigits; ++digitsItr) {

				  const TileDigits* tileDigit = *digitsItr;
				  const Identifier tileDigitID = tileDigit->cell_ID(); // return a cell_id (up to sampling info; pmt & adc set to 0)

				  if (m_caloCellHelper->is_tile(tileDigitID)) {

					  int section = m_tileID->section(tileDigitID);
					  int sample = m_tileID->sample(tileDigitID);

					  // gap detector - D4 & C10 cells
					  // both pmt of D4 belong to the same TT
					  if(section==3 && (sample==1 || sample==2) ) {
						  Identifier ttId = m_tileCablingService->cell2tt_id(tileDigitID);
						  m_mTTTileDigits[ttId.get_identifier32().get_compact()].push_back(tileDigit);

					  // barrel & ext. barrel - D cells
					  } else if( (section==1 || section==2) && sample==2) {

						  // special treatment of D-cells
						  // each pmt is affected to a different trigger tower
						  // each D-CaloCell is then added to 2 TT. This must ne taken into account when computing the TT energy
						  // from the CaloCells by dividing the D-cell energy by 2.
						  Identifier lTTId = m_tileCablingService->pmt2tt_id(m_tileID->pmt_id(tileDigitID, 0));
						  m_mTTTileDigits[lTTId.get_identifier32().get_compact()].push_back(tileDigit);

						  Identifier rTTId = m_tileCablingService->pmt2tt_id(m_tileID->pmt_id(tileDigitID, 1));
						  m_mTTTileDigits[rTTId.get_identifier32().get_compact()].push_back(tileDigit);

					  // barrel & ext. barrel - A & BC cells
					  } else if( (section==1 || section==2) && (sample==0 || sample==1) ) {
						  Identifier ttId = m_tileCablingService->cell2tt_id(tileDigitID);
						  m_mTTTileDigits[ttId.get_identifier32().get_compact()].push_back(tileDigit);
					  }
				  }
			  }
		  }
	  }

	  m_bTileDigitsInitialized= true;
	  return m_bTileDigitsInitialized;

	  return true;
  }



  void L1CaloCells2TriggerTowers::reset() {
    this->resetCaloCells();
    this->resetLArDigits();
    this->resetTileDigits();
  }

  void L1CaloCells2TriggerTowers::resetCaloCells() {
    m_bInitialized = false;
    m_mTTCaloCells.clear();
  }

  void L1CaloCells2TriggerTowers::resetLArDigits() {
    m_bLArDigitsInitialized = false;
    m_mTTLArDigits.clear();
  }

  void L1CaloCells2TriggerTowers::resetTileDigits() {
    m_bTileDigitsInitialized = false;
    m_mTTTileDigits.clear();
  }


  std::vector<const LArDigit*> L1CaloCells2TriggerTowers::larDigits(const Identifier& ttId) const {

    std::map<unsigned int, std::vector<const LArDigit*> >::const_iterator it = m_mTTLArDigits.find(ttId.get_identifier32().get_compact());
    if(it!=m_mTTLArDigits.end()) {
      return it->second;

    } else {
      return std::vector<const LArDigit*>();
    }
  }

  std::vector<const TileDigits*> L1CaloCells2TriggerTowers::tileDigits(const Identifier& ttId) const {

    std::map<unsigned int, std::vector<const TileDigits*> >::const_iterator it = m_mTTTileDigits.find(ttId.get_identifier32().get_compact());
    if(it!=m_mTTTileDigits.end()) {
      return it->second;

    } else {
      return std::vector<const TileDigits*>();
    }
  }



  std::vector<double> L1CaloCells2TriggerTowers::samples(const Identifier& ttId) const {

    if (m_lvl1Helper->is_tile(ttId)) {

      std::vector<const TileDigits*> vTileDigits(this->tileDigits(ttId));

      std::vector<double> vADCSum;
      int nSum=0;

      std::vector<const TileDigits*>::const_iterator it_digit = vTileDigits.begin();
      for(;it_digit!=vTileDigits.end();++it_digit) {

        std::vector<double> vADC((*it_digit)->get_digits());

        int adcSize(vADC.size());
        if(vADCSum.size()==0) vADCSum.resize(adcSize);

        for(int i=0;i<adcSize;++i) {
          vADCSum[i]+=vADC[i];
        }
        ++nSum;
      }

      std::vector<double>::iterator it_adc = vADCSum.begin();
      if(nSum > 0){
        const double inv_nSum = 1. / static_cast<double> (nSum);
        for(;it_adc!=vADCSum.end();++it_adc){
          (*it_adc) *= inv_nSum;
        }
      }

      return vADCSum;

    //LArg cells
    } else {
      //log<<MSG::INFO<<"is_lar"<<endmsg;
      std::vector<const LArDigit*> vLArDigits(this->larDigits(ttId));

      std::vector<double> vADCSum;
      int nSum=0;

      std::vector<const LArDigit*>::const_iterator it_digit = vLArDigits.begin();
      for(;it_digit!=vLArDigits.end();++it_digit) {

        std::vector<short> vADC((*it_digit)->samples());

        int adcSize(vADC.size());
        if(vADCSum.size()==0) vADCSum.resize(adcSize);

        for(int i=0;i<adcSize;++i) {
          vADCSum[i]+=vADC[i];
        }
        ++nSum;
      }

      std::vector<double>::iterator it_adc = vADCSum.begin();
      if(nSum > 0){
        const double inv_nSum = 1. / static_cast<double> (nSum);
        for(;it_adc!=vADCSum.end();++it_adc){
          (*it_adc) *= inv_nSum;
        }
      }

      return vADCSum;
    }
  }


  void L1CaloCells2TriggerTowers::dumpCaloCells(const Identifier& ttId) const {

    std::map<unsigned int, std::vector<const CaloCell*> >::const_iterator it = m_mTTCaloCells.find(ttId.get_identifier32().get_compact());
    if(it!=m_mTTCaloCells.end()) {

      ATH_MSG_INFO("Trigger Tower: " << ttId.get_compact() <<" - "<< m_lvl1Helper->pos_neg_z(ttId)<<" "<<m_lvl1Helper->sampling(ttId)<<" "<<m_lvl1Helper->region(ttId)<<" "<<m_lvl1Helper->eta(ttId)<<" "<<m_lvl1Helper->phi(ttId)<<" "<<m_lvl1Helper->layer(ttId)<<" - E: "<< this->energy(ttId) );

      std::vector<const CaloCell*> vCells = it->second;
      this->dump(vCells);

    } else {
      ATH_MSG_INFO("No trigger tower with Id="<< ttId.get_compact() <<" found.");
    }
  }

  void L1CaloCells2TriggerTowers::dumpCaloCells() const {


    ATH_MSG_INFO("dumping TT->CaloCells: "<< m_mTTCaloCells.size());
    std::map<unsigned int, std::vector<const CaloCell*> >::const_iterator it = m_mTTCaloCells.begin();
    for(;it!=m_mTTCaloCells.end();++it) {
      Identifier ttId(it->first);

      ATH_MSG_INFO("Trigger Tower: " << ttId.get_compact() <<" - "<< m_lvl1Helper->pos_neg_z(ttId)<<" "<<m_lvl1Helper->sampling(ttId)<<" "<<m_lvl1Helper->region(ttId)<<" "<<m_lvl1Helper->eta(ttId)<<" "<<m_lvl1Helper->phi(ttId)<<" "<<m_lvl1Helper->layer(ttId)<<" - E: "<< this->energy(ttId) );

      std::vector<const CaloCell*> vCells = it->second;
      this->dump(vCells);
    }
  }

  void L1CaloCells2TriggerTowers::dump(const std::vector<const CaloCell*>& vCells) const {


    ATH_MSG_INFO("ncells: "<< vCells.size());
    std::vector<const CaloCell*>::const_iterator itCells = vCells.begin();
    for(;itCells!=vCells.end();++itCells) {
      Identifier Id = (*itCells)->ID();

      if(m_caloCellHelper->is_tile(Id)) {
        int section = m_caloCellHelper->section(Id);
        int side = m_caloCellHelper->side(Id);
        int module = m_caloCellHelper->module(Id);
        int tower = m_caloCellHelper->tower(Id);
        int sample = m_caloCellHelper->sample(Id);
        ATH_MSG_INFO(" * tile - section: "<<section << ", side: "<<side<< ", module: "<<module<< ", tower: "<<tower<< ", sample: "<<sample<<" - eta: "<< (*itCells)->eta()<<", phi: "<< (*itCells)->phi()<< ", E:" << (*itCells)->energy() );

      } else if (m_caloCellHelper->is_fcal(Id)) {
        //needd fcal case
        int pos_neg = m_caloCellHelper->pos_neg(Id);
        int module = m_caloCellHelper->sampling(Id);
        int eta = m_caloCellHelper->eta(Id);
        int phi = m_caloCellHelper->phi(Id);
        ATH_MSG_INFO(" * fcal - pos_neg_z: "<<pos_neg<< ", module: "<<module<<", ieta: "<<eta<< ", iphi: "<<phi<<" - eta: "<< (*itCells)->eta()<<", phi: "<< (*itCells)->phi()<< ", E:" << (*itCells)->energy() );

      } else {
        int pos_neg = m_caloCellHelper->pos_neg(Id);
        int sampling = m_caloCellHelper->sampling(Id);
        int region = m_caloCellHelper->region(Id);
        int eta = m_caloCellHelper->eta(Id);
        int phi = m_caloCellHelper->phi(Id);
        ATH_MSG_INFO(" * larg - pos_neg_z: "<<pos_neg<< ", sampling: "<<sampling<< ", region: "<<region<< ", ieta: "<<eta<< ", iphi: "<<phi<<" - eta: "<< (*itCells)->eta()<<", phi: "<< (*itCells)->phi()<< ", E:" << (*itCells)->energy() );
      }
    }
  }

  void L1CaloCells2TriggerTowers::dump(const std::vector<const LArDigit*>& vLArDigits) const {


    ATH_MSG_INFO( "ncells: "<< vLArDigits.size() );
    std::vector<const LArDigit*>::const_iterator itLArDigits = vLArDigits.begin();
    for(;itLArDigits!=vLArDigits.end();++itLArDigits) {

      const LArDigit* larDigit= *itLArDigits;
      HWIdentifier hwId = larDigit->channelID();
      Identifier larDigitID = m_larCablingSvc->cnvToIdentifier(hwId);

      ATH_MSG_INFO( "pos_neg: " << m_caloCellHelper->pos_neg(larDigitID)<<", sampling: "<< m_caloCellHelper->sampling(larDigitID)<<", region: "<< m_caloCellHelper->region(larDigitID)<<", eta: "<< m_caloCellHelper->eta(larDigitID)<<", phi: "<< m_caloCellHelper->phi(larDigitID) );
            std::vector<short> vADC= larDigit->samples();
            std::vector<short>::const_iterator it_sample = vADC.begin();
            for(;it_sample!=vADC.end();++it_sample) std::cout<< *it_sample<<" ";
            std::cout<<std::endl;
    }
  }

  void L1CaloCells2TriggerTowers::dumpDigits(const Identifier& ttId) const {

    // if Tile
    if (m_lvl1Helper->is_tile(ttId)) {
      this->dumpTileDigits(ttId);

    //LArg cells
    } else {
      this->dumpLArDigits(ttId);
    }
  }


  void L1CaloCells2TriggerTowers::dumpLArDigits(const Identifier& ttId) const {

    std::map<unsigned int, std::vector<const LArDigit*> >::const_iterator it = m_mTTLArDigits.find(ttId.get_identifier32().get_compact());
    if(it!=m_mTTLArDigits.end()) {

      ATH_MSG_INFO( "Trigger Tower: " << ttId.get_compact() <<" - "<< m_lvl1Helper->pos_neg_z(ttId)<<" "<<m_lvl1Helper->sampling(ttId)<<" "<<m_lvl1Helper->region(ttId)<<" "<<m_lvl1Helper->eta(ttId)<<" "<<m_lvl1Helper->phi(ttId)<<" "<<m_lvl1Helper->layer(ttId)<<" - E: "<< this->energy(ttId) );
      std::vector<const LArDigit*> vLArDigits = it->second;
      this->dump(vLArDigits);
    } else {
      ATH_MSG_INFO( "No trigger tower with Id="<< ttId.get_compact() <<" found." );
    }
  }

  void L1CaloCells2TriggerTowers::dumpLArDigits() const {

    ATH_MSG_INFO( "dumping TT->LArDigits: "<< m_mTTLArDigits.size() );
    std::map<unsigned int, std::vector<const LArDigit*> >::const_iterator it = m_mTTLArDigits.begin();
    for(;it!=m_mTTLArDigits.end();++it) {
      Identifier ttId(it->first);

      ATH_MSG_INFO( "Trigger Tower: " << ttId.get_compact() <<" - "<< m_lvl1Helper->pos_neg_z(ttId)<<" "<<m_lvl1Helper->sampling(ttId)<<" "<<m_lvl1Helper->region(ttId)<<" "<<m_lvl1Helper->eta(ttId)<<" "<<m_lvl1Helper->phi(ttId)<<" "<<m_lvl1Helper->layer(ttId)<<" - E: "<< this->energy(ttId) );
      std::vector<const LArDigit*> vLArDigits = it->second;
      this->dump(vLArDigits);
    }
  }


  void L1CaloCells2TriggerTowers::dump(const std::vector<const TileDigits*>& vTileDigits) const {

    ATH_MSG_INFO( "ncells: "<< vTileDigits.size() );
    std::vector<const TileDigits*>::const_iterator itTileDigits = vTileDigits.begin();
    for(;itTileDigits!=vTileDigits.end();++itTileDigits) {

      const TileDigits* tileDigit= *itTileDigits;
      Identifier tileDigitID = tileDigit->identify();

      int section = m_caloCellHelper->section(tileDigitID);
      int side = m_caloCellHelper->side(tileDigitID);
      int module = m_caloCellHelper->module(tileDigitID);
      int tower = m_caloCellHelper->tower(tileDigitID);
      int sample = m_caloCellHelper->sample(tileDigitID);
      ATH_MSG_INFO( "section: "<<section << ", side: "<<side<< ", module: "<<module<< ", tower: "<<tower<< ", sample: "<<sample );
            std::vector<double> vADC = tileDigit->get_digits();
            std::vector<double>::const_iterator it_sample = vADC.begin();
            for(;it_sample!=vADC.end();++it_sample){
              ATH_MSG_INFO("  --> "<< *it_sample);
              ATH_MSG_INFO(" ");
            }
    }
  }

  void L1CaloCells2TriggerTowers::dumpTileDigits(const Identifier& ttId) const {

    std::map<unsigned int, std::vector<const TileDigits*> >::const_iterator it = m_mTTTileDigits.find(ttId.get_identifier32().get_compact());
    if(it!=m_mTTTileDigits.end()) {

      ATH_MSG_INFO( "Trigger Tower: " << ttId.get_compact() <<" - "<< m_lvl1Helper->pos_neg_z(ttId)<<" "<<m_lvl1Helper->sampling(ttId)<<" "<<m_lvl1Helper->region(ttId)<<" "<<m_lvl1Helper->eta(ttId)<<" "<<m_lvl1Helper->phi(ttId)<<" "<<m_lvl1Helper->layer(ttId)<<" - E: "<< this->energy(ttId) );
      std::vector<const TileDigits*> vTileDigits = it->second;
      this->dump(vTileDigits);
    } else {
      ATH_MSG_INFO( "No trigger tower with Id="<< ttId.get_compact() <<" found." );
    }
  }

  void L1CaloCells2TriggerTowers::dumpTileDigits() const {
    ATH_MSG_INFO( "dumping TT->TileDigits: "<< m_mTTTileDigits.size() );
    std::map<unsigned int, std::vector<const TileDigits*> >::const_iterator it = m_mTTTileDigits.begin();
    for(;it!=m_mTTTileDigits.end();++it) {
      Identifier ttId(it->first);

      ATH_MSG_INFO( "Trigger Tower: " << ttId.get_compact() <<" - "<< m_lvl1Helper->pos_neg_z(ttId)<<" "<<m_lvl1Helper->sampling(ttId)<<" "<<m_lvl1Helper->region(ttId)<<" "<<m_lvl1Helper->eta(ttId)<<" "<<m_lvl1Helper->phi(ttId)<<" "<<m_lvl1Helper->layer(ttId)<<" - E: "<< this->energy(ttId) );
      std::vector<const TileDigits*> vTileDigits = it->second;
      this->dump(vTileDigits);
    }
  }
} // end of namespace
