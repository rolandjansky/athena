/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: Denis Oliveira Damazio                              +
// + Institute .....: BNL                                                 +
// + Creation date .: 29/04/2021                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArHITtoCell.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "StoreGate/StoreGateSvc.h"
#include "boost/foreach.hpp"
#include <cmath>
#include <random>
#include <sys/time.h>

#include <mutex>

/**
 * @brief Standard Gaudi algorithm constructor.
 */
LArHITtoCell::LArHITtoCell(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_calo_id_manager(0)
{
}

StatusCode LArHITtoCell::initialize(){
	
	ATH_CHECK( m_hitMapKey.initialize() );
	if ( m_isSC ) {
	  ATH_CHECK( m_scidtool.retrieve() );
	  ATH_CHECK( m_caloSuperCellMgrKey.initialize() );
	}
	else {
	  ATH_CHECK( m_caloMgrKey.initialize() );
	}
	ATH_CHECK( detStore()->retrieve (m_calo_id_manager, "CaloIdManager") );
        
        CHECK( detStore()->retrieve(m_OflHelper,"CaloCell_ID") );
        ATH_CHECK( m_cablingKey.initialize() );
	ATH_CHECK( m_fracSKey.initialize() );
	ATH_CHECK( m_cellKey.initialize() );
        
        //
        //..... need of course the LVL1 helper
        //
        m_scHelper = m_calo_id_manager->getCaloCell_SuperCell_ID();
        if (!m_scHelper) {
          ATH_MSG_ERROR( "Could not access CaloCell_SuperCell_ID helper");
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_DEBUG( "Successfully accessed CaloCell_SuperCell_ID helper");
        }

	return StatusCode::SUCCESS;
}

/**
 * @brief Algorithm execute method.
 */
StatusCode LArHITtoCell::execute(const EventContext& context) const
{


  SG::ReadHandle<LArHitEMap> hitmap(m_hitMapKey,context);
  const LArHitEMap* hitmapPtr = hitmap.cptr();

  const LArOnOffIdMapping* cabling = this->retrieve(context, m_cablingKey);
  if(!cabling) {
     ATH_MSG_ERROR("Do not have cabling map !!!");
     return StatusCode::FAILURE;
  }

  auto fracS = this->retrieve(context,m_fracSKey);
  if (!fracS ) {
     ATH_MSG_ERROR("Do not have SC fracs !!!");
     return StatusCode::FAILURE;
  }

  const CaloDetDescrManager_Base* dd_mgr{nullptr};
  if(m_isSC) {
    SG::ReadCondHandle<CaloSuperCellDetDescrManager> caloSuperCellMgrHandle{m_caloSuperCellMgrKey,context};
    ATH_CHECK(caloSuperCellMgrHandle.isValid());
    dd_mgr = *caloSuperCellMgrHandle;
  }
  else {
    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,context};
    ATH_CHECK(caloMgrHandle.isValid());
    dd_mgr = *caloMgrHandle;
  }

  int it = 0;
  int it_end = hitmapPtr->GetNbCells();
  std::vector< std::pair<float,float> > energy_time;
  int max (0);
  if ( m_isSC ) max = m_scHelper->calo_cell_hash_max ();
  else max = m_OflHelper->calo_cell_hash_max ();
  energy_time.assign(max,std::pair<float,float>(0.0,0.0));
  for( ; it!=it_end;++it) {
    IdentifierHash hash(it);
    const LArHitList& hitlist = hitmapPtr->GetCell(it);
    const std::vector<std::pair<float,float> >& timeE = hitlist.getData();
    if (timeE.size() == 0 ) continue;
    if ( m_isSC ){ // convert hash to sc hash
    Identifier cellId = m_OflHelper->cell_id(hash);
    Identifier scId = m_scidtool->offlineToSuperCellID(cellId);
    IdentifierHash scHash = m_scHelper->calo_cell_hash(scId) ;
    if ( scHash.value() == 999999 ) continue;
    hash = scHash;
    }
    std::vector<std::pair<float,float> >::const_iterator first = timeE.begin();
    std::vector<std::pair<float,float> >::const_iterator last  = timeE.end();
    while(first!=last) {
      float time   = (*first).second;;
      float energy = (*first).first;
      if ( (std::abs(time) < 13)  ) {
	 energy_time[hash].first+=energy;
	 energy_time[hash].second+=(energy*time);
      }
      ++first;
    }
   } // enf of for in hits

   auto  outputContainerCellPtr = std::make_unique<CaloCellContainer>();
   DataPool<CaloCell> dataPool;
   if (dataPool.allocated()==0)
        dataPool.reserve (max);
   outputContainerCellPtr->reserve( max );
   for(int i=0;i<max;i++) {
	float energy = energy_time[i].first;
	if ( energy > 1 ){ // not zero
	  float time = energy_time[i].second / energy;
	  const CaloDetDescrElement* dde(nullptr);
          CaloCell* ss = dataPool.nextElementPtr();
	  HWIdentifier hw;
          if ( m_isSC ){
    	    Identifier scId = m_scHelper->cell_id(IdentifierHash(i));
	    hw = cabling->createSignalChannelID(scId);
            dde = dd_mgr->get_element (scId);
	  } else {
            Identifier cellId = m_OflHelper->cell_id(IdentifierHash(i));
	    hw = cabling->createSignalChannelID(cellId);
            dde = dd_mgr->get_element (cellId);
	  }
       
          ss->setCaloDDE(dde);
	  if ( fracS->FSAMPL(hw) < 0.00001 ) continue;
          ss->setEnergy(energy/fracS->FSAMPL(hw));
          ss->setTime(time);
          ss->setGain((CaloGain::CaloGain)0);
          // for super-cells provenance and time are slightly different
          uint16_t prov = 0x2000;
          ss->setProvenance(prov);
	  // misuse of a variable
	  ss->setQuality((uint16_t)1);
       
          outputContainerCellPtr->push_back(ss);
        } // energy greater than 50
   }
   SG::WriteHandle<CaloCellContainer>outputContainer(m_cellKey,context);
   ATH_CHECK(outputContainer.record(std::move(outputContainerCellPtr) ) );

  return StatusCode::SUCCESS;
}

