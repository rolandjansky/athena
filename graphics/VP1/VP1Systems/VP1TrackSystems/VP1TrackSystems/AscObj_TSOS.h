/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AscObj_TSOS                         //
//                                                            //
//  Description: Handle for TSOS's                            //
//                                                            //
//  Author: Troels Kofoed Jacobsen                            //
//          Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef ASCOBJ_TSOS_H
#define ASCOBJ_TSOS_H

#include "VP1TrackSystems/AssociatedObjectHandleBase.h"
#include "VP1TrackSystems/TrackCommonFlags.h"
#include "VP1TrackSystems/TrkObjToString.h"
#include "TrkParameters/TrackParameters.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <QTreeWidgetItem>
#include "VP1Utils/HitToSoNode.h"


typedef AmgSymMatrix(2) AmgCovMatrix; // new Eigen-based CovarianceMatrix



class SoTranslation;
namespace Trk {
  class RIO_OnTrack;
  class CompetingRIOsOnTrack;
  class Surface;
  class TrackStateOnSurface;
  class MeasurementBase;
}

class AscObj_TSOS : public AssociatedObjectHandleBase {
public:

  AscObj_TSOS(TrackHandleBase *, const Trk::TrackStateOnSurface *,
        unsigned indexOfPointOnTrack);
  void setDistToNextPar(const double&);

  void buildShapes(SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  QStringList clicked();
  void zoomView(); //!< Depending on the controller settings, will attempt to zoom the view to this TSOS, for example in click()
  bool isShortMeasurement(); //!< Return true if TRT/MDT & shortened mode is on, false otherwise  
  
  virtual void setVisible(bool);
  
  TrackCommonFlags::TSOSPartsFlags parts() const { return m_parts; }
  bool hasParameters() const { return m_parts & TrackCommonFlags::TSOS_TrackPars; }
  bool hasParError() const { return m_parts & TrackCommonFlags::TSOS_AnyParsErrors; }
  bool hasError() const { return m_parts & TrackCommonFlags::TSOS_AnyErrors; }
  bool hasMaterialEffect() const { return m_parts & TrackCommonFlags::TSOS_AnyMaterialEffects; }
  bool hasSurface() const { return m_parts & TrackCommonFlags::TSOS_AnySurface; }
  bool hasMeasurement() const { return m_parts & TrackCommonFlags::TSOS_AnyMeasurement; }

  const Trk::MeasurementBase * measurement() const;
  const Trk::Surface * surface() const;
  const Trk::RIO_OnTrack * rioOnTrack() const;
  const Trk::CompetingRIOsOnTrack * competingRIOsOnTrack() const;
  const Trk::TrackStateOnSurface * trackStateOnSurface() const {return m_tsos;}

  //Add identify() method???

  Amg::Vector3D approxCenter() const;

  virtual bool initiatesOwnZooms() const { return true; }
  
  virtual QTreeWidgetItem* browserTreeItem() const {return m_objBrowseTree;}
  virtual void setBrowserTreeItem(QTreeWidgetItem* obt)  {m_objBrowseTree=obt;}

protected:
  int regionIndex() const;
  double lodCrossOverValue() const {return 1000; }

private:
  virtual ~AscObj_TSOS() {}//Private so it can only be deleted by TrackHandleBase
  const Trk::TrackStateOnSurface * m_tsos;
  TrackCommonFlags::TSOSPartsFlags m_parts;
  unsigned m_indexOfPointOnTrack;
  double m_distToNextPar;
  QTreeWidgetItem* m_objBrowseTree;

  double deviationFromMeasurement(const bool& absolute = false);
  void addDeviationFromMeasurementInfoToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  void addTransformToSurface( SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  void addTrackParamInfoToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed,
          bool showPars, bool showParsErrors, bool showSurfaces );
  void addErrors(const Trk::Surface& theSurface, const AmgSymMatrix(5)& tmpCovMat,
                 const Amg::Vector2D& localPos, const Amg::Vector3D& p1, bool showSurfaces,
                 SoSeparator* shape_simple, SoSeparator* shape_detailed, bool force1D, bool applyLocalTrans );
  void addRIO_OnTrackInfoToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed, const Trk::RIO_OnTrack* rio, bool blockDrawingOfGP=false);
  void addCompetingRIO_OnTrackInfoToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  void addSurfaceToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  void addMaterialEffectsToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed);

  void ensureInitSeps( SoSeparator*&shape_simple, SoSeparator*&shape_detailed);
  SoTranslation* getZTranslationTube( const Trk::Surface *, const double& maxTrans ) const;

  TrkObjToString m_objToType; 
  HitToSoNode m_hitToSoNode;
};

#endif
