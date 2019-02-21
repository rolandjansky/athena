/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingETComponentMap_v1.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODCaloEvent/CaloCluster.h"

#include "xAODTracking/TrackParticle.h"

#include "xAODBase/IParticleContainer.h"
#include "xAODBase/ObjectType.h"

#include <iterator>
#include <cstdio>

using namespace xAOD;

/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

MissingETComponentMap_v1::MissingETComponentMap_v1(SG::OwnershipPolicy own, SG::IndexTrackingPolicy trackIndices)
  : DataVector<MissingETComponent_v1>(own,trackIndices)
{
  this->f_setupLookupCache(m_clusterLinks,m_clusterLinkReserve); 
  this->f_setupLookupCache(m_trackLinks,m_trackLinkReserve); 
}

MissingETComponentMap_v1::MissingETComponentMap_v1(MissingETComponentMap_v1::iterator first, MissingETComponentMap_v1::iterator last,
						   SG::OwnershipPolicy own, SG::IndexTrackingPolicy trackIndices)
  : DataVector<MissingETComponent_v1>(first,last,own,trackIndices)
{
  this->f_setupLookupCache(m_clusterLinks,m_clusterLinkReserve); 
  this->f_setupLookupCache(m_trackLinks,m_trackLinkReserve); 
}

MissingETComponentMap_v1::~MissingETComponentMap_v1()
{ this->clearCache(); }

/////////////
// Finders //
/////////////

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::find(const MissingET* pMET) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject() != pMET ) { ++fCont; }
  return fCont;
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::find(const MissingET* pMET)
{
  iterator fCont(this->begin());
  iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject() != pMET ) { ++fCont; }
  return fCont;
}

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::find(const std::string& name) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while (fCont != lCont && (*fCont)->metObject()->name() != name ) { ++fCont; }
  return fCont;
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::find(const std::string& name)
{
  iterator fCont(this->begin());
  iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->name() != name ) { ++fCont; }
  return fCont;
}

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::find(MissingETBase::Types::bitmask_t src) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->source() != src ) { ++fCont; }
  return fCont;
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::find(MissingETBase::Types::bitmask_t src)
{
  iterator fCont(this->begin());
  iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->source() != src ) { ++fCont; }
  return fCont;
}

size_t MissingETComponentMap_v1::findIndex(const MissingET* pMET) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject() != pMET ) { ++fCont; }
  if ( fCont != lCont ) return (size_t)std::distance<const_iterator>(this->begin(),fCont);
  return MissingETBase::Numerical::invalidIndex();
}

size_t MissingETComponentMap_v1::findIndex(const std::string& name) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->name() != name ) { ++fCont; }
  if ( fCont != lCont ) return (size_t)std::distance<const_iterator>(this->begin(),fCont);
  return MissingETBase::Numerical::invalidIndex();
}

size_t MissingETComponentMap_v1::findIndex(MissingETBase::Types::bitmask_t src) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->source() != src ) { ++fCont; }
  if ( fCont != lCont ) return (size_t)std::distance<const_iterator>(this->begin(),fCont);
  return MissingETBase::Numerical::invalidIndex();
}

/////////////////////////
// Retrieve MET object //
/////////////////////////

const MissingET* MissingETComponentMap_v1::retrieveMissingET(const std::string& name) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->name() != name ) { ++fCont; }
  if ( fCont != lCont ) return (*fCont)->metObject();
  return 0;
}

const MissingET* MissingETComponentMap_v1::retrieveMissingET(MissingETBase::Types::bitmask_t src,bool excl) const
{ return excl ? this->f_retrieveMissingETExcl(src) :  this->f_retrieveMissingETIncl(src); }

const MissingET* MissingETComponentMap_v1::retrieveMissingET(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw,bool excl) const
{ return excl ? this->f_retrieveMissingETExcl(src,sw) : this->f_retrieveMissingETIncl(src,sw); }

///////////////////////////////////
// Internal MET object retrieval //
///////////////////////////////////

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETExcl(MissingETBase::Types::bitmask_t src)  const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->source() != src ) { ++fCont; }
  if ( fCont != lCont ) return (*fCont)->metObject();
  return 0;
}

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETExcl(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw) const
{ 
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && ( (*fCont)->metObject()->source() != src || (*fCont)->statusWord() != sw ) ) { ++fCont; }
  if ( fCont != lCont ) return (*fCont)->metObject();
  return 0;
}

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETIncl(MissingETBase::Types::bitmask_t src) const
{ 
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && !MissingETBase::Source::hasPattern((*fCont)->metObject()->source(),src) ) { ++fCont; }
  if ( fCont != lCont ) return (*fCont)->metObject();
  return 0;
}

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETIncl(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && ( !MissingETBase::Source::hasPattern((*fCont)->metObject()->source(),src) ||
                              !MissingETBase::Status::Tags::hasPattern((*fCont)->statusWord(),sw) ) ) { ++fCont; }
  if ( fCont != lCont ) return (*fCont)->metObject();
  return 0;
}

/////////////////////////////
// Object usage management //
/////////////////////////////

bool MissingETComponentMap_v1::checkUsage(const  IParticle* pPart,MissingETBase::UsageHandler::Policy p) const
{ MissingETBase::Types::object_vector_t objects; objects.push_back(pPart); return this->checkUsage(objects,p); }

bool MissingETComponentMap_v1::checkUsage(MissingETBase::Types::object_vector_t& signals,MissingETBase::UsageHandler::Policy p) const 
{
  bool retVal(false);
  switch ( p )
    {
    case MissingETBase::UsageHandler::OnlyCluster:
      { if ( f_extractSignal<xAOD::Type::CaloCluster>(signals) ) { retVal = this->f_checkObjectUsage(signals,m_clusterLinks); }	break; }
    case MissingETBase::UsageHandler::OnlyTrack:
      { if ( f_extractSignal<xAOD::Type::TrackParticle>(signals) ) { retVal = this->f_checkObjectUsage(signals,m_trackLinks); } break; }
    case MissingETBase::UsageHandler::TrackCluster:
      {
	MissingETBase::Types::object_vector_t ins(signals.begin(),signals.end());
	size_t nSig(0);
	if ( f_extractSignal<xAOD::Type::CaloCluster>(signals) )
	  { nSig = signals.size(); this->f_checkObjectUsage(signals,m_clusterLinks); }
	if ( f_extractSignal<xAOD::Type::TrackParticle>(ins) )
	  { nSig += ins.size();	this->f_checkObjectUsage(ins,m_trackLinks); }
	signals.insert(signals.end(),ins.begin(),ins.end());
	retVal = signals.size() == nSig;
	break;
      }
    case MissingETBase::UsageHandler::PhysicsObject:
      { retVal = this->f_checkObjectUsage(signals,m_particleLinks); break; }
    default:
      { break; }
    } // switch 
  return retVal;
}

bool MissingETComponentMap_v1::f_checkObjectUsage(MissingETBase::Types::object_vector_t& signals, const signal_vector_t& signalLinks) const
{
  if ( signals.empty() || signalLinks.empty() ) return false;
  // loop on signals
  size_t oldSize(signals.size());
  MissingETBase::Types::object_vector_t::iterator fSig(signals.begin());
  while ( fSig != signals.end() ) 
    {
      size_t fIdx((*fSig)->index()); 
      if( fIdx >= signalLinks.size() ) {
	// if invalid, something probably went wrong in reconstruction, so complain
	// this should raise an out-of-range exception
	printf("MissingETComponentMap::f_checkObjectUsage(...) - ERROR - searching for a signal with index (%lu) beyond signal container range (%lu).", fIdx, signalLinks.size());
	printf("MissingETComponentMap::f_checkObjectUsage(...) - ERROR - this may indicate an inconsistency in the signal containers.");	
      }
      // signal already used
      if ( signalLinks.at(fIdx) != MissingETBase::Numerical::invalidLink() ) { fSig = signals.erase(fSig); }
      // signal not yet used
      else { ++fSig; }
    } // loop on signals
  return oldSize != signals.size(); 
}

bool MissingETComponentMap_v1::f_checkObjectUsage(MissingETBase::Types::object_vector_t& objects, const particle_map_t& physicsLinks) const
{
  if ( objects.empty() ) { return false; }
  //
  size_t oldSize(objects.size());
  MissingETBase::Types::object_vector_t::iterator fObj(objects.begin());
  while ( fObj != objects.end() )
    { if ( physicsLinks.find(*fObj) != physicsLinks.end() ) { fObj = objects.erase(fObj); } else { ++fObj; } }
  return objects.size() != oldSize;
}

/////////////////
// Add objects //
/////////////////

bool MissingETComponentMap_v1::addObjects(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList,MissingETBase::UsageHandler::Policy p)
{
  bool flag(false);
  switch ( p )
    {
    case MissingETBase::UsageHandler::OnlyCluster:
      { flag = this->setClusters(pMET,pPart,signalList); break; }
    case MissingETBase::UsageHandler::OnlyTrack:
      { flag = this->setTracks(pMET,pPart,signalList); break; }
    case MissingETBase::UsageHandler::TrackCluster:
      { bool cFlg(this->setClusters(pMET,pPart,signalList)); bool tFlg(this->setTracks(pMET,pPart,signalList)); flag = cFlg || tFlg; break; } 
    case MissingETBase::UsageHandler::PhysicsObject:
      { flag = this->setObjects(pMET,pPart,signalList); break; }
    default:
      { break; }
    }
  return flag;
}

bool MissingETComponentMap_v1::setClusters(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList)
{
  // find all clusters
  // check input: MissingET object pointer
  size_t cIdx(this->findIndex(pMET)); if ( cIdx == MissingETBase::Numerical::invalidIndex() ) { return false; }
  // check input: contributing particle pointer
  size_t pIdx(this->at(cIdx)->findIndex(pPart)); if ( pIdx == MissingETBase::Numerical::invalidIndex()  ) { return false; }
  indexedlink_t ld(cIdx,pIdx); 
  // make sure cache is not cleared
  bool firstCluster = true;
  MissingETBase::Types::object_vector_t::const_iterator fSig(signalList.begin());
  MissingETBase::Types::object_vector_t::const_iterator lSig(signalList.end());
  for ( ; fSig != lSig; ++fSig ) {
    if((*fSig)->type()==xAOD::Type::CaloCluster) {
      if(firstCluster) {
	firstCluster = false;
	if ( m_clusterLinks.empty() )  { 
	  const IParticleContainer* pCont(static_cast<const IParticleContainer*>((*fSig)->container()));
	  // if ( pCont == 0 )
	  //   { printf("MissingETComponentMap::setClusters(...) - WARNING   - cannot access cluster container (invalid NULL pointer), use %i words\n",(int)nClus); }
	  // else
	  m_clusterLinks.resize(pCont->size(), MissingETBase::Numerical::invalidLink());
	} // empty cluster link list
      } //first cluster
      // all clusters refer to the same MET contribution
      m_clusterLinks[(*fSig)->index()] = ld;
    } // extract CaloClusters
  } // loop over signal list

  return !firstCluster; // true if we processed any clusters
}

bool MissingETComponentMap_v1::setTracks(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList)
{
  // find all clusters
  // check input: MissingET object pointer
  size_t cIdx(this->findIndex(pMET)); if ( cIdx == MissingETBase::Numerical::invalidIndex() ) { return false; }
  // check input: contributing particle pointer
  size_t pIdx(this->at(cIdx)->findIndex(pPart)); if ( pIdx == MissingETBase::Numerical::invalidIndex()  ) { return false; }
  indexedlink_t ld(cIdx,pIdx); 
  // make sure cache is not cleared
  bool firstTrack = true;
  MissingETBase::Types::object_vector_t::const_iterator fSig(signalList.begin());
  MissingETBase::Types::object_vector_t::const_iterator lSig(signalList.end());
  for ( ; fSig != lSig; ++fSig ) {
    if((*fSig)->type()==xAOD::Type::TrackParticle) {
      if(firstTrack) {
	firstTrack = false;
	if ( m_trackLinks.empty() ) { 
	  const IParticleContainer* pCont(static_cast<const IParticleContainer*>((*fSig)->container()));
	  // if ( pCont == 0 )
	  //   { printf("MissingETComponentMap::setTracks(...) - WARNING   - cannot access track container (invalid NULL pointer), use %i words\n",(int)nTrack); }
	  // else
	  m_trackLinks.resize(pCont->size(), MissingETBase::Numerical::invalidLink());
	} // empty track link list
      } // first track
      // all tracks refer to the same MET contribution
      m_trackLinks[(*fSig)->index()] = ld;
    } // extract TrackParticles
  } // loop over signal list

  return !firstTrack; // true if we processed any clusters
}

bool MissingETComponentMap_v1::setObjects(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& objects)
{
  // check input: MissingET object pointer
  size_t cIdx(this->findIndex(pMET)); if ( cIdx == MissingETBase::Numerical::invalidIndex() ) { return false; }
  // loop input objects
  size_t oldSize(m_particleLinks.size());
  MissingETBase::Types::object_vector_t::const_iterator fObj(objects.begin());
  MissingETBase::Types::object_vector_t::const_iterator lObj(objects.end());
  for ( ; fObj != lObj; ++fObj ) { size_t pIdx(this->at(cIdx)->findIndex(pPart)); m_particleLinks[*fObj] = indexedlink_t(cIdx,pIdx); }
  //
  return oldSize < m_particleLinks.size();
} 

////////////////////
// Cache handlers //
////////////////////
 
void MissingETComponentMap_v1::f_setupLookupCache(signal_vector_t& signalLinks,size_t reserve)
{ signalLinks.clear(); if ( reserve > 0 ) { signalLinks.reserve(reserve); } }

void MissingETComponentMap_v1::f_clearLookupCache()
{ m_clusterLinks.clear(); m_trackLinks.clear(); m_particleLinks.clear(); }

void MissingETComponentMap_v1::f_fillLookupCache()
{
  ////
}

size_t MissingETComponentMap_v1::getComponentIndex(const IParticle* pPart) const { return f_getIndex<0>(pPart); }
size_t MissingETComponentMap_v1::getObjectIndex(const IParticle* pPart)    const { return f_getIndex<1>(pPart); }
// {
//   // check on cluster signal
//   const CaloCluster* pClus(dynamic_cast<const CaloCluster*>(pPart));
//   if ( pClus != 0 ) { return  pClus->index() < m_clusterLinks.size() ? m_clusterLinks.at(pClus->index()).get<1>() : MissingETBase::Numerical::invalidIndex(); }
//   // check on track index
//   const TrackParticle* pTrk(dynamic_cast<const TrackParticle*>(pPart));
//   if ( pTrk != 0 ) { return pTrk->index() < m_trackLinks.size() ? m_trackLinks.at(pTrk->index()).get<1>() : MissingETBase::Numerical::invalidIndex(); }
//   // check on physics objects
//   particle_map_t::const_iterator fPart(m_particleLinks.find(pPart));
//   return fPart != m_particleLinks.end() ? (fPart->second).get<1>() : MissingETBase::Numerical::invalidIndex();
// } 

//bool MissingETComponent_v1::addLink(const IParticle* pPart,MissingETComponent* pContrib,MissingETBase::Types::bitmask_t status)
//{
  // check if cluster signal object
  //  CaloCluster* pClus = dynamic_cast<const CaloCluster*>(pPart);
  //  if ( pClus != 0 ) return this->addCluster(pClus,pContrib,status);
  // check if track signal object
  //  TrackParticle* pTrk = dynamic_cast<const TrackParticle*>(pPart);
  //  if ( pTrk != 0 ) return this->addTrack(pTrk,pContrib,status);
  // build link key and data
//  particle_key_t key(pPart,status); 
//  contrib_idx_t idx = pContrib->index();
  // add to map
//  particle_map_t::iterator fLnk(m_partMETLookup.find(key));
//  if ( fLnk == m_partMETLookup.end() ) { m_partMETLookup.insert(particle_map_t::value_type(key,idx)); }
//  else { fLnk->second = idx; }
//  return true; 
//} 

//bool MissingETComponent_v1::addCluster(const CaloCluster* pClus,MissingETComponent* pContrib,MissingETBase::Types::bitmask_t /* status */)
//{
//  if ( m_clusterIndices.empty() ) { m_clusterIndices.resize(pClus->container()->size(),size_t(-1)); }
  //
//  if ( !this->checkCluster(pClus) ) { m_clusterIndices[pClus->index()] = pContrib->index(); return true; }
//  else { return false; }
//}

//bool MissingETComponent_v1::addTrack(const TrackParticle* pTrack,MissingETComponent* pContrib,MissingETBase::Types::bitmask_ty /* status */ )
//{
// if ( m_trackIndices.empty() ) { m_trackIndices.resize(pTrack->container()->size(),size_t(-1)); }
  //
//  if ( !this->checkTrack(pTrack) ) { m_trackIndices[pTrack->index()] = pContrib->index(); return true; }
//  else { return false; }
//}

//bool MissingETComponent_v1::checkCluster(const CaloCluster* pClus) const
//{ return pClus->index() < m_clusterIndices.size() && m_clusterIndices.at(pClus->index()) != size_t(-1); }

//bool MissingETComponenty_v1::checkTrack(const TrackParticle* pTrk) const
//{ return pTrk->index() < m_trackIndices.size() && m_trackIndices.at(pTrk->index()) != size_t(-1); }
