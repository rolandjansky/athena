/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __GeoSpecialShapes_LAr_RDBParamRecords_H__
#define __GeoSpecialShapes_LAr_RDBParamRecords_H__

#include "RDBAccessSvc/IRDBAccessSvc.h"

class RDBParamReader;

/// @class RDBParamRecords
/// Helper class to simplify parameter reading
/// Fully independent from other parts of package
class RDBParamRecords
{
  public:
    // ???
    typedef RDBParamRecords self_t;

    RDBParamRecords(RDBParamReader* rdbParAcc, IRDBRecordset_ptr initRecSet);
    RDBParamRecords(const RDBParamRecords& other);
    RDBParamRecords& operator=(const RDBParamRecords& other) = delete;
    ~RDBParamRecords();
    self_t & fallback_to(const std::string& node, const std::string& tag, const std::string& tag2node="");
    const self_t & param(unsigned int &outval,    const std::string& name, unsigned int index=0) const;
    const self_t & param(int &outval,             const std::string& name, unsigned int index=0) const;
    const self_t & param(double &outval,     const std::string& name, const double units=1.0, unsigned int index=0) const;
    const self_t & param(std::string &outval, const std::string& name, unsigned int index=0) const;

  private:
    RDBParamReader* m_rdbParAcc;
    IRDBRecordset_ptr m_RecSet;
};

#endif // __GeoSpecialShapes_LAr_RDBParamRecords_H__
