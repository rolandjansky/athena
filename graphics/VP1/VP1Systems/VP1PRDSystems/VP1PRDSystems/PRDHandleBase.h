/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLEBASE_H
#define PRDHANDLEBASE_H

//Fixme: check and doublecheck how to reduce memory used per prd (also includes maps in collhandle).

#include "VP1PRDSystems/PRDSysCommonData.h"

#include "GeoModelKernel/GeoPVConstLink.h"

//#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <QFlags>
#include <QStringList>




class PRDCollHandleBase;
class SoMaterial;
class SoNode;
class SoTransform;
class SoSeparator;

namespace Trk {
  class PrepRawData;
}

class TracksAndSegments;

class PRDHandleBase {
public:

  PRDHandleBase(PRDCollHandleBase*);
  virtual ~PRDHandleBase();

  void setVisible(bool);//use by the collection handle.
  bool visible() const { return m_visible; }
  virtual void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed) = 0;
  virtual int regionIndex() { return 0; }//prds near each other should return same index (used for optimal LOD usage in AUTO detail mode) (only called once/event)
  virtual bool highLight() const { return false; }//Return true to get hit highlighted (for e.g. a TRT hits with HT).
                                            //Outlier highlighting overrides this and is handled in the base classes.
  virtual bool transformUsesSurfacePositionOnly() const { return false; }//Override and return true if should use just the surface position.

  //Return current shapes (0 if not built at the moment):
  SoSeparator* sepSimple() const;
  SoSeparator* sepDetailed() const;

  PRDSysCommonData * common() const { return m_common; }

  virtual Amg::Vector3D center() const;//Default impl. returns positionPRD()
  Amg::Vector3D positionPRD() const;// The center of the getPRD()
  Amg::Vector3D positionSecondPRD() const;//The center of the getSecondPRD(). returns (0,0,0) if not available.

  //For PRDCollHandleBase only:
  Amg::Transform3D getTransform_CLHEP() const;
  virtual SoTransform * createTransform() const;
  void update3DObjects();//Call whenever some setting changes in a way which requires changes to the shape of the 3D representation of the prd.
  void updateMaterial();

  virtual QStringList clicked() const;//Called when user selects the node. The returned strings will be displayed in the textbox.
  //Reimplement the next two for muon prds:
  virtual bool inMuonChamber() const { return false; }
  virtual GeoPVConstLink parentMuonChamberPV() const { return GeoPVConstLink(); }

  PRDCollHandleBase * collHandle() const;

  virtual const Trk::PrepRawData * getPRD() const = 0;//Reimplement to allow access to prd pointer in a generic way.
  virtual const Trk::PrepRawData * getSecondPRD() const { return 0; }//For SCT spacepoints

  virtual bool isSane() const {return true;} //!< Returns false if the PRD is not safe to draw
private:

  // Illegal to copy/assign a PRDHandleBase:
  PRDHandleBase(const PRDHandleBase & );
  PRDHandleBase & operator= (const PRDHandleBase & );

  class Imp;
  Imp * m_d;
  PRDSysCommonData * m_common;
//   inline void registerTransform(SoTransform*);
  bool m_visible;//Here for inlining

};

#endif
