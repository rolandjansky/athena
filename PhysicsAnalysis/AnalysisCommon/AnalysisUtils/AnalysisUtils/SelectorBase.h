/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
SelectorBase.h:  Base class for selectors, the derived class is a template for
                  SelectorBase, ie class Derived: public SelectorBase<Derived>
                 

                  Expects an InputKey for the branch with objects to be selected
                  and a SelectedParticlesKey for the output branch

NOTE: Derived class for container XXXContainer must define
   public:
      typedef XXXContainer Container;
      typedef XXX          Contained;

                  S. Protopopescu 15-Sept-2008

***********************************************************************/

#ifndef SELECTORBASE_H
#define SELECTORBASE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ParticleEvent/SelectedParticles.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <string>
#include <cmath>

template<class Derived> 
class SelectorBase : public AthAlgorithm {

 public:
    SelectorBase(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SelectorBase(){;}
    virtual StatusCode initialize();
    virtual StatusCode userExecute(){return StatusCode::SUCCESS;}
    StatusCode execute();
    virtual StatusCode finalize();
    bool pt_eta_range(double pt, double eta);
    // Derived class must supply
    // bool accept(const Contained*); 
    // it should use pt_eta_range(pt,eta)
    // For each object that this method returns true
    // a flag will be set to true in a SelectedParticles branch
    std::string getInputKey(){return m_inputKey;}

 protected:

  StoreGateSvc*   m_pSG;
  MsgStream* mLog;
  std::string m_inputKey;
  std::string m_inselectedKey;
  std::string m_selectedKey;
  std::string m_inlinksKey;
  std::string m_linksKey;
  bool m_all;
  bool m_nogood;
  bool m_inSel;
  bool m_inLink;
  bool m_gensel;
  bool m_genlink;
  double m_ptmin;
  double m_ptmax;
  double m_etamin;
  double m_etamax;
  double m_absetamax;
  double m_absetamin;
};
    // 
    // Templated implementation must go here...
    // 
    
template<class Derived>
    SelectorBase<Derived>::SelectorBase(const std::string& name, ISvcLocator* pSvcLocator)
      : AthAlgorithm(name, pSvcLocator),
        m_pSG(0),
        mLog(0),
        m_gensel(false),
        m_genlink(false)
{
  declareProperty("InputKey", m_inputKey = "NONE");
  declareProperty("InSelectedParticlesKey", m_inselectedKey = "NONE");
  declareProperty("OutSelectedParticlesKey", m_selectedKey = "NONE");
  declareProperty("InParticleLinksKey", m_inlinksKey = "NONE");
  declareProperty("OutParticleLinksKey", m_linksKey = "NONE");
  declareProperty("PtMin",m_ptmin = 15.0*CLHEP::GeV);
  declareProperty("PtMax",m_ptmax = 10000.0*CLHEP::GeV);
  declareProperty("EtaMax",m_etamax = 2.5);
  declareProperty("EtaMin",m_etamin = -2.5);
  declareProperty("AbsEtaMax",m_absetamax = 2.5);
  declareProperty("AbsEtaMin",m_absetamin =0);
  declareProperty("SelectAll",m_all =false);
  m_nogood=false;
  m_inSel=false;
  m_inLink=false;
}
template<class Derived>
    StatusCode SelectorBase<Derived>::initialize()
{
  mLog=new MsgStream(messageService(), name());
  m_genlink=true;
  m_gensel=true;
  (*mLog) <<MSG::INFO <<"Initializing Selector " <<name() <<endreq;
  if(m_inputKey==std::string("NONE")) m_nogood=true;
  if(m_selectedKey==std::string("NONE")) m_gensel=false;
  if(m_inselectedKey!=std::string("NONE")) m_inSel=true;
  if(m_linksKey==std::string("NONE") ) m_genlink=false;
  if(m_inlinksKey!=std::string("NONE")) m_inLink=true;

  (*mLog) << MSG::INFO << "InputKey="<<m_inputKey.c_str()<<", OutSelectedParticlesKey="<<m_selectedKey.c_str()
	  <<", InSelectedParticlesKey="<<m_inselectedKey.c_str()
	  <<", InParticlesLinksKey="<<m_inlinksKey.c_str()
	  <<", OutParticleLinksKey="<<m_linksKey.c_str()<<endreq;
  (*mLog)<< MSG::INFO<<", PtMin="<<m_ptmin;
  (*mLog)<< MSG::INFO<<", PtMax="<<m_ptmax;
  (*mLog)<< MSG::INFO<<", EtaMin="<<m_etamin;
  (*mLog)<< MSG::INFO<<", EtaMax="<<m_etamax;
  (*mLog)<< MSG::INFO<<", AbsEtaMin="<<m_absetamin;
  (*mLog)<< MSG::INFO<<", AbsEtaMax="<<m_absetamax;
  (*mLog)<< MSG::INFO<<", SelectAll="<<m_all;
  if(m_all) (*mLog)<< MSG::INFO<<"  All items will be accepted";
  (*mLog)<< MSG::INFO<<endreq;
  
  m_nogood=m_nogood && (!m_genlink && !m_gensel); 
  if(m_nogood){
      (*mLog) << MSG::FATAL << "Must supply names for InputKey, OutSelectedParticlesKey and/or OutParticleLinksKey" 
	      << endreq;
  }
  m_nogood=m_inSel && m_inLink;
  if(m_nogood){
      (*mLog) << MSG::FATAL << "Cannot have both InSelectedParticlesKey and InParticlesLinksKey, must chose one" 
	      << endreq;
  }

  // Get a SG pointer:
  if (StatusCode::FAILURE == service("StoreGateSvc", m_pSG) ) {
    (*mLog) << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return StatusCode::FAILURE;
  } 

  if(m_nogood) return StatusCode::FAILURE;

  (*mLog) << MSG::INFO << "Created StoreGate pointer m_pSG" << endreq;
  (*mLog) <<MSG::INFO <<"Done Initializing " <<name() <<endreq;
  return StatusCode::SUCCESS;
}

template<class Derived>
    StatusCode SelectorBase<Derived>::execute()
{
    typedef typename Derived::Container Container;
    //typedef typename Derived::Contained Contained;
    typedef typename Derived::Links Links;

    StatusCode sc;

    (*mLog) <<MSG::DEBUG <<std::endl <<endreq;
    (*mLog) <<MSG::DEBUG <<"Executing Selector " <<name() <<endreq;
    sc=userExecute();
    if( sc.isFailure()) return sc;
    const Container* particles = 0;
    const SelectedParticles* inselparts=0;
    const Links* inlinks=0;
    if(m_inSel){
	sc = m_pSG->retrieve(inselparts,m_inselectedKey);
	if(  sc.isFailure()) {
	    (*mLog) << MSG::ERROR
		    << "Could not retrieve InSelectedParticles with name "
		    << m_inselectedKey		    << endreq;
	    return sc;
	}    
    }
    if(m_inLink){
	sc = m_pSG->retrieve(inlinks,m_inlinksKey);
	if(  sc.isFailure()) {
	    (*mLog) << MSG::ERROR
		    << "Could not retrieve InParticleLinks with name "
		    << m_inlinksKey
		    << endreq;
	    return sc;
	}    
    }

    sc = m_pSG->retrieve(particles,m_inputKey);
    if(  sc.isFailure() || particles==0) {
	(*mLog) << MSG::ERROR
		<< "Could not retrieve Container with name "
		<< m_inputKey
		<< endreq;
	return sc;
    }
    int npart=particles->size();
    (*mLog)<< MSG::DEBUG<<" >>>> got Container,  no. of particles="<<npart<<endreq;

    SelectedParticles* selparts=0;
    if(m_gensel){
	//   SelectedParticles option
        selparts=new SelectedParticles();
	(*mLog)<< MSG::DEBUG<<" >>>> Generate SelectedParticles";
	if(m_pSG->record(selparts,m_selectedKey).isFailure()) return StatusCode::FAILURE;
	typename Container::const_iterator ipItr = particles->begin();
	typename Container::const_iterator ipEnd = particles->end();
	int npart=particles->size();
	selparts->SetMaxBits(npart);
	unsigned ipart=0;
	if(!m_inLink){
	    if(m_inSel) (*mLog)<< MSG::DEBUG<<" using InSelectedParticles("<<m_inselectedKey
			       <<"), no. particles="<<inselparts->numGood()
			       <<" <<<"<<endreq;
	    else (*mLog)<< MSG::DEBUG<<" looping over whole Container <<<"<<endreq;
	    for(; ipItr != ipEnd; ++ipItr) {
		if(m_inSel && !inselparts->isGood(ipart)){
		    (*mLog) << MSG::DEBUG<<" rejected ipart="<<ipart<<endreq;
		    ipart++;
		    continue;
		}
		if( m_all || static_cast<Derived*>(this)->accept(*ipItr)){
		    selparts->SetBit(ipart);
		    (*mLog)<< MSG::DEBUG<<" accepted ipart="<<ipart<<endreq;
		}else (*mLog) << MSG::DEBUG<<" rejected ipart="<<ipart<<endreq;
		ipart++;
	    }
	}else{
	    // loop over input links
	    typename Links::const_iterator ilItr = inlinks->begin();
	    typename Links::const_iterator ilEnd = inlinks->end();
	    (*mLog)<< MSG::DEBUG<<" using input  ParticleLinks ("<<m_inlinksKey<<"), no. of particles="
		   <<inlinks->size()<<"  <<<<<"<<endreq;
	    int ipart=0;
	    for(; ilItr != ilEnd; ++ilItr) {
		ipart=(*ilItr).index();
		//const Contained* cpart=(*inlinks)[ipart];
		//Contained* part=const_cast<Contained *>(cpart);
		if( m_all || static_cast<Derived*>(this)->accept(*ilItr) ){
		    selparts->SetBit(ipart);
		    (*mLog)<< MSG::DEBUG<<" accepted ipart="<<ipart<<endreq;
		}else (*mLog) << MSG::DEBUG<<" rejected ipart="<<ipart<<endreq;
	    }
	}
	(*mLog)<< MSG::DEBUG<<" SelectedParticles bits: "<<selparts->displayBits()<<endreq;
    } // end of SelectedParticles option

    //  ParticleLinks option
    if(m_genlink){
	(*mLog)<< MSG::DEBUG<<" >>>> Generate ParticleLinks";
	Links* links=new Links();
	if(m_pSG->record(links,m_linksKey).isFailure()) return StatusCode::FAILURE;

	if(m_gensel){
	    // selection already done, selparts filled
	    typename Container::const_iterator ipItr = particles->begin();
	    typename Container::const_iterator ipEnd = particles->end();
	    unsigned ipart=0;
	    for(; ipItr != ipEnd; ++ipItr) {
		if(selparts->isGood(ipart)){
		    ElementLink<Container> el(*particles,ipart);
		    links->push_back(el);
		}
		ipart++;
	    }
	    return sc;
	}
	if(m_inLink){
	    typename Links::const_iterator ilItr = inlinks->begin();
	    typename Links::const_iterator ilEnd = inlinks->end();
	    (*mLog)<< MSG::DEBUG<<" using input  ParticleLinks ("<<m_inlinksKey<<"), no. of particles="
		   <<inlinks->size()<<"  <<<<<"<<endreq;
	    unsigned ipart=0;
	    for(; ilItr != ilEnd; ++ilItr) {
		ipart=(*ilItr).index();
		//const Contained* cpart=(*inlinks)[ipart];
		//Contained* part=const_cast<Contained *>(cpart);
		if(  m_all || static_cast<Derived*>(this)->accept(*ilItr) ){
		    links->push_back(*ilItr);
		    (*mLog)<< MSG::DEBUG<<" accepted ipart="<<ipart<<endreq;
		}else (*mLog) << MSG::DEBUG<<" rejected ipart="<<ipart<<endreq;
	    }
	}else{
	    if(m_inSel) (*mLog)<< MSG::DEBUG<<" using InSelectedParticles("<<m_inselectedKey
			       <<"), no. particles="<<inselparts->numGood()
			       <<" <<<"<<endreq;
	    else (*mLog)<< MSG::DEBUG<<" looping over whole Container <<<"<<endreq;
	    typename Container::const_iterator ipItr = particles->begin();
	    typename Container::const_iterator ipEnd = particles->end();
	    unsigned ipart=0;
	    for(; ipItr != ipEnd; ++ipItr) {
		if(m_inSel && !inselparts->isGood(ipart)){
		    (*mLog) << MSG::DEBUG<<" rejected ipart="<<ipart<<endreq;
		    ipart++;
		    continue;
		}
		if(  m_all || static_cast<Derived*>(this)->accept(*ipItr) ){
		    ElementLink<Container> el(*particles,ipart);
		    links->push_back(el);
		    //(*mLog)<< MSG::DEBUG<<" accepted ipart="<<ipart<<", pt="<<(*particles)[ipart]->pt()/1000.<<endreq;
		    (*mLog)<< MSG::DEBUG<<" accepted ipart="<<ipart<<", pt="<<(*el)->pt()/1000.0<<endreq;
		}else (*mLog) << MSG::DEBUG<<" rejected ipart="<<ipart<<endreq;
		ipart++;
	    }
	}
    } // end of ParticleLinks option
    return sc;
}

template<class Derived>
    StatusCode  SelectorBase<Derived>::finalize() {

  (*mLog) << MSG::INFO << "Finalizing Selector " << name() << endreq;

  return StatusCode::SUCCESS;
}

template<class Derived>
bool SelectorBase<Derived>::pt_eta_range(double pt, double eta){
	if(pt<m_ptmin) return false;
	if(pt>m_ptmax) return false;
	if(fabs(eta)>m_absetamax) return false;
	if(fabs(eta)<m_absetamin) return false;
	if(eta>m_etamax) return false;
	if(eta<m_etamin) return false;
	return true;
    }
#endif
