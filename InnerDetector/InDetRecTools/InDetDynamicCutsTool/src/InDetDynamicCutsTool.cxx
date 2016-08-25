///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetDynamicCutsTool.cxx 
// Implementation file for class InDetDynamicCutsTool
// Author: C.Rauchegger<christoph.rauchegger@cern.ch>
/////////////////////////////////////////////////////////////////// 

// InDetDynamicCutsTool includes
#include "InDetDynamicCutsTool/InDetDynamicCutsTool.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"


namespace InDet {

	// Constructors
	////////////////
	InDetDynamicCutsTool::InDetDynamicCutsTool( const std::string& type, 
				    const std::string& name, 
				    const IInterface* parent ) : 
		::AthAlgTool  ( type, name, parent   ),
		  m_noOfEtaBins(1)
	{
		declareInterface< IInDetDynamicCutsTool >(this);
		//
		// Property declaration
		// 
		declareProperty("etaBins" ,							m_etaBins = {-1.0});		// highest eta is maxEta. This means default is up to eta 2.7. Which means first bin is from -1.0 (which is eta 0.0, because we use fabs(eta)) to 2.7
		declareProperty("maxEta"  ,							m_maxEta = 2.7 );
	
		declareProperty("etaWidthBrem",            m_etaWidthBrem = {0.2});						  //cut for brem
		declareProperty("maxdImpactSSSSeeds",      m_maxdImpactSSSSeeds = {20.0});			// max impact on seeds SSS
		declareProperty("maxDoubleHoles",          m_maxDoubleHoles = {1});							// max number of double holes
		declareProperty("maxHoles",                m_maxHoles = {2});										// max number of Si holes 
		declareProperty("maxPixelHoles",           m_maxPixelHoles = {1});							// max number of Pixel holes
		declareProperty("maxPrimaryImpact",        m_maxPrimaryImpact = {2.0});				  // max Rphi IP (primaries) 
		declareProperty("maxSctHoles",             m_maxSctHoles = {2});								// max number of SCT holes
		declareProperty("maxShared",               m_maxShared = {1});									// max number of shared
		declareProperty("maxZImpact",              m_maxZImpact = {250.0});						  // max Z IP
		declareProperty("minClusters",             m_minClusters = {9});								// min number of (Si) clusters (Si hits)
		declareProperty("minPixelHits",            m_minPixelHits = {0});								// min number of pixel hits	
		declareProperty("minPT",                   m_minPT = {900.0});									// min pT
		declareProperty("minPTBrem",               m_minPTBrem = {1000.0});						  // min pT for brem reocvery
		declareProperty("minSiNotShared",          m_minSiNotShared = {8});							// min number of NOT shared 
		declareProperty("maxHolesGapPattern",      m_maxHolesGapPattern = {2});					// max holes gap in pattern
		declareProperty("maxHolesPattern",         m_maxHolesPattern = {2});						// max holes in pattern
		declareProperty("nWeightedClustersMin",    m_nWeightedClustersMin = {6});				// nWeightedClustersMin
		declareProperty("phiWidthBrem",            m_phiWidthBrem = {0.3});						  // phi Width of road for brem (cut for brem)
		declareProperty("Xi2max",                  m_Xi2max = {9.0});									  // Xi2 max
		declareProperty("Xi2maxNoAdd",             m_Xi2maxNoAdd = {25.0}); 						// Xi2 max no add

	}

	// Destructor
	///////////////
	InDetDynamicCutsTool::~InDetDynamicCutsTool()
	{}

	// Athena algtool's Hooks
	////////////////////////////
	StatusCode InDetDynamicCutsTool::initialize()
	{
		ATH_MSG_INFO ("Initializing " << name() << "...");

    m_noOfEtaBins = m_etaBins.size();
    unsigned int noOfEtaBins = m_etaBins.size();
    
    if (noOfEtaBins != m_etaWidthBrem.size()) {
      ATH_MSG_ERROR( "No. of cut values (etaWidthBrem) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxdImpactSSSSeeds.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxdImpactSSSSeeds) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxDoubleHoles.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxDoubleHoles) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxHoles.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxHoles) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxPixelHoles.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxPixelHoles) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxPrimaryImpact.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxPrimaryImpact) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxSctHoles.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxSctHoles) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxShared.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxShared) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxZImpact.size()) {
      ATH_MSG_ERROR( "No. of cut values (maxZImpact) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_minClusters.size()) {
      ATH_MSG_ERROR( "No. of cut values (minClusters) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_minPixelHits.size()) {
      ATH_MSG_ERROR( "No. of cut values (minPixel) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_minPT.size()) {
      ATH_MSG_ERROR( "No. of cut values (minPT) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_minPTBrem.size()) {
      ATH_MSG_ERROR( "No. of cut values (minPTBrem) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_minSiNotShared.size()) {
      ATH_MSG_ERROR( "No. of cut values (minSiNotShared) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxHolesGapPattern.size()) {
      ATH_MSG_ERROR( "No. of cut values (nHolesGapMax) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_maxHolesPattern.size()) {
      ATH_MSG_ERROR( "No. of cut values (nHolesMax) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_nWeightedClustersMin.size()) {
      ATH_MSG_ERROR( "No. of cut values (nWeightedClustersMin) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_phiWidthBrem.size()) {
      ATH_MSG_ERROR( "No. of cut values (phiWidthBrem) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_Xi2max.size()) {
      ATH_MSG_ERROR( "No. of cut values (Xi2max) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }
    if (noOfEtaBins != m_Xi2maxNoAdd.size()) {
      ATH_MSG_ERROR( "No. of cut values (Xi2maxNoAdd) and no. of eta bins MUST be the same." );
      return StatusCode::FAILURE;
    }


		
		// Some Tests
		/*
		ATH_MSG_FATAL ("#cr# Initializing " << name() << "...");

		ATH_MSG_FATAL ("#cr# m_etaBins.size " << m_etaBins.size() << "...");
		ATH_MSG_FATAL ("#cr# m_etaBins: " << m_etaBins << "...");

		ATH_MSG_FATAL ("#cr# m_minClusters.size " << m_minClusters.size() << "...");
		ATH_MSG_FATAL ("#cr# m_minClusters: " << m_minClusters << "...");

		ATH_MSG_FATAL ("eta 0: " << getMinClustersByEta(0.));	
		ATH_MSG_FATAL ("eta 1.1: " << getMinClustersByEta(1.1));	
		ATH_MSG_FATAL ("eta 2.6999: " << getMinClustersByEta(2.6999));	
		ATH_MSG_FATAL ("eta 2.7: " << getMinClustersByEta(2.7));
		ATH_MSG_FATAL ("eta 2.7001: " << getMinClustersByEta(2.7001));
		ATH_MSG_FATAL ("eta 2.8: " << getMinClustersByEta(2.8));		
		ATH_MSG_FATAL ("eta 3.99: " << getMinClustersByEta(3.99));		
		ATH_MSG_FATAL ("eta 4.0: " << getMinClustersByEta(4.0));
		ATH_MSG_FATAL ("eta 4.0001: " << getMinClustersByEta(4.001));
		ATH_MSG_FATAL ("eta 4.1: " << getMinClustersByEta(4.1));
		ATH_MSG_FATAL ("eta 95: " << getMinClustersByEta(95));			
		ATH_MSG_FATAL ("eta 2.3000: " << getMinClustersByEta(2.3000));
		ATH_MSG_FATAL ("eta 2.30003: " << getMinClustersByEta(2.30003));
		ATH_MSG_FATAL ("eta 2.3001: " << getMinClustersByEta(2.3001));
		ATH_MSG_FATAL ("eta 2.30017: " << getMinClustersByEta(2.30017));
		ATH_MSG_FATAL ("eta 2.3003: " << getMinClustersByEta(2.3003));
		ATH_MSG_FATAL ("eta 2.30035: " << getMinClustersByEta(2.30035));
		*/

		/*
		std::map<std::string, double> myCutsMap;
		myCutsMap = getAllCutsByEta(1.0);
		for (auto& itrMap : myCutsMap) {
			ATH_MSG_FATAL ("#cr string: " << itrMap.first << " double: " 								<< itrMap.second);
		}

		ATH_MSG_FATAL ("#cr size: " << myCutsMap.size());
		ATH_MSG_FATAL ("#cr minClusters: " << myCutsMap["minClusters"]);
		ATH_MSG_FATAL ("#cr Xi2maxNoAdd: " << myCutsMap["Xi2maxNoAdd"]);
		ATH_MSG_FATAL ("#cr nix: " << myCutsMap["nix"]);
		ATH_MSG_FATAL ("#cr test: " << myCutsMap["test"]);
		*/
		
		ATH_MSG_INFO ("--- Dynamic cuts ---");
		ATH_MSG_INFO ("Eta bins (size=" << m_noOfEtaBins << "): " << m_etaBins);
		ATH_MSG_INFO ("etaWidthBrem: " << m_etaWidthBrem);
		ATH_MSG_INFO ("maxdImpactSSSSeeds: " << m_maxdImpactSSSSeeds);
		ATH_MSG_INFO ("maxDoubleHoles: " << m_maxDoubleHoles);
		ATH_MSG_INFO ("maxHoles: " << m_maxHoles);
		ATH_MSG_INFO ("maxPixelHoles: " << m_maxPixelHoles);
		ATH_MSG_INFO ("maxPrimaryImpact: " << m_maxPrimaryImpact);
		ATH_MSG_INFO ("maxSctHoles: " << m_maxSctHoles);
		ATH_MSG_INFO ("maxShared: " << m_maxShared);
		ATH_MSG_INFO ("maxZImpact: " << m_maxZImpact);
		ATH_MSG_INFO ("minClusters: " << m_minClusters);
		ATH_MSG_INFO ("minPixelHits: " << m_minPixelHits);
		ATH_MSG_INFO ("minPT: " << m_minPT);
		ATH_MSG_INFO ("minPTBrem: " << m_minPTBrem);
		ATH_MSG_INFO ("minSiNotShared: " << m_minSiNotShared);
		ATH_MSG_INFO ("nHolesGapMax: " << m_maxHolesGapPattern);
		ATH_MSG_INFO ("nHolesMax: " << m_maxHolesPattern);
		ATH_MSG_INFO ("nWeightedClustersMin: " << m_nWeightedClustersMin);
		ATH_MSG_INFO ("phiWidthBrem: " << m_phiWidthBrem);
		ATH_MSG_INFO ("Xi2max: " << m_Xi2max);
		ATH_MSG_INFO ("Xi2maxNoAdd: " << m_Xi2maxNoAdd);				
		

		return StatusCode::SUCCESS;
	}

	StatusCode InDetDynamicCutsTool::finalize()
	{
		ATH_MSG_INFO ("Finalizing " << name() << "...");

		/*
		// Test printing eta (cache) map
		for (auto& itrMap : m_etaIndexMap) {
			ATH_MSG_FATAL ("#cr eta: " << itrMap.first << " index: " 								<< itrMap.second);
		}
		*/

		return StatusCode::SUCCESS;
	}

	int InDetDynamicCutsTool::getIndexByEta(double eta) {
		//double etaRounded = round(eta*10000)/10000;		// round eta to 4 digits for eta cache map

		/*
		// V1
		for (unsigned int i=0; i < m_noOfEtaBins-1; i++) {
			if (fabs(eta) > m_etaBins[i] && fabs(eta) <= m_etaBins[i+1]) {			// maybe only fabs(eta) <= m_etaBins[i+1] also enough
				m_etaIndexMap.insert( std::make_pair(fabs(etaRounded),i) );	// insert into map
				return i;
			}
		}
		*/

		/*
		// V2
		for (unsigned int i=m_noOfEtaBins-1; i >= 0; i--) {
			if (fabs(eta) > m_etaBins[i]){
				m_etaIndexMap.insert( std::make_pair(fabs(etaRounded),i) );	// insert into map
				return i;
			}
		}
		*/
	
		// look if the eta value (rounded to 4 digits) has already been searched for

/*
    if (fabs(eta) > m_maxEta)		// eta is too large
			return -1;
			*/
		// maybe a warning instead

		// V3
		/*
		// using a map with all etas and use a find() in there might be slower than the ifs
		auto itIndex = m_etaIndexMap.find(fabs(etaRounded));
		if (itIndex != m_etaIndexMap.end()) { 	// yes
			return itIndex->second;
			//ATH_MSG_FATAL("#cr2# found: eta: " << eta << " rounded: " << fabs(etaRounded));
		}
		*/
		
		double absEta = fabs(eta);

		// no, determine the index from the bin-vector
		for (int i=m_noOfEtaBins-1; i >= 0; i--) {
			if (absEta > m_etaBins[i]) {
				//m_etaIndexMap.insert( std::make_pair(fabs(etaRounded),i) );	// insert into map
				//ATH_MSG_FATAL("#cr2# NOT found: eta: " << eta << " rounded: " << fabs(etaRounded));
				return i;
			}
		}

		return -1;		// not found
	}


	std::map<std::string, double> InDetDynamicCutsTool::getAllCutsByEta(double eta) {
		std::map<std::string, double> allCutsMap;

		int etaIndex = getIndexByEta(eta);

		if (etaIndex > -1) {	// found
			// integer cuts, should be automatically "upconverted" to double
			allCutsMap.insert(std::make_pair("maxDoubleHoles",          m_maxDoubleHoles         [etaIndex]));
			allCutsMap.insert(std::make_pair("maxHoles",                m_maxHoles               [etaIndex]));
			allCutsMap.insert(std::make_pair("maxPixelHoles",           m_maxPixelHoles          [etaIndex]));
			allCutsMap.insert(std::make_pair("maxSctHoles",             m_maxSctHoles            [etaIndex]));
			allCutsMap.insert(std::make_pair("maxShared",               m_maxShared              [etaIndex]));
			allCutsMap.insert(std::make_pair("minClusters",             m_minClusters            [etaIndex]));
			allCutsMap.insert(std::make_pair("minPixelHits",            m_minPixelHits           [etaIndex]));
			allCutsMap.insert(std::make_pair("minSiNotShared",          m_minSiNotShared         [etaIndex]));
			allCutsMap.insert(std::make_pair("maxHolesGapPattern",      m_maxHolesGapPattern     [etaIndex]));
			allCutsMap.insert(std::make_pair("maxHolesPattern",         m_maxHolesPattern        [etaIndex]));
			allCutsMap.insert(std::make_pair("nWeightedClustersMin",    m_nWeightedClustersMin   [etaIndex]));

			// double cuts
			allCutsMap.insert(std::make_pair("etaWidthBrem",            m_etaWidthBrem           [etaIndex]));	
			allCutsMap.insert(std::make_pair("maxdImpactSSSSeeds",      m_maxdImpactSSSSeeds     [etaIndex]));
			allCutsMap.insert(std::make_pair("maxPrimaryImpact",        m_maxPrimaryImpact       [etaIndex]));	
			allCutsMap.insert(std::make_pair("maxZImpact",              m_maxZImpact             [etaIndex]));
			allCutsMap.insert(std::make_pair("minPT",                   m_minPT                  [etaIndex]));
			allCutsMap.insert(std::make_pair("minPTBrem",               m_minPTBrem              [etaIndex]));
			allCutsMap.insert(std::make_pair("phiWidthBrem",            m_phiWidthBrem           [etaIndex]));	
			allCutsMap.insert(std::make_pair("Xi2max",                  m_Xi2max                 [etaIndex]));
			allCutsMap.insert(std::make_pair("Xi2maxNoAdd",             m_Xi2maxNoAdd            [etaIndex]));
		}

		return allCutsMap;

	}

	std::map<std::string, double> InDetDynamicCutsTool::getAllCutsByTrackEta(const Trk::Track* track) {
		return getAllCutsByEta(track->trackParameters()->front()->eta());
	}

	std::map<std::string, int> InDetDynamicCutsTool::getAllIntegerCutsByEta(double eta) {
		std::map<std::string, int> allIntegerCutsMap;

		int etaIndex = getIndexByEta(eta);

		if (etaIndex > -1) {	// found

			allIntegerCutsMap.insert(std::make_pair("maxDoubleHoles",          m_maxDoubleHoles         [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("maxHoles",                m_maxHoles               [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("maxPixelHoles",           m_maxPixelHoles          [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("maxSctHoles",             m_maxSctHoles            [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("maxShared",               m_maxShared              [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("minClusters",             m_minClusters            [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("minPixelHits",            m_minPixelHits           [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("minSiNotShared",          m_minSiNotShared         [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("maxHolesGapPattern",      m_maxHolesGapPattern     [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("maxHolesPattern",         m_maxHolesPattern        [etaIndex]));
			allIntegerCutsMap.insert(std::make_pair("nWeightedClustersMin",    m_nWeightedClustersMin   [etaIndex]));
		}

		return allIntegerCutsMap;

	}

	std::map<std::string, int> InDetDynamicCutsTool::getAllIntegerCutsByTrackEta(const Trk::Track* track) {
		return getAllIntegerCutsByEta(track->trackParameters()->front()->eta());
	}

	std::map<std::string, double> InDetDynamicCutsTool::getAllDoubleCutsByEta(double eta) {
		std::map<std::string, double> allDoubleCutsMap;

		int etaIndex = getIndexByEta(eta);

		if (etaIndex > -1) {	// found

			allDoubleCutsMap.insert(std::make_pair("etaWidthBrem",            m_etaWidthBrem           [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("maxdImpactSSSSeeds",      m_maxdImpactSSSSeeds     [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("maxPrimaryImpact",        m_maxPrimaryImpact       [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("maxZImpact",              m_maxZImpact             [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("minPT",                   m_minPT                  [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("minPTBrem",               m_minPTBrem              [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("phiWidthBrem",            m_phiWidthBrem           [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("Xi2max",                  m_Xi2max                 [etaIndex]));
			allDoubleCutsMap.insert(std::make_pair("Xi2maxNoAdd",             m_Xi2maxNoAdd            [etaIndex]));
		}

		return allDoubleCutsMap;

	}

	std::map<std::string, double> InDetDynamicCutsTool::getAllDoubleCutsByTrackEta(const Trk::Track* track) {
		return getAllDoubleCutsByEta(track->trackParameters()->front()->eta());
	}
	  
}	// end namespace

