/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZPrimeElectronSelector.cxx 651267 2015-03-03 16:40:48Z ssnyder $
/**
 * @file ExoticsD3PDMaker/src/ZPrimeElectronSelector.cxx
 * @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
 * @date Jan, 2011
 * @brief select high pt electrons, and filter with electrons.
 */

#include "ZPrimeElectronSelector.h"
#include "AthenaKernel/errorcheck.h"
#include "AnalysisTools/AANTupleParams.h"
#include <iostream>
#include <float.h>

// egamma includes
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egammaParamDefs.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "CaloEvent/CaloCluster.h"
#include "TrkTrackSummary/TrackSummary.h"

namespace D3PD {

ZPrimeElectronSelector::ZPrimeElectronSelector (const std::string& name,
                    ISvcLocator* svcloc)
  : AthFilterAlgorithm (name, svcloc),
    //m_dumpStoreGate(false),
    m_nEventsProcessed(0),
    m_nEventsSelected(0),
    m_nElectronBefore(0),
    m_nElectronAfter(0)
{
  declareProperty("inputCollection",  m_inCollKey="", "StoreGate key of the input container/collection" );
  declareProperty("outputCollection", m_outCollKey="", "StoreGate key of the output container/collection" );

  declareProperty("minNumberPassed", m_minNumberPassed=0, "Min number of selected objects" );
  declareProperty("maxNumberPassed", m_maxNumberPassed=INT_MAX, "Max number of selected objects" );

  declareProperty("ptMin", m_ptMin=1.0*DBL_MAX, "Min P_T" );
  declareProperty("etMin", m_etMin=1.0*DBL_MAX, "Min E_T" );
  declareProperty("clPtMin", m_clPtMin=1.0*DBL_MAX, "Min cluster P_T" );
  declareProperty("reclPtMin", m_reclPtMin=1.0*DBL_MAX, "Min recalculated cluster P_T" );
  declareProperty("nSiHitsMin", m_siHitsMin=4, "Min hits of sct+pixel hits for track selection" );
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode ZPrimeElectronSelector::initialize()
{
  m_nEventsProcessed=0;
  m_nEventsSelected=0;
  m_nElectronBefore=0;
  m_nElectronAfter=0;

  ATH_MSG_INFO ("inCollKey      =" << m_inCollKey);
  ATH_MSG_INFO ("outCollKey     =" << m_outCollKey);
  ATH_MSG_INFO ("minNumberPassed=" << m_minNumberPassed);
  ATH_MSG_INFO ("maxNumberPassed=" << m_maxNumberPassed);
  ATH_MSG_INFO ("ptMin          =" << m_ptMin);
  ATH_MSG_INFO ("etMin          =" << m_etMin);
  ATH_MSG_INFO ("clPtMin        =" << m_clPtMin);
  ATH_MSG_INFO ("reclPtMin      =" << m_reclPtMin);
  ATH_MSG_INFO ("siHitsMin      =" << m_siHitsMin);

  return StatusCode::SUCCESS;
}

/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode ZPrimeElectronSelector::execute()
{
  // increase the event counter
  m_nEventsProcessed+=1;

  //  StatusCode
  StatusCode sc = StatusCode::SUCCESS ;

  // Retrieve the input container
  const ElectronContainer* elContainer;
  sc = evtStore()->retrieve( elContainer, m_inCollKey );
  if( sc.isFailure()  ||  !elContainer ) {
    ATH_MSG_WARNING( "fail to retrieve ElectronContainer: " << m_inCollKey);
    return StatusCode::SUCCESS;
  }

  // Create the output container
  ElectronContainer*newElContainer=0;
  if(m_outCollKey!=""){
    newElContainer=new ElectronContainer(SG::VIEW_ELEMENTS);
    if ( newElContainer == NULL ){
      REPORT_MESSAGE (MSG::ERROR) << "fail to create new ElectronContainer";
      return StatusCode::FAILURE;
    }

    // Reserve memory for half (rounded up) of the input size.
    // This way, the vector gets doubled at most once while
    // the memory stays smaller when no doubling is needed.
    newElContainer->reserve( (int)ceil( elContainer->size()/2.0 ) );
    sc = evtStore()->record ( newElContainer, m_outCollKey );
    if ( sc.isFailure() || !newElContainer ){
      REPORT_MESSAGE (MSG::ERROR) <<  "fail to record ElectronContainer: " << m_outCollKey;
      return StatusCode::FAILURE;
    }
  }

  // loop over electron container
  ElectronContainer::const_iterator elItr  = elContainer->begin();
  ElectronContainer::const_iterator elItrE = elContainer->end();
  unsigned int nGoodEl=0;
  for(; elItr != elItrE; ++elItr) {
    m_nElectronBefore++;

    bool isGood=false;

    //check pt
    if((*elItr)->pt()>m_ptMin)isGood=true;

    //check et
    if((*elItr)->et()>m_etMin)isGood=true;

    //check cluster pt
    const CaloCluster*cluster = (*elItr)->cluster();
    if(cluster->pt()>m_clPtMin)isGood=true;

    //check recalculated cluster pt
    double reclPt=0;
    if(cosh(cluster->eta())!=0)reclPt=cluster->e()/cosh(cluster->eta());
    const Rec::TrackParticle* trackPart = (*elItr)->trackParticle();
    if ( trackPart ){
      const Trk::TrackSummary*ts=trackPart->trackSummary();
      int nSCTHits=ts->get(Trk::numberOfSCTHits);
      int nPixHits=ts->get(Trk::numberOfPixelHits);
      int nSiHits=nSCTHits+nPixHits;
      if(nSiHits>=m_siHitsMin && cosh(trackPart->eta())!=0){
        if(cosh(trackPart->eta())!=0)reclPt=cluster->e()/cosh(trackPart->eta());
      }
    }
    if(reclPt>m_reclPtMin)isGood=true;

    ATH_MSG_DEBUG( (*elItr)->pt() << " " << (*elItr)->et() << " " << (*elItr)->cluster()->pt() << " " << reclPt);
    

    //continue if not good
    if(!isGood)continue;

    //passed all selections
    nGoodEl++;

    // add electron which passes selections
    if ( m_outCollKey != "" )newElContainer->push_back((*elItr));
  }

  //set new electron container
  if ( m_outCollKey != "" ){
    sc = evtStore()->setConst( newElContainer );
    if ( sc.isFailure() || !newElContainer ){
      REPORT_MESSAGE (MSG::ERROR) << "fail to set ElectronContainer: " << m_outCollKey;
      return sc;
    }
  }


  //event check
  if(nGoodEl>=m_minNumberPassed&&nGoodEl<=m_maxNumberPassed){
    setFilterPassed(true);
    m_nEventsSelected++;
    m_nElectronAfter+=nGoodEl;
  }else{
    setFilterPassed(false);
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode ZPrimeElectronSelector::finalize()
{
  ATH_MSG_INFO( "******Summary******" );
  ATH_MSG_INFO( "Number of processed events:          " << m_nEventsProcessed);
  ATH_MSG_INFO( "Number of    passed events:          " << m_nEventsSelected);
  ATH_MSG_INFO( "Average number of electrons (all) :  " << float(m_nElectronBefore)/m_nEventsProcessed);
  ATH_MSG_INFO( "Average number of electrons (thin):  " << float(m_nElectronAfter)/m_nEventsSelected);
  ATH_MSG_INFO( "*******************");
  return StatusCode::SUCCESS;
}

}//D3PD
