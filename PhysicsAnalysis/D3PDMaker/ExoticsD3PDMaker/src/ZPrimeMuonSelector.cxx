/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZPrimeMuonSelector.cxx 651267 2015-03-03 16:40:48Z ssnyder $
/**
 * @file ExoticsD3PDMaker/src/ZPrimeMuonSelector.cxx
 * @author Hulin Wang <Hulin.Wang@cern.ch>
 * @date Jan, 2012
 * @brief select events with >=2 high pT muons.
 */

#include "ZPrimeMuonSelector.h"
#include "AthenaKernel/errorcheck.h"
#include "AnalysisTools/AANTupleParams.h"
#include <iostream>
#include <float.h>

#include "Particle/TrackParticle.h"
#include "CaloEvent/CaloCluster.h"
#include "TrkTrackSummary/TrackSummary.h"

namespace D3PD {

ZPrimeMuonSelector::ZPrimeMuonSelector (const std::string& name,
                    ISvcLocator* svcloc)
  : AthFilterAlgorithm (name, svcloc),
    //m_dumpStoreGate(false),
    m_nEventsProcessed(0),
    m_nEventsSelected(0),
    m_nStacoBefore(0),
    m_nMuidBefore(0),
    m_nStacoAfter(0),
    m_nMuidAfter(0)
{
  declareProperty("inputCollection_staco",  m_inCollKey_staco="", "StoreGate key of the input container/collection" );
  declareProperty("inputCollection_muid",  m_inCollKey_muid="", "StoreGate key of the input container/collection" );
  declareProperty("outputCollection_staco", m_outCollKey_staco="", "StoreGate key of the output container/collection" );
  declareProperty("outputCollection_muid", m_outCollKey_muid="", "StoreGate key of the output container/collection" );

  declareProperty("minNumberPassed", m_minNumberPassed=0, "Min number of selected objects" );
  declareProperty("maxNumberPassed", m_maxNumberPassed=INT_MAX, "Max number of selected objects" );

  declareProperty("ptMin", m_ptMin=-1.0*DBL_MAX, "Min P_T" );
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode ZPrimeMuonSelector::initialize()
{
  m_nEventsProcessed=0;
  m_nEventsSelected=0;
  m_nStacoBefore=0;
  m_nMuidBefore=0;
  m_nStacoAfter=0;
  m_nMuidAfter=0;

  ATH_MSG_INFO ("inCollKey_staco      =" << m_inCollKey_staco);
  ATH_MSG_INFO ("inCollKey_muid      =" << m_inCollKey_muid);
  ATH_MSG_INFO ("outCollKey_staco     =" << m_outCollKey_staco);
  ATH_MSG_INFO ("outCollKey_muid     =" << m_outCollKey_muid);
  ATH_MSG_INFO ("minNumberPassed=" << m_minNumberPassed);
  ATH_MSG_INFO ("maxNumberPassed=" << m_maxNumberPassed);
  ATH_MSG_INFO ("ptMin          =" << m_ptMin);

  return StatusCode::SUCCESS;
}

/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode ZPrimeMuonSelector::execute()
{
  // increase the event counter
  m_nEventsProcessed+=1;

  //  StatusCode
  StatusCode sc = StatusCode::SUCCESS ;

  // Retrieve the input container
  const Analysis::MuonContainer* muonTES_staco;
  sc = evtStore()->retrieve( muonTES_staco, m_inCollKey_staco );
  if( sc.isFailure()  ||  !muonTES_staco ) {
    ATH_MSG_WARNING( "fail to retrieve muon staco Container: " << m_inCollKey_staco);
    return StatusCode::SUCCESS;
  }

  const Analysis::MuonContainer* muonTES_muid;
  sc = evtStore()->retrieve( muonTES_muid, m_inCollKey_muid );
  if( sc.isFailure()  ||  !muonTES_muid ) {
    ATH_MSG_WARNING( "fail to retrieve muon muid Container: " << m_inCollKey_muid);
    return StatusCode::SUCCESS;
  }

  // Create the output container 
  Analysis::MuonContainer* new_muonTES_staco=0;
  if(m_outCollKey_staco!=""){
    new_muonTES_staco=new Analysis::MuonContainer(SG::VIEW_ELEMENTS);
    if ( new_muonTES_staco == NULL ){
      REPORT_MESSAGE (MSG::ERROR) << "fail to create new staco Container";
      return StatusCode::FAILURE;
    }

    sc = evtStore()->record ( new_muonTES_staco, m_outCollKey_staco );
    if ( sc.isFailure() || !new_muonTES_staco ){
      REPORT_MESSAGE (MSG::ERROR) <<  "fail to record new staco Container: " << m_outCollKey_staco;
      return StatusCode::FAILURE;
    }
  }


  Analysis::MuonContainer* new_muonTES_muid=0;
  if(m_outCollKey_muid!=""){
    new_muonTES_muid=new Analysis::MuonContainer(SG::VIEW_ELEMENTS);
    if ( new_muonTES_muid == NULL ){
      REPORT_MESSAGE (MSG::ERROR) << "fail to create new muid Container";
      return StatusCode::FAILURE;
    }

    sc = evtStore()->record ( new_muonTES_muid, m_outCollKey_muid );
    if ( sc.isFailure() || !new_muonTES_muid ){
      REPORT_MESSAGE (MSG::ERROR) <<  "fail to record new muid Container: " << m_outCollKey_muid;
      return StatusCode::FAILURE;
    }
  }

  // loop over staco container
  Analysis::MuonContainer::const_iterator muonItr_staco = muonTES_staco->begin();
  Analysis::MuonContainer::const_iterator muonItrE_staco = muonTES_staco->end();

  unsigned int nGoodMuon_staco=0;
  for(muonItr_staco = muonTES_staco->begin(); muonItr_staco != muonItrE_staco; muonItr_staco++) {
    m_nStacoBefore++;

    bool isGood=false;

    //check pt
    if( (*muonItr_staco)->isCombinedMuon() && (*muonItr_staco)->pt()>m_ptMin )isGood=true;


    ATH_MSG_DEBUG( (*muonItr_staco)->pt() );
    

    //continue if not good
    if(!isGood)continue;

    //passed all selections
    nGoodMuon_staco++;

    // add electron which passes selections
    if ( m_outCollKey_staco != "" ){
      Analysis::Muon* mu_staco = const_cast<Analysis::Muon*> (*muonItr_staco);
      new_muonTES_staco->push_back(mu_staco);      
    }


  }


  // loop over electron container
  Analysis::MuonContainer::const_iterator muonItr_muid = muonTES_muid->begin();
  Analysis::MuonContainer::const_iterator muonItrE_muid = muonTES_muid->end();

  unsigned int nGoodMuon_muid=0;
  for(muonItr_muid = muonTES_muid->begin(); muonItr_muid != muonItrE_muid; muonItr_muid++) {
    m_nMuidBefore++;

    bool isGood=false;

    //check pt
    if( (*muonItr_muid)->isCombinedMuon() && (*muonItr_muid)->pt()>m_ptMin )isGood=true;


    ATH_MSG_DEBUG( (*muonItr_muid)->pt() );


    //continue if not good
    if(!isGood)continue;

    //passed all selections
    nGoodMuon_muid++;

    // add electron which passes selections
    if ( m_outCollKey_muid != "" ){
      Analysis::Muon* mu_muid = const_cast<Analysis::Muon*> (*muonItr_muid);
      new_muonTES_muid->push_back(mu_muid);
    }
  }





  //set new electron container
  if ( m_outCollKey_staco != "" ){
    sc = evtStore()->setConst( new_muonTES_staco );
    if ( sc.isFailure() || !new_muonTES_staco ){
      REPORT_MESSAGE (MSG::ERROR) << "fail to set Staco Container: " << m_outCollKey_staco;
      return sc;
    }
  }

  if ( m_outCollKey_muid != "" ){
    sc = evtStore()->setConst( new_muonTES_muid );
    if ( sc.isFailure() || !new_muonTES_muid ){
      REPORT_MESSAGE (MSG::ERROR) << "fail to set Muid Container: " << m_outCollKey_muid;
      return sc;
    }
  }




  //event check
  if(nGoodMuon_staco>=m_minNumberPassed || nGoodMuon_muid>=m_minNumberPassed){
    setFilterPassed(true);
    m_nEventsSelected++;
    m_nStacoAfter+=nGoodMuon_staco;
    m_nMuidAfter+=nGoodMuon_muid;
  }else{
    setFilterPassed(false);
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode ZPrimeMuonSelector::finalize()
{
  ATH_MSG_INFO( "******Summary******" );
  ATH_MSG_INFO( "Number of processed events:          " << m_nEventsProcessed);
  ATH_MSG_INFO( "Number of    passed events:          " << m_nEventsSelected);
  ATH_MSG_INFO( "Average number of stacos (all) :  " << float(m_nStacoBefore)/m_nEventsProcessed);
  ATH_MSG_INFO( "Average number of muids (all) :  " << float(m_nMuidBefore)/m_nEventsProcessed);
  ATH_MSG_INFO( "Average number of stacos (thin):  " << float(m_nStacoAfter)/m_nEventsSelected);
  ATH_MSG_INFO( "Average number of muids (thin):  " << float(m_nMuidAfter)/m_nEventsSelected);
  ATH_MSG_INFO( "*******************");
  return StatusCode::SUCCESS;
}

}//D3PD
