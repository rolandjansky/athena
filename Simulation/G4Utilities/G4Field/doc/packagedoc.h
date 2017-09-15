/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page G4Field_page G4Field
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4Field_G4Field Introduction

This package provides a simple interface to G4 Fields and for modifying the magnetic field to be used during simulation.

@section G4Field_G4Field Class Overview

The package includes the following classes:
 - AtlasField , H8Field , H8FieldSym , ConstantField : Various base classes for fields which can be used during simulation (including, of course, test beam simulation)
 - ConstantFieldHandler : An interface for constructing constant fields
 - ConstantFieldXMLHandler : An interface for constructing constant fields from an XML file
 - FieldMapMgr : The manager that handles the fields which will be used during the simulation



*/
