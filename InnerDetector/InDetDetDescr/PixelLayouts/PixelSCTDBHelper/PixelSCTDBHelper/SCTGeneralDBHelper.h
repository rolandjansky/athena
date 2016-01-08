/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTModule_SCTGeneralDBHelper_H
#define SCTModule_SCTGeneralDBHelper_H

#include <string>

#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"


  class SCTGeneralDBHelper {
    
  public:
    
    // Constructor 
    SCTGeneralDBHelper(const OraclePixGeoAccessor* geoAccessor);
    
    // General
    double    safety() const;

private:
  
  const OraclePixGeoAccessor* m_geoAccessor;

  IRDBRecordset_ptr   generalTable;

};

#endif // SCT_GeoModel_SCTGeneralDBHelper_H
