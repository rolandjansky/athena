///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// MissingCellListTool.cxx 
// Implementation file for class MissingCellListTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// JetMomentTools includes
#include "JetRecCalo/MissingCellListTool.h"
// FrameWork includes
#include "AthenaKernel/errorcheck.h"
// StoreGate
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandle.h"

//
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileBadChanTool.h"



// /////////////////////////////////////////////////////////////////// 
// // Public methods: 
// /////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
MissingCellListTool::MissingCellListTool(const std::string& name) :
  asg::AsgTool  (  name   ),
  m_tileCabling("TileCablingSvc",name),
  m_tileTool("TileBadChanTool",this)
{

  //declareInterface<IMissingCellListTool>( this );  

  declareProperty("AddCellList", m_userAddedCells);    
  declareProperty("RemoveCellList", m_userRemovedCells );
  declareProperty("AddBadCells", m_addBadCells=true );
  declareProperty("DeltaRmax", m_rmax=1.0);
  // ported from JetBadChanCorrTool
  declareProperty("AddCellFromTool", m_addCellFromTool = false);
  //608517, deadReadout | deadPhys | missingFEB | highNoiseHG | highNoiseMG | highNoiseLG | spor    adicBurstNoise
  declareProperty("LArMaskBit", m_larMaskBit = ( 1<< 0 | 1<< 2 | 1<<16 | 1<<8 | 1<<11 | 1<<14 | 1<<19 )); 
  declareProperty("TileMaskBit", m_tileMaskBit = ( 1<< 0 )); //1, dead
}

// Destructor
///////////////
MissingCellListTool::~MissingCellListTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode MissingCellListTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  CHECK( m_tileCabling.retrieve() );
  //
  ATH_CHECK(m_badCellMap_key.initialize());
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());
  ATH_CHECK(m_cells_name.initialize(m_addBadCells));
  ATH_CHECK(m_BCKey.initialize(m_addCellFromTool));
  if (m_addCellFromTool) {
    ATH_CHECK( m_tileTool.retrieve() );
  } else {
    m_tileTool.disable();
  }
  return StatusCode::SUCCESS;
}

StatusCode MissingCellListTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}


int MissingCellListTool::execute() const {

  //cellset_t missingCells;
  auto badandmissingCellsGeomMap = std::make_unique<jet::CaloCellFastMap>();
  jet::cellset_t & badandmissingCells = badandmissingCellsGeomMap->cells();
  badandmissingCellsGeomMap->init(m_rmax);
  
  // get permanently missing cells.
  const std::vector<Identifier> & tiledisconnected = m_tileCabling->disconnectedCells();
  badandmissingCells.insert(tiledisconnected.begin(), tiledisconnected.end());

  // from user given cells
  for(cellidvec_t::const_iterator it=m_userAddedCells.begin(); it != m_userAddedCells.end(); ++it){
    badandmissingCells.insert( Identifier(*it) ) ;
  }
  // remove user give cells.
  for(cellidvec_t::const_iterator it=m_userRemovedCells.begin(); it != m_userRemovedCells.end(); ++it){
    badandmissingCells.erase( Identifier(*it) );
  }
  // ---------------------------

  SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle {m_caloDetDescrMgrKey};
  const CaloDetDescrManager* caloDDM = *caloDetDescrMgrHandle;

  if(m_addBadCells) {
    // (In run1 this part possibly added several times the same cell in the geometric map)
    SG::ReadHandle<CaloCellContainer> cc (m_cells_name);
    if ( !cc.isValid() ) {
      ATH_MSG_ERROR("Unable to retrieve CaloCellContainer AllCalo from event store.");
      return 1;
    }
    const CaloCellContainer * cells = cc.ptr();
    CaloCellContainer::const_iterator it= cells->begin();
    CaloCellContainer::const_iterator itE= cells->end();
    for(; it!=itE; ++it){
      if( (*it)->badcell() ) {
        const CaloCell * c= *it;
        badandmissingCells.insert( c->ID() );
      }
    }
  }


  if(m_addCellFromTool){
    const CaloCell_ID*  calo_id = caloDDM->getCaloCell_ID();    
    const TileBadChanTool* tileTool = dynamic_cast<const TileBadChanTool*>(m_tileTool.operator->()); 
    SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
    const LArBadChannelCont *bcCont {*readHandle};
    if(!bcCont) {
      ATH_MSG_ERROR( "Do not have Bad chan container !!!" );
      return 1;
    }
    std::vector<Identifier>::const_iterator idItr = calo_id->cell_begin();
    std::vector<Identifier>::const_iterator idItrE = calo_id->cell_end();
    for(; idItr!=idItrE; idItr++){

      // check if needs insertion.
      bool insert = false;
      if(calo_id->is_tile(*idItr)){
        CaloBadChannel bc = tileTool->caloStatus(*idItr);
        insert =  (bc.packedData() & m_tileMaskBit) != 0 ;
      } else {
        LArBadChannel bc = bcCont->offlineStatus(*idItr);
        insert = (bc.packedData() & m_larMaskBit)  != 0 ;
      } 

      if(insert) {
        badandmissingCells.insert( *idItr );
      }
    }
  }

  // fill the geometric map : ------------------
  jet::cellset_t::iterator lit = badandmissingCells.begin();
  jet::cellset_t::iterator litE = badandmissingCells.end();
  for( ; lit != litE; ++lit){
    const CaloDetDescrElement * dde = caloDDM->get_element(*lit);
    jet::CellPosition p(dde->eta(), dde->phi(), *lit, dde->getSampling());
    badandmissingCellsGeomMap->insert( p );
  }  
  // ---------------------------


  ATH_MSG( DEBUG ) << " total bad and missing "<< badandmissingCells.size() << "  "<< badandmissingCellsGeomMap->size() << endmsg;

  SG::WriteHandle<jet::CaloCellFastMap> badCellMap(m_badCellMap_key);
  StatusCode sc = badCellMap.record(std::move(badandmissingCellsGeomMap));
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to record badandmissingCellsGeomMap in event store: "
                  << m_badCellMap_key);
    return 1;
  }

  return 0;
}

