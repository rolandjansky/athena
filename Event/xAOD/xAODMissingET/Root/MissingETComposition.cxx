/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/MissingETComposition.h"
#include "xAODJet/JetAttributes.h"

#include <cstdio>

namespace xAOD {

  // ------------------------------------------------------ generate contribution

  // MissingETComponent* MissingETComposition::Contribution::generate()
  // { return new MissingETComponent(); }

  // MissingETComponent* MissingETComposition::Contribution::generate(const MissingET* pMET)
  // { return new MissingETComponent(pMET); }

  // MissingETComponent* MissingETComposition::Contribution::generate(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::weight_t& weight,MissingETBase::Types::bitmask_t status)
  // { return new MissingETComponent(pMET,pPart,weight,status); }

  // MissingETComponent* MissingETComposition::Contribution::generate(const MissingET* pMET,const IParticle* pPart,double wpx,double wpy,double wet,MissingETBase::Types::bitmask_t status)
  // { return new MissingETComponent(pMET,pPart,wpx,wpy,wet,status); }

  // -------------------------------------------------------- insert contribution
  bool MissingETComposition::add(MissingETComponentMap* pMap,const MissingET* pMET,MissingETBase::Types::bitmask_t sw)
  {
    // input check
    if ( pMap == 0 || pMET == 0 ) 
      { printf("MissingETComposition::add - ERROR - possible invalid pointer values: MissingETComponentMap* = %p, MissingET* = %p\n",(void*)pMap,(void*)pMET); return false; }
    // check if MET object already in map
    if ( pMap->find(pMET) != pMap->end() ) 
      { printf("MissingETComposition::add - WARNING - MissingET object \042%s\042 already in map, not added again\n",pMET->name().c_str()); return false; }
    // insert object
    pMap->push_back(new MissingETComponent(pMET,sw));
    return pMap->back() != 0;
  }

  bool MissingETComposition::insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,MissingETBase::Types::weight_t weight)
  { return insert(pMap,pMET,pPart,weight.wpx(),weight.wpy(),weight.wet()); }

  bool MissingETComposition::insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,double wpx,double wpy,double wet)
  { 
    // input check
    if ( pMap == 0 || pMET == 0 )
      { printf("MissingETComposition::insert - ERROR - possible invalid pointer values: MissingETComponentMap* = %p, MissingET* = %p\n",(void*)pMap,(void*)pMET); return false; }
    // check if MET object already in map
    MissingETComponentMap::iterator fMap(pMap->find(pMET));
    if (  fMap == pMap->end() ) 
      { printf("MissingETComposition::insert - ERROR - referenced MissingET object \042%%%s\042 not yet in map, add first!\n",pMET->name().c_str()); return false; }

    (*fMap)->addObject(pPart,wpx,wpy,wet);
    return true;
  }

  bool MissingETComposition::insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart, 
				    const MissingETBase::Types::object_vector_t& signalList,MissingETBase::Types::weight_t weight,MissingETBase::UsageHandler::Policy p)
  { return insert(pMap,pMET,pPart,signalList,weight.wpx(),weight.wpy(),weight.wet(),p); }

  bool MissingETComposition::insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart, 
				    const MissingETBase::Types::object_vector_t& signalList,double wpx,double wpy,double wet,MissingETBase::UsageHandler::Policy p)
  {
    bool flag(insert(pMap,pMET,pPart,wpx,wpy,wet)); 
    if ( flag && !signalList.empty() ) { flag = pMap->addObjects(pMET,pPart,signalList,p); }
    return flag;
  }

  // ------------------------------------------ find contribution from a particle

  // It is assumed that the default constructor for DataVector<MissingETComponent>::(const_)iterator initalizes
  // with a (const MissingETComponent*)0 value (correct according to bits/stl_iterator.h). A test of the return
  // value of the functions below should then be:
  //
  // MissingETComponentMap::iterator f = MissingETComposition::find(pMap,pMET);
  // if ( *f == 0 ) { printf("MissingET object <%s> not found in map\n",pMET->name().c_str()); }
  //
  // Note that this should work because the payload of DataVector<MissingETComponent> is actually a pointer.

  MissingETComponentMap::const_iterator MissingETComposition::find(const MissingETComponentMap* pMap,const MissingET* pMET)
  { return pMap != 0 ? pMap->find(pMET) : MissingETComponentMap::const_iterator(); }

  MissingETComponentMap::iterator MissingETComposition::find(MissingETComponentMap* pMap,const MissingET* pMET)
  { return pMap != 0 ? pMap->find(pMET) : MissingETComponentMap::iterator(); }

  MissingETComponentMap::const_iterator MissingETComposition::find(const MissingETComponentMap* pMap,const std::string& metName)
  { return pMap !=0 ? pMap->find(metName) : MissingETComponentMap::const_iterator(); }

  MissingETComponentMap::iterator MissingETComposition::find(MissingETComponentMap* pMap,const std::string& metName)
  { return pMap !=0 ? pMap->find(metName) : MissingETComponentMap::iterator(); }

  MissingETComponentMap::const_iterator MissingETComposition::find(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src)
  { return pMap !=0 ? pMap->find(src) : MissingETComponentMap::const_iterator(); }

  MissingETComponentMap::iterator MissingETComposition::find(MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src)
  { return pMap !=0 ? pMap->find(src) : MissingETComponentMap::iterator(); }

  MissingETComponentMap::const_iterator MissingETComposition::find(const MissingETComponentMap* pMap,const IParticle* pPart)
  {
    if ( pMap == 0 ) { return MissingETComponentMap::const_iterator(); }
    // linear search - FIXME: method find(const IParticle*) in MissingETComponentMap ??
    MissingETComponentMap::const_iterator fCont(pMap->begin()); 
    MissingETComponentMap::const_iterator lCont(pMap->end());
    bool found(false);
    while ( fCont != lCont && !found ) { found = (*fCont)->findIndex(pPart) != MissingETBase::Numerical::invalidIndex(); if ( !found ) { ++fCont; } }
    return fCont;
  }

  MissingETComponentMap::iterator MissingETComposition::find(MissingETComponentMap* pMap,const IParticle* pPart) 
  {
    if ( pMap == 0 ) { return MissingETComponentMap::iterator(); }
    // linear search - FIXME: method find(const IParticle*) in MissingETComponentMap ??
    MissingETComponentMap::iterator fCont(pMap->begin()); 
    MissingETComponentMap::iterator lCont(pMap->end());
    bool found(false);
    while ( fCont != lCont && !found ) { found = (*fCont)->findIndex(pPart) != MissingETBase::Numerical::invalidIndex(); if ( !found ) { ++fCont; } }
    return fCont;
  }

  const MissingETComponent* MissingETComposition::getComponent(const MissingETComponentMap* pMap,const IParticle* pPart)
  { 
    if ( pMap == 0 ) { return (const MissingETComponent*)0; }
    else { MissingETComponentMap::const_iterator fCont(find(pMap,pPart)); return fCont != pMap->end() ? *fCont : (const MissingETComponent*)0; }
  } 

  MissingETComponent* MissingETComposition::getComponent(MissingETComponentMap* pMap,const IParticle* pPart)
  { 
    if ( pMap == 0 ) { return (MissingETComponent*)0; }
    else { MissingETComponentMap::iterator fCont(find(pMap,pPart)); return fCont != pMap->end() ? *fCont : (MissingETComponent*)0; }
  } 

  const MissingETComponent* MissingETComposition::getComponent(const MissingETComponentMap* pMap,const MissingET* pMET)
  { MissingETComponentMap::const_iterator fCont(find(pMap,pMET)); return fCont != pMap->end() ? *fCont : (const MissingETComponent*)0; }

  MissingETComponent* MissingETComposition::getComponent(MissingETComponentMap* pMap,const MissingET* pMET)
  { MissingETComponentMap::iterator fCont(find(pMap,pMET)); return fCont != pMap->end() ? *fCont : (MissingETComponent*)0; }

  const  MissingETComponent*  MissingETComposition::getComponent(const MissingETComponentMap* pMap,const std::string& metName)
  { MissingETComponentMap::const_iterator fCont(find(pMap,metName)); return fCont != pMap->end() ? *fCont : (const MissingETComponent*)0; }

  MissingETComponent*  MissingETComposition::getComponent(MissingETComponentMap* pMap,const std::string& metName)
  { MissingETComponentMap::iterator fCont(find(pMap,metName)); return fCont != pMap->end() ? *fCont : (MissingETComponent*)0; }

  const MissingETComponent* MissingETComposition::getComponent(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t sw)
  { MissingETComponentMap::const_iterator fCont(find(pMap,sw)); return fCont != pMap->end() ? *fCont : (const MissingETComponent*)0; }

  MissingETComponent* MissingETComposition::getComponent(MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t sw)
  { MissingETComponentMap::iterator fCont(find(pMap,sw)); return fCont != pMap->end() ? *fCont : (MissingETComponent*)0; }

  // ------------------------------------------------------------- accessing data

  // -- kinematic weights
  MissingETBase::Types::weight_t MissingETComposition::getWeight(const MissingETComponentMap* pMap,const IParticle* pPart)
  //{ return getWeight(pMap,find(pMap,pPart,status)); }
  { MissingETComponentMap::const_iterator fCont(find(pMap,pPart)); return fCont == pMap->end() ? MissingETBase::Types::weight_t(0.,0.,0.) : (*fCont)->weight(pPart); }

  MissingETBase::Types::weight_t MissingETComposition::getWeight(MissingETComponentMap::const_iterator fCont,const IParticle* pPart)
  { return *fCont != 0 ? (*fCont)->weight(pPart) : MissingETBase::Types::weight_t(0.,0.,0.); }

  // -- status word
  MissingETBase::Types::bitmask_t MissingETComposition::getStatusWord(const MissingETComponentMap* pMap,const MissingET* pmetObj)
  { MissingETComponentMap::const_iterator fCont(find(pMap,pmetObj)); return fCont == pMap->end() ? MissingETBase::Status::clearedStatus() : (*fCont)->statusWord(); }

  MissingETBase::Types::bitmask_t MissingETComposition::getStatusWord(MissingETComponentMap::const_iterator fCont)
  { return *fCont != 0 ? (*fCont)->statusWord() : MissingETBase::Status::clearedStatus(); }

  // -- MissingET object
  const MissingET* MissingETComposition::getMissingET(const MissingETComponentMap* pMap,const IParticle* pPart)
  { return getMissingET(find(pMap,pPart)); }

  const MissingET* MissingETComposition::getMissingET(MissingETComponentMap::const_iterator fCont)
  { return *fCont != 0  ? (*fCont)->metObject() : (const MissingET*)0; }

  const MissingET* MissingETComposition::getMissingET(const MissingETComponentMap* pMap,const std::string& name)
  { return getMissingET(find(pMap,name)); }

  const MissingET* MissingETComposition::getMissingET(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t sw)
  { return getMissingET(find(pMap,sw)); }

  // -------------------------------------------------------- filling MET objects



  bool MissingETComposition::add(MissingETAssociationMap* pMap,const Jet* pJet, const std::vector<ElementLink<IParticleContainer> >& jetconst,
				 const MissingETBase::Types::constvec_t& trkvec)
  {
    // input check
    if ( pMap == 0 || pJet == 0 ) 
      { printf("MissingETComposition::add - ERROR - possible invalid pointer values: MissingETAssociationMap* = %p, Jet* = %p\n",(void*)pMap,(void*)pJet); return false; }
    // check if jet already in map
    if ( pMap->find(pJet) != pMap->end() ) 
      { printf("MissingETComposition::add - WARNING - jet with index %lu already in map, not added again\n",pJet->index()); return false; }
    // insert object
    size_t nextIdx = pMap->size();
    pMap->push_back(new MissingETAssociation(pJet));
    pMap->setJetConstituents(jetconst,nextIdx);
    // printf("Jet track vector: px %f, py %f\n",trkvec.cpx(),trkvec.cpy());
    pMap->back()->setJetTrkVec(trkvec);
    return pMap->back() != 0;
  }

  bool MissingETComposition::add(MissingETAssociationMap* pMap,const Jet* pJet, const std::vector<const IParticle*>& jettracks)
  {
    // input check
    if ( pMap == 0 || pJet == 0 ) 
      { printf("MissingETComposition::add - ERROR - possible invalid pointer values: MissingETAssociationMap* = %p, Jet* = %p\n",(void*)pMap,(void*)pJet); return false; }
    // check if jet already in map
    if ( pMap->find(pJet) != pMap->end() ) 
      { printf("MissingETComposition::add - WARNING - jet with index %lu already in map, not added again\n",pJet->index()); return false; }
    // insert object
    // size_t nextIdx = pMap->size();
    std::vector<ElementLink<IParticleContainer> > jetconst = pJet->constituentLinks();
    MissingETBase::Types::constvec_t trkvec;
    if(jettracks.size()>0) {
      const IParticleContainer* pTrkCont = static_cast<const IParticleContainer*>(jettracks.front()->container());
      for(const auto& trk : jettracks) {  
    	ElementLink<IParticleContainer> link(*pTrkCont,trk->index());
    	jetconst.push_back(link);
    	trkvec += *trk;
      }
    }
    return add(pMap,pJet,jetconst,trkvec);
  }

  bool MissingETComposition::addMiscAssociation(MissingETAssociationMap* pMap)
  {
    // input check
    if ( pMap == 0 ) 
      { printf("MissingETComposition::addMiscAssociation - ERROR - possible invalid pointer values: MissingETAssociationMap* = %p\n",(void*)pMap); return false; }
    // insert object
    pMap->push_back(new MissingETAssociation((const Jet*)NULL,true));
    return pMap->back() != 0;
  }

  bool MissingETComposition::insert(MissingETAssociationMap* pMap,size_t jetIndex,const IParticle* pPart,const std::vector<const IParticle*>& constlist)
  { 
    // input check
    if ( pMap == 0 )
      { printf("MissingETComposition::insert - ERROR - possible invalid pointer value: MissingETAssociationMap* = %p\n",(void*)pMap); return false; }
    // check if MET object already in map

    // printf("MissingETComposition::insert - INFO - associated to jet %lu\n",jetIndex);

    if ( jetIndex==MissingETBase::Numerical::invalidIndex() || jetIndex>=pMap->size())
      { printf("MissingETComposition::insert - WARNING - no MissingETAssociation matching this jet, could not add\n"); return false; }

    (*pMap)[jetIndex]->addObject(pPart,constlist);
    return true;
  }

  bool MissingETComposition::insert(MissingETAssociationMap* pMap,const Jet* pJet,const IParticle* pPart,const std::vector<const IParticle*>& constlist)
  { 
    // input check
    if ( pMap == 0 || pJet == 0 )
      { printf("MissingETComposition::insert - ERROR - possible invalid pointer values: MissingETAssociationMap* = %p, Jet* = %p\n",(void*)pMap,(void*)pJet); return false; }
    // check if jet already in map
    size_t jetIndex(pMap->findIndex(pJet));
    if ( jetIndex==MissingETBase::Numerical::invalidIndex())
      { printf("MissingETComposition::insert - WARNING - jet with pointer %p not in map, could not add\n",(void*)pJet); return false; }

    return insert(pMap,jetIndex,pPart,constlist);
  }

  bool MissingETComposition::insert(MissingETAssociationMap* pMap,const IParticle* pPart,const std::vector<const IParticle*>& constlist,
				    std::map<const IParticle*,MissingETBase::Types::constvec_t> pOverride)
  { 
    // input check
    if ( pMap == 0 )
      { printf("MissingETComposition::insert - ERROR - possible invalid pointer values: MissingETAssociationMap* = %p\n",(void*)pMap); return false; }
    
    // loop over constituents and try to identify an appropriate association
    // FIXME: check in case more than one appropriate index found?
    std::map<size_t,std::vector<const IParticle*> > constMap;
    for(const auto& signal : constlist) {
      size_t jetIndex = pMap->findIndexByJetConst(signal);
      // printf("Constituent type %i, jetIndex %lu\n",signal->type(),jetIndex);
      constMap[jetIndex].push_back(signal);
    }
    std::map<size_t,std::vector<const IParticle*> >::iterator jetIter;
    for(auto& jetIter : constMap) {
      if(jetIter.first!=MissingETBase::Numerical::invalidIndex()) {
        insert(pMap,jetIter.first,pPart,jetIter.second);
	// printf("Associated object (type %d, pt %f) with jet %lu\n",pPart->type(),pPart->pt(),jetIter.first);
        (*pMap)[jetIter.first]->addOverrideMom(pOverride);
      } else {
	insertMisc(pMap,pPart,jetIter.second);
        pMap->getMiscAssociation()->addOverrideMom(pOverride);
      }
    }
    if (!constMap.size()) insertMisc(pMap,pPart,std::vector<const IParticle*>());
    return true;
  }

  bool MissingETComposition::setJetConstSum(MissingETAssociationMap* metMap,const Jet* jet,const std::vector<const IParticle*>& altConsts,
				    std::map<const IParticle*,MissingETBase::Types::constvec_t> pOverride) {
    std::vector<ElementLink<IParticleContainer> > jetconst;
    if(altConsts.size()>0) {
      for(const auto& alt : altConsts) {  
        const IParticleContainer* pCont = static_cast<const IParticleContainer*>(alt->container());
    	ElementLink<IParticleContainer> link(*pCont,alt->index());
    	jetconst.push_back(link);
      }
    }
    metMap->setJetConstituents(jetconst,jet);
    if (altConsts.size()>0) insert(metMap,jet,altConsts,pOverride);
    else insert(metMap,jet,jet,altConsts);
    return true;
  }

  bool MissingETComposition::insertMisc(MissingETAssociationMap* pMap,const IParticle* pPart,const std::vector<const IParticle*>& constlist)
  { 
    // input check
    if ( pMap == 0 )
      { printf("MissingETComposition::insertMisc - ERROR - possible invalid pointer values: MissingETAssociationMap* = %p\n",(void*)pMap); return false; }
    
    size_t jetIndex(MissingETBase::Numerical::invalidIndex());
    // loop over constituents and make sure these don't belong to some other association
    // printf("Associated object (type %d, pt %f) with no jet\n",pPart->type(),pPart->pt());
    for(const auto& signal : constlist) {
      // printf("MissingETComposition::insertMisc - INFO - constituent pt %f\n", signal->pt());
      jetIndex = pMap->findIndexByJetConst(signal);
      if(jetIndex!=MissingETBase::Numerical::invalidIndex()) {
	printf("MissingETComposition::insertMisc - ERROR - object %p with constituent %p is associated to jet %lu, will not place in misc association\n",(void*)pPart,(void*)signal,jetIndex); return false;
      }
    }
    pMap->getMiscAssociation()->addObject(pPart,constlist);

    return true;
  }

  MissingETAssociationMap::const_iterator MissingETComposition::find(const MissingETAssociationMap* pMap,const Jet* pJet)
  { return pMap != 0 ? pMap->find(pJet) : MissingETAssociationMap::const_iterator(); }

  MissingETAssociationMap::iterator       MissingETComposition::find(MissingETAssociationMap* pMap,const Jet* pJet)
  { return pMap != 0 ? pMap->find(pJet) : MissingETAssociationMap::iterator(); }

  MissingETAssociationMap::const_iterator MissingETComposition::find(const MissingETAssociationMap* pMap,const IParticle* pPart)
  {
    if ( pMap == 0 ) { return MissingETAssociationMap::const_iterator(); }
    // linear search - FIXME: method find(const IParticle*) in MissingETAssociationMap ??
    MissingETAssociationMap::const_iterator fAssoc(pMap->begin());
    MissingETAssociationMap::const_iterator lAssoc(pMap->end());
    bool found(false);
    while ( fAssoc != lAssoc && !found ) { found = (*fAssoc)->findIndex(pPart) != MissingETBase::Numerical::invalidIndex(); if ( !found ) { ++fAssoc; } }
    return fAssoc;
  }

  MissingETAssociationMap::iterator MissingETComposition::find(MissingETAssociationMap* pMap,const IParticle* pPart)
  {
    if ( pMap == 0 ) { return MissingETAssociationMap::iterator(); }
    // linear search - FIXME: method find(const IParticle*) in MissingETAssociationMap ??
    MissingETAssociationMap::iterator fAssoc(pMap->begin()); 
    MissingETAssociationMap::iterator lAssoc(pMap->end());
    bool found(false);
    while ( fAssoc != lAssoc && !found ) { found = (*fAssoc)->findIndex(pPart) != MissingETBase::Numerical::invalidIndex(); if ( !found ) { ++fAssoc; } }
    return fAssoc;
  }

  MissingETBase::Types::constvec_t MissingETComposition::getConstVec(const MissingETAssociationMap* pMap,const IParticle* pPart,MissingETBase::UsageHandler::Policy p)
  { 
    MissingETBase::Types::constvec_t totalvec;
    std::vector<const MissingETAssociation*> assocs = getAssociations(pMap,pPart);
    for (size_t i = 0; i < assocs.size(); i++) {
      if (p==MissingETBase::UsageHandler::OnlyTrack) {
	totalvec += assocs[i]->trkVec(pPart);
      } else {
	totalvec += assocs[i]->calVec(pPart);
      }
    }
    return totalvec;
  } 
  
  bool MissingETComposition::objSelected(const MissingETAssociationHelper& helper,const IParticle* obj) {
    std::vector<const MissingETAssociation*> assocs = getAssociations(helper.map(),obj);
    for(size_t i = 0; i < assocs.size(); i++) if(helper.objSelected(assocs[i],obj)) return true;
    return false;
  }


  bool MissingETComposition::selectIfNoOverlaps(MissingETAssociationHelper& helper,const IParticle* obj,MissingETBase::UsageHandler::Policy p) {
    std::vector<const MissingETAssociation*> assocs = getAssociations(helper.map(),obj);
    bool overlaps(false);
    for(size_t i = 0; i < assocs.size(); i++) overlaps |= assocs[i]->hasOverlaps(helper,obj,p);
    if (overlaps) return false; 
    for(size_t i = 0; i < assocs.size(); i++) helper.setObjSelectionFlag(assocs[i],obj,true);
    return true;
  }

  std::vector<const MissingETAssociation*> MissingETComposition::getAssociations(const MissingETAssociationMap* pMap,const IParticle* pPart)
  { 
    std::vector<const MissingETAssociation*> assocs;
    if ( pMap == 0 ) { return assocs; }
    else { 
      for (MissingETAssociationMap::const_iterator fAssoc = pMap->begin();fAssoc!=pMap->end(); fAssoc++) {
	if ((*fAssoc)->findIndex(pPart) != MissingETBase::Numerical::invalidIndex()) assocs.push_back(*fAssoc);
      }
    }
    return assocs;
  } 

  std::vector<MissingETAssociation*> MissingETComposition::getAssociations(MissingETAssociationMap* pMap,const IParticle* pPart)
  { 
    std::vector<MissingETAssociation*> assocs;
    if ( pMap == 0 ) { return assocs; }
    else { 
      for (MissingETAssociationMap::iterator fAssoc = pMap->begin();fAssoc!=pMap->end(); fAssoc++) {
	if ((*fAssoc)->findIndex(pPart) != MissingETBase::Numerical::invalidIndex()) assocs.push_back(*fAssoc);
      }
    }
    return assocs;
  } 

  const MissingETAssociation* MissingETComposition::getAssociation(const MissingETAssociationMap* pMap,const Jet* pJet)
  { MissingETAssociationMap::const_iterator fAssoc(find(pMap,pJet)); return fAssoc != pMap->end() ? *fAssoc : (const MissingETAssociation*)0; }

  MissingETAssociation* MissingETComposition::getAssociation(MissingETAssociationMap* pMap,const Jet* pJet)
  { MissingETAssociationMap::iterator fAssoc(find(pMap,pJet)); return fAssoc != pMap->end() ? *fAssoc : (MissingETAssociation*)0; }

  // MissingETBase::Types::constvec_t MissingETComposition::getConstVec(const MissingETAssociationMap* pMap,const IParticle* pPart);

  // MissingETBase::Types::constvec_t MissingETComposition::getConstVec(MissingETAssociationMap::const_iterator fAssoc,const IParticle* pPart);

  const Jet* MissingETComposition::getRefJet(const MissingETAssociationMap* pMap,const IParticle* pPart)
  {
    MissingETAssociationMap::const_iterator fAssoc = find(pMap,pPart);
    return fAssoc==pMap->end() ? 0 : getRefJet(fAssoc);
  }

  const Jet* MissingETComposition::getRefJet(MissingETAssociationMap::const_iterator fAssoc)
  { return *fAssoc != 0  ? (*fAssoc)->refJet() : (const Jet*) 0; }
}
