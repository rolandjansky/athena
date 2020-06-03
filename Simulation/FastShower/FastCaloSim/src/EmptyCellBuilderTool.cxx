/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/EmptyCellBuilderTool.h"
#include "FastCaloSim/FastSimCell.h"

#include "DataModel/DataPool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

EmptyCellBuilderTool::EmptyCellBuilderTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :BasicCellBuilderTool(type, name, parent)
{ 
  declareInterface<ICaloCellMakerTool>( this );  
}


EmptyCellBuilderTool::~EmptyCellBuilderTool()
{
}


StatusCode EmptyCellBuilderTool::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO <<  "Initialisating started" << endreq ;

  StatusCode sc=BasicCellBuilderTool::initialize();

  log << MSG::INFO <<  "Initialisating finished" << endreq ;
  return sc;
}

StatusCode EmptyCellBuilderTool::process(CaloCellContainer * theCellContainer)
{
  MsgStream log( msgSvc(), name() );
  
  create_empty_calo(theCellContainer);

  ++m_nEvent;
//  log << MSG::INFO << "Executing finished calo size=" <<theCellContainer->size()<< endreq;
  return StatusCode::SUCCESS;
}

void EmptyCellBuilderTool::create_empty_calo(CaloCellContainer * theCellContainer)
{
  MsgStream log( msgSvc(), name() );
  
  ATH_MSG_DEBUG("Executing start calo size=" <<theCellContainer->size()<<" Event="<<m_nEvent);
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
    log << MSG::DEBUG << "before: CellsPTile.capacity()="<<CellsPTile.capacity()<<" CellsPTile.allocated()="<<CellsPTile.allocated()<<endreq;
    log << MSG::DEBUG << "before: CellsPCalo.capacity()="<<CellsPCalo.capacity()<<" CellsPCalo.allocated()="<<CellsPCalo.allocated()<<endreq;
  #endif  

  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_caloDDM->element_begin();calo_iter<m_caloDDM->element_end();++calo_iter) {
    const CaloDetDescrElement* theDDE=*calo_iter;
    if(theDDE) {
//      if(n%10000==0) {
//        log << MSG::DEBUG <<"  "<<n<<" : det_elm eta=" <<theDDE->eta()<<" phi="<<theDDE->phi()<<" hash="<<theDDE->calo_hash()<< endreq;
//      }
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
  //        theCaloCell=new CaloCell(theDDE,0.0,0.0,1.0,CaloGain::INVALIDGAIN);
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
//      ++n;
    }  
  }

  log << MSG::DEBUG << ncreate<<" cells created, "<<nfound<<" cells already found: size="<<theCellContainer->size()<<" e="<<E_tot<<" ; et="<<Et_tot<<". Now initialize and order calo..." << endreq;

  // check whether has max hash id size
  const CaloCell_ID * theCaloCCIDM   = m_caloDDM->getCaloCell_ID() ;
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
    log << MSG::DEBUG << "before: CellsPTile.capacity()="<<CellsPTile.capacity()<<" CellsPTile.allocated()="<<CellsPTile.allocated()<<endreq;
    log << MSG::DEBUG << "before: CellsPCalo.capacity()="<<CellsPCalo.capacity()<<" CellsPCalo.allocated()="<<CellsPCalo.allocated()<<endreq;
  #endif  

//  log << MSG::INFO << "theCellContainer->hasTotalSize()="<<theCellContainer->hasTotalSize()<< endreq;
//  log << MSG::INFO << "theCellContainer->isOrderedAndComplete()="<<theCellContainer->isOrderedAndComplete()<< endreq;
//  log << MSG::INFO << "theCellContainer->isOrdered()="<<theCellContainer->isOrdered()<< endreq;
}

