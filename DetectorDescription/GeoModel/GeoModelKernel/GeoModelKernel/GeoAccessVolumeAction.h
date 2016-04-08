/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CE11DD8028A.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CE11DD8028A.cm

//## begin module%3CE11DD8028A.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CE11DD8028A.cp

//## Module: GeoAccessVolumeAction%3CE11DD8028A; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoAccessVolumeAction.h

#ifndef GeoAccessVolumeAction_h
#define GeoAccessVolumeAction_h 1

//## begin module%3CE11DD8028A.additionalIncludes preserve=no
//## end module%3CE11DD8028A.additionalIncludes

//## begin module%3CE11DD8028A.includes preserve=yes
#include <vector>
#include "CLHEP/Geometry/Transform3D.h"
//## end module%3CE11DD8028A.includes

// GeoNodeAction
#include "GeoModelKernel/GeoNodeAction.h"
//## begin module%3CE11DD8028A.additionalDeclarations preserve=yes
class GeoVPhysVol;
class GeoTransform;
//## end module%3CE11DD8028A.additionalDeclarations


//## begin GeoAccessVolumeAction%3CE11DD8028A.preface preserve=yes
//## end GeoAccessVolumeAction%3CE11DD8028A.preface

//## Class: GeoAccessVolumeAction%3CE11DD8028A
//	Accesses volumes, returning the ith volume and also its
//	transformation relative to the parent.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoAccessVolumeAction : public GeoNodeAction  //## Inherits: <unnamed>%3CE11E0602CC
{
  //## begin GeoAccessVolumeAction%3CE11DD8028A.initialDeclarations preserve=yes
  //## end GeoAccessVolumeAction%3CE11DD8028A.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoAccessVolumeAction%3CE1249902FE
      //	Constructor for the access volume action.
      GeoAccessVolumeAction (unsigned int Index);

    //## Destructor (generated)
      virtual ~GeoAccessVolumeAction();


    //## Other Operations (specified)
      //## Operation: handleTransform%3CE121B602D4
      //	Handles a Transform.
      virtual void handleTransform (const GeoTransform *xform);

      //## Operation: handlePhysVol%3CE12437022B
      //	Handles a physical volume.
      virtual void handlePhysVol (const GeoPhysVol *vol);

      //## Operation: handleFullPhysVol%3CE12437023F
      //	Handles a physical volume.
      virtual void handleFullPhysVol (const GeoFullPhysVol *vol);

      //## Operation: getVolume%3CE124F50044
      //	Returns the selected physical volume or NULL if not
      //	found.
      PVConstLink getVolume () const;

      //## Operation: getTransform%3CE1257A02B2
      //	Returns the transformation to the volume.
      const HepGeom::Transform3D & getTransform () const;

      //## Operation: getDefTransform%3CE1317A0331
      //	Returns the default ransformation to the volume.
      const HepGeom::Transform3D & getDefTransform () const;

      //## Operation: getName%3CEB0CE5029B
      //	Returns the name of the volume.  From nametag or serial
      //	denominator.
      const std::string & getName () const;

      //## Operation: handleNameTag%3CEB0D370018
      //	Handles a Name Tag.
      virtual void handleNameTag (const GeoNameTag *nameTag);

      //## Operation: handleSerialDenominator%3CEB0D370040
      //	Handles a Serial Denominator.
      virtual void handleSerialDenominator (const GeoSerialDenominator *sD);

      //## Operation: handleSerialTransformer%3D208CCE030A
      //	Handles a Serial Transformer
      virtual void handleSerialTransformer (const GeoSerialTransformer  *sT);

      //## Operation: handleIdentifierTag%3E217F0E0298
      //	Handles an Identifier Tag.
      virtual void handleIdentifierTag (const GeoIdentifierTag *idTag);

      //## Operation: getId%3E217F5C0268
      //	Returns the id of the volume.  From the identifier.
      Query<int> getId () const;

    // Additional Public Declarations
      //## begin GeoAccessVolumeAction%3CE11DD8028A.public preserve=yes
      virtual void handleSerialIdentifier(const GeoSerialIdentifier *sI);
      //## end GeoAccessVolumeAction%3CE11DD8028A.public
  protected:
    // Additional Protected Declarations
      //## begin GeoAccessVolumeAction%3CE11DD8028A.protected preserve=yes
      //## end GeoAccessVolumeAction%3CE11DD8028A.protected

  private:
    //## Constructors (generated)
      GeoAccessVolumeAction(const GeoAccessVolumeAction &right);

    //## Assignment Operation (generated)
      GeoAccessVolumeAction & operator=(const GeoAccessVolumeAction &right);

    // Data Members for Class Attributes

      //## Attribute: m_volume%3CE11F8900CA
      //	Returns a pointer to the ith physical volume under this
      //	one.
      //## begin GeoAccessVolumeAction::m_volume%3CE11F8900CA.attr preserve=no  private: PVConstLink {U} 
      PVConstLink m_volume;
      //## end GeoAccessVolumeAction::m_volume%3CE11F8900CA.attr

      //## Attribute: m_transform%3CE1204D0216
      //	The transformation to the ith volume.
      //## begin GeoAccessVolumeAction::m_transform%3CE1204D0216.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_transform;
      //## end GeoAccessVolumeAction::m_transform%3CE1204D0216.attr

      //## Attribute: m_defTransform%3CE1315700E2
      //	The default transformation to the ith volume.
      //## begin GeoAccessVolumeAction::m_defTransform%3CE1315700E2.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_defTransform;
      //## end GeoAccessVolumeAction::m_defTransform%3CE1315700E2.attr

      //## Attribute: m_index%3CE124570082
      //	The volume which we are interested in seeking.
      //## begin GeoAccessVolumeAction::m_index%3CE124570082.attr preserve=no  private: unsigned int {U} 
      unsigned int m_index;
      //## end GeoAccessVolumeAction::m_index%3CE124570082.attr

      //## Attribute: m_counter%3CE12AAE005E
      //	The volume which we are interested in seeking.
      //## begin GeoAccessVolumeAction::m_counter%3CE12AAE005E.attr preserve=no  private: unsigned int {U} 
      unsigned int m_counter;
      //## end GeoAccessVolumeAction::m_counter%3CE12AAE005E.attr

      //## Attribute: m_name%3CEB0C910038
      //	The name of the volume.  From a nametag or a serial
      //	denominator.
      //## begin GeoAccessVolumeAction::m_name%3CEB0C910038.attr preserve=no  private: mutable std::string {U} 
      mutable std::string m_name;
      //## end GeoAccessVolumeAction::m_name%3CEB0C910038.attr

      //## Attribute: _id%3E217FA3027E
      //	The identifier of the volume.  From an identifier tag.
      //## begin GeoAccessVolumeAction::_id%3E217FA3027E.attr preserve=no  private: mutable Query<int> {U} 
      mutable Query<int> m_id;
      //## end GeoAccessVolumeAction::_id%3E217FA3027E.attr

      //## Attribute: m_nameTag%3CEB0F5A02DC
      //	A pointer to a name tag.  If the volume is named.
      //## begin GeoAccessVolumeAction::m_nameTag%3CEB0F5A02DC.attr preserve=no  private: const GeoNameTag * {U} 
      const GeoNameTag *m_nameTag;
      //## end GeoAccessVolumeAction::m_nameTag%3CEB0F5A02DC.attr

      //## Attribute: m_serialDenominator%3CEB0F6300E0
      //	A pointer to a serial denominator.  If one exists.
      //## begin GeoAccessVolumeAction::m_serialDenominator%3CEB0F6300E0.attr preserve=no  private: const GeoSerialDenominator * {U} 
      const GeoSerialDenominator *m_serialDenominator;
      //## end GeoAccessVolumeAction::m_serialDenominator%3CEB0F6300E0.attr

      //## Attribute: m_idTag%3E217FEC0297
      //	A pointer to an identifier tag.  If the volume is
      //	identified.
      //## begin GeoAccessVolumeAction::m_idTag%3E217FEC0297.attr preserve=no  private: const GeoIdentifierTag * {U} 
      const GeoIdentifierTag *m_idTag;
      //## end GeoAccessVolumeAction::m_idTag%3E217FEC0297.attr

      //## Attribute: m_pendingTransformList%3E2ACE98035A
      //	List of Pending Transformations.
      //## begin GeoAccessVolumeAction::m_pendingTransformList%3E2ACE98035A.attr preserve=no  private: std::vector<const GeoTransform *>  {U} 
      std::vector<const GeoTransform *>  m_pendingTransformList;
      //## end GeoAccessVolumeAction::m_pendingTransformList%3E2ACE98035A.attr

      //## Attribute: m_serialDenomPosition%3E2ACF380274
      //	Position of the serial denominator.  Used to assign a
      //	numeric suffix to the name, eg BaseName+99
      //## begin GeoAccessVolumeAction::m_serialDenomPosition%3E2ACF380274.attr preserve=no  private: unsigned int {U} 
      unsigned int m_serialDenomPosition;
      //## end GeoAccessVolumeAction::m_serialDenomPosition%3E2ACF380274.attr

    // Additional Private Declarations
      //## begin GeoAccessVolumeAction%3CE11DD8028A.private preserve=yes
      const GeoSerialIdentifier *m_serialIdentifier;
      unsigned int m_serialIdentPosition;
      //## end GeoAccessVolumeAction%3CE11DD8028A.private
  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoAccessVolumeAction%3CE11DD8028A.implementation preserve=yes
      //## end GeoAccessVolumeAction%3CE11DD8028A.implementation

};

//## begin GeoAccessVolumeAction%3CE11DD8028A.postscript preserve=yes
//## end GeoAccessVolumeAction%3CE11DD8028A.postscript

// Class GeoAccessVolumeAction 

//## begin module%3CE11DD8028A.epilog preserve=yes
//## end module%3CE11DD8028A.epilog


#endif
