/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/versions/MissingETComponent_v1.h"
#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODMissingET/versions/MissingETContainer_v1.h"

#include <iterator>

using namespace xAOD;
using namespace MissingETBase::Types;

///////////////////////////////////////////////////
// MissingETComponent_v1::Weight Implementations //
///////////////////////////////////////////////////

MissingETComponent_v1::Weight& MissingETComponent_v1::Weight::operator+=(const Weight& wght)
{ this->wpx() += wght.wpx(); this->wpy() += wght.wpy(); this->wet() += wght.wet(); return *this; }

MissingETComponent_v1::Weight& MissingETComponent_v1::Weight::operator-=(const Weight& wght)
{ this->wpx() -= wght.wpx(); this->wpy() -= wght.wpy(); this->wet() -= wght.wet(); return *this; }

MissingETComponent_v1::Weight& MissingETComponent_v1::Weight::operator*=(const Weight& wght)
{ this->wpx() *= wght.wpx(); this->wpy() *= wght.wpy(); this->wet() *= wght.wet(); return *this; }

MissingETComponent_v1::Weight& MissingETComponent_v1::Weight::operator*=(double scale)
{ this->wpx() *= scale; this->wpy() *= scale; this->wet() *= scale; return *this; }

MissingETComponent_v1::Weight& MissingETComponent_v1::Weight::operator/=(const Weight& wght)
{ 
  this->wpx() = MissingETBase::Numerical::divide(this->wpx(),wght.wpx());
  this->wpy() = MissingETBase::Numerical::divide(this->wpy(),wght.wpy());
  this->wet() = MissingETBase::Numerical::divide(this->wet(),wght.wet());
  return *this;
}

MissingETComponent_v1::Weight& MissingETComponent_v1::Weight::operator/=(double scale)
{ 
  if ( scale == 0. ) { this->wpx() = 0.; this->wpy() = 0.; this->wet() = 0.; return *this; }
  else { return this->operator*=(1./scale); }
}

bool MissingETComponent_v1::Weight::operator==(const Weight& wght) const
{
  return 
    MissingETBase::Numerical::isEqual(this->wpx(),wght.wpx()) &&
    MissingETBase::Numerical::isEqual(this->wpy(),wght.wpy()) &&
    MissingETBase::Numerical::isEqual(this->wet(),wght.wet());
}

//////////////////////////////////
// Constructors and destructors //
//////////////////////////////////

MissingETComponent_v1::MissingETComponent_v1( bool createStore )
   : SG::AuxElement(){

   if( createStore ) createPrivateStore();
}

MissingETComponent_v1::MissingETComponent_v1( const MissingET* pmetObj,
                                              MissingETBase::Types::bitmask_t sw )
   : SG::AuxElement(){

   createPrivateStore();
   setMET( pmetObj );
   setStatusWord( sw );
}

MissingETComponent_v1::MissingETComponent_v1( const MissingET* pmetObj,
                                              const IParticle* pPart,
                                              double wpx, double wpy, double wet,
                                              MissingETBase::Types::bitmask_t sw )
   : SG::AuxElement(){

   createPrivateStore();
   setMET( pmetObj );
   setStatusWord( sw );
   addObject( pPart, wpx, wpy, wet );
}

MissingETComponent_v1::MissingETComponent_v1( const MissingET* pmetObj,
                                              const IParticle* pPart,
                                              const Weight& wght,
                                              MissingETBase::Types::bitmask_t sw )
   : SG::AuxElement(){

   createPrivateStore();
   setMET( pmetObj );
   setStatusWord( sw );
   addObject( pPart, wght );
}

MissingETComponent_v1::MissingETComponent_v1(const MissingETComponent_v1& compDescr)
  : SG::AuxElement(compDescr)
{ this->makePrivateStore(&compDescr); }

MissingETComponent_v1::MissingETComponent_v1(const MissingETComponent_v1& compDescr,MissingETBase::Types::bitmask_t sw)
  : SG::AuxElement()
{ this->makePrivateStore(compDescr); this->setStatusWord(sw); }

MissingETComponent_v1& MissingETComponent_v1::operator=(const MissingETComponent_v1& compDescr)
{ 
  if((&compDescr) != this) {
    this->setStatusWord(compDescr.statusWord());
    this->setMetLink(compDescr.metLink());
    this->setObjectLinks(compDescr.objectLinks());
    this->setWpx(compDescr.wpx());
    this->setWpy(compDescr.wpy());
    this->setWet(compDescr.wet());  
  }
  return  *this;
}

MissingETComponent_v1::~MissingETComponent_v1() { }

///////////////////////////
// Data store management //
///////////////////////////

bool MissingETComponent_v1::addObject(const IParticle* pPart,double wpx,double wpy,double wet)
{
  // check if object is in store
  size_t idx(this->findIndex(pPart));
  // found - add weights
  if ( idx != MissingETBase::Numerical::invalidIndex() )
    { this->f_wpx()[idx] += wpx; this->f_wpy()[idx] += wpy; this->f_wet()[idx] += wet; return false; }
  // new object
  else
    {
      MissingETBase::Types::objlink_t oLnk; f_setObject<IParticle,MissingETBase::Types::objlink_t>(pPart,oLnk); 
      // add to stores
      this->f_objectLinks().push_back(oLnk);this->f_wpx().push_back(wpx);this->f_wpy().push_back(wpy);this->f_wet().push_back(wet);
      bool linkset = f_setLink<MissingETBase::Types::objlink_t>(oLnk);
      return linkset;
    }
}

// void MissingETComponent_v1::updateLinks()
// {
//   this->updateMETLink();
//   static SG::AuxElement::Accessor<MissingETBase::Types::objlink_vector_t> acc("objectLinks");
//   if(acc.isAvailableWritable(*this)) {
//     MissingETBase::Types::objlink_vector_t::iterator fLnk(this->f_objectLinks().begin());
//     MissingETBase::Types::objlink_vector_t::iterator lLnk(this->f_objectLinks().end());
//     for ( ; fLnk != lLnk; ++fLnk ) { 
//       // avoid validity check to prevent crashes due to thinning
//       //     if(f_setLink<MissingETBase::Types::objlink_t>(*fLnk)){      
//       fLnk->toPersistent();
//       //      }
//     } 
//   }
// }

// void MissingETComponent_v1::updateMETLink() {
//   static SG::AuxElement::Accessor<MissingETBase::Types::metlink_t> acc("metLink");
//   if(acc.isAvailableWritable(*this)) {
//     if(f_setLink<MissingETBase::Types::metlink_t>(this->f_metLink())) {
//       this->f_metLink().toPersistent();
//     }
//   }
// }

bool MissingETComponent_v1::removeContrib()
{
  if ( this->empty() ) { return false; }
  else 
    { size_t nEnt(this->size()); for  ( size_t pIdx(0); pIdx<nEnt; ++pIdx ) { this->f_removeContrib(pIdx); } return true; }
}

bool MissingETComponent_v1::f_removeContrib(size_t pIdx)
{
  objlink_vector_t::iterator fCon(this->f_objectLinks().begin()); std::advance<objlink_vector_t::iterator>(fCon,pIdx);
  this->f_objectLinks().erase(fCon);
  std::vector<double>::iterator fWpx(this->f_wpx().begin()); std::advance<std::vector<double>::iterator>(fWpx,pIdx);
  this->f_wpx().erase(fWpx);
  std::vector<double>::iterator fWpy(this->f_wpy().begin()); std::advance<std::vector<double>::iterator>(fWpy,pIdx);
  this->f_wpy().erase(fWpy);
  std::vector<double>::iterator fWet(this->f_wet().begin()); std::advance<std::vector<double>::iterator>(fWet,pIdx);
  this->f_wet().erase(fWet);
  return true;
}

bool MissingETComponent_v1::resetContrib(size_t pIdx)
{
  if ( pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->size() )
    {
      this->f_wpx()[pIdx] = MissingETBase::Numerical::wpxDefault();
      this->f_wpy()[pIdx] = MissingETBase::Numerical::wpyDefault();
      this->f_wet()[pIdx] = MissingETBase::Numerical::wetDefault();
      return true;
    }
  return false;
}

bool MissingETComponent_v1::resetContrib() 
{
  if ( this->empty() ) { return false; }
  else
    {
      size_t nEnt(this->size());
      for ( size_t pIdx(0); pIdx < nEnt; ++pIdx)
	{
	  this->f_wpx()[pIdx] = MissingETBase::Numerical::wpxDefault();
	  this->f_wpy()[pIdx] = MissingETBase::Numerical::wpyDefault();
	  this->f_wet()[pIdx] = MissingETBase::Numerical::wetDefault();
	} 
      return true;
    } 
}

size_t MissingETComponent_v1::findIndex(const IParticle* pPart) const
{
  objlink_vector_t::const_iterator fLnk(this->objectLinks().begin());
  while(fLnk != this->objectLinks().end() && *(*fLnk) != pPart){ ++fLnk; }
  if(fLnk != this->objectLinks().end())
    return std::distance(this->objectLinks().begin(),fLnk);
  return MissingETBase::Numerical::invalidIndex();
}

/////////////
// Setters //
/////////////

bool MissingETComponent_v1::setWpx(const std::vector<double>& wcv)
{ this->f_wpx().clear(); this->f_wpx().insert(this->f_wpx().end(),wcv.begin(),wcv.end()); return !this->f_wpx().empty(); }
bool MissingETComponent_v1::setWpy(const std::vector<double>& wcv)
{ this->f_wpy().clear(); this->f_wpy().insert(this->f_wpy().end(),wcv.begin(),wcv.end()); return !this->f_wpy().empty(); }
bool MissingETComponent_v1::setWet(const std::vector<double>& wcv)
{ this->f_wet().clear(); this->f_wet().insert(this->f_wet().end(),wcv.begin(),wcv.end()); return !this->f_wet().empty(); }

//////////
// FIXME: failsafe implementation checks on invalidKey and vector index - vector index may be sufficient?
bool MissingETComponent_v1::setWpx(size_t pIdx,double wpx)
{ if ( pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->f_wpx().size() ) { this->f_wpx()[pIdx] = wpx; return true; } else { return false; } }
bool MissingETComponent_v1::setWpy(size_t pIdx,double wpy)
{ if ( pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->f_wpy().size() ) { this->f_wpy()[pIdx] = wpy; return true; } else { return false; } }
bool MissingETComponent_v1::setWet(size_t pIdx,double wet)
{ if ( pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->f_wet().size() ) { this->f_wet()[pIdx] = wet; return true; } else { return false; } }
bool MissingETComponent_v1::setWeight(size_t pIdx,double wpx,double wpy,double wet)
{ 
  if ( pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->f_wet().size() )  
    { this->f_wpx()[pIdx] = wpx;  this->f_wpy()[pIdx] = wpy;  this->f_wet()[pIdx] = wet; return true; }  
  else
    { return false; }
}

bool MissingETComponent_v1::setMET(const MissingET* pmetObj,MissingETBase::Types::bitmask_t sw)
{ 
  bool wasSet(this->f_metLink().getStorableObjectPointer() != nullptr);
  f_setObject<MissingET,MissingETBase::Types::metlink_t>(pmetObj,this->f_metLink());
  this->f_statusWord() = sw;
  return wasSet;
}

bool MissingETComponent_v1::setMET(const MissingETContainer_v1* pmetCont,size_t pmetIdx,MissingETBase::Types::bitmask_t sw)
{
  bool wasSet(this->f_metLink().getStorableObjectPointer() != nullptr);
  this->f_metLink().toIndexedElement(*pmetCont,pmetIdx);
  this->f_statusWord() = sw;
  return wasSet; }

bool MissingETComponent_v1::setMetLink(const MissingETBase::Types::metlink_t& metLnk)
{
  bool wasSet(this->f_metLink().getStorableObjectPointer() != nullptr);
  this->f_metLink() = metLnk;
  return wasSet; }

bool MissingETComponent_v1::setObjectLinks(const MissingETBase::Types::objlink_vector_t& objLnks)
{ 
  bool wasSet(!this->f_objectLinks().empty()); 
  if ( wasSet ) {
    this->f_objectLinks().clear(); this->f_objectLinks().insert(this->f_objectLinks().end(),objLnks.begin(),objLnks.end());
    for(MissingETBase::Types::objlink_vector_t::iterator iLink=this->f_objectLinks().begin();
	iLink!=this->f_objectLinks().end(); ++iLink) {
      this->f_setLink<MissingETBase::Types::objlink_t>(*iLink);
    }
  }
  return wasSet;
}


/////////////
// Getters //
/////////////
  
double MissingETComponent_v1::wpx(size_t pIdx) const
{ return pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->size() ? this->wpx()[pIdx] : MissingETBase::Numerical::wpxError(); }
double MissingETComponent_v1::wpy(size_t pIdx) const
{ return pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->size() ? this->wpy()[pIdx] : MissingETBase::Numerical::wpyError(); }
double MissingETComponent_v1::wet(size_t pIdx) const
{ return pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->size() ? this->wet()[pIdx] : MissingETBase::Numerical::wetError(); }

std::vector<const IParticle*> MissingETComponent_v1::objects() const
{ 
  std::vector<const IParticle*> pVec;
  for ( objlink_vector_t::const_iterator fLnk(this->objectLinks().begin()); fLnk != this->objectLinks().end(); ++fLnk) { pVec.push_back(*(*fLnk)); }
  return pVec;
}

std::vector<const IParticle*> MissingETComponent_v1::objects(MissingETBase::Types::weight_vector_t& kinePars) const
{
  size_t nEnt(this->objectLinks().size());
  std::vector<const IParticle*> pVec(nEnt,(const IParticle*)nullptr);
  kinePars.clear(); kinePars.resize(nEnt,Weight(0.,0.,0.)); 
  for ( size_t idx(0); idx < nEnt; ++idx ) 
    { pVec[idx] = *(this->objectLinks().at(idx)); kinePars.at(idx) = Weight(this->wpx().at(idx),this->wpy().at(idx),this->wet().at(idx)); }
  return pVec;
}
										   
std::vector<const IParticle*> MissingETComponent_v1::objects(const std::vector<double>*& wpxPtr,const std::vector<double>*& wpyPtr,const std::vector<double>*& wetPtr) const
{ wpxPtr = &(this->wpx()); wpyPtr = &(this->wpy()); wetPtr = &(this->wet()); return this->objects(); } 

//////////////////////////////
// Weight object generators //
//////////////////////////////

MissingETComponent_v1::Weight MissingETComponent_v1::weight(size_t pIdx) const
{ return pIdx != MissingETBase::Numerical::invalidIndex() && pIdx < this->size() 
    ? Weight(this->wpx().at(pIdx),this->wpy().at(pIdx),this->wet(pIdx)) 
    : Weight(MissingETBase::Numerical::wpxError(),MissingETBase::Numerical::wpyError(),MissingETBase::Numerical::wetError()); }

/////////////////
// Comparators //
/////////////////

bool MissingETComponent_v1::operator==(const MissingETComponent_v1& compDescr) const
{
  // FIXME compare only MET object link, statusword, and number of contributing particles 
  return 
    this->metLink()    == compDescr.metLink()    &&
    this->statusWord() == compDescr.statusWord() &&
    this->size()       == compDescr.size();
}

///////////////////////
// Other function(s) //
///////////////////////

void MissingETComponent_v1::createPrivateStore() {

   makePrivateStore();
   f_metLink()    = metlink_t();
   f_statusWord() = MissingETBase::Status::clearedStatus(); 
   f_wpx().clear();
   f_wpy().clear();
   f_wet().clear();
   f_objectLinks().clear();

   return;
}
