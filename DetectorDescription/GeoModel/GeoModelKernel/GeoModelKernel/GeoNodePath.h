/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3DCE87C10157.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DCE87C10157.cm

//## begin module%3DCE87C10157.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3DCE87C10157.cp

//## Module: GeoNodePath%3DCE87C10157; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoNodePath.h

#ifndef GeoNodePath_h
#define GeoNodePath_h 1

//## begin module%3DCE87C10157.additionalIncludes preserve=no
//## end module%3DCE87C10157.additionalIncludes

//## begin module%3DCE87C10157.includes preserve=yes
//## end module%3DCE87C10157.includes

// Path
#include "GeoModelKernel/Path.h"
//## begin module%3DCE87C10157.additionalDeclarations preserve=yes
#include "GeoModelKernel/GeoVPhysVol.h"
//## end module%3DCE87C10157.additionalDeclarations


//## begin GeoNodePath%3DCE87C10157.preface preserve=yes
//## end GeoNodePath%3DCE87C10157.preface

//## Class: GeoNodePath%3DCE87C10157; Instantiated Class
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef Path< GeoVPhysVol  > GeoNodePath;

//## begin GeoNodePath%3DCE87C10157.postscript preserve=yes
//## end GeoNodePath%3DCE87C10157.postscript

//## begin module%3DCE87C10157.epilog preserve=yes
//## end module%3DCE87C10157.epilog


#endif
