///////////////////////// -*- C++ -*- /////////////////////////////// JetCnv_p5.cxx

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class JetCnv_p5
// Author: R.Seuster<seuster@cern.ch>
///////////////////////////////////////////////////////////////////

// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/NavigableCnv_p1.h"

#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

// JetEvent includes
#include "JetEvent/Jet.h"
#include "JetEvent/JetMomentMap.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/JetAssociationBase.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p5.h"
#include "JetEventTPCnv/JetSignalStateCnv.h"

#include <vector>
using std::vector;



typedef NavigableCnv_p1<
           Navigable<INavigable4MomentumCollection,double>
           > NavigableCnv_t;

// pre-allocate converters
// static const P4ImplPxPyPzECnv_p1   momCnv;
static const NavigableCnv_t        navCnv;
static const ParticleBaseCnv_p1 pbsCnv;

void JetCnv_p5::persToTrans( const Jet_p5* pers,
                             Jet* trans, 
                             MsgStream& msg ) const
{
  if(msg.level() == MSG::DEBUG ) msg << MSG::DEBUG << "Loading Jet from persistent state... "  << endmsg;
  
  navCnv.persToTrans( &pers->m_nav,      
		      &trans->navigableBase(), msg );
  // store as floats and use compressors for the signal state
  // momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(),  msg );
  pbsCnv.persToTrans( &pers->m_partBase, &trans->particleBase(), msg);

  trans->m_jetAuthor = pers->m_author;

  if(pers->m_usedForTrigger) {
    trans->m_RoIword = pers->m_recoStatus;
    trans->m_constituentSigState = Jet::statename_t::UNKNOWN;
  } else {
    trans->m_RoIword = 0xFFFFFFFF;
    trans->m_constituentSigState = pers->m_recoStatus & 0x1 ? Jet::statename_t::CALIBRATED : Jet::statename_t::UNCALIBRATED;
  }
  
  trans->m_constituentsN =  pers->m_constituentsN;
  if (trans->m_constituentsN > 10000 ) {
    // then we assume this is non physical. Most probably this means
    // we are reading pre 15.1.0 files were m_constituentsN was not saved
    // and that ROOT is assigning a default value. we reset it to 0
    trans->m_constituentsN = 0;
  }


  // Make sure we get consistent results between debug/opt.
#if __FLT_EVAL_METHOD__ == 2
  long double px = pers->m_momentum.m_px;
  long double py = pers->m_momentum.m_py;
  long double pz = pers->m_momentum.m_pz;
  long double m = pers->m_momentum.m_m;
#else
   double px = pers->m_momentum.m_px;
   double py = pers->m_momentum.m_py;
   double pz = pers->m_momentum.m_pz;
   double m = pers->m_momentum.m_m;
#endif
   double e;
  if(m<0) e = std::sqrt(px*px+py*py+pz*pz-m*m);
  else e = std::sqrt(px*px+py*py+pz*pz+m*m);
  trans->setPx(px);
  trans->setPy(py);
  trans->setPz(pz);
  trans->setE(e);
  pbsCnv.persToTrans( &pers->m_partBase, &trans->particleBase(), msg);
  
  if(msg.level() == MSG::DEBUG ) { 
  
    msg << MSG::DEBUG << "Reading jets  " << trans->jetAuthor() << "  pers_e = "<<e 
      //<< " reco status " << std::hex << trans->m_recoStatus << std::dec 
	<< "  sig state = "<< trans->signalState()  << endmsg; 
  }

  
  // deal with signal state
  if ( pers->m_rawSignal.size() >= 1 )
    {
      SignalStateCnv statecnv;
      JetConverterTypes::momentum mom = statecnv.decompress( pers->m_rawSignal, pers->m_momentum, msg );
      
      //trans->setSignalState(P4SignalState::JETEMSCALE);
      px = mom.m_px;
      py = mom.m_py;
      pz = mom.m_pz;
      m =  mom.m_m;
      e=sqrt(px*px+py*py+pz*pz+m*m);
      trans->setRawPx(px);
      trans->setRawPy(py);
      trans->setRawPz(pz);
      trans->setRawE(e);
      
      
    } else {
      // no raw signal has been saved this means that JETEMSCALE == JETFINAL (truth jets for ex)
      Jet::hlv_t vec4 = trans->hlv();
      trans->setSignalState(P4SignalState::JETEMSCALE);
      trans->set4Mom(vec4);
      trans->setSignalState(P4SignalState::JETFINAL);
    }

  if(msg.level() == MSG::DEBUG ) {
    msg << MSG::DEBUG << "   --> signal state saved : ";
    if ( pers->m_rawSignal.size() >= 1 ){      
      msg << MSG::DEBUG << " raw_e =" << trans->getRawE() << "  constscale_e=" << trans->getCScaleE() << "  final_e="<<trans->e() << endmsg;
    } else msg << MSG::DEBUG<< " None " << endmsg;
  }


  // deal with constituent state :
  if(trans->constituentSignalState() == P4SignalState::UNCALIBRATED){
    // we assume constituent scale is JETEMSCALE ... 
    // For now, this is correct in any known situation

    trans->setCScaleE(trans->getRawE());
    trans->setCScalePx(trans->getRawPx());
    trans->setCScalePy(trans->getRawPy());
    trans->setCScalePz(trans->getRawPz());

    trans->setSignalState(P4SignalState::JETFINAL);
  } else {

    // Then we recompute JETCONSTITUENTSCALE :
    Jet::hlv_t vec4 = trans->constituent_sum4Mom(); // WARNING constituent_sum4Mom(s) is tricky
    trans->setCScaleE( vec4.e());
    trans->setCScalePx(vec4.px());
    trans->setCScalePy(vec4.py());
    trans->setCScalePz(vec4.pz());

    trans->setSignalState(P4SignalState::JETFINAL);
  }
  if(msg.level() == MSG::DEBUG ) msg << MSG::DEBUG << "   --> raw constituent : "<< (trans->constituentSignalState() == P4SignalState::UNCALIBRATED)   
				     << "  new constscale_e ="<< trans->getCScaleE() << endmsg; 



  JetKeyDescriptorInstance * keydesc = JetKeyDescriptorInstance::instance();
   std::vector<std::string> momentNames = keydesc->getKeys(JetKeyConstants::ShapeCat);
   if( (pers)->m_shapeStore.size() >0 ){
     if( momentNames.size() < (pers)->m_shapeStore.size() ) { if( ! pers->m_usedForTrigger ) {
         msg << MSG::WARNING << " JetCnv_p5 can't convert moments ! num max keys = "<< momentNames.size() << " persistant jet has n= "<< (pers)->m_shapeStore.size() <<endmsg; }
     }
     else {
       for(size_t i=0;i<(pers)->m_shapeStore.size();i++){
         trans->setMoment(momentNames[i], (pers)->m_shapeStore[i], true);
       }
     }
   }
    // Translate recoStatus
   double jetTime = (pers->m_recoStatus >> 16)*0.01;
   double jetQuality = ((pers->m_recoStatus & 65535) >> 3)*(1./8191.);
   // Store these in the moments?
   
   trans->setMoment("Timing", jetTime);
   trans->setMoment("LArQuality", jetQuality);

   const_cast<Jet_p5*>(pers)->m_shapeStore.clear();
  
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
      m_taginfoCnv.createTransientConst(&(pers->m_tagJetInfo), msg);
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
      m_tagAssCnv.createTransientConst(&(pers->m_associations), msg);

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
  
  if(msg.level() == MSG::DEBUG )   msg << MSG::DEBUG << "Loaded Jet from persistent state [OK]. Final e=" << trans->e()
				       << endmsg;
  return;
}




// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

void JetCnv_p5::transToPers( const Jet*  trans, 
                             Jet_p5* pers, 
                             MsgStream& msg ) const
{

  // This part is only for trigger now 
  pers->m_usedForTrigger = true;



  if( ! s_write0constit) {
    navCnv.transToPers( &trans->navigableBase(), &pers->m_nav, msg );
  }

  // we store the momentum now as floats, plus use signal state compressors
  // momCnv.transToPers( &trans->momentumBase(), &pers->m_momentum, msg );
  pbsCnv.transToPers( &trans->particleBase(), &pers->m_partBase, msg);
  
  
  //Jet::signalstate_t s = trans->signalState();
  pers->m_rawSignal.clear();
  pers->m_momentum = JetConverterTypes::momentum( trans->px(),
        					  trans->py(),
        					  trans->pz(),
        					  trans->m() );

  if(msg.level() == MSG::DEBUG ) { 
  
    msg << MSG::DEBUG << "Writing jets  " << trans->jetAuthor() << "  trans_e = "<< trans->e()
        << "  skipping constituent : "<< s_write0constit  << endmsg; 
  }
  

  

  pbsCnv.transToPers( &trans->particleBase(), &pers->m_partBase, msg);
  
  pers->m_author = trans->m_jetAuthor;


  // for trigger : recostatus == roiword
  pers->m_recoStatus =     trans->m_RoIword ;


  pers->m_constituentsN = trans->m_constituentsN;


  /// persistify moments :
  const JetMomentMap * map = trans->getMomentMap();
  if(map) {
    const std::vector<JetMoment::MomentData> *rec = map->getRecord(trans->m_jetId);
    if( rec ) {
      std::vector<std::string > keys= JetKeyDescriptorInstance::instance()->getKeys(JetKeyConstants::ShapeCat);
      (pers)->m_shapeStore.resize(keys.size());
      for(size_t i=0;i< rec->size();++i){
        (pers)->m_shapeStore[i] = rec->operator[](i).moment();
      }
    }
  }
  // std::vector<std::string> momentkeys = trans->getShapeKeys();
  // (pers)->m_shapeStore.resize(momentkeys.size());
  // for(size_t i=0;i< momentkeys.size();++i){
  //   (pers)->m_shapeStore[i] = trans->getShape(momentkeys[i]);
  // }
  
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

  if(msg.level() == MSG::DEBUG ){
    msg << MSG::DEBUG << "Created persistent state of Jet [OK]" << endmsg;
  }
  return;
}
