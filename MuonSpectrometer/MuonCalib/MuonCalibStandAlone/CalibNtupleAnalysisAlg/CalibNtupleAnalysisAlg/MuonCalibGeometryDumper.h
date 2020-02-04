/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibGeometryDumper_h
#define MuonCalibGeometryDumper_h

//athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"


namespace coral {
  class IRelationalDomain;
  class AttributeList;
  class IConnection;
  class ISession;
  class Context;
  class ITableDataEditor;
}
 
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
namespace MuonGM {
  class MuonDetectorManager;
  class MdtReadoutElement;
}

class TFile;
class TTree;
#include "CLHEP/Geometry/Transform3D.h"
namespace ROOT {
  namespace Math{
    class Transform3D;
  }
}

namespace MuonCalib {

class IIdToFixedIdTool;
class MdtTubeGeomertyRow;
class MdtChamberGeometryRow;

class MuonCalibGeometryDumper : public AthAlgorithm {
 public:
//=============================constructor - destructor=========================
  /** Algorithm Constructor */
  MuonCalibGeometryDumper(const std::string& name, ISvcLocator *pSvcLocator);
  /** Algorithm destrucrtor*/
  ~MuonCalibGeometryDumper()=default;
//=============================public members===================================
  /** Is called at the beginning of the analysis */
  StatusCode initialize();
  /** execute function NOTE: This will read all events at once*/
  inline StatusCode execute() {
    return StatusCode::SUCCESS;
  }
//============================private members===================================
 private:
  void loadServices();
  std::string m_rootFile;
  std::string m_idToFixedIdToolType; // type of the muon fixed id tool
  std::string m_idToFixedIdToolName; // name of the muon fixed id tool
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon detector manager
  const IIdToFixedIdTool *m_id_tool; // identifier converter
  inline bool dump_mdt_geometry();
  inline void fillLayer(const Identifier &ch_id, const MuonGM::MdtReadoutElement *detEl, MdtTubeGeomertyRow &row, TTree *tree, const int &ml, const int &ly);
  inline void init_root_transform(ROOT::Math::Transform3D &local_to_global, const HepGeom::Transform3D &hep_transform);
};

}//namespace MuonCalib
#endif //code guard
