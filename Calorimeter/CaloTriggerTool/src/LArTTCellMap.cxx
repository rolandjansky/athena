/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTriggerTool/LArTTCellMap.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>

LArTTCellMap::LArTTCellMap( )
  : AthMessaging (Athena::getMessageSvc(), "LArTTCellMap")
{
}


LArTTCellMap::~LArTTCellMap()
{

}


void LArTTCellMap::set( const LArTTCell& m )
{

  convert_to_P(m);

  ATH_MSG_DEBUG( " LArTTCell size = "<<m.size() );

  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "LArTTCellMap");

  const LArEM_ID* em_id = nullptr;
  const LArHEC_ID* hec_id = nullptr;
  const LArFCAL_ID* fcal_id = nullptr;
  const CaloLVL1_ID* lvl1_id = nullptr;

  StatusCode status=detStore->retrieve(em_id);
  if(status.isFailure()){
    ATH_MSG_ERROR( "Cannot retrieve em_id" );
  }
  status=detStore->retrieve(hec_id);
  if(status.isFailure()){
    ATH_MSG_ERROR( "Cannot retrieve hec_id" );
  }
  status=detStore->retrieve(fcal_id);
  if(status.isFailure()){
    ATH_MSG_ERROR( "Cannot retrieve fcal_id" );
  }
  status=detStore->retrieve(lvl1_id);
  if(status.isFailure()){
    ATH_MSG_ERROR( "Cannot retrieve lvl1_id" );
  }
  LArTTCell::const_iterator it  = m.begin();
  LArTTCell::const_iterator it_e  = m.end();

  // useful in debug phase; can be removed afterwards
  std::set<Identifier> cellIdSet;

  try   {
    for (; it!=it_e; ++it)      {
      const LArTTCell_t& t = *it;

      Identifier id ;

      if(t.det==0){
	// EM
	id = em_id->channel_id(t.pn,t.sample,t.region,t.eta,t.phi);
      } else      if(t.det==1){
	// HEC
	id = hec_id->channel_id(t.pn,t.sample,t.region,t.eta,t.phi);
      } else      if(t.det==2){
	// FCAL sample==region
	id = fcal_id->channel_id(t.pn,t.region,t.eta,t.phi);

      } else        {
	ATH_MSG_ERROR( " Wrong input Detector Number " << t.det );
      }

      Identifier sid = lvl1_id->layer_id(t.tpn,t.tsample,t.tregion,t.teta,t.tphi,t.layer);

      ATH_MSG_VERBOSE( " db struct= "
                       <<" det="<<t.det
                       <<" pn="<<t.pn
                       <<" region="<<t.region
                       <<" sample="<<t.sample
                       <<" eta="<<t.eta<<" phi="<<t.phi
                       <<" trig pn="<<t.tpn
                       <<" trig region="<<t.tregion
                       <<" trig sample="<<t.tsample
                       <<" trig eta="<<t.teta<<" trig phi="<<t.tphi
                       <<" layer="<<t.layer );
      ATH_MSG_VERBOSE( " lvl1 id = " << sid<<" offline id ="<<id );

      if(!(cellIdSet.insert(id)).second) {
        ATH_MSG_ERROR( " Duplicate cell id "
                       << lvl1_id->show_to_string(id)
                       << " in TT= "
                       << lvl1_id->show_to_string(sid) );
      }
      m_cell2ttIdMap[id] = sid;

      std::map<Identifier,std::vector<Identifier> >::const_iterator it_find = m_tt2cellIdMap.find(sid);
      if(it_find==m_tt2cellIdMap.end()) {
		// a vector of Ids does not already exist for this sid, we reserve the number of elements
		int nElements = 1;

		if(t.tsample==0) {
			if(t.tregion==0) {
				if(t.layer==0) {
					nElements = 4;
				} else if(t.layer==1) {
					nElements = 32;
				}  else if(t.layer==2) {
					nElements = 16;
				}  else if(t.layer==3) {
					nElements = 8;
				}

			} else if(t.tregion==1) {
				nElements = 4;
			} else if(t.tregion==2) {
				nElements = 2;
			} else if(t.tregion==3) {
				nElements = 16;
			}

		} else if(t.tsample==1) {
			if(t.tregion==3) {
				if(t.layer==0) {
					nElements = 8;
				} else if(t.layer==1) {
					nElements = 4;
				}
			}
		}
		m_tt2cellIdMap[sid].reserve(nElements);
      }
      m_tt2cellIdMap[sid].push_back(id);
    }
  }


  catch (LArID_Exception& except)  {
    ATH_MSG_ERROR( " Failed in LArTTCellMap::set " );
    ATH_MSG_ERROR( (std::string) except  );
  }

  ATH_MSG_DEBUG( " LArTTCellMap::set : number of cell Ids="<<m_cell2ttIdMap.size() );

  detStore->release() ;

  return;

}

Identifier LArTTCellMap::whichTTID(const Identifier& id) const
{

 std::map<Identifier,Identifier>::const_iterator it =m_cell2ttIdMap.find(id);

 if(it!=m_cell2ttIdMap.end()){
	return (*it).second;
 }

 ATH_MSG_ERROR( " Offline TT ID not found for cell "<< id );

 return  Identifier();

}





const std::vector<Identifier>&
LArTTCellMap::createCellIDvec(const Identifier & sid) const
{

 std::map<Identifier,std::vector<Identifier> >::const_iterator
	it=m_tt2cellIdMap.find(sid);

 if(it!=m_tt2cellIdMap.end()){
	return (*it).second;
 }

 ATH_MSG_VERBOSE( " vector of offline cell ID not found, TT id = " <<sid.get_compact() );

 static const std::vector<Identifier> v;
 return  v ;

}

LArTTCell_P* LArTTCellMap::getP()
{
  return &m_persData;
}

void LArTTCellMap::convert_to_P( const LArTTCell& d )
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


void LArTTCellMap::convert_to_D( const LArTTCell_P& p,  LArTTCell& d )
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

void LArTTCellMap::set (const LArTTCell_P& p)
{
  LArTTCell d ;
  convert_to_D(p,d);
  set(d);
  m_persData.m_version = p.m_version;
  return ;
}
