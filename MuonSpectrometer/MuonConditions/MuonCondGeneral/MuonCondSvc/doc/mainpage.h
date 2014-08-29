/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MuonCondSvc package
@author Monica.Verducci@cern.ch

 
@section MuonCondSvcIntro Introduction
This package contains the services 
responsible to provide to the
clients (reconstruction) the conditions information from the COOL Database

MuonAlignmentDbSvc uses a Tool to retreive the constants: 
- MuonCalib::MuonAlignmentDbTool provides access to the CLOBS in COOL for the A and B lines.

MuonDetectorStatusDbSvc
- MuonCalib::MuonDetectorStatusDbTool provides access to the CLOBS in COOL for the teube status.


@ref used_MuonCondSvc

@ref requirements_MuonCondSvc
*/

/**
@page used_MuonCondSvc Used Packages
@htmlinclude used_packages.html
*/
/**
@page requirements_MuonCondSvc Requirements
@include requirements
*/
