/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3DCE8CE30228.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DCE8CE30228.cm

//## begin module%3DCE8CE30228.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3DCE8CE30228.cp

//## Module: GeoShapePath%3DCE8CE30228; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapePath.h

#ifndef GeoShapePath_h
#define GeoShapePath_h 1

//## begin module%3DCE8CE30228.additionalIncludes preserve=no
//## end module%3DCE8CE30228.additionalIncludes

//## begin module%3DCE8CE30228.includes preserve=yes
#include "GeoModelKernel/GeoShape.h"
//## end module%3DCE8CE30228.includes

// Path
#include "GeoModelKernel/Path.h"
//## begin module%3DCE8CE30228.additionalDeclarations preserve=yes
//## end module%3DCE8CE30228.additionalDeclarations


//## begin GeoShapePath%3DCE8CE30228.preface preserve=yes
//## end GeoShapePath%3DCE8CE30228.preface

//## Class: GeoShapePath%3DCE8CE30228; Instantiated Class
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef Path< GeoShape  > GeoShapePath;

//## begin GeoShapePath%3DCE8CE30228.postscript preserve=yes
//## end GeoShapePath%3DCE8CE30228.postscript

//## begin module%3DCE8CE30228.epilog preserve=yes
//## end module%3DCE8CE30228.epilog


#endif
