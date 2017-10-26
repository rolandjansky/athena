/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandleBase                     //
//                                                            //
//  Description: Base class for track handles.                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLEBASE_H
#define TRACKHANDLEBASE_H

#include "VP1TrackSystems/TrackCommonFlags.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "GeoModelKernel/GeoPVConstLink.h"

//#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <QStringList>
#include <QTreeWidgetItem>
#include <vector>
#include <set>
#include <QList>
#include <QFlags>


class AssocObjAttachmentHandle;
class AssociatedObjectHandleBase;
class TrackCollHandleBase;
class TrackSysCommonData;
class SoMaterial;
class SoNode;
class TrackLODHandle;

namespace Trk {
  class Track;
  class FitQuality;
}

class TrackHandleBase {
public:

  static int numberOfInstances();

  TrackHandleBase(TrackCollHandleBase*);
  virtual ~TrackHandleBase();

  TrackSysCommonData * common() const;

  void setVisible(bool);//!< use by the collection handle.
  bool visible() const { return m_visible; }
  TrackCollHandleBase * collHandle() const { return m_collhandle; }
  const std::set<GeoPVConstLink>& touchedMuonChambers() const;
  void update3DObjects(bool invalidatePropagatedPoints = false, float maxR=0.0);
  void updateInDetProjections();
  void updateMuonProjections();
  void updateMaterial();
  void rerandomiseRandomMaterial();
  void updateRandomColourTransparencyAndBrightness();
  SoMaterial * currentMaterial() const { return m_currentmaterial; }

  QStringList baseInfo() const;
  /** Called when user selects the node (stringlist is displayed in messagebox). 
  Reimplementations should add info momInfo() to get momentum, eta, phi, pid,charge, mass printed.*/
  virtual QStringList clicked() const = 0;

  virtual Amg::Vector3D momentum() const { return Amg::Vector3D(0,0,0); }
  virtual const Amg::Vector3D * startPoint() const;//!< returns 0 if can't find start point.
  virtual const Amg::Vector3D * endPoint() const;//!< returns 0 if can't find start point.
  virtual int pdgCode() const { return 0; }//!< 0 means unknown

  //Default implementation of the following two methods will based on
  //pdgCode(). In the case of charge(), if it can't be determined
  //based on pdgCode(), and attempt will be made by looking at the
  //first track parameters charge (if a Trk::Track is
  //provided). Override in case the charge/mass can be determined, but
  //the pdg code not. (or if there is a faster way to get the charge than by constructing the Trk::Track first).
  //
  //NB: update: it is the calculateCharge() method which must be reimplemented for the charge!
  double charge() const;//Returns unknown() in case of trouble.
  virtual double mass() const;//Returns unknown() in case of trouble.

  bool hasCharge() const { return charge()!=unknown(); }
  bool hasMass() const { return mass()!=unknown(); }

  static double unknown() { return -999.0; }

  virtual bool hasSubSystemHitInfo() const { return false; }
  virtual bool isIDTrack() const ;
  virtual unsigned getNPixelHits() const { return 0; }
  virtual unsigned getNSCTHits() const { return 0; }
  virtual unsigned getNTRTHits() const { return 0; }
  virtual unsigned getNMuonHits() const { return (getNMDTHits()+getNRPCHits()+getNTGCHits()+getNCSCHits()); }
  virtual unsigned getNMuonPhiHits() const { return 0; } //!< The number of phi hits is part of hit counts below (i.e. RPC hits include phi hits), but reported separately due to importance.
  virtual unsigned getNMDTHits() const { return 0; }
  virtual unsigned getNRPCHits() const { return 0; }
  virtual unsigned getNTGCHits() const { return 0; }
  virtual unsigned getNCSCHits() const { return 0; }
  virtual const Trk::FitQuality* getFitQuality() const {return 0;}

  QList<AssociatedObjectHandleBase*>  getVisibleMeasurements() const;
  TrackCommonFlags::TSOSPartsFlags shownTSOSParts() const;
  TrackCommonFlags::TSOSPartsFlags customColouredTSOSParts() const;

  std::vector< Amg::Vector3D > * hackGetPointsPropagated();//!< FIXME. Used to get position of track for use in short tubes etc.
  
  virtual QString shortInfo() const { return QString("Not yet implemented"); } //!< returns mom and hit information about track
  virtual QString type() const { return QString("Not yet implemented"); } //!< return very short word with type (maybe link with collection type?)
  
  // Object browser code
  // @TODO revisit this once plan for more general approach is completed.
  virtual void fillObjectBrowser(QList<QTreeWidgetItem *>& list); //!< Overload in child classes
  void updateObjectBrowser(){};
  QTreeWidgetItem* browserTreeItem() const ;
  
protected:

  virtual double calculateCharge() const;
  
  //Ways of providing path information:
  // * Provide a Trk::Track
  // * Provide a vector of Amg::Vector3D (this precludes extrapolation) // BEFORE EIGEN MIGRATION WAS: HepGeom::Point3D<double>'s
  //
  //Override exactly one of the two following methods, in order to
  //provide path information. It will be called exactly once and the
  //derived class destructor will be responsible of any necessary
  //memory cleanup of the returned objects:
  virtual const Trk::Track *              provide_pathInfoTrkTrack() const { return 0; };
  //  virtual const std::vector< HepGeom::Point3D<double> > * provide_pathInfoPoints() const { return 0; }
  virtual const std::vector< Amg::Vector3D > * provide_pathInfoPoints() const { return 0; }

  /** Default implementation of this next method bases hypothesis on pdgCode() and charge():*/
  virtual Trk::ParticleHypothesis extrapolationParticleHypothesis() const;

  virtual void ensureTouchedMuonChambersInitialised() const {};//!< If not earlier, touched muon chambers must me registered at latest when this method is invoked
  void registerTouchedMuonChamber(const GeoPVConstLink&) const;//!< Call this from constructor - (at least) once for each muon chamber "touched" by this track.

  virtual void visibleStateChanged();//!< override if need to take action in this case.
  virtual void currentMaterialChanged() {};//!< override if need to take action in this case.

  QList<AssociatedObjectHandleBase*> getAllAscObjHandles() const; //!< Return list of all AscObj handles. Not guaranteed to be filled unless tsoses are visible.

private:

  // It is illegal to copy/assign a TrackHandleBase:
  TrackHandleBase(const TrackHandleBase & );
  TrackHandleBase & operator= (const TrackHandleBase & );

  friend class AssocObjAttachmentHandle;
  class Imp;
  Imp * m_d;


  //Here for inlining:
  bool m_visible;
  TrackCollHandleBase * m_collhandle;
  SoMaterial * m_currentmaterial;
  void clearLine();
  void registerTrack();

  void updateShapes_TSOSWithMeasurements();
  void updateShapes_TSOSWithErrors();
  void updateShapes_TSOSWithMaterialEffects();
  void setShownTSOSParts(TrackCommonFlags::TSOSPartsFlags f);
  void setCustomColouredTSOSParts(TrackCommonFlags::TSOSPartsFlags f);

  //For associated objects:
  friend class AssociatedObjectHandleBase;
  friend class TrackCollHandleBase; // Troels: maybe not needed...
  AssocObjAttachmentHandle * getAttachmentHandle(int regionIndex, const double& crossoverval);
protected:
  void registerAssocObject(AssociatedObjectHandleBase*);//!<Trackhandle assumes ownership

};

class AssocObjAttachmentHandle {
public:
  void attachNodes( SoNode*simple, SoNode*detailed, bool unpickable );
  void setPickableStateOfNodes( SoNode*simple, SoNode*detailed, bool unpickable );//!<Will attach if not currently attached.
  void detachNodes( SoNode*simple, SoNode*detailed );
private:
  friend class TrackHandleBase;//The life of these classes are entirely owned by TrackHandleBase.
  friend class TrackHandleBase::Imp;//The life of these classes are entirely owned by TrackHandleBase.
  AssocObjAttachmentHandle(TrackLODHandle *,TrackHandleBase* );
  ~AssocObjAttachmentHandle();
  AssocObjAttachmentHandle(const AssocObjAttachmentHandle & );
  AssocObjAttachmentHandle & operator= (const AssocObjAttachmentHandle & );
  void trackMaterialChanged();
  void trackVisibilityChanged();
  class Imp;
  Imp * m_d;
};


#endif
