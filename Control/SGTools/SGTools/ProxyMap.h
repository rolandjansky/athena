/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 the DataStore proxy map
 ----------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: ProxyMap.h,v 1.2 2003-04-16 01:49:26 calaf Exp $

#ifndef SGTOOLS_PROXYMAP_H
#define SGTOOLS_PROXYMAP_H

#ifndef _CPP_MAP
 #include <map>
#endif
#ifndef _CPP_STRING
 #include <string>
#endif

namespace SG {
  class DataProxy;

  typedef std::map<std::string, DataProxy*>   ProxyMap;
  typedef ProxyMap::iterator                  ProxyIterator;
  typedef ProxyMap::const_iterator            ConstProxyIterator;
}

#endif // TOOLS_PROXYMAP_H
