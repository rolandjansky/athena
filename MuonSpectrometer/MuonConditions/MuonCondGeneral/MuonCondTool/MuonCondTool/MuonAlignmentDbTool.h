/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MUONALIGNMENTDBTOOL_H
#define MUONCONDTOOL_MUONALIGNMENTDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IMuonAlignmentDbTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "StoreGate/StoreGateSvc.h"
class Identifier; 
class StoreGateSvc; 
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class IIOVSvc;
class StatusCode;

class MuonAlignmentDbTool: public AthAlgTool, public IMuonAlignmentDbTool
{

public:    

  MuonAlignmentDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);
  
  inline std::string aLineFolderName() const;
  inline std::string bLineFolderName() const;
  inline std::string iLineFolderName() const;
  inline std::string asBuiltFolderName() const;
  std::vector<std::string> abLineFolderNames() const;
  inline const ALineMapContainer* ALineContainer() const;
  inline const BLineMapContainer* BLineContainer() const;
  inline const CscInternalAlignmentMapContainer* ILineContainer() const;
  inline const MdtAsBuiltMapContainer* AsBuiltContainer() const;

  std::string aLineDataLocation() { return m_alineDataLocation; }
  std::string bLineDataLocation() { return m_blineDataLocation; }
  std::string iLineDataLocation() { return m_ilineDataLocation; }
  std::string asBuiltDataLocation() { return m_asbuiltDataLocation; }
  std::vector<std::string> parlineFolder() { return m_parlineFolder; }
    
  virtual StatusCode initialize();
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadAlignALine(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadAlignBLine(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadAlignILine(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadAlignAsBuilt(IOVSVC_CALLBACK_ARGS);

private: 
 
  StatusCode loadAlignABLines(std::string folderName);
  StatusCode loadAlignILines(std::string folderName);
  StatusCode loadAlignAsBuilt(std::string folderName);
  int stationPhiTGC(std::string stName, int fi, int zi_input) const;
 
  void setALinesFromAscii() const;
  void setAsBuiltFromAscii() const;
  void dumpALines(const std::string& folderName);
  void dumpBLines(const std::string& folderName);
  void dumpILines(const std::string& folderName);
  void dumpAsBuilt(const std::string& folderName);
  long int getLastIOVforThisFolder(std::string& folderName);
  void setLastIOVforThisFolder(std::string& folderName, long int iov);
    

  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;
  const MdtIdHelper* m_mdtIdHelper;
  const CscIdHelper* m_cscIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const TgcIdHelper* m_tgcIdHelper;

  std::string m_geometryVersion;    
    
  mutable ALineMapContainer * m_alineData;
  mutable BLineMapContainer * m_blineData;
  mutable CscInternalAlignmentMapContainer * m_ilineData;
  mutable MdtAsBuiltMapContainer * m_asbuiltData;

  std::string      m_alineDataLocation;
  std::string      m_blineDataLocation;
  std::string      m_ilineDataLocation;
  std::string      m_asbuiltDataLocation;
  std::vector<std::string>       m_parlineFolder;
  std::map<std::string,long int> m_lastIOVread; // map of last IOV read per folder (key is the folder name)
 
  bool m_dumpALines;
  bool m_dumpBLines;
  bool m_dumpILines;
  bool m_dumpAsBuilt;
  bool m_ILinesFromDb;
  
  std::string m_aLinesFile;
  std::string m_asBuiltFile;

  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;

};
 
#endif
