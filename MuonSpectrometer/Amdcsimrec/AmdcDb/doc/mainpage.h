/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage AmdcDb Package

@author samusog@cern.ch

@section IntroductionAmdcDb Introduction

AmdcDb is a package of the Amdcsimrec package.
It contains an implementation of IRDBAccessSvc used
to do comparison of Amdc and Oracle tables. 

A documentation in pdf format is available in
the CVS repository of the package in the sub dir doc/

@section OverviewAmdcDb Class Overview

The AmdcDb package contains:

  - AmdcDb          is an implementation of the class IRDBAccessSvc
  
  - AmdcDbRecord    is an implementation of the class IRDBRecord

  - AmdcDbRecordset is an implementation of the class IRDBRecordset

  - AmdcDbSvc is a class managing set of AmdcDbRecordset

  - AmdcDbSvcMakerFromRDB is a class to set instantiation of  AmdcDbSvc from an IRDBAccessSvc

  - AmdcDbSvcMakerFromAmdc is a class to set instantiation of AmdcDbSvc from an Amdcsimrec interface

  - AmdcDb2Sql is a class producing Sql files


@section ExtrasAmdcDb Extra Pages

  - @ref used_AmdcDb
  - @ref requirements_AmdcDb

*/

/**
@page used_AmdcDb Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_AmdcDb Requirements
@include requirements
*/
