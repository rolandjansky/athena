/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigSiSpacePointTool_page TrigSiSpacePointTool Package

This package contains an interface class ISpacePointProvider

@author D.Emeliyanov AT rlDOTacDOTuk

@section TrigSiSpacePointTool_TrigSiSpacePointToolIntro Introduction

This package defines an abstract interface for the LVL2 Silicon 
data providers (TrigOnlineSpacePointTool, TrigOfflineSpacePointTool and 
LazySpacePointProviderTool)

The interface defines the following methods

|code

  virtual spacePointVec getSpacePoints( double roiEtaMin, double roiEtaMax, 
                                        double roiPhiMin, double roiPhiMax,
                                        bool getPixeSP=true, bool getSctSP=true) = 0;

  virtual spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true) = 0;
  

  virtual StatusCode fillCollections(double, double, double,double,double,double,bool,bool,
                                     std::vector<int>&, std::vector<int>&) = 0;
  virtual StatusCode fillCollections(double,double,double,double,bool,bool,
                                     std::vector<int>&, std::vector<int>&) = 0;
  virtual StatusCode fillCollections(bool,bool,
                                     std::vector<int>&, std::vector<int>&) = 0;

  virtual const std::vector<int>* fillPixelDataErrors() = 0;
  virtual const std::vector<int>* fillSCT_DataErrors() = 0;

|endcode

Note that getSpacePoints methods are obsolete and must not be used. 
    


*/
