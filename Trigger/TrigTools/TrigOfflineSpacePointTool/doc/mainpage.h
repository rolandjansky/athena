/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrigOfflineSpacePointTool  Package

This package contains implementations of TrigOfflineSpacePointTool and 
LazySpacePointProviderTool classes

@author D.Emeliyanov AT rlDOTacDOTuk

@section TrigOfflineSpacePointToolIntro Introduction

This package contains implementations of the two LVL2 Silicon 
data providers -  TrigOfflineSpacePointTool and LazySpacePointProviderTool. 
TrigOfflineSpacePointTool takes spacepoints from offline containers stored in 
the StoreGate and converts them into LVL2 format (TrigSiSpacePoint). 
LazySpacePointProviderTool is essentially RoI-based version of offline 
data preparation chain very similar to the EF data preparation algorithm. 

Both packages are based on ISpacePointProvider abstract interface and 
implement the following virtual methods


|code

  spacePointVec getSpacePoints( double roiEtaMin, double roiEtaMax, 
                                double roiPhiMin, double roiPhiMax,
                                bool getPixeSP=true, bool getSctSP=true);

  spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true);

  StatusCode fillCollections(double, double, double, double, double, double, bool, bool,
                             std::vector<int>&, std::vector<int>&);

  StatusCode fillCollections(double, double, double, double, bool, bool,
                             std::vector<int>&, std::vector<int>&);
  StatusCode fillCollections(bool, bool,
                             std::vector<int>&, std::vector<int>&);

  const std::vector<int>* fillPixelDataErrors();
  const std::vector<int>* fillSCT_DataErrors();

|endcode

Note that getSpacePoints methods are obsolete and must not be used. 
    
@section TrigOfflineSpacePointToolReq Requirements

@include requirements

*/
