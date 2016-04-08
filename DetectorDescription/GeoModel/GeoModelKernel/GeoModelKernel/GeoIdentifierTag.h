/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3E21775800E1.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E21775800E1.cm

//## begin module%3E21775800E1.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3E21775800E1.cp

//## Module: GeoIdentifierTag%3E21775800E1; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoIdentifierTag.h

#ifndef GeoIdentifierTag_h
#define GeoIdentifierTag_h 1

//## begin module%3E21775800E1.additionalIncludes preserve=no
//## end module%3E21775800E1.additionalIncludes

//## begin module%3E21775800E1.includes preserve=yes
//## end module%3E21775800E1.includes

// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3E21775800E1.additionalDeclarations preserve=yes
//## end module%3E21775800E1.additionalDeclarations


//## begin GeoIdentifierTag%3E21775800E1.preface preserve=yes
//## end GeoIdentifierTag%3E21775800E1.preface

//## Class: GeoIdentifierTag%3E21775800E1
//	This class gives an integral identifer to the following
//	physical volume.
//
//	Courtesy of Philipp Shieferdecker (LMU Munich).
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoIdentifierTag : public GeoGraphNode  //## Inherits: <unnamed>%3E21775E00FD
{
  //## begin GeoIdentifierTag%3E21775800E1.initialDeclarations preserve=yes
  //## end GeoIdentifierTag%3E21775800E1.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoIdentifierTag%3E21795602D9
      //	Constructor for the identifier tag. Takes an unsigned
      //	int.
      GeoIdentifierTag (int id);


    //## Other Operations (specified)
      //## Operation: exec%3E217932038B
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: identifier%3E2178DF0075
      //	Specifies the identifier of the next physical volume in
      //	the node tree.
      int getIdentifier () const;

    // Additional Public Declarations
      //## begin GeoIdentifierTag%3E21775800E1.public preserve=yes
      //## end GeoIdentifierTag%3E21775800E1.public

  protected:
    //## Destructor (generated)
      virtual ~GeoIdentifierTag();

    // Additional Protected Declarations
      //## begin GeoIdentifierTag%3E21775800E1.protected preserve=yes
      //## end GeoIdentifierTag%3E21775800E1.protected

  private:
    //## Constructors (generated)
      GeoIdentifierTag(const GeoIdentifierTag &right);

    //## Assignment Operation (generated)
      GeoIdentifierTag & operator=(const GeoIdentifierTag &right);

    // Data Members for Class Attributes

      //## begin GeoM_IdentifierTag::m_identifier%3E2178DF0075.attr preserve=no  public: int {U} 
      int m_identifier;
      //## end GeoM_IdentifierTag::m_identifier%3E2178DF0075.attr

    // Additional Private Declarations
      //## begin GeoIdentifierTag%3E21775800E1.private preserve=yes
      //## end GeoIdentifierTag%3E21775800E1.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoIdentifierTag%3E21775800E1.implementation preserve=yes
      //## end GeoIdentifierTag%3E21775800E1.implementation

};

//## begin GeoIdentifierTag%3E21775800E1.postscript preserve=yes
//## end GeoIdentifierTag%3E21775800E1.postscript

// Class GeoIdentifierTag 

//## Get and Set Operations for Class Attributes (inline)

inline int GeoIdentifierTag::getIdentifier () const
{
  //## begin GeoIdentifierTag::getIdentifier%3E2178DF0075.get preserve=no
  return m_identifier;
  //## end GeoIdentifierTag::getIdentifier%3E2178DF0075.get
}

//## begin module%3E21775800E1.epilog preserve=yes
//## end module%3E21775800E1.epilog


#endif
