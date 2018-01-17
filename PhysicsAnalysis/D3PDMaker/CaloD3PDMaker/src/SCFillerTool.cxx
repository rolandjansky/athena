/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 */


#include "AthenaKernel/errorcheck.h"

#include <sstream>

#include "LArCabling/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <cmath>

#include "SCFillerTool.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SCFillerTool::SCFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<CaloCellContainer> (type, name, parent), 
        m_larCablingSvc("LArCablingService"),
	m_tableFilled(false),
	m_caloEtaSelection( false ),
	m_caloPhiSelection( false ),
	m_caloLayerSelection( false ),
	m_caloSelection( false ), 
  	m_tileDLayerOption( false ) {

  declareProperty( "CaloEtaCut", m_etaCut );
  declareProperty( "CaloPhiCut", m_phiCut );
  declareProperty( "CaloLayers", m_caloLayers );
  // 1=EMB; 2=EMEC Outer Wheel; 3=EMEC Inner Wheel; 4=HEC; 5=FCAL; 6=Tile LB; 7=Tile EB
  declareProperty( "CaloDetectors", m_caloNums ); 
  declareProperty( "TileDLayerOption", m_tileDLayerOption);

  book().ignore(); // Avoid coverity warnings.
}

StatusCode SCFillerTool::initialize() {

  ServiceHandle<StoreGateSvc> detStore("DetectorStore", name());
  CHECK( detStore.retrieve() ) ;
    
  //  CHECK ( detStore->retrieve(m_dd_man)   );
  CHECK ( detStore->retrieve(m_emid)   );
  CHECK ( detStore->retrieve(m_fcalid) );
  CHECK ( detStore->retrieve(m_hecid)  );
  CHECK ( detStore->retrieve(m_tileid) );
  CHECK ( detStore->retrieve(m_onlineHelper));

  const unsigned int nSubCalo = 7; // static_cast< int >( CaloCell_ID::NSUBCALO );

  //check calo number specified
  m_caloSelection = false;
  if( m_caloNums.size() == 0 ) {
    ATH_MSG_INFO( " *** SCFillerTool: No calorimeter selection" );
    return StatusCode::SUCCESS;
  } else if( m_caloNums.size() > nSubCalo ) {
    REPORT_MESSAGE( MSG::FATAL )
      << " More than " << nSubCalo << " calo specified. "
      << "Must be wrong. Stop.";
    return StatusCode::FAILURE;
  } else {
    m_caloSelection = true;
    for( unsigned int index = 0; index < m_caloNums.size(); ++index ) {
      if( m_caloNums[ index ]> nSubCalo ) {
	REPORT_MESSAGE( MSG::FATAL )
	  << "Invalid calo specification:" 
	  << m_caloNums[index] << "Stop.";
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO( "*** SCFillerTool:  Select calorimeter " << m_caloNums[ index ] );
      }
    }
  }

  m_caloEtaSelection = false;
  if ( m_etaCut.size() >0 ) {
    if ( m_etaCut.size()== 2 && m_etaCut[0]<m_etaCut[1] ) {
      REPORT_MESSAGE( MSG::INFO ) <<  "*** SCFillerTool: Eta Selection specified: " << m_etaCut[0] << ", " << m_etaCut[1];
      m_caloEtaSelection=true;
    } else {
      REPORT_MESSAGE( MSG::FATAL ) << " *** SCFillerTool: Invalid Eta Cut Range specified: Abort!";
      return StatusCode::FAILURE;
    }
  }

  m_caloPhiSelection = false;
  if ( m_phiCut.size() >0 ) {
    if ( m_phiCut.size() == 2 && m_phiCut[0]<m_phiCut[1] ) {
      REPORT_MESSAGE( MSG::INFO ) << "*** SCFillerTool: Phi Selection specified: "  << m_phiCut[0] << ", " << m_phiCut[1];
      m_caloPhiSelection = true;
    } else {
      REPORT_MESSAGE( MSG::FATAL ) << " *** SCFillerTool: Invalid Phi Cut Range specified: Abort!";
      return StatusCode::FAILURE;
    }
  }

  m_caloLayerSelection = false;
  if ( m_caloLayers.size()>0 ) {
    if ( m_caloLayers.size()>28 ) {
      REPORT_MESSAGE( MSG::FATAL ) << " *** SCFillerTool: Invalid No. of Calo Layer selection: " << m_caloLayers.size() << " Abort!";
      return StatusCode::FAILURE;
    }
    for( unsigned int j=0; j<m_caloLayers.size(); j++) {
      if (m_caloLayers[j]>27  ) {
	REPORT_MESSAGE( MSG::FATAL ) << " *** SCFillerTool: Invalid Calo Layer selection: " << m_caloLayers[j] << " Abort!";
	return StatusCode::FAILURE;
      }
    }
    m_caloLayerSelection = true;
  }

  ATH_MSG_INFO( " *** SCFillerTool: completed" );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode SCFillerTool::book()
{
  CHECK( addVariable ("nSC", m_nSC ));
  CHECK( addVariable ("E", m_E));
  CHECK( addVariable ("eta", m_eta));
  CHECK( addVariable ("phi", m_phi));
  CHECK( addVariable ("Et", m_Et));
  CHECK( addVariable ("calo", m_calo));
  CHECK( addVariable ("region", m_region));
  CHECK( addVariable ("sampling", m_sampling));
  CHECK( addVariable ("ieta", m_ieta));
  CHECK( addVariable ("jphi", m_jphi));
  CHECK( addVariable ("hid", m_hashid));
  CHECK( addVariable ("time", m_time));
  CHECK( addVariable ("quality", m_quality));

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode SCFillerTool::fill (const CaloCellContainer& p) {

   if ( !m_tableFilled ) {
     m_tableFilled = true;
     fillHashTables(p);
     dumpHashTables(p);
     ATH_MSG_INFO("**** after fillHashTables in first event");
   }
   
   resetEnergies();

   typedef CaloCellContainer CONTAINER;
   CONTAINER::const_iterator f_cell = p.begin();
   CONTAINER::const_iterator l_cell = p.end();
   for( ; f_cell != l_cell; ++f_cell ) {
     const CaloCell* cell = ( *f_cell ); 
     const Identifier id = cell->ID();
     int scid = m_sc_hashTable[id];
     m_sc_energy[scid] += cell->energy();
   }

   int nSC = 0;
   std::vector<int>::iterator it;
   it=m_sc_list.begin();
   for ( ; it!=m_sc_list.end(); ++it ) {
     int key = *it;
     //      int sc_hash = (sc_side << 28) | ( sc_calo << 24 ) | (sc_region << 20 ) | ( sc_layer << 16 ) | ( sc_ieta << 8 ) | ( sc_jphi );
     int pn = ((key >> 28) & 0x1) ? 1: -1;
     int calo = ((key >> 24) & 0xf)*pn;
     int region =  (key>>20) & 0xf;
     unsigned int lay = (key>>16) & 0xf;
     int ieta = (key>>8) & 0xff;
     int jphi = key & 0xff;

     float feta = m_sc_eta[key];
     float fphi = m_sc_phi[key];
     int lardet = abs((key >> 24) & 0xf);

     if (m_caloSelection) {
       std::vector< unsigned int >::const_iterator theFound =
	 std::find( m_caloNums.begin(), m_caloNums.end(), lardet );
       if( theFound == m_caloNums.end() ) continue;
     }
     if ( m_caloEtaSelection ) {
       if (feta<m_etaCut[0] || feta>m_etaCut[1]) continue;
     }
     if ( m_caloPhiSelection ) {
       if (fphi<m_phiCut[0] || fphi>m_phiCut[1]) continue;
     }
     if ( m_caloLayerSelection ) {
       bool tmp=false;
       for( unsigned int j=0; j<m_caloLayers.size(); j++) {
	 if (m_caloLayers[j]==lay) tmp=true;
       }
       if (!tmp) continue;
    }
     nSC++;
     m_E->push_back( m_sc_energy[ key ]  );
     m_calo->push_back( calo );
     m_region->push_back( region );
     m_sampling->push_back( lay );
     m_ieta->push_back( ieta );
     m_jphi->push_back( jphi );
     m_hashid->push_back( key );
     m_eta->push_back( feta  );
     m_phi->push_back( m_sc_phi[ key ] );
     m_Et->push_back( m_sc_energy[key]/cosh( feta ) );
   }
  *m_nSC = nSC;

  return StatusCode::SUCCESS;
}
  
void  SCFillerTool::resetEnergies() {
  std::map<int,double>::iterator it;
  it=m_sc_energy.begin();
  for ( ; it!=m_sc_energy.end(); ++it ) {
    (*it).second=0.0;
  }
}

//---------------------------------------------------------------------------------
void SCFillerTool::dumpHashTables( const CaloCellContainer& p ) {
//---------------------------------------------------------------------------------
  typedef CaloCellContainer CONTAINER;
  CONTAINER::const_iterator f_cell = p.begin();
  CONTAINER::const_iterator l_cell = p.end();

  for( ; f_cell != l_cell; ++f_cell ) {
      const CaloCell* cell = ( *f_cell ); 
      Identifier id = cell->ID();
      int posneg = (cell->eta()>=0.0) ? 1: -1;
      int sc_hash = m_sc_hashTable[ id ];
      //      m_sc_compId[sc_hash];
      if ( posneg > 0 ) {
	if ( m_emid->is_lar_em(id) ) {
	  if (m_emid->is_em_barrel(id) ) {
	    int ieta = m_emid->eta(id);
	    int jphi = m_emid->phi(id);
	    int lay = m_emid->sampling(id);
	    // int reg = m_emid->region(id);
	    std::cout << "SCFillerTool::dumpHashTables() [ONLY EM BARREL] ==> posneg,lay,ieta,jphi= " << posneg << ", " << lay << ", " << ieta << ", " << jphi << " ID: " << id << " sc_hash: " << sc_hash << std::endl;
	  }
	}
      }
  }
  std::map< int, std::vector<const CaloCell*> >::iterator it = m_sc_compId.begin();
  for( ; it!=m_sc_compId.end(); ++it) {
    int scID = (*it).first;
    int key = scID;
    std::vector<const CaloCell*> v = (*it).second;
    int nl = v.size();
    std::vector<const CaloCell*>::iterator vit;
    int pn = ((key >> 28) & 0x1) ? 1: -1;
    int calo = ((key >> 24) & 0xf)*pn;
    int region =  (key>>20) & 0xf;
    unsigned int lay = (key>>16) & 0xf;
    int ieta = (key>>8) & 0xff;
    int jphi = key & 0xff;
    if (abs(calo)==1 && pn>0) {
      std::cout << "SCFillerTool::dumpHashTables() ===> scID = " << scID << " calo,pn,region,lay,ieta,jphi: " << calo << ", " << pn << ", " << region  << ", " << lay << ", " << ieta << ", " << jphi << std::endl;
      std::cout << "SCFillerTool::dumpHashTables() ===> scID = " << scID << " No. of cells: " << nl << std::endl;
      std::cout << "SCFillerTool::dumpHashTables() ===>        " ;
      for( vit=v.begin(); vit!= v.end(); vit++) {
	std::cout << *vit << ", " ;
      }
      std::cout << "" << std::endl;
    }
  }
}

//---------------------------------------------------------------------------------
void SCFillerTool::fillHashTables( const CaloCellContainer& p ) {
//---------------------------------------------------------------------------------
  typedef CaloCellContainer CONTAINER;
  CONTAINER::const_iterator f_cell = p.begin();
  CONTAINER::const_iterator l_cell = p.end();

  int /*posneg,*/reg,lay,ieta,jphi;
  int sc_side, sc_calo, sc_region, sc_layer, sc_ieta, sc_jphi;
  //float sc_feta, sc_fphi;

  for( ; f_cell != l_cell; ++f_cell ) {
      const CaloCell* cell = ( *f_cell ); 
      Identifier id = cell->ID();
      //posneg = (cell->eta()>=0.0) ? 1: -1;
      sc_side = (cell->eta()>=0.0) ? 1: 0;
      sc_ieta = -999;
      sc_jphi = -999;
      //sc_feta = 0.;
      //sc_fphi = 0.;
      sc_calo = -1;
      sc_region = -1;
      sc_layer = -1;

      ieta = -999;
      jphi = -999;

      if ( m_emid->is_lar_em(id) ) {
	ieta = m_emid->eta(id);
	jphi = m_emid->phi(id);
	lay = m_emid->sampling(id);
	reg = m_emid->region(id);
	sc_region = reg;
	sc_layer = lay;
	if (m_emid->is_em_barrel(id) ) {
	  sc_calo = 1;
	  if ( lay == 0 ) {
	    sc_ieta = int(ieta/4);
	    sc_jphi = jphi;
	  } else if ( lay == 1 ) {
	    sc_ieta = (reg==0) ? int(ieta/8) : 56+ieta;
	    sc_jphi = (reg==0) ? jphi: int(jphi/4);
	  } else if ( lay == 2 ) { 
	    sc_ieta = (reg==0) ? ieta: 56 + ieta;
	    sc_jphi = int(jphi/4);
	  } else if ( lay == 3 ) { 
	    sc_ieta = int(ieta/2); 
	    sc_jphi = int(jphi/4);
	  }
	} else if ( m_emid->is_em_endcap_outer(id) ) {
	  sc_calo = 2;
	  if ( lay == 0 ) {
	    sc_ieta = int(ieta/4);
	    sc_jphi = jphi;
	    sc_region = 1;
	  } else if (lay == 1) {
	    //	    sc_ieta = (reg<=1) ? 0 : ((reg==2) ? 1+int(ieta/8) : ((reg==3) ? 13+int(ieta/8) : ((reg==4) ? 19+int(ieta/4): 35)));
	  sc_ieta = (reg<=1) ? 0 : ((reg==2) ? 1+int(ieta/8) : ((reg==3) ? 13+int(ieta/4) : ((reg==4) ? 25+int(ieta/4): 41)));
	    sc_jphi = jphi;
	    sc_region = (reg<2) ? 0: reg-1;
	  } else if (lay==2) {
	    sc_ieta = (reg==0) ? 0 : 1+ieta;  
	    sc_jphi = int(jphi/4);
	    sc_region = (ieta<3) ? 0: ((ieta<15) ? 1 : ((ieta<23) ? 2 : ((ieta<39) ? 3 : 4)));
	  } else if (lay==3) {
	    sc_ieta = int(ieta/2); 
	    sc_jphi = int(jphi/4);
	    sc_region = (ieta<6) ? 1: ((ieta<10) ? 2 : ((ieta<18) ? 3: 4));
	  } else {
	    sc_ieta = -999;
	    sc_jphi = -999;
	    sc_region = -999;
	  }
	} else if ( m_emid->is_em_endcap_inner(id) ) {
	  sc_calo = 3;
	  sc_ieta =  (ieta<6) ? int(ieta/2) : 47;
	  sc_jphi =  int(jphi/2);
	  sc_region = (ieta<6) ? 5 : 6;
	} 
      } else if (m_hecid->is_lar_hec(id)) {
	sc_layer  = m_hecid->sampling(id);
	sc_region = m_hecid->region(id);
	sc_calo= 4;
	ieta = m_hecid->eta(id);
	jphi = m_hecid->phi(id);
	sc_ieta = (sc_region==0) ? ieta : 10+ieta; 
	sc_jphi = jphi;
	sc_region = 0;
      } else if (m_fcalid->is_lar_fcal(id)) {
	HWIdentifier hwid = m_larCablingSvc->createSignalChannelID( id );
	sc_calo = 5;
	const int lay = m_fcalid->module(id);
	const int s = m_onlineHelper->slot(hwid);
	const int c  = m_onlineHelper->channel(hwid);
	const int c64 = c%64;
	sc_layer = lay;
	if (lay==3) {
	  sc_ieta = 4-int((c%16)/4);
	  int jphip = 8*(s-14) + 3 + 4*int(c/64)-int(c64/16);
	  int jphin = 8*(s-14) + 7 - int(c/16);
	  sc_jphi = (sc_side)>0 ? jphip : jphin;
	  sc_region = 0;
	} else if (lay==2) {
	  sc_ieta = ((c%32)<16 ? 4 : 8) - int((c%32)/4);
	  int jphip = 4*(s-10) + 1 + 2*int(c/64)-int(c64/32);
	  int jphin =  ((s>=12) ? 14 -4*(s-12): 6 -4*(s-10) ) + int(c64/32) -2*int(c/64);
	  sc_jphi = (sc_side>0) ? jphip : jphin;
	  sc_region = 0;
	} else if (lay==1) {
	  ieta  = 15-int(c64%16) + 16*int(c64/16);
	  sc_ieta = int(ieta/4);
	  int jphip = int(c/64) + 2*(s- ((s==9) ? 2 : 1));
	  int jphin = ((s<5) ? 7 - 2*(s-1) : 15 - 2*(s-((s==9)?6:5))) - int(c/64);
	  sc_jphi = (sc_side>0) ? jphip : jphin;
	  if (c64<16) {
	    sc_region = 0;
	  } else if (c64>=16 && c64<32 ) {
	    sc_region = 1;
	  } else if (c64>=32 && c64<48 ) {
	    sc_region = 2;
	  } else {
	    sc_region = 3;
	  }
	}
      } else if (m_tileid->is_tile(id) ) {
	sc_region = 0;
	sc_layer = m_tileid->sampling(id);
	sc_side = (m_tileid->side(id)>0) ? 1 : 0;
	ieta = m_tileid->tower(id);
	jphi = m_tileid->module(id);
	sc_ieta = ieta;
	sc_jphi = jphi;
	if ( m_tileid->is_tile_barrel(id) )
	  sc_calo = 6;
	if ( m_tileid->is_tile_extbarrel(id) )
	  sc_calo = 7;
      }
      if( sc_ieta<0 || sc_jphi<0 || sc_calo<=0 || sc_region<0 || sc_layer<0 ) continue;
      // This is a kludge so that for HEC/Tile (sc_calo=4,6,7) the SC hash id don't have any layer information...(11/27/12)
      if (sc_calo==4)
	sc_layer = 0;
      if (sc_calo==6 || sc_calo==7) {
	if ( sc_layer!=2 || !m_tileDLayerOption)  
	  sc_layer = 0; 
      }
      int sc_hash = (sc_side << 28) | ( sc_calo << 24 ) | (sc_region << 20 ) | ( sc_layer << 16 ) | ( sc_ieta << 8 ) | ( sc_jphi );
      m_sc_hashTable[ id ] =  sc_hash;
      m_sc_compId[sc_hash].push_back( cell );
  }

  std::map< int, std::vector<const CaloCell*> >::iterator it = m_sc_compId.begin();
  for( ; it!=m_sc_compId.end(); ++it) {
    int scID = (*it).first;
    m_sc_list.push_back( scID );
    std::vector<const CaloCell*> v = (*it).second;
    int nl = v.size();
    std::vector<const CaloCell*>::iterator vit;
    /*
    if ( ((scID >> 24) & 0xf) >= 6 ) {
      std::cout << "*** KAKKA TILECAL: THIS IS A TILECAL SCELL hash= " << std::hex << scID << " no. of cells: " << nl << std::endl;
      for( vit=v.begin(); vit!=v.end(); ++vit) {
	std::cout  << " " << (*vit)->ID() << " ";
      }
      std::cout  << "" << std::endl;
      for( vit=v.begin(); vit!=v.end(); ++vit) {
	std::cout  << " " << (*vit)->eta() << " ";
      }
      std::cout  << "" << std::endl;
      for( vit=v.begin(); vit!=v.end(); ++vit) {
	std::cout  << " " << (*vit)->phi() << " ";
      }
      std::cout  << "" << std::endl;
    }
    */
    double feta = 0;
    double fphi = 0;
    for( vit=v.begin(); vit!=v.end(); ++vit) {
      feta += (*vit)->eta();
      fphi += (*vit)->phi();
    }
    if (nl>0) {
      feta /= nl;
      fphi /= nl;
    }
    m_sc_eta[ scID ] = feta;
    m_sc_phi[ scID ] = fphi;
  } // for (; it!=m_sc_compId...)


}


} // namespace D3PD

