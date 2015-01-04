/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**                                                                                
   @class BunchDataUtil                                                           
   @brief Utility class to to decode data from FILLPARAMS COOL folder              
   @brief Can be used with FillParamsTool or python FillParamsData                 
   @author E.Torrence                                                              
**/

#ifndef COOLLUMIUTILITIES_BUNCHDATAUTIL_H
#define COOLLUMIUTILITIES_BUNCHDATAUTIL_H

#include <vector>
#include <string>

#include "CoralBase/AttributeList.h"
#include "CoolKernel/Record.h"
#include "CoolLumiUtilities/LumiBlobUtil.h"

class BunchDataUtil {

 public:
  BunchDataUtil();

  int nx1Storage(const cool::Record& rec);
  int ny1Storage(const cool::Record& rec);

  int nx1Storage(const coral::AttributeList& attrList1) const;
  int ny1Storage(const coral::AttributeList& attrList1) const;

  int nx2Storage(const cool::Record& rec);
  int ny2Storage(const cool::Record& rec);

  int nx2Storage(const coral::AttributeList& attrList1) const;
  int ny2Storage(const coral::AttributeList& attrList1) const;

  unsigned int nBunch1Data() const;
  unsigned int nBunch2Data() const;

  const std::vector<unsigned int>& bunch1Data() const;
  const std::vector<unsigned int>& bunch2Data() const;

  // Clear all data vectors                                                         
  void clear();

  // Fill values from COOL, returns false on error                                 
  bool setValue(const coral::AttributeList& attrList1);
  //  bool setValue(const coral::AttributeList& attrList2);
  bool setValue(const cool::Record& rec);

  bool setCollidingBCID1s(std::vector<unsigned int> &);
  bool setCollidingBCID2s(std::vector<unsigned int> &);

  // Error string contains error from setValue() operation                         
  std::string error;

 private:
  // Data                                                                          

  std::vector<unsigned int> m_collidingBCID1s;
  std::vector<unsigned int> m_collidingBCID2s;

  std::vector<unsigned int> m_bunch1Data;
  std::vector<unsigned int> m_bunch2Data;
  class LumiBlobUtil m_blobUtil;

};

#endif
