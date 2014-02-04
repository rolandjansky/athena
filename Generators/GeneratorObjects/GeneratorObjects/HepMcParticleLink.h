/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include <limits>  /* numeric_limits */
#include <string>

namespace HepMC {
  class GenParticle;
  class GenEvent;
}
class IProxyDict;

class HepMcParticleLink {
public:  
  typedef unsigned int index_type;

  /// \name structors
  //@{
  HepMcParticleLink() { init_dict(); }
  HepMcParticleLink(index_type barCode, index_type eventIndex = 0) :
    m_extBarcode(barCode, eventIndex) { init_dict(); }
  HepMcParticleLink(const HepMC::GenParticle* p, index_type eventIndex = 0);
  HepMcParticleLink(const HepMcParticleLink& rhs) : 
    m_ptrs(rhs.m_ptrs),
    m_extBarcode(rhs.m_extBarcode),
    m_have_particle (rhs.m_have_particle){  }

  HepMcParticleLink& operator= (const HepMcParticleLink& rhs)
  {
    if (this != &rhs) {
      m_ptrs = rhs.m_ptrs;
      m_extBarcode = rhs.m_extBarcode;
      m_have_particle = rhs.m_have_particle;
    }
    return *this;
  }
////  HepMcParticleLink(const HepMC::GenParticle* part, 
////		    const HepMC::GenEvent* pevt); //FIXME NOT YET 
  //@}
  
  /// \name pointer interface
  //@{
  /// @throws std::runtime_error when the element is not found
  const HepMC::GenParticle& operator* () const { return *cptr(); }//FIXME
  const HepMC::GenParticle* operator->() const { return cptr(); }
  operator const HepMC::GenParticle* () const { return cptr(); }
  bool operator!() const {return !isValid();}
  bool operator == (const HepMcParticleLink& rhs) const { 
    return ( m_extBarcode== rhs.m_extBarcode ); }
  bool operator != (const HepMcParticleLink& rhs) const { 
    return !(operator==(rhs)); }
  bool operator < (const HepMcParticleLink& rhs) const {
    return ( m_extBarcode < rhs.m_extBarcode ); }
  friend std::ostream& operator << (std::ostream&, const HepMcParticleLink&);
  friend MsgStream& operator << (MsgStream&, const HepMcParticleLink&);

  //@}

  /// \name indexing accessors (e.g. for writing)
  //@{
  int barcode() const { return int(m_extBarcode.barcode()); } //FIXME ret type
  index_type eventIndex() const { return index_type(m_extBarcode.eventIndex()); }
  long compress() const { return (this->barcode() << 16) + this->eventIndex(); };
  //@}
  
  static const std::string DEFAULTKEY;
  static const std::string DC2DEFAULTKEY;
  static const std::string AODKEY;

  bool isValid() const { return (0 != cptr()); }
  const HepMC::GenParticle* cptr() const;

  class ExtendedBarCode {
  public:
    ExtendedBarCode() : m_BC(0), m_evtIndex(0) {}
    ExtendedBarCode(index_type barcode, index_type eventIndex) :
      m_BC(barcode), m_evtIndex(eventIndex)
    {
      assert(eventIndex < std::numeric_limits<unsigned short>::max()); 
    }

    unsigned long barcode() const { return m_BC; }
    unsigned short eventIndex() const { return m_evtIndex; }
    bool operator == (const ExtendedBarCode& rhs ) const
    { 
      return ( (this->m_BC)==(rhs.m_BC) && (this->m_evtIndex)==(rhs.m_evtIndex) ) ;
    }
    bool operator != (const ExtendedBarCode& rhs ) const {
      return !(operator==(rhs));
    }
    bool operator < (const ExtendedBarCode& rhs ) const
    { 
      return ((m_evtIndex < rhs.m_evtIndex)  ||
	      ((m_evtIndex == rhs.m_evtIndex) && (m_BC < rhs.m_BC)) ) ;
    }
    void print(std::ostream& os) const {
      os << "Event index " << m_evtIndex
	 << ", Barcode " << m_BC;
    }
    void print(MsgStream& os) const {
      os << "Event index " << m_evtIndex
	 << ", Barcode " << m_BC;
    }
    
  private:
    unsigned long m_BC;
    unsigned short m_evtIndex;
  }
#ifdef __GNUC__
  __attribute__ ((packed))
#endif
;


  // Reset the static cached dictionary pointer.
  // Sometimes we can get the wrong one if these objects get constructed
  // too early during program initialization, such as when reading
  // dictionaries.
  static void resetCachedDictionary();

 private:
  static std::string HOSTKEY;
  static IProxyDict** s_defsource_ptr;

  // We need to remember the default data source when we're constructed.
  // But once we have the particle pointer, we don't need it any more.
  // Thus, we can put them in a union.  If m_have_particle is true,
  // then m_particle is valid; otherwise, m_dict.  Note that adding
  // m_have_particle doesn't add to the aligned size of this structure
  // due to the trailing short in ExtendedBarCode.  But we have to use
  // the __packed__ attribute on ExtendedBarCode above to prevent
  // it from being padded out.
  mutable union {
    HepMC::GenParticle* m_particle;
    IProxyDict* m_dict;
  } m_ptrs; /* transient */
  ExtendedBarCode m_extBarcode;
  mutable bool m_have_particle; /* transient */

  void init_dict();
  bool find_hostkey() const;
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
