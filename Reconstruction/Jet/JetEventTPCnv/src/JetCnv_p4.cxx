///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p4.cxx 
// Implementation file for class JetCnv_p4
// Author: R.Seuster<seuster@cern.ch>
///////////////////////////////////////////////////////////////////

// STL includes

// JetEvent includes
#include "JetEvent/Jet.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/JetAssociationBase.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/NavigableCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p4.h"
#include "JetEventTPCnv/JetSignalStateCnv.h"

#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

#include <vector>
using std::vector;

typedef NavigableCnv_p1<
           Navigable<INavigable4MomentumCollection,double>
           > NavigableCnv_t;

// pre-allocate converters
static P4ImplPxPyPzECnv_p1   momCnv;
static NavigableCnv_t        navCnv;
static ParticleBaseCnv_p1 pbsCnv;


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

JetCnv_p4::JetCnv_p4()
  : m_badIndex (false),
    m_nIndexTest (0)
{
}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void JetCnv_p4::persToTrans( const Jet_p4* pers,
                             Jet* trans, 
                             MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Loading Jet from persistent state...  e = "<< pers->m_momentum.m_ene
      << endreq;

  navCnv.persToTrans( &pers->m_nav,      
		      &trans->navigableBase(), msg );
  momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(),  msg );
  pbsCnv.persToTrans( &pers->m_partBase, &trans->particleBase(), msg);
  
  msg << MSG::DEBUG << "    after momCnv e=" << trans->e() <<endreq; 

  trans->m_jetAuthor = pers->m_author;

  //
  // create the store only if non-zero size (waste of space for constituents)
  // Use swap to move the vector from one to the other - this way we
  // don't waste time doing a copy.
  //


   JetKeyDescriptorInstance * keydesc = JetKeyDescriptorInstance::instance();
   std::vector<std::string> momentNames = keydesc->getKeys(JetKeyConstants::ShapeCat);
   if( (pers)->m_shapeStore.size() >0 ){
     if( momentNames.size() < (pers)->m_shapeStore.size() ) {}
       //       if( ! pers->m_usedForTrigger ) { msg << MSG::WARNING << " JetCnv_p4 can't convert moments ! num max keys = "<< momentNames.size() << " persistant jet has n="<< (pers)->m_shapeStore.size() <<endreq; }
     else {
       for(size_t i=0;i<(pers)->m_shapeStore.size();i++){
         trans->setMoment(momentNames[i], (pers)->m_shapeStore[i], true);
       }
     }
   }
  const_cast<Jet_p4*>(pers)->m_shapeStore.clear();

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
        // We want to read in the association objects and stick them
        // into the store vector, at the indices that are stored in the
        // objects.
        //
        // A catch, though, is that in 14.2.0, the indices are not filled
        // in properly --- one gets garbage.  In that case, one can use
        // the setAssociation method to store the association.  We don't
        // want to do that, though, if the index information is available.
        //
        // Further complicating this, there seems to be no reliable way
        // of distinguishing between these cases, at least not without
        // trying to dig the version string out of metadata, which has
        // its own set of issues.
        //
        // Instead, we'll do this.  The first 10 times we go through
        // here, we'll cross-check the saved index against what we
        // compute via getIndex().  If there are any mismatches,
        // we'll revert to using setAssociation from then on.
        // Otherwise, we'll just use the index directly.
        //
        // Here, m_nIndexTest is the number of times we've done
        // the cross-check.  m_badIndex is set to true if we find
        // a mismatch.

        // The stored index.
        unsigned int index = ass[i]->keyIndex();

        // Cross-check against getIndex() for up to the first 10 times.
        if (m_nIndexTest < 10 && !m_badIndex) {
          ++m_nIndexTest;
          if (JetKeyDescriptorInstance::s_instance.getIndex ("JetAssociations",
                                              ass[i]->name()) != index)
            m_badIndex = true;
        }

        // Store the association, either using setAssociation, or
        // directly using the stored index.
        if (m_badIndex)
          trans->setAssociation (ass[i]);
        else {
          if (index >= store.size()) {
            store.resize(index+1, 0);
          }
          store[index] = ass[i];
        }

	ass[i] = 0; // Make sure nothing bad happens.
      }
      delete pass;
    }
  }
  
   // default signal state
   trans->setSignalState(P4SignalState::CALIBRATED);

   SignalStateCnv statecnv;

   trans->setRawE  ( statecnv.ratio_from_char( pers->m_ratioE) * trans->e() );
   trans->setRawPx ( statecnv.ratio_from_char(pers->m_ratioPx) * trans->px());
   trans->setRawPy ( statecnv.ratio_from_char(pers->m_ratioPy) * trans->py());
   trans->setRawPz ( statecnv.ratio_from_char(pers->m_ratioPz) * trans->pz());
   

   msg << MSG::DEBUG << "Loaded Jet from persistent state [OK]"
       << endreq;
   return;
}

void JetCnv_p4::transToPers( const Jet* trans, 
                             Jet_p4* pers, 
                             MsgStream& msg ) 
{
  navCnv.transToPers( &trans->navigableBase(), &pers->m_nav, msg );
  momCnv.transToPers( &trans->momentumBase(), &pers->m_momentum, msg );
  pbsCnv.transToPers( &trans->particleBase(), &pers->m_partBase, msg);

  
  pers->m_author = trans->m_jetAuthor;
  
  // if ( bool(trans->m_shapeStore) )
  //   pers->m_shapeStore  = *(trans->m_shapeStore);
  // else
  //   pers->m_shapeStore.clear();

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
      if (assInfo[i] != 0) {
	goodAssInfo.push_back(assInfo[i]);
      }
    }

    m_tagAssCnv.transToPers(&goodAssInfo, &pers->m_associations, msg);
  }

   SignalStateCnv statecnv;
   trans->setSignalState(P4SignalState::CALIBRATED);

   pers->m_ratioE  = statecnv.char_from_ratio(trans->getRawE()  / trans->e() );
   pers->m_ratioPx = statecnv.char_from_ratio(trans->getRawPx() / trans->px());
   pers->m_ratioPy = statecnv.char_from_ratio(trans->getRawPy() / trans->py());
   pers->m_ratioPz = statecnv.char_from_ratio(trans->getRawPz() / trans->pz());
   


   msg << MSG::DEBUG << "Created persistent state of Jet [OK]  e="<< pers->m_momentum.m_ene
      << endreq;
  return;
}
