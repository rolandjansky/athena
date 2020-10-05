/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



// Don't use the StoreGateSvc interface here, so that this code
// will work in root too.

#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/StoreGate.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "SGTools/CurrentEventStore.h"
#include "SGTools/DataProxy.h"
#include <sstream>
#include <cassert>


namespace {


/**
 * @brief StoreGate keys to try for each EBC_EVCOLL enum.
 */
constexpr static int NKEYS = 5;
const
std::string s_keys[EBC_NCOLLKINDS][NKEYS] =
  {
    {"TruthEvent","G4Truth","GEN_AOD","GEN_EVENT","Bkg_TruthEvent"},
    {"TruthEvent_PU","G4Truth_PU","GEN_AOD_PU","GEN_EVENT_PU","Bkg_TruthEvent_PU"},
    {"TruthEvent_HighPtPU","G4Truth_HighPtPU","GEN_AOD_HighPtPU","GEN_EVENT_HighPtPU","Bkg_TruthEvent_HighPtPU"},
    {"TruthEvent_Cavern","G4Truth_Cavern","GEN_AOD_Cavern","GEN_EVENT_Cavern","Bkg_TruthEvent_Cavern"},
  };


/**
 * @brief Hint about where to start searching.
 *
 * On a successful SG lookup of a McEventCollection, we store here the index
 * in the s_keys array of the key that worked.  We can that start the
 * search at that position the next time.
 */
std::atomic<unsigned> s_hints[EBC_NCOLLKINDS] = {NKEYS, NKEYS, NKEYS, NKEYS};


const unsigned short CPTRMAXMSGCOUNT = 100;


} // anonymous namespace


//**************************************************************************
// ExtendedBarCode
//


/**
 * @brief Translate event collection enum to a char ('a'..'d').
 */
char
HepMcParticleLink::ExtendedBarCode::eventCollectionAsChar (EBC_EVCOLL evtColl)
{
  static const char codes[EBC_NCOLLKINDS] = {'a', 'b', 'c', 'd'};
  assert (evtColl < EBC_NCOLLKINDS);
  return codes[evtColl];
}


/**
 * @brief Translate event char ('a'..'d') to an enum.
 */
EBC_EVCOLL
HepMcParticleLink::ExtendedBarCode::eventCollectionFromChar (char evtColl)
{
  switch (evtColl) {
  case 'a': return EBC_MAINEVCOLL;
  case 'b': return EBC_FIRSTPUEVCOLL;
  case 'c': return EBC_SECONDPUEVCOLL;
  case 'd': return EBC_THIRDPUEVCOLL;
  default:
    // Should not reach this
    MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
    log << MSG::ERROR << " Wrong event collection (" << std::string(&evtColl,1) << ") set in HepMcParticleLink ExtendedBarCode object !!!" << endmsg;
  }
  return EBC_MAINEVCOLL;
}


/**
 * @brief Dump in textual format to a stream.
 */
void HepMcParticleLink::ExtendedBarCode::print (std::ostream& os) const
{
  os << "Event index " ;
  index_type index, position;
  eventIndex (index, position);
  if (position != UNDEFINED) {
    os << position << " (position in collection) ";
  }
  else {
    os << index;
  }
  os << ", Barcode " << m_BC
     << ", McEventCollection "
     << HepMcParticleLink::getLastEventCollectionName(this->getEventCollection())
     << "(" << eventCollectionAsChar (m_evtColl) << ")";
}


/**
 * @brief Dump in textual format to a MsgStream.
 */
void HepMcParticleLink::ExtendedBarCode::print (MsgStream& os) const
{
  std::ostringstream ss;
  print (ss);
  os << ss.str();
}


//**************************************************************************
// HepMcParticleLink
//


/**
 * @brief Constructor.
 * @param p Particle to reference.
 * @param eventIndex Identifies the target GenEvent in a McEventCollection,
 *        as either the number if @c isIndexEventPosition is IS_INDEX,
 *        or the position in the container
 *        if isIndexEventPosition is IS_POSITION.
 *        0 always means the first event in the collection.
 * @param evColl The targeted event collection, as an enum.
 * @param positionFlag: See @c eventIndex.
 * @param sg Optional specification of a specific store to reference.
 */
HepMcParticleLink::HepMcParticleLink (const HepMC::GenParticle* part,
                                      uint32_t eventIndex,
                                      EBC_EVCOLL evColl,
                                      PositionFlag positionFlag /*= IS_INDEX*/,
                                      IProxyDict* sg /*= SG::CurrentEventStore::store()*/)
  : m_ptrs (part),
    m_extBarcode((0 != part) ? HepMC::barcode(part) : 0, eventIndex, evColl, positionFlag)
{
  assert(part);

  if (part != 0 && positionFlag == IS_POSITION) {
    if (const McEventCollection* pEvtColl = retrieveMcEventCollection(sg)) {
      const HepMC::GenEvent *pEvt = pEvtColl->at (eventIndex);
      m_extBarcode.makeIndex (pEvt->event_number(), eventIndex);
    }
    else {
      MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
      log << MSG::WARNING << "cptr: McEventCollection not found" << endmsg;
    }
  }
}


/**
 * @brief Dereference.
 */
const HepMC::GenParticle* HepMcParticleLink::cptr() const
{
  const IProxyDict* sg = nullptr;
  auto p = m_ptrs.get (sg);
  if (!p) {
    if (0 == barcode()) {
#if 0
      MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
      log << MSG::DEBUG
             << "cptr: no truth particle associated with this hit (barcode==0)."
             << " Probably this is a noise hit" << endmsg;
#endif
      return nullptr;
    }
    if (!sg) {
      sg = SG::CurrentEventStore::store();
    }
    if (const McEventCollection* pEvtColl = retrieveMcEventCollection(sg)) {
      const HepMC::GenEvent *pEvt = nullptr;
      index_type index, position;
      m_extBarcode.eventIndex (index, position);
      if (index == 0) {
        pEvt = pEvtColl->at(0);
      }
      else if (position != ExtendedBarCode::UNDEFINED) {
        if (position < pEvtColl->size()) {
          pEvt = pEvtColl->at (position);
        }
        else {
#if 0
          MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
          log << MSG::WARNING << "cptr: position = " << position << ", McEventCollection size = "<< pEvtColl->size() << endmsg;
#endif
          return nullptr;
        }
      }
      else {
        pEvt = pEvtColl->find (index);
      }

      if (0 != pEvt) {
        auto pp = HepMC::barcode_to_particle(pEvt,barcode());
        if (pp) {
#ifdef HEPMC3
          m_ptrs.set (sg, pp.get());
          p=pp.get();
#else
          m_ptrs.set (sg, pp);
          p=pp;
#endif
        }
        if (position != ExtendedBarCode::UNDEFINED) {
          m_extBarcode.makeIndex (pEvt->event_number(), position);
        }
      } else {
        MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
        if (position != ExtendedBarCode::UNDEFINED) {
          log << MSG::WARNING
            << "cptr: Mc Truth not stored for event at " << position
            << endmsg;
        } else {
          log << MSG::WARNING
            << "cptr: Mc Truth not stored for event with event number " << index
            << endmsg;
        }
      }
    } else {
      MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
      log << MSG::WARNING << "cptr: McEventCollection not found" << endmsg;
    }
  }
  return p;
}


/**
 * @brief Return the event number of the referenced GenEvent.
 *        0 means the first GenEvent in the collection.
 */
HepMcParticleLink::index_type HepMcParticleLink::eventIndex() const
{
  index_type index, position;
  m_extBarcode.eventIndex (index, position);
  if (index == ExtendedBarCode::UNDEFINED) {
    const HepMC::GenEvent* pEvt{};
    const IProxyDict* sg{};
    auto p __attribute__ ((unused)) = m_ptrs.get (sg);
    if (const McEventCollection* coll = retrieveMcEventCollection (getEventCollection(),sg)) {
      if (position < coll->size()) {
        pEvt = coll->at (position);
      }
      if (pEvt) {
        const int event_number = pEvt->event_number();
        auto pp = HepMC::barcode_to_particle(pEvt,barcode());
        if (pp) {
#ifdef HEPMC3
          m_ptrs.set (sg, pp.get());
#else
          m_ptrs.set (sg, pp);
#endif
        }
        if(event_number>-1) {
          index = static_cast<index_type>(event_number);
          m_extBarcode.makeIndex (index, position);
          return index;
        }
      }
    }
  }
  // Don't trip the assertion for a null link.
  if (barcode() == 0 ) {  // || barcode() == 0x7fffffff)
    if (index != ExtendedBarCode::UNDEFINED) {
      return index;
    }
    return 0;
  }
  cptr();
  m_extBarcode.eventIndex (index, position);
  assert (index != ExtendedBarCode::UNDEFINED);
  return index;
}


/**
 * @brief Return the position in the McEventCollection of the
 *        (first) GenEvent with a given event number
 */
HepMcParticleLink::index_type
HepMcParticleLink::getEventPositionInCollection (const IProxyDict* sg) const
{
  index_type index, position;
  m_extBarcode.eventIndex (index, position);
  if (position != ExtendedBarCode::UNDEFINED) {
    return position;
  }
  if (index == 0) {
    return 0;
  }

  EBC_EVCOLL evColl = getEventCollection();
  std::vector<index_type> positions = getEventPositionInCollection(index, evColl, sg);
  return positions[0];
}


/**
 * @brief Return the position in the McEventCollection of the
 *        (first) GenEvent with a given event number
 */
std::vector<HepMcParticleLink::index_type>
HepMcParticleLink::getEventPositionInCollection (index_type index, EBC_EVCOLL evColl, const IProxyDict* sg)
{
  std::vector<index_type> positions; positions.reserve(1);
  const int intIndex = static_cast<int>(index);
  if (const McEventCollection* coll = retrieveMcEventCollection (evColl,sg)) {
    size_t sz = coll->size();
    for (size_t i = 0; i < sz; i++) {
      if ((*coll)[i]->event_number() == intIndex) {
        positions.push_back(i);
      }
    }
  }
  if (positions.empty() ) {
    positions.push_back(ExtendedBarCode::UNDEFINED);
  }
  return positions;
}


/**
 * @brief Return the event number of the GenEvent at the specified
 *        position in the McEventCollection.
 */
int HepMcParticleLink::getEventNumberAtPosition (index_type position, EBC_EVCOLL evColl, const IProxyDict* sg)
{
  if (const McEventCollection* coll = retrieveMcEventCollection (evColl,sg)) {
    if (position < coll->size()) {
      return coll->at (position)->event_number();
    }
  }
#if 0
  MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
  log << MSG::WARNING << "getEventNumberAtPosition: position = " << position << ", McEventCollection size = "<< coll->size() << endmsg;
#endif
  return -999;
}


/**
 * @brief Return the corresponding enum from a McEventCollection name.
 */
EBC_EVCOLL HepMcParticleLink::find_enumFromKey (const std::string& evCollName)
{
  for (unsigned int iEnum=0; iEnum<EBC_NCOLLKINDS; ++iEnum) {
    for (unsigned int iName=0;iName<NKEYS;iName++)
      if (evCollName==s_keys[iEnum][iName]) {
        return static_cast<EBC_EVCOLL>(iEnum);
      }
  }

  MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
  log << MSG::WARNING << "HepMcParticleLink::find_enumFromKey(" << evCollName << "): trying to find enum for unknown McEventCollection, returning " << EBC_MAINEVCOLL << endmsg;
  return EBC_MAINEVCOLL;
}


/** 
 * @brief Alter the persistent part of the link.
 */
void HepMcParticleLink::setExtendedBarCode (const ExtendedBarCode& extBarcode)
{
  m_extBarcode = extBarcode;
  m_ptrs.store (SG::CurrentEventStore::store());
}


/**
 * @brief Look up the event collection we're targeting.
 * @param evColl McEventCollection type
 * @param sg Target event store.
 * May return nullptr if the collection is not found.
 */
const McEventCollection*
HepMcParticleLink::retrieveMcEventCollection (EBC_EVCOLL evColl, const IProxyDict* sg)
{
  const McEventCollection* pEvtColl = nullptr;
  SG::DataProxy* proxy = find_proxy (evColl, sg);
  if (proxy) {
    pEvtColl = SG::DataProxy_cast<McEventCollection> (proxy);
    if (!pEvtColl) {
      MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
      log << MSG::WARNING << "cptr: McEventCollection not found" << endmsg;
    }
  }
  return pEvtColl;
}

/**
 * @brief Look up the event collection we're targeting.
 * @param sg Target event store.
 * May return nullptr if the collection is not found.
 */
const McEventCollection*
HepMcParticleLink::retrieveMcEventCollection (const IProxyDict* sg) const
{
  EBC_EVCOLL evColl = getEventCollection();
  return retrieveMcEventCollection(evColl, sg);
}


/**
 * @brief Find the proxy for the target event collection.
 * @param sg Target event store.
 * May return nullptr if the collection is not found.
 */
SG::DataProxy* HepMcParticleLink::find_proxy (EBC_EVCOLL evColl, const IProxyDict* sg)
{
  const CLID clid = ClassID_traits<McEventCollection>::ID();
  assert (evColl < EBC_NCOLLKINDS);
  unsigned int hint_orig = s_hints[evColl];
  if (hint_orig >= NKEYS) hint_orig = 0;
  unsigned int hint = hint_orig;
  do {
    SG::DataProxy* proxy = sg->proxy (clid, s_keys[evColl][hint]);
    if (proxy) {
      if (hint != s_hints[evColl]) {
        s_hints[evColl] = hint;
      }
      static std::atomic<unsigned> findCount {0};
      if(++findCount == 1) {
        MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
        log << MSG::INFO << "find_proxy: Using " << s_keys[evColl][hint]
            <<" as McEventCollection key for this job " << endmsg;
      }
      return proxy;
    }
    ++hint;
    if (hint >= NKEYS) hint = 0;
  } while (hint != hint_orig);

  MsgStream log (Athena::getMessageSvc(), "HepMcParticleLink");
  static std::atomic<unsigned long> msgCount {0};
  unsigned int count = ++msgCount;
  if (count <= CPTRMAXMSGCOUNT) {
    log << MSG::WARNING << "find_proxy: No Valid MC event Collection found "
        <<   endmsg;
  }
  if (count == CPTRMAXMSGCOUNT) {
    log << MSG::WARNING <<"find_proxy: suppressing further messages about valid MC event Collection. Use \n"
        << "  msgSvc.setVerbose += [HepMcParticleLink]\n"
        << "to see all messages" << endmsg;
  }
  if (count > CPTRMAXMSGCOUNT) {
    log << MSG::VERBOSE << "find_proxy: No Valid MC event Collection found "
        << endmsg;
  }
  return nullptr;
}


/**
 * @brief Return the most recent SG key used for a particular collection type.
 * @param evColl The targeted event collection, as an enum.
 */
std::string HepMcParticleLink::getLastEventCollectionName (EBC_EVCOLL evColl)
{
  static const std::string unset[EBC_NCOLLKINDS] =
    {
     "CollectionNotSet",
     "PUCollectionNotSet",
     "PU2CollectionNotSet",
     "PU3CollectionNotSet",
    };
  assert (evColl < EBC_NCOLLKINDS);
  unsigned idx = s_hints[evColl];
  if (idx < NKEYS) {
    return s_keys[evColl][idx];
  }
  return unset[evColl];
}


/**
 * @brief Output operator.
 * @param os Stream to which to output.
 * @param link Link to dump.
 */
std::ostream&
operator<< (std::ostream& os, const HepMcParticleLink& link)
{
  link.m_extBarcode.print(os);
  return os;
}


/**
 * @brief Output operator.
 * @param os MsgStream to which to output.
 * @param link Link to dump.
 */
MsgStream&
operator<< (MsgStream& os, const HepMcParticleLink& link)
{
  link.m_extBarcode.print(os);
  return os;
}
