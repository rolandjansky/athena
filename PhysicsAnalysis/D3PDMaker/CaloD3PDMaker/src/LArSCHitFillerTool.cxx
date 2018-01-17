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
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArElecCalib/ILArfSampl.h"

#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <cmath>

#include "LArSCHitFillerTool.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LArSCHitFillerTool::LArSCHitFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<LArHitContainer> (type, name, parent), 
        m_larCablingSvc("LArCablingService"),
	m_caloEtaSelection( false ),
	m_caloPhiSelection( false ),
	m_caloLayerSelection( false ),
	m_caloSelection( false ),
	m_scidtool ("CaloSuperCellIDTool"),
	m_sc_dd_mgr(0)
     {

  declareProperty( "CaloEtaCut", m_etaCut );
  declareProperty( "CaloPhiCut", m_phiCut );
  declareProperty( "CaloLayers", m_caloLayers );
  declareProperty( "CaloDetectors", m_caloNums ); 
  // 1=EMB; 2=EMEC Outer Wheel; 3=EMEC Inner Wheel; 4=HEC; 5=FCAL

  declareProperty ("SCIDTool", m_scidtool,
                   "Offline / supercell mapping tool.");

  book().ignore(); // Avoid coverity warnings.
}


StatusCode LArSCHitFillerTool::initialize()
{
  ServiceHandle<StoreGateSvc> detStore("DetectorStore", name());
  CHECK( detStore.retrieve() ) ;
  CHECK( m_scidtool.retrieve() );
  CHECK( detStore->retrieve (m_sc_dd_mgr, "CaloSuperCellMgr") );
  CHECK( detStore->retrieve (m_sc_idHelper) );
    
  CHECK ( detStore->retrieve(m_emid)   );
  CHECK ( detStore->retrieve(m_fcalid) );
  CHECK ( detStore->retrieve(m_hecid)  );
  CHECK ( detStore->retrieve(m_tileid) );
  CHECK ( detStore->retrieve(m_onlineHelper));

  CHECK ( detStore->retrieve(m_dd_fSampl));

  const unsigned int nSubCalo = static_cast< int >( CaloCell_ID::NSUBCALO );

  //check calo number specified
  m_caloSelection = false;
  if( m_caloNums.size() == 0 ) {
    ATH_MSG_INFO( " *** LArSCHitFillerTool: No calorimeter selection" );
    // return StatusCode::SUCCESS;
  } else if( m_caloNums.size() > nSubCalo ) {
    REPORT_MESSAGE( MSG::FATAL )
      << " More than " << nSubCalo << " calo specified. "
      << "Must be wrong. Stop.";
    return StatusCode::FAILURE;
  } else {
    m_caloSelection = true;
    for( unsigned int index = 0; index < m_caloNums.size(); ++index ) {
      if( m_caloNums[ index ]>= nSubCalo ) {
	REPORT_MESSAGE( MSG::FATAL )
	  << "Invalid calo specification:" 
	  << m_caloNums[index] << "Stop.";
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO( "*** LArSCHitFillerTool:  Select calorimeter " << m_caloNums[ index ] );
      }
    }
  }

  m_caloEtaSelection = false;
  if ( m_etaCut.size() >0 ) {
    if ( m_etaCut.size()== 2 && m_etaCut[0]<m_etaCut[1] ) {
      REPORT_MESSAGE( MSG::INFO ) <<  "*** LArSCHitFillerTool: Eta Selection specified: " << m_etaCut[0] << ", " << m_etaCut[1];
      m_caloEtaSelection=true;
    } else {
      REPORT_MESSAGE( MSG::FATAL ) << " *** LArSCHitFillerTool: Invalid Eta Cut Range specified: Abort!";
      return StatusCode::FAILURE;
    }
  }

  m_caloPhiSelection = false;
  if ( m_phiCut.size() >0 ) {
    if ( m_phiCut.size() == 2 && m_phiCut[0]<m_phiCut[1] ) {
      REPORT_MESSAGE( MSG::INFO ) << "*** LArSCHitFillerTool: Phi Selection specified: "  << m_phiCut[0] << ", " << m_phiCut[1];
      m_caloPhiSelection = true;
    } else {
      REPORT_MESSAGE( MSG::FATAL ) << " *** LArSCHitFillerTool: Invalid Phi Cut Range specified: Abort!";
      return StatusCode::FAILURE;
    }
  }

  m_caloLayerSelection = false;
  if ( m_caloLayers.size()>0 ) {
    if ( m_caloLayers.size()>28 ) {
      REPORT_MESSAGE( MSG::FATAL ) << " *** LArSCHitFillerTool: Invalid No. of Calo Layer selection: " << m_caloLayers.size() << " Abort!";
      return StatusCode::FAILURE;
    }
    for( unsigned int j=0; j<m_caloLayers.size(); j++) {
      if (m_caloLayers[j]>27  ) {
	REPORT_MESSAGE( MSG::FATAL ) << " *** LArSCHitFillerTool: Invalid Calo Layer selection: " << m_caloLayers[j] << " Abort!";
	return StatusCode::FAILURE;
      }
    }
    m_caloLayerSelection = true;
  }


  ATH_MSG_INFO( " *** LArSCHitFillerTool: caloEtaSelection " << m_caloEtaSelection );
  ATH_MSG_INFO( " *** LArSCHitFillerTool: caloPhiSelection " << m_caloPhiSelection );
  ATH_MSG_INFO( " *** LArSCHitFillerTool: caloLayerSelection " << m_caloLayerSelection );

  ATH_MSG_INFO( " *** LArSCHitFillerTool: completed" );
  return StatusCode::SUCCESS;
}

/**
 * @brief Book variables for this block.
 */
StatusCode LArSCHitFillerTool::book()
{
  CHECK( addVariable ("nSC", m_nSC ));
  CHECK( addVariable ("E", m_E));
  CHECK( addVariable ("eta", m_eta));
  CHECK( addVariable ("phi", m_phi));
  CHECK( addVariable ("Eoff", m_Eoff));
  CHECK( addVariable ("Et", m_Et));
  CHECK( addVariable ("fsampl", m_fsampl));
  CHECK( addVariable ("calo", m_calo));
  CHECK( addVariable ("region", m_region));
  CHECK( addVariable ("sampling", m_sampling));
  CHECK( addVariable ("ieta", m_ieta));
  CHECK( addVariable ("jphi", m_jphi));
  //CHECK( addVariable ("hid", m_hashid));
  CHECK( addVariable ("IdCells", m_offlId));
 
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
StatusCode LArSCHitFillerTool::fill (const LArHitContainer& p) {
  
  std::map<int, double> sc_E;

  typedef LArHitContainer CONTAINER;
  CONTAINER::const_iterator f_hit = p.begin();
  CONTAINER::const_iterator l_hit = p.end();
  for( ; f_hit != l_hit; ++f_hit ) {
    const LArHit* hit = ( *f_hit ); 
    const Identifier cell_id = hit->cellID();
    Identifier sc_offl_id = m_scidtool->offlineToSuperCellID (cell_id);
    int hashId = m_sc_idHelper->calo_cell_hash( sc_offl_id) ; 
    if (hashId < 999999)
      sc_E[hashId] += hit->energy();
  }
  

  int nSC = 0;

  std::map<int, double>::const_iterator it = sc_E.begin(); 
  std::map<int, double>::const_iterator it_e = sc_E.end(); 

  for ( ; it!=it_e ;  ++it  ) {
    int hashId = (*it).first; 
    Identifier offlId = m_sc_idHelper->cell_id(hashId) ;
    float fsampl = m_dd_fSampl->FSAMPL(offlId);
    double E = (*it).second; 

    // int pn = m_sc_idHelper->pos_neg(offlId); 
    int calo = m_sc_idHelper->sub_calo(hashId); 
    int region =  m_sc_idHelper->region(offlId); 
    unsigned int lay = m_sc_idHelper->sampling(offlId); 
    int ieta = m_sc_idHelper->eta(offlId); 
    int jphi = m_sc_idHelper->phi(offlId); 
    float feta = m_sc_dd_mgr->get_element(offlId)->eta(); 
    float fphi = m_sc_dd_mgr->get_element(offlId)->phi(); 
    int lardet = calo; 

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
    m_E->push_back( E  );
    m_calo->push_back( calo );
    m_region->push_back( region );
    m_sampling->push_back( lay );
    m_ieta->push_back( ieta );
    m_jphi->push_back( jphi );
    m_offlId->push_back( offlId.get_identifier32().get_compact() );
    m_eta->push_back( feta  );
    m_phi->push_back( fphi );
    double offEnergy = E; 
    if ( fsampl>0. ) {
      offEnergy /= fsampl;
    } else {
      offEnergy = 0;
    }
    m_Eoff->push_back( offEnergy );
    m_Et->push_back( offEnergy/cosh( feta ) );
    m_fsampl->push_back( fsampl );
  }
  *m_nSC = nSC;
  
  return StatusCode::SUCCESS;
  
}
  

} // namespace D3PD

