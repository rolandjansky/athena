/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MULTILEVELALIGNMENTDBTOOL_H
#define MUONCONDTOOL_MULTILEVELALIGNMENTDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/IMultiLevelAlignmentDbTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include <CLHEP/Geometry/Transform3D.h>
#include "GeoPrimitives/GeoPrimitives.h"

/**
   @file MultiLevelAlignmentDbTool.h
   @class MultiLevelAlignmentDbTool
   
   @brief Tool used retrieve L0, L1, and/or L3 alignment data from the condtions DB or ascii files (for testing), and determine final A-lines to be used for alignment.

   @author Robert Harrington <roberth@bu.edu>
   @date 4-13-09
*/


class Identifier;  
class MuonIdHelper;
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class IIOVSvc;
class StatusCode;
//class AlignableTransform;

namespace MuonGM {
  class MuonDetectorManager;
}

class MultiLevelAlignmentDbTool: virtual public IMultiLevelAlignmentDbTool, public AthAlgTool {
  
 public:    

  enum MLevelId { L2_EI=1, L2_EE=2, L2_EM=3, L2_EO=4, // same as stationRegion of MuonIdHelper
		  L2_B0=1, L2_B1=2, L2_B2=3, L2_B3=4, 
		  L2_B4=5, L2_B5=6, L2_B6=7, L2_B7=8,  
		  L1_ECA=100000010, L1_ECC=100000020, L1_BAR=100000030};

  MultiLevelAlignmentDbTool(const std::string& type, 
			    const std::string& name, 
			    const IInterface* parent); 
  
  virtual ~MultiLevelAlignmentDbTool();


  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);
  
  std::string aLineDataLocation() {return m_abLineDbTool->aLineDataLocation();}
  std::string bLineDataLocation() {return m_abLineDbTool->bLineDataLocation();}

  /*
    inline std::string level3FolderName() const;
    inline std::string level1FolderName() const;
    inline std::string level0FolderName() const;
    
    std::vector<std::string> alignmentFolderNames() const;
    
    inline const ALineMapContainer* level3Container() const;
    inline const ALineMapContainer* level1Container() const;
    inline const ALineMapContainer* level0Container() const;
  */

  virtual StatusCode initialize();

  void decomposeTransform(const Amg::Transform3D& transform, double* values);

 private:     
  
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  StatusCode loadAlignALine(IOVSVC_CALLBACK_ARGS);
  StatusCode loadAlignBLine(IOVSVC_CALLBACK_ARGS);

  StatusCode initializeMdtAlignObjs();
  StatusCode createAlignObjects() const;
  StatusCode readAlignTextFile(const std::string& filename, const std::string& level);

  // returns folder of ALineDbTool
  std::string aLineFolderName() const;
  std::vector<std::string> abLineFolderNames() const;
  std::string bLineFolderName() const;

  virtual const ALineMapContainer* ALineContainer() const;
  virtual const BLineMapContainer* BLineContainer() const;

  virtual const ALineMapContainer* level0Container() const;
  virtual const ALineMapContainer* level1Container() const;
  virtual const ALineMapContainer* level23Container() const;
  virtual const ALineMapContainer* level3Container() const;
  virtual const CscInternalAlignmentMapContainer* ILineContainer() const;
  virtual const MdtAsBuiltMapContainer* AsBuiltContainer() const;

  const Amg::Transform3D level0Transform() const;
  const Amg::Transform3D level1Transform(Identifier id) const;
  const Amg::Transform3D level2Transform(Identifier id) const;
  const Amg::Transform3D level23Transform(Identifier id) const;
  const Amg::Transform3D level3Transform(Identifier id) const;
  const Amg::Transform3D alignToGlobalTransform(Identifier id) const;
  const Amg::Transform3D globalToL23ModuleTransform(Identifier id) const;
  

  /*
    virtual StatusCode loadL3Align(IOVSVC_CALLBACK_ARGS);
    virtual StatusCode loadL3Align(std::string folderName);
    virtual StatusCode loadL1Align(IOVSVC_CALLBACK_ARGS);
    virtual StatusCode loadL1Align(std::string folderName);
    virtual StatusCode loadL0Align(IOVSVC_CALLBACK_ARGS);
    virtual StatusCode loadL0Align(std::string folderName);
  */

  int stationPhiTGC(std::string stName, int fi, int zi_input) const;

  AlignableTransform*        getTransPtr(const std::string key) const;
  const AlignableTransform* cgetTransPtr(const std::string key) const;
  
  Identifier identify(std::string name, int stPhi, int stEta,
		      bool check, bool* isValid, std::string level) const;
  Identifier getL23ModuleId(Identifier id) const;

  const MuonIdHelper* muonIdHelper(Identifier ID) const;

  ToolHandle<IMuonAlignmentDbTool> m_abLineDbTool;

  const MuonGM::MuonDetectorManager* p_muonMgr;

  IIOVSvc* m_IOVSvc;

  const MuonIdHelper* m_muonIdHelper;
  const MdtIdHelper*  m_mdtIdHelper;
  const CscIdHelper*  m_cscIdHelper;
  const RpcIdHelper*  m_rpcIdHelper;
  const TgcIdHelper*  m_tgcIdHelper;
  
  mutable ALineMapContainer * m_alineData;
  mutable BLineMapContainer * m_blineData;

  std::vector<std::string> m_abParlineFolderNames;

  //std::string              m_alineDataLocation;
  //std::string              m_blineDataLocation;

  mutable ALineMapContainer* m_alignModuleALines;
  
  mutable ALineMapContainer * m_level0Data;
  mutable ALineMapContainer * m_level1Data;
  mutable ALineMapContainer * m_level23Data;
  mutable ALineMapContainer * m_level3Data;
  /*
  std::string      m_level3DataLocation;
  std::string      m_level1DataLocation;
  std::string      m_level0DataLocation;
  std::vector<std::string>      m_alignmentFolder;
  */

  const DataHandle<AlignableTransformContainer> 
                           m_aligncontainerhandle ; //!< persistifiable object

  std::string              m_alignroot;             //!< root alignment folder
  std::vector<int>         m_alignchans;            //!< alignment channels
  std::vector<std::string> m_alignobjs;             //!< alignment keys (folder names)

  std::string m_level0AsciiFile;
  std::string m_level1AsciiFile;
  std::string m_level2AsciiFile;
  std::string m_level23AsciiFile;
  std::string m_level3AsciiFile;

  bool m_doL0;
  bool m_doL1;
  bool m_doL23;
  bool m_doL3;

  bool m_useCOOLonly;

  bool m_dumpALines;
  bool m_doSimpleAdd;
};



#endif
