///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IInDetDynamicCutsTool.h 
// Header file for class IInDetDynamicCutsTool
// Author: C.Rauchegger <christoph.rauchegger@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef IINDETDYNAMICCUTSTOOL_H
#define IINDETDYNAMICCUTSTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"

namespace InDet {

	static const InterfaceID IID_IInDetDynamicCutsTool("IInDetDynamicCutsTool", 1, 0);

	class IInDetDynamicCutsTool
  	: virtual public ::IAlgTool
	{ 

  	/////////////////////////////////////////////////////////////////// 
	  // Public methods: 
  	/////////////////////////////////////////////////////////////////// 
	 public: 

  	/** Destructor: 
	   */
  	virtual ~IInDetDynamicCutsTool() {};

	  static const InterfaceID& interfaceID();


  	virtual double	getEtaWidthBremByEta 						(double eta) = 0;
		virtual double	getMaxdImpactSSSSeedsByEta 			(double eta) = 0;
		virtual int 		getMaxDoubleHolesByEta 					(double eta) = 0;
		virtual int 		getMaxHolesByEta 								(double eta) = 0;
		virtual int 		getMaxHolesGapPatternByEta 			(double eta) = 0;
		virtual int 		getMaxHolesPatternByEta 				(double eta) = 0;
		virtual int 		getMaxPixelHolesByEta 					(double eta) = 0;
		virtual double	getMaxPrimaryImpactByEta 				(double eta) = 0;
		virtual int 		getMaxSctHolesByEta 						(double eta) = 0;
		virtual int 		getMaxSharedByEta 							(double eta) = 0;
		virtual double	getMaxZImpactByEta 							(double eta) = 0;
		virtual int 		getMinClustersByEta 						(double eta) = 0;		
		virtual int 		getMinPixelHitsByEta 						(double eta) = 0;
		virtual double	getMinPTByEta 									(double eta) = 0;
		virtual double	getMinPTBremByEta 							(double eta) = 0;
		virtual int 		getMinSiNotSharedByEta 					(double eta) = 0;
		virtual int 		getNWeightedClustersMinByEta		(double eta) = 0;
		virtual double	getPhiWidthBremByEta 						(double eta) = 0;
		virtual double	getXi2maxByEta 									(double eta) = 0;
		virtual double	getXi2maxNoAddByEta 						(double eta) = 0;

		virtual std::map<std::string, double> getAllCutsByTrackEta(const Trk::Track* track) = 0;
		virtual std::map<std::string, double> getAllCutsByEta(double eta) = 0;

		virtual std::map<std::string, int> getAllIntegerCutsByTrackEta(const Trk::Track* track) = 0;
		virtual std::map<std::string, int> getAllIntegerCutsByEta(double eta) = 0;

		virtual std::map<std::string, double> getAllDoubleCutsByTrackEta(const Trk::Track* track) = 0;
		virtual std::map<std::string, double> getAllDoubleCutsByEta(double eta) = 0;

	 protected: 

	}; 


	/////////////////////////////////////////////////////////////////// 
	/// Inline methods: 
	/////////////////////////////////////////////////////////////////// 
	inline const InterfaceID& IInDetDynamicCutsTool::interfaceID() 
	{ 
		return IID_IInDetDynamicCutsTool; 
	}

}// end namespace

#endif //> !IINDETDYNAMICCUTSTOOL_H
