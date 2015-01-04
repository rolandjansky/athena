/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**                                                                                                                 
   @class BunchDescription                                                                                            
   @brief Utility class to to decode data from FILLPARAMS COOL folder                                               
   @brief Can be used with FillParamsTool or python FillParamsData                                                  
   @author E.Torrence                                                                                               
**/

#ifndef COOLLUMIUTILITIES_BUNCHDESCRIPTION_H
#define COOLLUMIUTILITIES_BUNCHDESCRIPTION_H

#include <vector>
#include <string>

#include "CoralBase/AttributeList.h"
#include "CoolKernel/Record.h"

class BunchDescription {

 public:
  BunchDescription();

  // Number of filled bunches (length of vectors below)                                                             
  /* unsigned int nBeam1Bunches() const;                                                                            
  unsigned int nBeam2Bunches() const;                                                                               
  unsigned int nLuminousBunches() const;                                                                            
  //  unsigned int nbcidmasks() const;                                                                              
                                                                                                                    
  // Arrays with BCID numbers for filled bunches                                                                    
  const std::vector<unsigned int>& beam1Bunches() const;                                                            
  const std::vector<unsigned int>& beam2Bunches() const;                                                            
  const std::vector<unsigned int>& luminousBunches() const;                                                         
  // const std::vector<unsigned int>& bcidmasks() const;                                                            
                                                                                                                    
  */

  unsigned int nBunchD1() const;

  const std::vector<unsigned int>& bunchD() const;


  // Clear all data vectors                                                                                          
  void clear();

  // Fill values from COOL, returns false on error                                                                  
  bool setValue(const coral::AttributeList& attrList);
  bool setValue(const cool::Record& rec);

  // Error string contains error from setValue() operation                                                          
  std::string error;


 private:
  // Data                                                                                                           
  /*                                                                                                                
  std::vector<unsigned int> m_beam1Bunches;                                                                         
  std::vector<unsigned int> m_beam2Bunches;                                                                         
  std::vector<unsigned int> m_luminousBunches;                                                                      
  */
  std::vector<unsigned int> m_bunchD;

};

#endif





