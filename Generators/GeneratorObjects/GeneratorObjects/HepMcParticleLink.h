/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTS_HEPMCPARTICLELINK_H
#define GENERATOROBJECTS_HEPMCPARTICLELINK_H
/** @class HepMcParticleLink
 * @brief a link optimized in size for a GenParticle in a McEventCollection
 *
 * @see McEventCollection, GenEvent, ElementLink
 * @author Paolo Calafiura
 * $Id: HepMcParticleLink.h,v 1.11 2007-09-13 05:53:34 dquarrie Exp $
 **/
#include <cassert>
#include <cstdint> /*int32_t etc*/
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include <map>
#include <limits>  /* numeric_limits */
#include <string>
#include "StoreGate/ReadHandle.h"


namespace HepMC {
  class GenParticle;
  class GenEvent;
}
class IProxyDict;
class McEventCollection;

enum EBC_EVCOLL{
  EBC_MAINEVCOLL=0,   //Usually contains the HS GenEvent
  EBC_FIRSTPUEVCOLL,  //Low pt minbias PU, or full PU collection where applicable (i.e. Fast Chain)
  EBC_SECONDPUEVCOLL, //High pt minbias PU
  EBC_THIRDPUEVCOLL,  //Reserved for cavern background
  EBC_NCOLLKINDS      //Just contains the maximum of the enum (for loops)
};

class HepMcParticleLink {
public:
  typedef uint32_t barcode_type;
  typedef uint32_t index_type;

  /// \name structors
  //@{
  HepMcParticleLink() : m_particle(nullptr), m_have_particle(false) { }
  HepMcParticleLink(barcode_type barCode, uint32_t eventIndex = 0, EBC_EVCOLL evColl=EBC_MAINEVCOLL, bool isIndexEventPosition=false) : m_particle(nullptr), m_have_particle(false)
  { m_extBarcode = ExtendedBarCode(barCode, eventIndex, evColl, ((!barCode && !eventIndex) || isIndexEventPosition));}
  HepMcParticleLink(barcode_type barCode, uint32_t eventIndex, std::string evCollName, bool isIndexEventPosition=false) : m_particle(nullptr), m_have_particle(false)
  { m_extBarcode = ExtendedBarCode(barCode, eventIndex, find_enumFromKey(evCollName), ((!barCode && !eventIndex) || isIndexEventPosition));}
  HepMcParticleLink(const HepMC::GenParticle* p, uint32_t eventIndex = 0, EBC_EVCOLL evColl=EBC_MAINEVCOLL, bool isIndexEventPosition=false);
  HepMcParticleLink(const HepMC::GenParticle* p, uint32_t eventIndex, std::string evCollName, bool isIndexEventPosition=false);
  HepMcParticleLink(const HepMcParticleLink& rhs) :
    m_particle(rhs.m_particle),
    m_extBarcode(rhs.m_extBarcode),
    m_have_particle (rhs.m_have_particle){ if (!rhs.m_have_particle) {m_particle = nullptr;} }

  HepMcParticleLink& operator= (const HepMcParticleLink& rhs)
  {
    if (this != &rhs) {
      m_particle = rhs.m_particle;
      m_extBarcode = rhs.m_extBarcode;
      m_have_particle = rhs.m_have_particle;
      if (!rhs.m_have_particle) {m_particle = nullptr;}
    }
    return *this;
  }
  ////  HepMcParticleLink(const HepMC::GenParticle* part,
  ////                const HepMC::GenEvent* pevt); //FIXME NOT YET
  //@}

  /// \name pointer interface
  //@{
  /// @throws std::runtime_error when the element is not found
  const HepMC::GenParticle& operator* () const { return *cptr(); }//FIXME
  const HepMC::GenParticle* operator->() const { return cptr(); }
  operator const HepMC::GenParticle* () const { return cptr(); }
  bool operator!() const {return !isValid();}
  bool operator == (const HepMcParticleLink& rhs) const {
    if (m_extBarcode==rhs.m_extBarcode)
      return true;
    else if (m_extBarcode.barcode()==rhs.m_extBarcode.barcode() && m_extBarcode.getEventCollectionAsChar()==rhs.m_extBarcode.getEventCollectionAsChar()) {
      if (m_extBarcode.isIndexAsEventPosition()==rhs.m_extBarcode.isIndexAsEventPosition())
        return false; //No need to check m_extBarcode.eventIndex() since if they were similar the first 'if' would have been passed
      else {
        index_type num1=getEventPositionInCollection(), num2=rhs.getEventPositionInCollection();
        return (num1==num2);
      }
    }
    else
      return false;
  }
  bool operator != (const HepMcParticleLink& rhs) const {
    return !(operator==(rhs)); }
  bool operator < (const HepMcParticleLink& rhs) const {
    if (m_extBarcode.getEventCollectionAsChar() < rhs.m_extBarcode.getEventCollectionAsChar()) return true;
    if (!m_extBarcode.isIndexAsEventPosition() && !rhs.m_extBarcode.isIndexAsEventPosition())
      return ( m_extBarcode < rhs.m_extBarcode );
    if (m_extBarcode.getEventCollectionAsChar()==rhs.m_extBarcode.getEventCollectionAsChar()) {
      index_type num1=m_extBarcode.eventIndex(), num2=rhs.m_extBarcode.eventIndex();
      if (m_extBarcode.isIndexAsEventPosition())
        num1=getEventNumberForEventPosition(num1);
      if (rhs.m_extBarcode.isIndexAsEventPosition())
        num2=getEventNumberForEventPosition(num2);
      if (num1<num2) return true;
      if (num1==num2) return (m_extBarcode.barcode() < rhs.m_extBarcode.barcode());
    }
    return false;
  }
  friend std::ostream& operator << (std::ostream&, const HepMcParticleLink&);
  friend MsgStream& operator << (MsgStream&, const HepMcParticleLink&);

  //@}

  /// \name indexing accessors (e.g. for writing)
  //@{
  int barcode() const { return int(m_extBarcode.barcode()); } //FIXME ret type
  static index_type getEventNumberForEventPosition(EBC_EVCOLL evColl, index_type position, IProxyDict* sg);
  static index_type getEventNumberForEventPosition(EBC_EVCOLL evColl, index_type position) ; //if the an old event index (position in the collection) is stored, returns the event number of the corresponding GenEvent in the specified collection
  index_type getEventNumberForEventPosition(index_type position) const; //if the an old event index (position in the collection) is stored, returns the event number of the corresponding GenEvent
  static index_type getEventPositionForEventNumber(EBC_EVCOLL evColl, index_type evNumber); //Returns the position/index in the specified collection of the event having the given event number
  index_type getEventPositionInCollection() const { if (m_extBarcode.isIndexAsEventPosition()) return m_extBarcode.eventIndex(); else return getEventPositionForEventNumber(this->getEventCollection(),m_extBarcode.eventIndex());};//Returns the position/index of the event in the collection
  index_type eventIndex() const { if (m_extBarcode.isIndexAsEventPosition()) {return getEventNumberForEventPosition(m_extBarcode.eventIndex());} return index_type(m_extBarcode.eventIndex()); }
  bool isIndexAsEventPosition() const { return m_extBarcode.isIndexAsEventPosition(); }
  EBC_EVCOLL getEventCollection() const { return m_extBarcode.getEventCollection(); };
  void setEventCollection(EBC_EVCOLL evColl) { m_extBarcode.setEventCollection(evColl); };
  static std::string getEventCollectionAsString(EBC_EVCOLL evColl);
  std::string getEventCollectionAsString() const {return getEventCollectionAsString(m_extBarcode.getEventCollection());};
  char getEventCollectionAsChar() const {return m_extBarcode.getEventCollectionAsChar();};
  ///hash the 32 bit barcode and 16 bit eventindex into a 32bit int
  barcode_type compress() const {
    return ( ((m_extBarcode.barcode()&0xFFFF) << 16) |
             eventIndex() );
  };
  //@}

  bool isValid() const { return (0 != cptr()); }
  const HepMC::GenParticle* cptr() const;

  //Return the corresponding enum from a McEventCollection name
  static EBC_EVCOLL find_enumFromKey(std::string evCollName);

  //Returns the McEventCollection
  static const McEventCollection* retrieveMcEventCollection(EBC_EVCOLL evColl);
  const  McEventCollection* retrieveMcEventCollection() const { return retrieveMcEventCollection(this->getEventCollection());};

  class ExtendedBarCode {
  public:
    /// All 1's. Used to represent an undefined index/position.
    constexpr static index_type UNDEFINED = ~static_cast<index_type>(0);

    ExtendedBarCode() : m_BC(0), m_evtIndex(0), m_isIndexEventPosition(false) {setEventCollection(EBC_MAINEVCOLL);}
    ExtendedBarCode(barcode_type barcode, index_type eventIndex, EBC_EVCOLL evtColl=EBC_MAINEVCOLL, bool isIndexEventPosition=false) :
      m_BC(barcode), m_evtIndex(eventIndex), m_isIndexEventPosition(isIndexEventPosition)
    {
      assert(eventIndex < std::numeric_limits<unsigned int>::max());
      setEventCollection(evtColl);
    }
    ExtendedBarCode(barcode_type barcode, index_type eventIndex, char evtColl, bool isIndexEventPosition=false) :
      m_BC(barcode), m_evtIndex(eventIndex), m_isIndexEventPosition(isIndexEventPosition)
    {
      assert(eventIndex < std::numeric_limits<unsigned int>::max());
      setEventCollection(getEventCollection(evtColl));
    }
    ExtendedBarCode(const ExtendedBarCode& rhs) { m_BC=rhs.m_BC; m_evtIndex=rhs.m_evtIndex; m_evtColl=rhs.m_evtColl; m_isIndexEventPosition=rhs.m_isIndexEventPosition; };
    ExtendedBarCode& operator= (const ExtendedBarCode& rhs) { if (this != &rhs) { m_BC=rhs.m_BC; m_evtIndex=rhs.m_evtIndex; m_evtColl=rhs.m_evtColl; m_isIndexEventPosition=rhs.m_isIndexEventPosition;} return *this; };

    inline barcode_type barcode() const { return m_BC; }
    inline index_type eventIndex() const { return m_evtIndex; }
    inline bool isIndexAsEventPosition() const { return m_isIndexEventPosition; }
    void setEventCollection(EBC_EVCOLL evColl);
    static EBC_EVCOLL getEventCollection(char evtColl);
    EBC_EVCOLL getEventCollection() const {return getEventCollection(m_evtColl);} ;
    std::string getEventCollectionAsString() const {return HepMcParticleLink::getEventCollectionAsString(this->getEventCollection());};
    inline char getEventCollectionAsChar() const {return m_evtColl;};
    bool operator == (const ExtendedBarCode& rhs ) const
    {
      return ( (this->m_BC)==(rhs.m_BC) && (this->m_evtColl)==(rhs.m_evtColl) && (this->m_evtIndex)==(rhs.m_evtIndex) && (this->m_isIndexEventPosition)==(rhs.m_isIndexEventPosition) ) ;
    }
    bool operator != (const ExtendedBarCode& rhs ) const {
      return !(operator==(rhs));
    }
    bool operator < (const ExtendedBarCode& rhs ) const
    {
      return ((m_evtColl < rhs.m_evtColl) ||
              ((m_evtColl == rhs.m_evtColl) && (m_evtIndex < rhs.m_evtIndex))  ||
              ((m_evtColl == rhs.m_evtColl) && (m_evtIndex == rhs.m_evtIndex) && (m_BC < rhs.m_BC)) ) ;
    }
    void print(std::ostream& os) const {
      os << "Event index " << m_evtIndex << ((m_isIndexEventPosition) ? " (position in collection) " : "")
         << ", Barcode " << m_BC
         << ", McEventCollection " << this->getEventCollectionAsString() << "(" << m_evtColl << ")";
    }
    void print(MsgStream& os) const {
      os << "Event index " << m_evtIndex << ((m_isIndexEventPosition) ? " (position in collection) " : "")
         << ", Barcode " << m_BC
         << ", McEventCollection " << this->getEventCollectionAsString() << "(" << m_evtColl << ")";
    }

  private:
    friend class HepMcParticleLinkCnv_p1;
    friend class HepMcParticleLinkCnv_p2;
    friend class HepMcParticleLinkCnv_p3;

    barcode_type m_BC;
    index_type m_evtIndex;
    char m_evtColl;
    bool m_isIndexEventPosition;
  }
#ifdef __GNUC__
    __attribute__ ((packed))
#endif
    ;
  void setExtendedBarcode(ExtendedBarCode extBarcode) { m_extBarcode = extBarcode; m_particle=nullptr; m_have_particle=false; };

private:
  friend class HepMcParticleLinkCnv_p1;
  friend class HepMcParticleLinkCnv_p2;
  friend class HepMcParticleLinkCnv_p3;

  static const std::string s_MAINEVCOLKEYS[4];
  static const std::string s_FIRSTPUEVCOLKEYS[4];
  static const std::string s_SECONDPUEVCOLKEYS[4];
  static const std::string s_THIRDPUEVCOLKEYS[4];
  static std::string s_HOSTKEY_MAINEVCOL;
  static std::string s_HOSTKEY_FIRSTPUEVCOL;
  static std::string s_HOSTKEY_SECONDPUEVCOL;
  static std::string s_HOSTKEY_THIRDPUEVCOL;
  static SG::ReadHandle<McEventCollection>* s_MAP_MCEVENTCOLLECTION_READHANDLE[EBC_NCOLLKINDS];

  // If m_have_particle is true,
  // then m_particle is valid; otherwise, m_dict.  Note that adding
  // m_have_particle doesn't add to the aligned size of this structure
  // due to the trailing short in ExtendedBarCode.  But we have to use
  // the __packed__ attribute on ExtendedBarCode above to prevent
  // it from being padded out.
  mutable HepMC::GenParticle* m_particle;
  ExtendedBarCode m_extBarcode;
  mutable bool m_have_particle; /* transient */

  static std::string * find_hostkey(EBC_EVCOLL evColl);
};

inline
std::ostream&
operator << (std::ostream& os, const HepMcParticleLink& link) {
  link.m_extBarcode.print(os); return os;
}

inline
MsgStream&
operator << (MsgStream& os, const HepMcParticleLink& link) {
  link.m_extBarcode.print(os); return os;
}

#endif
