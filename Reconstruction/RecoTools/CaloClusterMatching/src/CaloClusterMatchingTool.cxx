///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterMatchingTool.cxx 
// Implementation file for class CaloClusterMatchingTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// CaloClusterMatching includes
#include "CaloClusterMatchingTool.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "TopoClusterMap.h"
#include "CxxUtils/make_unique.h"
#include <algorithm>
#include <utility>

namespace ClusterMatching {

  using namespace xAOD;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  CaloClusterMatchingTool::CaloClusterMatchingTool( const std::string& name ) : 
    AsgTool  ( name ),
    m_elementLinkDec("TopoClusterLinks")
  {
    //
    // Property declaration
    // 
    declareProperty( "RequirePositiveE",       m_reqPosE         = true );
    declareProperty( "MinSharedEfrac",         m_minSharedEfrac  = 0.2  );
    declareProperty( "InputClusterCollection", m_clustersIn  = "CaloCalTopoClusters"  );
    declareProperty( "ClusterMapName",         m_mapName     = "TopoClusterEtaPhiMap" );
    declareProperty( "ElementLinkName",        m_elementLinkName = "constituentClusterLinks" );
  }

  // Destructor
  ///////////////
  CaloClusterMatchingTool::~CaloClusterMatchingTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode CaloClusterMatchingTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if(m_elementLinkName.empty()) {
      ATH_MSG_ERROR("Empty name provided for TopoCluster ElementLinks -- aborting");
      return StatusCode::FAILURE;
    } else {
      m_elementLinkDec = SG::AuxElement::Decorator<std::vector<ElementLink<CaloClusterContainer> > >(m_elementLinkName);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode CaloClusterMatchingTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  StatusCode CaloClusterMatchingTool::fillClusterMap() const
  {
    if( evtStore()->contains<TopoClusterMap>(m_mapName) ) return StatusCode::SUCCESS;

    const CaloClusterContainer* topoclusters(nullptr);
    ATH_CHECK( evtStore()->retrieve(topoclusters,m_clustersIn) );
    auto tcmap = CxxUtils::make_unique<TopoClusterMap>();
    ATH_CHECK( tcmap->SetTopoClusters(topoclusters) );

    ATH_CHECK( evtStore()->record(std::move(tcmap),m_mapName) );
    return StatusCode::SUCCESS;
  }

  // return shared fraction of clustered cell energy
  float CaloClusterMatchingTool::getClusterSharedEfrac(const xAOD::CaloCluster& refCluster,
						       const xAOD::CaloCluster& testCluster) const
  {
    int nSharedCells(0);
    float sharedE(0.);
    float totalE(0.);
    for(auto tcItr = testCluster.cell_begin(); tcItr!=testCluster.cell_end(); ++tcItr) {
      if(tcItr->e()>1e-9 || !m_reqPosE) {
	for(auto rcItr = refCluster.cell_begin(); rcItr!=refCluster.cell_end(); ++rcItr) {
	  if(*tcItr && *rcItr && (*tcItr == *rcItr)) {
	    ++nSharedCells;
	    sharedE += tcItr->e();
	  }
	}
	totalE += tcItr->e();
      }
    }
    if(nSharedCells>0) {
      ATH_MSG_VERBOSE("Reference cluster and target cluster share " << nSharedCells << " cells, " << sharedE << " / " << totalE << " MeV");
      ATH_MSG_VERBOSE("DeltaR = " << refCluster.p4().DeltaR(testCluster.p4()));
    }
    return totalE<1e-9 ? 0. : sharedE/totalE;
  }

  // return true if clusters share a given fraction of their cell energy
  bool CaloClusterMatchingTool::clustersAreMatched(const xAOD::CaloCluster& refCluster,
						   const xAOD::CaloCluster& testCluster) const
  {
    return getClusterSharedEfrac(refCluster,testCluster)>m_minSharedEfrac;
  }

  // fill a list of clusters from the testClusters container that match the reference cluster
  // match criteria determined by calling clustersAreMatched
  // return true if matchedClusters list is non-empty
  bool CaloClusterMatchingTool::getMatchedClusters(const xAOD::CaloCluster& refCluster,
						   const std::vector<const xAOD::CaloCluster*>& testClusters,
						   std::vector<const xAOD::CaloCluster*>& matchedClusters) const
  {
    // check that it's empty?
    matchedClusters.clear();

    for(const auto& testCluster : testClusters) {
      if(clustersAreMatched(refCluster, *testCluster)) matchedClusters.push_back(testCluster);
    }
    return !matchedClusters.empty();
  }

  // fill a list of clusters from the configured cluster container that match the reference cluster
  // match criteria determined by calling clustersAreMatched
  // return true if matchedClusters list is non-empty
  bool CaloClusterMatchingTool::getMatchedClusters(const xAOD::CaloCluster& refCluster,
						   std::vector<const xAOD::CaloCluster*>& matchedClusters,
						   bool useLeadingCellEtaPhi) const
  {
    const TopoClusterMap* tcmap(0);
    bool gotMap(false);
    if( evtStore()->contains<TopoClusterMap>(m_mapName) || fillClusterMap().isSuccess()) {
      if( evtStore()->retrieve(tcmap,m_mapName).isSuccess() ) {
	gotMap = true;
      }
    }
    if(!gotMap) {
      ATH_MSG_WARNING("Failed to get cluster eta-phi map!");
      return false;
    }

    float refEta(0.), refPhi(0.);
    if(useLeadingCellEtaPhi) { // needed for muons because muon clusters have no eta/phi
      const CaloCell* leadcell(nullptr);
      float leadcellE(0.);
      for(auto refItr = refCluster.cell_begin(); refItr!=refCluster.cell_end(); ++refItr) {
	if(refItr->e() > leadcellE) leadcell = *refItr;
      }
      if(leadcell) {
	refEta = leadcell->eta();
	refPhi = leadcell->phi();
      }
    } else {
      refEta = refCluster.eta();
      refPhi = refCluster.phi();
    }

    // for now use the standard matching sizes determined by egamma, but may need to change for muons??
    // egamma shower is probably wider than muon cell track
    const std::vector<const xAOD::CaloCluster*> testClusters = 
      tcmap->RetrieveTopoClusters(refEta, refPhi, refCluster.e()*cosh(refEta));

    return getMatchedClusters(refCluster, testClusters, matchedClusters);
  }

  // fill a list of clusters from the testClusters container that match the reference cluster
  // match criteria determined by calling clustersAreMatched
  // return true if matchedClusters list is non-empty
  bool CaloClusterMatchingTool::getMatchedClusters(const xAOD::CaloCluster& refCluster,
						   const std::vector<const xAOD::CaloCluster*>& testClusters,
						   std::vector<std::pair<const xAOD::CaloCluster*, float> >& matchedClustersAndE) const
  {
    // check that it's empty?
    matchedClustersAndE.clear();

    for(const auto& testCluster : testClusters) {
      float sharedEfrac = getClusterSharedEfrac(refCluster, *testCluster);
      if(sharedEfrac>m_minSharedEfrac) {
	matchedClustersAndE.push_back(std::make_pair(testCluster,sharedEfrac));
      }
    }
    return !matchedClustersAndE.empty();
  }

  // fill a list of clusters from the configured cluster container that match the reference cluster
  // match criteria determined by calling clustersAreMatched
  // return true if matchedClusters list is non-empty
  bool CaloClusterMatchingTool::getMatchedClusters(const xAOD::CaloCluster& refCluster,
						   std::vector<std::pair<const xAOD::CaloCluster*, float> >& matchedClustersAndE,
						   bool useLeadingCellEtaPhi) const
  {
    const TopoClusterMap* tcmap(0);
    bool gotMap(false);
    if( evtStore()->contains<TopoClusterMap>(m_mapName) || fillClusterMap().isSuccess()) {
      if( evtStore()->retrieve(tcmap,m_mapName).isSuccess() ) {
	gotMap = true;
      }
    }
    if(!gotMap) {
      ATH_MSG_WARNING("Failed to get cluster eta-phi map!");
      return false;
    }

    float refEta(0.), refPhi(0.);
    if(useLeadingCellEtaPhi) { // needed for muons because muon clusters have no eta/phi
      const CaloCell* leadcell(nullptr);
      float leadcellE(0.);
      for(auto refItr = refCluster.cell_begin(); refItr!=refCluster.cell_end(); ++refItr) {
	if(refItr->e() > leadcellE) leadcell = *refItr;
      }
      if(leadcell) {
	refEta = leadcell->eta();
	refPhi = leadcell->phi();
      }
    } else {
      refEta = refCluster.eta();
      refPhi = refCluster.phi();
    }

    // for now use the standard matching sizes determined by egamma, but may need to change for muons??
    // egamma shower is probably wider than muon cell track
    const std::vector<const xAOD::CaloCluster*> testClusters = 
      tcmap->RetrieveTopoClusters(refEta, refPhi, refCluster.e()*cosh(refEta));

    return getMatchedClusters(refCluster, testClusters, matchedClustersAndE);
  }

  // set ElementLinks to clusters from the configured cluster container that match the reference cluster
  // works via getMatchedClusters
  // return true if matchedClusters list is non-empty
  StatusCode CaloClusterMatchingTool::linkMatchedClusters(const xAOD::CaloCluster& refCluster,
							  const std::vector<const xAOD::CaloCluster*>& testClusters,
							  bool (*gtrthan)(const std::pair<const xAOD::CaloCluster*,float>& pair1,
									  const std::pair<const xAOD::CaloCluster*,float>& pair2)) const
  {
    if(m_elementLinkName.empty()) {
      ATH_MSG_WARNING("No valid ElementLink name specified -- cannot decorate with empty branch name");
      return StatusCode::FAILURE;
    }

    std::vector<std::pair<const CaloCluster*,float> > matchedClustersAndE;
    std::vector<ElementLink<CaloClusterContainer> > tcLinks;
    std::vector<float> tcSharedE;
    if(!testClusters.empty()) {
      const CaloClusterContainer* pClCont = static_cast<const CaloClusterContainer*>(testClusters.front()->container());
      if(getMatchedClusters(refCluster, testClusters, matchedClustersAndE)) {
	std::sort(matchedClustersAndE.begin(),matchedClustersAndE.end(),gtrthan);
	for(const auto& tcAndE : matchedClustersAndE) {
	  tcLinks.push_back(ElementLink<CaloClusterContainer>(*pClCont,tcAndE.first->index()));
	  tcSharedE.push_back(tcAndE.second);
	}
      }
    }
    // apply the decoration -- no exceptions
    m_elementLinkDec(refCluster) = tcLinks;
    ATH_MSG_VERBOSE("Decorate cluster " << refCluster.index() << " with " << m_elementLinkDec(refCluster).size() << " tc links");

    return StatusCode::SUCCESS;
  }

  // set ElementLinks to clusters from the configured cluster container that match the reference cluster
  // works via getMatchedClusters
  // return true if matchedClusters list is non-empty
  StatusCode CaloClusterMatchingTool::linkMatchedClusters(const xAOD::CaloCluster& refCluster,
							  bool useLeadingCellEtaPhi,
							  bool (*gtrthan)(const std::pair<const xAOD::CaloCluster*,float>& pair1,
									  const std::pair<const xAOD::CaloCluster*,float>& pair2)) const
  {
    if(m_elementLinkName.empty()) {
      ATH_MSG_WARNING("No valid ElementLink name specified -- cannot decorate with empty branch name");
      return StatusCode::FAILURE;
    }

    std::vector<std::pair<const CaloCluster*,float> > matchedClustersAndE;
    std::vector<ElementLink<CaloClusterContainer> > tcLinks;
    std::vector<float> tcSharedE;
    // no need to worry about return value.
    if(getMatchedClusters(refCluster, matchedClustersAndE, useLeadingCellEtaPhi)) {
      const CaloClusterContainer* pClCont = static_cast<const CaloClusterContainer*>(matchedClustersAndE.front().first->container());
      std::sort(matchedClustersAndE.begin(),matchedClustersAndE.end(),gtrthan);
      for(const auto& tcAndE : matchedClustersAndE) {
	tcLinks.push_back(ElementLink<CaloClusterContainer>(*pClCont,tcAndE.first->index()));
	tcSharedE.push_back(tcAndE.second);
      }
    }
    // apply the decoration -- no exceptions
    m_elementLinkDec(refCluster) = tcLinks;
    ATH_MSG_VERBOSE("Decorate cluster " << refCluster.index() << " with " << m_elementLinkDec(refCluster).size() << " tc links");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


}
