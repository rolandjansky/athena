///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainerCnv_p2.cxx 
// Implementation file for class egDetailContainerCnv_p2
/////////////////////////////////////////////////////////////////// 


#include "egammaEvent/egDetailContainer.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetailContainerCnv_p2.h"

#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/EMConvert.h"
#include "egammaEvent/EMBremFit.h"
#include "egammaEvent/EMTrackFit.h"
#include "egammaEvent/EMErrorDetail.h"

#include <cfloat>

#ifdef MSG_DEBUG
#undef MSG_DEBUG
#endif
#define MSG_DEBUG(log, x) \
     do { if (log.level()<=MSG::DEBUG) { log << MSG::DEBUG << x << endreq; } } while (0)


void egDetailContainerCnv_p2::persToTrans( const egDetailContainer_p2    *pers,
                                           egDetailContainer *trans,
                                           MsgStream                   &msg )
{
  trans->clear();
  egDetailContainer_p2::const_iterator it = pers->begin();

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
          tmpEMTrackMatch->set_parameter(key, egParams[i]);
	}
	else {
	  msg << MSG::ERROR << "Read unknown parameter for "
              << it->m_className << " Index " << key << endreq;
	  continue;
	}
      }
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

void egDetailContainerCnv_p2::transToPers( const egDetailContainer *trans,
                                           egDetailContainer_p2                *pers,
                                           MsgStream                         &msg )
{
    egDetailContainer::const_iterator it = trans->begin();
    for( ; it != trans->end(); ++it ){
      
      egDetail_p2 detail;
      std::vector<float>& egParam       = detail.m_egDetailFloatParams;
      std::vector<unsigned int>& egEnum = detail.m_egDetailEnumParams;

      unsigned int nbOfEgammaParams = (int) egammaParameters::lastEgammaParam;
      // first we need to cast 
      const EMShower     *tmpEMShower     = dynamic_cast<const EMShower*>     (*it);
      const EMTrackMatch *tmpEMTrackMatch = dynamic_cast<const EMTrackMatch*> (*it);
      const EMConvert    *tmpEMConvert    = dynamic_cast<const EMConvert*>    (*it);
      const EMBremFit    *tmpEMBremFit    = dynamic_cast<const EMBremFit*>    (*it);
      const EMTrackFit    *tmpEMTrackFit    = dynamic_cast<const EMTrackFit*>    (*it);
      const EMErrorDetail    *tmpEMErrorDetail    = dynamic_cast<const EMErrorDetail*>    (*it);
      if ( !tmpEMShower      && 
	   !tmpEMTrackMatch  && 
	   !tmpEMConvert     &&
	   !tmpEMBremFit     &&
	   !tmpEMTrackFit    &&
	   !tmpEMErrorDetail  ) {
	msg << MSG::ERROR <<  "Unknown egDetail type will not write its data" << endreq;
	continue;
      }

      double tmpParam;
      if ( tmpEMShower ) {
	MSG_DEBUG(msg, "decoded detail as  " << tmpEMShower->className());
	detail.m_className = tmpEMShower->className();
	for ( unsigned int i=0; i<nbOfEgammaParams; i++ ) {
	  tmpParam = tmpEMShower->parameter( (egammaParameters::ParamDef)i);
	  if ( tmpParam != egammaParameters::EgParamUndefined ) {
	    if (tmpParam > FLT_MAX) tmpParam = FLT_MAX;
	    if (tmpParam < - FLT_MAX) tmpParam = - FLT_MAX;
	    egEnum.push_back(i);
	    egParam.push_back(tmpParam);
	  }
	}
      }
      else if ( tmpEMTrackMatch ) {
	MSG_DEBUG(msg, "decoded detail as  " << tmpEMTrackMatch->className());
	detail.m_className = tmpEMTrackMatch->className();
	for ( unsigned int i=0; i<nbOfEgammaParams; i++ ) {
	  tmpParam = tmpEMTrackMatch->parameter( (egammaParameters::ParamDef)i);
	  if ( tmpParam != egammaParameters::EgParamUndefined ) {
	    if (tmpParam > FLT_MAX) tmpParam = FLT_MAX;
	    if (tmpParam < - FLT_MAX) tmpParam = - FLT_MAX;
	    egEnum.push_back(i);
	    egParam.push_back(tmpParam);
	  }
	}
      }
      else if ( tmpEMConvert ) {
	MSG_DEBUG(msg, "decoded detail as  " << tmpEMConvert->className());
	detail.m_className = tmpEMConvert->className();
	for ( unsigned int i=0; i<nbOfEgammaParams; i++ ) {
	  tmpParam = tmpEMConvert->parameter( (egammaParameters::ParamDef)i);
	  if ( tmpParam != egammaParameters::EgParamUndefined ) {
	    if (tmpParam > FLT_MAX) tmpParam = FLT_MAX;
	    if (tmpParam < - FLT_MAX) tmpParam = - FLT_MAX;
	    egEnum.push_back(i);
	    egParam.push_back(tmpParam);
	  }
	}
      }
      else if ( tmpEMBremFit ) {
	MSG_DEBUG(msg, "decoded detail as  " << tmpEMBremFit->className());
	detail.m_className = tmpEMBremFit->className();
	for ( unsigned int i=0; i<nbOfEgammaParams; i++ ) {
	  tmpParam = tmpEMBremFit->parameter( (egammaParameters::ParamDef)i);
	  if ( tmpParam != egammaParameters::EgParamUndefined ) {
	    if (tmpParam > FLT_MAX) tmpParam = FLT_MAX;
	    if (tmpParam < - FLT_MAX) tmpParam = - FLT_MAX;
	    egEnum.push_back(i);
	    egParam.push_back(tmpParam);
	  }
	}
      }
      else if ( tmpEMTrackFit ) {
	MSG_DEBUG(msg, "decoded detail as  " << tmpEMTrackFit->className());
	detail.m_className = tmpEMTrackFit->className();
	for ( unsigned int i=0; i<nbOfEgammaParams; i++ ) {
	  tmpParam = tmpEMTrackFit->parameter( (egammaParameters::ParamDef)i);
	  if ( tmpParam != egammaParameters::EgParamUndefined ) {
	    if (tmpParam > FLT_MAX) tmpParam = FLT_MAX;
	    if (tmpParam < - FLT_MAX) tmpParam = - FLT_MAX;
	    egEnum.push_back(i);
	    egParam.push_back(tmpParam);
	  }
	}
      }
      else if ( tmpEMErrorDetail ) {
	MSG_DEBUG(msg, "decoded detail as  " << tmpEMErrorDetail->className());
	detail.m_className = tmpEMErrorDetail->className();
	for ( unsigned int i=0; i<nbOfEgammaParams; i++ ) {
	  tmpParam = tmpEMErrorDetail->parameter( (egammaParameters::ParamDef)i);
	  if ( tmpParam != egammaParameters::EgParamUndefined ) {
	    if (tmpParam > FLT_MAX) tmpParam = FLT_MAX;
	    if (tmpParam < - FLT_MAX) tmpParam = - FLT_MAX;
	    egEnum.push_back(i);
	    egParam.push_back(tmpParam);
	  }
	}
      }
 
      pers->push_back( detail );
    }

}
