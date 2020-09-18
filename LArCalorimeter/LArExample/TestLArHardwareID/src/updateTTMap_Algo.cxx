/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "TestLArHardwareID/updateTTMap_Algo.h"

#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/LArTTCell_P.h"
#include "CaloTriggerTool/LArTTCellMap.h"

/********************************************************/
updateTTMap_Algo::updateTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  AthAlgorithm( name , pSvcLocator) ,
  m_dumpMap(false)
{
  declareProperty("dumpMap", m_dumpMap );
}

updateTTMap_Algo::~updateTTMap_Algo()
{
}

// ==============================================================
StatusCode updateTTMap_Algo::initialize(){
// ==============================================================
  ATH_MSG_INFO ( " initializing " );
  return StatusCode::SUCCESS;
}


// ====================================================================================
StatusCode updateTTMap_Algo::execute(){
// ====================================================================================

  ATH_MSG_INFO ( "=> updateTTMap_Algo::Executing " );

  const LArTTCellMap* ttCellMap_c = nullptr;
  ATH_CHECK( detStore()->retrieve( ttCellMap_c ) );
  LArTTCellMap*   ttCellMap = const_cast<LArTTCellMap*>(ttCellMap_c); 
  
  //
  // first read the initial structure
  //
  LArTTCell_P* ttCell_P = ttCellMap->getP(); 
  typedef std::vector<LArTTCell_P::LArTTCell_P_t> VTTCELL; 
  VTTCELL::iterator it   = ttCell_P->m_v.begin(); 

  ATH_MSG_DEBUG(" Initial LArTTCell_P version = "<<ttCell_P->m_version);

  // set version to 1
  ttCell_P->m_version = 1 ;
  ATH_MSG_DEBUG(" Final LArTTCell_P version = "<<ttCell_P->m_version);

  unsigned int nLineI=0;
  unsigned int nLineF=0;
  unsigned int nLineE=0;

  while (it!=ttCell_P->m_v.end()) {
    LArTTCell_P::LArTTCell_P_t& t = *it;    
    ATH_MSG_VERBOSE
      (" det="<<t.det
       <<" pn="<<t.pn 
       <<" sample="<<t.sample
       <<" region="<<t.region
       <<" eta="<<t.eta
       <<" phi="<<t.phi
       <<" trig_pn="<<t.tpn
       <<" trig_samp="<<t.tsample
       <<" trig_reg="<<t.tregion
       <<" trig_eta="<<t.teta
       <<" trig_phi="<<t.tphi
       <<" layer="<<t.layer
       );
    nLineI++;

    if((t.det)==0 && (t.pn)==1 && (t.tregion)==0 && (t.tphi)<4){

      it++;
      nLineF++;

    } else {

      it = ttCell_P->m_v.erase(it);
      nLineE++;
      
    }

  }
  
  VTTCELL::iterator it2   = ttCell_P->m_v.begin(); 
  VTTCELL::iterator it_e2 = ttCell_P->m_v.end(); 
  for (; it2!=it_e2;++it2)       {
    LArTTCell_P::LArTTCell_P_t& t = *it2;    
    ATH_MSG_VERBOSE
      (" kept: det="<<t.det
       <<" pn="<<t.pn 
       <<" sample="<<t.sample
       <<" region="<<t.region
       <<" eta="<<t.eta
       <<" phi="<<t.phi
       <<" trig_pn="<<t.tpn
       <<" trig_samp="<<t.tsample
       <<" trig_reg="<<t.tregion
       <<" trig_eta="<<t.teta
       <<" trig_phi="<<t.tphi
       <<" layer="<<t.layer
       );
  }

  ATH_MSG_DEBUG(" nb of lines in initial struct= " << nLineI );
  ATH_MSG_DEBUG(" nb of lines in final struct= " << nLineF );
  ATH_MSG_DEBUG(" nb of lines erased= " << nLineE );
  
  return StatusCode::SUCCESS ;
}

/********************************************************/
StatusCode updateTTMap_Algo::finalize(){
	
  ATH_MSG_INFO ( " finalizing " );
  return StatusCode::SUCCESS ; 
}
