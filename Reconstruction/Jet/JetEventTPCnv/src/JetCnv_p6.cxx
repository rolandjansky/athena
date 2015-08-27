///////////////////////// -*- C++ -*- /////////////////////////////// JetCnv_p6.cxx

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class JetCnv_p6
// Author: R.Seuster<seuster@cern.ch>
///////////////////////////////////////////////////////////////////

// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/NavigableCnv_p2.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "JetEventTPCnv/JetMomentMapCnv_p6.h"

// for origin
#include "VxVertex/VxContainer.h"


// JetEvent includes
#define private public
#define protected public
// because of the NavWrapper, the navigation is now a private datamember of the NavWrapper,
//  inaccessable for us via friendship
#include "JetEvent/Jet.h"
#undef protected
#undef private
#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/JetAssociationBase.h"
#include "JetEvent/JetINav4MomAssociation.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p6.h"
#include "JetEventTPCnv/JetSignalStateCnv.h"

#include "DataModelAthenaPool/DataLinkCnv_p2.h"


#include <vector>
using std::vector;

typedef ElementLinkCnv_p3<ElementLink<VxContainer> > VxLinkCnv_t;

// pre-allocate converters
static VxLinkCnv_t    vxCnv;



typedef NavigableCnv_p2<
  Navigable<INavigable4MomentumCollection,double>, float
           > NavigableCnv_t;

// pre-allocate converters
// static P4ImplPxPyPzECnv_p1   momCnv;
static NavigableCnv_t        navCnv;
//static DataLinkCnv_p2<DataLink<JetMomentMap> >    momentMapCnv;



double JetCnvE(float px,float py, float pz, float m){
  return std::sqrt( px*px + py*py + pz*pz + m*m);
}

void JetCnv_p6::persToTrans( const Jet_p6* pers,
                             Jet* trans, 
                             MsgStream& msg ) 
{
  if(msg.level() == MSG::DEBUG ) msg << MSG::DEBUG << "JetCnv_p6 Loading Jet from persistent state... "  << endreq;
  
  navCnv.persToTrans( &pers->m_nav,      
		      &trans->navigableBase(), msg );
  // store as floats and use compressors for the signal state
  // momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(),  msg );
  //pbsCnv.persToTrans( &pers->m_partBase, &trans->particleBase(), msg);
  trans->m_part.m_dataType = (ParticleDataType::DataType) pers->m_dataType;
  vxCnv.persToTrans( &pers->m_origin, &trans->m_part.m_origin, msg );

  
  trans->m_jetAuthor = pers->m_author;
  trans->m_RoIword =  pers->m_RoIword;
  trans->m_constituentsN =  pers->m_constituentsN;
  trans->m_jetId  = pers->m_jetId ;
  if (trans->m_constituentsN > 10000 ) {
    // then we assume this is non physical. Most probably this means
    // we are reading pre 15.1.0 files were m_constituentsN was not saved
    // and that ROOT is assigning a default value. we reset it to 0
    trans->m_constituentsN = 0;
  }

  

  trans->m_constituentSigState = (P4SignalState::State) pers->m_constituentSigState;

  //trans->setCalE( JetCnvE( pers->m_px ,pers->m_py ,pers->m_pz , pers->m_m ) );
  const float *momentum = (pers->m_momentum);
  trans->setCalPx( momentum[ 0 ] );
  trans->setCalPy( momentum[ 1 ] );
  trans->setCalPz( momentum[ 2 ] );
  trans->setCalE ( momentum[ 3 ] );

  trans->setRawPx( momentum[ 4 ] );
  trans->setRawPy( momentum[ 5 ] );
  trans->setRawPz( momentum[ 6 ] );
  trans->setRawE ( momentum[ 7 ] );

  trans->setCScalePx( momentum[ 8 ] );
  trans->setCScalePy( momentum[ 9 ] );
  trans->setCScalePz( momentum[ 10 ] );
  trans->setCScaleE ( momentum[ 11 ] );

  
  // ALWAYS read at FINAL state.
  trans->setSignalState(P4SignalState::JETFINAL);

  





  //momentMapCnv.persToTrans(&pers->m_moments , &trans->m_momentMapLink, msg);
  
  //
  // The tag info store can contain nulls. The system
  // doesn't deal well with that, so we need to filter them out
  // first. We are dealing with pointers here, not actual objects.
  //
  
  if (pers->m_tagJetInfo.size() == 0) {
    if (trans->m_tagInfoStore != 0) {
      trans->m_tagInfoStore->clear();
    }
  } else {
    if (trans->m_tagInfoStore != 0) {
      delete trans->m_tagInfoStore;
    }
    
    vector<const JetTagInfoBase *> *ptags =
      m_taginfoCnv.createTransient(&(pers->m_tagJetInfo), msg);
    if (ptags != 0) {
      vector<const JetTagInfoBase*> &tags (*ptags);
      for (unsigned int i = 0; i < tags.size(); i++) {
	trans->addInfo(tags[i]);
      }
      delete ptags;
    }
  }
  
  //
  // The tag associations are similar to the tag info store
  // above.
  //
  
  if (pers->m_associations.size() == 0) {
    if (trans->m_assocStore != 0) {
      trans->m_assocStore->clear();
    }
  } else {
    if (trans->m_assocStore != 0) {
      delete trans->m_assocStore;
    }
    trans->m_assocStore = new vector<const JetAssociationBase*> ();
    vector<const JetAssociationBase *> *pass =
      m_tagAssCnv.createTransient(&(pers->m_associations), msg);

    if (pass != 0) {
      vector<const JetAssociationBase *> &ass (*pass);
      vector<const JetAssociationBase *> &store (*trans->m_assocStore);
      for (unsigned int i = 0; i < ass.size(); i++) {
	unsigned int index = ass[i]->m_keyIndex;
	if (index >= store.size()) {
	  store.resize(index+1, 0);
	}
	store[index] = ass[i];
	ass[i] = 0; // Make sure nothing bad happens.
      }
      delete pass;
    }
  }

  // Force PseudoJet pointer to null
  trans->setFastjetPtr(NULL);
  
  if(msg.level() == MSG::DEBUG )   msg << MSG::DEBUG << "Loaded Jet from persistent state [OK]. Final e=" << trans->e()
				       << endreq;
  return;
}




// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

void JetCnv_p6::transToPers( const Jet* trans, 
                             Jet_p6* pers, 
                             MsgStream& msg ) 
{
  if(msg.level() == MSG::DEBUG ) msg << MSG::DEBUG << "JetCnv_p6 writing Jet to persistent state... " << trans->jetAuthor() <<"  id="<<trans->id() << endreq;

  // ALWAYS persistified FINAL state.
  trans->setSignalState(P4SignalState::JETFINAL);

  if( ! s_write0constit) {
    navCnv.transToPers( &trans->navigableBase(), &pers->m_nav, msg );
  }

  // we store the momentum now as floats, plus use signal state compressors
  // momCnv.transToPers( &trans->momentumBase(), &pers->m_momentum, msg );
  //pbsCnv.transToPers( &trans->particleBase(), &pers->m_partBase, msg);
  

  pers->m_author = trans->m_jetAuthor;
  pers->m_RoIword = trans->m_RoIword;  
  pers->m_constituentsN = trans->m_constituentsN;
  pers->m_jetId  = trans->m_jetId ;  

  //std::cout << " JetCnv_p6  id="<< pers->m_jetId << " trans id="<< trans->m_jetId << std::endl;

  //momentMapCnv.transToPers( &trans->m_momentMapLink, &pers->m_moments ,msg);  

  pers->m_dataType = pers->m_dataType;
  vxCnv.transToPers( &trans->m_part.m_origin, &pers->m_origin, msg );

  pers->m_constituentSigState = trans->m_constituentSigState;

  pers->m_momentum[ 0 ] = trans->getCalPx( );
  pers->m_momentum[ 1 ] = trans->getCalPy( );
  pers->m_momentum[ 2 ] = trans->getCalPz( );
  pers->m_momentum[ 3 ] = trans->getCalE( );

  pers->m_momentum[ 4 ] = trans->getRawPx( );
  pers->m_momentum[ 5 ] = trans->getRawPy( );
  pers->m_momentum[ 6 ] = trans->getRawPz( );
  pers->m_momentum[ 7 ] = trans->getRawE( );

  pers->m_momentum[ 8 ] = trans->getCScalePx( );
  pers->m_momentum[ 9 ] = trans->getCScalePy( );
  pers->m_momentum[ 10 ] = trans->getCScalePz( );
  pers->m_momentum[ 11 ] = trans->getCScaleE( );

  
  ///
  /// Sometimes a null pointer works its way into the list. Drop them.
  /// This most frequently happens due to thinning.
  ///
  
  if (trans->m_tagInfoStore != 0) {
    vector<const JetTagInfoBase*> goodTagInfo;
    const vector<const JetTagInfoBase*> &tagInfo(*(trans->m_tagInfoStore));
    for (unsigned int i = 0; i < tagInfo.size(); i++) {
      if (tagInfo[i] != 0) {
        goodTagInfo.push_back(tagInfo[i]);
      }
    }
    m_taginfoCnv.transToPers(&goodTagInfo, &(pers->m_tagJetInfo), msg);
  }
  
  ///
  /// Same logic for the ass store as the tag info store.
  ///
  
  if (trans->m_assocStore != 0) {
    vector<const JetAssociationBase*> goodAssInfo;
    const vector<const JetAssociationBase*> &assInfo(*trans->m_assocStore);
    
    for (unsigned int i = 0; i < assInfo.size(); i++) {
      if( (assInfo[i] != 0) && 
          ( ! bool( dynamic_cast<const JetINav4MomAssociation* >(assInfo[i])) )  ){// don't save JetINav4MomAssociation : we don't have persistant class. THIS IS AN AWFULL HACK
	goodAssInfo.push_back(assInfo[i]);
      }
    }
    
    m_tagAssCnv.transToPers(&goodAssInfo, &pers->m_associations, msg);
  }

  if(msg.level() == MSG::DEBUG ){
    msg << MSG::DEBUG << "Created persistent state of Jet [OK]" << endreq;
  }
  return;
}
