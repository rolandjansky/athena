/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISCT_FillCabling_H
#define ISCT_FillCabling_H
/**   
 *   @file ISCT_Fill_Cabling.h
 *
 *   @brief Interface for services which fill an SCT cabling object
 *
 *   @author Shaun Roe
 *   @date 05/10/2008
 */

//STL includes
#include <string>

//Gaudi includes
#include "GaudiKernel/IInterface.h"


//fwd declarations
class SCT_CablingSvc;
class StatusCode;
 
/**
 *    @class ISCT_FillCabling
 *    @brief Interface base class for objects which fill the SCT Cabling.
 *
 */
class ISCT_FillCabling:virtual public IInterface{
public:
  ///Virtual destructor
  virtual ~ISCT_FillCabling(){}
  
  /// interfaceID re-implemented from IInterface
  static const InterfaceID & interfaceID();

  /** May set the data source to textFile, database etc
  * @param[in] @c string name of datasource
  */
  virtual StatusCode setDataSource(const std::string & dataSource) = 0;
  
  /** Gets the data source
  * @return @c string name of datasource
  */
  virtual std::string getDataSource() const = 0;
  
  /**Fill the cabling maps
  * @param[in] @c SCT_CablingSvc& , reference to the underlying data service
  */
  virtual StatusCode fillMaps(SCT_CablingSvc * cabling) = 0;
  
  /**Report whether the map was filled
  * @return @c bool
  */
  virtual bool filled() const = 0;
  
  /**Report whether the object can fill its data during the initialize phase
  * @return @c bool
  */
  virtual bool canFillDuringInitialize() const = 0;
};//end of class

inline const InterfaceID & ISCT_FillCabling::interfaceID(){
  static const InterfaceID IID("ISCT_FillCabling",1,0);
  return IID;
}

#endif

