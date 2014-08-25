// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
/// obsolete methods DO NOT USE
///////////////////////////////////////////////////////////////////////


public:

  /// obsolete HashID methods

  void DetHashIDList(DETID detectorID, 
		     double etaMin, double etaMax,
		     double phiMin, double phiMax, 
		     std::vector<IdentifierHash>* IDList );

  // inner detector 
  void DetHashIDList(DETID detectorID,
		     double zMin,   double zMax,
		     double etaMin, double etaMax,
		     double phiMin, double phiMax, 
		     std::vector<IdentifierHash>* IDList);


  void DetHashIDList(DETID detectorID,
		     double zMin,   double zMax,
		     double etaMin, double etaMax,
		     double phiMin, double phiMax, 
		     double xBeam,  double yBeam,
		     std::vector<IdentifierHash>* IDList);


  void DetHashIDList(DETID detectorID,
		     const IRoiDescriptor& roi,
		     std::vector<IdentifierHash>* IDList);

  // inner detector layer
  void DetHashIDList(DETID detectorID, long layer, 
		     double zMin,   double zMax,
		     double etaMin, double etaMax,
		     double phiMin, double phiMax,
		     std::vector<IdentifierHash>* IDList);

  void DetHashIDList(DETID detectorID, long layer,
		     const IRoiDescriptor& roi,
		     std::vector<IdentifierHash>* IDList);

  // inner detector full scan
  void DetHashIDList(DETID detectorID, 
		     std::vector<IdentifierHash>* IDList );

  // calorimeter 
  void DetHashIDList(DETID detectorID, long sampling, 
		     double etaMin, double etaMax,
		     double phiMin, double phiMax,
		     std::vector<IdentifierHash>* IDList);
				
  // muon
  void DetHashIDList(DETID detectorID, TYPEID typeID, 
		     double etaMin, double etaMax,
		     double phiMin, double phiMax, 
		     std::vector<IdentifierHash>* IDList);
	


  /// obsolete rob identifier methods
	    
  void DetROBIDListUint(DETID detectorID, 
			double etaMin,double etaMax,
			double phiMin, double phiMax, 
			std::vector<uint32_t>& outputROBIDList);
				
  // inner detector  
  void DetROBIDListUint(DETID detectorID,
			double zMin,   double zMax,
			double etaMin, double etaMax,
			double phiMin, double phiMax, 
			std::vector<uint32_t>& outputROBIDList);


  void DetROBIDListUint(DETID detectorID,
			double zMin,   double zMax,
			double etaMin, double etaMax,
			double phiMin, double phiMax, 
			double xBeam,  double yBeam,
			std::vector<uint32_t>& outputROBIDList);


  // inner detector by layer 
  void DetROBIDListUint(DETID detectorID, long layer, 
			double zMin,   double zMax,
			double etaMin, double etaMax,
			double phiMin, double phiMax, 
			std::vector<uint32_t>& outputROBIDList);



  // calorimeter
  void DetROBIDListUint(DETID detectorID, long sampling, 
			double etaMin, double etaMax,
			double phiMin, double phiMax, 
			std::vector<uint32_t>& outputROBIDList);





  // Combined inner detector rob id and hash id methods
 
  void DetROBHashIDList(DETID detectorID,
			double zMin,   double zMax,
			double etaMin, double etaMax,
			double phiMin, double phiMax, 
			std::vector<uint32_t>&       outputROBIDList,
			std::vector<IdentifierHash>* IDList );

  void DetROBHashIDList(DETID detectorID, 
			const IRoiDescriptor& roi,
			std::vector<uint32_t>&       outputROBIDList,
			std::vector<IdentifierHash>* IDList);


  // inner detector layer
  void DetROBHashIDList(DETID detectorID, long layer, 
			double zMin,   double zMax,
			double etaMin, double etaMax,
			double phiMin, double phiMax,
			std::vector<uint32_t>&       outputROBIDList,
			std::vector<IdentifierHash>* IDList);

  void DetROBHashIDList(DETID detectorID, long layer, 
			const IRoiDescriptor& roi, 
			std::vector<uint32_t>&       outputROBIDList,
			std::vector<IdentifierHash>* IDList);

  // inner detector full scan
  void DetROBHashIDList(DETID detectorID, 
			std::vector<uint32_t>&       outputROBIDList,
			std::vector<IdentifierHash>* IDList );






  // Find disabled inner detector modules from a layer
  void DetDisabledHashIDList(DETID detectorID, long layer, 
			     double zMin,   double zMax,
			     double etaMin, double etaMax,
			     double phiMin, double phiMax,
			     std::vector<IdentifierHash>* IDList);

#if 0
  void DetDisabledHashIDList(DETID detectorID, long layer, 
			     const IRoiDescriptor& roi,
			     std::vector<IdentifierHash>* IDList);
#endif


  void getDisabledLayers(DETID detectorID, 
			 int subdetector,
			 double zMin,   double zMax,
			 double etaMin, double etaMax,
			 double phiMin, double phiMax,
			 std::vector<int>& layers);

  void getDisabledLayers(DETID detectorID, 
			 double zMin,   double zMax,
			 double etaMin, double etaMax,
			 double phiMin, double phiMax,
			 std::vector<int>& barrellayers, 
			 std::vector<int>& endAlayers, 
			 std::vector<int>& endClayers );
	      
