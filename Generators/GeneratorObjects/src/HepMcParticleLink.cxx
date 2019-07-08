/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// Don't use the StoreGateSvc interface here, so that this code
// will work in root too.

#include <cassert>
#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/StoreGate.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "SGTools/CurrentEventStore.h"
#include "SGTools/DataProxy.h"

#include "GeneratorObjects/HepMcParticleLink.h"

const std::string HepMcParticleLink::s_DEFAULTKEY("TruthEvent");
const std::string HepMcParticleLink::s_DC2DEFAULTKEY("G4Truth");
const std::string HepMcParticleLink::s_AODKEY("GEN_AOD");
std::string HepMcParticleLink::s_HOSTKEY;

namespace {
  MsgStream& mlog() {
    static MsgStream s_log(Athena::getMessageSvc(), "HepMcParticleLink");     
    return s_log;
  }
  const unsigned short CPTRMAXMSGCOUNT(100);
}

HepMcParticleLink::HepMcParticleLink(const HepMC::GenParticle* part, 
				     uint32_t eventIndex) :
    m_extBarcode((0 != part) ? part->barcode() : 0, eventIndex),
    m_have_particle (true)
{
  m_ptrs.m_particle = const_cast<HepMC::GenParticle*>(part);
  assert(part);
}

const HepMC::GenParticle* HepMcParticleLink::cptr() const {
  if (!m_have_particle) {
    if (0 == barcode()) {
#if 0
      mlog() << MSG::DEBUG
	     << "cptr: no truth particle associated with this hit (barcode==0)."
	     << " Probably this is a noise hit" << endmsg;
#endif
      return nullptr;
    }
    CLID clid = ClassID_traits<McEventCollection>::ID();
    const McEventCollection* pEvtColl(0);
    if(s_HOSTKEY.empty() ) {
      if (!find_hostkey()) return nullptr;
    }
    SG::DataProxy* proxy = m_ptrs.m_dict->proxy (clid, s_HOSTKEY);
    if (!proxy) {
      if (!find_hostkey()) return nullptr;
      proxy = m_ptrs.m_dict->proxy (clid, s_HOSTKEY);
    }
    if (proxy && 
	(0 != (pEvtColl = SG::DataProxy_cast<McEventCollection> (proxy)))) {
      const HepMC::GenEvent *pEvt((0 == eventIndex()) ?
				  pEvtColl->at(0) : //original event is at EvtColl[0]
				  pEvtColl->find(eventIndex()));
      if (0 != pEvt) {
	m_ptrs.m_particle = pEvt->barcode_to_particle(barcode());
        m_have_particle = true;
      } else {
	mlog() << MSG::WARNING
	       << "cptr: Mc Truth not stored for event " << eventIndex()
               << " of " << m_ptrs.m_dict->name() << "/" << s_HOSTKEY
	       << endreq;
      }
    } else {
      mlog() << MSG::WARNING << "cptr: McEventCollection called " << s_HOSTKEY
             << " not found in " << m_ptrs.m_dict->name() << endreq;
    }
  }

  if (m_have_particle)
    return m_ptrs.m_particle;
  return nullptr;
}



// This is a bit overly complicated at the moment in order to work
// with both the old and new DataModel.
void HepMcParticleLink::init_dict()
{
  m_have_particle = false;
  m_ptrs.m_dict = SG::CurrentEventStore::store();
}


bool HepMcParticleLink::find_hostkey() const
{
  static unsigned short msgCount(0);
  assert (!m_have_particle);
  CLID clid = ClassID_traits<McEventCollection>::ID();
  s_HOSTKEY.clear();
  if (m_ptrs.m_dict->proxy (clid, s_DEFAULTKEY))
    s_HOSTKEY=s_DEFAULTKEY;
  else if (m_ptrs.m_dict->proxy (clid, s_DC2DEFAULTKEY))
    s_HOSTKEY=s_DC2DEFAULTKEY;
  else if (m_ptrs.m_dict->proxy (clid, s_AODKEY))
    s_HOSTKEY=s_AODKEY;
  if (!s_HOSTKEY.empty()) {
    mlog() << MSG::INFO << "find_hostkey: Using " << m_ptrs.m_dict->name() << "/"
           << s_HOSTKEY << " as McEventCollection key for this job " << endreq;
    return true;
  }
  if (msgCount<CPTRMAXMSGCOUNT) {
    mlog() << MSG::WARNING << "find_hostkey: No Valid MC event Collection found "
	   <<   endreq;
    ++msgCount;
  } else if (msgCount==CPTRMAXMSGCOUNT) {
    mlog() << MSG::WARNING <<"find_hostkey: suppressing further messages about valid MC event Collection. Use \n"
	   << "  msgSvc.setVerbose += [HepMcParticleLink]\n"
	   << "to see all messages" << endreq;
    ++msgCount;
  } else {
    mlog() << MSG::VERBOSE << "find_hostkey: No Valid MC event Collection found "
	   << endreq;
  }
  return false;
}


