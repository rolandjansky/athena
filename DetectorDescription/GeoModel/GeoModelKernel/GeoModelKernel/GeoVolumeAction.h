/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF51210227.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF51210227.cm

//## begin module%3CDF51210227.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDF51210227.cp

//## Module: GeoVolumeAction%3CDF51210227; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVolumeAction.h

#ifndef GeoVolumeAction_h
#define GeoVolumeAction_h 1

//## begin module%3CDF51210227.additionalIncludes preserve=no
//## end module%3CDF51210227.additionalIncludes

//## begin module%3CDF51210227.includes preserve=yes
//## end module%3CDF51210227.includes

// GeoTraversalState
#include "GeoModelKernel/GeoTraversalState.h"
//## begin module%3CDF51210227.additionalDeclarations preserve=yes
//## end module%3CDF51210227.additionalDeclarations


//## begin GeoVolumeAction%3CDF51210227.preface preserve=yes
//## end GeoVolumeAction%3CDF51210227.preface

//## Class: GeoVolumeAction%3CDF51210227; Abstract
//	This type of action sees and traverses a simpler tree of
//	positioned volumes.  It does not descend into
//	transformations or other properties, but simply
//	accumulates their effect into an internal state that can
//	be accessed during traversal.  That state includes, for
//	example, the position relative to the parent.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoVolumeAction 
{
  //## begin GeoVolumeAction%3CDF51210227.initialDeclarations preserve=yes
public:
  enum Type
  { TOP_DOWN, BOTTOM_UP };
  //## end GeoVolumeAction%3CDF51210227.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoVolumeAction%3CE234DF02D0
      //	Constructor.
      GeoVolumeAction (Type type = TOP_DOWN);

    //## Destructor (generated)
      virtual ~GeoVolumeAction();


    //## Other Operations (specified)
      //## Operation: handleVPhysVol%3CE2033201E1
      //	Callback method. Overriden by users.
      virtual void handleVPhysVol (const GeoVPhysVol *);

      //## Operation: terminate%3CE2046003DA
      //	Cause the action to terminate.
      void terminate ();

      //## Operation: shouldTerminate%3CE20461001A
      //	Determine whether the action should terminate.
      bool shouldTerminate () const;

      //## Operation: getState%3CE23D830046
      //	Returns the traversal state.  From this one can get
      //	placement information about the volume, even if it is a
      //	shared instance.
      const GeoTraversalState* getState () const;

      //## Operation: getState%3CE204D803A0
      //	Returns the traversal state, as above.  Non const type.
      GeoTraversalState* getState ();

      //## Operation: getType%3CE23408014B
      //	Returns the type: GeoVolumeAction::TOP_DOWN or GeoVolume
      //	Action::BOTTOM_UP.
      GeoVolumeAction::Type getType () const;

    // Additional Public Declarations
      //## begin GeoVolumeAction%3CDF51210227.public preserve=yes
      //## end GeoVolumeAction%3CDF51210227.public

  protected:
    // Additional Protected Declarations
      //## begin GeoVolumeAction%3CDF51210227.protected preserve=yes
      //## end GeoVolumeAction%3CDF51210227.protected

  private:
    //## Constructors (generated)
      GeoVolumeAction(const GeoVolumeAction &right);

    //## Assignment Operation (generated)
      GeoVolumeAction & operator=(const GeoVolumeAction &right);

    // Data Members for Class Attributes

      //## Attribute: m_type%3CE202D10046
      //	This type specifies that the action should be carried
      //	out from the bottom up or from the top down.
      //## begin GeoVolumeAction::m_type%3CE202D10046.attr preserve=no  private: Type {U} type
      Type m_type;
      //## end GeoVolumeAction::m_type%3CE202D10046.attr

      //## Attribute: m_terminate%3CE236D401E0
      //	Termination flag; causes an abortion of action execution.
      //## begin GeoVolumeAction::m_terminate%3CE236D401E0.attr preserve=no  private: bool {U} false
      bool m_terminate;
      //## end GeoVolumeAction::m_terminate%3CE236D401E0.attr

    // Additional Private Declarations
      //## begin GeoVolumeAction%3CDF51210227.private preserve=yes
      //## end GeoVolumeAction%3CDF51210227.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::Actions::State management%3CDF52ED0335
      //## Role: GeoVolumeAction::m_traversalState%3CDF52EE028C
      //## begin GeoVolumeAction::m_traversalState%3CDF52EE028C.role preserve=no  public: GeoTraversalState {1 -> 1VHgN}
      GeoTraversalState m_traversalState;
      //## end GeoVolumeAction::m_traversalState%3CDF52EE028C.role

    // Additional Implementation Declarations
      //## begin GeoVolumeAction%3CDF51210227.implementation preserve=yes
      //## end GeoVolumeAction%3CDF51210227.implementation

};

//## begin GeoVolumeAction%3CDF51210227.postscript preserve=yes
//## end GeoVolumeAction%3CDF51210227.postscript

// Class GeoVolumeAction 

//## begin module%3CDF51210227.epilog preserve=yes
//## end module%3CDF51210227.epilog


#endif
