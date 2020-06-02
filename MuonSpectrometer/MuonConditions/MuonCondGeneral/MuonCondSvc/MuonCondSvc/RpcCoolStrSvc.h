/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPCCOOLSTRSVC_H
#define MUONCONDSVC_RPCCOOLSTRSVC_H

/**RpcCoolStrSvc - Class with methods for reading and writing to cool databae*/

#include "MuonCondInterface/RpcICoolStrSvc.h"
#include "AthenaBaseComps/AthService.h"

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "Identifier/Identifier.h"
#include "MuonCondData/RpcCondParType.h"
#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcOnlineDBEntry.h"
#include "MuonCondData/RpcCalibDataContainer.h"

#include <vector>
#include <string>
#include <fstream>

template <class TYPE> class SvcFactory;

namespace MuonCalib {
  /**
    @class RpcCoolStrSvc

    @author lampen@physics.arizona.edu

    @brief COOL read/write routines

    Contains the major code to write and read calibration files to the database. 
   */

  class RpcCalibDBEntry;

  class RpcCoolStrSvc : public AthService, virtual public RpcICoolStrSvc
  {
   friend class SvcFactory<RpcCoolStrSvc>;

    public:
    RpcCoolStrSvc(const std::string& name, ISvcLocator* svc);
    virtual ~RpcCoolStrSvc()=default;

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    virtual const InterfaceID& type() const;
    virtual StatusCode initialize();

    /**putFile copies data from a calibration file to the database. 
      @param filename The name of the file to copy from.
     */
    virtual StatusCode putFile(const std::string filename) const;
    /**makeFile reads data from the db and creates a claibration file 
      @param filename The name of the file to write to
     */
    virtual StatusCode makeFile(const std::string filename) const;
    /**putOnlineFile copies data from a file containing dead channels to the database. 
      @param filename The name of the file to copy from.
     */
    virtual StatusCode putOnlineFile(const std::string filename) const;
    /**makeOnlineFile copies dead chans from the db to the file. 
      @param filename The name of the file to write to
     */
    virtual StatusCode makeOnlineFile(const std::string filename) const;

    private:
    /**p_detstore hold a pointer to the transient data storage*/
    StoreGateSvc* p_detstore;

    mutable std::vector<const RpcCalibDBEntry*> m_theEntries;
    mutable std::vector<const RpcOnlineDBEntry*> m_theOnlineEntries;

    /**Cool folder name*/
    std::string m_folder;

    virtual StatusCode writeToDB() const;
    virtual StatusCode writeToOnlineDB() const;


  };




}
#endif
