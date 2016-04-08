///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainerCnv_p1.cxx 
// Implementation file for class egDetailContainerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


#include "egammaEvent/egDetailContainer.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetailContainerCnv_p1.h"

#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/EMConvert.h"
#include "egammaEvent/EMBremFit.h"
#include "egammaEvent/EMTrackFit.h"
#include "egammaEvent/EMErrorDetail.h"

#ifdef MSG_DEBUG
#undef MSG_DEBUG
#endif
#define MSG_DEBUG(log, x) \
     do { if (log.level()<=MSG::DEBUG) { log << MSG::DEBUG << x << endreq; } } while (0)

void egDetailContainerCnv_p1::persToTrans( const egDetailContainer_p1    *pers,
                                           egDetailContainer *trans,
                                           MsgStream                   &msg )
{
  trans->clear();
  egDetailContainer_p1::const_iterator it = pers->begin();

  for( ; it != pers->end(); ++it ){

    const std::vector<float>& egParams = it->m_egDetailFloatParams;
    const std::vector<unsigned int>& egEnum = it->m_egDetailEnumParams;
    // if you crash here, then there is a data corruption...
    assert( egEnum.size() == egParams.size() );
  
    if ( it->m_className == "EMShower" ) {
      
      EMShower     *tmpEMShower     = new EMShower();
      for ( unsigned int i=0; i< egEnum.size(); i++ ) {
	if ( tmpEMShower->hasParameter( (egammaParameters::ParamDef) egEnum[i]) ) {
	  tmpEMShower->set_parameter((egammaParameters::ParamDef) egEnum[i],egParams[i]);
	}
	else {
	  msg << MSG::ERROR << "Read unknown parameter for " << it->m_className 
	      << " Index " << egEnum[i] << endreq;
	  continue;
	}
      }
      tmpEMShower->set_linkIndex(0);
      trans->push_back(tmpEMShower);
      
    } 
    else if ( it->m_className == "EMTrackMatch" ) {
      
      EMTrackMatch *tmpEMTrackMatch = new EMTrackMatch();
      for ( unsigned int i=0; i< egEnum.size(); i++ ) {
        egammaParameters::ParamDef key =
          static_cast<egammaParameters::ParamDef> (egEnum[i]);
        if (key == egammaParameters::EoverP) continue;
        if (key == egammaParameters::EtaCorrMag)
          key = egammaParameters::etaSampling1;
	if ( tmpEMTrackMatch->hasParameter( key ) ) {
          tmpEMTrackMatch->set_parameter(key,egParams[i]);
	}
	else {
	  msg << MSG::ERROR << "Read unknown parameter for " << it->m_className 
	      << " Index " << key << endreq;
	  continue;
	}
      }
      tmpEMTrackMatch->set_linkIndex(0);
      trans->push_back(tmpEMTrackMatch);
      
    }
    else if ( it->m_className == "EMConvert" ) {
      
      EMConvert    *tmpEMConvert    = new EMConvert();
      for ( unsigned int i=0; i< egEnum.size(); i++ ) {
	if ( tmpEMConvert->hasParameter( (egammaParameters::ParamDef) egEnum[i]) ) {
	  tmpEMConvert->set_parameter((egammaParameters::ParamDef) egEnum[i],egParams[i]);
	}
	else {
	  msg << MSG::ERROR << "Read unknown parameter for " << it->m_className 
	      << " Index " << egEnum[i] << endreq;
	  continue;
	}
      }
      tmpEMConvert->set_linkIndex(0);
      trans->push_back(tmpEMConvert);

    }
    else if ( it->m_className == "EMBremFit" ) {
      
      EMBremFit    *tmpEMBremFit    = new EMBremFit();
      for ( unsigned int i=0; i< egEnum.size(); i++ ) {
	if ( tmpEMBremFit->hasParameter( (egammaParameters::ParamDef) egEnum[i]) ) {
	  tmpEMBremFit->set_parameter((egammaParameters::ParamDef) egEnum[i],egParams[i]);
	}
	else {
	  msg << MSG::ERROR << "Read unknown parameter for " << it->m_className 
	      << " Index " << egEnum[i] << endreq;
	  continue;
	}
      }
      tmpEMBremFit->set_linkIndex(0);
      trans->push_back(tmpEMBremFit);
    }
    else if ( it->m_className == "EMTrackFit" ) {
					
	EMTrackFit    *tmpEMTrackFit    = new EMTrackFit();
	for ( unsigned int i=0; i< egEnum.size(); i++ ) {
       	  if ( tmpEMTrackFit->hasParameter( (egammaParameters::ParamDef) egEnum[i]) ) {
	   tmpEMTrackFit->set_parameter((egammaParameters::ParamDef) egEnum[i],egParams[i]);
          } 	
          else {
            msg << MSG::ERROR << "Read unknown parameter for " << it->m_className 
		<< " Index " << egEnum[i] << endreq;
	    continue;
          }
	}
	tmpEMTrackFit->set_linkIndex(0);
        trans->push_back(tmpEMTrackFit);

    }
    else if ( it->m_className == "EMErrorDetail" ) {
					
	EMErrorDetail    *tmpEMErrorDetail    = new EMErrorDetail();
	for ( unsigned int i=0; i< egEnum.size(); i++ ) {
       	  if ( tmpEMErrorDetail->hasParameter( (egammaParameters::ParamDef) egEnum[i]) ) {
	   tmpEMErrorDetail->set_parameter((egammaParameters::ParamDef) egEnum[i],egParams[i]);
          } 	
          else {
            msg << MSG::ERROR << "Read unknown parameter for " << it->m_className 
		<< " Index " << egEnum[i] << endreq;
	    continue;
          }
	}
	tmpEMErrorDetail->set_linkIndex(0);
        trans->push_back(tmpEMErrorDetail);

    }
    else {
      msg << MSG::ERROR << "Read unknown egDetail type: " << it->m_className << endreq;
      continue;
    }
    //  trans = (dynamic_cast<EMShower*>(trans));
    
  }

  MSG_DEBUG(msg, "finished egDetailContainer perstoTrans");
}


void egDetailContainerCnv_p1::transToPers( const egDetailContainer* /*trans*/,
                                           egDetailContainer_p1* /*pers*/,
                                           MsgStream& /*msg*/)
{
  throw GaudiException("Deprecated method, we have evolved to use egDetail_p2.Please migrate to use egDetailContainerCnv_p2::transToPers","egDetailContainerCnv_p1::transToPers", StatusCode::FAILURE);
  return;
}
