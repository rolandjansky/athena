
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISCT_FillCabling_H
#define ISCT_FillCabling_H
/**   
 *   @file ISCT_Fill_Cabling.h
 *
 *   @brief Interface for tools which fill an SCT cabling object
 *
 *   @author Shaun Roe
 *   @date 05/10/2008
 */

//Gaudi includes
#include "GaudiKernel/IAlgTool.h"

//STL includes
#include <string>

//fwd declarations
class SCT_CablingData;
class StatusCode;
 
/**
 *    @class ISCT_FillCabling
 *    @brief Interface base class for objects which fill the SCT Cabling.
 *
 */
class ISCT_FillCabling: virtual public IAlgTool {
 public:
  /// Virtual destructor
  virtual ~ISCT_FillCabling() = default;
  
  /// InterfaceID re-implemented from IInterface
  DeclareInterfaceID(ISCT_FillCabling, 1, 0);

  /** May set the data source to textFile, database etc
   * @param[in] @c string name of datasource
   */
  virtual StatusCode setDataSource(const std::string& dataSource) = 0;
  
  /** Get the data source
   * @return @c string name of datasource
   */
  virtual std::string getDataSource() const = 0;
  
  /** Get the cabling maps
   * @return @c cabling map object
   */
  virtual SCT_CablingData getMaps() const = 0;
  
  /** Report whether the map was filled
   * @return @c bool
   */
  virtual bool filled() const = 0;
  
  /** Report whether the object can fill its data during the initialize phase
   * @return @c bool
   */
  virtual bool canFillDuringInitialize() const = 0;
};//end of class

#endif // ISCT_FillCabling_H
