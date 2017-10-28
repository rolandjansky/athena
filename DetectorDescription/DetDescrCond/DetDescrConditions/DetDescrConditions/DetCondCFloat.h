/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETCONDCFLOAT_H
#define DETDESCRCOND_DETCONDCFLOAT_H
/**
 * @class DetCondCFloat
 *
 * @brief DetCondCFloat is a class to hold sets of Identifiers and 
 * arrays of floats for detector element specific information
 * Richard Hawkings, started 29/1/04
 * formerly DetCondFloat but renamed to DetCondCFloat to allow DetCondFloat
 * to be a parallel virtual class with no CLASS_DEF
 * Latter is inherited by specialised TRT conditions data classes
 *
 * Based on AlignableTransform but with vectors of floats of arbitrary size 
 * (at initialisation time). These objects are designed to reside in the
 * detector store and be kept in the conditionsDB.
 *
 * Methods are provided to create a new object with a float vector of a 
 * specified size, add an Identifier and associated array of floats,
 * and to associate a new Identifier with some already stored set of floats
 * This sharing mechanism can be used to tranparently associate one set of
 * constants to a large number of identifiers (i.e. detector elements)
 * and then increase the granularity later
 *
 * Access to the data is provided by the find method, which returns a pointer
 * to the floats associated with a particular identifier (or a null pointer
 * if the identifier is not known. The index within the internal storage
 * of a particular identifier's data can also be retrieved, for use in 
 * a subsequent call to the share method (this index is also returned by
 * the add method).
 * print and print2 methods print the data with varying amounts of detail
 */

#include <vector>
#include <map>
#include <algorithm>
#include "Identifier/Identifier.h"

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif

#include "AthenaKernel/CondCont.h"

class DetCondCFloat {
 public:
  DetCondCFloat();
  DetCondCFloat(const int size,const std::string tag);
  virtual ~DetCondCFloat();

  int size() const;
  const std::string& tag() const;

  int add(const Identifier& ident, const float* f);
  void share(const Identifier& ident, const int index);
  const float* find(const Identifier& ident) const;
  int findindex(const Identifier& ident) const;
  void print() const;
  void print2() const;

 private:
  friend class DetCondCFloatCnv_p1;

  int m_size;
  std::string m_tag;
  typedef std::less<Identifier> lessp;
  typedef std::map<Identifier,int,lessp> BufferMap;
  BufferMap m_bufmap;
  std::vector<float> m_buf;
};

CLASS_DEF(DetCondCFloat,247459965,1)
CONDCONT_DEF( DetCondCFloat , 85257013 );

// inline functions for DetCondCFloat

inline DetCondCFloat::DetCondCFloat() : m_size(0), m_tag("null") {}

inline DetCondCFloat::DetCondCFloat(int size,const std::string tag) : 
m_size(size), m_tag(tag) {}

inline DetCondCFloat::~DetCondCFloat() {}

inline int DetCondCFloat::size() const {return m_size;}

inline const std::string& DetCondCFloat::tag() const {return m_tag;}

#endif // DETDESCRCOND_DETCONDCFLOAT_H
