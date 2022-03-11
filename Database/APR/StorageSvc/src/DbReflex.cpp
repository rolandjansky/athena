/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbReflex.cpp 717955 2016-01-15 13:34:52Z mnowak $
//====================================================================
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbTransform.h"
#include "POOLCore/DbPrint.h"

#include "TInterpreter.h"

#include <cstring>

#include "AthContainers/tools/threading.h"
typedef AthContainers_detail::upgrade_mutex mutex_t;
typedef AthContainers_detail::upgrading_lock<mutex_t> upgrading_lock_t;
mutex_t guidMapMutex;

/* Lookup table for translating GUIDs into class names.
   Here for convenience to avoid searching through ROOT dictionary information
   which can be slow and produce side effects.
   This must mutch the GUIDs in selection.xml files.
   GUIDs should never change, so it is safe to hardcode them here.
*/
const std::pair<Guid, const char*> GuidToClname[] = {
   {Guid("F41DF744-242D-11E6-B472-02163E010CEC"), "xAOD::TrackParticleAuxContainer_v3"}
  ,{Guid("8251F481-EA4C-4852-AE72-BED87E6FD2FB"), "xAOD::MuonAuxContainer_v4"}
};

using namespace pool;
using namespace std;

void genMD5(const string& s, void* code);

typedef map<Guid,TypeH> GuidMap;
static GuidMap& guid_mapping()  {
  static GuidMap s_map ATLAS_THREAD_SAFE; // protected by guidMapMutex
  return s_map;
}

typedef map<TypeH,Guid> TypeMap;
static TypeMap& type_mapping()  {
  static TypeMap s_map ATLAS_THREAD_SAFE; // protected by guidMapMutex
  return s_map;
}

/// Access to reflection information by type name
const TypeH DbReflex::forTypeName(const string& name)  {
   return TypeH(name);
}

/// Access to reflection information by type info
const TypeH DbReflex::forTypeInfo(const type_info& info)  {
   TypeH t = TypeH(info);
   return (t)? t : forTypeName( DbTypeInfo::typeName(info) );
}

string DbReflex::fullTypeName(const TypeH& typ)  {
  return typ.Name();
}


/// Convert Guid to nomalized string (all char upper case!)
Guid DbReflex::guid(const TypeH& type)
{
   upgrading_lock_t lock(guidMapMutex);
   TypeMap::iterator i = type_mapping().find(type);
   if( i != type_mapping().end() )  {
      return (*i).second;
   }
  string idstr;
  if( type )  {
        idstr = type.Properties().PropertyAsString("id");
        /* 
        if( idstr.empty() ) {
           cout << "!! WARNING, type " << type.Name() << " does not have property ClassID" << endl
                << "===========" << endl;
        }
        cout << "GUID: Class " << type.Name() << " has GUID= " << idstr << endl;
        */
     lock.upgrade();
     if( !idstr.empty() )  {
        Guid id(idstr);
        guid_mapping()[id] = type;
        type_mapping()[type] = id;
        return id;
     }
     else  {
        Guid id;
        // fill GUID with 16byte MD5 binary checksum digest of the typename
        genMD5(fullTypeName(type), &id);
        guid_mapping()[id] = type;
        type_mapping()[type] = id;
        return id;
     }
  }
  return Guid::null();
}


/// Access to reflection information by Guid
const TypeH DbReflex::forGuid(const Guid& id)
{
   {
      upgrading_lock_t lock(guidMapMutex);
      // First check if the mapping is already known 
      GuidMap& m = guid_mapping();
      GuidMap::iterator k = m.find(id);
      if ( k != m.end() )  {
         return (*k).second;
      }
   }
   
  // Check the transformation map
  const DbTypeInfo* info = 0;
  if ( DbTransform::getShape(id, info).isSuccess() )  {
     TypeH t = info->clazz();
     if ( t )  {
        Guid g = guid(t);
        if ( ::memcmp(&g, &id, sizeof(Guid))==0 )  {           
           return t;
        }
     }
  }

  DbPrint log("APR:DbReflex:forGuid");

  for( auto& el : GuidToClname ) {
     if( id==el.first ) {
        // we have this guid in the hardcoded mapping
        const TypeH typ( el.second );
        if( typ ) {
           Guid g( guid( typ ) );  // call this to update guid->type maps
           if( g != id ) {
              // inconsistency with XML and hardcoded GUID??
              log << DbPrintLvl::Error << "GUID query for " << id << " found GUID missmatch! "
                  << el.second << " -> " << g << DbPrint::endmsg;
           }
        }
        return typ;
     }
  }
  
  static std::mutex guidScanMutex;
  std::lock_guard<std::mutex> lock (guidScanMutex);

  // GUID not in the map: scan all known types. refresh the map
  log << DbPrintLvl::Warning << " doing GUID scan on ALL types for Class ID=" << id << DbPrint::endmsg;

  // disable TClass autoloading/parsing, restore settings on return
  class AutoloadGuard {
   public:
    int al = 0, ap = 0;
    AutoloadGuard() {
       al = gInterpreter->SetClassAutoloading( 0 );
       ap = gInterpreter->SetClassAutoparsing( 0 );
    }
    ~AutoloadGuard() {
       gInterpreter->SetClassAutoloading( al );
       gInterpreter->SetClassAutoparsing( ap );
    }
  } ALG;

  size_t sz = TypeH::TypeSize();
  for(size_t i=0; i<sz; ++i)  {
     TypeH t = TypeH::TypeAt(i); // This may change/increase TypeH::TypeSize(), can't optimize
     size_t sz_new = TypeH::TypeSize();
     if (sz_new > sz) {
        log << DbPrintLvl::Debug << " ROOT gClassTable size increase for " << t.Name() << DbPrint::endmsg;
        sz = sz_new;
     }
     if( t.IsClass() || t.IsStruct() )  {
        Guid g = guid(t);
        if( ::memcmp(&g, &id, sizeof(Guid))==0 )  {
           log << DbPrintLvl::Debug << "Resolved class ID " << id << " to " << t.Name() << DbPrint::endmsg;
           return t;
        }
     }
  }

  log << DbPrintLvl::Warning << "Type lookup for class ID " << id << " failed" << DbPrint::endmsg; 
  return TypeH();
}
