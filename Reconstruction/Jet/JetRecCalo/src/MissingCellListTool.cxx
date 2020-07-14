///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MissingCellListTool.cxx 
// Implementation file for class MissingCellListTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// JetMomentTools includes
#include "JetRecCalo/MissingCellListTool.h"

// STL includes

// FrameWork includes
//#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/errorcheck.h"

// StoreGate
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

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
  m_tileTool("TileBadChanTool")
{

  //declareInterface<IMissingCellListTool>( this );  

  declareProperty("AddCellList", m_userAddedCells);    
  declareProperty("RemoveCellList", m_userRemovedCells );

  declareProperty("AddBadCells", m_addBadCells=true );

  declareProperty("DeltaRmax", m_rmax=1.0);


  // ported from JetBadChanCorrTool
  declareProperty("AddCellFromTool", m_addCellFromTool = false);
  declareProperty("LArMaskBit", m_larMaskBit = ( 1<< 0 | 1<< 2 | 1<<16 | 1<<8 | 1<<11 | 1<<14 | 1<<19 )); //608517, deadReadout | deadPhys | missingFEB | highNoiseHG | highNoiseMG | highNoiseLG | sporadicBurstNoise
  declareProperty("TileMaskBit", m_tileMaskBit = ( 1<< 0 )); //1, dead

  declareProperty("MissingCellMapName",m_missingCellMapName = "MissingCaloCellsMap");
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
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  CHECK( m_tileCabling.retrieve() );

  CHECK(detStore()->retrieve(m_caloDDM) );

  //m_badandmissingCellsGeomMap.init(m_rmax);


  // Register listener for BeginEvent
  //incidentSvc->addListener(this, "BeginEvent");



  if ( m_addCellFromTool) {
    CHECK( m_tileTool.retrieve() );
    ATH_CHECK( m_BCKey.initialize() );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode MissingCellListTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

// IMissingCellListTool::const_iterator MissingCellListTool::begin() {
//   if(prepareCellList().isFailure()) return m_missingCells.end();
//   return m_missingCells.begin();
// }
// IMissingCellListTool::const_iterator MissingCellListTool::end()   {
//   if(prepareCellList().isFailure()) return m_missingCells.end();
//   return m_missingCells.end();
// }
// size_t MissingCellListTool::size() {
//   if(prepareCellList().isFailure()) return 0;
//   return m_missingCells.size();
// }


int MissingCellListTool::execute() const {

  //if(! m_needSetup) return StatusCode::SUCCESS;

  // clear cell containers.

  //cellset_t missingCells;
  jet::CaloCellFastMap * badandmissingCellsGeomMap = new jet::CaloCellFastMap();
  
  jet::cellset_t & badandmissingCells = badandmissingCellsGeomMap->cells();

  badandmissingCellsGeomMap->init(m_rmax);
  
  // get permanently missing cells.
  const std::vector<Identifier> & tiledisconnected = m_tileCabling->disconnectedCells();
  badandmissingCells.insert(tiledisconnected.begin(), tiledisconnected.end());

  // ATH_MSG( DEBUG ) << " tile disconnected "<< tiledisconnected.size() << "  "<< missingCells.size() << endmsg; 
  
  // // fill bad and missing cell list. ------------
  // badandmissingCells.insert( missingCells.begin(), missingCells.end());

  // from user given cells
  for(cellidvec_t::const_iterator it=m_userAddedCells.begin(); it != m_userAddedCells.end(); ++it){
    badandmissingCells.insert( Identifier(*it) ) ;
  }
  // remove user give cells.
  for(cellidvec_t::const_iterator it=m_userRemovedCells.begin(); it != m_userRemovedCells.end(); ++it){
    badandmissingCells.erase( Identifier(*it) );
  }
  // ---------------------------


  if(m_addBadCells) {
    // (In run1 this part possibly added several times the same cell in the geometric map)
    const CaloCellContainer * cells;
    StatusCode sc = evtStore()->retrieve(cells, "AllCalo");
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR("Unable to retrieve CaloCellContainer AllCalo from event store.");
      return 1;
    }
 
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
    const CaloCell_ID*  calo_id = m_caloDDM->getCaloCell_ID();    
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
    const CaloDetDescrElement * dde = m_caloDDM->get_element(*lit);
    jet::CellPosition p(dde->eta(), dde->phi(), *lit, dde->getSampling());
    badandmissingCellsGeomMap->insert( p );
  }  
  // ---------------------------


  ATH_MSG( DEBUG ) << " total bad and missing "<< badandmissingCells.size() << "  "<< badandmissingCellsGeomMap->size() << endmsg; 
  

  StatusCode sc = evtStore()->record(badandmissingCellsGeomMap, m_missingCellMapName) ;
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Unable to record badandmissingCellsGeomMap in event store: " << m_missingCellMapName);
   return 1;
  }

  //  m_needSetup = false;
  return 0;
}


//  ////////////////////////////////////////////////////////////////////////////////
//  // Handle incidents
//  ////////////////////////////////////////////////////////////////////////////////
// void MissingCellListTool::handle(const Incident& inc)
//  {
//    ATH_MSG_DEBUG ("handle " << name() << "..."<< inc.type());
//    if (inc.type()=="BeginEvent") {
//      ATH_MSG(DEBUG) << " got begining event" << endmsg;
//      m_needSetup = true;
//      //if(prepareCellList().isFailure()) ATH_MSG(ERROR)<< " Could not build missing cell list !!"<<endmsg;
//    }
   
//  }
