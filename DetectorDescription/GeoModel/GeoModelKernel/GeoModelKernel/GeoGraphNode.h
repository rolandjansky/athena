/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD34600238.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD34600238.cm

//## begin module%3CDD34600238.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDD34600238.cp

//## Module: GeoGraphNode%3CDD34600238; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoGraphNode.h

#ifndef GeoGraphNode_h
#define GeoGraphNode_h 1

//## begin module%3CDD34600238.additionalIncludes preserve=no
//## end module%3CDD34600238.additionalIncludes

//## begin module%3CDD34600238.includes preserve=yes
//## end module%3CDD34600238.includes

// RCBase
#include "GeoModelKernel/RCBase.h"
//## begin module%3CDD34600238.additionalDeclarations preserve=yes
class GeoNodeAction;
class GeoVPhysVol;
//## end module%3CDD34600238.additionalDeclarations


//## begin GeoGraphNode%3CDD34600238.preface preserve=yes
//## end GeoGraphNode%3CDD34600238.preface

//## Class: GeoGraphNode%3CDD34600238; Abstract
//	Represents Nodes that can be assembled together to make
//	a graph.  Subclasses (for now):
//
//	* GeoVTransform
//	     *GeoFixedTransform.     .. for fixed transforms
//	     *GeoVariableTransform  .. for variable transforms.
//
//	* GeoVPhysVol
//	     *GeoPhysVol       .. for normal recursive access.
//	     *GeoRAPhysVol  .. for random access
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoGraphNode : public RCBase  //## Inherits: <unnamed>%3CDD34800252
{
  //## begin GeoGraphNode%3CDD34600238.initialDeclarations preserve=yes
  //## end GeoGraphNode%3CDD34600238.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoGraphNode%3CDD42320389
      //	Default constructor.
      GeoGraphNode ();


    //## Other Operations (specified)
      //## Operation: exec%3CDF54C40260
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

      //## Operation: dockTo%3CE3D83D025E
      //	When a node is added to a parent in the graph, the node
      //	is always notified.  What happens at that time is up to
      //	the node.  Most nodes do not need to do anything.
      //	Some--the alignable transforms in particular--need to
      //	take some actions, such as adding the parent volume to a
      //	list.
      virtual void dockTo (GeoVPhysVol* );

    // Additional Public Declarations
      //## begin GeoGraphNode%3CDD34600238.public preserve=yes
      //## end GeoGraphNode%3CDD34600238.public

  protected:
    //## Destructor (generated)
      virtual ~GeoGraphNode();

    // Additional Protected Declarations
      //## begin GeoGraphNode%3CDD34600238.protected preserve=yes
      //## end GeoGraphNode%3CDD34600238.protected

  private:
    //## Constructors (generated)
      GeoGraphNode(const GeoGraphNode &right);

    //## Assignment Operation (generated)
      GeoGraphNode & operator=(const GeoGraphNode &right);

    // Additional Private Declarations
      //## begin GeoGraphNode%3CDD34600238.private preserve=yes
      //## end GeoGraphNode%3CDD34600238.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoGraphNode%3CDD34600238.implementation preserve=yes
      //## end GeoGraphNode%3CDD34600238.implementation

};

//## begin GeoGraphNode%3CDD34600238.postscript preserve=yes
//## end GeoGraphNode%3CDD34600238.postscript

// Class GeoGraphNode 

//## begin module%3CDD34600238.epilog preserve=yes
//## end module%3CDD34600238.epilog


#endif
