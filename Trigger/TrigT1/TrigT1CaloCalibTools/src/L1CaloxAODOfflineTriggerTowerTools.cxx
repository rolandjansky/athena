/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

// Like it says above, please chop up this code until it does what you want !

#include "TrigT1CaloCalibTools/L1CaloxAODOfflineTriggerTowerTools.h"
#include "StoreGate/ReadHandle.h"

#include <algorithm> // for std::transform
#include <iterator> // for std::back_inserter
#include <utility> // for std::move

namespace LVL1{
  
  L1CaloxAODOfflineTriggerTowerTools::L1CaloxAODOfflineTriggerTowerTools( const std::string& name ) :
    asg::AsgTool( name ),
    m_cells2tt("LVL1::L1CaloCells2TriggerTowers/L1CaloCells2TriggerTowers"),
    m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
    m_rxMapTool("LVL1::L1CaloFcal23Cells2RxMappingTool/L1CaloFcal23Cells2RxMappingTool"),
    m_caloMgr(nullptr),
    m_lvl1Helper(nullptr),
    m_ttSvc(nullptr),
    m_dbPpmChanCalib(nullptr)
  {      
  }
    
  StatusCode 
  L1CaloxAODOfflineTriggerTowerTools::initialize()
  {
    ATH_MSG_INFO("initialize LVL1::L1CaloxAODOfflineTriggerTowerTools");
    
    // Get the L1CaloCells2TriggerTowers Tool 
    CHECK( m_cells2tt.retrieve() );    
    
    // Get the L1CaloTTIdTools Tool
    CHECK( m_l1CaloTTIdTools.retrieve() );
    
    // Get the Fcal23 mapping tool
    CHECK( m_rxMapTool.retrieve() );
    
    CHECK( detStore()->retrieve ( m_caloMgr, "CaloIdManager") );
    m_lvl1Helper = m_caloMgr->getLVL1_ID();
    
    //Retrieve cabling & tt services
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    IToolSvc* toolSvc;

    CHECK( svcLoc->service( "ToolSvc",toolSvc  ) );
    CHECK( toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc) );

    CHECK( m_caloCellContainerKey.initialize() );

    // Return gracefully:    
    return StatusCode::SUCCESS; 
  }
  
  StatusCode 
  L1CaloxAODOfflineTriggerTowerTools::finalize()
  {
    ATH_MSG_INFO("finalize LVL1::L1CaloxAODOfflineTriggerTowerTools");    
    // Return gracefully:    
    return StatusCode::SUCCESS;    
  }
  
  /// Calo Cells into maps for L1Calo use
  StatusCode                                 
  L1CaloxAODOfflineTriggerTowerTools::initCaloCells()
  {
    SG::ReadHandle<CaloCellContainer> cells (m_caloCellContainerKey);
    m_cells2tt->initCaloCellsTriggerTowers( *cells );
    
    // Return gracefully:
    return StatusCode::SUCCESS;      
  }
  
  std::vector<L1CaloRxCoolChannelId>         
  L1CaloxAODOfflineTriggerTowerTools::receivers( const xAOD::TriggerTower& tt ) const
  {
    std::vector<L1CaloRxCoolChannelId> rx = m_ttSvc->cnvCoolChannelIdToRxId( tt.coolId() );
    std::vector<L1CaloRxCoolChannelId> output;
    
    // EM
    if (tt.sampling() == 0) {
      // Sort EM Crack recievers. End cap first, barrel second
      if (rx.size() == 3) {

        // For the EM crack region, there are 3 recievers.
        // 1 Rx have Crate 0 or 1  - correspond to the barrel before the Octopus cable magic - ignore
        // 2 Rx have Crate 2 or 3  - correspond to the Rx we want:
        //      1 Rx has inputConn() == 0 - EMEC receiver
        //      1 Rx has inputConn() == 2 - Barrel receiver
        
        std::vector<L1CaloRxCoolChannelId> rx2;
        for (auto i : rx) {
          if (i.crate() == 2 || i.crate() == 3) {
            rx2.push_back( i );
          }
        }
        
        if (rx2.size() == 2) {

          unsigned int inputA = rx2.at(0).inputConn();
          unsigned int inputB = rx2.at(1).inputConn();

          // End cap first, barrel second
          if (inputA == 2 && inputB == 0) {
            output.push_back(rx2.at(1));
            output.push_back(rx2.at(0));
          }
          if (inputA == 0 && inputB == 2) {
            output.push_back(rx2.at(0));
            output.push_back(rx2.at(1));
          }
        }
      }

      if (rx.size() == 1) {
        output.push_back( rx.at(0) );
      }        
      
    } // end EM
    
    // Had
    if (tt.sampling() == 1) {
      // Sort FCAL 23 recievers. Always have -ve eta first and +ve eta second
      if (rx.size() == 2) {
        // Odd receiver is lowest |eta|
        unsigned int inputPairA = rx.at(0).inputPair();
        unsigned int inputPairB = rx.at(1).inputPair();

        bool oddA(false),oddB(false);
        if(inputPairA % 2 != 0){oddA = true;}
        if(inputPairB % 2 != 0){oddB = true;}

        if (oddA && !oddB) {
          output.push_back(rx.at(0));
          output.push_back(rx.at(1));
        }
        if (!oddA && oddB) {
          output.push_back(rx.at(1));
          output.push_back(rx.at(0));
        }        
      }
      if (rx.size() == 1) {
        output.push_back( rx.at(0) );
      }      
    } // end Had
    
    return output;
  }
  
  std::vector<unsigned int>                  
  L1CaloxAODOfflineTriggerTowerTools::receiversId( const xAOD::TriggerTower& tt ) const
  {
    std::vector<L1CaloRxCoolChannelId> rx = receivers( tt );
    std::vector<unsigned int> output;
    for (auto i : rx) {
      output.push_back( i.id() );
    }
    return output;
  }
  
  
  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::isTile( const xAOD::TriggerTower& tt )  const
  {
    Identifier id = towerID( tt );
    if (m_lvl1Helper->is_tile( id )) {
      return 1;
    }
    return 0;
  }


  std::vector<const CaloCell*>               
  L1CaloxAODOfflineTriggerTowerTools::getCaloCells( const xAOD::TriggerTower& tt )  const
  {
    Identifier id = towerID( tt );
    return m_cells2tt->caloCells( id );
  }

  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::nCaloCells( const xAOD::TriggerTower& tt )  const
  {
    return getCaloCells( tt ).size();    
  }

  std::vector<unsigned int>                  
  L1CaloxAODOfflineTriggerTowerTools::nCaloCellsByLayer( const xAOD::TriggerTower& tt )  const
  {
    std::vector<unsigned int> output;
    Identifier id = towerID( tt );
    std::vector<std::vector<const CaloCell*>> cells = m_cells2tt->caloCellsByLayer( id );
    for (auto i : cells) {
      output.push_back( i.size() );
    }
    return output;      
  }

//   std::vector<unsigned int>                  
//   L1CaloxAODOfflineTriggerTowerTools::nCaloCellsByReceiver( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<unsigned int> output;
//     return output;      
//   }
//  
//   std::vector<unsigned int>                  
//   L1CaloxAODOfflineTriggerTowerTools::nCaloCellsByReceiverByLayer( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<unsigned int> output;
//     return output;     
//   }
 

  float                                      
  L1CaloxAODOfflineTriggerTowerTools::caloCellsEnergy( const xAOD::TriggerTower& tt )  const
  {
    Identifier id = towerID( tt );
    return m_cells2tt->energy( id );
  }
  
  float                                      
  L1CaloxAODOfflineTriggerTowerTools::caloCellsET( const xAOD::TriggerTower& tt )  const
  {
    Identifier id = towerID( tt );
    return m_cells2tt->et( id );
  }  

  std::vector<float>                         
  L1CaloxAODOfflineTriggerTowerTools::caloCellsEnergyByLayer( const xAOD::TriggerTower& tt )  const
  {
    std::vector<float> output;
    Identifier id = towerID( tt );
    std::vector<std::vector<const CaloCell*>> cells = m_cells2tt->caloCellsByLayer( id );
    for (auto i : cells) {
      output.push_back( m_cells2tt->energy( i ) );
    }
    return output;     
  }
  
  std::vector<float>                         
  L1CaloxAODOfflineTriggerTowerTools::caloCellsETByLayer( const xAOD::TriggerTower& tt )  const
  {
    std::vector<float> output;
    Identifier id = towerID( tt );
    std::vector<std::vector<const CaloCell*>> cells = m_cells2tt->caloCellsByLayer( id );
    for (auto i : cells) {
      output.push_back( m_cells2tt->et( i ) );
    }
    return output;     
  }

  std::vector<std::vector<const CaloCell*>>
  L1CaloxAODOfflineTriggerTowerTools::caloCellsByReceiver( const xAOD::TriggerTower& tt ) const
  {
    std::vector<std::vector<const CaloCell*>> output;
    const auto& rx = receivers(tt);
    if (rx.size() == 1) {
      output = { getCaloCells(tt) };
    } else if(rx.size() == 2) {
      output = ((tt.layer() == 0) ? sortEMCrackCells(getCaloCells(tt)) : sortFCAL23Cells(getCaloCells(tt), rx));
    } else {
      // shouldn't happen ... non-critical code, warning is sufficient
      ATH_MSG_WARNING("Found TT with " << rx.size() << " receivers!");
    }
    return output;
  }

  std::vector<float>                         
  L1CaloxAODOfflineTriggerTowerTools::caloCellsEnergyByReceiver( const xAOD::TriggerTower& tt )  const
  {
    using std::begin;
    using std::end;
    using std::back_inserter;

    std::vector<float> energyByReceiver;
    const auto& cellsByReceiver = caloCellsByReceiver(tt);
    std::transform(begin(cellsByReceiver), end(cellsByReceiver),
                   back_inserter(energyByReceiver),
                   [this](const std::vector<const CaloCell*>& c) -> float {
                     return m_cells2tt->energy( c );
                   });

    return energyByReceiver;     
  }

  std::vector<float>                         
  L1CaloxAODOfflineTriggerTowerTools::caloCellsETByReceiver( const xAOD::TriggerTower& tt )  const
  {
    using std::begin;
    using std::end;
    using std::back_inserter;

    std::vector<float> etByReceiver;
    const auto& cellsByReceiver = caloCellsByReceiver(tt);
    std::transform(begin(cellsByReceiver), end(cellsByReceiver),
                   back_inserter(etByReceiver),
                   [this](const std::vector<const CaloCell*>& c) -> float {
                     return m_cells2tt->et( c );
                   });

    return etByReceiver;     
  }

  std::vector<std::vector<std::vector<const CaloCell*>>>
  L1CaloxAODOfflineTriggerTowerTools::caloCellsByLayerByReceiver( const xAOD::TriggerTower& tt ) const
  {
    using std::begin;
    using std::end;
    using std::back_inserter;
    using std::vector;

    vector<vector<vector<const CaloCell*>>> output;
    const auto& rx = receivers(tt);
    if(rx.size() == 1) {
      const auto& cellsByLayer = m_cells2tt->caloCellsByLayer(towerID(tt));
      std::transform(begin(cellsByLayer), end(cellsByLayer),
                     back_inserter(output),
                     [](vector<const CaloCell*> C)->vector<vector<const CaloCell*>> {
                       return {C};
                     });
    } else if(rx.size() == 2) {
      Identifier id = towerID( tt );
      const auto& cellsByLayer = m_cells2tt->caloCellsByLayer( id );
      for(const auto& cells : cellsByLayer) {
        const auto& cellsByReceiver = ((tt.layer() == 0) ? sortEMCrackCells(cells) : sortFCAL23Cells(cells, rx));
        output.push_back(cellsByReceiver);
      }
    } else {
      ATH_MSG_WARNING("Found TT with " << rx.size() << " receivers!");
    }

    return output;
  }

  std::vector<std::vector<float>>
  L1CaloxAODOfflineTriggerTowerTools::caloCellsEnergyByLayerByReceiver( const xAOD::TriggerTower& tt ) const
  {
    using std::begin;
    using std::end;
    using std::back_inserter;

    std::vector<std::vector<float>> output;
    for(const auto& cellsByReceiver : caloCellsByLayerByReceiver(tt)) {
      std::vector<float> cellsEnergyByReceiver;
      for(const auto& cells : cellsByReceiver) {
        cellsEnergyByReceiver.push_back(m_cells2tt->energy(cells));
      }
      output.push_back(std::move(cellsEnergyByReceiver));
    }

    return output;
  }


  std::vector<std::vector<float>>
  L1CaloxAODOfflineTriggerTowerTools::caloCellsETByLayerByReceiver( const xAOD::TriggerTower& tt ) const
  {
    using std::begin;
    using std::end;
    using std::back_inserter;

    std::vector<std::vector<float>> output;
    for(const auto& cellsByReceiver : caloCellsByLayerByReceiver(tt)) {
      std::vector<float> cellsEnergyByReceiver;
      for(const std::vector<const CaloCell*>& cells : cellsByReceiver) {
        cellsEnergyByReceiver.push_back(m_cells2tt->et(cells));
      }
      output.push_back(std::move(cellsEnergyByReceiver));
    }

    return output;
  }


//   
// 
//   unsigned int                               
//   L1CaloxAODOfflineTriggerTowerTools::badCaloCells( const xAOD::TriggerTower& tt )  const
//   {
//     
//     return 0;    
//   }

  float                                      
  L1CaloxAODOfflineTriggerTowerTools::caloCellsQuality( const xAOD::TriggerTower& tt )  const
  {
    float output(-999.9);
    std::vector<const CaloCell*> cells = getCaloCells( tt );
    if (isTile( tt )) {
      output = TileCaloQuality( cells );
    }
    else {
      output = LArCaloQuality( cells );
    }
    return output;
  }
  
  float 
  L1CaloxAODOfflineTriggerTowerTools::LArCaloQuality( const std::vector<const CaloCell*> &cells ) const
  {
    float nom(0.), denom(0.);
    for (auto i : cells) {
      if ((i)->provenance() & 0x2000) {
        nom += ((i)->e() * (i)->quality());
        denom += (i)->e();
      }
    }

    if (denom != 0.) {
      nom /= denom;
    }
    else {
      nom = -1e6; // error value
    }
    return nom;
  }  
  
  float 
  L1CaloxAODOfflineTriggerTowerTools::TileCaloQuality( const std::vector<const CaloCell*> &cells ) const
  {
    float nom(0.), denom(0.);
    for (auto i : cells) {
      const TileCell *tileCell = dynamic_cast<const TileCell*>( (i) );

      if (tileCell && tileCell->provenance() & 0x8080) {
        nom += tileCell->e() * std::max(tileCell->qual1(), tileCell->qual2());
        denom += tileCell->e();
      }
    }

    if (denom != 0.) {
      nom /= denom;
    }
    else {
      nom = -1e6; // error value
    }
    return nom;
  }  
  
  
// 
//   float                                      
//   L1CaloxAODOfflineTriggerTowerTools::nCaloCellsNonNominal( const xAOD::TriggerTower& tt )  const
//   {
//     return 0;    
//   }
// 
//   std::vector<float>                         
//   L1CaloxAODOfflineTriggerTowerTools::nCaloCellsNonNominalByLayer( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<float> output;
//     return output;    
//   }
// 
//   std::vector<std::vector<float>>            
//   L1CaloxAODOfflineTriggerTowerTools::nCaloCellsNonNominalByReceiverByLayer( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<std::vector<float>> output;
//     return output;    
//   }
// 
//   float                                      
//   L1CaloxAODOfflineTriggerTowerTools::nonNominalMeanScale( const xAOD::TriggerTower& tt )  const
//   {
//     return 0;    
//   }
// 
//   std::vector<float>                         
//   L1CaloxAODOfflineTriggerTowerTools::nonNominalMeanScaleByLayer( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<float> output;
//     return output;    
//   }
// 
//   std::vector<float>                         
//   L1CaloxAODOfflineTriggerTowerTools::nonNominalMeanScaleByReceiver( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<float> output;
//     return output;
//   }
// 
//   std::vector<std::vector<float>>            
//   L1CaloxAODOfflineTriggerTowerTools::nonNominalMeanScaleByReceiverByLayer( const xAOD::TriggerTower& tt )  const
//   {
//     std::vector<std::vector<float>> output;
//     return output;
//   }
  
  // Sort Calo Cells
  std::vector<std::vector<const CaloCell*>>              
  L1CaloxAODOfflineTriggerTowerTools::sortEMCrackCells(const std::vector<const CaloCell*> &cells) const
  {
    std::vector<std::vector<const CaloCell*>> output;
    std::vector<const CaloCell*> emb;
    std::vector<const CaloCell*> emec;

    // End Cap first, Barrel second

    for( auto i : cells ){
      int layer = m_cells2tt->layerNames(i);

      if( layer <= 3){
        emb.push_back( (i) );
      }
      if( layer >= 4){
        emec.push_back( (i) );
      }
    }

    output.push_back(emec);
    output.push_back(emb);
    return output;    
  }
  
  std::vector<std::vector<const CaloCell*>>              
  L1CaloxAODOfflineTriggerTowerTools::sortFCAL23Cells(const std::vector<const CaloCell*> &cells,const std::vector<L1CaloRxCoolChannelId>& rx) const
  {
    // vectors of calo cells for the different receivers
    std::vector<const CaloCell*> cellsA;
    std::vector<const CaloCell*> cellsB;
    // output
    std::vector<std::vector<const CaloCell*>> output;

    // RxID of the different receivers
    unsigned int rxidA = rx.at(0).id();
    unsigned int rxidB = rx.at(1).id();

    // Loop over calo cells and use mapping tool to assign cells to different receivers
    for( auto i : cells ){
      unsigned int someRxId = m_rxMapTool->offlineCell2RxId( (i)->ID().get_identifier32().get_compact() );
      if(someRxId == rxidA){cellsA.push_back(i);}
      if(someRxId == rxidB){cellsB.push_back(i);}
    }
    output.push_back(cellsA);
    output.push_back(cellsB);
    return output;    
  }
   

  //  Database Attributes
  const coral::AttributeList*                
  L1CaloxAODOfflineTriggerTowerTools::DbAttributes( const xAOD::TriggerTower& tt , const CondAttrListCollection* dbAttrList )  const
  {
    typedef CondAttrListCollection::const_iterator Itr_db;
    const coral::AttributeList* attrList(nullptr);
    for(Itr_db i=dbAttrList->begin();i!=dbAttrList->end();++i){
      if( i->first == tt.coolId() ){
        attrList = &(i->second);
        break;
      }
    }
    return attrList;    
  }
   
  std::vector<const coral::AttributeList*>   
  L1CaloxAODOfflineTriggerTowerTools::DbRxGainsAttributes( const xAOD::TriggerTower& tt , const CondAttrListCollection* dbAttrList )  const
  {
    typedef CondAttrListCollection::const_iterator Itr_db;
    std::vector<const coral::AttributeList*> v_attr;
    std::vector<unsigned int> rxID = receiversId( tt );
    for( auto r : rxID ){
    for(Itr_db i=dbAttrList->begin();i!=dbAttrList->end();++i){
        if( i->first == r ){
          v_attr.push_back( &(i->second) );
          break;
        }
      }     
    }
    return v_attr;
  }


  // Database access
  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::ModuleId( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "ModuleId"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;
  }

  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::ErrorCode( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "ErrorCode"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value; 
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprDacScanResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprDacScanResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;  
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprPedestalRunResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprPedestalRunResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprNoiseRunResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprNoiseRunResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;  
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprTimingResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprTimingResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;  
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprSatBcidResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprSatBcidResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;    
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprFirFilterResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprFirFilterResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;
  }

  unsigned long long                         
  L1CaloxAODOfflineTriggerTowerTools::PprLutValuesResultsTimeStamp( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned long long value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PprLutValuesResultsTimeStamp"){
        value = (unsigned long long) (*attrList)[attrNum].data<unsigned long long>();
        break;
      }
    }
    return value;   
  }

  double                                     
  L1CaloxAODOfflineTriggerTowerTools::DacOffset( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    double value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "DacOffset"){
        value = (double) (*attrList)[attrNum].data<double>();
        break;
      }
    }
    return value;   
  }

  double                                     
  L1CaloxAODOfflineTriggerTowerTools::DacSlope( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    double value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "DacSlope"){
        value = (double) (*attrList)[attrNum].data<double>();
        break;
      }
    }
    return value;   
  }

  double                                     
  L1CaloxAODOfflineTriggerTowerTools::PedMean( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    double value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PedMean"){
        value = (double) (*attrList)[attrNum].data<double>();
        break;
      }
    }
    return value;   
  }

  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::PedValue( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "PedValue"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;   
  }

  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::FullDelayData( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FullDelayData"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::SyncDelayBcid( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "SyncDelayBcid"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::InBcidNegedge( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "InBcidNegedge"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;  
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::ExtBcidThreshold( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "ExtBcidThreshold"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::SatBcidThreshLow( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "SatBcidThreshLow"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::SatBcidThreshHigh( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "SatBcidThreshHigh"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;    
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::SatBcidLevel( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "SatBcidLevel"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::BcidEnergyRangeLow( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "BcidEnergyRangeLow"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::BcidEnergyRangeHigh( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "BcidEnergyRangeHigh"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  unsigned short int                         
  L1CaloxAODOfflineTriggerTowerTools::FirStartBit( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FirStartBit"){
        value = (unsigned short int) (*attrList)[attrNum].data<unsigned short int>();
        break;
      }
    }
    return value;   
  }

  short                                      
  L1CaloxAODOfflineTriggerTowerTools::FirCoeff1( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FirCoeff1"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;   
  }

  short                                      
  L1CaloxAODOfflineTriggerTowerTools::FirCoeff2( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FirCoeff2"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;   
  }

  short                                      
  L1CaloxAODOfflineTriggerTowerTools::FirCoeff3( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FirCoeff3"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;   
  }

  short                                      
  L1CaloxAODOfflineTriggerTowerTools::FirCoeff4( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FirCoeff4"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value;   
  }

  short                                      
  L1CaloxAODOfflineTriggerTowerTools::FirCoeff5( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "FirCoeff5"){
        value = (short) (*attrList)[attrNum].data<short>();
        break;
      }
    }
    return value; 
  }

  unsigned short                             
  L1CaloxAODOfflineTriggerTowerTools::LutStrategy( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "LutStrategy"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short>();
        break;
      }
    }
    return value;  
  }

  unsigned short                             
  L1CaloxAODOfflineTriggerTowerTools::LutOffset( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "LutOffset"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short>();
        break;
      }
    }
    return value;   
  }

  unsigned short                             
  L1CaloxAODOfflineTriggerTowerTools::LutNoiseCut( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "LutNoiseCut"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short>();
        break;
      }
    }
    return value;  
  }

  unsigned short                             
  L1CaloxAODOfflineTriggerTowerTools::LutSlope( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned short value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "LutSlope"){
        value = (unsigned short) (*attrList)[attrNum].data<unsigned short >();
        break;
      }
    }
    return value;  
  }


  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::DeadChannel( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "disabledBits"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;  
  }

  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::DisabledTower( const xAOD::TriggerTower& tt ) const
  {
    const coral::AttributeList* attrList = DbAttributes( tt , m_dbPpmChanCalib );
    unsigned int value = 0;
    for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
      if((*attrList)[attrNum].specification().name() == "ErrorCode"){
        value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
        break;
      }
    }
    return value;   
  }


  float                                      
  L1CaloxAODOfflineTriggerTowerTools::RxGain( const xAOD::TriggerTower& /* tt */ ) const
  {
    float value = 0;
//     for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
//       if((*attrList)[attrNum].specification().name() == "gain"){
//         value = (float) (*attrList)[attrNum].data<float>();
//         break;
//       }
//     }
    return value;   
  }

  unsigned int                               
  L1CaloxAODOfflineTriggerTowerTools::RxStatus( const xAOD::TriggerTower& /* tt */ ) const
  {
    unsigned int value = 0;
//     for(unsigned int attrNum=0;attrNum<attrList->size();++attrNum){
//       if((*attrList)[attrNum].specification().name() == "status"){
//         value = (unsigned int) (*attrList)[attrNum].data<unsigned int>();
//         break;
//       }
//     }
    return value;  
  }
  
  Identifier                                 
  L1CaloxAODOfflineTriggerTowerTools::towerID( const xAOD::TriggerTower& tt ) const
  {
    return m_lvl1Helper->tower_id( pos_neg_z(tt) , tt.sampling() , region(tt) , ieta(tt) , iphi(tt) );  
  }
  
  int                                        
  L1CaloxAODOfflineTriggerTowerTools::pos_neg_z( const xAOD::TriggerTower& tt ) const
  {
    return m_l1CaloTTIdTools->pos_neg_z( tt.eta() );  
  }
  
  int                                        
  L1CaloxAODOfflineTriggerTowerTools::region( const xAOD::TriggerTower& tt ) const
  {
    return m_l1CaloTTIdTools->regionIndex( tt.eta() );  
  }
  
  int                                        
  L1CaloxAODOfflineTriggerTowerTools::ieta( const xAOD::TriggerTower& tt ) const
  {
    return m_l1CaloTTIdTools->etaIndex( tt.eta() );  
  }
  
  int                                        
  L1CaloxAODOfflineTriggerTowerTools::iphi( const xAOD::TriggerTower& tt ) const
  {
    return m_l1CaloTTIdTools->phiIndex( tt.eta() , tt.phi() );  
  }
    
}
