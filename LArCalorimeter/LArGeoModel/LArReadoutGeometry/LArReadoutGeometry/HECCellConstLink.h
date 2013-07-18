/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%445D13DC008E.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%445D13DC008E.CLHEP::cm

//## begin module%445D13DC008E.cp preserve=no
//## end module%445D13DC008E.cp

//## Module: HECCellConstLink%445D13DC008E; Pseudo Package specification
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/LArReadoutGeometry/HECCellConstLink.h

#ifndef HECCellConstLink_h
#define HECCellConstLink_h 1

//## begin module%445D13DC008E.additionalIncludes preserve=no
//## end module%445D13DC008E.additionalIncludes

//## begin module%445D13DC008E.includes preserve=yes
#include "LArReadoutGeometry/HECCell.h"
//## end module%445D13DC008E.includes

// ConstLink
#include "GeoModelKernel/ConstLink.h"
//## begin module%445D13DC008E.additionalDeclarations preserve=yes
//## end module%445D13DC008E.additionalDeclarations


//## begin HECCellConstLink%445D13DC008E.preface preserve=yes
//## end HECCellConstLink%445D13DC008E.preface

//## Class: HECCellConstLink%445D13DC008E; Instantiated Class
//	Smart Pointer to HEC Cells.  This reference counted link
//	allocates on demand. It audits the total pointer count
//	and collects the garbage when nobody's looking.
//## Category: LArReadoutGeometry%445002CB020C
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef ConstLink< HECCell  > HECCellConstLink;

//## begin HECCellConstLink%445D13DC008E.postscript preserve=yes
//## end HECCellConstLink%445D13DC008E.postscript

//## begin module%445D13DC008E.epilog preserve=yes
//## end module%445D13DC008E.epilog


#endif
