/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1GEOMETRYSYSTEM_H
#define VP1GEOMETRYSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1GeometrySystem                            //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Derived from V-atlas geometry system by Joe Boudreau.              //
//  Origins of initial version dates back to ~1996, initial VP1        //
//  version by TK (May 2007) and almost entirely rewritten Oct 2007    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"
#include "GeoModelKernel/GeoPVConstLink.h"//It would be good to get rid of this.
#include "VP1GeometrySystems/VP1GeoFlags.h"
#include "VP1GeometrySystems/VolumeHandle.h"//fixme
#include <set>
#include <map>
#include <QStack>
#include <QString>

class VP1GeometrySystem : public IVP13DSystemSimple {

  Q_OBJECT

public:


  VP1GeometrySystem( const VP1GeoFlags::SubSystemFlags& SubSystemsTurnedOn = VP1GeoFlags::None,
		     QString name = "Geo" );
  virtual ~VP1GeometrySystem();

  //Method that channel can use to override defaults:
  void setGeometrySelectable(bool);
  void setZoomToVolumeOnClick(bool);
  void setOrientViewToMuonChambersOnClick(bool);
  void setAutoAdaptMuonChambersToEventData(bool);


  QWidget * buildController();

  void systemcreate(StoreGateSvc*detstore);
  void buildPermanentSceneGraph(StoreGateSvc* detstore, SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc*, SoSeparator *) {}
  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  void systemuncreate();

  QByteArray saveState();
  void restoreFromState(QByteArray);
  void enableMuonChamberLabels(bool, bool);//!< first is t0s, 2nd is hits

signals:
  void appropriateMDTProjectionsChanged(int);//To give hints to prd/track/segment systems about
  //0: No projections, 1: Project to end of tubes, 2: Project to end of chamber volume.
  void plotSpectrum(QStack<QString>&, int copyNumber=-1); // Send information oabout selected volume to the PartSpectSystem
  //The stack represents the path to the selected volume. The entries of this patch have form Volname::CopyNo
  //The Volname is either physical volume name, or, in case the former is absent, the logical volume name
  //The ::CopyNo suffix is added only when CopyNo is applicable

public slots:

  void reiconizeToShowSpecificMuonChambers(const std::set<GeoPVConstLink>&);
  void orientViewToMuonChamber(const GeoPVConstLink& chamberPV);//Zooms/rotates to obtain orthogonal endview of to muon chamber
  void setCurvedSurfaceRealism(int);//Accepts values in the range 0..100.
  void muonChamberT0sChanged(const std::map<GeoPVConstLink, float>&,int);//!< The map is the dt0 per chamber, plus a label identifier, which by convention (!) is 0=Moore, 1=Muonboy
protected slots:
  void checkboxChanged();
  void updateTransparency();
  void adaptMuonChambersToEventData();
  void adaptMuonChambersStyleChanged();
  void emit_appropriateMDTProjectionsChanged();

  void autoAdaptPixelsOrSCT(bool,bool,bool,bool,bool,bool);//pixel,brl,ecA,ecC,bcmA,bcmC
  void resetSubSystems(VP1GeoFlags::SubSystemFlags);
  void autoExpandByVolumeOrMaterialName(bool,QString);//volname: (false,namestr), matname: (true,namestr)
  void actionOnAllNonStandardVolumes(bool);//true: zap, false: expand.

  void volumeStateChangeRequested(VolumeHandle*,VP1GeoFlags::VOLSTATE);
  void volumeResetRequested(VolumeHandle*);

  void setShowVolumeOutLines(bool);

  void saveMaterialsToFile(QString,bool);//(filename,onlyChangedMaterials)
  void loadMaterialsFromFile(QString);//filename
  
  void setLabels(int);
  void setLabelPosOffsets(QList<int>);

protected:
  class Imp;
  Imp * d;
};

#endif
