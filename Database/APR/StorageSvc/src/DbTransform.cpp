/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/Guid.h"
#include "StorageSvc/DbTransform.h"
#include "StorageSvc/DbTypeInfo.h"
#include <algorithm>
#include <map>


#include "AthContainers/tools/threading.h"
typedef AthContainers_detail::upgrade_mutex mutex_t;
typedef AthContainers_detail::upgrading_lock<mutex_t> upgrading_lock_t;
typedef SG_STD_OR_BOOST::lock_guard<mutex_t> lock_guard_t;
mutex_t shapesMutex;


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

/// debug function for unit tests
const ShapeVector allShapes() {
   ShapeVector sv;
   for( auto& map_entry : _Init::shapes() ) {
      for( const DbTypeInfo* info : map_entry.second ) {
         sv.push_back(info);
      }
   }
   return sv;
}



/// Access shape registry
DbStatus DbTransform::getShape(const Guid& shape_Guid, const DbTypeInfo*& shape)
{
   upgrading_lock_t lock(shapesMutex); 
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
   upgrading_lock_t lock(shapesMutex); 
   for( auto shape: _Init::shape(shape_Guid) ) {
      if( shape->toString() == rep )
         return shape;
   }
   return 0;
}


/// Register a new shape in the registry
// This method does nothing if one tries to register the same *pointer* - very unlikely after 'new()'
// May result in duplicate identical shapes registered (in MT case) but that should be OK
DbStatus DbTransform::regShape(const DbTypeInfo*  shape)  {
  if ( shape )    {
    lock_guard_t lock(shapesMutex); 
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
     lock_guard_t lock(shapesMutex); 
     _Init::ownedShapes().push_back(shape);
     shape->addRef();
  }
}

/// Access entry in shape registry
DbStatus DbTransform::removeShape (const DbTypeInfo* shape)  {
  if ( shape )    {
    if (_Init::shapes().size() == 0) return Success;
    lock_guard_t lock(shapesMutex); 
    ShapeVector& v = _Init::shape(shape->shapeID());
    ShapeVector::iterator i=find(v.begin(), v.end(), shape);
    if ( i != v.end() ) {
      v.erase(i);
      return Success;
    }
  }
  return Error;
}

