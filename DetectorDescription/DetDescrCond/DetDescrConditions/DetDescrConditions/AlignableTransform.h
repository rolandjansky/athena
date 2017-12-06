/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_ALIGNTRANS_H
#define DETDESCRCOND_ALIGNTRANS_H
// AlignableTransform.h - simple class to hold Identifier and transform for
//                        alignment information
// Richard Hawkings, started 23/9/03
//
// AlignableTransform consists of a vector of AlignTransMember objects,
// each of which is contains an Identifier and a HepGeom::Transform3D
// It is designed to reside in the detector store and be kept in the 
// conditionsDB. The AlignTransTool AlgTool (in DetDescrTools) provides
// a user interface to manage these objects

#include <vector>
#include <algorithm>
#include "Identifier/Identifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "boost/range/iterator_range.hpp"

class AlignableTransform {
 public:
  AlignableTransform();
  AlignableTransform(std::string tag);
  virtual ~AlignableTransform();

  // embedded class AlignTransMember holds the information for one module
  // i.e. module identifier and associated transform
  class AlignTransMember {
  public:
    AlignTransMember();
    AlignTransMember(const Identifier& ident, const HepGeom::Transform3D& trans);
    const Identifier& identify() const;
    const HepGeom::Transform3D& transform() const;
    void setTransform(const HepGeom::Transform3D& trans);

  private:
    Identifier m_ident;
    HepGeom::Transform3D m_trans;
  };

  // const and non-const iterator definitions
  typedef std::vector<AlignTransMember>::const_iterator AlignTransMem_citr;
  typedef boost::iterator_range<AlignTransMem_citr> AlignTransMem_crange;
  AlignTransMem_citr begin() const;
  AlignTransMem_citr end() const;
  AlignTransMem_crange range() const;
  typedef std::vector<AlignTransMember>::iterator AlignTransMem_itr;
  typedef boost::iterator_range<AlignTransMem_itr> AlignTransMem_range;
  AlignTransMem_itr mbegin();
  AlignTransMem_itr mend();
  AlignTransMem_range mrange();
  // return pointer to particular module
  AlignTransMem_citr findIdent(const Identifier& ident) const;
  AlignTransMem_itr findIdent(const Identifier& ident);
  // return tag name for AlignableTransform
  const std::string& tag() const;
  size_t size() const;

  // add a new member to the AlignableTransform
  void add(const Identifier& ident,const HepGeom::Transform3D& trans);
  // reset the specified transform
  bool update(const Identifier& ident,const HepGeom::Transform3D& trans);
  // tweak, i.e. combine new transform with that already existing
  bool tweak(const Identifier& ident,const HepGeom::Transform3D& trans);
  void print() const;
  void print2() const;
  // sort transform vector, to be called after adding new transforms
  void sortv();

 private:
  friend class AlignableTransformCnv_p1;
  std::string m_tag;
  std::vector<AlignTransMember> m_vec;
};

CLASS_DEF(AlignableTransform,158187411,1)

inline const Identifier& 
AlignableTransform::AlignTransMember::identify() const
{ return m_ident;}

inline const HepGeom::Transform3D& 
AlignableTransform::AlignTransMember::transform() const
{ return m_trans;}

inline void AlignableTransform::AlignTransMember::setTransform(
   const HepGeom::Transform3D& trans) { m_trans=trans;}

inline bool operator< (const AlignableTransform::AlignTransMember& lhs, 
                const AlignableTransform::AlignTransMember& rhs) {
  return (lhs.identify() < rhs.identify());
}

inline AlignableTransform::AlignableTransform() { m_tag="null"; }
inline AlignableTransform::~AlignableTransform() {}

inline AlignableTransform::AlignTransMem_citr
   AlignableTransform::begin() const { return m_vec.begin();}
inline AlignableTransform::AlignTransMem_citr
   AlignableTransform::end() const { return m_vec.end();}
inline AlignableTransform::AlignTransMem_crange
   AlignableTransform::range() const { return AlignTransMem_crange(begin(), end()); }

inline AlignableTransform::AlignTransMem_itr
   AlignableTransform::mbegin() { return m_vec.begin();}
inline AlignableTransform::AlignTransMem_itr
   AlignableTransform::mend() { return m_vec.end();}
inline AlignableTransform::AlignTransMem_range
   AlignableTransform::mrange() { return AlignTransMem_range(mbegin(), mend()); }

inline AlignableTransform::AlignTransMem_citr
  AlignableTransform::findIdent(const Identifier& ident) const {
  AlignableTransform::AlignTransMember obj(ident,HepGeom::Transform3D());
  AlignableTransform::AlignTransMem_citr rc=std::lower_bound(m_vec.begin(),m_vec.end(),obj);
  if (rc!=m_vec.end() && rc->identify()!=ident) rc=m_vec.end();
  return rc;
}

// same as previous but non-const
inline AlignableTransform::AlignTransMem_itr
  AlignableTransform::findIdent(const Identifier& ident) {
  AlignableTransform::AlignTransMember obj(ident,HepGeom::Transform3D());
  AlignableTransform::AlignTransMem_itr rc=std::lower_bound(m_vec.begin(),m_vec.end(),obj);
  if (rc!=m_vec.end() && rc->identify()!=ident) rc=m_vec.end();
  return rc;
}

inline const std::string& AlignableTransform::tag() const {return m_tag;}

inline size_t AlignableTransform::size() const {return m_vec.size();}

inline void AlignableTransform::sortv() { 
   sort(m_vec.begin(),m_vec.end()); }

#endif // DETDESCRCOND_ALIGNTRANS_H
