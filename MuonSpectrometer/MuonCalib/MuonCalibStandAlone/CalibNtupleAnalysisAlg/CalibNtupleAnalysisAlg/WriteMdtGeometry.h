/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WriteMdtGeometry_h
#define WriteMdtGeometry_h

//athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 

namespace coral {
  class IRelationalDomain;
  class AttributeList;
  class ISessionProxy;
  class Context;
  class ITableDataEditor;
}
 
//class MdtIdHelper;
#include "MuonIdHelpers/MdtIdHelper.h"
namespace MuonGM {
  class MuonDetectorManager;
  class MdtReadoutElement;
}

namespace MuonCalib {

class IIdToFixedIdTool;

class WriteMdtGeometry : public AthAlgorithm {
 public:
//=============================constructor - destructor=========================
  /** Algorithm Constructor */
  WriteMdtGeometry(const std::string &name, ISvcLocator *pSvcLocator);
  /** Algorithm destrucrtor*/
  ~WriteMdtGeometry();
//=============================public members===================================
  /** Is called at the beginning of the analysis */
  StatusCode initialize();
  /** execute function NOTE: This will read all events at once*/
  inline StatusCode execute() {
    return StatusCode::SUCCESS;
  }
  /** finalize functions */
  inline StatusCode finalize() {
    return StatusCode::SUCCESS;
  }
//============================private members===================================
 private:
  coral::Context *m_context;
  coral::IRelationalDomain& domain( const std::string &connectionString );
  void loadServices();
  coral::ISessionProxy *m_session;
  std::string connectionString;
  std::string WorkingSchema;
  std::string m_username, m_password;
  std::string m_MDT_ID_helper; // name of the MDT ID helper
  std::string m_idToFixedIdToolType; // type of the muon fixed id tool
  std::string m_idToFixedIdToolName; // name of the muon fixed id tool
  const MdtIdHelper *m_MdtIdHelper; // pointer to the MDT ID helper
  const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
						     // detector manager
  const IIdToFixedIdTool *m_id_tool; // identifier converter
		
  inline bool fill_db(coral::ITableDataEditor& editor);
  inline void fillLayer(const MuonGM::MdtReadoutElement *detEl, coral::AttributeList &rowBuffer, const int &ml, const int &ly);
		
  void OpenConnection();
  void CloseConnection(bool commit);
};

}//namespace MuonCalib
#endif //code guard
