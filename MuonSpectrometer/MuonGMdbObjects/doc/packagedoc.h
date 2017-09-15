/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonGMdbObjects_page The MuonGMdbObjects Package

@section MuonGMdbObjects_IntroMuonGMdbObjects Introduction

This package contains objects used for reading from the Geometry DataBase the main tables containing the MS primary numbers;
The need for this kind of objects came at the time when the Nova DB and the Oracle DB where running at the same time, holding copies of the same layouts in addition to
the Combined Test Beam layout, stored only in NOVA and the newest ATLAS layouts written only in Oracle.

In order to avoid duplicating the code (and classes) reading the geometry database for the Nova and Oracle case, the classes in this package have been designed 
following the data structure of the Nova Objects and using templated functions in the reading code (MuonGM::DBReader.h).



*/
