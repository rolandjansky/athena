/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef SimpleMMClusterBuilderTool_h
#define SimpleMMClusterBuilderTool_h

#include "GaudiKernel/ToolHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

class MmIdHelper;
namespace MuonGM
{
  class MuonDetectorManager;
}

//
// Simple clusterization tool for MicroMegas
//
namespace Muon
{
  
  class SimpleMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {

  public:
    /** Default constructor */
    SimpleMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
     
    /** Default destructor */
    virtual ~SimpleMMClusterBuilderTool();

    /** standard initialize method */
    virtual StatusCode initialize();
    
    /** standard finalize method */
    virtual StatusCode finalize();

    StatusCode getClusters(std::vector<Muon::MMPrepData>& stripsVect, 
			   std::vector<Muon::MMPrepData*>& clustersVect);

  private: 

    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager* m_muonMgr;
    const MmIdHelper* m_mmIdHelper;
    
};


}
#endif
