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

typedef vector<const DbTypeInfo*> ShapeVector;
typedef map<Guid, ShapeVector>    Shapes;

namespace {
  struct _Init {
    Shapes      m_map;
    ShapeVector m_ownedShapes;
    
    static _Init& instance()  {
      static _Init _inst;
      return _inst;
    }
    _Init() {}
    ~_Init() {
       for( const DbTypeInfo* info : ownedShapes() ) {
          info->deleteRef();
       }
       ownedShapes().clear();
       shapes().clear();
    }
    static Shapes&      shapes()                     {  return instance().m_map;  }
    static ShapeVector& shape(const Guid& guid)      {  return instance().m_map[guid]; }
    static ShapeVector& ownedShapes()                {  return instance().m_ownedShapes; }
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


/// Access shape registry
const DbTypeInfo* DbTransform::getShape(const Guid& shape_Guid, const std::string& rep)
{
  for( auto shape: _Init::shape(shape_Guid) ) {
     if( shape->toString() == rep )
        return shape;
  }
  return 0;
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

/// keep shape until the end
void DbTransform::ownShape(const DbTypeInfo*  shape)  {
  if ( shape ) {
     _Init::ownedShapes().push_back(shape);
     shape->addRef();
  }
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

