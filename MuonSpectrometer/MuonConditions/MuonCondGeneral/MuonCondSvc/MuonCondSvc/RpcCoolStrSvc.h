/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPCCOOLSTRSVC_H
#define MUONCONDSVC_RPCCOOLSTRSVC_H
/**RpcCoolStrSvc - Class with methods for reading and writing to cool databae*/

#include <vector>
#include <string>
#include <fstream>

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"


//Added for attribute list declarations (might not need all of these)
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"

//Added to use RpcIdHelper
#include "Identifier/Identifier.h"
//#include "MuonGeoModel/MuonDetectorManager.h"
// temporary includes to access CLOBs
//#include "CoolKernel/ExtendedAttributeListSpecification.h"
//#include "CoolKernel/PredefinedStorageHints.h"

//Calib conditions data classes
#include "MuonCondData/RpcCondParType.h"
#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcOnlineDBEntry.h"
#include "MuonCondData/RpcCalibDataContainer.h"

#include "MuonCondInterface/RpcICoolStrSvc.h"

class RpcIdHelper;
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
    virtual ~RpcCoolStrSvc();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    virtual const InterfaceID& type() const;


    virtual StatusCode initialize();
    virtual StatusCode finalize();

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

    //Private Data Members:

    /**p_detstore hold a pointer to the transient data storage*/
    StoreGateSvc* p_detstore;

  

    /**RpcIdHelper is used to convert from identifiers to hash ids. MuonDetector manager is a
      requirement on RpcIdHelper*/

    mutable std::vector<const RpcCalibDBEntry*> m_theEntries;
    mutable std::vector<const RpcOnlineDBEntry*> m_theOnlineEntries;


    const RpcIdHelper* m_rpcId;
    //      const MuonGM::MuonDetectorManager * m_muonMgr;

    /// Conditions Attribute List collections used for getting datahandles for callback functions*/
    // const CondAttrListCollection* m_runAtrColl;
    // const CondAttrListCollection* m_pulserAtrColl;
  /**m_log used for sending messages*/
    mutable MsgStream m_log;
    /**Cool folder name*/
    std::string m_folder;

    bool m_debugLevel;

    virtual StatusCode writeToDB() const;
    virtual StatusCode writeToOnlineDB() const;


  };




}
#endif // MUONCONDSVC_CSCCOOLSTRSVC_H
