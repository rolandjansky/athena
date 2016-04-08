///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p3.cxx 
// Implementation file for class JetCnv_p3
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
#include "JetEventTPCnv/JetCnv_p3.h"
#include "JetEventTPCnv/JetSignalStateCnv.h"

typedef NavigableCnv_p1<
           Navigable<INavigable4MomentumCollection,double>
           > NavigableCnv_t;

// pre-allocate converters
static P4ImplPxPyPzECnv_p1   momCnv;
static NavigableCnv_t        navCnv;


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void JetCnv_p3::persToTrans( const Jet_p3* pers,
                             Jet* trans, 
                             MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Loading Jet from persistent state...  e = "<< pers->m_momentum.m_ene
      << endreq;

  navCnv.persToTrans( &pers->m_nav,      
		      &trans->navigableBase(), msg );
  momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(),  msg );
  
  msg << MSG::DEBUG << "    after momCnv e=" << trans->e() <<endreq; 

  trans->m_jetAuthor = pers->m_author;

  // trans->m_combinedLikelihood = std::vector<double>( pers->m_combinedLikelihood );
  // trans->setCombinedLikelihood(pers->m_combinedLikelihood);
   
   

   JetKeyDescriptorInstance * keydesc = JetKeyDescriptorInstance::instance();
   std::vector<std::string> momentNames = keydesc->getKeys(JetKeyConstants::ShapeCat);
   if( momentNames.size() != (pers)->m_shapeStore.size() ) msg << MSG::ERROR << " JEtCnv_p2 can't convert moments ! expected moment n= "<< momentNames.size() << " persistatn has "<< (pers)->m_shapeStore.size() <<endreq;
   else for(size_t i=0;i<momentNames.size();i++){
       trans->setMoment(momentNames[i], (pers)->m_shapeStore[i], true);
     }
  
  if (trans->m_tagInfoStore)
    const_cast<Jet_p3*>(pers)->m_tagJetInfo.swap (*trans->m_tagInfoStore);
  else if ( (pers)->m_tagJetInfo.size() > 0 ) {
    trans->m_tagInfoStore = new Jet::tagstore_t;
    const_cast<Jet_p3*>(pers)->m_tagJetInfo.swap (*trans->m_tagInfoStore);
  }
  
  if (trans->m_assocStore)
    const_cast<Jet_p3*>(pers)->m_associations.swap (*trans->m_assocStore);
  else if ( (pers)->m_associations.size() > 0 ) {
    trans->m_assocStore   = new Jet::assostore_t;
    const_cast<Jet_p3*>(pers)->m_associations.swap (*trans->m_assocStore);
   }
  
   // Avoid memory leaks.
   for (size_t i = 0; i < pers->m_tagJetInfo.size(); i++)
     delete pers->m_tagJetInfo[i];
   for (size_t i = 0; i < pers->m_associations.size(); i++)
     delete pers->m_associations[i];

   
   const_cast<Jet_p3*>(pers)->m_shapeStore.clear();
   const_cast<Jet_p3*>(pers)->m_tagJetInfo.clear();
   const_cast<Jet_p3*>(pers)->m_associations.clear();

   // default signal state
   trans->setSignalState(P4SignalState::CALIBRATED);


   SignalStateCnv statecnv;

   trans->setRawE( statecnv.ratio_from_char( pers->m_ratioE) * trans->e() ) ;
   trans->setRawPx( statecnv.ratio_from_char(pers->m_ratioPx) * trans->px() ) ;
   trans->setRawPy( statecnv.ratio_from_char(pers->m_ratioPy) * trans->py() ) ;
   trans->setRawPz( statecnv.ratio_from_char(pers->m_ratioPz) * trans->pz() ) ;

  // Jet comes from a DataPool.
  // So we need to be sure to re-initialize everything in the Jet.
  static Jet jtmp;
  trans->particleBase() = jtmp.particleBase();

   msg << MSG::DEBUG << "Loaded Jet from persistent state [OK]"
       << endreq;
   return;
}

void JetCnv_p3::transToPers( const Jet* trans, 
                             Jet_p3* pers, 
                             MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Creating persistent state of Jet... e="<< trans->e() << "  "<< trans->momentumBase().e()
      << endreq;

  pers->m_ownPointers = false;

  navCnv.transToPers( &trans->navigableBase(), &pers->m_nav,      msg );
  momCnv.transToPers( &trans->momentumBase(),  &pers->m_momentum, msg );

  
  pers->m_author = trans->m_jetAuthor;
  
  // no more needed : we don't save nomore to p3
  //pers->m_combinedLikelihood = std::vector<double>( trans->m_combinedLikelihood );
  
  if ( bool(trans->m_tagInfoStore) )
    pers->m_tagJetInfo   = *(trans->m_tagInfoStore);
  else
    pers->m_tagJetInfo.clear();
  if ( bool(trans->m_assocStore) )
    pers->m_associations = *(trans->m_assocStore);
  else
    pers->m_associations.clear();
  


   SignalStateCnv statecnv;
   trans->setSignalState(P4SignalState::CALIBRATED);

   pers->m_ratioE  = statecnv.char_from_ratio(trans->getRawE () / trans->e() );
   pers->m_ratioPx = statecnv.char_from_ratio(trans->getRawPx() / trans->px());
   pers->m_ratioPy = statecnv.char_from_ratio(trans->getRawPy() / trans->py());
   pers->m_ratioPz = statecnv.char_from_ratio(trans->getRawPz() / trans->pz());
   


   msg << MSG::DEBUG << "Created persistent state of Jet [OK]  e="<< pers->m_momentum.m_ene
      << endreq;
  return;
}
