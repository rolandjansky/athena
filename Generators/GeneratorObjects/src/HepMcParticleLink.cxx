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

SG::ReadHandle<McEventCollection>* HepMcParticleLink::s_MAP_MCEVENTCOLLECTION_READHANDLE[EBC_NCOLLKINDS];
std::string HepMcParticleLink::s_HOSTKEY_MAINEVCOL;
std::string HepMcParticleLink::s_HOSTKEY_FIRSTPUEVCOL;
std::string HepMcParticleLink::s_HOSTKEY_SECONDPUEVCOL;
std::string HepMcParticleLink::s_HOSTKEY_THIRDPUEVCOL;
const std::string HepMcParticleLink::s_MAINEVCOLKEYS[4]={"TruthEvent","G4Truth","GEN_AOD","GEN_EVENT"};
const std::string HepMcParticleLink::s_FIRSTPUEVCOLKEYS[4]={"TruthEvent_PU","G4Truth_PU","GEN_AOD_PU","GEN_EVENT_PU"};
const std::string HepMcParticleLink::s_SECONDPUEVCOLKEYS[4]={"TruthEvent_HighPtPU","G4Truth_HighPtPU","GEN_AOD_HighPtPU","GEN_EVENT_HighPtPU"};
const std::string HepMcParticleLink::s_THIRDPUEVCOLKEYS[4]={"TruthEvent_Cavern","G4Truth_Cavern","GEN_AOD_Cavern","GEN_EVENT_Cavern"};

namespace {
  MsgStream& mlog() {
    static MsgStream s_log(Athena::getMessageSvc(), "HepMcParticleLink");     
    return s_log;
  }
  const unsigned short CPTRMAXMSGCOUNT(100);
}

HepMcParticleLink::HepMcParticleLink(const HepMC::GenParticle* part,
                                     uint32_t eventIndex,
                                     EBC_EVCOLL evColl,
                                     bool isIndexEventPosition) :
  m_extBarcode((0 != part) ? part->barcode() : 0, eventIndex, evColl, isIndexEventPosition)
{
  m_particle = const_cast<HepMC::GenParticle*>(part);
  m_have_particle=true;
  assert(part);
}

HepMcParticleLink::HepMcParticleLink(const HepMC::GenParticle* part,
                                     uint32_t eventIndex,
                                     std::string evCollName,
                                     bool isIndexEventPosition) :
  m_extBarcode((0 != part) ? part->barcode() : 0, eventIndex, find_enumFromKey(evCollName), isIndexEventPosition)
{
  m_particle = const_cast<HepMC::GenParticle*>(part);
  m_have_particle=true;
  assert(part);
}


HepMcParticleLink::index_type HepMcParticleLink::getEventNumberForEventPosition(EBC_EVCOLL evColl, HepMcParticleLink::index_type position, IProxyDict* sg) {
    CLID clid = ClassID_traits<McEventCollection>::ID();
    const McEventCollection* pEvtColl(nullptr);
    const std::string * colNames;
    if (evColl==EBC_MAINEVCOLL) {
      colNames=s_MAINEVCOLKEYS;
    }
    else if (evColl==EBC_FIRSTPUEVCOLL) {
      colNames=s_FIRSTPUEVCOLKEYS;
    }
    else if (evColl==EBC_SECONDPUEVCOLL) {
      colNames=s_SECONDPUEVCOLKEYS;
    }
    else if (evColl==EBC_THIRDPUEVCOLL) {
    colNames=s_THIRDPUEVCOLKEYS;
    }
    else {
      return ExtendedBarCode::UNDEFINED;
    }

    for (unsigned int iName=0; iName<4; iName++) {
      SG::DataProxy* proxy = sg->proxy(clid, colNames[iName]);
      if (proxy) {
        pEvtColl = SG::DataProxy_cast<McEventCollection> (proxy);
        if(pEvtColl && pEvtColl->size()>position) {
          const HepMC::GenEvent *pEvt(pEvtColl->at(position));
          if(pEvt) {
            return pEvt->event_number();
          }
          break;
        }
      }
    }
    return ExtendedBarCode::UNDEFINED;
}

HepMcParticleLink::index_type HepMcParticleLink::getEventNumberForEventPosition(EBC_EVCOLL evColl, HepMcParticleLink::index_type position) {
  const McEventCollection* pEvtColl = retrieveMcEventCollection(evColl);
  if (pEvtColl && position<pEvtColl->size()) {
    const HepMC::GenEvent * pEvt(pEvtColl->at(position));
    if (pEvt) {
      return pEvt->event_number();
    }
  }
  std::cout << "Failed to find GenEvent at position " << position << " in " << getEventCollectionAsString(evColl) << ". McEventCollection size is " << pEvtColl->size() << std::endl;
  return ExtendedBarCode::UNDEFINED;
}

HepMcParticleLink::index_type HepMcParticleLink::getEventNumberForEventPosition(HepMcParticleLink::index_type position) const {
  return getEventNumberForEventPosition(this->getEventCollection(),position);
}


HepMcParticleLink::index_type HepMcParticleLink::getEventPositionForEventNumber(EBC_EVCOLL evColl, HepMcParticleLink::index_type evNumber) {
  const McEventCollection* pEvtColl = retrieveMcEventCollection(evColl);
  if (pEvtColl) {
    for (unsigned int position=0; position<pEvtColl->size(); position++) {
      const HepMC::GenEvent * pEvt(pEvtColl->at(position));
      if (pEvt && static_cast<unsigned int>(pEvt->event_number())==evNumber) { return position; }
    }
  }
  return ExtendedBarCode::UNDEFINED;
}

const McEventCollection* HepMcParticleLink::retrieveMcEventCollection(EBC_EVCOLL evColl) {
  if (s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl] && s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl]->isValid()) {
    return s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl]->ptr();
  }
  else {
    //Let's first determine the McEventCollection name to use
    std::string * hostkeyToGetFrom=nullptr;
    if (evColl==EBC_MAINEVCOLL)
      hostkeyToGetFrom=&s_HOSTKEY_MAINEVCOL;
    else if (evColl==EBC_FIRSTPUEVCOLL)
      hostkeyToGetFrom=&s_HOSTKEY_FIRSTPUEVCOL;
    else if (evColl==EBC_SECONDPUEVCOLL)
      hostkeyToGetFrom=&s_HOSTKEY_SECONDPUEVCOL;
    else if (evColl==EBC_THIRDPUEVCOLL)
      hostkeyToGetFrom=&s_HOSTKEY_THIRDPUEVCOL;
    else {
      //Should not reach this
      mlog() << MSG::ERROR << " Looking for incorrect event collection in HepMcParticleLink !!!" << endreq;
      return nullptr;
    }

    if (! (hostkeyToGetFrom=find_hostkey(evColl))) return nullptr;
    if (s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl]) delete s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl];
    s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl] = new SG::ReadHandle<McEventCollection>(*hostkeyToGetFrom);
  }//if not already-existing ReadHandle

  return s_MAP_MCEVENTCOLLECTION_READHANDLE[evColl]->ptr();
}


const HepMC::GenParticle* HepMcParticleLink::cptr() const {
  if (m_have_particle) { return m_particle; }
  else {
    if (0 == barcode()) {
#if 0
      mlog() << MSG::DEBUG
             << "cptr: no truth particle associated with this hit (barcode==0)."
             << " Probably this is a noise hit" << endmsg;
#endif
      return nullptr;
    }

    const McEventCollection* pEvtColl(nullptr);
    if ((pEvtColl = retrieveMcEventCollection())) {
      const int eventNumber = static_cast<int>(eventIndex());
      if(!pEvtColl->empty()) {
        const index_type index = m_extBarcode.eventIndex();
        bool tryPosition = (isIndexAsEventPosition() && index < pEvtColl->size()) || (0 == index); //original event is at EvtColl[0]
        const HepMC::GenEvent *pEvt( tryPosition ?
                                     pEvtColl->at(index) :
                                     pEvtColl->find(eventNumber));
        if (nullptr != pEvt) {
          m_particle = pEvt->barcode_to_particle(barcode());
          m_have_particle = true;
          return m_particle;
        }
        else {
          const std:: string posString = isIndexAsEventPosition() ? "at position " : "with event number ";
          mlog() << MSG::WARNING
                 << "cptr: Mc Truth not stored for GenEvent "
                 << posString.c_str()
                 << eventNumber
                 << ", size of McEventCollection = " << pEvtColl->size()
                 << endreq;
        }
      }
      else {
        mlog() << MSG::WARNING << "cptr: McEventCollection empty!" << endreq;
      }
    }
    else {
      mlog() << MSG::WARNING << "cptr: McEventCollection not found" << endreq;
    }
  }//if (!m_have_particle)

  return nullptr;
}


std::string * HepMcParticleLink::find_hostkey(EBC_EVCOLL evColl)
{
  static unsigned short msgCount(0);

  const std::string * colNames;
  std::string * hostkeyToSet=nullptr;
  if (evColl==EBC_MAINEVCOLL) {
    colNames=s_MAINEVCOLKEYS;
    hostkeyToSet=&s_HOSTKEY_MAINEVCOL;
  }
  else if (evColl==EBC_FIRSTPUEVCOLL) {
    colNames=s_FIRSTPUEVCOLKEYS;
    hostkeyToSet=&s_HOSTKEY_FIRSTPUEVCOL;
  }
  else if (evColl==EBC_SECONDPUEVCOLL) {
    colNames=s_SECONDPUEVCOLKEYS;
    hostkeyToSet=&s_HOSTKEY_SECONDPUEVCOL;
  }
  else if (evColl==EBC_THIRDPUEVCOLL) {
    colNames=s_THIRDPUEVCOLKEYS;
    hostkeyToSet=&s_HOSTKEY_THIRDPUEVCOL;
  }
  else {
    //Should not reach this
    mlog() << MSG::ERROR << " Looking for incorrect event collection in HepMcParticleLink !!!" << endreq;
    return nullptr;
  }

  hostkeyToSet->clear();
  for (unsigned int iName=0; iName<4 && hostkeyToSet->empty(); iName++) {
    if (SG::ReadHandle<McEventCollection>(colNames[iName]).isValid()) {
      *hostkeyToSet=colNames[iName];
    }
  }
  if (!hostkeyToSet->empty()) {
    mlog() << MSG::INFO << "find_hostkey: Using " << *hostkeyToSet
	   <<" as McEventCollection key for this job " << endreq;
    return hostkeyToSet;
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
  return nullptr;
}


EBC_EVCOLL HepMcParticleLink::find_enumFromKey(std::string evCollName) {
  const std::string * colNames;
  for (unsigned int iEnum=0; iEnum<EBC_NCOLLKINDS; iEnum++) {
    switch (iEnum) {
     case 0:
      colNames=s_MAINEVCOLKEYS;    break;
     case 1:
      colNames=s_FIRSTPUEVCOLKEYS; break;
     case 2:
      colNames=s_SECONDPUEVCOLKEYS; break;
     case 3:
      colNames=s_THIRDPUEVCOLKEYS; break;
    }
    for (unsigned int iName=0;iName<4;iName++)
      if (evCollName==colNames[iName])
	return (EBC_EVCOLL)iEnum;
  }

  mlog() << MSG::WARNING << "HepMcParticleLink::find_enumFromKey(" << evCollName << "): trying to find enum for unknown McEventCollection, returning " << EBC_MAINEVCOLL << endreq;
  return EBC_MAINEVCOLL;
}



//Returns the stored name for the collection found in SG for the types of GenEvents passed as argument
std::string HepMcParticleLink::getEventCollectionAsString(EBC_EVCOLL evColl) {
  switch (evColl) {
    case EBC_MAINEVCOLL:
      if (s_HOSTKEY_MAINEVCOL.empty()) {
	std::string * pStr = find_hostkey(evColl);
	if (pStr)
	  return (s_HOSTKEY_MAINEVCOL=*pStr);
	else
	  return "CollectionNotSet";
      }
      return s_HOSTKEY_MAINEVCOL;
      break;
    case EBC_FIRSTPUEVCOLL:
      if (s_HOSTKEY_FIRSTPUEVCOL.empty()) {
	std::string * pStr = find_hostkey(evColl);
	if (pStr)
	  return (s_HOSTKEY_FIRSTPUEVCOL=*pStr);
	else
	  return "PUCollectionNotSet";
      }
      return s_HOSTKEY_FIRSTPUEVCOL;
      break;
     case EBC_SECONDPUEVCOLL:
      if (s_HOSTKEY_SECONDPUEVCOL.empty()) {
	std::string * pStr = find_hostkey(evColl);
	if (pStr)
	  return (s_HOSTKEY_SECONDPUEVCOL=*pStr);
	else
	  return "PUCollectionNotSet";
      }
      return s_HOSTKEY_SECONDPUEVCOL;
      break;
     case EBC_THIRDPUEVCOLL:
      if (s_HOSTKEY_THIRDPUEVCOL.empty()) {
	std::string * pStr = find_hostkey(evColl);
	if (pStr)
	  return (s_HOSTKEY_THIRDPUEVCOL=*pStr);
	else
	  return "PUCollectionNotSet";
      }
      return s_HOSTKEY_THIRDPUEVCOL;
      break;
     default:
       //Should not reach this
       mlog() << MSG::ERROR << " Trying to retrieve collection name for wrong event collection enum in HepMcParticleLink !!!" << endreq;
       return (!s_HOSTKEY_MAINEVCOL.empty()) ? s_HOSTKEY_MAINEVCOL : s_HOSTKEY_MAINEVCOL=*find_hostkey(evColl);
  }
}

//Accessors serving as interface between enums defining the kind of collections used and the format in memory (char)
void HepMcParticleLink::ExtendedBarCode::setEventCollection(EBC_EVCOLL evColl) {
  if (evColl==EBC_MAINEVCOLL)
    m_evtColl='a';
  else if (evColl==EBC_FIRSTPUEVCOLL)
    m_evtColl='b';
  else if (evColl==EBC_SECONDPUEVCOLL)
    m_evtColl='c';
  else if (evColl==EBC_THIRDPUEVCOLL)
    m_evtColl='d';
  else {
    //Should not reach this
    mlog() << MSG::ERROR << " Trying to set wrong event collection (" << evColl << ") to HepMcParticleLink ExtendedBarCode object !!!" << endreq;
    m_evtColl='a';
  }
}
EBC_EVCOLL HepMcParticleLink::ExtendedBarCode::getEventCollection(char evtColl) {
  if (evtColl=='a')
    return EBC_MAINEVCOLL;
  else if (evtColl=='b')
    return EBC_FIRSTPUEVCOLL;
  else if (evtColl=='c')
    return EBC_SECONDPUEVCOLL;
  else if (evtColl=='d')
    return EBC_THIRDPUEVCOLL;
  //Should not reach this
  mlog() << MSG::ERROR << " Wrong event collection (" << std::string(&evtColl,1) << ") set in HepMcParticleLink ExtendedBarCode object !!!" << endreq;
  return EBC_MAINEVCOLL;
}

