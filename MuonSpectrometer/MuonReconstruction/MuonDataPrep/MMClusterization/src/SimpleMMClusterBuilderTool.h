/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef SimpleMMClusterBuilderTool_h
#define SimpleMMClusterBuilderTool_h

#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"


#include "MuonIdHelpers/IMuonIdHelperSvc.h"


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
			   std::vector<Muon::MMPrepData*>& clustersVect) const;

  private: 
    /// Muon Detector Descriptor
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    
    bool m_useErrorParametrization;
    uint m_maxHoleSize;
    
};


}
#endif
