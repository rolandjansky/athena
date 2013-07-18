/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%44662CA903B1.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%44662CA903B1.CLHEP::cm

//## begin module%44662CA903B1.cp preserve=no
//## end module%44662CA903B1.cp

//## Module: EMECCellConstLink%44662CA903B1; Pseudo Package specification
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/LArReadoutGeometry/EMECCellConstLink.h

#ifndef EMECCellConstLink_h
#define EMECCellConstLink_h 1

//## begin module%44662CA903B1.additionalIncludes preserve=no
//## end module%44662CA903B1.additionalIncludes

//## begin module%44662CA903B1.includes preserve=yes
#include "LArReadoutGeometry/EMECCell.h"
//## end module%44662CA903B1.includes

// ConstLink
#include "GeoModelKernel/ConstLink.h"
//## begin module%44662CA903B1.additionalDeclarations preserve=yes
//## end module%44662CA903B1.additionalDeclarations


//## begin EMECCellConstLink%44662CA903B1.preface preserve=yes
//## end EMECCellConstLink%44662CA903B1.preface

//## Class: EMECCellConstLink%44662CA903B1; Instantiated Class
//	Smart Pointer to EMEC Cells.  This reference counted
//	link allocates on demand. It audits the total pointer
//	count and collects the garbage when nobody's looking.
//## Category: LArReadoutGeometry%445002CB020C
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef ConstLink< EMECCell  > EMECCellConstLink;

//## begin EMECCellConstLink%44662CA903B1.postscript preserve=yes
//## end EMECCellConstLink%44662CA903B1.postscript

//## begin module%44662CA903B1.epilog preserve=yes
//## end module%44662CA903B1.epilog


#endif
