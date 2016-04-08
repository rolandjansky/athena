/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D1F73570100.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D1F73570100.cm

//## begin module%3D1F73570100.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3D1F73570100.cp

//## Module: GeoSerialTransformer%3D1F73570100; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoSerialTransformer.h

#ifndef GeoSerialTransformer_h
#define GeoSerialTransformer_h 1

//## begin module%3D1F73570100.additionalIncludes preserve=no
//## end module%3D1F73570100.additionalIncludes

//## begin module%3D1F73570100.includes preserve=yes
//## end module%3D1F73570100.includes

// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3D1F73570100.additionalDeclarations preserve=yes
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoXF.h"
//## end module%3D1F73570100.additionalDeclarations


//## begin GeoSerialTransformer%3D1F73570100.preface preserve=yes
//## end GeoSerialTransformer%3D1F73570100.preface

//## Class: GeoSerialTransformer%3D1F73570100
//	This class takes a physical volume and places it
//	according to a transformation field, N times.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoSerialTransformer : public GeoGraphNode  //## Inherits: <unnamed>%3D1F73A60014
{
  //## begin GeoSerialTransformer%3D1F73570100.initialDeclarations preserve=yes
  //## end GeoSerialTransformer%3D1F73570100.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoSerialTransformer%3D1F75650201
      GeoSerialTransformer (const GeoVPhysVol *volume, const GeoXF::Function *func, unsigned int copies);


    //## Other Operations (specified)
      //## Operation: exec%3D1F753D031C
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

      //## Operation: getFunction%3FD60E350091
      //	Returns the transformation field itself.
      const GeoXF::Function * getFunction () const;

    // Additional Public Declarations
      //## begin GeoSerialTransformer%3D1F73570100.public preserve=yes

  // Returns the volume:
  PVConstLink getVolume () const
  {
    return m_physVol;
  }

  // Returns the number of copies:
  unsigned int getNCopies () const
  {
    return m_nCopies;
  }

  // Returns the transformation to the ith copy:
  HepGeom::Transform3D getTransform (int i) const
  {
    return (*m_function) (i);
  }
      //## end GeoSerialTransformer%3D1F73570100.public
  protected:
    //## Destructor (generated)
      virtual ~GeoSerialTransformer();

    // Additional Protected Declarations
      //## begin GeoSerialTransformer%3D1F73570100.protected preserve=yes
      //## end GeoSerialTransformer%3D1F73570100.protected

  private:
    //## Constructors (generated)
      GeoSerialTransformer(const GeoSerialTransformer &right);

    //## Assignment Operation (generated)
      GeoSerialTransformer & operator=(const GeoSerialTransformer &right);

    // Data Members for Class Attributes

      //## Attribute: m_nCopies%3D1F73CB039C
      //	Number of copies of a physical volume to generate.
      //## begin GeoSerialTransformer::m_nCopies%3D1F73CB039C.attr preserve=no  private: unsigned int {U} 
      unsigned int m_nCopies;
      //## end GeoSerialTransformer::m_nCopies%3D1F73CB039C.attr

      //## Attribute: m_function%3D1F748300D0
      //	Recipe for the transformation; specifically, a
      //	Transform-valued m_function of a single variable.
      //## begin GeoSerialTransformer::m_function%3D1F748300D0.attr preserve=no  private: const GeoXF::Function * {U} 
      const GeoXF::Function *m_function;
      //## end GeoSerialTransformer::m_function%3D1F748300D0.attr

      //## Attribute: m_physVol%3D2089F9009A
      //	The physical volume to be multiply placed.
      //## begin GeoSerialTransformer::m_physVol%3D2089F9009A.attr preserve=no  private: const GeoVPhysVol * {U} 
      const GeoVPhysVol *m_physVol;
      //## end GeoSerialTransformer::m_physVol%3D2089F9009A.attr

    // Additional Private Declarations
      //## begin GeoSerialTransformer%3D1F73570100.private preserve=yes
      //## end GeoSerialTransformer%3D1F73570100.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoSerialTransformer%3D1F73570100.implementation preserve=yes
      //## end GeoSerialTransformer%3D1F73570100.implementation

};

//## begin GeoSerialTransformer%3D1F73570100.postscript preserve=yes
//## end GeoSerialTransformer%3D1F73570100.postscript

// Class GeoSerialTransformer 

//## begin module%3D1F73570100.epilog preserve=yes
//## end module%3D1F73570100.epilog


#endif
