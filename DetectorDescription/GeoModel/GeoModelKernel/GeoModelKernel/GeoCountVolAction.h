/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF59FE02DC.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF59FE02DC.cm

//## begin module%3CDF59FE02DC.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDF59FE02DC.cp

//## Module: GeoCountVolAction%3CDF59FE02DC; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoCountVolAction.h

#ifndef GeoCountVolAction_h
#define GeoCountVolAction_h 1

//## begin module%3CDF59FE02DC.additionalIncludes preserve=no
//## end module%3CDF59FE02DC.additionalIncludes

//## begin module%3CDF59FE02DC.includes preserve=yes
//## end module%3CDF59FE02DC.includes

// GeoNodeAction
#include "GeoModelKernel/GeoNodeAction.h"
//## begin module%3CDF59FE02DC.additionalDeclarations preserve=yes
//## end module%3CDF59FE02DC.additionalDeclarations


//## begin GeoCountVolAction%3CDF59FE02DC.preface preserve=yes
//## end GeoCountVolAction%3CDF59FE02DC.preface

//## Class: GeoCountVolAction%3CDF59FE02DC
//	A simple action to count the number of volumes.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoCountVolAction : public GeoNodeAction  //## Inherits: <unnamed>%3CDF5A0F025E
{
  //## begin GeoCountVolAction%3CDF59FE02DC.initialDeclarations preserve=yes
  //## end GeoCountVolAction%3CDF59FE02DC.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoCountVolAction%3CDF5B0F031A
      //	Constructor for the action.
      GeoCountVolAction ();

    //## Destructor (generated)
      virtual ~GeoCountVolAction();


    //## Other Operations (specified)
      //## Operation: handlePhysVol%3CE0CFC4014A
      //	Handles a physical volume.
      virtual void handlePhysVol (const GeoPhysVol *);

      //## Operation: handleFullPhysVol%3CE0CFC4015E
      //	Handles a physical volume.
      virtual void handleFullPhysVol (const GeoFullPhysVol *);

      //## Operation: handleSerialTransformer%3D208CA50301
      //	Handles a Serial Transformer
      virtual void handleSerialTransformer (const GeoSerialTransformer  *st);

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: count%3CDF5A150375
      //	The volume count.
      unsigned int getCount () const;

    // Additional Public Declarations
      //## begin GeoCountVolAction%3CDF59FE02DC.public preserve=yes
      //## end GeoCountVolAction%3CDF59FE02DC.public

  protected:
    // Additional Protected Declarations
      //## begin GeoCountVolAction%3CDF59FE02DC.protected preserve=yes
      //## end GeoCountVolAction%3CDF59FE02DC.protected

  private:
    //## Constructors (generated)
      GeoCountVolAction(const GeoCountVolAction &right);

    //## Assignment Operation (generated)
      GeoCountVolAction & operator=(const GeoCountVolAction &right);

    // Data Members for Class Attributes

      //## begin GeoCountVolAction::count%3CDF5A150375.attr preserve=no  public: unsigned int {U} 
      unsigned int m_count;
      //## end GeoCountVolAction::count%3CDF5A150375.attr

    // Additional Private Declarations
      //## begin GeoCountVolAction%3CDF59FE02DC.private preserve=yes
      //## end GeoCountVolAction%3CDF59FE02DC.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoCountVolAction%3CDF59FE02DC.implementation preserve=yes
      //## end GeoCountVolAction%3CDF59FE02DC.implementation

};

//## begin GeoCountVolAction%3CDF59FE02DC.postscript preserve=yes
//## end GeoCountVolAction%3CDF59FE02DC.postscript

// Class GeoCountVolAction 

//## Get and Set Operations for Class Attributes (inline)

inline unsigned int GeoCountVolAction::getCount () const
{
  //## begin GeoCountVolAction::getCount%3CDF5A150375.get preserve=no
  return m_count;
  //## end GeoCountVolAction::getCount%3CDF5A150375.get
}

//## begin module%3CDF59FE02DC.epilog preserve=yes
//## end module%3CDF59FE02DC.epilog


#endif
