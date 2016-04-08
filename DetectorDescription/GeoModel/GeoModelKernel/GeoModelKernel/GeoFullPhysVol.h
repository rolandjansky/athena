/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD87550219.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD87550219.cm

//## begin module%3CDD87550219.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDD87550219.cp

//## Module: GeoFullPhysVol%3CDD87550219; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoFullPhysVol.h

#ifndef GeoFullPhysVol_h
#define GeoFullPhysVol_h 1

//## begin module%3CDD87550219.additionalIncludes preserve=no
//## end module%3CDD87550219.additionalIncludes

//## begin module%3CDD87550219.includes preserve=yes
#include <vector>
#include <string>
class GeoLogVol;
//## end module%3CDD87550219.includes

// GeoVFullPhysVol
#include "GeoModelKernel/GeoVFullPhysVol.h"
//## begin module%3CDD87550219.additionalDeclarations preserve=yes
//## end module%3CDD87550219.additionalDeclarations


//## begin GeoFullPhysVol%3CDD87550219.preface preserve=yes
//## end GeoFullPhysVol%3CDD87550219.preface

//## Class: GeoFullPhysVol%3CDD87550219
//	This class represent a physical volume whose absolute
//	default and aligned positions are known and held in
//	cache.
//
//	These pieces should not be part of a subtree which is
//	used as a shared instance.  For if it is, then of course
//	the absolute position of the piece is an absurdity.  The
//	run time error will result as soon as the absolute
//	position is requested.
//
//	These elements may be declared to readout geometry. See
//	the class ReadoutElement.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoFullPhysVol : public GeoVFullPhysVol  //## Inherits: <unnamed>%3CDD8F950126
{
  //## begin GeoFullPhysVol%3CDD87550219.initialDeclarations preserve=yes
  //## end GeoFullPhysVol%3CDD87550219.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoFullPhysVol%3CDD899100DB
      //	Constructor for the full physical volume.
      //## Preconditions:
      //	Constructor for the physical volume.
      GeoFullPhysVol (const GeoLogVol* LogVol);


    //## Other Operations (specified)
      //## Operation: add%3CDD8ADA0138
      //	Adds a Graph Node to the Geometry Graph.
      void add (GeoGraphNode* graphNode);

      //## Operation: getNChildVols%3CDEA0B70318
      //	Returns the number of child physical volumes.
      virtual unsigned int getNChildVols () const;

      //## Operation: getChildVol%3CDEA0B70336
      //	Returns the ith child volume
      virtual PVConstLink  getChildVol (unsigned int index) const;

      //## Operation: getXToChildVol%3CDEA0B70354
      //	Returns the transform to the ith volume.
      virtual HepGeom::Transform3D getXToChildVol (unsigned int index) const;

      //## Operation: getDefXToChildVol%3CDEA0B70372
      //	Returns the default transform to the ith volume.
      virtual HepGeom::Transform3D getDefXToChildVol (unsigned int index) const;

      //## Operation: exec%3CE0D088000B
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

      //## Operation: getNameOfChildVol%3CEB1777018C
      //	Returns the name of the child.
      virtual std::string getNameOfChildVol (unsigned int i) const;

      //## Operation: getIdOfChildVol%3E2AE8120232
      //	Returns the id of the child.
      virtual Query<int> getIdOfChildVol (unsigned int i) const;

      //## Operation: getNChildVolAndST%3FD32F5201B9
      //	Returns the number of child physical volumes and Serial
      //	Transformers.
      virtual unsigned int getNChildVolAndST () const;

    // Additional Public Declarations
      //## begin GeoFullPhysVol%3CDD87550219.public preserve=yes
      GeoFullPhysVol* clone(bool attached = true);
      const GeoFullPhysVol* cloneOrigin() const;
      // The following method breaks consistency of cloned volumes!
      // Use it only in Simulation jobs and
      // don't call it until geometry has been completely translated to G4
      void clear(); // drop subtree
      virtual HepGeom::Transform3D getX    () const;
      virtual HepGeom::Transform3D getDefX () const;
      virtual unsigned int getNChildNodes() const;
      virtual const GeoGraphNode * const *getChildNode (unsigned int i) const;
      virtual const GeoGraphNode * const *findChildNode(const GeoGraphNode *n) const;
      //## end GeoFullPhysVol%3CDD87550219.public

  protected:
    //## Destructor (generated)
      virtual ~GeoFullPhysVol();

    // Additional Protected Declarations
      //## begin GeoFullPhysVol%3CDD87550219.protected preserve=yes
      //## end GeoFullPhysVol%3CDD87550219.protected

  private:
    //## Constructors (generated)
      GeoFullPhysVol(const GeoFullPhysVol &right);

    //## Assignment Operation (generated)
      GeoFullPhysVol & operator=(const GeoFullPhysVol &right);

    // Data Members for Class Attributes

      //## Attribute: m_daughters%3CDD8A9F02CE
      //	Hold the list of children.
      //## begin GeoFullPhysVol::m_daughters%3CDD8A9F02CE.attr preserve=no  private: std::vector<const GeoGraphNode *> {U} 
      std::vector<const GeoGraphNode *> m_daughters;
      //## end GeoFullPhysVol::m_daughters%3CDD8A9F02CE.attr

    // Additional Private Declarations
      //## begin GeoFullPhysVol%3CDD87550219.private preserve=yes
      const GeoFullPhysVol* m_cloneOrigin;
      //## end GeoFullPhysVol%3CDD87550219.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoFullPhysVol%3CDD87550219.implementation preserve=yes
      //## end GeoFullPhysVol%3CDD87550219.implementation

};

//## begin GeoFullPhysVol%3CDD87550219.postscript preserve=yes
//## end GeoFullPhysVol%3CDD87550219.postscript

// Class GeoFullPhysVol 

//## begin module%3CDD87550219.epilog preserve=yes
//## end module%3CDD87550219.epilog


#endif
