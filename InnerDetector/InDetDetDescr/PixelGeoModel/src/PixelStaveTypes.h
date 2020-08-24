/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelStaveTypes_H
#define PixelStaveTypes_H

// Class to interpret and query PixelStaveType table

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IGeometryDBSvc;
#include <map>

class PixelStaveTypes {

public :
  PixelStaveTypes(const IGeometryDBSvc * db, IRDBRecordset_ptr table);
  int getFluidType(int layer, int phiModule) const;
  int getBiStaveType(int layer, int phiModule) const;

private :
  class Key 
  {
  public:
    Key(int layer_in, int phiModule_in);
    int layer;
    int phiModule;
    bool operator<(const Key &rhs) const;
  };

  class Datum
  {
  public:
    Datum(int fluidType_in = 0, int biStaveType_in = 0);
    int fluidType;
    int biStaveType;
  };

  const Datum & getData(int layer, int phiModule) const;
  
  typedef std::map<Key, Datum> MapType;
  MapType m_dataLookup;

  std::map<int,int> m_maxSector;
  
  static const Datum s_defaultDatum;

};

#endif // PixelStaveTypes_H
