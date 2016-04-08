/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD3E9AB0054.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD3E9AB0054.cm

//## begin module%3CD3E9AB0054.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD3E9AB0054.cp

//## Module: GeoPhysVol%3CD3E9AB0054; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPhysVol.h

#ifndef GeoPhysVol_h
#define GeoPhysVol_h 1

//## begin module%3CD3E9AB0054.additionalIncludes preserve=no
//## end module%3CD3E9AB0054.additionalIncludes

//## begin module%3CD3E9AB0054.includes preserve=yes
//## end module%3CD3E9AB0054.includes

// GeoVPhysVol
#include "GeoModelKernel/GeoVPhysVol.h"
//## begin module%3CD3E9AB0054.additionalDeclarations preserve=yes
//## end module%3CD3E9AB0054.additionalDeclarations


//## begin GeoPhysVol%3CD3E9AB0054.preface preserve=yes
//## end GeoPhysVol%3CD3E9AB0054.preface

//## Class: GeoPhysVol%3CD3E9AB0054
//	A GeoPhysVol is a physical volume for normal recursive
//	access only.  This means, normally, that it represents a
//	piece of detector that nobody cares about except for
//	tracing engines like GEANT.  More specifically:
//
//
//	*  The position of the physical volume is not kept with
//	the physical volume.  It can easily be obtained by
//	traversing detector graph (ComputeTransformAction), but
//	it cannot be obtained in constant time.  If you need to
//	know the absolute default and/or aligned position of the
//	detector, you need to use a different kind of physical
//	volume called a GeoRAPhysicalVolume, which has a higher
//	memory cost.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPhysVol : public GeoVPhysVol  //## Inherits: <unnamed>%3CD3E9B50315
{
  //## begin GeoPhysVol%3CD3E9AB0054.initialDeclarations preserve=yes
  //## end GeoPhysVol%3CD3E9AB0054.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoPhysVol%3CD4040202A8
      //	Constructor.
      GeoPhysVol (const GeoLogVol* LogVol);


    //## Other Operations (specified)
      //## Operation: add%3CDD8AD20263
      //	Adds a Graph Node to the Geometry Graph.
      void add (GeoGraphNode* graphNode);

      //## Operation: getNChildVols%3CDEB5690059
      //	Returns the number of child physical volumes.
      virtual unsigned int getNChildVols () const;

      //## Operation: getChildVol%3CDEB5690077
      //	Returns the ith child volume
      virtual PVConstLink  getChildVol (unsigned int index) const;

      //## Operation: getXToChildVol%3CDEB5690095
      //	Returns the transform to the ith volume.
      virtual HepGeom::Transform3D getXToChildVol (unsigned int index) const;

      //## Operation: getDefXToChildVol%3CDEB56900B3
      //	Returns the default transform to the ith volume.
      virtual HepGeom::Transform3D getDefXToChildVol (unsigned int index) const;

      //## Operation: exec%3CE0D091019F
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

      //## Operation: getNameOfChildVol%3CEB17A0024A
      //	Returns the name of the child.
      virtual std::string getNameOfChildVol (unsigned int i) const;

      //## Operation: getIdOfChildVol%3E2ADC070296
      //	Returns the id of the child.
      virtual Query<int> getIdOfChildVol (unsigned int i) const;

      //## Operation: getNChildVolAndST%3FD32F39000A
      //	Returns the number of child physical volumes and Serial
      //	Transformers.
      virtual unsigned int getNChildVolAndST () const;

    // Additional Public Declarations
      //## begin GeoPhysVol%3CD3E9AB0054.public preserve=yes
      virtual HepGeom::Transform3D getX    () const;
      virtual HepGeom::Transform3D getDefX () const;
      virtual unsigned int getNChildNodes() const;
      virtual const GeoGraphNode * const *getChildNode (unsigned int i) const;
      virtual const GeoGraphNode * const *findChildNode(const GeoGraphNode *n) const;
   //## end GeoPhysVol%3CD3E9AB0054.public

  protected:
    //## Destructor (generated)
      virtual ~GeoPhysVol();

    // Additional Protected Declarations
      //## begin GeoPhysVol%3CD3E9AB0054.protected preserve=yes
      //## end GeoPhysVol%3CD3E9AB0054.protected

  private:
    //## Constructors (generated)
      GeoPhysVol(const GeoPhysVol &right);

    //## Assignment Operation (generated)
      GeoPhysVol & operator=(const GeoPhysVol &right);

    // Data Members for Class Attributes

      //## Attribute: _daughters%3CDD8A84033D
      //	Hold the list of children.
      //## begin GeoPhysVol::m_daughters%3CDD8A84033D.attr preserve=no  private: std::vector<const GeoGraphNode *> {U} 
      std::vector<const GeoGraphNode *> m_daughters;
      //## end GeoPhysVol::m_daughters%3CDD8A84033D.attr

    // Additional Private Declarations
      //## begin GeoPhysVol%3CD3E9AB0054.private preserve=yes
      //## end GeoPhysVol%3CD3E9AB0054.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPhysVol%3CD3E9AB0054.implementation preserve=yes
      //## end GeoPhysVol%3CD3E9AB0054.implementation

};

//## begin GeoPhysVol%3CD3E9AB0054.postscript preserve=yes
//## end GeoPhysVol%3CD3E9AB0054.postscript

// Class GeoPhysVol 

//## begin module%3CD3E9AB0054.epilog preserve=yes
//## end module%3CD3E9AB0054.epilog


#endif
