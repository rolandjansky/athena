/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CEA349303CC.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CEA349303CC.cm

//## begin module%3CEA349303CC.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CEA349303CC.cp

//## Module: GeoSerialDenominator%3CEA349303CC; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoSerialDenominator.h

#ifndef GeoSerialDenominator_h
#define GeoSerialDenominator_h 1

//## begin module%3CEA349303CC.additionalIncludes preserve=no
//## end module%3CEA349303CC.additionalIncludes

//## begin module%3CEA349303CC.includes preserve=yes
#include <string>
//## end module%3CEA349303CC.includes

// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3CEA349303CC.additionalDeclarations preserve=yes
//## end module%3CEA349303CC.additionalDeclarations


//## begin GeoSerialDenominator%3CEA349303CC.preface preserve=yes
//## end GeoSerialDenominator%3CEA349303CC.preface

//## Class: GeoSerialDenominator%3CEA349303CC
//	This class generates a name for all physical volumes
//	following it.  The name is a prefix string followed by a
//	serial number that starts at zero.  The naming scheme
//	applies to all following sibling physical volumes.  The
//	naming is reset only when a GeoNameTag or another Geo
//	SerialDenominator is found.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoSerialDenominator : public GeoGraphNode  //## Inherits: <unnamed>%3CEA34A000AA
{
  //## begin GeoSerialDenominator%3CEA349303CC.initialDeclarations preserve=yes
  //## end GeoSerialDenominator%3CEA349303CC.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoSerialDenominator%3CEA45B1016D
      //	Constructor for the serial denominator.  Takes a base
      //	name.
      GeoSerialDenominator (const std::string &BaseName);


    //## Other Operations (specified)
      //## Operation: exec%3CEA44D600BE
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: baseName%3CEA4485022A
      //	Specifies the name of the next physical volume in the
      //	node tree.
      const std::string& getBaseName () const;

    // Additional Public Declarations
      //## begin GeoSerialDenominator%3CEA349303CC.public preserve=yes
      //## end GeoSerialDenominator%3CEA349303CC.public

  protected:
    //## Destructor (generated)
      virtual ~GeoSerialDenominator();

    // Additional Protected Declarations
      //## begin GeoSerialDenominator%3CEA349303CC.protected preserve=yes
      //## end GeoSerialDenominator%3CEA349303CC.protected

  private:
    //## Constructors (generated)
      GeoSerialDenominator(const GeoSerialDenominator &right);

    //## Assignment Operation (generated)
      GeoSerialDenominator & operator=(const GeoSerialDenominator &right);

    // Data Members for Class Attributes

      //## begin GeoSerialDenominator::m_baseName%3CEA4485022A.attr preserve=no  public: std::string {U} 
      std::string m_baseName;
      //## end GeoSerialDenominator::m_baseName%3CEA4485022A.attr

    // Additional Private Declarations
      //## begin GeoSerialDenominator%3CEA349303CC.private preserve=yes
      //## end GeoSerialDenominator%3CEA349303CC.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoSerialDenominator%3CEA349303CC.implementation preserve=yes
      //## end GeoSerialDenominator%3CEA349303CC.implementation

};

//## begin GeoSerialDenominator%3CEA349303CC.postscript preserve=yes
//## end GeoSerialDenominator%3CEA349303CC.postscript

// Class GeoSerialDenominator 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoSerialDenominator::getBaseName () const
{
  //## begin GeoSerialDenominator::getBaseName%3CEA4485022A.get preserve=no
  return m_baseName;
  //## end GeoSerialDenominator::getBaseName%3CEA4485022A.get
}

//## begin module%3CEA349303CC.epilog preserve=yes
//## end module%3CEA349303CC.epilog


#endif
