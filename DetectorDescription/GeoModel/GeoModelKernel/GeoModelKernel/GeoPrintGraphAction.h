/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3FB018ED0310.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3FB018ED0310.cm

//## begin module%3FB018ED0310.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3FB018ED0310.cp

//## Module: GeoPrintGraphAction%3FB018ED0310; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPrintGraphAction.h

#ifndef GeoPrintGraphAction_h
#define GeoPrintGraphAction_h 1

//## begin module%3FB018ED0310.additionalIncludes preserve=no
//## end module%3FB018ED0310.additionalIncludes

//## begin module%3FB018ED0310.includes preserve=yes
#include <iostream>
//## end module%3FB018ED0310.includes

// GeoNodeAction
#include "GeoModelKernel/GeoNodeAction.h"
//## begin module%3FB018ED0310.additionalDeclarations preserve=yes
//## end module%3FB018ED0310.additionalDeclarations


//## begin GeoPrintGraphAction%3FB018ED0310.preface preserve=yes
//## end GeoPrintGraphAction%3FB018ED0310.preface

//## Class: GeoPrintGraphAction%3FB018ED0310
//	This action prints the node tree.  It can be configured
//	to print a message on the following types of nodes:
//
//	* Transforms
//	* Physical Volumes and Full Physical Volumes.
//	* Transforms
//	* SerialDenominators
//	* SerialTransforms
//	* IdentifierTag
//
//	These must be switched on (using setNotificationState())
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPrintGraphAction : public GeoNodeAction  //## Inherits: <unnamed>%3FB0190202B6
{
  //## begin GeoPrintGraphAction%3FB018ED0310.initialDeclarations preserve=yes
  enum Type { TRANSFORM,
	      VOLUME,
	      NAMETAG,
	      IDENTIFIERTAG};
  //## end GeoPrintGraphAction%3FB018ED0310.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoPrintGraphAction%3FB027EA0056
      //	Constructor for the print graph action.
      GeoPrintGraphAction (std::ostream &o	// Constructor.  Takes an output stream.
      );

    //## Destructor (generated)
      virtual ~GeoPrintGraphAction();


    //## Other Operations (specified)
      //## Operation: handleTransform%3FB027E903A8
      //	Handles a Transform.
      virtual void handleTransform (const GeoTransform *xform);

      //## Operation: handlePhysVol%3FB027E903DA
      //	Handles a physical volume.
      virtual void handlePhysVol (const GeoPhysVol *vol);

      //## Operation: handleFullPhysVol%3FB027EA0024
      //	Handles a physical volume.
      virtual void handleFullPhysVol (const GeoFullPhysVol *vol);

      //## Operation: handleNameTag%3FB027EA0129
      //	Handles a Name Tag.
      virtual void handleNameTag (const GeoNameTag *nameTag);

      //## Operation: handleSerialDenominator%3FB027EA0151
      //	Handles a Serial Denominator.
      virtual void handleSerialDenominator (const GeoSerialDenominator *sD);

      //## Operation: handleSerialTransformer%3FB027EA016F
      //	Handles a Serial Transformer
      virtual void handleSerialTransformer (const GeoSerialTransformer  *sT);

      //## Operation: handleIdentifierTag%3FB027EA0197
      //	Handles an Identifier Tag.
      virtual void handleIdentifierTag (const GeoIdentifierTag *idTag);

      //## Operation: setNotification%3FB029F700BF
      //	Sets the notification state.  Default: everything on.
      void setNotification (Type type, bool state);

    // Additional Public Declarations
      //## begin GeoPrintGraphAction%3FB018ED0310.public preserve=yes
      //## end GeoPrintGraphAction%3FB018ED0310.public

  protected:
    // Additional Protected Declarations
      //## begin GeoPrintGraphAction%3FB018ED0310.protected preserve=yes
      //## end GeoPrintGraphAction%3FB018ED0310.protected

  private:
    //## Constructors (generated)
      GeoPrintGraphAction(const GeoPrintGraphAction &right);

    //## Assignment Operation (generated)
      GeoPrintGraphAction & operator=(const GeoPrintGraphAction &right);


    //## Other Operations (specified)
      //## Operation: indent%3FD385CE0289
      //	Indent the print.
      void indent ();

    // Data Members for Class Attributes

      //## Attribute: m_nameTag%3FB027D0026C
      //	A pointer to a name tag.  If the volume is named.
      //## begin GeoPrintGraphAction::m_nameTag%3FB027D0026C.attr preserve=no  private: const GeoNameTag * {U} NULL
      const GeoNameTag *m_nameTag;
      //## end GeoPrintGraphAction::m_nameTag%3FB027D0026C.attr

      //## Attribute: m_serialDenominator%3FB027D00280
      //	A pointer to a serial denominator.  If one exists.
      //## begin GeoPrintGraphAction::m_serialDenominator%3FB027D00280.attr preserve=no  private: const GeoSerialDenominator * {U} NULL
      const GeoSerialDenominator *m_serialDenominator;
      //## end GeoPrintGraphAction::m_serialDenominator%3FB027D00280.attr

      //## Attribute: m_idTag%3FB027D00294
      //	A pointer to an identifier tag.  If the volume is
      //	identified.
      //## begin GeoPrintGraphAction::m_idTag%3FB027D00294.attr preserve=no  private: const GeoIdentifierTag * {U} NULL
      const GeoIdentifierTag *m_idTag;
      //## end GeoPrintGraphAction::m_idTag%3FB027D00294.attr

      //## Attribute: m_pendingTransformList%3FB027D002A8
      //	List of Pending Transformations.
      //## begin GeoPrintGraphAction::m_pendingTransformList%3FB027D002A8.attr preserve=no  private: std::vector<const GeoTransform *>  {U} 
      std::vector<const GeoTransform *>  m_pendingTransformList;
      //## end GeoPrintGraphAction::m_pendingTransformList%3FB027D002A8.attr

      //## Attribute: m_transformState%3FB02A5E0294
      //	On/off flag for transforms.
      //## begin GeoPrintGraphAction::m_transformState%3FB02A5E0294.attr preserve=no  private: bool {U} true
      bool m_transformState;
      //## end GeoPrintGraphAction::m_transformState%3FB02A5E0294.attr

      //## Attribute: m_volumeState%3FB02A81021C
      //	On/off flag for physical volumes.
      //## begin GeoPrintGraphAction::m_volumeState%3FB02A81021C.attr preserve=no  private: bool {U} true
      bool m_volumeState;
      //## end GeoPrintGraphAction::m_volumeState%3FB02A81021C.attr

      //## Attribute: m_nametagState%3FB02AAA033E
      //	On/off flag for name tags.
      //## begin GeoPrintGraphAction::m_nametagState%3FB02AAA033E.attr preserve=no  private: bool {U} true
      bool m_nametagState;
      //## end GeoPrintGraphAction::m_nametagState%3FB02AAA033E.attr

      //## Attribute: m_serialDenominatorState%3FB02ACB0377
      //	On/off flag for serial denominators.
      //## begin GeoPrintGraphAction::m_serialDenominatorState%3FB02ACB0377.attr preserve=no  private: bool {U} true
      bool m_serialDenominatorState;
      //## end GeoPrintGraphAction::m_serialDenominatorState%3FB02ACB0377.attr

      //## Attribute: m_serialTransformerState%3FB02AE30277
      //	On/off flag for serial transformers.
      //## begin GeoPrintGraphAction::m_serialTransformerState%3FB02AE30277.attr preserve=no  private: bool {U} true
      bool m_serialTransformerState;
      //## end GeoPrintGraphAction::m_serialTransformerState%3FB02AE30277.attr

      //## Attribute: m_identifierState%3FB02AFC018D
      //	On/off flag for identifier tags.
      //## begin GeoPrintGraphAction::m_identifierState%3FB02AFC018D.attr preserve=no  private: bool {U} true
      bool m_identifierState;
      //## end GeoPrintGraphAction::m_identifierState%3FB02AFC018D.attr

      //## Attribute: m_o%3FD3842802EA
      //## begin GeoPrintGraphAction::m_o%3FD3842802EA.attr preserve=no  private: std::ostream & {U} o
      std::ostream &m_o;
      //## end GeoPrintGraphAction::m_o%3FD3842802EA.attr

      //## Attribute: m_indented%3FD384420057
      //	Flag for indent (intially 0)
      //## begin GeoPrintGraphAction::m_indented%3FD384420057.attr preserve=no  private: mutable bool {U} false
      mutable bool m_indented;
      //## end GeoPrintGraphAction::m_indented%3FD384420057.attr

    // Additional Private Declarations
      //## begin GeoPrintGraphAction%3FB018ED0310.private preserve=yes
      //## end GeoPrintGraphAction%3FB018ED0310.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPrintGraphAction%3FB018ED0310.implementation preserve=yes
      //## end GeoPrintGraphAction%3FB018ED0310.implementation

};

//## begin GeoPrintGraphAction%3FB018ED0310.postscript preserve=yes
//## end GeoPrintGraphAction%3FB018ED0310.postscript

// Class GeoPrintGraphAction 

//## begin module%3FB018ED0310.epilog preserve=yes
//## end module%3FB018ED0310.epilog


#endif
