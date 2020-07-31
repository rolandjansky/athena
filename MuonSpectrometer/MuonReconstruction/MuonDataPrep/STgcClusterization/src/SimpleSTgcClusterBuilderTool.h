/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimpleSTgcClusterBuilderTool_h
#define SimpleSTgcClusterBuilderTool_h

#include "STgcClusterization/ISTgcClusterBuilderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <vector>
#include <string>
#include <set>

//
// Simple clusterization tool for STgc
//
namespace Muon
{
  
  class ISTgcClusterBuilderTool;

  class SimpleSTgcClusterBuilderTool : virtual public ISTgcClusterBuilderTool, public AthAlgTool {

  public:

    /** Default constructor */
    SimpleSTgcClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
    
    /** Default destructor */
    virtual ~SimpleSTgcClusterBuilderTool()=default;

    /** standard initialize method */
    virtual StatusCode initialize();

    StatusCode getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
			   std::vector<Muon::sTgcPrepData*>& clustersVect)const;

  private: 

    double m_chargeCut;
    unsigned int m_maxHoleSize;
    double m_addError;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    /// private functions
    void dumpStrips( std::vector<Muon::sTgcPrepData>& stripsVect,
		     std::vector<Muon::sTgcPrepData*>& clustersVect )const;
  
    bool addStrip(const Muon::sTgcPrepData& strip,std::vector<std::set<unsigned int>> clustersStripNum[2][4], 
                  std::vector<std::vector<Muon::sTgcPrepData>> clusters[2][4])const;


  };


}
#endif

