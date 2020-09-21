/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : CalibHitToCaloCell.cxx
//
//  Author   : Gia  		gia.khoriauli@cern.ch
//  Created  : March, 2005
//
//  DESCRIPTION:
//     Algorithm to make CaloCell from CaloCalibrationHit 
//
//     This algorithm creates two kinds of CaloCell from CalibHits by default.
//	One is CaloCells with CalibHit TOTAL energy
//	Second is CaloCels with CalibHit VISIBLE energy
//
//     However, if one declares the names of the appropriate 
//     CaloCellContainers the CaloCells with either EM or NONEM energy
//     can be created and stored also.
//
//*****************************************************************************

// Calo include
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"  
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloCalibHitRec/CalibHitToCaloCell.h"

// Tile includes
#include "TileEvent/TileCell.h"

// LAr includes
#include "LArRecEvent/LArCell.h"


CalibHitToCaloCell::CalibHitToCaloCell(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator),
     m_store_Tot(false),
     m_store_Vis(false),
     m_store_Em(false),
     m_store_NonEm(false),
     m_storeUnknown(false),
     m_caloCell_Tot("TotalCalibCell"), m_caloCell_Vis("VisCalibCell"), 
     m_caloCell_Em(""), m_caloCell_NonEm(""),
     m_caloCell_ID(nullptr),
     m_caloDM_ID(nullptr),
     //m_tile_ID(0),
     //m_larEm_ID(0),
     //m_larHec_ID(0),
     //m_larFcal_ID(0),
     m_nchan(0)

//	The names suggestion if one needs to have them
//
//      m_caloCell_Em("EmCalibCell"), m_caloCell_NonEm("NonEmCalibCell")

{
  m_tileActiveHitCnt   = "TileCalibHitActiveCell";
  m_tileInactiveHitCnt = "TileCalibHitInactiveCell";
  m_tileDMHitCnt       = "TileCalibHitDeadMaterial";
  m_larActHitCnt   = "LArCalibrationHitActive";
  m_larInactHitCnt = "LArCalibrationHitInactive";
  m_larDMHitCnt    = "LArCalibrationHitDeadMaterial";

  declareProperty("StoreUnknownCells", m_storeUnknown);

  declareProperty("CellTotEne",    m_caloCell_Tot);
  declareProperty("CellVisEne",    m_caloCell_Vis);
  declareProperty("CellEmEne",     m_caloCell_Em);
  declareProperty("CellNonEmEne",  m_caloCell_NonEm);
}


CalibHitToCaloCell::~CalibHitToCaloCell()
{
}


////////////////   INITIALIZE   ///////////////////////
StatusCode CalibHitToCaloCell::initialize() 
{ 
  // retrieve ID helpers from det store
  ATH_MSG_INFO("initialisation ID helpers" );

  ATH_CHECK(  detStore()->retrieve(m_caloCell_ID) );
  ATH_CHECK(  detStore()->retrieve(m_caloDM_ID) );

  m_store_Tot = !m_caloCell_Tot.empty();
  m_store_Vis = !m_caloCell_Vis.empty();
  m_store_Em = !m_caloCell_Em.empty();
  m_store_NonEm = !m_caloCell_NonEm.empty();

  ATH_MSG_INFO("initialisation completed" );
  return StatusCode::SUCCESS;
}


/////////////////   EXECUTE   //////////////////////
StatusCode CalibHitToCaloCell::execute()
{
    const CaloDetDescrManager* caloDDMgr = nullptr;
    ATH_CHECK(  detStore()->retrieve(caloDDMgr, "CaloMgr") );

    // OUTPUT CONTAINERS
    CaloCellContainer* cnt   = nullptr;
    CaloCellContainer* cnt_1 = nullptr; 
    CaloCellContainer* cnt_2 = nullptr;
    CaloCellContainer* cnt_3 = nullptr;
    if(m_store_Tot)    cnt   = new CaloCellContainer();
    if(m_store_Vis)    cnt_1 = new CaloCellContainer(); 
    if(m_store_Em)     cnt_2 = new CaloCellContainer();
    if(m_store_NonEm)  cnt_3 = new CaloCellContainer();

    // INPUT CONTAINERS
    const CaloCalibrationHitContainer* tile_actHitCnt;
    const CaloCalibrationHitContainer* tile_inactHitCnt;
    const CaloCalibrationHitContainer* tile_dmHitCnt;
    const CaloCalibrationHitContainer* lar_actHitCnt; 
    const CaloCalibrationHitContainer* lar_inactHitCnt;
    const CaloCalibrationHitContainer* lar_dmHitCnt;
    
    //retrieving input Calibhit containers
    ATH_CHECK( evtStore()->retrieve(tile_actHitCnt, m_tileActiveHitCnt) );
    ATH_CHECK( evtStore()->retrieve(tile_inactHitCnt, m_tileInactiveHitCnt) );
    ATH_CHECK( evtStore()->retrieve(tile_dmHitCnt,    m_tileDMHitCnt) );
    ATH_CHECK( evtStore()->retrieve(lar_actHitCnt,    m_larActHitCnt) );
    ATH_CHECK( evtStore()->retrieve(lar_inactHitCnt,  m_larInactHitCnt) );
    ATH_CHECK( evtStore()->retrieve(lar_dmHitCnt,     m_larDMHitCnt) );

    ATH_MSG_DEBUG("CaloCalibrationHitContainers retrieved successfuly" );

    //count
    m_nchan=0;
    int em_nchan=0;
    int hec_nchan=0;
    int fcal_nchan=0;
    int lar_unknown_nchan=0;
    int tile_nchan=0; 
    int tile_unknown_nchan = 0 ;
   
    bool new_id = false;    

    std::vector<Identifier> ID;

    //clean up CaloCell vectors 
    //for 'this' event 
    m_Cells_Tot.clear();
    m_Cells_Vis.clear();
    m_Cells_Em.clear();
    m_Cells_NonEm.clear();


    if(lar_inactHitCnt)
    {
	if( lar_inactHitCnt->Size() != 0 )
	{
	    CaloCalibrationHitContainer::const_iterator it  = lar_inactHitCnt->begin();
	    CaloCalibrationHitContainer::const_iterator end = lar_inactHitCnt->end();

	    for(; it != end; it++) 
	    {
		double Etot   = (*it)->energyTotal();
    		double Eem    = (*it)->energy(0);
    		double Enonem = (*it)->energy(1);
                double Evis   = Eem + Enonem;

	        Identifier id=(*it)->cellID();

		//check if this ID is LAr one
		if(m_caloCell_ID->is_lar(id)) 
		{
		    const CaloDetDescrElement* caloDDE = caloDDMgr->get_element(id);
		
		    if(m_store_Tot) {
			    LArCell* calibCell = new LArCell(caloDDE, id, Etot, 0., 0, 0, CaloGain::UNKNOWNGAIN );
                	    m_Cells_Tot.push_back(calibCell) ; 
	            }

                    if(m_store_Vis) {
			    LArCell* calibCell_1 = new LArCell(caloDDE, id, Evis, 0., 0, 0, CaloGain::UNKNOWNGAIN );
             		    m_Cells_Vis.push_back(calibCell_1) ;
		    }

                    if(m_store_Em) {
			    LArCell* calibCell_2 = new LArCell(caloDDE, id, Eem, 0., 0, 0, CaloGain::UNKNOWNGAIN );
                	    m_Cells_Em.push_back(calibCell_2) ;
		    }

                    if(m_store_NonEm) {
			    LArCell* calibCell_3 = new LArCell(caloDDE, id, Enonem, 0., 0, 0, CaloGain::UNKNOWNGAIN );
        	            m_Cells_NonEm.push_back(calibCell_3) ;
		    }

	            ID.push_back(id);
		    ++m_nchan;
		}
		//another story...
		else 
		{
                  ATH_MSG_DEBUG( "non-LAr ID in LArInactive " 
                                 << id.getString() << " sub_calo " << m_caloCell_ID->sub_calo(id)  );
		    if(m_storeUnknown)
		    {
                      ATH_MSG_DEBUG("Ianctive CalibHit doesn't respect to any LArCell - "
                                    <<"create dummy LArCell to store energy "  );

                   	if(m_store_Tot) {			
                        	LArCell* calibCell = new LArCell();
		        	calibCell->setEnergy(Etot);
				m_Cells_Tot.push_back(calibCell) ;
		   	}

                   	if(m_store_Vis) {
            	        	LArCell* calibCell_1 = new LArCell();
                		calibCell_1->setEnergy(Evis);
            	        	m_Cells_Vis.push_back(calibCell_1) ;
		   	}

                	if(m_store_Em) {
                		LArCell* calibCell_2 = new LArCell();
                		calibCell_2->setEnergy(Eem);
                		m_Cells_Em.push_back(calibCell_2) ;
		   	}

                	if(m_store_NonEm) {
                		LArCell* calibCell_3 = new LArCell();
            	        	calibCell_3->setEnergy(Enonem);
                		m_Cells_NonEm.push_back(calibCell_3) ;
		   	}
                	
			ID.push_back(id);
			++m_nchan;		    
		    }	
		}
	    }
	}
    }


    if(lar_actHitCnt)
    {
	if( lar_actHitCnt->Size() != 0 )
	{
	    //fix the current size of hits vector. for looping a bit later
	    int hits_vec_size = (int)ID.size() ;

	    CaloCalibrationHitContainer::const_iterator it    = lar_actHitCnt->begin();
	    CaloCalibrationHitContainer::const_iterator end   = lar_actHitCnt->end();

    	    for(; it != end; it++) 
	    {
    		Identifier id=(*it)->cellID();

	        //merge inactive and active hits 
		//from the same cell together
		for (int n=0; n!=hits_vec_size; n++)
		{
        	    if( id == ID[n] ) 
		    {
                        if(m_store_Tot)   m_Cells_Tot[n]->add_energy((*it)->energyTotal()) ;
                        if(m_store_Vis)   m_Cells_Vis[n]->add_energy((*it)->energy(0) + (*it)->energy(1)) ;
                        if(m_store_Em)    m_Cells_Em[n]->add_energy((*it)->energy(0)) ;
                        if(m_store_NonEm) m_Cells_NonEm[n]->add_energy((*it)->energy(1)) ;

			new_id = false;
		        break;
		    }
		    else
		    {
			new_id = true;
		    }
		}
		if(new_id)
		{
                    double Etot   = (*it)->energyTotal();
                    double Eem    = (*it)->energy(0);
                    double Enonem = (*it)->energy(1);
                    double Evis   = Eem + Enonem;
		
               	    if(m_caloCell_ID->is_lar(id))
            	    {
			const CaloDetDescrElement* caloDDE = caloDDMgr->get_element(id);
                    	
                   	if(m_store_Tot) {
                        	LArCell* calibCell = new LArCell(caloDDE, id, Etot, 0., 0, 0, CaloGain::UNKNOWNGAIN );
                        	m_Cells_Tot.push_back(calibCell) ;
			}
                
		        if(m_store_Vis) {
                        	LArCell* calibCell_1 = new LArCell(caloDDE, id, Evis, 0., 0, 0, CaloGain::UNKNOWNGAIN );
                        	m_Cells_Vis.push_back(calibCell_1) ;
			}

	                if(m_store_Em) {
                        	LArCell* calibCell_2 = new LArCell(caloDDE, id, Eem, 0., 0, 0, CaloGain::UNKNOWNGAIN );
        	                m_Cells_Em.push_back(calibCell_2) ;
			}
 
	                if(m_store_NonEm) {
                        	LArCell* calibCell_3 = new LArCell(caloDDE, id, Enonem, 0., 0, 0, CaloGain::UNKNOWNGAIN );
        	                m_Cells_NonEm.push_back(calibCell_3) ;
			}

	                ID.push_back(id);
	    	        ++m_nchan;
        	    }
        	    //another story...
		    else 
		    { 
                      ATH_MSG_DEBUG( "non-LAr ID in LArActive " 
                                     << id.getString() << " sub_calo " << m_caloCell_ID->sub_calo(id)  );
                        if(m_storeUnknown)
                        {
                          ATH_MSG_DEBUG("Active CalibHit doesn't respect to any LArCell - "
                                        <<"create dummy LArCell to store energy " );


                            if(m_store_Tot) {
                        	LArCell* calibCell = new LArCell();
                               	calibCell->setEnergy(Etot);
                        	m_Cells_Tot.push_back(calibCell) ;
                    	    }

	                    if(m_store_Vis) {
        	                LArCell* calibCell_1 = new LArCell();
                	        calibCell_1->setEnergy(Evis);
                        	m_Cells_Vis.push_back(calibCell_1) ;
                    	    }

                    	    if(m_store_Em) {
                            	LArCell* calibCell_2 = new LArCell();
                            	calibCell_2->setEnergy(Eem);
                            	m_Cells_Em.push_back(calibCell_2) ;
                    	    }

                    	    if(m_store_NonEm) {
                            	LArCell* calibCell_3 = new LArCell();
                            	calibCell_3->setEnergy(Enonem);
                            	m_Cells_NonEm.push_back(calibCell_3) ;
                    	    }

                       	    ID.push_back(id);
                       	    ++m_nchan;
			}
		    }
		}
	    }
	}
    } 

    //Now, put LArCells in the containers keeping
    //the order. First goes EM, then HEC and so on
    if(!m_Cells_Tot.empty())
    {
	for(int itr=0; itr!=m_nchan; itr++)
	{
	    if(m_caloCell_ID->is_em(m_Cells_Tot[itr]->ID()))
	    {
		if(m_store_Tot)   cnt->push_back(m_Cells_Tot[itr]);
		if(m_store_Vis)   cnt_1->push_back(m_Cells_Vis[itr]);
                if(m_store_Em)    cnt_2->push_back(m_Cells_Em[itr]);
                if(m_store_NonEm) cnt_3->push_back(m_Cells_NonEm[itr]);

		++em_nchan;
	    }
	}
	if(em_nchan!=0)  
	{
	    if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::LAREM);
            if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::LAREM);
            if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::LAREM);
            if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::LAREM);
	}

        for(int itr=0; itr!=m_nchan; itr++)
        {
	    if(m_caloCell_ID->is_hec(m_Cells_Tot[itr]->ID()))
	    {
                if(m_store_Tot)   cnt->push_back(m_Cells_Tot[itr]);
                if(m_store_Vis)   cnt_1->push_back(m_Cells_Vis[itr]);
                if(m_store_Em)    cnt_2->push_back(m_Cells_Em[itr]);
                if(m_store_NonEm) cnt_3->push_back(m_Cells_NonEm[itr]);

                ++hec_nchan;
	    }
	}
        if(hec_nchan!=0)
        {
            if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::LARHEC);
            if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::LARHEC);
            if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::LARHEC);
            if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::LARHEC);
        }

        for(int itr=0; itr!=m_nchan; itr++)
        {
            if(m_caloCell_ID->is_fcal(m_Cells_Tot[itr]->ID()))
	    {
                if(m_store_Tot) 	cnt->push_back(m_Cells_Tot[itr]);
                if(m_store_Vis) 	cnt_1->push_back(m_Cells_Vis[itr]);
                if(m_store_Em)    cnt_2->push_back(m_Cells_Em[itr]);
                if(m_store_NonEm) cnt_3->push_back(m_Cells_NonEm[itr]);

                ++fcal_nchan;
	    }
	}
        if(fcal_nchan!=0)
        {
            if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::LARFCAL);
            if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::LARFCAL);
            if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::LARFCAL);
            if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::LARFCAL);
        }

    
        if(m_storeUnknown)
        {
            for(int itr=0; itr!=m_nchan; itr++)
	    {
    	        if(!(m_caloCell_ID->is_lar(m_Cells_Tot[itr]->ID())))
        	{
            	    if(m_store_Tot)   cnt->push_back(m_Cells_Tot[itr]);
            	    if(m_store_Vis)   cnt_1->push_back(m_Cells_Vis[itr]);
            	    if(m_store_Em)    cnt_2->push_back(m_Cells_Em[itr]);
            	    if(m_store_NonEm) cnt_3->push_back(m_Cells_NonEm[itr]);

            	    ++lar_unknown_nchan;
        	}
    	    }

	    if(lar_unknown_nchan!=0)
    	    {
        	if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::NOT_VALID);
    	        if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::NOT_VALID);
        	if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::NOT_VALID);
    	        if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::NOT_VALID);
    	    }
	}
    } 

    ATH_MSG_VERBOSE("--- LAr INFO --- "<<m_nchan );
    ATH_MSG_VERBOSE("LArCells  = "<<m_nchan );
    ATH_MSG_VERBOSE("EMCells   = "<<em_nchan );
    ATH_MSG_VERBOSE("HECCells  = "<<hec_nchan );
    ATH_MSG_VERBOSE("FCALCells = "<<fcal_nchan );
    ATH_MSG_VERBOSE("NOT_VALID (LAr) = "<<lar_unknown_nchan );


    if(tile_actHitCnt) 
    {  
        if( (tile_actHitCnt->Size()) != 0 )  
	{
	    CaloCalibrationHitContainer::const_iterator it = tile_actHitCnt->begin();
	    CaloCalibrationHitContainer::const_iterator end = tile_actHitCnt->end();
	
	    for(; it != end; it++) 
	    {
    	        Identifier id=(*it)->cellID();

		double Etot   = (*it)->energyTotal();
    		double Eem    = (*it)->energy(0);
    		double Enonem = (*it)->energy(1);
                double Evis   = Eem + Enonem;

	        if(m_caloCell_ID->is_tile(id))
		{
	            const CaloDetDescrElement* caloDDE = caloDDMgr->get_element(id);

		    if(m_store_Tot) {
        		TileCell* calibCell = new TileCell(caloDDE, id, Etot, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt->push_back(calibCell);
		    }
		
		    if(m_store_Vis) {
            		TileCell* calibCell_1 = new TileCell(caloDDE, id, Evis, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt_1->push_back(calibCell_1);
		    }

		    if(m_store_Em) {
        		TileCell* calibCell_2 = new TileCell(caloDDE, id, Eem, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt_2->push_back(calibCell_2);
		    }

		    if(m_store_NonEm) {
            		TileCell* calibCell_3 = new TileCell(caloDDE, id, Enonem, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt_3->push_back(calibCell_3);
	    	    }

		    ID.push_back(id);
		    ++tile_nchan;
		    ++m_nchan;
		}
	        else 
		{
                  ATH_MSG_DEBUG( "non-Tile ID in Tile " 
                                 << id.getString() << " sub_calo " << m_caloCell_ID->sub_calo(id)  );
            	    if(m_storeUnknown)
            	    {
                      ATH_MSG_DEBUG("Active CalibHit doesn't respect to any TileCell - "
                                    <<"create dummy TileCell to store energy " );

			if(m_store_Tot) {
            		    TileCell* calibCell = new TileCell();
	        	    calibCell->setEnergy(Etot);
    	    	    	    cnt->push_back(calibCell);
			}
			
			if(m_store_Vis) {
        		    TileCell* calibCell_1 = new TileCell();
            	    	    calibCell_1->setEnergy(Evis);
        	    	    cnt_1->push_back(calibCell_1);
			}

			if(m_store_Em) {
            	    	    TileCell* calibCell_2 = new TileCell();
            	    	    calibCell_2->setEnergy(Eem);
            	    	    cnt_2->push_back(calibCell_2);
			}

			if(m_store_NonEm) {
            	    	    TileCell* calibCell_3 = new TileCell();
            	    	    calibCell_3->setEnergy(Enonem);
            	    	    cnt_3->push_back(calibCell_3);
			}

            	        ID.push_back(id);
		        ++tile_unknown_nchan;
            		++m_nchan;
		    }
		}
	    }      

	    if(tile_nchan!=0)
    	    {
        	if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::TILE);
	        if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::TILE);
		if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::TILE);
	        if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::TILE);
	    }

    	    if(tile_unknown_nchan!=0)
    	    {
		if( !(cnt->hasCalo(CaloCell_ID::NOT_VALID)) )
	        {
        	    if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::NOT_VALID);
            	    if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::NOT_VALID);
            	    if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::NOT_VALID);
            	    if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::NOT_VALID);
        	}
	    }
	}
    }


    if(tile_inactHitCnt) 
    {  
        if( (tile_inactHitCnt->Size()) != 0 )  
	{
	    CaloCalibrationHitContainer::const_iterator it = tile_inactHitCnt->begin();
	    CaloCalibrationHitContainer::const_iterator end = tile_inactHitCnt->end();
	
	    for(; it != end; it++) 
	    {
    	        Identifier id=(*it)->cellID();

		double Etot   = (*it)->energyTotal();
    		double Eem    = (*it)->energy(0);
    		double Enonem = (*it)->energy(1);
                double Evis   = Eem + Enonem;

	        if(m_caloCell_ID->is_tile(id))
		{
	            const CaloDetDescrElement* caloDDE = caloDDMgr->get_element(id);

		    if(m_store_Tot) {
        		TileCell* calibCell = new TileCell(caloDDE, id, Etot, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt->push_back(calibCell);
		    }
		
		    if(m_store_Vis) {
            		TileCell* calibCell_1 = new TileCell(caloDDE, id, Evis, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt_1->push_back(calibCell_1);
		    }

		    if(m_store_Em) {
        		TileCell* calibCell_2 = new TileCell(caloDDE, id, Eem, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt_2->push_back(calibCell_2);
		    }

		    if(m_store_NonEm) {
            		TileCell* calibCell_3 = new TileCell(caloDDE, id, Enonem, 0., 0, 0, CaloGain::UNKNOWNGAIN, 0.0, 0.0 );
            	        cnt_3->push_back(calibCell_3);
	    	    }

		    ID.push_back(id);
		    ++tile_nchan;
		    ++m_nchan;
		}
	        else 
		{
                   ATH_MSG_DEBUG( "non-Tile ID in Tile " 
                                  << id.getString() << " sub_calo " << m_caloCell_ID->sub_calo(id)  );
            	    if(m_storeUnknown)
            	    {
                       ATH_MSG_DEBUG("Inactive CalibHit doesn't respect to any TileCell - "
                                     <<"create dummy TileCell to store energy " );

			if(m_store_Tot) {
            		    TileCell* calibCell = new TileCell();
	        	    calibCell->setEnergy(Etot);
    	    	    	    cnt->push_back(calibCell);
			}
			
			if(m_store_Vis) {
        		    TileCell* calibCell_1 = new TileCell();
            	    	    calibCell_1->setEnergy(Evis);
        	    	    cnt_1->push_back(calibCell_1);
			}

			if(m_store_Em) {
            	    	    TileCell* calibCell_2 = new TileCell();
            	    	    calibCell_2->setEnergy(Eem);
            	    	    cnt_2->push_back(calibCell_2);
			}

			if(m_store_NonEm) {
            	    	    TileCell* calibCell_3 = new TileCell();
            	    	    calibCell_3->setEnergy(Enonem);
            	    	    cnt_3->push_back(calibCell_3);
			}

            	        ID.push_back(id);
		        ++tile_unknown_nchan;
            		++m_nchan;
		    }
		}
	    }      

	    if(tile_nchan!=0)
    	    {
        	if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::TILE);
	        if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::TILE);
		if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::TILE);
	        if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::TILE);
	    }

    	    if(tile_unknown_nchan!=0)
    	    {
		if( !(cnt->hasCalo(CaloCell_ID::NOT_VALID)) )
	        {
        	    if(m_store_Tot)   cnt->setHasCalo(CaloCell_ID::NOT_VALID);
            	    if(m_store_Vis)   cnt_1->setHasCalo(CaloCell_ID::NOT_VALID);
            	    if(m_store_Em)    cnt_2->setHasCalo(CaloCell_ID::NOT_VALID);
            	    if(m_store_NonEm) cnt_3->setHasCalo(CaloCell_ID::NOT_VALID);
        	}
	    }
	}
    }

    ATH_MSG_VERBOSE("--- TILE INFO --- "<<m_nchan );
    ATH_MSG_VERBOSE("TileCells = "<<tile_nchan );
    ATH_MSG_VERBOSE("NOT_VALID (Tile) = "<<tile_unknown_nchan );
    ATH_MSG_VERBOSE("ALL CELLS = "<<m_nchan );


/* UNDER DEVELOPING... UNDER DEVELOPING... UNDER DEVELOPING...

    if( (dmHitCnt->Size()) != 0  )
    {
	CaloCalibrationHitContainer::const_iterator it = dmHitCnt->begin();
	CaloCalibrationHitContainer::const_iterator end = dmHitCnt->end();

	for(; it != end; it++) 
	{
	    Identifier id=(*it)->cellID();
            ++m_dm_nchan;
	}
    }
*/

    

////these doesn't work because NOT_VALID LArCells exist !!! 
//    cnt->updateCaloIterators();
//    cnt->order();


    //Record CaloCellContainer-s in the StoreGate.
    //Check the containers name, if it was declared
    //then record it.
    if(m_store_Tot)   ATH_CHECK( evtStore()->record(cnt,   m_caloCell_Tot,   false) );
    if(m_store_Vis)   ATH_CHECK( evtStore()->record(cnt_1, m_caloCell_Vis,   false) );
    if(m_store_Em)    ATH_CHECK( evtStore()->record(cnt_2, m_caloCell_Em,    false) );
    if(m_store_NonEm) ATH_CHECK( evtStore()->record(cnt_3, m_caloCell_NonEm, false) );

    ID.clear();
 
    ATH_MSG_DEBUG("execute() completed successfully" );
    return StatusCode::SUCCESS;
}


/////////////////   FINALIZE   //////////////////////
StatusCode CalibHitToCaloCell::finalize()
{
  ATH_MSG_INFO("finalize() successfully" );
  return StatusCode::SUCCESS;
}


//needed only when developing 
void CalibHitToCaloCell::test_energy(Energy* energy)
{
  ATH_MSG_INFO( "Total   =  "<<(*energy)[0]    <<"  |  "
                << "Visible =  "<<(*energy)[1]    <<"  |  "    
                << "Em      =  "<<(*energy)[2]    <<"  |  " 
                << "NonEm   =  "<<(*energy)[3]     );
}
