/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF52C70131.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF52C70131.cm

//## begin module%3CDF52C70131.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDF52C70131.cp

//## Module: GeoTraversalState%3CDF52C70131; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTraversalState.h

#ifndef GeoTraversalState_h
#define GeoTraversalState_h 1

//## begin module%3CDF52C70131.additionalIncludes preserve=no
//## end module%3CDF52C70131.additionalIncludes

//## begin module%3CDF52C70131.includes preserve=yes
#include "CLHEP/Geometry/Transform3D.h"
#include <stack>
#include <string>
//## end module%3CDF52C70131.includes

// GeoNodePath
#include "GeoModelKernel/GeoNodePath.h"
//## begin module%3CDF52C70131.additionalDeclarations preserve=yes
//## end module%3CDF52C70131.additionalDeclarations


//## begin GeoTraversalState%3CDF52C70131.preface preserve=yes
//## end GeoTraversalState%3CDF52C70131.preface

//## Class: GeoTraversalState%3CDF52C70131
//	This incorporates a traversal state, including the
//	following information:
//
//	* The absolute transform from the place where the action
//	started.
//
//	* The default absolute transform from the place where
//	the action started.
//
//	* The path to the node.
//
//	* The depth
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoTraversalState 
{
  //## begin GeoTraversalState%3CDF52C70131.initialDeclarations preserve=yes
  //## end GeoTraversalState%3CDF52C70131.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoTraversalState%3CE2368201BA
      GeoTraversalState ();

    //## Destructor (generated)
      virtual ~GeoTraversalState();


    //## Other Operations (specified)
      //## Operation: getTransform%3CE20A810113
      //	Gets the transformation of the current node with respect
      //	to its parent.
      const HepGeom::Transform3D & getTransform () const;

      //## Operation: getName%3CEB1B1C0365
      //	Gets the name of the current node.
      const std::string & getName () const;

      //## Operation: getDefTransform%3CE20C070218
      //	Gets the default transformation of the current node with
      //	respect to its parent.
      const HepGeom::Transform3D & getDefTransform () const;

      //## Operation: getAbsoluteName%3CEB1B4E017C
      //	Gets the absolute name of the current node.
      const std::string & getAbsoluteName () const;

      //## Operation: getDefAbsoluteTransform%3CE20A9001B5
      //	Gets the default absolute transformation to the current
      //	node.
      const HepGeom::Transform3D & getDefAbsoluteTransform () const;

      //## Operation: getAbsoluteTransform%3CE20C0701F0
      //	Gets the absolute transformation to the current node.
      const HepGeom::Transform3D & getAbsoluteTransform () const;

      //## Operation: setTransform%3CE21D9601BC
      //	Sets the transform for the current node.
      void setTransform (const HepGeom::Transform3D &transform);

      //## Operation: setName%3CEB1D6300B2
      //	Sets the name for the current node.
      void setName (const std::string &name);

      //## Operation: setDefTransform%3CE21DC501E2
      //	Sets the default absolute transform for the current node.
      void setDefTransform (const HepGeom::Transform3D &transform);

      //## Operation: nextLevel%3CE21DDB0392
      //	Goes to the next level.  Pushes the last absolute
      //	transform onto the stack so that it can be retreived
      //	when going back to the previous level.
      void nextLevel (const GeoVPhysVol* pv);

      //## Operation: previousLevel%3CE21F1C011D
      //	Goes to the previous level.  Pops the last absolute
      //	transform onto the stack, restoring the absolute
      //	transform to that of the previous level.
      void previousLevel ();

      //## Operation: getPath%3CE22E2F0120
      //	Returns the path.
      const GeoNodePath * getPath () const;

      //## Operation: setId%3E2AD91A0290
      //	Sets the identifier for the current node.
      void setId (const Query<int> &id);

      //## Operation: getId%3E2AD9380289
      //	Gets the id of the current node.
      const Query<int> getId () const;

    // Additional Public Declarations
      //## begin GeoTraversalState%3CDF52C70131.public preserve=yes
      //## end GeoTraversalState%3CDF52C70131.public

  protected:
    // Additional Protected Declarations
      //## begin GeoTraversalState%3CDF52C70131.protected preserve=yes
      //## end GeoTraversalState%3CDF52C70131.protected

  private:
    //## Constructors (generated)
      GeoTraversalState(const GeoTraversalState &right);

    //## Assignment Operation (generated)
      GeoTraversalState & operator=(const GeoTraversalState &right);

    // Data Members for Class Attributes

      //## Attribute: m_absTransformList%3CE20A5803BD
      //	A list of tranformations for all nodes visited at all
      //	previous levels of traversal.
      //## begin GeoTraversalState::m_absTransformList%3CE20A5803BD.attr preserve=no  private: std::stack<HepGeom::Transform3D> {U} 
      std::stack<HepGeom::Transform3D> m_absTransformList;
      //## end GeoTraversalState::m_absTransformList%3CE20A5803BD.attr

      //## Attribute: m_defAbsTransformList%3CE20A5E039E
      //	A list of default tranformations for all nodes visited
      //	at all previous levels of traversal.
      //## begin GeoTraversalState::m_defAbsTransformList%3CE20A5E039E.attr preserve=no  private: std::stack<HepGeom::Transform3D> {U} 
      std::stack<HepGeom::Transform3D> m_defAbsTransformList;
      //## end GeoTraversalState::m_defAbsTransformList%3CE20A5E039E.attr

      //## Attribute: m_absNameList%3CEB1A08019B
      //	List of volume names.
      //## begin GeoTraversalState::m_absNameList%3CEB1A08019B.attr preserve=no  private: std::stack<std::string> {U} 
      std::stack<std::string> m_absNameList;
      //## end GeoTraversalState::m_absNameList%3CEB1A08019B.attr

      //## Attribute: m_absTransform%3CE21EAF004E
      //	The absolute transform for the present volume.
      //## begin GeoTraversalState::m_absTransform%3CE21EAF004E.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_absTransform;
      //## end GeoTraversalState::m_absTransform%3CE21EAF004E.attr

      //## Attribute: m_defAbsTransform%3CE21EB60044
      //	The default absolute transform.
      //## begin GeoTraversalState::m_defAbsTransform%3CE21EB60044.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_defAbsTransform;
      //## end GeoTraversalState::m_defAbsTransform%3CE21EB60044.attr

      //## Attribute: m_absName%3CEB1AA90279
      //	The absolute name.
      //## begin GeoTraversalState::m_absName%3CEB1AA90279.attr preserve=no  private: std::string {U} 
      std::string m_absName;
      //## end GeoTraversalState::m_absName%3CEB1AA90279.attr

      //## Attribute: m_transform%3CE2309B030C
      //	The transform from parent to current.
      //## begin GeoTraversalState::m_transform%3CE2309B030C.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_transform;
      //## end GeoTraversalState::m_transform%3CE2309B030C.attr

      //## Attribute: m_defTransform%3CE230BA0126
      //	The default transform from parent to current.
      //## begin GeoTraversalState::m_defTransform%3CE230BA0126.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_defTransform;
      //## end GeoTraversalState::m_defTransform%3CE230BA0126.attr

      //## Attribute: m_name%3CEB1ACE0005
      //	The relative name.
      //## begin GeoTraversalState::m_name%3CEB1ACE0005.attr preserve=no  private: std::string {U} 
      std::string m_name;
      //## end GeoTraversalState::m_name%3CEB1ACE0005.attr

      //## Attribute: m_id%3E2AD8EB02EC
      //	And identifier for this volume.
      //## begin GeoTraversalState::m_id%3E2AD8EB02EC.attr preserve=no  private: Query<int> {U} 
      Query<int> m_id;
      //## end GeoTraversalState::m_id%3E2AD8EB02EC.attr

    // Additional Private Declarations
      //## begin GeoTraversalState%3CDF52C70131.private preserve=yes
      //## end GeoTraversalState%3CDF52C70131.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::Actions::Path management%3CE204AE007E
      //## Role: GeoTraversalState::path%3CE204AF03AC
      //	The path from the point at which the action started, to
      //	the current node.
      //## begin GeoTraversalState::path%3CE204AF03AC.role preserve=no  public: GeoNodePath {1 -> 1VHgN}
      GeoNodePath m_path;
      //## end GeoTraversalState::path%3CE204AF03AC.role

    // Additional Implementation Declarations
      //## begin GeoTraversalState%3CDF52C70131.implementation preserve=yes
      //## end GeoTraversalState%3CDF52C70131.implementation

};

//## begin GeoTraversalState%3CDF52C70131.postscript preserve=yes
//## end GeoTraversalState%3CDF52C70131.postscript

// Class GeoTraversalState 

//## begin module%3CDF52C70131.epilog preserve=yes
//## end module%3CDF52C70131.epilog


#endif
