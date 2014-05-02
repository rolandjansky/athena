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

      PVConstLink                           _parent;
      PVConstLink                           _volume;
      HepGeom::Transform3D                        _transform;
      HepGeom::Transform3D                        _defTransform;

      unsigned int                          _majorIndex;
      unsigned int                          _minorIndex;
      unsigned int                          _minorLimit;
      const GeoSerialTransformer           *_serialTransformer;

      const GeoNameTag                     *_nameTag;
      const GeoSerialDenominator           *_serialDenominator;
      const GeoIdentifierTag               *_idTag;
      std::vector<const GeoTransform *>     _pendingTransformList;
      unsigned int                          _serialDenomPosition;
      const GeoSerialIdentifier            *_serialIdentifier;
      unsigned int                          _serialIdentPosition;
      unsigned int                          _volCount;

      bool                                  _hasAlignTrans;
};

#endif
