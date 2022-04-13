/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "EmptyCellBuilderTool.h"
#include "FastCaloSim/FastSimCell.h"

#include "AthAllocators/DataPool.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"

EmptyCellBuilderTool::EmptyCellBuilderTool(const std::string& type, 
					   const std::string& name, 
					   const IInterface* parent)
  : base_class(type, name, parent)
{ 
}

StatusCode EmptyCellBuilderTool::initialize()
{
  ATH_MSG_INFO("Initialisating started");
  ATH_CHECK(m_caloMgrKey.initialize());
  ATH_MSG_INFO("Initialisating finished");
  return StatusCode::SUCCESS;
}

StatusCode
EmptyCellBuilderTool::process (CaloCellContainer* theCellContainer,
                               const EventContext& ctx) const
{
  create_empty_calo(ctx, theCellContainer);
  return StatusCode::SUCCESS;
}

void EmptyCellBuilderTool::create_empty_calo(const EventContext& ctx,
                                             CaloCellContainer * theCellContainer) const
{
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  if(!caloMgrHandle.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve CaloDetDescrManager!");
    std::abort();
  }
  const CaloDetDescrManager* caloDDM = *caloMgrHandle;

  ATH_MSG_DEBUG("Executing start calo size=" <<theCellContainer->size()<<" Event="<<ctx.evt());
  bool check_exist=false;
  if(theCellContainer->size()>0) {
    check_exist=true;
    ATH_MSG_DEBUG("  CaloCells do already exist: check required when creating!");
  }
  
  int ncreate=0;
  int nfound=0;
  double E_tot=0;
  double Et_tot=0;

  #if FastCaloSim_project_release_v1 == 12
  #else  
    DataPool<FastSimTileCell> CellsPTile(10000);
    DataPool<FastSimCaloCell> CellsPCalo(190000);
    ATH_MSG_DEBUG("before: CellsPTile.capacity()="<<CellsPTile.capacity()<<" CellsPTile.allocated()="<<CellsPTile.allocated());
    ATH_MSG_DEBUG("before: CellsPCalo.capacity()="<<CellsPCalo.capacity()<<" CellsPCalo.allocated()="<<CellsPCalo.allocated());
  #endif

  for(const CaloDetDescrElement* theDDE : caloDDM->element_range()) {
    if(theDDE) {
      CaloCell* theCaloCell=0;
      
      if(check_exist) theCaloCell=(CaloCell*)(theCellContainer->findCell(theDDE->calo_hash()));
      
      if(theCaloCell) {
        theCaloCell->setGain(CaloGain::INVALIDGAIN);
        E_tot+=theCaloCell->energy();
        Et_tot+=theCaloCell->energy()/cosh(theCaloCell->eta());
        ++nfound;
      } else {
        CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
        #if FastCaloSim_project_release_v1 == 12
          if(calo==CaloCell_ID::TILE) {
            theCaloCell=new FastSimTileCell();
          } else {
            theCaloCell=new FastSimCaloCell();
          }
        #else  
          if(calo==CaloCell_ID::TILE) {
            theCaloCell = CellsPTile.nextElementPtr();
          } else {
            theCaloCell = CellsPCalo.nextElementPtr();
          }

          theCaloCell->setEnergy(0);
          theCaloCell->setTime(0);
          theCaloCell->setQuality(1.0);
          theCaloCell->setGain(CaloGain::INVALIDGAIN);
          theCaloCell->setCaloDDE(theDDE);
        #endif  

        theCellContainer->push_back(theCaloCell);
        ++ncreate;
      }  
    }  
  }

  ATH_MSG_DEBUG(ncreate<<" cells created, "<<nfound<<" cells already found: size="<<theCellContainer->size()<<" e="<<E_tot<<" ; et="<<Et_tot<<". Now initialize and order calo...");

  // check whether has max hash id size
  const CaloCell_ID * theCaloCCIDM   = caloDDM->getCaloCell_ID() ;
  unsigned int hashMax=theCaloCCIDM->calo_cell_hash_max();
  if (theCellContainer->size()<hashMax) {
    ATH_MSG_DEBUG("CaloCellContainer size " << theCellContainer->size() << " smaller than hashMax: " << hashMax);
  }
  else if (theCellContainer->size()==hashMax)  {
    ATH_MSG_DEBUG("CaloCellContainer size " << theCellContainer->size() << " correspond to hashMax : " << hashMax);
    theCellContainer->setHasTotalSize(true);
  } 	
  else {
    ATH_MSG_WARNING("CaloCellContainer size " << theCellContainer->size() 
	<< " larger than hashMax ! Too many cells ! " << hashMax);

  }

  // check whether in order and complete
  if (theCellContainer->checkOrderedAndComplete()){
    ATH_MSG_DEBUG("CaloCellContainer ordered and complete");
    theCellContainer->setIsOrderedAndComplete(true);
    theCellContainer->setIsOrdered(true);
  } else {	
    ATH_MSG_DEBUG("CaloCellContainer not ordered or incomplete");
    theCellContainer->setIsOrderedAndComplete(false);
    // check whether in order
    if (theCellContainer->checkOrdered()){
      ATH_MSG_DEBUG("CaloCellContainer ordered");
      theCellContainer->setIsOrdered(true);
    } else {	
      ATH_MSG_DEBUG("CaloCellContainer not ordered");
      theCellContainer->setIsOrdered(false);
    }
  }

  if (!theCellContainer->isOrdered()) theCellContainer->order();

  #if FastCaloSim_project_release_v1 == 12
  #else  
    ATH_MSG_DEBUG("before: CellsPTile.capacity()="<<CellsPTile.capacity()<<" CellsPTile.allocated()="<<CellsPTile.allocated());
    ATH_MSG_DEBUG("before: CellsPCalo.capacity()="<<CellsPCalo.capacity()<<" CellsPCalo.allocated()="<<CellsPCalo.allocated());
  #endif  
}

