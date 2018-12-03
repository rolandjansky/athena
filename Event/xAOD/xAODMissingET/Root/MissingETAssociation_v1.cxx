/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/versions/MissingETAssociation_v1.h"
#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODPFlow/PFO.h"

#include <iterator>
#include <cstdio>

using namespace MissingETBase::Types;
using std::vector;

namespace xAOD {

  MissingETBase::Types::bitmask_t getObjMask(size_t objIndex)
  { 
    if(objIndex==MissingETBase::Numerical::invalidIndex()) return 0;
    return 1<<objIndex;
  }

  ///////////////////////////////////////////////////
  // MissingETAssociation_v1::ConstVec Implementations //
  ///////////////////////////////////////////////////

  MissingETAssociation_v1::ConstVec& MissingETAssociation_v1::ConstVec::operator+=(const ConstVec& cvec)
  {
    this->cpx() += cvec.cpx(); this->cpy() += cvec.cpy(); this->cpz() += cvec.cpz(); this->ce() += cvec.ce();
    this->sumpt() += cvec.sumpt();
    return *this;
  }

  MissingETAssociation_v1::ConstVec& MissingETAssociation_v1::ConstVec::operator-=(const ConstVec& cvec)
  {
    this->cpx() -= cvec.cpx(); this->cpy() -= cvec.cpy(); this->cpz() -= cvec.cpz(); this->ce() -= cvec.ce();
    this->sumpt() -= cvec.sumpt();
    return *this;
  }

  MissingETAssociation_v1::ConstVec& MissingETAssociation_v1::ConstVec::operator*=(double scale)
  {
    this->cpx() *= scale; this->cpy() *= scale; this->cpz() *= scale; this->ce() *= scale;
    this->sumpt() *= scale;
    return *this;
  }

  MissingETAssociation_v1::ConstVec& MissingETAssociation_v1::ConstVec::operator/=(double scale)
  {
    if ( scale <1e-9 ) { this->cpx() = 0.; this->cpy() = 0.; this->cpz() = 0.; this->ce() = 0.; this->sumpt() = 0.; return *this; }
    else { return this->operator*=(1./scale); }
  }

  bool MissingETAssociation_v1::ConstVec::operator==(const ConstVec& cvec) const
  {
    return
      MissingETBase::Numerical::isEqual(this->cpx(),cvec.cpx()) &&
      MissingETBase::Numerical::isEqual(this->cpy(),cvec.cpy()) &&
      MissingETBase::Numerical::isEqual(this->cpz(),cvec.cpz()) &&
      MissingETBase::Numerical::isEqual(this->ce(),cvec.ce()) &&
      MissingETBase::Numerical::isEqual(this->sumpt(),cvec.sumpt());
  }

  double MissingETAssociation_v1::ConstVec::cpt() const
  {
    return sqrt(this->cpx()*this->cpx()+this->cpy()*this->cpy());
  }

  ///////////////////////////////
  // Internally used constants //
  ///////////////////////////////

  size_t MissingETAssociation_v1::m_objConstLinkReserve = 50; 
  size_t MissingETAssociation_v1::m_contribObjReserve = 10;

  //////////////////////////////////
  // Constructors and destructors //
  //////////////////////////////////

  MissingETAssociation_v1::MissingETAssociation_v1( bool createStore )
    : SG::AuxElement(),
      m_lastObjectPointer( 0 ),
      m_lastObjectIndex( MissingETBase::Numerical::invalidIndex() )
  {

    if( createStore ) createPrivateStore();
    initCache();
  }

  MissingETAssociation_v1::MissingETAssociation_v1( const Jet* pJet, bool isMisc )
    : SG::AuxElement(),
      m_lastObjectPointer( 0 ),
      m_lastObjectIndex( MissingETBase::Numerical::invalidIndex() )
  {

    createPrivateStore();
    f_isMisc() = isMisc;
    setRefJet( pJet );
    initCache();
  }

  MissingETAssociation_v1::MissingETAssociation_v1(const MissingETAssociation_v1& assocDescr)
    : SG::AuxElement()
    , m_lastObjectPointer(assocDescr.m_lastObjectPointer)
    , m_lastObjectIndex(assocDescr.m_lastObjectIndex)
  {
    this->makePrivateStore(&assocDescr);
    initCache();
  }

  MissingETAssociation_v1& MissingETAssociation_v1::operator=(const MissingETAssociation_v1& assocDescr)
  {
    if((&assocDescr) != this) { 
      m_lastObjectPointer = assocDescr.m_lastObjectPointer;
      m_lastObjectIndex   = assocDescr.m_lastObjectIndex;
      f_isMisc() = assocDescr.isMisc();
      this->setJetLink(assocDescr.jetLink());
      this->setObjectLinks(assocDescr.objectLinks());
      this->setOverlaps(assocDescr.overlapIndices(),assocDescr.overlapTypes());
      this->setCalPx(assocDescr.calpx());
      this->setCalPy(assocDescr.calpy());
      this->setCalPz(assocDescr.calpz());
      this->setCalE(assocDescr.cale());  
      this->setCalSumpt(assocDescr.calsumpt());  
      this->setCalKey(assocDescr.calkey());
      this->setTrkPx(assocDescr.trkpx());
      this->setTrkPy(assocDescr.trkpy());
      this->setTrkPz(assocDescr.trkpz());
      this->setTrkE(assocDescr.trke());  
      this->setTrkSumpt(assocDescr.trksumpt());  
      this->setTrkKey(assocDescr.trkkey());  
      this->setJetTrkPx(assocDescr.jettrkpx());
      this->setJetTrkPy(assocDescr.jettrkpy());
      this->setJetTrkPz(assocDescr.jettrkpz());
      this->setJetTrkE(assocDescr.jettrke());
    }
    return  *this;
  }

  MissingETAssociation_v1::~MissingETAssociation_v1() { }

  ///////////////////////////
  // Data store management //
  ///////////////////////////

  bool MissingETAssociation_v1::addObject(const IParticle* pPart,const std::vector<const IParticle*>& constlist)
  {
    // check if object is in store

    size_t idx(this->findIndex(pPart));
    // found - add const vectors
    if ( idx != MissingETBase::Numerical::invalidIndex() )
      { 
	m_objConstLinks[idx].clear();
	m_objConstLinks[idx].reserve(constlist.size());
	for(const auto& signal : constlist) {
	  const IParticleContainer* pCont = static_cast<const IParticleContainer*>(signal->container());
	  MissingETBase::Types::objlink_t el(*pCont,signal->index());
	  m_objConstLinks[idx].push_back(el);
	}
	return false; }
    // new object
    else
      {
	MissingETBase::Types::objlink_t oLnk; f_setObject<IParticle,MissingETBase::Types::objlink_t>(pPart,oLnk); 
	// add to stores
	this->f_objectLinks().push_back(oLnk);
	vector<MissingETBase::Types::objlink_t > linklist;
	linklist.reserve(constlist.size());
	for(const auto& signal : constlist) {
	  const IParticleContainer* pCont = static_cast<const IParticleContainer*>(signal->container());
	  MissingETBase::Types::objlink_t el(*pCont,signal->index());
	  linklist.push_back(el);
	}
	m_objConstLinks.push_back(linklist);
	this->f_overlapIndices().push_back(vector<size_t>(0));
	this->f_overlapTypes().push_back(vector<unsigned char>(0));
	bool linkset = f_setLink<MissingETBase::Types::objlink_t>(oLnk);
	return linkset;
      }
  }

  // void MissingETAssociation_v1::updateLinks()
  // {
  //   this->updateJetLink();
  //   static SG::AuxElement::Accessor<MissingETBase::Types::objlink_vector_t> acc("objectLinks");
  //   if(acc.isAvailableWritable(*this)) {
  //     for ( auto& link : this->f_objectLinks() ) { 
  // 	link.toPersistent();
  //     } 
  //   }
  // }

  // void MissingETAssociation_v1::updateJetLink() {
  //   if(!this->isMisc()) {
  //     static SG::AuxElement::Accessor<MissingETBase::Types::jetlink_t> acc("jetLink");
  //     if(acc.isAvailableWritable(*this)) {
  // 	if(f_setLink<MissingETBase::Types::jetlink_t>(this->f_jetLink())) {
  // 	  this->f_jetLink().toPersistent();
  // 	}
  //     }
  //   }
  // }

  bool MissingETAssociation_v1::removeContrib()
  {
    if ( this->empty() ) { return false; }
    else 
      { size_t nEnt(this->size()); for  ( size_t objIdx(0); objIdx<nEnt; ++objIdx ) { this->f_removeContrib(objIdx); } return true; }
  }

  bool MissingETAssociation_v1::f_removeContrib(size_t objIdx)
  {
    objlink_vector_t::iterator fCon(this->f_objectLinks().begin()); std::advance<objlink_vector_t::iterator>(fCon,objIdx);
    this->f_objectLinks().erase(fCon);
    vector<vector<size_t> >::iterator fOverlapIndices(this->f_overlapIndices().begin()); std::advance<vector<vector<size_t> >::iterator>(fOverlapIndices,objIdx);
    this->f_overlapIndices().erase(fOverlapIndices);
    vector<vector<unsigned char> >::iterator fOverlapTypes(this->f_overlapTypes().begin()); std::advance<vector<vector<unsigned char> >::iterator>(fOverlapTypes,objIdx);
    this->f_overlapTypes().erase(fOverlapTypes);
    this->f_calpx().clear();
    this->f_calpy().clear();
    this->f_calpz().clear();
    this->f_cale().clear();
    this->f_calsumpt().clear();
    this->f_calkey().clear();
    this->f_trkpx().clear();
    this->f_trkpy().clear();
    this->f_trkpz().clear();
    this->f_trke().clear();
    this->f_trksumpt().clear();
    this->f_trkkey().clear();
    return true;
  }

  bool MissingETAssociation_v1::resetContrib(size_t /*objIdx*/)
  {
    this->resetContrib();
    return true;
  }

  bool MissingETAssociation_v1::resetContrib() 
  {
    for (size_t idx = 0; idx < this->size(); idx++) this->f_overlapIndices()[idx].clear();
    for (size_t idx = 0; idx < this->size(); idx++) this->f_overlapTypes()[idx].clear();
    this->f_calpx().clear();
    this->f_calpy().clear();
    this->f_calpz().clear();
    this->f_cale().clear();
    this->f_calsumpt().clear();
    this->f_calkey().clear();
    this->f_trkpx().clear();
    this->f_trkpy().clear();
    this->f_trkpz().clear();
    this->f_trke().clear();
    this->f_trksumpt().clear();
    this->f_trkkey().clear();
    return true;
  }

  size_t MissingETAssociation_v1::findIndex(const IParticle* pPart) const
  {
    // printf("Misc assoc? %d\n", this->isMisc());
    // printf("Particle %p (pt %f), lastobjptr %p\n",(void*)pPart, pPart->pt(), (void*)m_lastObjectPointer);
    if ( m_lastObjectPointer != pPart ) {
      objlink_vector_t::const_iterator fLnk(this->objectLinks().begin());
      // FIXME: This is based on dereferencing the ElementLink, which is risky.
      while ( fLnk != this->objectLinks().end() ) {
	if((*fLnk).isValid()) {
	  // printf("Compare with particle %p (type %d, pt %f)\n", (void*) **fLnk,(**fLnk)->type(),(**fLnk)->pt() );
	  if(*(*fLnk) == pPart) break;
	}
	++fLnk;
      }
      if ( fLnk != this->objectLinks().end() ) 
	{ m_lastObjectPointer = pPart; m_lastObjectIndex = std::distance(this->objectLinks().begin(),fLnk); }
      else
	{ m_lastObjectPointer = (const IParticle*)0; m_lastObjectIndex = MissingETBase::Numerical::invalidIndex();  }
    }
    return m_lastObjectIndex;
  }

  size_t MissingETAssociation_v1::findCalIndex(MissingETBase::Types::bitmask_t mask) const
  {
    for (size_t ind = 0; ind < this->sizeCal(); ind++) if (mask==this->calkey()[ind]) return ind;
    return MissingETBase::Numerical::invalidIndex();
  }

  size_t MissingETAssociation_v1::findTrkIndex(MissingETBase::Types::bitmask_t mask) const
  {
    for (size_t ind = 0; ind < this->sizeTrk(); ind++) if (mask==this->trkkey()[ind]) return ind;
    return MissingETBase::Numerical::invalidIndex();
  }

  /////////////
  // Setters //
  /////////////

  bool MissingETAssociation_v1::setCalPx(const std::vector<double>& calpxvec)
  { this->f_calpx().clear(); this->f_calpx().insert(this->f_calpx().end(),calpxvec.begin(),calpxvec.end()); return !this->f_calpx().empty(); }
  bool MissingETAssociation_v1::setCalPy(const std::vector<double>& calpyvec)
  { this->f_calpy().clear(); this->f_calpy().insert(this->f_calpy().end(),calpyvec.begin(),calpyvec.end()); return !this->f_calpy().empty(); }
  bool MissingETAssociation_v1::setCalPz(const std::vector<double>& calpzvec)
  { this->f_calpz().clear(); this->f_calpz().insert(this->f_calpz().end(),calpzvec.begin(),calpzvec.end()); return !this->f_calpz().empty(); }
  bool MissingETAssociation_v1::setCalE(const std::vector<double>& calevec)
  { this->f_cale().clear(); this->f_cale().insert(this->f_cale().end(),calevec.begin(),calevec.end()); return !this->f_cale().empty(); }
  bool MissingETAssociation_v1::setCalSumpt(const std::vector<double>& calsumptvec)
  { this->f_calsumpt().clear(); this->f_calsumpt().insert(this->f_calsumpt().end(),calsumptvec.begin(),calsumptvec.end()); return !this->f_calsumpt().empty(); }
  bool MissingETAssociation_v1::setCalKey(const std::vector<MissingETBase::Types::bitmask_t>& calkeyvec)
  { this->f_calkey().clear(); this->f_calkey().insert(this->f_calkey().end(),calkeyvec.begin(),calkeyvec.end()); return !this->f_calkey().empty(); }

  bool MissingETAssociation_v1::setTrkPx(const std::vector<double>& trkpxvec)
  { this->f_trkpx().clear(); this->f_trkpx().insert(this->f_trkpx().end(),trkpxvec.begin(),trkpxvec.end()); return !this->f_trkpx().empty(); }
  bool MissingETAssociation_v1::setTrkPy(const std::vector<double>& trkpyvec)
  { this->f_trkpy().clear(); this->f_trkpy().insert(this->f_trkpy().end(),trkpyvec.begin(),trkpyvec.end()); return !this->f_trkpy().empty(); }
  bool MissingETAssociation_v1::setTrkPz(const std::vector<double>& trkpzvec)
  { this->f_trkpz().clear(); this->f_trkpz().insert(this->f_trkpz().end(),trkpzvec.begin(),trkpzvec.end()); return !this->f_trkpz().empty(); }
  bool MissingETAssociation_v1::setTrkE(const std::vector<double>& trkevec)
  { this->f_trke().clear(); this->f_trke().insert(this->f_trke().end(),trkevec.begin(),trkevec.end()); return !this->f_trke().empty(); }
  bool MissingETAssociation_v1::setTrkSumpt(const std::vector<double>& trksumptvec)
  { this->f_trksumpt().clear(); this->f_trksumpt().insert(this->f_trksumpt().end(),trksumptvec.begin(),trksumptvec.end()); return !this->f_trksumpt().empty(); }
  bool MissingETAssociation_v1::setTrkKey(const std::vector<MissingETBase::Types::bitmask_t>& trkkeyvec)
  { this->f_trkkey().clear(); this->f_trkkey().insert(this->f_trkkey().end(),trkkeyvec.begin(),trkkeyvec.end()); return !this->f_trkkey().empty(); }

  //////////
  // FIXME: failsafe implementation checks on invalidKey and vector index - vector index may be sufficient?
  bool MissingETAssociation_v1::setCalPx(size_t keyIdx,double calpx)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_calpx().size() ) { this->f_calpx()[keyIdx] = calpx; return true; } else { return false; } }
  bool MissingETAssociation_v1::setCalPy(size_t keyIdx,double calpy)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_calpy().size() ) { this->f_calpy()[keyIdx] = calpy; return true; } else { return false; } }
  bool MissingETAssociation_v1::setCalPz(size_t keyIdx,double calpz)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_calpz().size() ) { this->f_calpz()[keyIdx] = calpz; return true; } else { return false; } }
  bool MissingETAssociation_v1::setCalE(size_t keyIdx,double cale)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_cale().size() ) { this->f_cale()[keyIdx] = cale; return true; } else { return false; } }
  bool MissingETAssociation_v1::setCalSumpt(size_t keyIdx,double calsumpt)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_calsumpt().size() ) { this->f_calsumpt()[keyIdx] = calsumpt; return true; } else { return false; } }
  bool MissingETAssociation_v1::setCalKey(size_t keyIdx,MissingETBase::Types::bitmask_t calkey)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_calkey().size() ) { this->f_calkey()[keyIdx] = calkey; return true; } else { return false; } }
  bool MissingETAssociation_v1::setCalVec(size_t keyIdx,double calpx,double calpy,double calpz,double cale,double sumpt)
  {
    if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_cale().size() ) {
      this->f_calpx()[keyIdx] = calpx;  this->f_calpy()[keyIdx] = calpy;
      this->f_calpz()[keyIdx] = calpz;  this->f_cale()[keyIdx] = cale;
      this->f_calsumpt()[keyIdx] = sumpt; return true;
    } else { return false; }
  }
  bool MissingETAssociation_v1::addCalVec(MissingETBase::Types::bitmask_t key,double calpx,double calpy,double calpz,double cale,double sumpt) {
    for(MissingETBase::Types::bitmask_t testkey : this->f_calkey()) {
      // quit if key already exists.
      if(key==testkey) return false;
    }
    this->f_calkey().push_back(key);
    this->f_calpx().push_back(calpx);
    this->f_calpy().push_back(calpy);
    this->f_calpz().push_back(calpz);
    this->f_cale().push_back(cale);
    this->f_calsumpt().push_back(sumpt);
    return true;
  }

  bool MissingETAssociation_v1::setTrkPx(size_t keyIdx,double trkpx)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trkpx().size() ) { this->f_trkpx()[keyIdx] = trkpx; return true; } else { return false; } }
  bool MissingETAssociation_v1::setTrkPy(size_t keyIdx,double trkpy)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trkpy().size() ) { this->f_trkpy()[keyIdx] = trkpy; return true; } else { return false; } }
  bool MissingETAssociation_v1::setTrkPz(size_t keyIdx,double trkpz)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trkpz().size() ) { this->f_trkpz()[keyIdx] = trkpz; return true; } else { return false; } }
  bool MissingETAssociation_v1::setTrkE(size_t keyIdx,double trke)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trke().size() ) { this->f_trke()[keyIdx] = trke; return true; } else { return false; } }
  bool MissingETAssociation_v1::setTrkSumpt(size_t keyIdx,double trksumpt)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trksumpt().size() ) { this->f_trksumpt()[keyIdx] = trksumpt; return true; } else { return false; } }
  bool MissingETAssociation_v1::setTrkKey(size_t keyIdx,MissingETBase::Types::bitmask_t trkkey)
  { if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trkkey().size() ) { this->f_trkkey()[keyIdx] = trkkey; return true; } else { return false; } }
  bool MissingETAssociation_v1::setTrkVec(size_t keyIdx,double trkpx,double trkpy,double trkpz,double trke,double sumpt)
  { 
    if ( keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->f_trke().size() ) {
      this->f_trkpx()[keyIdx] = trkpx;  this->f_trkpy()[keyIdx] = trkpy;
      this->f_trkpz()[keyIdx] = trkpz;  this->f_trke()[keyIdx] = trke;
      this->f_trksumpt()[keyIdx] = sumpt; return true; 
    } else { return false; }
  }
  bool MissingETAssociation_v1::addTrkVec(MissingETBase::Types::bitmask_t key,double trkpx,double trkpy,double trkpz,double trke,double sumpt) {
    for(MissingETBase::Types::bitmask_t testkey : this->f_trkkey()) {
      // quit if key already exists.
      if(key==testkey) return false;
    }
    this->f_trkkey().push_back(key);
    this->f_trkpx().push_back(trkpx);
    this->f_trkpy().push_back(trkpy);
    this->f_trkpz().push_back(trkpz);
    this->f_trke().push_back(trke);
    this->f_trksumpt().push_back(sumpt);
    return true;
  }


  void MissingETAssociation_v1::setJetTrkPx(double px)
  { this->f_jettrkpx() = px; }
  void MissingETAssociation_v1::setJetTrkPy(double py)
  { this->f_jettrkpy() = py; }
  void MissingETAssociation_v1::setJetTrkPz(double pz)
  { this->f_jettrkpz() = pz; }
  void MissingETAssociation_v1::setJetTrkE(double e)
  { this->f_jettrke() = e; }
  void MissingETAssociation_v1::setJetTrkSumpt(double sumpt)
  { this->f_jettrksumpt() = sumpt; }
  void MissingETAssociation_v1::setJetTrkVec(double px,double py,double pz,double e,double sumpt)
  {
    this->f_jettrkpx() = px;  this->f_jettrkpy() = py;
    this->f_jettrkpz() = pz;  this->f_jettrke() = e; 
    this->f_jettrksumpt() = sumpt; 
  }

  void MissingETAssociation_v1::clearOverlaps()
  {
    this->f_overlapIndices().clear();
    this->f_overlapIndices().resize(this->f_overlapIndices().size(),vector<size_t>(0));
    this->f_overlapTypes().clear();
    this->f_overlapTypes().resize(this->f_overlapTypes().size(),vector<unsigned char>(0));
  }
  bool MissingETAssociation_v1::setOverlaps(const std::vector<std::vector<size_t> >& overlapIndexVector,const std::vector<std::vector<unsigned char> >& overlapTypeVector)
  {
    this->f_overlapIndices().clear(); this->f_overlapIndices().insert(this->f_overlapIndices().end(),overlapIndexVector.begin(),overlapIndexVector.end());
    this->f_overlapTypes().clear(); this->f_overlapTypes().insert(this->f_overlapTypes().end(),overlapTypeVector.begin(),overlapTypeVector.end());
    return !this->f_overlapIndices().empty();
  }
  bool MissingETAssociation_v1::setOverlaps(size_t objIdx,const std::vector<size_t>& overlapIndices,const std::vector<unsigned char>& overlapTypes)
  {
    if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_overlapIndices().size() ) {
      this->f_overlapIndices()[objIdx] = overlapIndices;
      this->f_overlapTypes()[objIdx] = overlapTypes;
      return true; 
    } else { return false; }
  }


  bool MissingETAssociation_v1::setRefJet(const Jet* pJet)
  { 
    bool wasSet(this->f_jetLink().getStorableObjectPointer() != 0);
    f_setObject<Jet,MissingETBase::Types::jetlink_t>(pJet,this->f_jetLink());
    return wasSet;
  }

  bool MissingETAssociation_v1::setRefJet(const JetContainer* pJetCont, size_t pJetIdx)
  {
    bool wasSet(this->f_jetLink().getStorableObjectPointer() != 0);
    this->f_jetLink().toIndexedElement(*pJetCont,pJetIdx);
    return wasSet;
  }

  bool MissingETAssociation_v1::setJetLink(const MissingETBase::Types::jetlink_t& jetLnk)
  {
    bool wasSet(this->f_jetLink().getStorableObjectPointer() != 0);
    this->f_jetLink() = jetLnk;
    return wasSet;
  }

  bool MissingETAssociation_v1::setObjectLinks(const MissingETBase::Types::objlink_vector_t& objLnks)
  { 
    bool wasSet(!this->f_objectLinks().empty()); 
    if ( wasSet ) {  this->f_objectLinks().clear(); this->f_objectLinks().insert(this->f_objectLinks().end(),objLnks.begin(),objLnks.end()); }
    return wasSet;
  }


  /////////////
  // Getters //
  /////////////
  
  double MissingETAssociation_v1::calpx(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() ? this->calpx()[keyIdx] : MissingETBase::Numerical::cpxError(); }
  double MissingETAssociation_v1::calpy(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() ? this->calpy()[keyIdx] : MissingETBase::Numerical::cpyError(); }
  double MissingETAssociation_v1::calpz(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() ? this->calpz()[keyIdx] : MissingETBase::Numerical::cpzError(); }
  double MissingETAssociation_v1::cale(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() ? this->cale()[keyIdx] : MissingETBase::Numerical::ceError(); }
  double MissingETAssociation_v1::calsumpt(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() ? this->calsumpt()[keyIdx] : MissingETBase::Numerical::sumptError(); }
  MissingETBase::Types::bitmask_t MissingETAssociation_v1::calkey(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() ? this->calkey()[keyIdx] : MissingETBase::Numerical::invalidIndex(); }

  double MissingETAssociation_v1::trkpx(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() ? this->trkpx()[keyIdx] : MissingETBase::Numerical::cpxError(); }
  double MissingETAssociation_v1::trkpy(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() ? this->trkpy()[keyIdx] : MissingETBase::Numerical::cpyError(); }
  double MissingETAssociation_v1::trkpz(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() ? this->trkpz()[keyIdx] : MissingETBase::Numerical::cpzError(); }
  double MissingETAssociation_v1::trke(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() ? this->trke()[keyIdx] : MissingETBase::Numerical::ceError(); }
  double MissingETAssociation_v1::trksumpt(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() ? this->trksumpt()[keyIdx] : MissingETBase::Numerical::sumptError(); }
  MissingETBase::Types::bitmask_t MissingETAssociation_v1::trkkey(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() ? this->trkkey()[keyIdx] : MissingETBase::Numerical::invalidIndex(); }

  std::vector<size_t> MissingETAssociation_v1::overlapIndices(size_t objIdx) const
  {
    return (objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size()) ? this->overlapIndices()[objIdx] : vector<size_t>(0);
  }

  std::vector<unsigned char> MissingETAssociation_v1::overlapTypes(size_t objIdx) const
  {
    return (objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size()) ? this->overlapTypes()[objIdx] : vector<unsigned char>(0);
  }

  std::vector<const IParticle*> MissingETAssociation_v1::objects() const
  { 
    vector<const IParticle*> pVec;
    for ( objlink_vector_t::const_iterator fLnk(this->objectLinks().begin()); fLnk != this->objectLinks().end(); ++fLnk) {pVec.push_back((*fLnk).isValid()?*(*fLnk):NULL); }
    return pVec;
  }

  std::vector<const IParticle*> MissingETAssociation_v1::objects(std::vector<constvec_t>& calVecs,std::vector<constvec_t>& trkVecs) const
  {
    size_t nEnt(this->objectLinks().size());
    vector<const IParticle*> pVec(nEnt,(const IParticle*)0);
    calVecs.clear(); calVecs.resize(this->sizeCal(),constvec_t()); 
    trkVecs.clear(); trkVecs.resize(this->sizeTrk(),constvec_t()); 
    for ( size_t idx(0); idx < nEnt; ++idx ) pVec[idx] = *(this->objectLinks().at(idx));
    for ( size_t idx(0); idx < this->sizeCal(); idx++) calVecs[idx] = constvec_t(this->calpx(idx),this->calpy(idx),this->calpz(idx),this->cale(idx),this->calsumpt(idx));
    for ( size_t idx(0); idx < this->sizeTrk(); idx++) trkVecs[idx] = constvec_t(this->trkpx(idx),this->trkpy(idx),this->trkpz(idx),this->trke(idx),this->trksumpt(idx));
    return pVec;
  }
										   
  std::vector<const IParticle*> MissingETAssociation_v1::objects(const std::vector<double>*& calpxPtr,const std::vector<double>*& calpyPtr,
								 const std::vector<double>*& calpzPtr,const std::vector<double>*& calePtr,
								 const std::vector<double>*& calsumptPtr,
								 const std::vector<double>*& trkpxPtr,const std::vector<double>*& trkpyPtr,
								 const std::vector<double>*& trkpzPtr,const std::vector<double>*& trkePtr,
								 const std::vector<double>*& trksumptPtr) const
  {
    calpxPtr = &(this->calpx()); calpyPtr = &(this->calpy()); calpzPtr = &(this->calpz()); calePtr = &(this->cale()); calsumptPtr = &(this->calsumpt());
    trkpxPtr = &(this->trkpx()); trkpyPtr = &(this->trkpy()); trkpzPtr = &(this->trkpz()); trkePtr = &(this->trke()); trksumptPtr = &(this->trksumpt());
    return this->objects();
  }
 
  //////////////////////////////
  // constvec_t object generators //
  //////////////////////////////

  MissingETAssociation_v1::ConstVec MissingETAssociation_v1::calVec(const IParticle* pPart) const
  {
    MissingETBase::Types::constvec_t totalvec;
    for (size_t iKey = 0; iKey < this->sizeCal(); iKey++) {
      if (this->calkey(iKey) & getObjMask(this->findIndex(pPart)))
	totalvec+=this->calVec(iKey);
    }
    return totalvec;
  }

  MissingETAssociation_v1::ConstVec MissingETAssociation_v1::trkVec(const IParticle* pPart) const
  {
    MissingETBase::Types::constvec_t totalvec;
    for (size_t iKey = 0; iKey < this->sizeTrk(); iKey++) {
      if (this->trkkey(iKey) & getObjMask(this->findIndex(pPart)))
	totalvec+=this->trkVec(iKey);
    }
    return totalvec;
  }

  constvec_t MissingETAssociation_v1::calVec(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeCal() 
      ? constvec_t(this->calpx(keyIdx),this->calpy(keyIdx),this->calpz(keyIdx),this->cale(keyIdx),this->calsumpt(keyIdx)) 
      : constvec_t(MissingETBase::Numerical::cpxError(),MissingETBase::Numerical::cpyError(),MissingETBase::Numerical::cpzError(),MissingETBase::Numerical::ceError(),MissingETBase::Numerical::sumptError()); }

  constvec_t MissingETAssociation_v1::trkVec(size_t keyIdx) const
  { return keyIdx != MissingETBase::Numerical::invalidIndex() && keyIdx < this->sizeTrk() 
      ? constvec_t(this->trkpx(keyIdx),this->trkpy(keyIdx),this->trkpz(keyIdx),this->trke(keyIdx),this->trksumpt(keyIdx)) 
      : constvec_t(MissingETBase::Numerical::cpxError(),MissingETBase::Numerical::cpyError(),MissingETBase::Numerical::cpzError(),MissingETBase::Numerical::ceError(),MissingETBase::Numerical::sumptError()); }

  constvec_t MissingETAssociation_v1::jetTrkVec() const
  { return constvec_t(this->jettrkpx(),this->jettrkpy(),this->jettrkpz(),this->jettrke(),this->jettrksumpt()); }

  /////////////////
  // Comparators //
  /////////////////

  bool MissingETAssociation_v1::operator==(const MissingETAssociation_v1& assocDescr) const
  {
    // FIXME compare only jet link and number of contributing particles 
    return 
      this->jetLink() == assocDescr.jetLink()    &&
      this->size()    == assocDescr.size();
  }


  ///////////////////////
  // Overlap functions //
  ///////////////////////

  void MissingETAssociation_v1::setObjSelectionFlag(size_t objIdx, bool status) const
  {
    if(status) m_useObjectFlags |= (1<<objIdx);
    else m_useObjectFlags &= ~(1<<objIdx);
  }

  bool MissingETAssociation_v1::objSelected(size_t objIdx) const
  {
    return bool(m_useObjectFlags & (1<<objIdx));
  }


  bool MissingETAssociation_v1::addOverlap(size_t objIdx,size_t overlapIndex,unsigned char overlapType)
  {
    if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_overlapIndices().size() ) {
      this->f_overlapIndices()[objIdx].push_back(overlapIndex);
      this->f_overlapTypes()[objIdx].push_back(overlapType);
      return true; 
    } else { return false; }
  }

  bool MissingETAssociation_v1::identifyOverlaps()
  {
    this->resetContrib();
    std::map<MissingETBase::Types::bitmask_t,MissingETBase::Types::constvec_t> trkOverlaps;
    std::map<MissingETBase::Types::bitmask_t,MissingETBase::Types::constvec_t> calOverlaps;
    std::map<ElementLink<IParticleContainer>,MissingETBase::Types::bitmask_t> objects;
    for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
      const vector<ElementLink<IParticleContainer> >& myConst = m_objConstLinks[iObj];
      for (size_t iConst=0; iConst<myConst.size(); ++iConst) objects[myConst[iConst]] |= 1 << iObj;
    }
    for (const auto& objpair : objects) {
      const IParticle* obj = *objpair.first;
      MissingETBase::Types::bitmask_t bm = objpair.second;
      if (obj->type()==xAOD::Type::TrackParticle) trkOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
      else if (obj->type()==xAOD::Type::ParticleFlow) {
	const PFO* pfo = static_cast<const PFO*>(obj);
	if(fabs(pfo->charge())>1e-9) {
	  // apply cPFO weight if present, only for the inclusive PFO sum
	  if (m_override.find(obj)!=m_override.end()) {
	    calOverlaps[bm] += m_override[obj];
	  } else {
	    calOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
	  }
	  trkOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
	} else {
	  if (m_override.find(obj)!=m_override.end()) {
	    calOverlaps[bm] += m_override[obj];
	  } else {
	    calOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
	  } // gets ignored otherwise?
	}
      } else if(obj->type()==xAOD::Type::TruthParticle) {
	const TruthParticle* tp = static_cast<const TruthParticle*>(obj);
	if(fabs(tp->charge())>0.)
	  trkOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
	if(!tp->isMuon())
	  calOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
      }
      else calOverlaps[bm] += MissingETBase::Types::constvec_t(*obj);
    }
    for (const auto& calpair : calOverlaps) {
      MissingETBase::Types::bitmask_t bm = calpair.first;
      MissingETBase::Types::constvec_t cv = calpair.second;
      this->addCalVec(bm,cv.cpx(),cv.cpy(),cv.cpz(),cv.ce(),cv.sumpt());
    }
    for (const auto& trkpair : trkOverlaps) {
      MissingETBase::Types::bitmask_t bm = trkpair.first;
      MissingETBase::Types::constvec_t cv = trkpair.second;
      this->addTrkVec(bm,cv.cpx(),cv.cpy(),cv.cpz(),cv.ce(),cv.sumpt());
    }

    bool foundOverlaps(false);
    for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
      foundOverlaps |= identifyOverlaps(iObj);
    }
    return foundOverlaps;
  }

  bool MissingETAssociation_v1::identifyOverlaps(size_t objIdx)
  {
    if ( objIdx == MissingETBase::Numerical::invalidIndex() ) return false;
    // extract constituent list
    const vector<MissingETBase::Types::objlink_t >& myConst = m_objConstLinks[objIdx];
    // loop over subsequent objects in this association
    // (assume earlier ones have been tested)
    vector<size_t>& myOverlaps = f_overlapIndices()[objIdx];
    for(size_t iTargetObj=objIdx+1; iTargetObj<this->objectLinks().size(); ++iTargetObj) {
      const vector<MissingETBase::Types::objlink_t >& checkConst = m_objConstLinks[iTargetObj];
      bool overlapFound(false);
      unsigned char overlapTypes(0);
      // if we somehow already recorded an overlap for this one, skip
      for(vector<size_t>::const_iterator iOverlap=myOverlaps.begin();
	  iOverlap!=myOverlaps.end() && !overlapFound; ++iOverlap) {
	overlapFound = (*iOverlap) == iTargetObj;
      }
      if(overlapFound) continue;
      // otherwise, compare constituents to check if any match
      for(const auto& mine : myConst) {
	for(const auto& target : checkConst) {
	  overlapFound |= ( mine == target );
	  if(mine == target) {
	    if((*mine)->type()==xAOD::Type::TruthParticle) overlapTypes |= 1;
	    else if((*mine)->type()==xAOD::Type::Other) overlapTypes |= 1 << xAOD::Type::NeutralParticle;
	    else overlapTypes |= 1 << (*mine)->type();
	  }
	}
      }
      // record the overlap
      if(overlapFound) {
	this->addOverlap(objIdx,iTargetObj,overlapTypes);
	this->addOverlap(iTargetObj,objIdx,overlapTypes);
      }
    }
    return overlapIndices(objIdx).size()>0;
  }

  bool MissingETAssociation_v1::hasOverlaps(size_t objIdx,MissingETBase::UsageHandler::Policy p) const
  {
    if ( objIdx == MissingETBase::Numerical::invalidIndex() ) return false;
    vector<size_t> indices = this->overlapIndices(objIdx);
    vector<unsigned char> types = this->overlapTypes(objIdx);
    for(size_t iOL=0; iOL<indices.size(); ++iOL) {
      if(this->objSelected(indices[iOL])) {
	// printf("Test object %lu for overlaps: OL type %i\n",indices[iOL],(int)types[iOL]);
	switch(p) {
	case MissingETBase::UsageHandler::TrackCluster:      
	  if((types[iOL] & 1<<xAOD::Type::CaloCluster) || (types[iOL] & 1<<xAOD::Type::TrackParticle)) {break;}
	  else {continue;}
	case MissingETBase::UsageHandler::OnlyCluster:
	  if(types[iOL] & 1<<xAOD::Type::CaloCluster) {break;}
	  else {continue;}
	case MissingETBase::UsageHandler::OnlyTrack:
	  if(types[iOL] & 1<<xAOD::Type::TrackParticle) {break;}
	  else {continue;}
	case MissingETBase::UsageHandler::ParticleFlow:
	  if(types[iOL] & 1<<xAOD::Type::ParticleFlow) {break;}
	  else {continue;}
	case MissingETBase::UsageHandler::TruthParticle:
	  if(types[iOL] & 1) {break;}
	  else {continue;}
	case MissingETBase::UsageHandler::AllCalo:
	  if(types[iOL] & ~(1<<xAOD::Type::TrackParticle)) {break;}
	  else {continue;}
	default: continue;
	}
	return true;
      }
    }
    return false;
  }

  bool MissingETAssociation_v1::hasAlternateConstVec() const
  {
    return (!this->isMisc() && this->containsPhysics(this->refJet()));
  }

  xAOD::JetFourMom_t MissingETAssociation_v1::getAlternateConstVec() const
  {
    xAOD::JetFourMom_t newvec;
    constvec_t newpt;
    size_t newptIndex = this->findIndex(this->refJet());
    if (newptIndex == MissingETBase::Numerical::invalidIndex()) return newvec;
    MissingETBase::Types::bitmask_t mask = (MissingETBase::Types::bitmask_t)1 << newptIndex;
    
    for (size_t iKey = 0; iKey < this->sizeCal(); iKey++) if (mask & this->calkey()[iKey]) newpt += this->calVec(iKey);
    newvec.SetPxPyPzE(newpt.cpx(),newpt.cpy(),newpt.cpz(),newpt.ce());
    return newvec; 
  }

  constvec_t MissingETAssociation_v1::overlapCalVec() const
  {
    constvec_t calvec;
    // printf("Test %lu keys for cal overlaps.\n",this->sizeCal());
    for (size_t iKey = 0; iKey < this->sizeCal(); iKey++) {
      bool selector = (m_useObjectFlags & this->calkey()[iKey]) ? !this->isMisc() : this->isMisc();
      // printf("Selector? %o\n",selector);
      if (selector) calvec+=this->calVec(iKey);
      // printf("Updated selector? %o. Overlaps? %o\n",selector,this->hasOverlaps(iKey,MissingETBase::UsageHandler::OnlyCluster)||this->hasOverlaps(iKey,MissingETBase::UsageHandler::ParticleFlow)||this->hasOverlaps(iKey,MissingETBase::UsageHandler::TruthParticle));
    }
    return calvec;
  }

  constvec_t MissingETAssociation_v1::overlapTrkVec() const
  {
    constvec_t trkvec;
    // printf("Test %lu keys for trk overlaps for %lu objects.\n",this->sizeTrk(),this->objectLinks().size());
    for (size_t iKey = 0; iKey < this->sizeTrk(); iKey++) {
      bool selector = (m_useObjectFlags & this->trkkey()[iKey]) ? !this->isMisc() : this->isMisc();
      // printf("Selector? %o\n",selector);
      if (selector) trkvec+=ConstVec(this->trkpx()[iKey],this->trkpy()[iKey],this->trkpz()[iKey],this->trke()[iKey],this->trksumpt()[iKey]);
      // printf("Updated selector? %o. Overlaps? %o\n",selector,this->hasOverlaps(iKey,MissingETBase::UsageHandler::TrackCluster)||this->hasOverlaps(iKey,MissingETBase::UsageHandler::ParticleFlow)||this->hasOverlaps(iKey,MissingETBase::UsageHandler::TruthParticle));
    }
    return trkvec;
  }

  bool MissingETAssociation_v1::containsPhysics(const IParticle* pPhys) const
  {
    for(const auto& link : this->objectLinks()) {
      if(link.isValid()) {
	if(pPhys == *link) {
	  return true;
	}
      }
    }
    return false;
  }

  bool MissingETAssociation_v1::containsSignal(const IParticle* pSig) const
  {
    for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
      for(const auto& link : m_objConstLinks[iObj]) {
	if(link.isValid()) {
	  if(pSig == *link) {
	    return true;
	  }
	}
      }
    }
    return false;
  }

  bool MissingETAssociation_v1::checkUsage(const IParticle* pSig,MissingETBase::UsageHandler::Policy p) const
  {
    if(MissingETAssociation_v1::testPolicy(pSig->type(),p)) {
      const IParticleContainer* pCont = static_cast<const IParticleContainer*>(pSig->container());
      MissingETBase::Types::objlink_t el(*pCont,pSig->index());
      for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
	if(objSelected(iObj)) {
	  for(const auto& link : m_objConstLinks[iObj]) {
	    if(el == link) {
	      return true;
	    }
	  }
	}
      }
    }
    return false;
  }


  ///////////////////////
  // Other function(s) //
  ///////////////////////

  void MissingETAssociation_v1::createPrivateStore()
  {

    makePrivateStore();
    f_jetLink()    = jetlink_t();
    f_objectLinks().clear();
    f_overlapIndices().clear();
    f_overlapTypes().clear();
    f_isMisc() = false;
    f_calpx().clear();
    f_calpy().clear();
    f_calpz().clear();
    f_cale().clear();
    f_calsumpt().clear();
    f_calkey().clear();
    f_trkpx().clear();
    f_trkpy().clear();
    f_trkpz().clear();
    f_trke().clear();
    f_trksumpt().clear();
    f_trkkey().clear();
    f_jettrkpx() = 0;
    f_jettrkpy() = 0;
    f_jettrkpz() = 0;
    f_jettrke() = 0;
    f_jettrksumpt() = 0;

    return;
  }

  void MissingETAssociation_v1::initCache()
  {
  
    m_objConstLinks.clear();
    m_contribObjects.clear();
    m_objConstLinks.reserve(50);
    m_useObjectFlags = 0;
    m_contribObjects.reserve(10);
  }

  void MissingETAssociation_v1::resetCache()
  {
    m_useObjectFlags = 0;
    m_lastObjectPointer = 0;
  }

  bool MissingETAssociation_v1::testPolicy(unsigned int type,MissingETBase::UsageHandler::Policy p) {
    switch(p) {
    case MissingETBase::UsageHandler::TrackCluster:      
      if(type==xAOD::Type::CaloCluster
	 || type==xAOD::Type::TrackParticle) {return true;}
      else {return false;}
    case MissingETBase::UsageHandler::OnlyCluster:
      if(type==xAOD::Type::CaloCluster) {return true;}
      else {return false;}
    case MissingETBase::UsageHandler::OnlyTrack:
      if(type==xAOD::Type::TrackParticle) {return true;}
      else {return false;}
    case MissingETBase::UsageHandler::ParticleFlow:
      if(type==xAOD::Type::ParticleFlow) {return true;}
      else {return false;}
    case MissingETBase::UsageHandler::AllCalo:
      if(type!=xAOD::Type::TrackParticle) {return true;}
      else {return false;}
    default: break;
    }
    return false;
  }

}
