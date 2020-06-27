/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcDb_H
#define AmdcDb_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "AthenaKernel/IOVSvcDefs.h"
#include "AmdcDb/AmdcDbMisc.h"
#include "AmdcDb/IRDBAccessSvcWithUpdate.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>

class StoreGateSvc;
class AmdcsimrecAthenaSvc;
class AmdcDbSvc;

template <class TYPE> class SvcFactory;

/**
 * @class AmdcDb
 *
 * This class is an implementation of the class IRDBAccessSvc
 * 
 * @author samusog@cern.ch
 *
 */

class AmdcDb final : public AthService, virtual public IRDBAccessSvcWithUpdate {
 public:
  static const InterfaceID& interfaceID() { return IID_IRDBAccessSvc; }

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;

  virtual bool       InitializedSvc() override;
  virtual bool       UsableSvc() override;
  virtual StatusCode UpdatedSvc(IOVSVC_CALLBACK_ARGS) override;

  StatusCode AmdcsimrecAthenaSvcUpdatedSvc(IOVSVC_CALLBACK_ARGS);
  
  virtual IRDBRecordset_ptr getRecordsetPtr(const std::string& node,
                                            const std::string& tag,
                                            const std::string& tag2node="",
                                            const std::string& connName="") override;

  //Functions of IRDBAccessSvc Not implemented
  virtual bool connect   (const std::string& connName) override;
  virtual bool disconnect(const std::string& connName) override;
  virtual bool shutdown(  const std::string& connName) override;
  virtual std::string getChildTag(const std::string& childNode,
				  const std::string& parentTag,
				  const std::string& parentNode,
				  const std::string& connName) override;
  std::unique_ptr<IRDBQuery> getQuery(const std::string& node,
			      const std::string& tag,
			      const std::string& tag2node,
			      const std::string& connName) override;
  virtual void getTagDetails(RDBTagDetails& tagDetails,
                             const std::string& tag,
                             const std::string& connName = "ATLASDD") override;
  friend class SvcFactory<AmdcDb>;

  // Standard Constructor
  AmdcDb(const std::string& name, ISvcLocator* svc);

  // Standard Destructor
  virtual ~AmdcDb();

 private:
   bool m_IsUsable ; //!< Tell usuability state 
   
   bool m_IsInitialized ; //!< Tell initialisation state 
   
   StoreGateSvc* p_detStore     ; //!< Pointer On detector store
  
   AmdcDbSvc* p_AmdcDbSvcFromAmdc ; //!< Pointer on AmdcDbSvc
   AmdcDbSvc* p_AmdcDbSvcFromRDB  ; //!< Pointer on AmdcDbSvc

   std::string m_AMDBtag; //!< name of Oracle node

   int m_UglyCodeOn ; //!< Control of a hack to reproduce old way 

   int m_FromAmdcEpsLengthMM ; //!< Control precision printing
   int m_FromAmdcEpsLengthCM ; //!< Control precision printing
   int m_FromAmdcEpsAngle    ; //!< Control precision printing
   int m_FromRDBEpsLengthMM  ; //!< Control precision printing
   int m_FromRDBEpsLengthCM  ; //!< Control precision printing  
   int m_FromRDBEpsAngle     ; //!< Control precision printing
   
   std::vector<std::string> m_TabFromAmdcEpsLengthMM ; //!< Control precision printing
   std::vector<std::string> m_TabFromAmdcEpsLengthCM ; //!< Control precision printing
   std::vector<std::string> m_TabFromAmdcEpsAngle    ; //!< Control precision printing
   std::vector<std::string> m_TabFromRDBEpsLengthMM  ; //!< Control precision printing
   std::vector<std::string> m_TabFromRDBEpsLengthCM  ; //!< Control precision printing  
   std::vector<std::string> m_TabFromRDBEpsAngle     ; //!< Control precision printing
   
   std::vector<int>         m_ValFromAmdcEpsLengthMM ; //!< Control precision printing
   std::vector<int>         m_ValFromAmdcEpsLengthCM ; //!< Control precision printing
   std::vector<int>         m_ValFromAmdcEpsAngle    ; //!< Control precision printing
   std::vector<int>         m_ValFromRDBEpsLengthMM  ; //!< Control precision printing
   std::vector<int>         m_ValFromRDBEpsLengthCM  ; //!< Control precision printing  
   std::vector<int>         m_ValFromRDBEpsAngle     ; //!< Control precision printing
   
   IRDBRecordset_ptr m_emptyRecordset ; //!< Pointer on an empty Recordset

   ServiceHandle<AmdcsimrecAthenaSvc> p_AmdcsimrecAthenaSvc;  //!< Pointer On AmdcsimrecAthenaSvc

   //Db Keys
   std::string m_detectorKey  ;
   std::string m_detectorNode ;

   StatusCode regFcnAmdcsimrecAthenaSvcUpdatedSvc();
   StatusCode DoUpdatedSvc();
   int m_AmdcsimrecAthenaSvcUpdatedSvcDONE ;
};

#endif

