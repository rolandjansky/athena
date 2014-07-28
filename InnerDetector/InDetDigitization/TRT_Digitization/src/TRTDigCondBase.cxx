/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigCondBase.h"
#include "TRTDigSettings.h"

#include <cmath>
#include <cstdlib> //Always include this when including cmath!

#include "InDetReadoutGeometry/TRT_DetElementCollection.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"

//helpers for identifiers and hitids:
#include "InDetIdentifier/TRT_ID.h"
#include "InDetSimEvent/TRTHitIdHelper.h"

// For the random numbers.
#include "CLHEP/Random/RandFlat.h"

//________________________________________________________________________________
TRTDigCondBase::TRTDigCondBase( const TRTDigSettings* digset,
				const InDetDD::TRT_DetectorManager* detmgr,
				const TRT_ID* trt_id,
				bool UseArgonStraws,   // added by Sasha for Argon
				bool useConditionsHTStatus, // added by Sasha for Argon
				ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc // added by Sasha for Argon
			      )
  : m_settings(digset), m_detmgr(detmgr), m_id_helper(trt_id),
    m_averageNoiseLevel(-1.0),
    m_crosstalk_noiselevel(-1.0),
    m_crosstalk_noiselevel_other_end(-1.0),
    m_msg ("TRTDigCondBase"),
    m_UseArgonStraws(UseArgonStraws),
    m_useConditionsHTStatus(useConditionsHTStatus),
    m_sumSvc(sumSvc)
{
  m_crosstalk_noiselevel = m_settings->crossTalkNoiseLevel();
  m_crosstalk_noiselevel_other_end = m_settings->crossTalkNoiseLevelOtherEnd();
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTDigCondBase::Constructor" << endreq;
}


//________________________________________________________________________________
float TRTDigCondBase::strawAverageNoiseLevel() const {
  
  if (m_averageNoiseLevel>=0.0) {
    return m_averageNoiseLevel;
  } else {
    std::map<int,StrawState>::const_iterator it(m_hitid_to_StrawState.begin());
    m_averageNoiseLevel = 0.;
    double tmp(0.);//we want double precision for this
    for ( ; it!=m_it_hitid_to_StrawState_End; ++it ) {
      tmp += it->second.noiselevel;
    };
    m_averageNoiseLevel = tmp / totalNumberOfActiveStraws();
    return m_averageNoiseLevel;
  };
}

//________________________________________________________________________________
void TRTDigCondBase::initialize() {

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTDigCondBase::initialize begin" << endreq;

  //id helpers:
  TRTHitIdHelper *hitid_helper(TRTHitIdHelper::GetHelper());

  //We loop through all of the detector elements registered in the manager
  InDetDD::TRT_DetElementCollection::const_iterator it(m_detmgr->getDetectorElementBegin());
  InDetDD::TRT_DetElementCollection::const_iterator itE(m_detmgr->getDetectorElementEnd());

  unsigned int strawcount(0);
  
  for (;it!=itE;++it) {
    const double strawLength((*it)->strawLength()); //Not used until much later..

    const Identifier id((*it)->identify());

    int barrel_endcap, ispos;
    //warning: It is quite likely that ispos should be called isneg,
    //but it is really all rather confusing...
    switch ( m_id_helper->barrel_ec(id) ) {
      case -2:  barrel_endcap = 1; ispos = 1; break;
      case -1:  barrel_endcap = 0; ispos = 1; break;
      case  1:  barrel_endcap = 0; ispos = 0; break;
      case  2:  barrel_endcap = 1; ispos = 0; break;
      default:
        std::cout << "TRTDigitization::TRTDigCondBase::createListOfValidStraws "
		  << "FATAL - identifier problems - skipping detector element!!" << std::endl;
 	continue;
    }

    const int ringwheel(m_id_helper->layer_or_wheel(id));
    int phisector(m_id_helper->phi_module(id));
    const int layer(m_id_helper->straw_layer(id));



    
    for (unsigned int iStraw(0); iStraw <(*it)->nStraws(); ++iStraw) {
      ++strawcount;
      
      const int hitid(hitid_helper->buildHitId( barrel_endcap, ispos, ringwheel, phisector,layer, iStraw));


    //<----------- Cosmic Hack ----------->
      //If cosmic run, and not in one of the relevant barrel modules -
      //we designate the straw as dead.

      if ((m_settings->killEndCap()) && (!(m_id_helper->is_barrel(id)) )) {
	m_deadstraws_hitids.insert( hitid );
	continue;
      } else if ((m_settings->killBarrel()) && ((m_id_helper->is_barrel(id)) )){
	m_deadstraws_hitids.insert( hitid );
	continue;
      }

      if (m_settings->cosmicFlag()) {
	  bool activeincosmic = false;
	  if (barrel_endcap==0) { // Barrel
	    //FIXME (temporary):
	    const int oldcalcmoduleID((hitid >> 10) & 0x0000001F);
	    if (oldcalcmoduleID != phisector) {
	      std::cout << "TRTDigitization::TRTDigCondBase:: ERROR in phisector calculation. OLD="
			<< oldcalcmoduleID<<", NEW="<<phisector
			<< " (please send this output line to kittel@nbi.dk !!!)."<<std::endl;
	      phisector = oldcalcmoduleID;
	    }
	    if (m_settings->cosmicFlag() == 1) {
	      if (phisector == 7 || phisector == 8 || phisector == 23 || phisector == 24)
		activeincosmic = true;
	    } else if (m_settings->cosmicFlag() == 2) {
	      if (phisector == 6 || phisector == 7 || phisector == 22 || phisector == 23)
		activeincosmic = true;
	    } else if (m_settings->cosmicFlag() == 3) {
	      if ( phisector == 6 || phisector == 7 )
		activeincosmic = true;
	    } else if (m_settings->cosmicFlag() == 4) {
	      if (phisector == 6 || phisector == 7)
		activeincosmic = true;
	      else if ((phisector == 22 || phisector == 23)&&ispos==1)
		activeincosmic = true;
	    }
	  }
      	  if (!activeincosmic) {
	    m_deadstraws_hitids.insert( hitid );
	    continue;
	  }
	}

      //<--------- Cosmic Hack End --------->
      //<----------------------------------->

      //<------------- CTB Hack ------------>
      //If CTB, everything not in the positive(negative?) barrel will be declared dead.
      if (m_settings->isCTB()){
	if (!(barrel_endcap == 0 && ispos == 0)) {
	  m_deadstraws_hitids.insert( hitid );
	  continue;
	}
      }
      //<----------- CTB Hack End ---------->
      //<----------------------------------->

      /// get ID of the straw
      Identifier strawId = m_id_helper->straw_id(m_id_helper->barrel_ec(id), phisector, ringwheel, layer, iStraw);
      
      //Get info about the straw conditions:
      bool isdead;
      double noiselevel, relative_noiseamplitude;
      setStrawStateInfo( strawId, strawLength, isdead, noiselevel, relative_noiseamplitude );

      if ( !isdead ) {

	//Create and fill strawstate for straw:
	StrawState strawstate;

	/// Sasha: same for Argon and Xenon straws
 	strawstate.noiselevel = noiselevel;
	//  (These two are later regulated noise code):

	/// Sasha: retrieve straw type (Argon or Xenon)
	bool isArgonStraw = IsArgonStraw(strawId);
	
	/// Sasha: write proper lowthreshold (depends from straw type: Xenon, Argon)
	strawstate.lowthreshold = ( !(hitid & 0x00200000) ) ? m_settings->lowThresholdBar(isArgonStraw) : m_settings->lowThresholdEC(isArgonStraw);

	strawstate.noiseamplitude= relative_noiseamplitude;
	
	//Insert into map:
	m_hitid_to_StrawState[ hitid ] = strawstate;

      } else {
	//Insert into map of active straws
	m_deadstraws_hitids.insert( hitid );
      }
    };
    
  };

  if (strawcount != m_deadstraws_hitids.size() + m_hitid_to_StrawState.size() ) {
    std::cout << "TRTDigitization::TRTDigCondBase::createListOfValidStraws FATAL - Two identical hitID's exists!!";
  };

  m_it_hitid_to_StrawState = m_hitid_to_StrawState.begin();
  m_it_hitid_to_StrawState_End = m_hitid_to_StrawState.end();
  m_it_deadstraws_hitids_End = m_deadstraws_hitids.end();

  //just put it to something:
  m_it_hitid_to_StrawState_Last = m_it_hitid_to_StrawState;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTDigCondBase::initialize end" << endreq;

  if (m_hitid_to_StrawState.empty()) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) <<"TRTDigCondBase::initialize it seems that ALL straws are dead/masked! This wont work." << endreq;
  }

  //just to avoid having an uninitialized iterator hanging around:
  m_all_it_hitid_to_StrawState_previous = m_hitid_to_StrawState.begin();

}

//________________________________________________________________________________
void TRTDigCondBase::resetGetNextNoisyStraw() {
  m_it_hitid_to_StrawState = m_hitid_to_StrawState.begin();
}


//________________________________________________________________________________
bool TRTDigCondBase::getNextNoisyStraw( CLHEP::HepRandomEngine* randengine, int& hitID, float& noiselvl ) {
  for (;m_it_hitid_to_StrawState!=m_it_hitid_to_StrawState_End;++m_it_hitid_to_StrawState) {
    noiselvl = m_it_hitid_to_StrawState->second.noiselevel;
    if ( CLHEP::RandFlat::shoot(randengine, 0.0, 1.0) < noiselvl ) {
      m_it_hitid_to_StrawState++; //Important! if removed, iterator is not incremented in case rand<noiselevel!!!
      hitID = m_it_hitid_to_StrawState->first;
      return true;
    };
  };
  return false;
}


//________________________________________________________________________________
bool TRTDigCondBase::crossTalkNoise( CLHEP::HepRandomEngine* randengine ) {

  const float noise(- m_crosstalk_noiselevel * log(CLHEP::RandFlat::shoot(randengine, 0.0, 1.0)));
  
  if ( CLHEP::RandFlat::shoot(randengine, 0.0, 1.0) < noise ) return true;
  return false;
}

//________________________________________________________________________________
bool TRTDigCondBase::crossTalkNoiseOtherEnd( CLHEP::HepRandomEngine* randengine ) {

  const float noise(- m_crosstalk_noiselevel_other_end * log(CLHEP::RandFlat::shoot(randengine, 0.0, 1.0)));
  
  if ( CLHEP::RandFlat::shoot(randengine, 0.0, 1.0) < noise ) return true;
  return false;
}

//________________________________________________________________________________



void TRTDigCondBase::display (const std::string& msg, int lvl) const
{
  this->msg() << static_cast<MSG::Level>(lvl) << msg << endreq;
}

void TRTDigCondBase::setLvl (int lvl)
{
  msg().setLevel ((MSG::Level)lvl);
}

void TRTDigCondBase::setLvl (const std::string& lvl)
{
  MSG::Level new_lvl = MSG::INFO;
  if (lvl=="debug")          { new_lvl = MSG::DEBUG;
  } else if (lvl=="info")    { new_lvl = MSG::INFO;
  } else if (lvl=="warning") { new_lvl = MSG::WARNING;
  } else if (lvl=="error")   { new_lvl = MSG::ERROR;
  } else {
    msg() << MSG::WARNING << "lvl [" << lvl << "] UNKNOWN !" << endreq;
    return;
  }
  msg().setLevel (new_lvl);
}

//_____________________________________________________________________________
bool TRTDigCondBase::IsArgonStraw(Identifier& TRT_Identifier) const {
  bool isArgonStraw = false;
  if(m_UseArgonStraws){
    if (m_useConditionsHTStatus) {
      if (m_sumSvc->getStatusHT(TRT_Identifier) != TRTCond::StrawStatus::Good) {
        isArgonStraw = true;
      }
    } else {
      isArgonStraw = true;
    }
  }
  return isArgonStraw;
}
