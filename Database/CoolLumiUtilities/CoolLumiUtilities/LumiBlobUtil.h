/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**                                                                                                                                                 
   @class LumiBlobUtil                                                                                                                            
   @brief Utility class to to decode data from FILLPARAMS COOL folder                                                                               
   @brief Can be used with FillParamsTool or python FillParamsData                                                                                  
   @author E.Torrence                                                                                                                               
**/

#ifndef COOLLUMIUTILITIES_LUMIBLOBUTIL_H
#define COOLLUMIUTILITIES_LUMIBLOBUTIL_H

#include <vector>
#include <string>

#include "CoralBase/AttributeList.h"
#include "CoolKernel/Record.h"

class LumiBlobUtil {

 public:
  LumiBlobUtil();

  int xvalue(const cool::Record& rec);
  int yvalue(const cool::Record& rec);

  int xvalue(const coral::AttributeList& attrList1) const;
  int yvalue(const coral::AttributeList& attrList1) const;

  // Clear all data vectors                                                                                                                         
  void clear();
  // Fill values from COOL, returns false on error                                                                                                  
  const std::vector<double>& bunchLumis() const;

  bool unpack(const cool::Float &ARI, const coral::Blob &blobBC, const std::vector<unsigned int> &PV);
  //  bool setValue(const coral::AttributeList& attrList1);
  //  bool setValue(const cool::Record& rec);
  // Set array of colliding BCIDs from FillParams                                                                                                   
  // Only needed for certain storage modes                                                                                                          

  // Error string contains error from setValue() operation                                    
  std::string error;
 private:
  // Data                                                                                                                                           
  std::vector<double> m_bunchLumis;

};

#endif
