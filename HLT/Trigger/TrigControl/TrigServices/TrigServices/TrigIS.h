/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_TRIGIS_H
#define TRIGSERVICES_TRIGIS_H

/**@class TrigIS
 * @brief IS publication service
 *
 * This service is a simple wrapper around ISInfoDictionary. Mainly to handle
 * exceptions and to setup the IPC infrastrucure. In addition, it may provide
 * asynchronous publishing (currently not implemented). By default the ownership
 * of the ISInfo object remains with the client. Hoever, for asynchronous publishing
 * this service takes the ownership and deletes the objects after publication.
 *
 * @author  Frank Winklmeier
 * @version $Id: TrigIS.h 5 2013-05-14 10:33:04Z ricab $
 */

#include "TrigKernel/ITrigIS.h"
#include "AthenaBaseComps/AthService.h"

#undef PACKAGE_VERSION
#include "ipc/partition.h"
#include "is/infodictionary.h"

// Forward declarations
class IPCPartition;
class ISInfoDictionary;
class ISInfo;

template <class TYPE> class SvcFactory;

class TrigIS : virtual public ITrigIS,
               public AthService {

public:
  TrigIS(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigIS();

  /*
   * Publish ISInfo object
   *
   * Forwards to IsInfoDictionary::checkin and catches exceptions.
   * Ownership of info remains with caller.
   */
  virtual StatusCode publish(const std::string& name, const ISInfo& info, bool keep_history=false);
  
  /*
   * Same as publish but might publish ISInfo object asynchronously.
   * Ownership of info is transfered to this service.
   */
  virtual void publish_async(const std::string& name, ISInfo* info, bool keep_history=false);

  /*
   * Return reference to ISInfoDictionary
   */
  virtual ISInfoDictionary& infoDict() { return m_isDict; }
  
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  virtual StatusCode initialize();

protected:
  friend class SvcFactory<TrigIS>;

  /// default constructor (do not use)
  TrigIS();

private:
  IPCPartition        m_partition;
  ISInfoDictionary    m_isDict;
  bool                m_validPartition;
  
  /* Properties */
  std::string         m_partitionName;  ///< partition name
};

#endif
