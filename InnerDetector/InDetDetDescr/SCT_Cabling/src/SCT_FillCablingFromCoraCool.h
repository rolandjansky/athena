/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_FillCablingFromCoraCool_H
#define SCT_FillCablingFromCoraCool_H
/**   
 *   @file SCT_FillCablingFromCoraCool.h
 *
 *   @brief Fills an SCT cabling object from the database
 *
 *   @author Shaun Roe
 *   @date 05/11/2008
 */

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Cabling/ISCT_FillCabling.h"

#include "AthenaPoolUtilities/CondAttrListVec.h"

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
 *    @class SCT_FillCablingFromCoraCool
 *    @brief Service which fill the SCT Cabling from the database, using CoraCool.
 *
 */
class SCT_FillCablingFromCoraCool: public extends<AthAlgTool, ISCT_FillCabling> {
public:
  //@name Service methods, reimplemented
  //@{
  SCT_FillCablingFromCoraCool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_FillCablingFromCoraCool() = default;
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
  
  /**Filling from database cannot be done during intialise.
   * @return @c bool
   */
  virtual bool canFillDuringInitialize() const {return false;}
private:
  //read from db
  StatusCode readDataFromDb(SCT_CablingData& data) const;
  //determine which folder to use; COMP200 style or CONDBR2 style
  std::string determineFolder(const std::string& option1, const std::string& option2) const;
  
  //retrieve a IOVDbSvc coracool dataset, give error message if it is empty or the pointer is zero
  bool successfulFolderRetrieve(const DataHandle<CondAttrListVec>& pDataVec, const std::string& folderName) const;

  bool insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn, SCT_CablingData& data) const;

  mutable std::atomic_bool m_filled;
  std::string m_source;
};//end of class

#endif
