/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_FillCablingFromCoolVector_H
#define SCT_FillCablingFromCoolVector_H
/**   
 *   @file SCT_FillCablingFromCoolVector.h
 *
 *   @brief Fills an SCT cabling object from the database,using Cool Vector Payloads
 *
 *   @author Shaun Roe
 *   @date 25/08/2014
 */

//STL includes
#include <string>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"

#include "GaudiKernel/ServiceHandle.h"

//Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"

//local includes
#include "SCT_Cabling/ISCT_FillCabling.h"


//fwd declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class SCT_CablingSvc;
class StatusCode;

/**
 *    @class SCT_FillCablingFromCoolVector
 *    @brief Service which fill the SCT Cabling from the database, using Cool Vector Payloads (CVP).
 *
 */
class SCT_FillCablingFromCoolVector: virtual public ISCT_FillCabling, public AthService{
  friend class SvcFactory<SCT_FillCablingFromCoolVector>;
public:
  //@name Service methods, reimplemented
  //@{
  SCT_FillCablingFromCoolVector(const std::string &name, ISvcLocator * svc);  
  virtual ~SCT_FillCablingFromCoolVector(){}
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //interfaceID() implementation is in the baseclass
  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  //@}
  
  /** May set the data source to textFile, database etc
   * @param[in] @c string name of datasource
   */
  virtual StatusCode setDataSource(const std::string & dataSource);
  
  /** Gets the data source
   * @return @c string name of datasource
   */
  virtual std::string getDataSource() const;
  
  /**Fill the cabling maps
   * @param[in] @c SCT_CablingSvc& , reference to the underlying data service
   */
  virtual StatusCode fillMaps(SCT_CablingSvc * cabling);
  
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
  StatusCode readDataFromDb(SCT_CablingSvc * cabling);
  
  //retrieve a IOVDbSvc coracool dataset, give error message if it is empty or the pointer is zero
  bool successfulFolderRetrieve(const DataHandle<CondAttrListVec> &pDataVec, const std::string & folderName);
  //retrieve a service and give err msg in case of failure
  template<class S>
  bool retrievedService(S & service, const std::string & serviceName){
    if (service.retrieve().isFailure()) return msg(MSG::ERROR)<<"The service "<<serviceName<<" could not be retrieved."<<endreq, false;
    return true; 
  }
  bool m_filled;
  std::string m_source;
  ServiceHandle<StoreGateSvc> m_detStore;
};//end of class
#endif

