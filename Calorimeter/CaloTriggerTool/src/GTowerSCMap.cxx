/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloIdentifier/GTower_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>

GTowerSCMap::GTowerSCMap( ): m_msg(0)
{

	IMessageSvc *msgSvc;
	StatusCode status =Gaudi::svcLocator()->service("MessageSvc",msgSvc);
	if(status.isFailure()){
		std::cout <<  "Cannot locate MessageSvc" << std::endl;
	}
	m_msg = new MsgStream ( msgSvc, "GTowerSCMap");
	msgSvc->release() ;
}


GTowerSCMap::~GTowerSCMap()
{

}


void GTowerSCMap::set( const LArTTCell& m )
{

	convert_to_P(m);

	if (m_msg->level() <= MSG::DEBUG) {
		(*m_msg) <<MSG::DEBUG<<" LArTTCell size = "<<m.size() <<endmsg;
	}
	StoreGateSvc * detStore;
	StatusCode status = Gaudi::svcLocator()->service("DetectorStore",detStore);
	if(status.isFailure()){
		(*m_msg) << MSG::ERROR <<  "Cannot locate DetectorStore" << endmsg;
	}

        const CaloCell_SuperCell_ID* cell_id;
        const Tile_SuperCell_ID* tile_id;

        const GTower_ID* gTower_id;


	status=detStore->retrieve(cell_id);
	if(status.isFailure()){
		(*m_msg) << MSG::ERROR <<  "Cannot retrieve em_id" << endmsg;
	}

        status=detStore->retrieve(tile_id);
        if(status.isFailure()){
                (*m_msg) << MSG::ERROR <<  "Cannot retrieve tile_id" << endmsg;
        }
	
	status=detStore->retrieve(gTower_id);
	if(status.isFailure()){
		(*m_msg) << MSG::ERROR <<  "Cannot retrieve gTower_id" << endmsg;
	}
	LArTTCell::const_iterator it  = m.begin();
	LArTTCell::const_iterator it_e  = m.end();
        unsigned int gT_max = gTower_id->tower_hash_max();
        m_tt2cellIdVec.resize(gT_max);
        unsigned int sc_max = cell_id->calo_cell_hash_max();
        m_cell2ttIdVec.resize(sc_max);

	// useful in debug phase; can be removed afterwards
	std::set<Identifier> cellIdSet;

	try   {
		for (; it!=it_e; ++it)      {
			const LArTTCell_t& t = *it;

			Identifier id ;

			id = cell_id->cell_id(t.det, t.pn,t.sample,t.region,t.eta,t.phi);
                        if (cell_id->is_tile(id)) id = tile_id->cell_id(id);
			Identifier sid = gTower_id->tower_id(t.tpn,t.tsample,t.tregion,t.teta,t.tphi);

			if (m_msg->level() <= MSG::VERBOSE) {
				(*m_msg) <<MSG::VERBOSE
				         << " db struct= "
				         <<" det="<<t.det
				         <<" pn="<<t.pn
				         <<" region="<<t.region
				         <<" sample="<<t.sample
				         <<" eta="<<t.eta<<" phi="<<t.phi
				         <<" trig pn="<<t.tpn
				         <<" trig region="<<t.tregion
				         <<" trig sample="<<t.tsample
				         <<" trig eta="<<t.teta<<" trig phi="<<t.tphi
				         << endmsg;

				(*m_msg) <<MSG::VERBOSE<< " gTower id = " << sid<<" offline id ="<<id<<endmsg;
			}

			if(!(cellIdSet.insert(id)).second) {
				(*m_msg) <<MSG::ERROR<<" Duplicate cell id "
				         << gTower_id->show_to_string(id)
				         << " "
				         << id.get_identifier32().get_compact()
				         << " det=" << t.det << ", pn=" << t.pn << ", region=" << t.region << ", sample=" << t.sample
				         << " in gTower = "
				         << gTower_id->show_to_string(sid)
				         << " "
				         << sid.get_identifier32().get_compact()
				         << endmsg;
			}
                        IdentifierHash gHash= gTower_id->tower_hash(sid);
                        IdentifierHash sHash = cell_id->calo_cell_hash(id);
                        m_cell2ttIdVec[sHash]=sid; //sHash is SC hash, and sid is jTower id
                        m_tt2cellIdVec[gHash].push_back(id);
		}
	}


	catch (LArID_Exception& except)  {
		(*m_msg) <<MSG::ERROR<<" Failed in GTowerSCMap::set " << endmsg;
		(*m_msg) <<MSG::ERROR<< (std::string) except  << endmsg ;
	}

	if (m_msg->level() <= MSG::DEBUG) {
		(*m_msg) <<MSG::DEBUG<<" GTowerSCMap::set : number of cell Ids="<<m_cell2ttIdVec.size()<<std::endl;
	}

	detStore->release() ;

	return;

}

Identifier GTowerSCMap::whichTTID(const int &sHash) const
{
  
        if(sHash<int(m_cell2ttIdVec.size())){

          return m_cell2ttIdVec.at(sHash);
        }

	(*m_msg) <<MSG::ERROR<<" Offline TT ID not found for cell "<< sHash <<endmsg;

	return  Identifier();
}





const std::vector<Identifier>&
GTowerSCMap::createCellIDvec(const int & gHash) const
{
    
	if(gHash<int(m_tt2cellIdVec.size())){
          return m_tt2cellIdVec.at(gHash);            
	}

	if (m_msg->level() <= MSG::VERBOSE) {
		(*m_msg) <<MSG::VERBOSE<<" vector of offline cell ID not found, TT id = " <<gHash<< endmsg;
	}

	static std::vector<Identifier> v;
	return  v ;

}

LArTTCell_P* GTowerSCMap::getP()
{
	return &m_persData;
}

void GTowerSCMap::convert_to_P( const LArTTCell& d )
{

	LArTTCell::const_iterator it = d.begin();
	LArTTCell::const_iterator it_e = d.end();

	for ( ;it!=it_e;++it){
		const LArTTCell_t& t = *it;
		LArTTCell_P::__t t2 ;

		t2.det = t.det;
		t2.pn  = t.pn;
		t2.sample = t.sample;
		t2.region = t.region;
		t2.eta = t.eta;
		t2.phi = t. phi;

		t2.tpn = t.tpn ;
		t2.tsample = t.tsample;
		t2.tregion = t.tregion;
		t2.teta = t.teta;
		t2.tphi = t.tphi;
		t2.layer = t.layer;

		m_persData.m_v.push_back(t2);
	}

}


void GTowerSCMap::convert_to_D( const LArTTCell_P& p,  LArTTCell& d )
{

	std::vector<LArTTCell_P::__t>::const_iterator it = p.m_v.begin();
	std::vector<LArTTCell_P::__t>::const_iterator it_e = p.m_v.end();

	d.clear();
	for ( ;it!=it_e;++it){
		const LArTTCell_P::__t& t = *it;
		LArTTCell_t t2 ;

		t2.det = t.det      ;
		t2.pn  = t.pn       ;
		t2.region = t.region;
		t2.sample = t.sample;
		t2.eta = t.eta      ;
		t2.phi = t.phi     ;

		t2.tpn  = t.tpn       ;
		t2.tregion = t.tregion;
		t2.tsample = t.tsample;
		t2.teta = t.teta      ;
		t2.tphi = t.tphi     ;
		t2.layer = t.layer     ;

		d.push_back(t2);
	}

}

void GTowerSCMap::set (const LArTTCell_P& p)
{
	LArTTCell d ;
	convert_to_D(p,d);
	set(d);
	m_persData.m_version = p.m_version;
	return ;
}
