/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/Guid.h"
#include "StorageSvc/DbTransform.h"
#include "StorageSvc/DbTypeInfo.h"
#include <algorithm>
#include <map>

using namespace std;
using namespace pool;

typedef Guid                      GuidKEY;
typedef vector<const DbTypeInfo*> ShapeVector;
typedef map<GuidKEY, ShapeVector> Shapes;

namespace {
  struct _Init {
    Shapes m_map;
    static _Init& instance()  {
      static _Init _inst;
      return _inst;
    }
    _Init()                                    {                                }
    ~_Init()                                   {      shapes().clear();         }
    static Shapes& shapes()                    {      return instance().m_map;  }
    static ShapeVector& shape(const Guid& guid){  return instance().m_map[guid];}
  };
}

/// Access shape registry
DbStatus DbTransform::getShape(const Guid& shape_Guid, const DbTypeInfo*& shape) {
  ShapeVector& v = _Init::shape(shape_Guid);
  if ( v.size() > 0 ) {
    shape = *(v.begin());
    return Success;
  }
  return Error;
}

/// Access entry in shape registry
DbStatus DbTransform::regShape(const DbTypeInfo*  shape)  {
  if ( shape )    {
    ShapeVector& v = _Init::shape(shape->shapeID());
    ShapeVector::iterator i=find(v.begin(), v.end(), shape);
    if ( i == v.end() ) {
      v.push_back(shape);
      return Success;
    }
  }
  return Error;
}

/// Access entry in shape registry
DbStatus DbTransform::removeShape (const DbTypeInfo* shape)  {
  if ( shape )    {
    if (_Init::shapes().size() == 0) return Success;
    ShapeVector& v = _Init::shape(shape->shapeID());
    ShapeVector::iterator i=find(v.begin(), v.end(), shape);
    if ( i != v.end() ) {
      v.erase(i);
      return Success;
    }
  }
  return Error;
}

