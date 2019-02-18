/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_FillCablingFromText_H
#define SCT_FillCablingFromText_H
/**   
 *   @file SCT_Fill_CablingFromText.h
 *
 *   @brief Fills an SCT cabling object from a plain text source
 *
 *   @author Shaun Roe
 *   @date 05/11/2008
 */

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Cabling/ISCT_FillCabling.h"

//STL includes
#include <atomic>
#include <string>

//fwd declarations
class IdentifierHash;
class SCT_CablingData;
class SCT_OnlineId;
class SCT_SerialNumber;
class StatusCode;
 
/**
 *    @class SCT_FillCablingFromText
 *    @brief Service which fill the SCT Cabling from plain text (a file).
 *
 */
class SCT_FillCablingFromText: public extends<AthAlgTool, ISCT_FillCabling> {
 public:
  //@name Service methods, reimplemented
  //@{
  SCT_FillCablingFromText(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_FillCablingFromText() = default;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //@}

  /** May set the data source to textFile, database etc
   * @param[in] @c string name of datasource
   */
  virtual StatusCode setDataSource(const std::string& dataSource);
  
  /** Gets the data source
   * @return @c string name of datasource
   */
  virtual std::string getDataSource() const;
  
  /**Fill the cabling maps
   * @return @c cabling map object
   */
  virtual SCT_CablingData getMaps() const;
  
  /**Report whether the map was filled
   * @return @c bool
   */
  virtual bool filled() const;
  
  /**Filling from text can be done during intialise, it doesn't need the db to be ready
   * @return @c bool
   */
  virtual bool canFillDuringInitialize() const {return true;}

 private:
  StatusCode readDataFromFile(SCT_CablingData& data) const;

  bool insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn, SCT_CablingData& data) const;

  mutable std::atomic_bool m_filled;
  std::string m_source;
};//end of class

#endif // SCT_FillCablingFromText_H
