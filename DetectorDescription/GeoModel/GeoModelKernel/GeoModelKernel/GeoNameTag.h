/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CEA34860084.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CEA34860084.cm

//## begin module%3CEA34860084.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CEA34860084.cp

//## Module: GeoNameTag%3CEA34860084; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoNameTag.h

#ifndef GeoNameTag_h
#define GeoNameTag_h 1

//## begin module%3CEA34860084.additionalIncludes preserve=no
//## end module%3CEA34860084.additionalIncludes

//## begin module%3CEA34860084.includes preserve=yes
#include <string>
//## end module%3CEA34860084.includes

// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3CEA34860084.additionalDeclarations preserve=yes
//## end module%3CEA34860084.additionalDeclarations


//## begin GeoNameTag%3CEA34860084.preface preserve=yes
//## end GeoNameTag%3CEA34860084.preface

//## Class: GeoNameTag%3CEA34860084
//	This class gives a name to the next physical volume.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoNameTag : public GeoGraphNode  //## Inherits: <unnamed>%3CEA34A70118
{
  //## begin GeoNameTag%3CEA34860084.initialDeclarations preserve=yes
  //## end GeoNameTag%3CEA34860084.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoNameTag%3CEA467D013E
      //	Constructor for the name tag.  Takes a base name.
      GeoNameTag (const std::string &BaseName);


    //## Other Operations (specified)
      //## Operation: exec%3CEA44BC00AC
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: name%3CEA34C5032E
      //	Specifies the name of the next physical volume in the
      //	node tree.
      const std::string& getName () const;

    // Additional Public Declarations
      //## begin GeoNameTag%3CEA34860084.public preserve=yes
      //## end GeoNameTag%3CEA34860084.public

  protected:
    //## Destructor (generated)
      virtual ~GeoNameTag();

    // Additional Protected Declarations
      //## begin GeoNameTag%3CEA34860084.protected preserve=yes
      //## end GeoNameTag%3CEA34860084.protected

  private:
    //## Constructors (generated)
      GeoNameTag(const GeoNameTag &right);

    //## Assignment Operation (generated)
      GeoNameTag & operator=(const GeoNameTag &right);

    // Data Members for Class Attributes

      //## begin GeoNameTag::m_name%3CEA34C5032E.attr preserve=no  public: std::string {U} 
      std::string m_name;
      //## end GeoNameTag::m_name%3CEA34C5032E.attr

    // Additional Private Declarations
      //## begin GeoNameTag%3CEA34860084.private preserve=yes
      //## end GeoNameTag%3CEA34860084.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoNameTag%3CEA34860084.implementation preserve=yes
      //## end GeoNameTag%3CEA34860084.implementation

};

//## begin GeoNameTag%3CEA34860084.postscript preserve=yes
//## end GeoNameTag%3CEA34860084.postscript

// Class GeoNameTag 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoNameTag::getName () const
{
  //## begin GeoNameTag::getName%3CEA34C5032E.get preserve=no
  return m_name;
  //## end GeoNameTag::getName%3CEA34C5032E.get
}

//## begin module%3CEA34860084.epilog preserve=yes
//## end module%3CEA34860084.epilog


#endif
