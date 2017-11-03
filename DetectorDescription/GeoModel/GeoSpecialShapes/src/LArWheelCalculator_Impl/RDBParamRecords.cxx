/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Helper class to simplify parameter reading

#include "RDBParamRecords.h"
#include "RDBParamReader.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

RDBParamRecords::RDBParamRecords(RDBParamReader* rdbParAcc, IRDBRecordset_ptr initRecSet)
  : m_rdbParAcc(rdbParAcc),
    m_RecSet(initRecSet)
{
}

RDBParamRecords::RDBParamRecords(const RDBParamRecords& other)
  : m_rdbParAcc (other.m_rdbParAcc),
    m_RecSet (other.m_RecSet)
{
}


RDBParamRecords::~RDBParamRecords()
{
}

RDBParamRecords::self_t & RDBParamRecords::fallback_to(const std::string& node,
                                                       const std::string& tag,
                                                       const std::string& tag2node)
{
  if(m_RecSet->size() == 0) {
    m_RecSet = m_rdbParAcc->_getRecordsetPtr(node, tag, tag2node);
  }
  return *this;
}

const RDBParamRecords::self_t & RDBParamRecords::param(int &outval,
                                                       const std::string& name,
                                                       unsigned int index) const
{
  outval = (*m_RecSet)[index]->getInt(name);
  return *this;
}

const RDBParamRecords::self_t & RDBParamRecords::param(unsigned int &outval,
                                                       const std::string& name,
                                                       unsigned int index) const
{
  outval = (unsigned int)(*m_RecSet)[index]->getInt(name);
  return *this;
}

const RDBParamRecords::self_t & RDBParamRecords::param(double &outval,
                                                       const std::string& name,
                                                       const double units,
                                                       unsigned int index) const
{
  outval = (*m_RecSet)[index]->getDouble(name)*units;
  return *this;
}

const RDBParamRecords::self_t & RDBParamRecords::param(std::string &outval,
                                                       const std::string& name,
                                                       unsigned int index) const
{
  outval = (*m_RecSet)[index]->getString(name);
  return *this;
}
