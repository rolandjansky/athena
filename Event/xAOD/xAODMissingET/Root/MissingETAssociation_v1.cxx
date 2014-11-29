/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/versions/MissingETAssociation_v1.h"
#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODJet/JetContainer.h"

#include <iterator>
#include <cstdio>

using namespace xAOD;
using namespace MissingETBase::Types;
using std::vector;

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
  if ( scale == 0. ) { this->cpx() = 0.; this->cpy() = 0.; this->cpz() = 0.; this->ce() = 0.; this->sumpt() = 0.; return *this; }
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
  this->setTrkPx(assocDescr.trkpx());
  this->setTrkPy(assocDescr.trkpy());
  this->setTrkPz(assocDescr.trkpz());
  this->setTrkE(assocDescr.trke());  
  this->setTrkSumpt(assocDescr.trksumpt());  
  this->setJetTrkPx(assocDescr.jettrkpx());
  this->setJetTrkPy(assocDescr.jettrkpy());
  this->setJetTrkPz(assocDescr.jettrkpz());
  this->setJetTrkE(assocDescr.jettrke());
  return  *this;
}

MissingETAssociation_v1::~MissingETAssociation_v1() { }

///////////////////////////
// Data store management //
///////////////////////////

bool MissingETAssociation_v1::addObject(const IParticle* pPart,const std::vector<const IParticle*>& constlist,
					double cpx,double cpy,double cpz,double ce,double csumpt,
					double tpx,double tpy,double tpz,double te,double tsumpt)
{
  // check if object is in store

  size_t idx(this->findIndex(pPart));
  // found - add const vectors
  if ( idx != MissingETBase::Numerical::invalidIndex() )
    { 
      this->f_calpx()[idx] += cpx; this->f_calpy()[idx] += cpy; this->f_calpz()[idx] += cpz; this->f_cale()[idx] += ce; this->f_calsumpt()[idx] += csumpt;
      this->f_trkpx()[idx] += tpx; this->f_trkpy()[idx] += tpy; this->f_trkpz()[idx] += tpz; this->f_trke()[idx] += te; this->f_trksumpt()[idx] += tsumpt;
      m_objConstLinks[idx].clear();
      m_objConstLinks[idx].reserve(constlist.size());
      for(vector<const IParticle*>::const_iterator iConst=constlist.begin();
	  iConst!=constlist.end(); ++iConst) {
	const IParticleContainer* pCont = dynamic_cast<const IParticleContainer*>((*iConst)->container());
	ElementLink<IParticleContainer> el(*pCont,(*iConst)->index());
	m_objConstLinks[idx].push_back(el);
      }
      return false; }
  // new object
  else
    {
      MissingETBase::Types::objlink_t oLnk; f_setObject<IParticle,MissingETBase::Types::objlink_t>(pPart,oLnk); 
      // add to stores
      this->f_objectLinks().push_back(oLnk);
      this->f_calpx().push_back(cpx);this->f_calpy().push_back(cpy);this->f_calpz().push_back(cpz);this->f_cale().push_back(ce);this->f_calsumpt().push_back(csumpt);
      this->f_trkpx().push_back(tpx);this->f_trkpy().push_back(tpy);this->f_trkpz().push_back(tpz);this->f_trke().push_back(te);this->f_trksumpt().push_back(tsumpt);
      vector<ElementLink<IParticleContainer> > linklist;
      linklist.reserve(constlist.size());
      for(vector<const IParticle*>::const_iterator iConst=constlist.begin();
	  iConst!=constlist.end(); ++iConst) {
	const IParticleContainer* pCont = dynamic_cast<const IParticleContainer*>((*iConst)->container());
	ElementLink<IParticleContainer> el(*pCont,(*iConst)->index());
	linklist.push_back(el);
      }
      m_objConstLinks.push_back(linklist);
      this->f_overlapIndices().push_back(vector<size_t>(0));
      this->f_overlapTypes().push_back(vector<unsigned char>(0));
      bool linkset = f_setLink<MissingETBase::Types::objlink_t>(oLnk);
      return linkset;
    }
}

void MissingETAssociation_v1::updateLinks()
{
  this->updateJetLink();
  static SG::AuxElement::Accessor<MissingETBase::Types::objlink_vector_t> acc("objectLinks");
  if(acc.isAvailableWritable(*this)) {
    MissingETBase::Types::objlink_vector_t::iterator fLnk(this->f_objectLinks().begin());
    MissingETBase::Types::objlink_vector_t::iterator lLnk(this->f_objectLinks().end());
    for ( ; fLnk != lLnk; ++fLnk ) { 
      fLnk->toPersistent();
    } 
  }
}

void MissingETAssociation_v1::updateJetLink() {
  if(!this->isMisc()) {
    static SG::AuxElement::Accessor<MissingETBase::Types::jetlink_t> acc("jetLink");
    if(acc.isAvailableWritable(*this)) {
      if(f_setLink<MissingETBase::Types::jetlink_t>(this->f_jetLink())) {
	this->f_jetLink().toPersistent();
      }
    }
  }
}

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
  vector<double>::iterator fCalPx(this->f_calpx().begin()); std::advance<vector<double>::iterator>(fCalPx,objIdx);
  this->f_calpx().erase(fCalPx);
  vector<double>::iterator fCalPy(this->f_calpy().begin()); std::advance<vector<double>::iterator>(fCalPy,objIdx);
  this->f_calpy().erase(fCalPy);
  vector<double>::iterator fCalPz(this->f_calpz().begin()); std::advance<vector<double>::iterator>(fCalPz,objIdx);
  this->f_calpz().erase(fCalPz);
  vector<double>::iterator fCalE(this->f_cale().begin()); std::advance<vector<double>::iterator>(fCalE,objIdx);
  this->f_cale().erase(fCalE);
  vector<double>::iterator fCalSumpt(this->f_calsumpt().begin()); std::advance<vector<double>::iterator>(fCalSumpt,objIdx);
  this->f_calsumpt().erase(fCalSumpt);
  vector<double>::iterator fTrkPx(this->f_trkpx().begin()); std::advance<vector<double>::iterator>(fTrkPx,objIdx);
  this->f_trkpx().erase(fTrkPx);
  vector<double>::iterator fTrkPy(this->f_trkpy().begin()); std::advance<vector<double>::iterator>(fTrkPy,objIdx);
  this->f_trkpy().erase(fTrkPy);
  vector<double>::iterator fTrkPz(this->f_trkpz().begin()); std::advance<vector<double>::iterator>(fTrkPz,objIdx);
  this->f_trkpz().erase(fTrkPz);
  vector<double>::iterator fTrkE(this->f_trke().begin()); std::advance<vector<double>::iterator>(fTrkE,objIdx);
  this->f_trke().erase(fTrkE);
  vector<double>::iterator fTrkSumpt(this->f_trksumpt().begin()); std::advance<vector<double>::iterator>(fTrkSumpt,objIdx);
  this->f_trksumpt().erase(fTrkSumpt);
  return true;
}

bool MissingETAssociation_v1::resetContrib(size_t objIdx)
{
  if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() )
    {
      this->f_overlapIndices()[objIdx].clear();
      this->f_overlapTypes()[objIdx].clear();
      this->f_calpx()[objIdx] = MissingETBase::Numerical::cpxDefault();
      this->f_calpy()[objIdx] = MissingETBase::Numerical::cpyDefault();
      this->f_calpz()[objIdx] = MissingETBase::Numerical::cpzDefault();
      this->f_cale()[objIdx]  = MissingETBase::Numerical::ceDefault();
      this->f_calsumpt()[objIdx]  = MissingETBase::Numerical::sumptDefault();
      this->f_trkpx()[objIdx] = MissingETBase::Numerical::cpxDefault();
      this->f_trkpy()[objIdx] = MissingETBase::Numerical::cpyDefault();
      this->f_trkpz()[objIdx] = MissingETBase::Numerical::cpzDefault();
      this->f_trke()[objIdx]  = MissingETBase::Numerical::ceDefault();
      this->f_trksumpt()[objIdx]  = MissingETBase::Numerical::sumptDefault();
      return true;
    }
  return false;
}

bool MissingETAssociation_v1::resetContrib() 
{
  if ( this->empty() ) { return false; }
  else
    {
      size_t nEnt(this->size());
      for ( size_t objIdx(0); objIdx < nEnt; ++objIdx)
	{
	  this->resetContrib(objIdx);
	} 
      return true;
    } 
}

size_t MissingETAssociation_v1::findIndex(const IParticle* pPart) const
{
  if ( m_lastObjectPointer != pPart ) {
    objlink_vector_t::const_iterator fLnk(this->objectLinks().begin());
    // FIXME: This is based on dereferencing the ElementLink, which is risky.
    while ( fLnk != this->objectLinks().end() && *(*fLnk) != pPart ) { ++fLnk; }
    if ( fLnk != this->objectLinks().end() ) 
      { m_lastObjectPointer = pPart; m_lastObjectIndex = std::distance(this->objectLinks().begin(),fLnk); }
    else
      { m_lastObjectPointer = (const IParticle*)0; m_lastObjectIndex = MissingETBase::Numerical::invalidIndex();  }
  }
  return m_lastObjectIndex;
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

//////////
// FIXME: failsafe implementation checks on invalidKey and vector index - vector index may be sufficient?
bool MissingETAssociation_v1::setCalPx(size_t objIdx,double calpx)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_calpx().size() ) { this->f_calpx()[objIdx] = calpx; return true; } else { return false; } }
bool MissingETAssociation_v1::setCalPy(size_t objIdx,double calpy)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_calpy().size() ) { this->f_calpy()[objIdx] = calpy; return true; } else { return false; } }
bool MissingETAssociation_v1::setCalPz(size_t objIdx,double calpz)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_calpz().size() ) { this->f_calpz()[objIdx] = calpz; return true; } else { return false; } }
bool MissingETAssociation_v1::setCalE(size_t objIdx,double cale)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_cale().size() ) { this->f_cale()[objIdx] = cale; return true; } else { return false; } }
bool MissingETAssociation_v1::setCalSumpt(size_t objIdx,double calsumpt)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_calsumpt().size() ) { this->f_calsumpt()[objIdx] = calsumpt; return true; } else { return false; } }
bool MissingETAssociation_v1::setCalVec(size_t objIdx,double calpx,double calpy,double calpz,double cale,double sumpt)
{
  if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_cale().size() ) {
    this->f_calpx()[objIdx] = calpx;  this->f_calpy()[objIdx] = calpy;
    this->f_calpz()[objIdx] = calpz;  this->f_cale()[objIdx] = cale;
    this->f_calsumpt()[objIdx] = sumpt; return true;
  } else { return false; }
}

bool MissingETAssociation_v1::setTrkPx(size_t objIdx,double trkpx)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_trkpx().size() ) { this->f_trkpx()[objIdx] = trkpx; return true; } else { return false; } }
bool MissingETAssociation_v1::setTrkPy(size_t objIdx,double trkpy)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_trkpy().size() ) { this->f_trkpy()[objIdx] = trkpy; return true; } else { return false; } }
bool MissingETAssociation_v1::setTrkPz(size_t objIdx,double trkpz)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_trkpz().size() ) { this->f_trkpz()[objIdx] = trkpz; return true; } else { return false; } }
bool MissingETAssociation_v1::setTrkE(size_t objIdx,double trke)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_trke().size() ) { this->f_trke()[objIdx] = trke; return true; } else { return false; } }
bool MissingETAssociation_v1::setTrkSumpt(size_t objIdx,double trksumpt)
{ if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_trksumpt().size() ) { this->f_trksumpt()[objIdx] = trksumpt; return true; } else { return false; } }
bool MissingETAssociation_v1::setTrkVec(size_t objIdx,double trkpx,double trkpy,double trkpz,double trke,double sumpt)
{ 
  if ( objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->f_trke().size() ) {
    this->f_trkpx()[objIdx] = trkpx;  this->f_trkpy()[objIdx] = trkpy;
    this->f_trkpz()[objIdx] = trkpz;  this->f_trke()[objIdx] = trke;
    this->f_trksumpt()[objIdx] = sumpt; return true; 
  } else { return false; }
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
  
double MissingETAssociation_v1::calpx(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->calpx()[objIdx] : MissingETBase::Numerical::cpxError(); }
double MissingETAssociation_v1::calpy(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->calpy()[objIdx] : MissingETBase::Numerical::cpyError(); }
double MissingETAssociation_v1::calpz(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->calpz()[objIdx] : MissingETBase::Numerical::cpzError(); }
double MissingETAssociation_v1::cale(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->cale()[objIdx] : MissingETBase::Numerical::ceError(); }
double MissingETAssociation_v1::calsumpt(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->calsumpt()[objIdx] : MissingETBase::Numerical::sumptError(); }

double MissingETAssociation_v1::trkpx(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->trkpx()[objIdx] : MissingETBase::Numerical::cpxError(); }
double MissingETAssociation_v1::trkpy(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->trkpy()[objIdx] : MissingETBase::Numerical::cpyError(); }
double MissingETAssociation_v1::trkpz(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->trkpz()[objIdx] : MissingETBase::Numerical::cpzError(); }
double MissingETAssociation_v1::trke(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->trke()[objIdx] : MissingETBase::Numerical::ceError(); }
double MissingETAssociation_v1::trksumpt(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() ? this->trksumpt()[objIdx] : MissingETBase::Numerical::sumptError(); }

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
  for ( objlink_vector_t::const_iterator fLnk(this->objectLinks().begin()); fLnk != this->objectLinks().end(); ++fLnk) { pVec.push_back(*(*fLnk)); }
  return pVec;
}

std::vector<const IParticle*> MissingETAssociation_v1::objects(std::vector<constvec_t>& calVecs,std::vector<constvec_t>& trkVecs) const
{
  size_t nEnt(this->objectLinks().size());
  vector<const IParticle*> pVec(nEnt,(const IParticle*)0);
  calVecs.clear(); calVecs.resize(nEnt,constvec_t()); 
  trkVecs.clear(); trkVecs.resize(nEnt,constvec_t()); 
  for ( size_t idx(0); idx < nEnt; ++idx ) {
    pVec[idx] = *(this->objectLinks().at(idx));
    calVecs[idx] = constvec_t(this->calpx(idx),this->calpy(idx),this->calpz(idx),this->cale(idx),this->calsumpt(idx));
    trkVecs[idx] = constvec_t(this->trkpx(idx),this->trkpy(idx),this->trkpz(idx),this->trke(idx),this->trksumpt(idx));
  }
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

constvec_t MissingETAssociation_v1::calVec(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() 
    ? constvec_t(this->calpx(objIdx),this->calpy(objIdx),this->calpz(objIdx),this->cale(objIdx),this->calsumpt(objIdx)) 
    : constvec_t(MissingETBase::Numerical::cpxError(),MissingETBase::Numerical::cpyError(),MissingETBase::Numerical::cpzError(),MissingETBase::Numerical::ceError(),MissingETBase::Numerical::sumptError()); }

constvec_t MissingETAssociation_v1::trkVec(size_t objIdx) const
{ return objIdx != MissingETBase::Numerical::invalidIndex() && objIdx < this->size() 
    ? constvec_t(this->trkpx(objIdx),this->trkpy(objIdx),this->trkpz(objIdx),this->trke(objIdx),this->trksumpt(objIdx)) 
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
    this->jetLink()    == assocDescr.jetLink()    &&
    this->size()       == assocDescr.size();
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
  const vector<ElementLink<IParticleContainer> >& myConst = m_objConstLinks[objIdx];
  // loop over subsequent objects in this association
  // (assume earlier ones have been tested)
  vector<size_t>& myOverlaps = f_overlapIndices()[objIdx];
  for(size_t iTargetObj=objIdx+1; iTargetObj<this->objectLinks().size(); ++iTargetObj) {
    const vector<ElementLink<IParticleContainer> >& checkConst = m_objConstLinks[iTargetObj];
    bool overlapFound(false);
    unsigned char overlapTypes(0);
    // if we somehow already recorded an overlap for this one, skip
    for(vector<size_t>::const_iterator iOverlap=myOverlaps.begin();
	iOverlap!=myOverlaps.end() && !overlapFound; ++iOverlap) {
      overlapFound = (*iOverlap) == iTargetObj;
    }
    if(overlapFound) continue;
    // otherwise, compare constituents to check if any match
    for(vector<ElementLink<IParticleContainer> >::const_iterator iMine=myConst.begin();
	iMine!=myConst.end(); ++iMine) {
      for(vector<ElementLink<IParticleContainer> >::const_iterator iTargetConst=checkConst.begin();
	  iTargetConst!=checkConst.end(); ++iTargetConst) {
	overlapFound |= ( (*iMine) == (*iTargetConst) );
	overlapTypes |= 1 << (**iMine)->type();
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
      //      printf("Test object %lu for overlaps: OL type %i\n",indices[iOL],(int)types[iOL]);
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
      default: continue;
      }
      return true;
    }
  }
  return false;
}

constvec_t MissingETAssociation_v1::overlapCalVec() const
{
  MissingETBase::Types::bitmask_t useObjectFlags_orig = m_useObjectFlags;

  constvec_t calvec;
  for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
    if(this->isMisc()) {
      // in misc association, add in the unused clusters
      // rather than subtracting the used ones
      if(!this->objSelected(iObj)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::OnlyCluster)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::ParticleFlow)) {
	// printf("overlapCalVec misc: %lu add px %f py %f sumpt %f \n",
	//        iObj,this->calVec(iObj).cpx(), this->calVec(iObj).cpy(),
	//        this->calVec(iObj).sumpt());
	calvec += this->calVec(iObj);
	this->setObjSelectionFlag(iObj,true);
      } // add clusters for unselected objects or overlapping objects
    } else {
      if(this->objSelected(iObj)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::OnlyCluster)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::ParticleFlow)) {
	calvec += this->calVec(iObj);
      } // remove clusters from selected, non-overlapping objects
    } // treatment for standard associations
  } // loop over associated objects

  m_useObjectFlags = useObjectFlags_orig;
  return calvec;
}

constvec_t MissingETAssociation_v1::overlapTrkVec() const
{
  MissingETBase::Types::bitmask_t useObjectFlags_orig = m_useObjectFlags;

  constvec_t trkvec;
  // Loop once ignoring muons
  for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
    if((*this->objectLinks()[iObj])->type()==xAOD::Type::Muon) continue;
    if(this->isMisc()) {
      // in misc association, add in the unused tracks
      // rather than subtracting the used ones
      if(!this->objSelected(iObj)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::TrackCluster)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::ParticleFlow)) {
	// printf("object %lu with type %i and pt %f\n",iObj,(*this->objectLinks()[iObj])->type(),(*this->objectLinks()[iObj])->pt());
	trkvec += this->trkVec(iObj);
	this->setObjSelectionFlag(iObj,true);
      } // add tracks for unselected objects or overlapping objects
    } else {
      if(this->objSelected(iObj)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::OnlyCluster)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::ParticleFlow)) {
	// printf("object %lu with type %i and pt %f\n",iObj,(*this->objectLinks()[iObj])->type(),(*this->objectLinks()[iObj])->pt());
	trkvec += this->trkVec(iObj);
      } // remove tracks from selected, non-overlapping objects
    } // treatment for standard associations
  } // loop over associated objects
  // Now do muons only
  for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
    if((*this->objectLinks()[iObj])->type()!=xAOD::Type::Muon) continue;
    if(this->isMisc()) {
      // in misc association, add in the unused tracks
      // rather than subtracting the used ones
      if(!this->objSelected(iObj)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::OnlyTrack)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::ParticleFlow)) {
	// printf("object %lu with type %i and pt %f\n",iObj,(*this->objectLinks()[iObj])->type(),(*this->objectLinks()[iObj])->pt());
	trkvec += this->trkVec(iObj);
	this->setObjSelectionFlag(iObj,true);
      } // add tracks for unselected objects or overlapping objects
    } else {
      if(this->objSelected(iObj)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::OnlyTrack)
	 && !this->hasOverlaps(iObj,MissingETBase::UsageHandler::ParticleFlow)) {
	// printf("object %lu with type %i and pt %f\n",iObj,(*this->objectLinks()[iObj])->type(),(*this->objectLinks()[iObj])->pt());
	trkvec += this->trkVec(iObj);
      } // remove tracks from selected, non-overlapping objects
    } // treatment for standard associations
  } // loop over associated objects
  
  m_useObjectFlags = useObjectFlags_orig;
  return trkvec;
}

bool MissingETAssociation_v1::contains(const IParticle* pSig) const
{
  const IParticleContainer* pCont = dynamic_cast<const IParticleContainer*>(pSig->container());
  ElementLink<IParticleContainer> el(*pCont,pSig->index());
  for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
    for(vector<ElementLink<IParticleContainer> >::const_iterator iLnk=m_objConstLinks[iObj].begin();
	iLnk!=m_objConstLinks[iObj].end(); ++iLnk) {
      if(*(*iLnk)==pSig) {
	return true;
      }
    }
  }
  return false;
}

bool MissingETAssociation_v1::checkUsage(const IParticle* pSig,MissingETBase::UsageHandler::Policy p) const
{
  if(MissingETAssociation_v1::testPolicy(pSig->type(),p)) {
    const IParticleContainer* pCont = dynamic_cast<const IParticleContainer*>(pSig->container());
    ElementLink<IParticleContainer> el(*pCont,pSig->index());
    for(size_t iObj=0; iObj<this->objectLinks().size(); ++iObj) {
      if(objSelected(iObj)) {
	for(vector<ElementLink<IParticleContainer> >::const_iterator iLnk=m_objConstLinks[iObj].begin();
	    iLnk!=m_objConstLinks[iObj].end(); ++iLnk) {
	  if(*(*iLnk)==pSig) {
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
  f_trkpx().clear();
  f_trkpy().clear();
  f_trkpz().clear();
  f_trke().clear();
  f_trksumpt().clear();
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
  default: break;
  }
  return false;
}
