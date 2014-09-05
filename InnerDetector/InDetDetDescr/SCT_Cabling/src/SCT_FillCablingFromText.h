/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//STL includes
#include <string>

//Gaudi includes
#include "AthenaBaseComps/AthService.h"

//local includes
#include "SCT_Cabling/ISCT_FillCabling.h"


//fwd declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class SCT_CablingSvc;
class StatusCode;
 
/**
 *    @class SCT_FillCablingFromText
 *    @brief Service which fill the SCT Cabling from plain text (a file).
 *
 */
class SCT_FillCablingFromText: virtual public ISCT_FillCabling, public AthService{
    friend class SvcFactory<SCT_FillCablingFromText>;
public:
  //@name Service methods, reimplemented
  //@{
  SCT_FillCablingFromText(const std::string &name, ISvcLocator * svc);  
  virtual ~SCT_FillCablingFromText(){}
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
  
  /**Filling from text can be done during intialise, it doesn't need the db to be ready
  * @return @c bool
  */
  virtual bool canFillDuringInitialize() const {return true;}
private:
  StatusCode readDataFromFile(SCT_CablingSvc * cabling);
  bool m_filled;
  std::string m_source;
};//end of class
#endif

