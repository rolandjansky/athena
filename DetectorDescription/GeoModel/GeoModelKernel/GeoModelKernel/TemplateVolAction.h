/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CE222400350.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CE222400350.cm

//## begin module%3CE222400350.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CE222400350.cp

//## Module: TemplateVolAction%3CE222400350; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/TemplateVolAction.h

#ifndef TemplateVolAction_h
#define TemplateVolAction_h 1

//## begin module%3CE222400350.additionalIncludes preserve=no
//## end module%3CE222400350.additionalIncludes

//## begin module%3CE222400350.includes preserve=yes
//## end module%3CE222400350.includes

// GeoVolumeAction
#include "GeoModelKernel/GeoVolumeAction.h"
//## begin module%3CE222400350.additionalDeclarations preserve=yes
//## end module%3CE222400350.additionalDeclarations


//## begin TemplateVolAction%3CE222400350.preface preserve=yes
//## end TemplateVolAction%3CE222400350.preface

//## Class: TemplateVolAction%3CE222400350
//	---------------------------------------------------------
//	-------------//
//	An example that does nothing, for people
//	to copy and fill in.
//	---------------------------------------------------------
//	-------------//
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class TemplateVolAction : public GeoVolumeAction  //## Inherits: <unnamed>%3CE22245000E
{
  //## begin TemplateVolAction%3CE222400350.initialDeclarations preserve=yes
  //## end TemplateVolAction%3CE222400350.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: TemplateVolAction%3CE2391403B5
      TemplateVolAction ();

    //## Destructor (generated)
      ~TemplateVolAction();


    //## Other Operations (specified)
      //## Operation: handleVPhysVol%3CE2357D018D
      //	Callback method. Overriden by users.
      virtual void handleVPhysVol (const GeoVPhysVol *);

    // Additional Public Declarations
      //## begin TemplateVolAction%3CE222400350.public preserve=yes
      //## end TemplateVolAction%3CE222400350.public

  protected:
    // Additional Protected Declarations
      //## begin TemplateVolAction%3CE222400350.protected preserve=yes
      //## end TemplateVolAction%3CE222400350.protected

  private:
    // Additional Private Declarations
      //## begin TemplateVolAction%3CE222400350.private preserve=yes
      //## end TemplateVolAction%3CE222400350.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin TemplateVolAction%3CE222400350.implementation preserve=yes
      //## end TemplateVolAction%3CE222400350.implementation

};

//## begin TemplateVolAction%3CE222400350.postscript preserve=yes
//## end TemplateVolAction%3CE222400350.postscript

// Class TemplateVolAction 

//## begin module%3CE222400350.epilog preserve=yes
//## end module%3CE222400350.epilog


#endif
