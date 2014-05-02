/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CE3D68701E6.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CE3D68701E6.cm

//## begin module%3CE3D68701E6.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CE3D68701E6.cp

//## Module: GeoClearAbsPosAction%3CE3D68701E6; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoClearAbsPosAction.h

#ifndef GeoClearAbsPosAction_h
#define GeoClearAbsPosAction_h 1

//## begin module%3CE3D68701E6.additionalIncludes preserve=no
//## end module%3CE3D68701E6.additionalIncludes

//## begin module%3CE3D68701E6.includes preserve=yes
//## end module%3CE3D68701E6.includes

// GeoNodeAction
#include "GeoModelKernel/GeoNodeAction.h"
//## begin module%3CE3D68701E6.additionalDeclarations preserve=yes
//## end module%3CE3D68701E6.additionalDeclarations


//## begin GeoClearAbsPosAction%3CE3D68701E6.preface preserve=yes
//## end GeoClearAbsPosAction%3CE3D68701E6.preface

//## Class: GeoClearAbsPosAction%3CE3D68701E6
//	Clears absolute postion information from full physical
//	volume nodes.  This action is initiated from a change in
//	the alignment of an alignable transformation...although
//	it may of course be used by other clients, too.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoClearAbsPosAction : public GeoNodeAction  //## Inherits: <unnamed>%3CE3D69E00A8
{
  //## begin GeoClearAbsPosAction%3CE3D68701E6.initialDeclarations preserve=yes
  //## end GeoClearAbsPosAction%3CE3D68701E6.initialDeclarations

  public:
    //## Constructors (generated)
      GeoClearAbsPosAction();

    //## Destructor (generated)
      virtual ~GeoClearAbsPosAction();


    //## Other Operations (specified)
      //## Operation: handleFullPhysVol%3CE3D721001A
      //	Handles a physical volume.
      virtual void handleFullPhysVol (const GeoFullPhysVol *vol);

    // Additional Public Declarations
      //## begin GeoClearAbsPosAction%3CE3D68701E6.public preserve=yes
      //## end GeoClearAbsPosAction%3CE3D68701E6.public

  protected:
    // Additional Protected Declarations
      //## begin GeoClearAbsPosAction%3CE3D68701E6.protected preserve=yes
      //## end GeoClearAbsPosAction%3CE3D68701E6.protected

  private:
    //## Constructors (generated)
      GeoClearAbsPosAction(const GeoClearAbsPosAction &right);

    //## Assignment Operation (generated)
      GeoClearAbsPosAction & operator=(const GeoClearAbsPosAction &right);

    // Additional Private Declarations
      //## begin GeoClearAbsPosAction%3CE3D68701E6.private preserve=yes
      //## end GeoClearAbsPosAction%3CE3D68701E6.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoClearAbsPosAction%3CE3D68701E6.implementation preserve=yes
      //## end GeoClearAbsPosAction%3CE3D68701E6.implementation

};

//## begin GeoClearAbsPosAction%3CE3D68701E6.postscript preserve=yes
//## end GeoClearAbsPosAction%3CE3D68701E6.postscript

// Class GeoClearAbsPosAction 

//## begin module%3CE3D68701E6.epilog preserve=yes
//## end module%3CE3D68701E6.epilog


#endif
