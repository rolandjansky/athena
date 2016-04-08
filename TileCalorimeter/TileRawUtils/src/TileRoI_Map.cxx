/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawUtils/TileRoI_Map.h" 

#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileCablingService.h" 

#include "StoreGate/StoreGateSvc.h"

// This class converts a Tile offline ID and Online Identifier into a RoI id, 
// 

static const InterfaceID IID_ITileRoI_Map("TileRoI_Map", 1, 0);

const InterfaceID& TileRoI_Map::interfaceID( ) 
{ return IID_ITileRoI_Map; }

// default contructor 
TileRoI_Map::TileRoI_Map( const std::string& type, const std::string& name,
        const IInterface* parent ) 
:  AthAlgTool(type,name,parent), m_print(false)
{
  declareInterface<TileRoI_Map>( this );

  // property 
  declareProperty("Print",m_print);

  return ; 
} 

StatusCode TileRoI_Map::initialize()
{ 
  ATH_CHECK( detStore()->retrieve(m_tileMgr) );
  ATH_CHECK( detStore()->retrieve(m_tileID, "TileID") );
  ATH_CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  // get the pointer to CablingSvc
  m_cablingSvc = TileCablingService::getInstance();

  m_idhash.initialize(m_tileHWID); 

  // intialize the internal map

  /** iterator over all ADC Identifiers */
  std::vector<HWIdentifier>::const_iterator first = m_tileHWID->adc_begin();
  std::vector<HWIdentifier>::const_iterator  last = m_tileHWID->adc_end();

  int nADC = 0;
  for ( ; first!=last; ++first) {
    if ( addEntry(*first) ) ++nADC;
  }

  if (m_print) {
    std::cout << " number of ADCs made in TileRoI_Map " << nADC<< std::endl; 
  }
  
  if(m_print) printEtaPhiRegions(); 

  return StatusCode::SUCCESS; 
}

TileRoI_Map::~TileRoI_Map( ) 
{
}


bool TileRoI_Map::addEntry(const HWIdentifier& adc_hwid)
{

  Identifier adc_id  = m_cablingSvc->h2s_adc_id(adc_hwid);

  if (adc_id.is_valid()) {

    Identifier pmt_id  = m_tileID->pmt_id(adc_id);
    Identifier cell_id = m_tileID->cell_id(adc_id);

    Identifier rId = m_cablingSvc->pmt2tt_id(pmt_id);

    m_offmap[ cell_id ] = rId; // mapping for Cell id
    m_offmap[ pmt_id  ] = rId; // mapping for PMT id
    m_offmap[ adc_id  ] = rId; // mapping for ADC id

    HWIdentifier chan_id = m_tileHWID->channel_id(adc_hwid);
    m_onlmap[ chan_id  ] = rId; // mapping for online channel id
    m_onlmap[ adc_hwid ] = rId; // mapping for online ADC id

    std::cout 
      << " HW id " << m_tileHWID->to_string(adc_hwid)
      << " id " << m_tileID->to_string(adc_id)
      << " tt id " << rId // don't know how to print TT ID
      << std::endl;

    // add entry for RoI to COLL_ID map. 
    // make an Tile frag ID
    int frag = m_tileHWID->frag(chan_id);
    COLL_ID_VEC& v = m_roimap[rId];
    COLL_ID_VEC::const_iterator it = v.begin(); 
    COLL_ID_VEC::const_iterator it_end = v.end(); 
    bool found = false;

    const CaloDetDescrElement* caloDDE = m_tileMgr->get_cell_element(cell_id);
    m_epmap[frag].set(caloDDE->eta(),caloDDE->phi()); 

    for(; it != it_end; ++it)
      if(frag == (*it) )   found = true; 
    if(!found) {
//	std::cout <<" Mapped RoI "<<rId.sample()<<" "<<rId.eta()<<" "
//		  <<rId.phi()<<" to frag "<<std::hex<<frag<<std::dec<<std::endl;
      v.push_back(frag);  
    }
  } else 
  {
    std::cout <<" Failed to get Identifier in TileRoI_Map::add "<< std::endl;
    return false;
  }

  return true;

} 


// Given offline channel ID, find TT ID. 
TileRoI_Map::TT_ID TileRoI_Map::TrigTowerID(const Identifier& id) 
 const throw(TileID_Exception)
{
  OFFMAP::const_iterator it = m_offmap.find(id); 
  if( it==m_offmap.end() ) 
  {
     std::string err(" TileRoI_Map:  not entry for ID ");
     err += m_tileID->to_string(id,-1) ; 
     TileID_Exception except(err,1); 
     throw except; 
  }

  return (*it).second; 

}


// Given online Channel ID, find TT ID. 
TileRoI_Map::TT_ID TileRoI_Map::TrigTowerID(const HWIdentifier& id) 
const throw(TileID_Exception)
{
  ONLMAP::const_iterator it = m_onlmap.find( id ); 
  if(it==m_onlmap.end() ) 
  {
     std::string err(" TileRoI_Map::TrigTowerID(const HWIdentifier&):  no entry for HWID "); 
     err += m_tileHWID->to_string(id,-1) ; 
     TileID_Exception except(err,2); 
     throw except; 
  }

  return (*it).second; 

}

const TileRoI_Map::COLL_ID_VEC& TileRoI_Map::CollectionID
               (const TileRoI_Map::TT_ID & RoIid ) const 
{
   ROIMAP::const_iterator it = m_roimap.find(RoIid); 
   if(it !=m_roimap.end()) {
	return (*it).second; 
   }
   std::cout <<" TileRoI_Map::CollectionID: can not find RoI" << std::endl;
   static COLL_ID_VEC v ; 
   return v; 
}

bool  TileRoI_Map::validId
               (const TileRoI_Map::TT_ID & RoIid ) const 
{
   ROIMAP::const_iterator it = m_roimap.find(RoIid); 
   return it !=m_roimap.end() ; 
}

void TileRoI_Map::printEtaPhiRegions()
{
  // print all Collection ID and eta phi region

  E_P_MAP::const_iterator it = m_epmap.begin(); 
  E_P_MAP::const_iterator it_end = m_epmap.end(); 
 
  std::cout <<" COLLECTION ID , HASHID,  ETAMIN,  ETAMAX, PHIMIN, PHIMAX  "<<std::endl ;

  TileHWID tileHW;

  for(; it!=it_end;++it){
        int frag = (*it).first;
        int hashid = m_idhash(frag); 

        float e1,e2,p1,p2 ;

        // check getEtaPhiRegion method. 
        getEtaPhiRegion(hashid,e1,e2,p1,p2); 

        // print them out.
        std::cout <<std::hex<< frag 
             << "  " <<hashid <<  "  "
             <<e1 << "  " 
             <<e2 << "  "
             <<p1 << "  "
             <<p2 << std::endl; 
  }
        std::cout << std::dec << std::endl;

}

bool TileRoI_Map:: getEtaPhiRegion(unsigned int hashid, 
                      float& etamin, float& etamax,
                      float& phimin, float& phimax ) const

{
  if( (int)hashid >= m_idhash.max() ) return false; 

  int frag = m_idhash.identifier(hashid); 
  E_P_MAP::const_iterator it = m_epmap.find(frag); 
  if(it == m_epmap.end()) return false; 

  etamin = (*it).second.etamin; 
  etamax = (*it).second.etamax; 
  phimin = (*it).second.phimin; 
  phimax = (*it).second.phimax; 

  return true; 

}
