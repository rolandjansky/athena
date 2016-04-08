/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoVolumeCursor_h
#define GeoVolumeCursor_h 1
#include <vector>
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoNodeAction.h"

class GeoVolumeCursor:public GeoNodeAction

{

  public:

      // Constructor 
      GeoVolumeCursor (PVConstLink parent);

      // Destructor 
      virtual ~GeoVolumeCursor();

      // Advance
      void next();

      // Check if we are at the last volume.
      bool atEnd() const;

      // Returns the selected physical volume or NULL if not found.
      PVConstLink getVolume () const;

      // Returns the transformation to the volume.
      HepGeom::Transform3D getTransform () const;

      // Returns the default ransformation to the volume.
      HepGeom::Transform3D getDefTransform () const;

      // Returns the name of the volume.  From nametag or serial denominator.
      std::string getName () const;
      
      // Returns the id of the volume.  From the identifier.
      Query<int> getId () const;

      bool hasAlignableTransform() const;

 private:

      // Handles a Transform.
      virtual void handleTransform (const GeoTransform *xform);

      // Handles a physical volume.
      virtual void handlePhysVol (const GeoPhysVol *vol);

      // Handles a physical volume.
      virtual void handleFullPhysVol (const GeoFullPhysVol *vol);

      // Handles a Name Tag.
      virtual void handleNameTag (const GeoNameTag *nameTag);

      // Handles a Serial Denominator.
      virtual void handleSerialDenominator (const GeoSerialDenominator *sD);

      // Handles a Serial Transformer
      virtual void handleSerialTransformer (const GeoSerialTransformer  *sT);

      // Handles an Identifier Tag.
      virtual void handleIdentifierTag (const GeoIdentifierTag *idTag);

      // Handles a Serial Identifier.
      virtual void handleSerialIdentifier(const GeoSerialIdentifier *sI);

      // Ressucitate (undo terminate)
      void resuscitate();

  private:

      //## Constructors (generated)
      GeoVolumeCursor(const GeoVolumeCursor &right);

      //## Assignment Operation (generated)
      GeoVolumeCursor & operator=(const GeoVolumeCursor &right);

      PVConstLink                           m_parent;
      PVConstLink                           m_volume;
      HepGeom::Transform3D                        m_transform;
      HepGeom::Transform3D                        m_defTransform;

      unsigned int                          m_majorIndex;
      unsigned int                          m_minorIndex;
      unsigned int                          m_minorLimit;
      const GeoSerialTransformer           *m_serialTransformer;

      const GeoNameTag                     *m_nameTag;
      const GeoSerialDenominator           *m_serialDenominator;
      const GeoIdentifierTag               *m_idTag;
      std::vector<const GeoTransform *>     m_pendingTransformList;
      unsigned int                          m_serialDenomPosition;
      const GeoSerialIdentifier            *m_serialIdentifier;
      unsigned int                          m_serialIdentPosition;
      unsigned int                          m_volCount;

      bool                                  m_hasAlignTrans;
};

#endif
