/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**                                                                                                      
   @class BunchLumisUtil                                                                                 
   @brief Utility class to to decode data from FILLPARAMS COOL folder                                    
   @brief Can be used with FillParamsTool or python FillParamsData                                       
   @author E.Torrence                                                                                    
**/

#ifndef COOLLUMIUTILITIES_BUNCHLUMISUTIL_H
#define COOLLUMIUTILITIES_BUNCHLUMISUTIL_H

#include <vector>
#include <string>

#include "CoralBase/AttributeList.h"
#include "CoolKernel/Record.h"
#include "CoolLumiUtilities/LumiBlobUtil.h"

class BunchLumisUtil {

 public:
  BunchLumisUtil();


  unsigned int nBunchLumis() const;

  int nxStorage(const cool::Record& rec);
  int nyStorage(const cool::Record& rec);

  int nxStorage(const coral::AttributeList& attrList1) const;
  int nyStorage(const coral::AttributeList& attrList1) const;

  const std::vector<double>& bunchLumis() const;
  

  // Clear all data vectors                                                                               
  void clear();
  
  // Fill values from COOL, returns false on error                                                       
  bool setValue(const coral::AttributeList& attrList1);
  bool setValue(const cool::Record& rec);
  // Set array of colliding BCIDs from FillParams
  // Only needed for certain storage modes
  bool setCollidingBCIDs(std::vector<unsigned int> &);

  // Error string contains error from setValue() operation                                               
  std::string error;

 private:
  // Data                                                                                                


  std::vector<double> m_bunchLumis;
  
  std::vector<unsigned int> m_collidingBCIDs;

  class LumiBlobUtil m_blobUtil;
  
};

#endif
