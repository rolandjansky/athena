/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READMETAHANDLEKEY_H
#define STOREGATE_READMETAHANDLEKEY_H

#include "StoreGate/MetaHandleKey.h"
#include <string>

namespace SG {

  template <class T> class ReadMetaHandle;
  
  template <class T> class ReadMetaHandleKey
    : public MetaHandleKey<T>
    {      
    public:
      friend class ReadMetaHandle<T>;
        
      ReadMetaHandleKey (const std::string& key
			 , const std::string& dbKey="")
	: MetaHandleKey<T>(key, dbKey, Gaudi::DataHandle::Reader)
	{}   

      template <
	class OWNER
	, class K
	, typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
	inline ReadMetaHandleKey( OWNER* owner
				  , std::string name
				  , const K& key={}
				  , std::string doc="") 
	: ReadMetaHandleKey<T>( key ) {
	auto p = owner->declareProperty(std::move(name), *this, std::move(doc));
	p->template setOwnerType<OWNER>();
      }

    };

} // namespace SG

#endif // not STOREGATE_READMETAHANDLEKEY_H
