///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetDynamicCutsTool.h 
// Header file for class InDetDynamicCutsTool
// Author: C.Rauchegger <Christoph.Rauchegger@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef INDETDYNAMICCUTSTOOL_INDETDYNAMICCUTSTOOL_H
#define INDETDYNAMICCUTSTOOL_INDETDYNAMICCUTSTOOL_H 1

// STL includes
#include <string>
#include <map>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// InDetRecInterfaces includes
#include "InDetRecToolInterfaces/IInDetDynamicCutsTool.h"

#include "TrkTrack/Track.h"

// Forward declaration

namespace InDet {
	class InDetDynamicCutsTool
		: virtual public IInDetDynamicCutsTool,
		          public AthAlgTool
	{ 

		/////////////////////////////////////////////////////////////////// 
		// Public methods: 
		/////////////////////////////////////////////////////////////////// 
	 public: 
		typedef std::map<double, int > EtaIndexMap;



		// Copy constructor: 

		/// Constructor with parameters: 
		InDetDynamicCutsTool( const std::string& type,
			   const std::string& name, 
			   const IInterface* parent );

		/// Destructor: 
		virtual ~InDetDynamicCutsTool(); 

		// Athena algtool's Hooks
		virtual StatusCode  initialize();
		virtual StatusCode  finalize();


		double	getEtaWidthBremByEta 						(double eta);
		double	getMaxdImpactSSSSeedsByEta 			(double eta);
		int 		getMaxDoubleHolesByEta 					(double eta);
		int 		getMaxHolesByEta 								(double eta);
		int 		getMaxHolesGapPatternByEta 			(double eta);
		int 		getMaxHolesPatternByEta 				(double eta);
		int 		getMaxPixelHolesByEta 					(double eta);
		double	getMaxPrimaryImpactByEta 				(double eta);
		int 		getMaxSctHolesByEta 						(double eta);
		int 		getMaxSharedByEta 							(double eta);
		double	getMaxZImpactByEta 							(double eta);
		int 		getMinClustersByEta 						(double eta);
		int 		getMinPixelHitsByEta 						(double eta);
		double	getMinPTByEta 									(double eta);
		double	getMinPTBremByEta 							(double eta);
		int 		getMinSiNotSharedByEta 					(double eta);
		int 		getNWeightedClustersMinByEta		(double eta);
		double	getPhiWidthBremByEta 						(double eta);
		double	getXi2maxByEta 									(double eta);
		double	getXi2maxNoAddByEta 						(double eta);

		std::map<std::string, double> getAllCutsByTrackEta(const Trk::Track* track);
		std::map<std::string, double> getAllCutsByEta(double eta);

		std::map<std::string, int> getAllIntegerCutsByTrackEta(const Trk::Track* track);
		std::map<std::string, int> getAllIntegerCutsByEta(double eta);

		std::map<std::string, double> getAllDoubleCutsByTrackEta(const Trk::Track* track);
		std::map<std::string, double> getAllDoubleCutsByEta(double eta);



		/////////////////////////////////////////////////////////////////// 
		// Private data: 
		/////////////////////////////////////////////////////////////////// 
	 private: 

		/// Default constructor: 
		InDetDynamicCutsTool();

		EtaIndexMap m_etaIndexMap;	// saves the (array) index corresponding to an eta value (rounded to 4 digits), should speed up the search

		std::vector<double> m_etaBins;
		double 							m_maxEta;
		int                 m_noOfEtaBins;

		std::vector<double> 	m_etaWidthBrem;							//cut for brem
		std::vector<double> 	m_maxdImpactSSSSeeds;				// max impact on seeds SSS
		std::vector<int> 			m_maxDoubleHoles;						// max number of double holes
		std::vector<int> 			m_maxHoles;									// max number of Si holes 
		std::vector<int> 			m_maxPixelHoles;						// max number of Pixel holes
		std::vector<double> 	m_maxPrimaryImpact;					// max Rphi IP (primaries) 
		std::vector<int> 			m_maxSctHoles;							// max number of SCT holes
		std::vector<int> 			m_maxShared;								// max number of shared
		std::vector<double> 	m_maxZImpact;								// max Z IP
		std::vector<int> 			m_minClusters;							// min number of clusters
		std::vector<int> 			m_minPixelHits;							// min number of pixel hits	
		std::vector<double> 	m_minPT;										// min pT
		std::vector<double> 	m_minPTBrem;								// min pT for brem reocvery
		std::vector<int> 			m_minSiNotShared;						// min number of NOT shared 
		std::vector<int> 			m_maxHolesGapPattern;				// max holes gap in pattern
		std::vector<int> 			m_maxHolesPattern;					// max holes in pattern
		std::vector<int> 			m_nWeightedClustersMin;			// nWeightedClustersMin
		std::vector<double> 	m_phiWidthBrem;							// phi Width of road for brem (cut for brem)
		std::vector<double> 	m_Xi2max;										// Xi2 max
		std::vector<double> 	m_Xi2maxNoAdd; 							// Xi2 max no add

		int	getIndexByEta 						(double eta);		

	}; 

	// I/O operators
	//////////////////////

	/////////////////////////////////////////////////////////////////// 
	// Inline methods: 
	///////////////////////////////////////////////////////////////////
	
	inline double InDetDynamicCutsTool::getEtaWidthBremByEta 						(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_etaWidthBrem[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getMaxdImpactSSSSeedsByEta 			(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxdImpactSSSSeeds[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxDoubleHolesByEta 					(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxDoubleHoles[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxHolesByEta 								(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxHoles[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxHolesGapPatternByEta 			(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxHolesGapPattern[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxHolesPatternByEta 				(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxHolesPattern[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxPixelHolesByEta 					(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxPixelHoles[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getMaxPrimaryImpactByEta 				(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxPrimaryImpact[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxSctHolesByEta 						(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxSctHoles[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMaxSharedByEta 							(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxShared[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getMaxZImpactByEta 							(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_maxZImpact[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMinClustersByEta 						(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_minClusters[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMinPixelHitsByEta 						(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_minPixelHits[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getMinPTByEta 									(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_minPT[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getMinPTBremByEta 							(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_minPTBrem[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getMinSiNotSharedByEta 					(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_minSiNotShared[etaIndex];

		return -1;		// not found
	}
	inline int InDetDynamicCutsTool::getNWeightedClustersMinByEta		(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_nWeightedClustersMin[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getPhiWidthBremByEta 						(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_phiWidthBrem[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getXi2maxByEta 									(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_Xi2max[etaIndex];

		return -1;		// not found
	}
	inline double InDetDynamicCutsTool::getXi2maxNoAddByEta 						(double eta) {
		int etaIndex = getIndexByEta(eta);
		if (etaIndex > -1)	// found
			return m_Xi2maxNoAdd[etaIndex];

		return -1;		// not found
	}
	
}	// end namespace



#endif //> !INDETDYNAMICCUTSTOOL_INDETDYNAMICCUTSTOOL_H
