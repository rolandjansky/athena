/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/versions/MissingETAssociationMap_v1.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODJet/JetAttributes.h"

#include "xAODBase/IParticleContainer.h"
#include "xAODBase/ObjectType.h"

#include <iterator>
#include <cstdio>

using namespace xAOD;

///////////////////////////////
// Internally used constants //
///////////////////////////////

/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

MissingETAssociationMap_v1::MissingETAssociationMap_v1(SG::OwnershipPolicy own, SG::IndexTrackingPolicy trackIndices)
  : DataVector<MissingETAssociation_v1>(own,trackIndices)
{
}

MissingETAssociationMap_v1::MissingETAssociationMap_v1(MissingETAssociationMap_v1::iterator first, MissingETAssociationMap_v1::iterator last,
						   SG::OwnershipPolicy own, SG::IndexTrackingPolicy trackIndices)
  : DataVector<MissingETAssociation_v1>(first,last,own,trackIndices)
{
}

MissingETAssociationMap_v1::~MissingETAssociationMap_v1()
{  }

/////////////
// Finders //
/////////////

MissingETAssociationMap_v1::const_iterator MissingETAssociationMap_v1::find(const Jet* pJet) const
{
  return this->f_findConst(pJet);
}

MissingETAssociationMap_v1::iterator MissingETAssociationMap_v1::find(const Jet* pJet)
{
  return this->f_find(pJet);
}

size_t MissingETAssociationMap_v1::findIndex(const Jet* pJet) const
{
  const_iterator fAssoc = this->f_findConst(pJet);
  if(fAssoc != this->end() && !(*fAssoc)->isMisc())
      return std::distance<const_iterator>(this->begin(),fAssoc);
  return MissingETBase::Numerical::invalidIndex();
}

///////////////////////
// Protected finders //
///////////////////////

MissingETAssociationMap_v1::const_iterator MissingETAssociationMap_v1::f_findConst(const Jet* pJet) const
{
  const_iterator fAssoc(this->begin());
  const_iterator lAssoc(this->end());
  while ( fAssoc != lAssoc && !(*fAssoc)->isMisc() && (*fAssoc)->refJet() != pJet ) { ++fAssoc; }
  return fAssoc;
}

MissingETAssociationMap_v1::iterator MissingETAssociationMap_v1::f_find(const Jet* pJet)
{
  iterator fAssoc(this->begin());
  iterator lAssoc(this->end());
  while ( fAssoc != lAssoc && !(*fAssoc)->isMisc() && (*fAssoc)->refJet() != pJet ) { ++fAssoc; }
  return fAssoc;
}

////////////////////////
// Cache of jet links //
////////////////////////

void MissingETAssociationMap_v1::f_setJetConstMap(std::map<ElementLink<IParticleContainer>, size_t> map)
{
  m_jetConstLinks.clear();
  m_jetConstLinks = map;
}

void MissingETAssociationMap_v1::f_clearJetConstMap()
{
  m_jetConstLinks.clear();  
}

bool MissingETAssociationMap_v1::setJetConstituents(const Jet* pJet)
{
  std::vector<ElementLink<IParticleContainer> > jetconst = pJet->constituentLinks();
  return this->setJetConstituents(jetconst,pJet);
}

bool MissingETAssociationMap_v1::setJetConstituents(const std::vector<ElementLink<IParticleContainer> >& constLinks, const Jet* pJet)
{
  size_t jetIdx = findIndex(pJet);  
  return this->setJetConstituents(constLinks,jetIdx);
}

bool MissingETAssociationMap_v1::setJetConstituents(const std::vector<ElementLink<IParticleContainer> >& constLinks, size_t jetIndex)
{
  bool setConst = (jetIndex!=MissingETBase::Numerical::invalidIndex());
  if(setConst) {
    for(std::vector<ElementLink<IParticleContainer> >::const_iterator iEL=constLinks.begin();
	iEL!=constLinks.end(); ++iEL) {
      m_jetConstLinks[*iEL] = jetIndex;
    }
  }
  return setConst;
}

MissingETAssociationMap_v1::const_iterator MissingETAssociationMap_v1::findByJetConst(const IParticle* pConst) const
{
  const IParticleContainer* pCont = static_cast<const IParticleContainer*>(pConst->container());
  ElementLink<IParticleContainer> el(*pCont,pConst->index());
  return findByJetConst(el);
}

MissingETAssociationMap_v1::iterator MissingETAssociationMap_v1::findByJetConst(const IParticle* pConst)
{
  const IParticleContainer* pCont = static_cast<const IParticleContainer*>(pConst->container());
  ElementLink<IParticleContainer> el(*pCont,pConst->index());
  return findByJetConst(el);
}

MissingETAssociationMap_v1::const_iterator MissingETAssociationMap_v1::findByJetConst(ElementLink<IParticleContainer> constLink) const
{
  MissingETAssociationMap_v1::const_iterator iAssoc(this->end());
  size_t index = findIndexByJetConst(constLink);
  if(index!=MissingETBase::Numerical::invalidIndex()) {
    iAssoc = this->begin() + index;
  }
  return iAssoc;
}

MissingETAssociationMap_v1::iterator MissingETAssociationMap_v1::findByJetConst(ElementLink<IParticleContainer> constLink)
{
  MissingETAssociationMap_v1::iterator iAssoc(this->end());
  size_t index = findIndexByJetConst(constLink);
  if(index!=MissingETBase::Numerical::invalidIndex()) {
    iAssoc = this->begin() + index;
  }
  return iAssoc;
}

size_t MissingETAssociationMap_v1::findIndexByJetConst(const IParticle* pConst) const
{
  const IParticleContainer* pCont = static_cast<const IParticleContainer*>(pConst->container());
  ElementLink<IParticleContainer> el(*pCont,pConst->index());
  return findIndexByJetConst(el);
}

size_t MissingETAssociationMap_v1::findIndexByJetConst(ElementLink<IParticleContainer> constLink) const
{
  size_t index = MissingETBase::Numerical::invalidIndex();
  std::map<ElementLink<IParticleContainer>, size_t>::const_iterator iConstMap = m_jetConstLinks.find(constLink);
  if (iConstMap==m_jetConstLinks.end()) for (std::map<ElementLink<IParticleContainer>, size_t>::const_iterator jConstMap = m_jetConstLinks.begin(); jConstMap!=m_jetConstLinks.end(); jConstMap++) if (*(jConstMap->first)==*constLink) iConstMap = jConstMap;
  if(iConstMap!=m_jetConstLinks.end()) {
    index = iConstMap->second;
    const_iterator fAssoc(this->begin());
    std::advance<const_iterator>(fAssoc,index);
  }
  return index;
}

const MissingETAssociation_v1* MissingETAssociationMap_v1::getMiscAssociation() const
{
  MissingETAssociationMap_v1::const_reverse_iterator rIter(this->rbegin());
  MissingETAssociationMap_v1::const_reverse_iterator fAssoc(this->rend());
  while ( rIter != fAssoc && !(*rIter)->isMisc() ) { ++rIter; }
  return rIter != fAssoc ? *rIter : (MissingETAssociation_v1*)0;
}

MissingETAssociation_v1* MissingETAssociationMap_v1::getMiscAssociation()
{ 
  MissingETAssociationMap_v1::reverse_iterator rIter(this->rbegin());
  MissingETAssociationMap_v1::reverse_iterator fAssoc(this->rend());
  while ( rIter != fAssoc && !(*rIter)->isMisc() ) { ++rIter; }
  return rIter != fAssoc ? *rIter : (MissingETAssociation_v1*)0;
}

bool MissingETAssociationMap_v1::identifyOverlaps()
{
  bool foundOverlaps(false);
  for(iterator iAssoc=this->begin(); iAssoc!=this->end(); ++iAssoc) {
    foundOverlaps |= (*iAssoc)->identifyOverlaps();
  }
  return foundOverlaps;
}

void MissingETAssociationMap_v1::clearOverlaps()
{
  for(iterator iAssoc=this->begin(); iAssoc!=this->end(); ++iAssoc) {
    (*iAssoc)->clearOverlaps();
  }
}

const xAOD::IParticleContainer* MissingETAssociationMap_v1::getUniqueSignals(const xAOD::IParticleContainer* signals,MissingETBase::UsageHandler::Policy p) const
{
  MissingETBase::Types::const_signal_vector_t* uniques = new MissingETBase::Types::const_signal_vector_t(SG::VIEW_ELEMENTS);
  for(IParticleContainer::const_iterator iSig=signals->begin();
      iSig!=signals->end(); ++iSig) {
    switch(p) {
    case MissingETBase::UsageHandler::TrackCluster:      
      if((*iSig)->type()==xAOD::Type::CaloCluster
	 || (*iSig)->type()==xAOD::Type::TrackParticle) {break;}
      else {continue;}
    case MissingETBase::UsageHandler::OnlyCluster:
      if((*iSig)->type()==xAOD::Type::CaloCluster) {break;}
      else {continue;}
    case MissingETBase::UsageHandler::OnlyTrack:
      if((*iSig)->type()==xAOD::Type::TrackParticle) {break;}
      else {continue;}
    case MissingETBase::UsageHandler::ParticleFlow:
      if((*iSig)->type()==xAOD::Type::ParticleFlow) {break;}
      else if((*iSig)->type()==xAOD::Type::FlowElement && ((static_cast<const xAOD::FlowElement*>(*iSig))->signalType() & xAOD::FlowElement::PFlow)) {break;}
      else {continue;}
    case MissingETBase::UsageHandler::TruthParticle:
      if((*iSig)->type()==xAOD::Type::TruthParticle) {break;}
      else {continue;}
    case MissingETBase::UsageHandler::AllCalo:
      if((*iSig)->type()!=xAOD::Type::TrackParticle) {break;}
      else {continue;}
    default: {continue;}
    }

    size_t assocIndex = findIndexByJetConst(*iSig);
    // printf("Cluster %lu (%p) ==> assoc %lu\n",(*iSig)->index(), (void*) *iSig, assocIndex);
    if(assocIndex==MissingETBase::Numerical::invalidIndex()) {
      // test misc association
      const MissingETAssociation_v1* assoc = this->getMiscAssociation();
      if(!assoc) {
      	uniques->push_back(*iSig);
      } else {
      	if(!assoc->containsSignal(*iSig)) {
	  // printf("Cluster is unique\n");
      	  uniques->push_back(*iSig);
      	} else {
	  // printf("Cluster is in misc\n");
	}
      }
    } // add if not associated to any jet
  }

  return uniques->asDataVector();
}

const xAOD::IParticleContainer* MissingETAssociationMap_v1::getOverlapRemovedSignals(const MissingETAssociationHelper& helper,const xAOD::IParticleContainer* signals,MissingETBase::UsageHandler::Policy p) const
{
  MissingETBase::Types::const_signal_vector_t* uniques = new MissingETBase::Types::const_signal_vector_t(SG::VIEW_ELEMENTS);
  for(const auto sig : *signals) {
    if(!MissingETAssociation_v1::testPolicy(sig->type(),p)) continue;
    
    size_t assocIndex = findIndexByJetConst(sig);
    if(assocIndex==MissingETBase::Numerical::invalidIndex()) {
      // test misc association
      const MissingETAssociation_v1* assoc = this->getMiscAssociation();
      if(!assoc) {
	uniques->push_back(sig);
      } else {
	if(!assoc->checkUsage(helper,sig,p)) {
	  uniques->push_back(sig);
	}
      }
    } // add if not associated to any jet
    else {
      if(!(*this)[assocIndex]->checkUsage(helper,sig,p)) {
	uniques->push_back(sig);	
      } // add if not associated to any selected object in its parent jet
    }
  }

  return uniques->asDataVector();
}
