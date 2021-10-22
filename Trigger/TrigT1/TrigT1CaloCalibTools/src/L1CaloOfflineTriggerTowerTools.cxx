/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

// Like it says above, please chop up this code until it does what you want !

#include "TrigT1CaloCalibTools/L1CaloOfflineTriggerTowerTools.h"
#include "StoreGate/ReadCondHandle.h"

namespace LVL1 {

  L1CaloOfflineTriggerTowerTools::L1CaloOfflineTriggerTowerTools( const std::string& name ) :
    asg::AsgTool( name ),
    m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
    m_cells2tt("LVL1::L1CaloCells2TriggerTowers/L1CaloCells2TriggerTowers", this),
    m_scidtool ("CaloSuperCellIDTool"),
    m_scaleCorrKey(""),
    m_cablingKey(""),
    m_tileBadChanTool("TileBadChanTool"),
    m_rxMapTool("LVL1::L1CaloFcal23Cells2RxMappingTool/L1CaloFcal23Cells2RxMappingTool")
    {
    m_caloMgr = 0;
    m_ttSvc = 0;
    m_ttOnlineIdHelper = 0;
    m_lvl1Helper = 0;
    m_l1CondSvc = 0;
    m_larOnlineID = 0;
    m_isLArHVCorrToolSet = false;
    m_tileHWID = 0;
    
    declareProperty("LArHVNonNomPara",m_LArHVNonNomPara = 1.003);
    declareProperty("TileBadChanTool", m_tileBadChanTool);
    declareProperty("RxMappingTool",m_rxMapTool);
  }



  //////////////////////////////////////////////////////
  //   Location, ID and Navigation Information        //
  //////////////////////////////////////////////////////

  // EM

  int L1CaloOfflineTriggerTowerTools::emPpmCrate(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->crate(HWId);
  }

  int L1CaloOfflineTriggerTowerTools::emPpmModule(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->module(HWId);
  }

  int L1CaloOfflineTriggerTowerTools::emPpmSubmodule(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->submodule(HWId);
  }

  int L1CaloOfflineTriggerTowerTools::emPpmChannel(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->channel(HWId);
  }

  unsigned int L1CaloOfflineTriggerTowerTools::emCoolChannelId(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(HWId);
    return coolId.id();
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::emLocation(const TriggerTower* tt) const{
    std::vector<int> location;
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    location.push_back(m_ttOnlineIdHelper->crate(HWId));
    location.push_back(m_ttOnlineIdHelper->module(HWId));
    location.push_back(m_ttOnlineIdHelper->submodule(HWId));
    location.push_back(m_ttOnlineIdHelper->channel(HWId));
    L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(HWId);
    location.push_back(coolId.id());
    return location;
  }

  std::vector<L1CaloRxCoolChannelId> L1CaloOfflineTriggerTowerTools::emReceivers(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(HWId);
    std::vector<L1CaloRxCoolChannelId> rx = m_ttSvc->cnvCoolChannelIdToRxId(coolId);

    std::vector<L1CaloRxCoolChannelId> output;

    // Sort EM Crack recievers. End cap first, barrel second
    if(rx.size() == 3){

      // For the EM crack region, there are 3 recievers.
      // 1 Rx have Crate 0 or 1  - correspond to the barrel before the Octopus cable magic - ignore
      // 2 Rx have Crate 2 or 3  - correspond to the Rx we want:
      //      1 Rx has inputConn() == 0 - EMEC receiver
      //      1 Rx has inputConn() == 2 - Barrel receiver


      typedef std::vector<L1CaloRxCoolChannelId>::const_iterator Itr_Id;
      std::vector<L1CaloRxCoolChannelId> rx2;
      for(Itr_Id a=rx.begin();a!=rx.end();++a){
        if((*a).crate() == 2 || (*a).crate() == 3){ // Select only interesting recievers
          rx2.push_back(*a);
        }
      }

      if(rx2.size() == 2){

        unsigned int inputA = rx2.at(0).inputConn();
        unsigned int inputB = rx2.at(1).inputConn();

        // End cap first, barrel second
        if(inputA == 2 && inputB == 0){
          output.push_back(rx2.at(1));
          output.push_back(rx2.at(0));
        }
        if(inputA == 0 && inputB == 2){
          output.push_back(rx2.at(0));
          output.push_back(rx2.at(1));
        }
      }

    }

    if(rx.size() == 1){
      output.push_back(rx.at(0));
    }

    return output;
  }

  std::vector<unsigned int> L1CaloOfflineTriggerTowerTools::emRxId(const TriggerTower* tt) const{
    typedef std::vector<L1CaloRxCoolChannelId>::const_iterator Itr;
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    std::vector<unsigned int> output;
    for(Itr i=rx.begin();i!=rx.end();++i){
      output.push_back( (*i).id() );
    }
    return output;
  }

  // Had

  int L1CaloOfflineTriggerTowerTools::hadPpmCrate(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->crate(HWId);
  }

  int L1CaloOfflineTriggerTowerTools::hadPpmModule(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->module(HWId);
  }

  int L1CaloOfflineTriggerTowerTools::hadPpmSubmodule(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->submodule(HWId);
  }

  int L1CaloOfflineTriggerTowerTools::hadPpmChannel(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    return m_ttOnlineIdHelper->channel(HWId);
  }

  unsigned int L1CaloOfflineTriggerTowerTools::hadCoolChannelId(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(HWId);
    return coolId.id();
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::hadLocation(const TriggerTower* tt) const{
    std::vector<int> location;
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    location.push_back(m_ttOnlineIdHelper->crate(HWId));
    location.push_back(m_ttOnlineIdHelper->module(HWId));
    location.push_back(m_ttOnlineIdHelper->submodule(HWId));
    location.push_back(m_ttOnlineIdHelper->channel(HWId));
    L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(HWId);
    location.push_back(coolId.id());
    return location;
  }

  int L1CaloOfflineTriggerTowerTools::hadIsTile(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    if(m_lvl1Helper->is_tile(Id) == true){
      return 1;
    }
    return 0;
  }

  std::vector<L1CaloRxCoolChannelId> L1CaloOfflineTriggerTowerTools::hadReceivers(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    HWIdentifier HWId = m_ttSvc->createTTChannelID(Id);
    L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(HWId);
    std::vector<L1CaloRxCoolChannelId> rx = m_ttSvc->cnvCoolChannelIdToRxId(coolId);

    std::vector<L1CaloRxCoolChannelId> output;

    // Sort FCAL 23 recievers. Always have -ve eta first and +ve eta second
    if(rx.size() == 2){

      // Odd receiver is lowest |eta|
      unsigned int inputPairA = rx.at(0).inputPair();
      unsigned int inputPairB = rx.at(1).inputPair();

      bool oddA(false),oddB(false);
      if(inputPairA % 2 != 0){oddA = true;}
      if(inputPairB % 2 != 0){oddB = true;}

      if(oddA == true && oddB == false){
        output.push_back(rx.at(0));
        output.push_back(rx.at(1));
      }
      if(oddA == false && oddB == true){
        output.push_back(rx.at(1));
        output.push_back(rx.at(0));
      }

    }
    if(rx.size() == 1){
      output.push_back(rx.at(0));
    }

    return output;
  }


  std::vector<unsigned int> L1CaloOfflineTriggerTowerTools::hadRxId(const TriggerTower* tt) const{
    typedef std::vector<L1CaloRxCoolChannelId>::const_iterator Itr;
    std::vector<L1CaloRxCoolChannelId> rx = this->hadReceivers(tt);
    std::vector<unsigned int> output;
    for(Itr i=rx.begin();i!=rx.end();++i){
      output.push_back( (*i).id() );
    }
    return output;
  }


  //////////////////////////////////////////////////////
  //         Energy, Et and Layer Information         //
  //////////////////////////////////////////////////////

  // EM

  std::vector<const CaloCell*> L1CaloOfflineTriggerTowerTools::emCells(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    return m_cells2tt->caloCells(Id);
  }

  int L1CaloOfflineTriggerTowerTools::emNCells(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    return cells.size();
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::emNCellsByLayer(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<int> v;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      v.push_back((*i).size());
    }
    return v;
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::emNCellsByReceiver(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<int> v;
    std::vector<std::vector<const CaloCell*> > cells = this->sortEMCrackCells(m_cells2tt->caloCells(Id));
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      v.push_back((*i).size());
    }
    return v;
  }

  std::vector<std::vector<int> > L1CaloOfflineTriggerTowerTools::emNCellsByReceiverByLayer(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<std::vector<int> > v;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortEMCrackCells((*i));
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back((*j).size());
	  else                      v2.push_back((*j).size());
        }
      } else if (rx.size() == 1) v1.push_back((*i).size());
    }
    v.push_back(v1);
    if (rx.size() == 2) v.push_back(v2);
    return v;
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::emTTCellsLayerNames(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    return m_cells2tt->layerNames(Id);
  }

  std::vector<std::vector<int> > L1CaloOfflineTriggerTowerTools::emTTCellsLayerNamesByReceiver(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<std::vector<int> > v;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortEMCrackCells((*i));
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back(m_cells2tt->layerNames((*j)[0]));
	  else                      v2.push_back(m_cells2tt->layerNames((*j)[0]));
        }
      } else if (rx.size() == 1) v1.push_back(m_cells2tt->layerNames((*i)[0]));
    }
    v.push_back(v1);
    if (rx.size() == 2) v.push_back(v2);
    return v;
  }

  float L1CaloOfflineTriggerTowerTools::emTTCellsEnergy(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    return m_cells2tt->energy( m_cells2tt->caloCells(Id) );
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::emTTCellsEnergyByLayer(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    std::vector<float> eByLayer;
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      eByLayer.push_back( m_cells2tt->energy( *i ) );
    }
    return eByLayer;
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::emTTCellsEnergyByReceiver(const TriggerTower* tt,const int mode) const{
    // Mode 0 : Return vector<float> even if it has 1 entry. Equivelent to emTTCellsEnergy
    // Mode 1 : Return vecotr<float> only if size is > 2. D3PDs don't need this
    std::vector<float> output;
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(rx.size() == 2){
      std::vector<std::vector<const CaloCell*> > vvcells = this->sortEMCrackCells(cells);
      for(Itr_vvCaloCells i=vvcells.begin();i!=vvcells.end();++i){
        output.push_back( m_cells2tt->energy( (*i) ) );
      }
    }

    if(rx.size() == 1 && mode == 0){
      output.push_back( m_cells2tt->energy( cells ) );
    }

    return output;
  }

  float L1CaloOfflineTriggerTowerTools::emTTCellsEt(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    return m_cells2tt->et( m_cells2tt->caloCells(Id) );
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::emTTCellsEtByLayer(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    std::vector<float> eByLayer;
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      eByLayer.push_back( m_cells2tt->et( *i ) );
    }
    return eByLayer;
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::emTTCellsEtByReceiver(const TriggerTower* tt,const int mode) const{
    // Mode 0 : Return vector<float> even if it has 1 entry. Equivelent to emTTCellsEt
    // Mode 1 : Return vecotr<float> only if size is > 2. D3PDs don't need this
    std::vector<float> output;
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(rx.size() == 2){
      std::vector<std::vector<const CaloCell*> > vvcells = this->sortEMCrackCells(cells);
      for(Itr_vvCaloCells i=vvcells.begin();i!=vvcells.end();++i){
        output.push_back( m_cells2tt->et( (*i) ) );
      }
    }

    if(rx.size() == 1 && mode == 0){
      output.push_back( m_cells2tt->et( cells ) );
    }

    return output;
  }


  std::vector<std::vector<const CaloCell*> > L1CaloOfflineTriggerTowerTools::sortEMCrackCells(const std::vector<const CaloCell*> &cells) const{
    std::vector<std::vector<const CaloCell*> > output;
    std::vector<const CaloCell*> emb;
    std::vector<const CaloCell*> emec;

    // End Cap first, Barrel second

    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      int layer = m_cells2tt->layerNames(*i);

      if( layer <= 3){
        emb.push_back( (*i) );
      }
      if( layer >= 4){
        emec.push_back( (*i) );
      }
    }

    output.push_back(emec);
    output.push_back(emb);
    return output;
  }

  // Had

  std::vector<const CaloCell*> L1CaloOfflineTriggerTowerTools::hadCells(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    return m_cells2tt->caloCells(Id);
  }

  int L1CaloOfflineTriggerTowerTools::hadNCells(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    return cells.size();
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::hadNCellsByLayer(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<int> v;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      v.push_back((*i).size());
    }
    return v;
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::hadNCellsByReceiver(const TriggerTower* tt) const{
    std::vector<unsigned int> rx = this->hadRxId(tt);
    Identifier Id = this->hadID(tt->eta(),tt->phi());

    std::vector<int> v;
    std::vector<std::vector<const CaloCell*> > cells = this->sortFCAL23Cells(m_cells2tt->caloCells(Id),rx);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      v.push_back((*i).size());
    }
    return v;
  }

  std::vector<std::vector<int> > L1CaloOfflineTriggerTowerTools::hadNCellsByReceiverByLayer(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<unsigned int> rx = this->hadRxId(tt);
    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<std::vector<int> > v;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortFCAL23Cells((*i),rx);
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back((*j).size());
	  else                      v2.push_back((*j).size());
        }
      } else if (rx.size() == 1) v1.push_back((*i).size());
    }
    v.push_back(v1);
    if (rx.size() == 2) v.push_back(v2);
    return v;
  }

  std::vector<int> L1CaloOfflineTriggerTowerTools::hadTTCellsLayerNames(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    return m_cells2tt->layerNames(Id);
  }

  std::vector<std::vector<int> > L1CaloOfflineTriggerTowerTools::hadTTCellsLayerNamesByReceiver(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<unsigned int> rx = this->hadRxId(tt);
    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<std::vector<int> > v;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortFCAL23Cells((*i),rx);
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back(m_cells2tt->layerNames((*j)[0]));
	  else                      v2.push_back(m_cells2tt->layerNames((*j)[0]));
        }
      } else if (rx.size() == 1) v1.push_back(m_cells2tt->layerNames((*i)[0]));
    }
    v.push_back(v1);
    if (rx.size() == 2) v.push_back(v2);
    return v;
  }

  float L1CaloOfflineTriggerTowerTools::hadTTCellsEnergy(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    return m_cells2tt->energy( m_cells2tt->caloCells(Id) );
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::hadTTCellsEnergyByLayer(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    std::vector<float> eByLayer;
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      eByLayer.push_back( m_cells2tt->energy( *i ) );
    }
    return eByLayer;
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::hadTTCellsEnergyByReceiver(const TriggerTower* tt,const int mode) const{
    // Mode 0 : Return vector<float> even if it has 1 entry. Equivelent to hadTTCellsEnergy
    // Mode 1 : Return vecotr<float> only if size is > 2. D3PDs don't need this
    std::vector<float> output;
    std::vector<unsigned int> rx = this->hadRxId(tt);
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(rx.size() == 2){
      std::vector<std::vector<const CaloCell*> > vvcells = this->sortFCAL23Cells(cells,rx);
      for(Itr_vvCaloCells i=vvcells.begin();i!=vvcells.end();++i){
        output.push_back( m_cells2tt->energy( (*i) ) );
      }
    }

    if(rx.size() == 1 && mode == 0){
      output.push_back( m_cells2tt->energy( cells ) );
    }

    return output;
  }

  float L1CaloOfflineTriggerTowerTools::hadTTCellsEt(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    return m_cells2tt->et( m_cells2tt->caloCells(Id) );
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::hadTTCellsEtByLayer(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    std::vector<float> eByLayer;
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      eByLayer.push_back( m_cells2tt->et( *i ) );
    }
    return eByLayer;
  }


  std::vector<float> L1CaloOfflineTriggerTowerTools::hadTTCellsEtByReceiver(const TriggerTower* tt,const int mode) const{
    // Mode 0 : Return vector<float> even if it has 1 entry. Equivelent to hadTTCellsEt
    // Mode 1 : Return vecotr<float> only if size is > 2. D3PDs don't need this
    std::vector<float> output;
    std::vector<unsigned int> rx = this->hadRxId(tt);
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(rx.size() == 2){
      std::vector<std::vector<const CaloCell*> > vvcells = this->sortFCAL23Cells(cells,rx);
      for(Itr_vvCaloCells i=vvcells.begin();i!=vvcells.end();++i){
        output.push_back( m_cells2tt->et( (*i) ) );
      }
    }

    if(rx.size() == 1 && mode == 0){
      output.push_back( m_cells2tt->et( cells ) );
    }

    return output;
  }

  float L1CaloOfflineTriggerTowerTools::tileCellEnergy(const TriggerTower* tt,IdTTL1CellMapType& map) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    if(m_lvl1Helper->is_tile(Id) == false){return 0.0;}
    else{
      IdTTL1CellMapType::const_iterator ttL1Cell(map.find(Id));
      IdTTL1CellMapType::const_iterator ttL1Cell_E(map.end());
      if(ttL1Cell != ttL1Cell_E){
        return ((ttL1Cell->second)->eneTower()) * 1e-3;
      }
      return 0.0;
    }
  }

  std::vector<std::vector<const CaloCell*> > L1CaloOfflineTriggerTowerTools::sortFCAL23Cells(const std::vector<const CaloCell*> &cells,
                                                                                const double eta) const{
    std::vector<std::vector<const CaloCell*> > output;

    std::vector<const CaloCell*> fcal2A;
    std::vector<const CaloCell*> fcal2B;
    std::vector<const CaloCell*> fcal3A;
    std::vector<const CaloCell*> fcal3B;
    double absEta = fabs(eta);

    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      int layer = m_cells2tt->layerNames(*i);
      const CaloDetDescrElement* caloDDE = (*i)->caloDDE();
      // _raw is ideal position
      double cellEta = fabs(caloDDE->eta_raw());
      double x = fabs(caloDDE->x_raw());
      double y = fabs(caloDDE->y_raw());

      // FCAL 2
      if (layer == 22) {
        // TTs split the eta range equally, hence the apparent inconsistencies in next line
        bool lowEta = (absEta < 4.05 && cellEta < 3.3) || (absEta > 4.05 && cellEta < 3.97);
        // Exceptions, based on radial map
        if ((x > 100. && x < 160. && y > 340. && y < 380.) ||
            (x > 320. && x < 360. && y > 170. && y < 210.) ||
            (x >  30. && x <  60. && y > 170. && y < 210.)) lowEta = true;
        if ((x > 160. && x < 190. && y > 340. && y < 380.) ||
            (x > 230. && x < 260. && y > 300. && y < 340.) ||
            (x > 130. && x < 160. && y > 140. && y < 170.) ||
            (x > 160. && x < 200. && y >  90. && y < 140.)) lowEta = false;
        if (lowEta) fcal2A.push_back(*i);
        else        fcal2B.push_back(*i);
      }
      // FCAL 3
      if (layer == 23) {
        bool lowEta = (absEta < 4.05 && cellEta < 3.43) || (absEta > 4.05 && cellEta < 3.9);
        // Exceptions
        double xs = caloDDE->x_raw();
        double ys = caloDDE->y_raw();
        if ((x  >  120. && x  <  160. && y  >  140. && y  <  200.) ||
            (x  >  160. && x  <  220. && y  >  100. && y  <  140.) ||
            (xs >  120. && xs <  160. && ys > -140. && ys < -100.) ||
            (xs > -160. && xs < -100. && ys >  100. && ys <  140.)) lowEta = true;
        if ((x  >    0. && x  <   60. && y  >  380. && y  <  420.) ||
            (xs >  320. && xs <  370. && ys >  140. && ys <  190.) ||
            (xs > -360. && xs < -320. && ys > -180. && ys < -140.)) lowEta = false;
        if (lowEta) fcal3A.push_back(*i);
        else        fcal3B.push_back(*i);
      }
    }

    unsigned int size2A = fcal2A.size();
    unsigned int size2B = fcal2B.size();
    unsigned int size3A = fcal3A.size();
    unsigned int size3B = fcal3B.size();

    // Always have lowEta first
    if(size2A > 0 && size2B > 0 && size3A == 0 && size3B == 0){ // FCAL 2
      output.push_back(fcal2A);
      output.push_back(fcal2B);
    }
    if(size2A == 0 && size2B == 0 && size3A > 0 && size3B > 0){ // FCAL 3
      output.push_back(fcal3A);
      output.push_back(fcal3B);
    }

    return output;
  }

  std::vector<std::vector<const CaloCell*> > L1CaloOfflineTriggerTowerTools::sortFCAL23Cells(const std::vector<const CaloCell*> &cells,
                                                                                const std::vector<unsigned int>& rxId) const{

    // vectors of calo cells for the different receivers
    std::vector<const CaloCell*> cellsA;
    std::vector<const CaloCell*> cellsB;
    // output
    std::vector<std::vector<const CaloCell*> > output;

    // RxID of the different receivers
    unsigned int rxidA = rxId.at(0);
    unsigned int rxidB = rxId.at(1);

    // Loop over calo cells and use mapping tool to assign cells to different receivers
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      unsigned int someRxId = m_rxMapTool->offlineCell2RxId( (*i)->ID().get_identifier32().get_compact() );
      if(someRxId == rxidA){cellsA.push_back(*i);}
      if(someRxId == rxidB){cellsB.push_back(*i);}
    }
    output.push_back(cellsA);
    output.push_back(cellsB);
    return output;
  }

  //////////////////////////////////////////////////////
  //      Bad Calo, High Voltage Information          //
  //////////////////////////////////////////////////////

  // LAr Member Functions

  float L1CaloOfflineTriggerTowerTools::LArNonNominalHV(const std::vector<const CaloCell*> &cells) const{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey, ctx);
    SG::ReadCondHandle<ILArHVScaleCorr> scaleCorr (m_scaleCorrKey, ctx);
    SG::ReadCondHandle<ILArHVScaleCorr> onlineScaleCorr (m_onlineScaleCorrKey, ctx);
    float nonNominal(0.0);
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      nonNominal += this->LArNonNominalHV( (*i), scaleCorr.cptr(), onlineScaleCorr.cptr(), cabling.cptr() );
    }
    return nonNominal;
  }

  float L1CaloOfflineTriggerTowerTools::LArNonNominalHV(const CaloCell* cell,
                                                        const ILArHVScaleCorr* scaleCorr,
                                                        const ILArHVScaleCorr* onlineScaleCorr,
                                                        const LArOnOffIdMapping* cabling) const{
    float nonNominal(0.0);
    HWIdentifier hwid=cabling->createSignalChannelID(cell->ID());
    float corr = scaleCorr->HVScaleCorr( hwid ) * onlineScaleCorr->HVScaleCorr( hwid );
    if(corr > m_LArHVNonNomPara){
      nonNominal = 1.0;
    }
    return nonNominal;
  }

  float L1CaloOfflineTriggerTowerTools::LArHVScale(const std::vector<const CaloCell*> &cells) const{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey, ctx);
    SG::ReadCondHandle<ILArHVScaleCorr> scaleCorr (m_scaleCorrKey, ctx);
    SG::ReadCondHandle<ILArHVScaleCorr> onlineScaleCorr (m_onlineScaleCorrKey, ctx);

    float scale(0.0),count(0.0);
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      scale += this->LArHVScale( (*i), scaleCorr.cptr(), onlineScaleCorr.cptr(), cabling.cptr() );
      count++;
    }
    if(count < 1.0){return 0.0;}
    return scale / count;
  }

  float L1CaloOfflineTriggerTowerTools::LArHVScale(const CaloCell* cell,
                                                   const ILArHVScaleCorr* scaleCorr,
                                                   const ILArHVScaleCorr* onlineScaleCorr,
                                                   const LArOnOffIdMapping* cabling) const
  {
    HWIdentifier hwid=cabling->createSignalChannelID(cell->ID());
    return scaleCorr->HVScaleCorr( hwid ) * onlineScaleCorr->HVScaleCorr( hwid );
  }


  float L1CaloOfflineTriggerTowerTools::LArCaloQuality(const std::vector<const CaloCell*> &cells) const{
    float nom(0.), denom(0.);
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      if((*i)->provenance() & 0x2000) {
        nom += ((*i)->e() * (*i)->quality());
        denom += (*i)->e();
      }
    }

    if(denom != 0.) {
      nom /= denom;
    }
    else{
      nom = -1e6; // error value
    }
    return nom;
  }

  // Tile Member functions

  float L1CaloOfflineTriggerTowerTools::TileNonNominal(const std::vector<const CaloCell*> &cells) const{
    float nonNominal(0.0);
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      nonNominal += this->TileNonNominal( (*i) );
    }
    return nonNominal;
  }

  float L1CaloOfflineTriggerTowerTools::TileNonNominal(const CaloCell* cell) const{

    // There are either 1 or 2 PMTs per Calo Cell
    // A PMT may have Half Gain L1 or No Gain L1
    //
    // Assign a score as follows.
    //
    // Case (a) : 2 PMTs in the Cell
    // Half gain counts as 0.25
    // No gain counts as 0.5
    // So if a cells has no gain in PMT1 and half gain in PMT2 it's score is 0.75
    //
    // Case (b) : 1 PMT in the Cell
    // Half gain counts as 0.5
    // No Gain counts as 1.0


    // Cast the CaloCell as a TileCell
    const TileCell *tileCell = dynamic_cast<const TileCell*>(cell);
    if (!tileCell) return 0.0;

    // Get IdentifierHash for both PMTs
    const CaloDetDescrElement * caloDDE = tileCell->caloDDE();
    IdentifierHash hash1 = caloDDE->onl1();
    IdentifierHash hash2 = caloDDE->onl2();

    // Values for PMT 1 and 2
    int gain1(-9),gain2(-9);
    float score(0.0),multiplier(1.0);
    bool adc1_noGainL1(false),adc1_halfGainL1(false);
    bool pmt2Valid(false),adc2_noGainL1(false),adc2_halfGainL1(false);

    // PMT 1
    gain1 = tileCell->gain1();
    HWIdentifier adc_id1 = m_tileHWID->adc_id(hash1,(gain1<0)?1:gain1);
    const TileBchStatus badChan1 = m_tileBadChanTool->getAdcStatus(adc_id1);
    adc1_noGainL1 = badChan1.isNoGainL1();
    adc1_halfGainL1 = badChan1.isHalfGainL1();

    // gap/crack scintillators only have one PMT per cell
    // PMT 2, if valid
    if(hash2 != TileHWID::NOT_VALID_HASH){
      pmt2Valid = true;
      gain2 = tileCell->gain2();
      HWIdentifier adc_id2 = m_tileHWID->adc_id(hash2,(gain2<0)?1:gain2);
      const TileBchStatus badChan2 = m_tileBadChanTool->getAdcStatus(adc_id2);
      adc2_noGainL1 = badChan2.isNoGainL1();
      adc2_halfGainL1 = badChan2.isHalfGainL1();
    }

    // Calculate the TileCell score

    // Check that a PMT can't contribute more than 0.5
    // Should not be the case, but assumptions cause bugs.....
    if(adc1_noGainL1 == true && adc1_halfGainL1 == true){adc1_halfGainL1 = false;}
    if(adc2_noGainL1 == true && adc2_halfGainL1 == true){adc2_halfGainL1 = false;}

    if(pmt2Valid == false){multiplier = 2.0;}
    if(adc1_halfGainL1 == true){score += 0.25;}
    if(adc1_noGainL1 == true){score += 0.5;}
    if(adc2_halfGainL1 == true){score += 0.25;}
    if(adc2_noGainL1 == true){score += 0.5;}

    return score * multiplier;
  }

  float L1CaloOfflineTriggerTowerTools::TileCaloQuality(const std::vector<const CaloCell*> &cells) const{
    float nom(0.), denom(0.);
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      const TileCell *tileCell = dynamic_cast<const TileCell*>( (*i) );

      if(tileCell && tileCell->provenance() & 0x8080) {
        nom += tileCell->e() * std::max(tileCell->qual1(), tileCell->qual2());
        denom += tileCell->e();
      }
    }

    if(denom != 0.) {
      nom /= denom;
    }
    else {
      nom = -1e6; // error value
    }
    return nom;
  }

  // EM

  float L1CaloOfflineTriggerTowerTools::emCaloQuality(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    return this->LArCaloQuality(cells);
  }

  float L1CaloOfflineTriggerTowerTools::emNCellsNonNominal(const TriggerTower* tt) const{
    if(m_isLArHVCorrToolSet == false){return -9.9;}
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    return this->LArNonNominalHV( cells );
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::emNCellsNonNominalByLayer(const TriggerTower* tt) const{
    std::vector<float> nNonNomHV;
    if(m_isLArHVCorrToolSet == false){return nNonNomHV;}

    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      nNonNomHV.push_back( this->LArNonNominalHV( (*i) ) );
    }
    return nNonNomHV;
  }

  std::vector<std::vector<float> > L1CaloOfflineTriggerTowerTools::emNCellsNonNominalByReceiverByLayer(const TriggerTower* tt) const{
    std::vector<std::vector<float> > nNonNomHV;
    if(m_isLArHVCorrToolSet == false){return nNonNomHV;}
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    std::vector<float> v1;
    std::vector<float> v2;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortEMCrackCells((*i));
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back(this->LArNonNominalHV((*j)));
	  else                      v2.push_back(this->LArNonNominalHV((*j)));
        }
      } else if (rx.size() == 1) v1.push_back(this->LArNonNominalHV((*i)));
    }
    nNonNomHV.push_back(v1);
    if (rx.size() == 2) nNonNomHV.push_back(v2);
    return nNonNomHV;
  }

  float L1CaloOfflineTriggerTowerTools::emNonNominalMeanScale(const TriggerTower* tt) const{
    if(m_isLArHVCorrToolSet == false){return -9.9;}
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    return this->LArHVScale( cells );
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::emNonNominalMeanScaleByLayer(const TriggerTower* tt) const{
    std::vector<float> NonNomHV;
    if(m_isLArHVCorrToolSet == false){return NonNomHV;}
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      NonNomHV.push_back( this->LArHVScale( (*i ) ) );
    }
    return NonNomHV;
  }

  
  std::vector<float> L1CaloOfflineTriggerTowerTools::emNonNominalMeanScaleByReceiver(const TriggerTower* tt) const{

    std::vector<float> output;
    if(m_isLArHVCorrToolSet == false){return output;}
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(rx.size() == 2){
      std::vector<std::vector<const CaloCell*> > vvcells = this->sortEMCrackCells(cells);
      for(Itr_vvCaloCells i=vvcells.begin();i!=vvcells.end();++i){
        output.push_back( this->LArHVScale( (*i) ) );
      }
    }

    if(rx.size() == 1){
      output.push_back( this->LArHVScale( cells ) );
    }

    return output;
  }

  std::vector<std::vector<float> > L1CaloOfflineTriggerTowerTools::emNonNominalMeanScaleByReceiverByLayer(const TriggerTower* tt) const{
    std::vector<std::vector<float> > output;
    if(m_isLArHVCorrToolSet == false){return output;}
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    std::vector<float> v1;
    std::vector<float> v2;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortEMCrackCells((*i));
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back(this->LArHVScale((*j)));
	  else                      v2.push_back(this->LArHVScale((*j)));
        }
      } else if (rx.size() == 1) v1.push_back(this->LArHVScale((*i)));
    }
    output.push_back(v1);
    if (rx.size() == 2) output.push_back(v2);
    return output;
  }
    

  // Had

  float L1CaloOfflineTriggerTowerTools::hadCaloQuality(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    bool isTile = m_lvl1Helper->is_tile(Id);

    if(isTile == true){return this->TileCaloQuality(cells);}
    if(isTile == false){return this->LArCaloQuality(cells);}
    return -9.9;
  }


  float L1CaloOfflineTriggerTowerTools::hadNCellsNonNominal(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    bool isTile = m_lvl1Helper->is_tile(Id);
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(isTile == false && m_isLArHVCorrToolSet == false){return -9.9;}

    if(isTile == false && m_isLArHVCorrToolSet == true){
      return this->LArNonNominalHV( cells );
    }

    if(isTile == true){
      return this->TileNonNominal( cells );
    }
    return -9.9;
  }


  std::vector<float> L1CaloOfflineTriggerTowerTools::hadNCellsNonNominalByLayer(const TriggerTower* tt) const{
    std::vector<float> nNonNomHV;
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    bool isTile = m_lvl1Helper->is_tile(Id);
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    if(isTile == false && m_isLArHVCorrToolSet == false){return nNonNomHV;}
    if(isTile == false && m_isLArHVCorrToolSet == true){
      for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
        nNonNomHV.push_back( this->LArNonNominalHV( (*i) ) );
      }
    }

    if(isTile == true){
      for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
        nNonNomHV.push_back( this->TileNonNominal( (*i) ) );
      }
    }

    return nNonNomHV;

  }

  std::vector<std::vector<float> > L1CaloOfflineTriggerTowerTools::hadNCellsNonNominalByReceiverByLayer(const TriggerTower* tt) const{
    std::vector<std::vector<float> > nNonNomHV;
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    bool isTile = m_lvl1Helper->is_tile(Id);
    if(isTile == false && m_isLArHVCorrToolSet == false){return nNonNomHV;}
    std::vector<unsigned int> rx = this->hadRxId(tt);
    std::vector<float> v1;
    std::vector<float> v2;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortFCAL23Cells((*i),rx);
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back( (isTile == false) ? this->LArNonNominalHV((*j))
	                                                            : this->TileNonNominal((*j)));
	  else                      v2.push_back( (isTile == false) ? this->LArNonNominalHV((*j))
	                                                            : this->TileNonNominal((*j)));
        }
      } else if (rx.size() == 1) v1.push_back( (isTile == false) ? this->LArNonNominalHV((*i))
                                                                 : this->TileNonNominal((*i)));
    }
    nNonNomHV.push_back(v1);
    if (rx.size() == 2) nNonNomHV.push_back(v2);
    return nNonNomHV;
  }

  float L1CaloOfflineTriggerTowerTools::hadNonNominalMeanScale(const TriggerTower* tt) const{

    Identifier Id = this->hadID(tt->eta(),tt->phi());
    bool isTile = m_lvl1Helper->is_tile(Id);
    if(isTile == true){return -9.9;}

    if(m_isLArHVCorrToolSet == false){return -9.9;}
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);
    return this->LArHVScale( cells );

  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::hadNonNominalMeanScaleByLayer(const TriggerTower* tt) const{
    std::vector<float> NonNomHV;
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    bool isTile = m_lvl1Helper->is_tile(Id);
    if(isTile == true){return NonNomHV;}

    if(m_isLArHVCorrToolSet == false){return NonNomHV;}
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);

    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      NonNomHV.push_back( this->LArHVScale( (*i ) ) );
    }
    return NonNomHV;
  }

  std::vector<float> L1CaloOfflineTriggerTowerTools::hadNonNominalMeanScaleByReceiver(const TriggerTower* tt) const{

    std::vector<float> output;
    if(m_isLArHVCorrToolSet == false){return output;}
    std::vector<unsigned int> rx = this->hadRxId(tt);
    Identifier Id = this->hadID(tt->eta(),tt->phi());

    bool isTile = m_lvl1Helper->is_tile(Id);
    if(isTile == true){return output;}
    
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);

    if(rx.size() == 2){
      std::vector<std::vector<const CaloCell*> > vvcells = this->sortFCAL23Cells(cells,rx);
      for(Itr_vvCaloCells i=vvcells.begin();i!=vvcells.end();++i){
        output.push_back( this->LArHVScale( (*i) ) );
      }
    }

    if(rx.size() == 1){
      output.push_back( this->LArHVScale( cells ) );
    }

    return output;

  }  

  std::vector<std::vector<float> > L1CaloOfflineTriggerTowerTools::hadNonNominalMeanScaleByReceiverByLayer(const TriggerTower* tt) const{
    std::vector<std::vector<float> > output;
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    bool isTile = m_lvl1Helper->is_tile(Id);
    if(isTile == true || m_isLArHVCorrToolSet == false){return output;}
    std::vector<unsigned int> rx = this->hadRxId(tt);
    std::vector<float> v1;
    std::vector<float> v2;
    std::vector<std::vector<const CaloCell*> > cells = m_cells2tt->caloCellsByLayer(Id);
    for(Itr_vvCaloCells i=cells.begin();i!=cells.end();++i){
      if ((*i).empty()) continue;
      if (rx.size() == 2) {
        std::vector<std::vector<const CaloCell*> > cellsRx = this->sortFCAL23Cells((*i),rx);
        for(Itr_vvCaloCells j=cellsRx.begin();j!=cellsRx.end();++j){
          if ((*j).empty()) continue;
          if (j == cellsRx.begin()) v1.push_back(this->LArHVScale((*j)));
	  else                      v2.push_back(this->LArHVScale((*j)));
        }
      } else if (rx.size() == 1) v1.push_back(this->LArHVScale((*i)));
    }
    output.push_back(v1);
    if (rx.size() == 2) output.push_back(v2);
    return output;
  }
  
  
  //////////////////////////////////////////////////////
  //  Phase 1 SuperCells                              //
  //////////////////////////////////////////////////////
  
  // EM
  std::vector<int> L1CaloOfflineTriggerTowerTools::emSuperCellIdentifiers(const TriggerTower* tt) const{
    Identifier Id = this->emID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);   
    return this->SuperCellIdentifiers( cells );
  }
  
  std::vector<int> L1CaloOfflineTriggerTowerTools::hadSuperCellIdentifiers(const TriggerTower* tt) const{
    Identifier Id = this->hadID(tt->eta(),tt->phi());
    std::vector<const CaloCell*> cells = m_cells2tt->caloCells(Id);   
    return this->SuperCellIdentifiers( cells );
  }  
  
  std::vector<int> L1CaloOfflineTriggerTowerTools::SuperCellIdentifiers(const std::vector<const CaloCell*> &cells) const{
    std::vector<int> output;
    std::map<int, std::vector<const CaloCell*> > sc_ids;
    for(Itr_vCaloCells i=cells.begin();i!=cells.end();++i){
      const CaloCell* cell = (*i);
      Identifier sCellID  = m_scidtool->offlineToSuperCellID ( cell->ID() );      
      sc_ids[sCellID.get_identifier32().get_compact()].push_back( cell ); 
    }
    for(std::map< int, std::vector<const CaloCell*> >::const_iterator i=sc_ids.begin();i!=sc_ids.end();++i){
      output.push_back( (*i).first );
    }    
    return output;
  }
  
  //////////////////////////////////////////////////////
  //           Database Attributes                    //
  //////////////////////////////////////////////////////
  
  // EM

  const coral::AttributeList*  L1CaloOfflineTriggerTowerTools::emDbAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const{
    unsigned int coolId = this->emCoolChannelId(tt);
    typedef CondAttrListCollection::const_iterator Itr_db;
    const coral::AttributeList* attrList = 0;
    for(Itr_db i=dbAttrList->begin();i!=dbAttrList->end();++i){
      if(i->first == coolId){
        attrList = &(i->second);
        break;
      }
    }
    return attrList;
  }


  std::vector<const coral::AttributeList*> L1CaloOfflineTriggerTowerTools::emDbRxGainsAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const {

    std::vector<L1CaloRxCoolChannelId> rx = this->emReceivers(tt);
    std::vector<const coral::AttributeList*> v_attr;
    typedef CondAttrListCollection::const_iterator Itr_db;
    typedef std::vector<L1CaloRxCoolChannelId>::const_iterator Itr_Id;

    for(Itr_Id a=rx.begin();a!=rx.end();++a){
      for(Itr_db i=dbAttrList->begin();i!=dbAttrList->end();++i){
        if(i->first == (*a).id()){
          v_attr.push_back( &(i->second) );
        }
      }
    }
    return v_attr;
  }

  // Had

  const coral::AttributeList*  L1CaloOfflineTriggerTowerTools::hadDbAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const{
    unsigned int coolId = this->hadCoolChannelId(tt);
    typedef CondAttrListCollection::const_iterator Itr_db;
    const coral::AttributeList* attrList = 0;
    for(Itr_db i=dbAttrList->begin();i!=dbAttrList->end();++i){
      if(i->first == coolId){
        attrList = &(i->second);
        break;
      }
    }
    return attrList;
  }

  std::vector<const coral::AttributeList*> L1CaloOfflineTriggerTowerTools::hadDbRxGainsAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const{

    std::vector<L1CaloRxCoolChannelId> rx = this->hadReceivers(tt);
    std::vector<const coral::AttributeList*> v_attr;
    typedef CondAttrListCollection::const_iterator Itr_db;
    typedef std::vector<L1CaloRxCoolChannelId>::const_iterator Itr_Id;

    for(Itr_Id a=rx.begin();a!=rx.end();++a){
      for(Itr_db i=dbAttrList->begin();i!=dbAttrList->end();++i){
        if(i->first == (*a).id()){
          v_attr.push_back( &(i->second) );
        }
      }
    }
    return v_attr;
  }



  //////////////////////////////////////////////////////
  //       Database Quantities                        //
  //////////////////////////////////////////////////////


  float L1CaloOfflineTriggerTowerTools::RxGain(const coral::AttributeList* attrList) const{
    float value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "gain"){
        value = (float) (*attrList)[attrNum].data<float>();
        break;
      }
    }
    return value;
  }

  unsigned int L1CaloOfflineTriggerTowerTools::RxStatus(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "status"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }


  unsigned int L1CaloOfflineTriggerTowerTools::ModuleId(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "ModuleId"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }

  unsigned int L1CaloOfflineTriggerTowerTools::ErrorCode(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "ErrorCode"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprDacScanResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprDacScanResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprPedestalRunResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprPedestalRunResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprNoiseRunResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprNoiseRunResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprTimingResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprTimingResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprSatBcidResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprSatBcidResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprFirFilterResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprFirFilterResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long L1CaloOfflineTriggerTowerTools::PprLutValuesResultsTimeStamp(const coral::AttributeList* attrList) const{
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PprLutValuesResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  double L1CaloOfflineTriggerTowerTools::DacOffset(const coral::AttributeList* attrList) const{
    double value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "DacOffset"){
        value = (double) (*attrList)[attrNum].data<double>();
        break;
      }
    }
    return value;
  }

  double L1CaloOfflineTriggerTowerTools::DacSlope(const coral::AttributeList* attrList) const{
    double value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "DacSlope"){
        value = (double) (*attrList)[attrNum].data<double>();
        break;
      }
    }
    return value;
  }

  double L1CaloOfflineTriggerTowerTools::PedMean(const coral::AttributeList* attrList) const{
    double value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PedMean"){
        value = (double) (*attrList)[attrNum].data<double>();
        break;
      }
    }
    return value;
  }

  unsigned int L1CaloOfflineTriggerTowerTools::PedValue(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "PedValue"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }

  unsigned int L1CaloOfflineTriggerTowerTools::FullDelayData(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FullDelayData"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::SyncDelayBcid(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "SyncDelayBcid"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }


  unsigned short int L1CaloOfflineTriggerTowerTools::InBcidNegedge(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "InBcidNegedge"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }


  unsigned short int L1CaloOfflineTriggerTowerTools::ExtBcidThreshold(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "ExtBcidThreshold"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::SatBcidThreshLow(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "SatBcidThreshLow"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::SatBcidThreshHigh(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "SatBcidThreshHigh"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::SatBcidLevel(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "SatBcidLevel"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::BcidEnergyRangeLow(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "BcidEnergyRangeLow"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::BcidEnergyRangeHigh(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "BcidEnergyRangeHigh"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  unsigned short int L1CaloOfflineTriggerTowerTools::FirStartBit(const coral::AttributeList* attrList) const{
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FirStartBit"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;
  }

  short L1CaloOfflineTriggerTowerTools::FirCoeff1(const coral::AttributeList* attrList) const{
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FirCoeff1"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;
  }

  short L1CaloOfflineTriggerTowerTools::FirCoeff2(const coral::AttributeList* attrList) const{
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FirCoeff2"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;
  }

  short L1CaloOfflineTriggerTowerTools::FirCoeff3(const coral::AttributeList* attrList) const{
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FirCoeff3"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;
  }

  short L1CaloOfflineTriggerTowerTools::FirCoeff4(const coral::AttributeList* attrList) const{
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FirCoeff4"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;
  }

  short L1CaloOfflineTriggerTowerTools::FirCoeff5(const coral::AttributeList* attrList) const{
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "FirCoeff5"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;
  }

  unsigned short L1CaloOfflineTriggerTowerTools::LutStrategy(const coral::AttributeList* attrList) const{
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "LutStrategy"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short>();
        break;
      }
    }
    return value;
  }

  unsigned short L1CaloOfflineTriggerTowerTools::LutOffset(const coral::AttributeList* attrList) const{
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "LutOffset"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short>();
        break;
      }
    }
    return value;
  }

  unsigned short L1CaloOfflineTriggerTowerTools::LutSlope(const coral::AttributeList* attrList) const{
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "LutSlope"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short >();
        break;
      }
    }
    return value;
  }

  unsigned short L1CaloOfflineTriggerTowerTools::LutNoiseCut(const coral::AttributeList* attrList) const{
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "LutNoiseCut"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short>();
        break;
      }
    }
    return value;
  }

  // Disabled Tower in >= 2011 data
  // Should use the DB folder /TRIGGER/L1Calo/V1/Conditions/DisabledTowers
  unsigned int L1CaloOfflineTriggerTowerTools::DisabledTower(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "disabledBits"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }

  // Dead Channels in 2010 data
  // Should use the DB folder /TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels
  unsigned int L1CaloOfflineTriggerTowerTools::DeadChannel(const coral::AttributeList* attrList) const{
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();attrNum++){
      if((*attrList)[attrNum].specification().name() == "ErrorCode"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }



  //////////////////////////////////////////////////////
  //                Internals                         //
  //////////////////////////////////////////////////////


  StatusCode L1CaloOfflineTriggerTowerTools::initialize(){
    ATH_CHECK( detStore()->retrieve(m_caloMgr) );

    m_ttOnlineIdHelper = m_caloMgr->getTTOnlineID();
    if(!m_ttOnlineIdHelper){ATH_MSG_ERROR("Could not access TTOnlineId helper");return StatusCode::FAILURE;}

    m_lvl1Helper = m_caloMgr->getLVL1_ID();
    if(!m_lvl1Helper){ATH_MSG_ERROR("Could not access CaloLVL1_ID helper");return StatusCode::FAILURE;}

    ATH_CHECK( detStore()->retrieve(m_tileHWID) );
    ATH_CHECK( m_tileBadChanTool.retrieve() );
    ATH_CHECK( m_rxMapTool.retrieve() );
    ATH_CHECK( detStore()->retrieve(m_larOnlineID,"LArOnlineID") );
    ATH_CHECK( m_l1CaloTTIdTools.retrieve() );
    ATH_CHECK( m_cells2tt.retrieve() );

    ATH_CHECK( detStore()->retrieve (m_caloMgr, "CaloIdManager") );
    ATH_CHECK( m_scidtool.retrieve() );
    
    //Retrieve cabling & tt services
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    IToolSvc* toolSvc = nullptr;

    ATH_CHECK( svcLoc->service( "ToolSvc",toolSvc  ) );
    ATH_CHECK( toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc) );

    // DB Access
    ATH_CHECK( service("L1CaloCondSvc", m_l1CondSvc) );

    ATH_CHECK( m_onlineScaleCorrKey.initialize() );

    ATH_MSG_INFO("L1Calo L1CaloOfflineTriggerTowerTools initialize() success!");
    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloOfflineTriggerTowerTools::finalize(){
    return StatusCode::SUCCESS;
  }

  void L1CaloOfflineTriggerTowerTools::LArHV(const SG::ReadCondHandleKey<ILArHVScaleCorr>& scaleCorrKey,
                                             const SG::ReadCondHandleKey<LArOnOffIdMapping>& cablingKey)
  {
    m_scaleCorrKey = scaleCorrKey;
    m_cablingKey = cablingKey;
    m_isLArHVCorrToolSet = true;
  }

  int L1CaloOfflineTriggerTowerTools::pos_neg_z(const double eta) const{
    return m_l1CaloTTIdTools->pos_neg_z(eta);
  }

  int L1CaloOfflineTriggerTowerTools::region(const double eta) const{
    return m_l1CaloTTIdTools->regionIndex(eta);
  }

  int L1CaloOfflineTriggerTowerTools::ieta(const double eta) const{
    return m_l1CaloTTIdTools->etaIndex(eta);
  }

  int L1CaloOfflineTriggerTowerTools::iphi(const double eta,const double phi) const{
    return m_l1CaloTTIdTools->phiIndex(eta,phi);
  }
  
  Identifier L1CaloOfflineTriggerTowerTools::ID(const double eta,const double phi,int layer) const{
    return m_lvl1Helper->tower_id(this->pos_neg_z(eta),layer,this->region(eta),this->ieta(eta),this->iphi(eta,phi));
  }

  Identifier L1CaloOfflineTriggerTowerTools::emID(const double eta,const double phi) const{
    return m_lvl1Helper->tower_id(this->pos_neg_z(eta),0,this->region(eta),this->ieta(eta),this->iphi(eta,phi));
  }

  Identifier L1CaloOfflineTriggerTowerTools::hadID(const double eta,const double phi) const{
    return m_lvl1Helper->tower_id(this->pos_neg_z(eta),1,this->region(eta),this->ieta(eta),this->iphi(eta,phi));
  }

  void L1CaloOfflineTriggerTowerTools::caloCells(const CaloCellContainer* cells){
    m_cells2tt->initCaloCellsTriggerTowers(*cells);
  }

} // end of namespace
