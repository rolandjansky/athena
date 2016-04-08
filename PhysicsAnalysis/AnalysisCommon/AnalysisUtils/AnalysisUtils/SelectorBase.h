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
#include "AthenaKernel/Units.h"
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
  m_genlink=true;
  m_gensel=true;
  ATH_MSG_INFO("Initializing Selector " <<name()  );
  if(m_inputKey==std::string("NONE")) m_nogood=true;
  if(m_selectedKey==std::string("NONE")) m_gensel=false;
  if(m_inselectedKey!=std::string("NONE")) m_inSel=true;
  if(m_linksKey==std::string("NONE") ) m_genlink=false;
  if(m_inlinksKey!=std::string("NONE")) m_inLink=true;

  ATH_MSG_INFO( "InputKey="<<m_inputKey.c_str()<<", OutSelectedParticlesKey="<<m_selectedKey.c_str()
                <<", InSelectedParticlesKey="<<m_inselectedKey.c_str()
                <<", InParticlesLinksKey="<<m_inlinksKey.c_str()
                <<", OutParticleLinksKey="<<m_linksKey.c_str() );
  ATH_MSG_INFO(", PtMin="<<m_ptmin
               << MSG::INFO<<", PtMax="<<m_ptmax
               << MSG::INFO<<", EtaMin="<<m_etamin
               << MSG::INFO<<", EtaMax="<<m_etamax
               << MSG::INFO<<", AbsEtaMin="<<m_absetamin
               << MSG::INFO<<", AbsEtaMax="<<m_absetamax
               << MSG::INFO<<", SelectAll="<<m_all  );
  if(m_all) ATH_MSG_INFO("  All items will be accepted"  );
  
  m_nogood=m_nogood && (!m_genlink && !m_gensel); 
  if(m_nogood){
    ATH_MSG_FATAL( "Must supply names for InputKey, OutSelectedParticlesKey and/or OutParticleLinksKey" );
  }
  m_nogood=m_inSel && m_inLink;
  if(m_nogood){
    ATH_MSG_FATAL( "Cannot have both InSelectedParticlesKey and InParticlesLinksKey, must chose one" );
  }

  ATH_MSG_INFO("Done Initializing " <<name()  );
  return StatusCode::SUCCESS;
}

template<class Derived>
    StatusCode SelectorBase<Derived>::execute()
{
    typedef typename Derived::Container Container;
    //typedef typename Derived::Contained Contained;
    typedef typename Derived::Links Links;

    ATH_MSG_DEBUG(std::endl  );
    ATH_MSG_DEBUG("Executing Selector " <<name()  );
    ATH_CHECK( userExecute() );
    const Container* particles = 0;
    const SelectedParticles* inselparts=0;
    const Links* inlinks=0;
    if(m_inSel){
      ATH_CHECK( evtStore()->retrieve(inselparts,m_inselectedKey) );
    }
    if(m_inLink){
      ATH_CHECK( evtStore()->retrieve(inlinks,m_inlinksKey) );
    }

    ATH_CHECK( evtStore()->retrieve(particles,m_inputKey) );
    int npart=particles->size();
    ATH_MSG_DEBUG(" >>>> got Container,  no. of particles="<<npart );

    SelectedParticles* selparts=0;
    if(m_gensel){
	//   SelectedParticles option
        selparts=new SelectedParticles();
	ATH_MSG_DEBUG(" >>>> Generate SelectedParticles" );
	ATH_CHECK(evtStore()->record(selparts,m_selectedKey) );
	typename Container::const_iterator ipItr = particles->begin();
	typename Container::const_iterator ipEnd = particles->end();
	int npart=particles->size();
	selparts->SetMaxBits(npart);
	unsigned ipart=0;
	if(!m_inLink){
          if(m_inSel) ATH_MSG_DEBUG(" using InSelectedParticles("<<m_inselectedKey
                                    <<"), no. particles="<<inselparts->numGood()
                                    <<" <<<" );
          else ATH_MSG_DEBUG(" looping over whole Container <<<" );
	    for(; ipItr != ipEnd; ++ipItr) {
		if(m_inSel && !inselparts->isGood(ipart)){
                    ATH_MSG_DEBUG(" rejected ipart="<<ipart );
		    ipart++;
		    continue;
		}
		if( m_all || static_cast<Derived*>(this)->accept(*ipItr)){
		    selparts->SetBit(ipart);
		    ATH_MSG_DEBUG(" accepted ipart="<<ipart );
		}else ATH_MSG_DEBUG(" rejected ipart="<<ipart );
		ipart++;
	    }
	}else{
	    // loop over input links
	    typename Links::const_iterator ilItr = inlinks->begin();
	    typename Links::const_iterator ilEnd = inlinks->end();
	    ATH_MSG_DEBUG(" using input  ParticleLinks ("<<m_inlinksKey<<"), no. of particles="
                          <<inlinks->size()<<"  <<<<<" );
	    int ipart=0;
	    for(; ilItr != ilEnd; ++ilItr) {
		ipart=(*ilItr).index();
		//const Contained* cpart=(*inlinks)[ipart];
		//Contained* part=const_cast<Contained *>(cpart);
		if( m_all || static_cast<Derived*>(this)->accept(*ilItr) ){
		    selparts->SetBit(ipart);
		    ATH_MSG_DEBUG(" accepted ipart="<<ipart );
		}else ATH_MSG_DEBUG(" rejected ipart="<<ipart );
	    }
	}
	ATH_MSG_DEBUG(" SelectedParticles bits: "<<selparts->displayBits() );
    } // end of SelectedParticles option

    //  ParticleLinks option
    if(m_genlink){
      ATH_MSG_DEBUG(" >>>> Generate ParticleLinks" );
	Links* links=new Links();
	ATH_CHECK( evtStore()->record(links,m_linksKey) );

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
	    return StatusCode::SUCCESS;
	}
	if(m_inLink){
	    typename Links::const_iterator ilItr = inlinks->begin();
	    typename Links::const_iterator ilEnd = inlinks->end();
	    ATH_MSG_DEBUG(" using input  ParticleLinks ("<<m_inlinksKey<<"), no. of particles="
                          <<inlinks->size()<<"  <<<<<" );
	    unsigned ipart=0;
	    for(; ilItr != ilEnd; ++ilItr) {
		ipart=(*ilItr).index();
		//const Contained* cpart=(*inlinks)[ipart];
		//Contained* part=const_cast<Contained *>(cpart);
		if(  m_all || static_cast<Derived*>(this)->accept(*ilItr) ){
		    links->push_back(*ilItr);
		    ATH_MSG_DEBUG(" accepted ipart="<<ipart );
		}else ATH_MSG_DEBUG(" rejected ipart="<<ipart );
	    }
	}else{
          if(m_inSel) ATH_MSG_DEBUG(" using InSelectedParticles("<<m_inselectedKey
                                    <<"), no. particles="<<inselparts->numGood()
                                    <<" <<<" );
          else ATH_MSG_DEBUG(" looping over whole Container <<<" );
	    typename Container::const_iterator ipItr = particles->begin();
	    typename Container::const_iterator ipEnd = particles->end();
	    unsigned ipart=0;
	    for(; ipItr != ipEnd; ++ipItr) {
		if(m_inSel && !inselparts->isGood(ipart)){
                    ATH_MSG_DEBUG(" rejected ipart="<<ipart );
		    ipart++;
		    continue;
		}
		if(  m_all || static_cast<Derived*>(this)->accept(*ipItr) ){
		    ElementLink<Container> el(*particles,ipart);
		    links->push_back(el);
		    ATH_MSG_DEBUG(" accepted ipart="<<ipart<<", pt="<<(*el)->pt()/Athena::Units::GeV );
		}else ATH_MSG_DEBUG(" rejected ipart="<<ipart );
		ipart++;
	    }
	}
    } // end of ParticleLinks option
    return StatusCode::SUCCESS;
}

template<class Derived>
    StatusCode  SelectorBase<Derived>::finalize() {

  ATH_MSG_INFO( "Finalizing Selector " << name()  );

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
