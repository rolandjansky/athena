/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WriteMdtGeometry_h
#define WriteMdtGeometry_h

//athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace coral {
  class IRelationalDomain;
  class AttributeList;
  class ISessionProxy;
  class Context;
  class ITableDataEditor;
}
 
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
  ~WriteMdtGeometry()=default;
//=============================public members===================================
  /** Is called at the beginning of the analysis */
  StatusCode initialize();
  /** execute function NOTE: This will read all events at once*/
  inline StatusCode execute() {
    return StatusCode::SUCCESS;
  }
//============================private members===================================
 private:
  coral::Context *m_context;
  coral::IRelationalDomain& domain( const std::string &connectionString );
  void loadServices();
  
  std::string m_connectionString;
  std::string m_WorkingSchema;
  std::string m_username, m_password;
  std::string m_idToFixedIdToolType; // type of the muon fixed id tool
  std::string m_idToFixedIdToolName; // name of the muon fixed id tool
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  coral::ISessionProxy *m_session;
  const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
  const IIdToFixedIdTool *m_id_tool; // identifier converter
		
  inline bool fill_db(coral::ITableDataEditor& editor);
  inline void fillLayer(const MuonGM::MdtReadoutElement *detEl, coral::AttributeList &rowBuffer, const int &ml, const int &ly);
		
  void OpenConnection();
  void CloseConnection(bool commit);
};

}//namespace MuonCalib
#endif //code guard
