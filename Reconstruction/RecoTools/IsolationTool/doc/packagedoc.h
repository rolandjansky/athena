/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page IsolationTool_page The Track Isolation Tools

 - A package with base implementations to calculate the track isolation et() given a TrackParticle and a 
delta R, or a Calorimeter isolation et() given an INavigable4Momentum (electron, photon, muon) and a delta R. 
The calorimeter isolation can be done only at the ESD level where the calorimeter cells and clusters are 
available. The track isoaltion can be done both at the ESD and AOD levels where the TrackParticles are 
available. 

 - The implementation details can be done elsewhere. For example, for the muon calorimeter isolation, this 
base implementation is overwritten in derived class in the package Reconstruction/MuonIdentification/MuonIsolationTools

 - For questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov

*/
