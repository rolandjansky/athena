/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD3E93F00F9.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD3E93F00F9.cm

//## begin module%3CD3E93F00F9.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD3E93F00F9.cp

//## Module: GeoVPhysVol%3CD3E93F00F9; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVPhysVol.h

#ifndef GeoVPhysVol_h
#define GeoVPhysVol_h 1

//## begin module%3CD3E93F00F9.additionalIncludes preserve=no
//## end module%3CD3E93F00F9.additionalIncludes

//## begin module%3CD3E93F00F9.includes preserve=yes
#include "GeoModelKernel/GeoPVLink.h"
#define SMARTLINKS
#ifdef  SMARTLINKS
typedef GeoPVLink PVLink;
typedef GeoPVConstLink PVConstLink;
#else
class GeoVPhysVol;
typedef GeoVPhysVol *PVLink;
typedef const GeoVPhysVol *PVConstLink;
#endif
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/Query.h"
#include <string>
//## end module%3CD3E93F00F9.includes

// GeoLogVol
#include "GeoModelKernel/GeoLogVol.h"
// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3CD3E93F00F9.additionalDeclarations preserve=yes
class GeoVolumeAction;
//## end module%3CD3E93F00F9.additionalDeclarations


//## begin GeoVPhysVol%3CD3E93F00F9.preface preserve=yes
//## end GeoVPhysVol%3CD3E93F00F9.preface

//## Class: GeoVPhysVol%3CD3E93F00F9; Abstract
//	Base Class for Physical Volumes.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoVPhysVol : public GeoGraphNode  //## Inherits: <unnamed>%3CDD39620209
{
  //## begin GeoVPhysVol%3CD3E93F00F9.initialDeclarations preserve=yes
  //## end GeoVPhysVol%3CD3E93F00F9.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoVPhysVol%3CD403D2012C
      //	Constructor for a VPhysVolume
      GeoVPhysVol (const GeoLogVol* LogVol);


    //## Other Operations (specified)
      //## Operation: isShared%3CDE971D0381
      //	Returns true if the physical volume is accessed by more
      //	than one parent.  Should check this before trusting the
      //	parent pointer.
      bool isShared () const;

      //## Operation: indexOf%3CDE97BC00B9
      //	Returns the index of a specific daughter volume.  The
      //	Query class can be used just like an unsigned int, but
      //	it provides and isValid() method to determine whether it
      //	is valid and throws an exception if an invalid value is
      //	blithely used.
      Query<unsigned int> indexOf (PVConstLink daughter) const;

      //## Operation: getParent%3CDE6F5903B1
      //	Gets the parent, if the parent is unique, and otherwise
      //	returns a NULL pointer.
      PVConstLink getParent () const;

      //## Operation: getLogVol%3CDE67950290
      //	Returns the logical volume.
      const GeoLogVol* getLogVol () const;

      //## Operation: getNChildVols%3CDE67540106
      //	Returns the number of child physical volumes.
      virtual unsigned int getNChildVols () const = 0;

      //## Operation: getChildVol%3CDE675F00DA
      //	Returns the ith child volume
      virtual PVConstLink  getChildVol (unsigned int index) const = 0;

      //## Operation: getXToChildVol%3CDE68C103B4
      //	Returns the transform to the ith volume.
      virtual HepGeom::Transform3D getXToChildVol (unsigned int index) const = 0;

      //## Operation: getDefXToChildVol%3CDE6764025E
      //	Returns the default transform to the ith volume.
      virtual HepGeom::Transform3D getDefXToChildVol (unsigned int index) const = 0;

      //## Operation: getNameOfChildVol%3CEB144D020B
      //	Returns the name of the child.
      virtual std::string getNameOfChildVol (unsigned int i) const = 0;

      //## Operation: apply%3CE238BB0078
      //	Applies an action to the volume.  The volume executes
      //	the action and the applies it to the children, either
      //	from the top down or the bottom up, depending upon the
      //	configuration of the action.
      void apply (GeoVolumeAction *action) const;

      //## Operation: dockTo%3DB85BC00025
      //	When a node is added to a parent in the graph, the node
      //	is always notified.  What happens at that time is up to
      //	the node.  Most nodes do not need to do anything.
      //	Some--the alignable transforms in particular--need to
      //	take some actions, such as adding the parent volume to a
      //	list.
      virtual void dockTo (GeoVPhysVol* parent);

      //## Operation: getIdOfChildVol%3E2ADB9902A2
      //	Returns the id of the child.
      virtual Query<int> getIdOfChildVol (unsigned int i) const = 0;

      //## Operation: getNChildVolAndST%3FD32EEF0152
      //	Returns the number of child physical volumes and Serial
      //	Transformers.
      virtual unsigned int getNChildVolAndST () const = 0;

    // Additional Public Declarations
      //## begin GeoVPhysVol%3CD3E93F00F9.public preserve=yes
      virtual HepGeom::Transform3D getX    () const = 0;
      virtual HepGeom::Transform3D getDefX () const = 0;
      virtual unsigned int getNChildNodes() const=0;
      virtual const GeoGraphNode * const *getChildNode (unsigned int i) const=0;
      virtual const GeoGraphNode * const *findChildNode(const GeoGraphNode *n) const=0;
      //## end GeoVPhysVol%3CD3E93F00F9.public

  protected:
    //## Destructor (generated)
      virtual ~GeoVPhysVol();

    // Additional Protected Declarations
      //## begin GeoVPhysVol%3CD3E93F00F9.protected preserve=yes
      //## end GeoVPhysVol%3CD3E93F00F9.protected

  private:
    //## Constructors (generated)
      GeoVPhysVol(const GeoVPhysVol &right);

    //## Assignment Operation (generated)
      GeoVPhysVol & operator=(const GeoVPhysVol &right);

    // Data Members for Class Attributes

      //## Attribute: m_parentPtr%3CDE6ECC02B4
      //	If one parent                ...pointer=PARENT;
      //	If no parent                  ...pointer=NULL.
      //	If more than one parent...pointer=this;
      //## begin GeoVPhysVol::m_parentPtr%3CDE6ECC02B4.attr preserve=no  private: const GeoVPhysVol* {U} 
      const GeoVPhysVol* m_parentPtr;
      //## end GeoVPhysVol::m_parentPtr%3CDE6ECC02B4.attr

    // Additional Private Declarations
      //## begin GeoVPhysVol%3CD3E93F00F9.private preserve=yes
      //## end GeoVPhysVol%3CD3E93F00F9.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::GraphNodes::Volume Component%3CDDA98A0002
      //## Role: GeoVPhysVol::m_logVol%3CDDA98A0324
      //## begin GeoVPhysVol::m_logVol%3CDDA98A0324.role preserve=no  public: GeoLogVol {1 -> 1RHgN}
      const GeoLogVol *m_logVol;
      //## end GeoVPhysVol::m_logVol%3CDDA98A0324.role

    // Additional Implementation Declarations
      //## begin GeoVPhysVol%3CD3E93F00F9.implementation preserve=yes
      //## end GeoVPhysVol%3CD3E93F00F9.implementation

};

//## begin GeoVPhysVol%3CD3E93F00F9.postscript preserve=yes
//## end GeoVPhysVol%3CD3E93F00F9.postscript

// Class GeoVPhysVol 


//## Other Operations (inline)
inline bool GeoVPhysVol::isShared () const
{
  //## begin GeoVPhysVol::isShared%3CDE971D0381.body preserve=yes
  return m_parentPtr == this;
  //## end GeoVPhysVol::isShared%3CDE971D0381.body
}

//## begin module%3CD3E93F00F9.epilog preserve=yes
//## end module%3CD3E93F00F9.epilog


#endif
