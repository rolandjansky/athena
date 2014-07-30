/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrigOnlineSpacePointTool Package

This package contains implementation of a LVL2 data providing tool. 
The TrigOnlineSpacePointTool has the following functionality:

   - it performs ByteStream(BS) conversion to Pixel and SCT clusters (RIOs)
   - it creates Pixel and SCT spacepoints

The BS conversion mechanism implemented in this tool effectively supersedes 
the London Scheme which was used in releases before 10.0.6. 

@author Dmitry.Emeliyanov@cern.ch

@section  TrigOnlineSpacePointTool Overview

The tool is based on an abstract interface SpacePointProvider (defined in 
Trigger/TrigTools/TrigSiSpacePointTool package). 

The interface contains the following methods:

  virtual spacePointVec getSpacePoints( double roiEtaMin, double roiEtaMax, 
					double roiPhiMin, double roiPhiMax,
					bool getPixeSP=true, bool getSctSP=true)

  virtual spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true)
  
  virtual StatusCode fillCollections(double,double,double,double,bool,bool,
				     std::vector<int>&, std::vector<int>&)

  virtual StatusCode fillCollections(bool,bool,
				     std::vector<int>&, std::vector<int>&)

where spacePointVec is a STL vector of TrigSiSpacePoints. The first two methods returns 
a vector of spacepoints while the other two make use of Identifiable Containers (IDC) - 
spacepoints are stored in Pixel and SCT IDCs. In turn, the IDCs are stored in the StoreGate. 

The first and third methods take RoI coordinates as input parameters while the other two methods 
simply create spacepoints for the whole ID (FullScan mode). Note that fillCollections methods 
fills std::vectors of HashIds of TrigSpacePointCollections stored in the IDCs. These vectors are 
supposed to be used in a LVL2 tracking algorithm which retrieves the spacepoint IDCs from the 
StoreGate and then iterates over them. 


*/  
