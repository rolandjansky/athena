/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_TRIGISHELPER_H
#define TRIGSERVICES_TRIGISHELPER_H

/**
 * @file   TrigISHelper.h
 * @brief  Helper tool for IS access
 * @author Frank Winklmeier, Werner Wiedenmann
 *
 * $Id: TrigISHelper.h 5 2013-05-14 10:33:04Z ricab $
 */

// STL includes
#include <string>
#include <vector>
#include <map>
#include <utility>

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

// To avoid compiler warnings about redefinition through omni includes
#undef PACKAGE_VERSION
// TDAQ includes
#include "is/info.h"
#include "is/infodictionary.h"
#include "is/infodynany.h"
#include "ipc/partition.h"


/**
 * @brief Helper tool for IS access
 *
 * This tool wraps some of the IS methods. Mainly to allow for configurable
 * partition and IS names. Use the properties (e.g. RunParams) to specify
 * them:
 *
 * Example: TrigISHelper.MyISInfo = initial/MyISServer.ISInfoName
 *
 * This will read the IS value from partition initial. If no partition is
 * explicitly specified use the value of the PartitionName property.
 */
class TrigISHelper : public AthAlgTool {

public:
  /// Aliases for IS objects this tool can read
  enum ISObject {
    RunParams,
    LumiBlock,
    SolenoidCurrent,
    ToroidCurrent
  };
    
public:

  TrigISHelper(const std::string &type, const std::string &name, const IInterface *parent);    
  virtual ~TrigISHelper();

  static const InterfaceID& interfaceID();  
  virtual StatusCode initialize();
  
  /**
   * @brief Wrapper for ISInfoDictionary::findValue
   *
   * Main difference to the IS method is that it returns immediatelly
   * if running without a partition.
   *
   * @param name      Name of IS value
   * @param value     Filled with IS value
   *
   * @return SUCCESS  Value retrieved or running without partition
   *         FAILURE  Could not retrieve value from IS
   */
  StatusCode findValue(ISObject obj, ISInfo& value);

  /**
   * @brief Get attributes of a given type for IS objects
   *
   * This method retrieves all attributes of type T from the
   * given IS object. This can be used in case there is no IS type
   * definition available.
   *
   * @param name      Name of IS value
   * @param values    Vector to be filled with attributes of type T
   *                 
   * @return SUCCESS  Value retrieved or running without partition
   *         FAILURE  Could not retrieve value from IS
   */
  template <class T>
  StatusCode getAttributes(ISObject obj, std::vector<T>& values);

  /**
   * @brief Read attribute of an ISNamedInfo object
   *
   * T is the type of the ISNamedInfo object, R the value type and
   * G is a function object that reads the attribute of T.
   *
   * @param name      Name of IS value
   * @param value     Filled with value of attribute
   * @param getter    Functor of type 'void (const T& t, R& r)' that fills r
   *                  with the value of an attribute of t.
   *                 
   * @return SUCCESS  Value retrieved or running without partition
   *         FAILURE  Could not retrieve value from IS
   */
  template <class T, class R, class G>
  StatusCode findValue(ISObject obj, R& value, G getter);

  /*
   * Check if partition for ISObject obj is valid (!= 'none')
   */
  bool validPartition(ISObject obj);

  const std::string& partition(ISObject obj) const;
  const std::string& isName(ISObject obj) const;
  
private:
  // Properties
  StringProperty                  m_partitionName;    ///< Default partition name
  bool                            m_ignoreIfMissing;
  std::map<ISObject, std::string> m_isprop;           ///< Names of IS objects

  /// Property handler
  void initPartitionName(Property& prop);
  
  /// Map ISObject -> (partition, IS name)
  typedef std::map<ISObject, std::pair<std::string, std::string> > ISObjectMap_t;
  ISObjectMap_t  m_isid;                      ///< partition and IS name for each ISObject
 
  /// Return (partition, IS) name pair from isid property
  std::pair<std::string,std::string> split(const std::string& isid); 
};


//--------------------------------------------------------------------------------
// Inline
//--------------------------------------------------------------------------------
inline bool TrigISHelper::validPartition(ISObject obj)
{
  const std::string& name = m_isid[obj].first;
  return (name!="None" && name!="NONE");  
}


inline const InterfaceID& TrigISHelper::interfaceID()
{
  static const InterfaceID _IID("TrigISHelper", 1, 0);
  return _IID;
}

//--------------------------------------------------------------------------------
// Templates
//--------------------------------------------------------------------------------
template <class T, class R, class G>
StatusCode TrigISHelper::findValue(ISObject obj, R& value, G getter)
{
  const std::string& partName = m_isid[obj].first;
  const std::string& isName = m_isid[obj].second;
  
  try {
    IPCPartition part(partName);
    T namedInfo(part, isName);
    namedInfo.checkout();
    getter(namedInfo, value);   // set 'value'
  }
  catch (daq::is::Exception& e) {
    ATH_MSG_WARNING("IS Exception reading " << isName << " from partition "
                    << partName << ". Exception was: " << e.what());

    return m_ignoreIfMissing ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Successfully read " << isName << " from IS: " << value);
  return StatusCode::SUCCESS;
}


template <class T>
StatusCode TrigISHelper::getAttributes(ISObject obj, std::vector<T>& values)
{
  const std::string& partName = m_isid[obj].first;
  const std::string& isName = m_isid[obj].second;

  ISInfoDynAny ida;
  try {
    IPCPartition part(partName);
    ISInfoDictionary isInfoDict(part);
    isInfoDict.findValue(isName, ida);
  }
  catch (daq::is::Exception& e) {
    ATH_MSG_WARNING("IS Exception reading " << isName << " from partition "
                    << partName << ". Exception was: " << e.what());

    return m_ignoreIfMissing ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }
  
  for (size_t i=0; i<ida.getAttributesNumber(); ++i) {
    if ( is::type2id<T>::id == ida.getAttributeType(i) ) {
      values.push_back(ida.getAttributeValue<T>(i));
    }
  }
  return StatusCode::SUCCESS;
}

#endif
