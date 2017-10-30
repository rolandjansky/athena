/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __GeoSpecialShapes_LAr_RDBParamReader_H__
#define __GeoSpecialShapes_LAr_RDBParamReader_H__

//#include "RDBAccessSvc/IRDBRecord.h"
//#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class RDBParamRecords;

/// Helper class to simplify parameter reading
/// Fully independent from other parts of package
class RDBParamReader
{
  public:
    /// ???
    typedef RDBParamReader self_t;

    RDBParamReader(IRDBAccessSvc* rdbAccess);
    ~RDBParamReader();

    RDBParamRecords data(const std::string& node, const std::string& tag,
                         const std::string& tag2node="");
    // for internal use only
    IRDBRecordset_ptr _getRecordsetPtr(const std::string& node,
                                       const std::string& tag,
                                       const std::string& tag2node="");
  private:
    IRDBAccessSvc* m_rdbAccess;
};

#endif // __GeoSpecialShapes_LAr_RDBParamReader_H__
