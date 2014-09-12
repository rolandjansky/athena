/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGESVC_DBSECTION_H
#define STORAGESVC_DBSECTION_H


namespace pool {
  class DbSection {
  public:
    long long int offset;
    long long int start;
    long long int length;
    DbSection() : offset(0LL), start(0LL), length(0LL) {}
    DbSection(long long int o, long long int s, long long int l) : offset(o), start(s), length(l) {}
  };
}

#endif // STORAGESVC_DBSECTION_H
