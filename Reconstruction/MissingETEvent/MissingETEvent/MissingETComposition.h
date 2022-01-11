// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETCOMPOSITION_H
#define MISSINGETEVENT_MISSINGETCOMPOSITION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "Navigation/Navigable.h"

#include "NavFourMom/INavigable4MomentumCollection.h"
#include "MissingETEvent/MissingET.h"

#include <vector>
#include <boost/tuple/tuple.hpp>

struct MissingETtags {

  typedef unsigned short bitmask_t;

  enum Tags {
    UNKNOWN                 = 0x0000,
    DEFAULT                 = 0x0001,
    SPECTRO                 = 0x0002,
    TRACK                   = 0x0004,
    REFMUON                 = 0x0008,
    MUID                    = 0x0010,
    EFLOW_CLUSTER           = 0x0020,
    REMOVED_CLUSTER         = 0x0040,
    VTX_SHIFTED_CLUSTER_DEF = 0x0081, // shifted default
    VTX_SHIFTED_CLUSTER     = 0x0080, // just shift indicator
    PILEUP_CORRECTED    = 0x1000,
    PU_TRACK_STVF       = 0x1100,
    PU_TRACK_SUM        = 0x1200,
    PU_TRACK_CONE       = 0x1400,
    PU_JET_AREA         = 0x1800,
    PU_JET_CORRECTION   = 0x3000,
    PU_JET_JVF          = 0x3100,
    PU_JET_CUSTOM_JVF   = 0x3200,
    PU_JET_JVFCUT       = 0x3300,
    PU_JET_AREA_JET     = 0x3800,
    PU_JET_AREA_JVF     = 0x5100
    //NEXT_COMES      = 0x0080,
    //THEN            = 0x0100,
  };

  // general test for tag pattern
  static bool usesTags(bitmask_t tag,bitmask_t test)
  { return (tag & test) == test; }
 
  //isDEFAULT means use default pt and no other bits are set
  static bool isDEFAULT(bitmask_t tag)
  { return tag == DEFAULT || tag == VTX_SHIFTED_CLUSTER_DEF; }

  //usesDEFAULT means use default pt but other bits could also be set
  static bool usesDEFAULT(bitmask_t tag)
  { return tag & DEFAULT; }
  
  static bool usesSPECTRO(bitmask_t tag)
  { return tag & SPECTRO; }

  static bool usesTRACK(bitmask_t tag)
  { return tag & TRACK; }

  static bool usesREFMUON(bitmask_t tag)
  { return tag & REFMUON; }

  static bool usesMUID(bitmask_t tag)
  { return tag & MUID; }

  static bool usesEFLOW_CLUSTER(bitmask_t tag)
  { return tag & EFLOW_CLUSTER; }

  static bool usesREMOVED_CLUSTER(bitmask_t tag)
  { return tag & REMOVED_CLUSTER; }

  static bool usesPILEUP_CORRECTED(bitmask_t tag)
  { return tag & PILEUP_CORRECTED; }

  static bool usesPU_TRACK_STVF(bitmask_t tag)
  { return tag & PU_TRACK_STVF; }

  static bool usesPU_TRACK_SUM(bitmask_t tag)
  { return tag & PU_TRACK_SUM; }

  static bool usesPU_TRACK_CONE(bitmask_t tag)
  { return tag & PU_TRACK_CONE; }

  static bool usesPU_JET_AREA(bitmask_t tag)
  { return tag & PU_JET_AREA; }

  static bool usesPU_JET_CORRECTION(bitmask_t tag)
  { return tag & PU_JET_CORRECTION; }

  static bool usesPU_JET_JVF(bitmask_t tag)
  { return tag & PU_JET_JVF; }
  
  static bool usesPU_JET_JVFCUT(bitmask_t tag)
  { return tag & PU_JET_JVFCUT; }
  
  static bool usesPU_JET_CUSTOM_JVF(bitmask_t tag)
  { return tag & PU_JET_CUSTOM_JVF; }

  static bool usesPU_JET_AREA_JET(bitmask_t tag)
  { return tag & PU_JET_AREA_JET; }

  static bool usesPU_JET_AREA_JVF(bitmask_t tag)
  { return tag & PU_JET_AREA_JVF; }

  static bool usesVTX_SHIFTED_CLUSTER(bitmask_t tag)
  { return tag & VTX_SHIFTED_CLUSTER; }

  static bool usesVTX_SHIFTED_CLUSTER_DEF(bitmask_t tag)
  { return tag & VTX_SHIFTED_CLUSTER_DEF; }

  static void setUsesDEFAULT(bitmask_t &tag)
  { tag = tag | DEFAULT;}

  static void setUsesSPECTRO(bitmask_t &tag)
  { tag = tag | SPECTRO;}

  static void setUsesTRACK(bitmask_t &tag)
  { tag = tag | TRACK;}

  static void setUsesREFMUON(bitmask_t &tag)
  { tag = tag | REFMUON;}

  static void setUsesMUID(bitmask_t &tag)
  { tag = tag | MUID;}

  static void setUsesEFLOW_CLUSTER(bitmask_t &tag)
  { tag = tag | EFLOW_CLUSTER;}

  static void setUsesREMOVED_CLUSTER(bitmask_t &tag)
  { tag = tag | REMOVED_CLUSTER;}

  static void setUsesPILEUP_CORRECTED(bitmask_t &tag)
  { tag = tag | PILEUP_CORRECTED;}

  static void setUsesPU_TRACK_STVF(bitmask_t &tag)
  { tag = tag | PU_TRACK_STVF;}

  static void setUsesPU_TRACK_SUM(bitmask_t &tag)
  { tag = tag | PU_TRACK_SUM;}

  static void setUsesPU_TRACK_CONE(bitmask_t &tag)
  { tag = tag | PU_TRACK_CONE;}

  static void setUsesPU_JET_AREA(bitmask_t &tag)
  { tag = tag | PU_JET_AREA;}

  static void setUsesPU_JET_CORRECTION(bitmask_t &tag)
  { tag = tag | PU_JET_CORRECTION; }

  static void setUsesPU_JET_JVF(bitmask_t &tag)
  { tag = tag | PU_JET_JVF; }

  static void setUsesPU_JET_JVFCUT(bitmask_t &tag)
  { tag = tag | PU_JET_JVFCUT; }

  static void setUsesPU_JET_CUSTOM_JVF(bitmask_t& tag)
  { tag = tag | PU_JET_CUSTOM_JVF; }

  static void setUsesPU_JET_AREA_JET(bitmask_t& tag)
  { tag = tag | PU_JET_AREA_JET; }

  static void setUsesPU_JET_AREA_JVF(bitmask_t& tag)
  { tag = tag | PU_JET_AREA_JVF; }

  static void setUsesVTX_SHIFTED_CLUSTER(bitmask_t& tag)
  { tag = tag | VTX_SHIFTED_CLUSTER; }

  static void setUsesVTX_SHIFTED_CLUSTER_DEF(bitmask_t& tag)
  { tag = tag | VTX_SHIFTED_CLUSTER_DEF; }
};

namespace MissingETHandlers {

  typedef boost::tuples::tuple<float,float,float,unsigned short int> struct_t;

  static float precision() { return 0.0001; }

  class Weight : public struct_t
  {
  public:
  // templated constructor
  template <typename PERS>
    /* explicit */ Weight(PERS&)
    {
      this->setWpx(0);
      this->setWpy(0);
      this->setWet(0);
      this->setStatusWord(0);
      std::cerr << "Rolf: calling general template for Weight c'tor ! You need to specialize it for " << typeid(PERS).name() << " !\n";
      abort();
    }

    Weight() : struct_t() { }
    Weight(float wpx,float wpy,float wet) : struct_t(wpx,wpy,wet,0) { }
    Weight(float wpx,float wpy,float wet, unsigned short int status) : struct_t(wpx,wpy,wet,status) { }
    virtual ~Weight() { }
    
    Weight(const Weight& w) : struct_t(w.wpx(),w.wpy(),w.wet(),w.statusWord()) { }
    Weight(Weight& w) : struct_t(w.wpx(),w.wpy(),w.wet(),w.statusWord()) { }

    float wpx() const { return (float)this->get<0>(); }
    float wpy() const { return (float)this->get<1>(); }
    float wet() const { return (float)this->get<2>(); }
    unsigned short int statusWord() const { return (unsigned short int)this->get<3>(); }

    void setWpx(float wpx) { this->get<0>() = wpx; }
    void setWpy(float wpy) { this->get<1>() = wpy; }
    void setWet(float wet) { this->get<2>() = wet; }
    void setStatusWord(unsigned short int status) { this->get<3>() = status; }

    Weight& operator+=(const Weight& w)
    {
      this->setWpx(w.wpx()+this->wpx());
      this->setWpy(w.wpy()+this->wpy());
      this->setWet(w.wet()+this->wet());
      return *this;
    }

    Weight& operator-=(const Weight& w)
    {
      this->setWpx(this->wpx()-w.wpx());
      this->setWpy(this->wpy()-w.wpy());
      this->setWet(this->wet()-w.wet());
      return *this;
    }

    Weight& operator*=(const Weight& w)
    {
      this->setWpx(w.wpx()*this->wpx());
      this->setWpy(w.wpy()*this->wpy());
      this->setWet(w.wet()*this->wet());
      return *this;
    }

    Weight& operator/=(const Weight& w)
    {
      this->setWpx(this->divide(this->wpx(),w.wpx()));
      this->setWpy(this->divide(this->wpy(),w.wpy()));
      this->setWet(this->divide(this->wet(),w.wet()));
      return *this;
    }

    bool operator==(const Weight& w) const
    { 
      return 
	isEqual(w.wpx(),this->wpx()) && 
	isEqual(w.wpy(),this->wpy()) &&
	isEqual(w.wet(),this->wet()); 
    }

    bool operator!=(const Weight& w) const
    { return !(this->operator==)(w); }

    Weight& operator=(const Weight& w)
    {
      this->setWpx(w.wpx());
      this->setWpy(w.wpy());
      this->setWet(w.wet());
      this->setStatusWord(w.statusWord());
      return *this;
    }

    static float divide(float x,float y)
    { return y != 0. ? x /y : 0.; }

    static bool isEqual(float x,float y)
    { return fabs(x-y) <= precision()*fabs(x); }

  };//class Weight


Weight operator*(const Weight& w1, 
                 const Weight& w2);
Weight operator*(const Weight& w, double scale);
Weight operator*(double scale, const Weight& w);
Weight operator/(const Weight& w1, 
                 const Weight& w2);
Weight operator+(const Weight& w1, 
                 const Weight& w2);
Weight operator-(const Weight& w1, 
                 const Weight& w2);


}//namespace MissingETHandlers



class MissingETComposition : 
  public Navigable<INavigable4MomentumCollection,MissingETHandlers::Weight>
{
public:

  typedef INavigable4MomentumCollection    collection_t;
  typedef collection_t::base_value_type    object_t;
  typedef MissingETHandlers::Weight            weight_t;
  typedef Navigable<collection_t,weight_t> base_t;
  typedef base_t::object_iter              iiterator_t;

  // iterators
  template<class OBJECT>
  class const_iterator 
  {
  public:
    const_iterator() { };
    const_iterator(const base_t* store,iiterator_t iiter) 
      : m_store(store), m_actual(iiter) { }
    virtual ~const_iterator() { }
    // copy
    const_iterator(const const_iterator& iter) 
      : m_store(iter.m_store), m_actual(iter.m_actual) { }

    const_iterator operator=(const const_iterator& iter) {
      if(&iter != this) { 
        m_store = iter.m_store;
        m_actual = iter.m_actual;
      }
      return *this;
    }

    // increment
    const_iterator operator++()
    {
      m_actual++;
      while ( m_actual != m_store->end() && 
	      dynamic_cast<const OBJECT*>(*m_actual) == 0 ) { m_actual++; };
      return *this;
    }
    const_iterator operator++(int)
    {
      const_iterator tmp(*this);
      m_actual++;
      while ( m_actual != m_store->end() &&
	      dynamic_cast<const OBJECT*>(*m_actual) == 0 ) { m_actual++; };
      return tmp;
    }

    // decrement
    const_iterator operator--()
    {
      m_actual--;
      while ( m_actual != m_store->begin() && 
	      dynamic_cast<const OBJECT*>(*m_actual) == 0 ) { m_actual--; };
      return m_actual != m_store->begin() ? *this : 0;
    }
    const_iterator operator--(int)
    {
      const_iterator tmp(*this);
      m_actual--;
      while ( m_actual != m_store->end() &&
	      dynamic_cast<const OBJECT*>(*m_actual) == 0 ) { m_actual--; };
      return tmp;
    }

    // equality
    bool operator==(const const_iterator& iter) const 
    { return m_store == iter.m_store && m_actual == iter.m_actual; }
    bool operator!=(const const_iterator& iter) const
    { return !(this->operator==)(iter); }

    // access
    const OBJECT* operator*() const
    {
      return dynamic_cast<const OBJECT*>(*m_actual);
    }
    const OBJECT*& operator->() const
    {
      if (dynamic_cast<const OBJECT*>(*m_actual)==0) {
	std::cout << "dynamic cast==0 in operator*" << std::endl;
      }

      return dynamic_cast<const OBJECT*>(*m_actual);
    }
    
    const OBJECT*   getObject() const { return this->operator*(); }
    const weight_t getParameter() const { return m_actual.getParameter(); }
    // helpful for copying compositions
    const INavigable4MomentumCollection* getContainer() const 
      { return m_store->getContainer(m_actual); }

  private:
    
    const base_t* m_store;
    iiterator_t   m_actual;
  };

  MissingETComposition();
  virtual ~MissingETComposition();

  void insert(const collection_t* pColl,const object_t* pObj,
	      weight_t weight)
  {
    this->insertElement(pColl,pObj,weight);
  }

  weight_t getWeight(const object_t* pObj) const
  {
    return this->getParameter(pObj);
  }

  template<class OBJECT>
  weight_t getWeight(const_iterator<OBJECT> fObj) const
  {
    return fObj.getParameter();
  }

  template<class OBJECT>
  const_iterator<OBJECT> beginIterator() const
  {
    if (this->begin() != this->end()) {
      const_iterator<OBJECT> temp(static_cast<const base_t*>(this), this->begin()); 
      if ((*temp)==0) {++temp; return temp;}
      return temp;
    }
    else { return const_iterator<OBJECT>(static_cast<const base_t*>(this), this->begin()); }
  }

  template<class OBJECT>
  const_iterator<OBJECT> endIterator() const
  {
    return const_iterator<OBJECT>(static_cast<const base_t*>(this), this->end()); 
  }

  template<class OBJECT>
  const_iterator<OBJECT> find(const OBJECT* pObj) const
  {
    const_iterator<OBJECT> fObj = this->beginIterator<OBJECT>();
    const_iterator<OBJECT> lObj = this->endIterator<OBJECT>();
    while ( fObj != lObj && pObj != *fObj ) { ++fObj; }
    return fObj;
  }

  // fill missing ET object - really adds!!
  template<class OBJECT>
    bool fillMissingET(MissingET* pMET,
		       unsigned short rejectTag=(MissingETtags::PILEUP_CORRECTED | 
					       MissingETtags::REMOVED_CLUSTER) ) const
  {
    // sanity check
    if ( pMET == 0 ) return false;
    // sum up
    float px(0.);
    float py(0.);
    float et(0.);
    float eta(0.);
    float e(0.);
    const_iterator<OBJECT> fObj(this->beginIterator<OBJECT>());
    const_iterator<OBJECT> lObj(this->endIterator<OBJECT>());
    for ( ; fObj != lObj; ++fObj )
      {
	if ( !MissingETtags::usesTags(fObj.statusWord(),rejectTag) )
	  {
	    px += (*fObj)->px() * fObj.getParameter().wpx(); 
	    py += (*fObj)->py() * fObj.getParameter().wpy(); 
	    et += (*fObj)->et() * fObj.getParameter().wet();
	    // eta
	    eta += (*fObj)->eta() * (*fObj)->e() * fObj.getParameter().wet(); 
	    e   += (*fObj)->e() * fObj.getParameter().wet();
	  } 
      }
    // get eta
    eta = e != 0. ? eta/e : 0.;
    //
    pMET->add(px,py,et,eta,1.0);
    return true;
  }

  // get contributions from given object
  template<class OBJECT>
    MissingET* getMissingET(unsigned short rejectTag=
			    (MissingETtags::PILEUP_CORRECTED | MissingETtags::REMOVED_CLUSTER) ) 
    const
  {
    //
    MissingET* pMET = new MissingET(MissingET::Ref);
    if ( this->fillMissingET<OBJECT>(pMET,rejectTag) ) return pMET;
    // error raised
    if ( pMET != 0 ) delete pMET;
    return (MissingET*)0;
  }

};

CLASS_DEF(MissingETComposition, 264867051, 1)

#endif
