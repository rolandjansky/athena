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
#include "AthLinks/tools/SGgetDataSource.h"
#include "SGTools/DataProxy.h"

#include "GeneratorObjects/HepMcParticleLink.h"

const std::string HepMcParticleLink::DEFAULTKEY("TruthEvent");
const std::string HepMcParticleLink::DC2DEFAULTKEY("G4Truth");
const std::string HepMcParticleLink::AODKEY("GEN_AOD");
std::string HepMcParticleLink::HOSTKEY;
IProxyDict** HepMcParticleLink::s_defsource_ptr = 0;

namespace {
  MsgStream& mlog() {
    static MsgStream s_log(Athena::getMessageSvc(), "HepMcParticleLink");     
    return s_log;
  }
  const unsigned short CPTRMAXMSGCOUNT(100);
}

HepMcParticleLink::HepMcParticleLink(const HepMC::GenParticle* part, 
				     index_type eventIndex) :
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
      return 0;
    }
    CLID clid = ClassID_traits<McEventCollection>::ID();
    const McEventCollection* pEvtColl(0);
    if(HOSTKEY.empty() ) {
      if (!find_hostkey()) return 0;
    }
    SG::DataProxy* proxy = m_ptrs.m_dict->proxy (clid, HOSTKEY);
    if (!proxy) {
      if (!find_hostkey()) return 0;
      proxy = m_ptrs.m_dict->proxy (clid, HOSTKEY);
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
	       << endreq;
      }
    } else {
      mlog() << MSG::WARNING << "cptr: McEventCollection not found" << endreq;
    }
  }

  if (m_have_particle)
    return m_ptrs.m_particle;
  return 0;
}



// This is a bit overly complicated at the moment in order to work
// with both the old and new DataModel.
#if 0
void HepMcParticleLink::init_dict()
{
  m_have_particle = false;
  if (!s_defsource_ptr)
    s_defsource_ptr = SG::getDataSourcePointer ("StoreGateSvc");
  m_ptrs.m_dict = *s_defsource_ptr;
}
#endif
namespace {
template <typename T>
IProxyDict** dict_compat_helper (T** p, int& offset)
{
  T* foo = *p;
  IProxyDict* bar = foo;
  offset = (char*)bar - (char*)foo;
  return reinterpret_cast<IProxyDict**>(p);
}
}
void HepMcParticleLink::init_dict()
{
  m_have_particle = false;
  static int offset = 0;
  if (!s_defsource_ptr) {
    s_defsource_ptr = dict_compat_helper (SG::getDataSourcePointer ("StoreGateSvc"),
                                          offset);
  }
  m_ptrs.m_dict = (IProxyDict*)((char*)*s_defsource_ptr + offset);
}


bool HepMcParticleLink::find_hostkey() const
{
  static unsigned short msgCount(0);
  assert (!m_have_particle);
  CLID clid = ClassID_traits<McEventCollection>::ID();
  HOSTKEY.clear();
  if (m_ptrs.m_dict->proxy (clid, DEFAULTKEY))
    HOSTKEY=DEFAULTKEY;
  else if (m_ptrs.m_dict->proxy (clid, DC2DEFAULTKEY))
    HOSTKEY=DC2DEFAULTKEY;
  else if (m_ptrs.m_dict->proxy (clid, AODKEY))
    HOSTKEY=AODKEY;
  if (!HOSTKEY.empty()) {
    mlog() << MSG::INFO << "find_hostkey: Using " << HOSTKEY
        <<" as McEventCollection key for this job " << endreq;
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


// Reset the static cached dictionary pointer.
// Sometimes we can get the wrong one if these objects get constructed
// too early during program initialization, such as when reading
// dictionaries.
void HepMcParticleLink::resetCachedDictionary()
{
  s_defsource_ptr = 0;
}
