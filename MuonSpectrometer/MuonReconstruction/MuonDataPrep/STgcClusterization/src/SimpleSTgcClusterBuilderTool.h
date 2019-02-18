/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef SimpleSTgcClusterBuilderTool_h
#define SimpleSTgcClusterBuilderTool_h

#include "GaudiKernel/ToolHandle.h"
#include "STgcClusterization/ISTgcClusterBuilderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <vector>
#include <set>

class sTgcIdHelper;
namespace MuonGM
{
  class MuonDetectorManager;
}


//
// Simple clusterization tool for MicroMegas
//
namespace Muon
{
  
  class ISTgcClusterBuilderTool;

  class SimpleSTgcClusterBuilderTool : virtual public ISTgcClusterBuilderTool, public AthAlgTool {

  public:

    /** Default constructor */
    SimpleSTgcClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
    
    /** Default destructor */
    virtual ~SimpleSTgcClusterBuilderTool();

    /** standard initialize method */
    virtual StatusCode initialize();
    
    /** standard finalize method */
    virtual StatusCode finalize();

    StatusCode getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
			   std::vector<Muon::sTgcPrepData*>& clustersVect);

  private: 
  
    /// Muon detector manager and helper
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const sTgcIdHelper* m_stgcIdHelper;

    std::vector<std::set<unsigned int>> m_clustersStripNum[3][5];
    std::vector<std::vector<Muon::sTgcPrepData>> m_clusters[3][5];

    bool addStrip(Muon::sTgcPrepData& strip);


  };


}
#endif

