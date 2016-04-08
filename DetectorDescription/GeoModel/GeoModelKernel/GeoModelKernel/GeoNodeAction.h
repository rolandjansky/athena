/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF50A901F3.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF50A901F3.cm

//## begin module%3CDF50A901F3.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDF50A901F3.cp

//## Module: GeoNodeAction%3CDF50A901F3; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoNodeAction.h

#ifndef GeoNodeAction_h
#define GeoNodeAction_h 1

//## begin module%3CDF50A901F3.additionalIncludes preserve=no
//## end module%3CDF50A901F3.additionalIncludes

//## begin module%3CDF50A901F3.includes preserve=yes
#include "GeoModelKernel/GeoGraphNode.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
//## end module%3CDF50A901F3.includes

// GeoNodePath
#include "GeoModelKernel/GeoNodePath.h"
//## begin module%3CDF50A901F3.additionalDeclarations preserve=yes
//## end module%3CDF50A901F3.additionalDeclarations


//## begin GeoNodeAction%3CDF50A901F3.preface preserve=yes
//## end GeoNodeAction%3CDF50A901F3.preface

//## Class: GeoNodeAction%3CDF50A901F3
//	This type of action sees the geometry graph in full
//	detail, including each transformation and property
//	node.  The function of this graph is to represent a
//	simpler tree of positioned volumes.  To visit only those
//	volumes while keeping track of the state (including
//	relative positioning), use the VolumeAction.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoNodeAction 
{
  //## begin GeoNodeAction%3CDF50A901F3.initialDeclarations preserve=yes
public:

  enum Depth
  { ALL_ANCESTORS = -1, SELF = 0, SELF_AND_CHILDREN = 1 };

  //## end GeoNodeAction%3CDF50A901F3.initialDeclarations

  public:
    //## Constructors (generated)
      GeoNodeAction();

    //## Destructor (generated)
      virtual ~GeoNodeAction();


    //## Other Operations (specified)
      //## Operation: handleNode%3CDF578701F8
      //	Handles a Node.
      virtual void handleNode (const GeoGraphNode *);

      //## Operation: handleTransform%3CDF585C01CC
      //	Handles a Transform.
      virtual void handleTransform (const GeoTransform *);

      //## Operation: handlePhysVol%3CDF5952028E
      //	Handles a physical volume.
      virtual void handlePhysVol (const GeoPhysVol *);

      //## Operation: handleFullPhysVol%3CE0C90A020C
      //	Handles a physical volume.
      virtual void handleFullPhysVol (const GeoFullPhysVol *);

      //## Operation: getPath%3CE10FA40340
      //	Returns a pointer to the path object.
      GeoNodePath* getPath ();

      //## Operation: getDepthLimit%3CE1111F024B
      Query<unsigned int>  getDepthLimit ();

      //## Operation: terminate%3CE12B130348
      //	Cause the action to terminate.
      void terminate ();

      //## Operation: shouldTerminate%3CE12B1C00AC
      //	Determine whether the action should terminate.
      bool shouldTerminate () const;

      //## Operation: handleNameTag%3CEA483D005C
      //	Handles a Name Tag.
      virtual void handleNameTag (const GeoNameTag *);

      //## Operation: handleSerialDenominator%3CEA4842008B
      //	Handles a Serial Denominator.
      virtual void handleSerialDenominator (const GeoSerialDenominator *);

      //## Operation: setDepthLimit%3CFB2BE301B2
      //	Sets a depth limit.
      void setDepthLimit (unsigned int limit);

      //## Operation: clearDepthLimit%3CFB2C2A02A5
      //	Clears a depth limit, if any.
      void clearDepthLimit ();

      //## Operation: handleSerialTransformer%3D208C590370
      //	Handles a Serial Transformer
      virtual void handleSerialTransformer (const GeoSerialTransformer  *);

      //## Operation: handleIdentifierTag%3E217DC1036B
      //	Handles an Identifier Tag.
      virtual void handleIdentifierTag (const GeoIdentifierTag *);

    // Additional Public Declarations
      //## begin GeoNodeAction%3CDF50A901F3.public preserve=yes
      virtual void handleSerialIdentifier(const GeoSerialIdentifier *);
      //## end GeoNodeAction%3CDF50A901F3.public
  protected:
    // Additional Protected Declarations
      //## begin GeoNodeAction%3CDF50A901F3.protected preserve=yes
      //## end GeoNodeAction%3CDF50A901F3.protected

  private:
    //## Constructors (generated)
      GeoNodeAction(const GeoNodeAction &right);

    //## Assignment Operation (generated)
      GeoNodeAction & operator=(const GeoNodeAction &right);

    // Data Members for Class Attributes

      //## Attribute: m_depth%3CE100F2009B
      //	A limit may be placed upon the depth to which the action
      //	descends.  0 = self.  1 = self and children.
      //## begin GeoNodeAction::m_depth%3CE100F2009B.attr preserve=no  private: Query<unsigned int> {U} 
      Query<unsigned int> m_depth;
      //## end GeoNodeAction::m_depth%3CE100F2009B.attr

      //## Attribute: m_terminate%3CE12AC4016E
      //	Termination flag; causes an abortion of action execution.
      //## begin GeoNodeAction::m_terminate%3CE12AC4016E.attr preserve=no  private: bool {U} 
 protected:
      bool m_terminate;
 private:
      //## end GeoNodeAction::m_terminate%3CE12AC4016E.attr

    // Additional Private Declarations
      //## begin GeoNodeAction%3CDF50A901F3.private preserve=yes
      //## end GeoNodeAction%3CDF50A901F3.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::Actions::Path management%3CE1095C0031
      //## Role: GeoNodeAction::m_path%3CE1095C026B
      //## begin GeoNodeAction::m_path%3CE1095C026B.role preserve=no  public: GeoNodePath {1 -> 1VHgN}
      GeoNodePath m_path;
      //## end GeoNodeAction::m_path%3CE1095C026B.role

    // Additional Implementation Declarations
      //## begin GeoNodeAction%3CDF50A901F3.implementation preserve=yes
      //## end GeoNodeAction%3CDF50A901F3.implementation

};

//## begin GeoNodeAction%3CDF50A901F3.postscript preserve=yes
//## end GeoNodeAction%3CDF50A901F3.postscript

// Class GeoNodeAction 

//## begin module%3CDF50A901F3.epilog preserve=yes
//## end module%3CDF50A901F3.epilog


#endif
