/////////////////////// -*- C++ -*- ////////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleLinksCnv_p1.h 
// Header file for class ParticleLinksCnv_p1, handles transient<->persistent ParticleLinks
// Template class, must be inherited by the class handling instantiations of ParticleLinks
// see ParticleBaseLinksCnv_p1
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLELINKSCNV_P1_H
#define PARTICLEEVENTTPCNV_PARTICLELINKSCNV_P1_H 1
 
// AthenaPoolCnvSvc includes
//#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h" (in DataLinkCnv_p1.h)
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "AthLinks/DataLink.h"
#include "GaudiKernel/MsgStream.h"
#include "ParticleEvent/ParticleLinks.h"
#include "ParticleEvent/SelectedParticles.h"
#include "ParticleEventTPCnv/ParticleLinks_p1.h"
#include <string>
#include <iostream>


template<class Container> 
class ParticleLinksCnv_p1 : public T_AthenaPoolTPCnvBase<
                                               ParticleLinks<Container>, 
                                               ParticleLinks_p1
                                               >  
{ 
 
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 
  public: 
 
    ParticleLinksCnv_p1(){;}
    virtual ~ParticleLinksCnv_p1(){;}
 
   /////////////////////////////////////////////////////////////////// 
   // Conversion methods: 
   ///////////////////////////////////////////////////////////////////
 
   virtual 
   void persToTrans( const ParticleLinks_p1* persObj, 
		     ParticleLinks<Container>* transObj, 
		     MsgStream &msg );
 
   /** Method creating the persistent representation @c Decay_p1
    *  from its transient representation @c Decay
    */
   virtual 
   void transToPers( const ParticleLinks<Container>* transObj, 
		     ParticleLinks_p1* persObj, 
		     MsgStream &msg );
 private:
   DataLinkCnv_p1<DataLink<Container> > m_dl;
}; 

#ifndef __REFLEX__

template<class Container>
void ParticleLinksCnv_p1<Container>::transToPers(const ParticleLinks<Container>* transObj, 
						 ParticleLinks_p1* persObj, 
						 MsgStream &msg ){
    msg<< MSG::DEBUG<<typeid(*transObj).name()<<" called"<<endreq;
    //std::cout<<">>> ParticleLinksCnv_p1 "<<typeid(*transObj).name()<<" called"<<std::endl;
    if(transObj->size()==0) return;
    SelectedParticles selparts;    SelectedParticles_p1 selparts_p1;

    typedef ParticleLinks<Container> Links;
    selparts.SetMaxBits(transObj->size());
    typename Links::const_iterator ilItr = transObj->begin();
    typename Links::const_iterator ilEnd = transObj->end();
    for(; ilItr != ilEnd; ++ilItr) {
      selparts.SetBit((*ilItr).index());      
    }

    std::string container_name=(&(*transObj)[0])->dataID();
    msg<< MSG::DEBUG<<"Container name="<<container_name<<endreq;
    selparts_p1.m_bits=selparts.AllBits();
    persObj->setSelectedParticles_p1(selparts_p1);
    DataLink<Container> dl(container_name);
    DataLink_p1 dl_p1;
    m_dl.transToPers(&dl,&dl_p1,msg);
    persObj->setDataLink_p1(dl_p1);
    msg << MSG::DEBUG << "Created SelectedParticles, persistent state of "<<typeid(*transObj).name()<<endreq;

}

template<class Container>
void ParticleLinksCnv_p1<Container>::persToTrans(const ParticleLinks_p1* persObj, 
						 ParticleLinks<Container>* transObj, 
						 MsgStream &msg ){
    DataLink<Container> dl;
    const DataLink_p1* dl_p1=persObj->dl_p1();
    m_dl.persToTrans(dl_p1,&dl,msg);
    dl.toTransient();
    //const Container *particles=dl.cptr();
    const SelectedParticles_p1* selparts_p1=persObj->sel();
    //typename Container::const_iterator ipItr = particles->begin();
    //typename Container::const_iterator ipEnd = particles->end();
    SelectedParticles selparts;
    selparts.Set (selparts_p1->m_bits);
    //SelectedParticles selparts(m_bits);
    std::vector<unsigned> goodparts;
    selparts.goodParticles (goodparts);
    transObj->reserve(goodparts.size());
    for (unsigned index : goodparts) {
      transObj->emplace_back (dl.key(), index);
    }
    
    msg << MSG::DEBUG << "Loading "<<typeid(*transObj).name()<<"from SelectedParticles"<<endreq;
}

#endif // not __REFLEX__


#endif // !PARTICLEEVENTTPCNV_PARTICLELINKSCNV_P1_H

