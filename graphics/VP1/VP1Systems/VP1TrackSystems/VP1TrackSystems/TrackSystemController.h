/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackSystemController               //
//                                                            //
//  Description: Controller widget for the track system       //
//               Keeps all nasty gui stuff internally, and    //
//               only presents the actual interesting data    //
//               with specialised access methods and signals. //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKSYSTEMCONTROLLER_H
#define TRACKSYSTEMCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VP1TrackSystems/TrackCommonFlags.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include <QFlags>
#include "GaudiKernel/ServiceHandle.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"

class TrackCollWidget;
class SoMaterial;
class SoDrawStyle;
// class SoLightModel;
class SoComplexity;
class TrackSysCommonData;

// class TrackObjectBrowserController;

class QTreeWidget;
class QTreeWidgetItem;

namespace Trk {
  class IExtrapolator;
  class ITrackFitter;
  class Track;
  class Volume;
}

namespace Muon {
    class MuonEDMPrinterTool;
}

class TrackSystemController : public VP1Controller {

  Q_OBJECT

public:

  TrackSystemController(IVP1System * sys);
  virtual ~TrackSystemController();

  void initTools();//comboboxes for selecting tools + tool creation is
		   //postponed until this call (so it can wait until
		   //systemcreate())

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  /*So we can enable/disable parts based on whether prd's & Tracks are selected:*/
  void setNumberOfSelectedPRDsAndTracks(unsigned prds, unsigned trks);

  TrackCollWidget * collWidget() const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////
  TrackCommonFlags::TrackPartsFlags shownTrackParts() const;
  int vertexProjectionAngle() const; //Ranges from 0-179 i.e. is in degrees. Might be disabled, in which case will be -ve.
    
  // double trackTubeRadius() const;//0 => don't show tubes for tracks
  enum PropagationOptionFlag {NoPropOptions=0x0000, IgnoreMEOT=0x0001, ExtendTrack=0x0002};
  Q_DECLARE_FLAGS(PropagationOptionFlags, PropagationOptionFlag)
  Trk::IExtrapolator * propagator() const;//Null for no propagator - i.e. show raw data.
  PropagationOptionFlags propagationOptions() const;
  float propMaxRadius() const;
  
  bool ignoreMeasurementEffectsOnTrackInProp() ; //!< If true (default) don't use TSOS with MEOT to draw track points
  // ITrackingVolumesSvc * trackingVolumeSvc() const;
  const Trk::Volume * extrapolateToThisVolume() const ; //!< Returns the volume to which we extrapolate ID tracks, or zero if no VolumesSvc found.
  Trk::ITrackFitter * trackFitter() const;
  
  Muon::MuonEDMPrinterTool * muonEDMPrinterTool() const;
  
  QString nameOfNewlyFittedCollections() const;//queried on-demand, no signal.
  TrackCommonFlags::SELECTIONMODE selectionMode() const;
  TrackCommonFlags::FITTERMODE fitterMode() const;
  bool fitterRemoveOutliers() const;
  Trk::ParticleHypothesis fitterParticleHypthesis() const;
  TrackCommonFlags::DETAILLEVEL assocObjDetailLevel() const;

  //Associated objects (base):
  TrackCommonFlags::TSOSPartsFlags shownTSOSParts() const;
  TrackCommonFlags::TSOSPartsFlags customColouredTSOSParts() const;
  bool useShortTRTMeasurements() const;
  bool useShortMDTMeasurements() const;
  bool drawMeasGlobalPositions() const;
  
  bool hideCustomSurfaces() const;
  bool hideTubeSurfaces() const;
  double measurementsShorttubesScale() const;
  double nStdDevForParamErrors() const;
  bool parTubeErrorsDrawCylinders() const;
  int numberOfPointsOnCircles() const;//will return numbers in {4,8,12,16,...}
  double materialEffectsOnTrackScale() const;


  bool showTruthAscObjs() const;//Fixme: obsolete
  //General cuts:
  VP1Interval cutAllowedPt() const;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
  //Cuts for number of hits per subsystem:
  QList<unsigned> cutRequiredNHits() const;//Empty list: no requirements. Otherwise a list
                                           // with a length of four (pixel,sct,trt,muon)

  void vertexCutsAllowed(bool); // called if vertex system is available.
  bool cutOnlyVertexAssocTracks() const;
  //Truth cuts:
  bool cutTruthFromIROnly() const;
  bool cutExcludeBarcodeZero() const;
  bool cutTruthExcludeNeutrals() const;
  //Interactions:
  bool showTotMomentumOnMultiTrackSelection() const;
  //No associated signals for the next four (only queried on-click):
  bool orientAndZoomOnSingleSelection() const;
  bool printInfoOnSingleSelection() const;
  bool printVerboseInfoOnSingleSelection() const;
  bool printTotMomentumOnMultiTrackSelection() const;

  //Material pointers returned by the next methods won't change during
  //the lifetime of the system (but their fields may get updated by
  //the controller):
  SoMaterial * getMaterialForPDGCode(const int& pdgcode) const;
  SoMaterial * getMaterialForCharge(const double& charge) const;
  SoMaterial * getMaterialForMomentum(const double& absmom) const;
  SoMaterial * customMatMeasurements() const;
  SoMaterial * customMatMeasurementsOutliers() const;
  SoMaterial * customMatParameters() const;
  SoMaterial * customMatHoleParameters() const;
  SoMaterial * customMatParameterErrors() const;
  SoMaterial * customMatMaterialEffects() const;
  SoMaterial * customMatSurfaces() const;

  //Overall drawstyles, complexities and light models:
  // SoDrawStyle * trackDrawStyle() const;
  // SoLightModel * trackLightModel() const;
  SoDrawStyle * ascObjDrawStyle() const;
  SoComplexity * ascObjComplexity() const;
  
  QTreeWidget* trackObjBrowser() const; //!< Returns a pointer to the Track Object Browser (if it exists)
  TrackSysCommonData * common() const; //!< Returns a pointer to the common data (if it exists)
  void setCommonData(TrackSysCommonData *) ; //!< set pointer to the common data

  // Return special information for VP1_TRKSYS_SHIFTCHAMBERS mode
  std::vector<double> alignmentShiftValue();
  int alignmentShiftLevel();
  
  // Labels
  bool doTrackLabels();
  enum TrackLabelMode {NoLabels=0x0000, P=0x0001, Pt=0x0002, Pid=0x0004, Hits=0x0008, 
                       FitQuality=0x0010, Direction=0x0020};
  Q_DECLARE_FLAGS(TrackLabelModes, TrackLabelMode)

  TrackLabelModes trackLabels();
  
  float trackLabelTrkOffset() ; //!< Percentage of postion along track
  QList<int> trackLabelPosOffset() ; //!< Offset in x,y,z
  
  int labelXOffset() ; //!< Absolute x offset
  int labelYOffset() ; //!< Absolute y offset
  int labelZOffset() ; //!< Absolute z offset

  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:
  void rerandomise();//This one doesn't have an access method, but is just emitted when needed.
  void refit();//[TEMPORARY] This one doesn't have an access method, but is just emitted when needed.
  void shownTrackPartsChanged(TrackCommonFlags::TrackPartsFlags);
  void vertexProjectionAngleChanged(int);
  void trackLabelsChanged(TrackSystemController::TrackLabelModes);
  void trackLabelTrkOffsetChanged(float);
  void trackLabelPosOffsetChanged(QList<int>);
  // void trackTubeRadiusChanged(const double&);
  void shownTSOSPartsChanged(TrackCommonFlags::TSOSPartsFlags);
  void customColouredTSOSPartsChanged(TrackCommonFlags::TSOSPartsFlags);
  void drawMeasGlobalPositionsChanged(bool);
  void useShortTRTMeasurementsChanged(bool);
  void useShortMDTMeasurementsChanged(bool);
  void measurementsShorttubesScaleChanged(double);
  void nStdDevForParamErrorsChanged(const double&);
  void parTubeErrorsDrawCylindersChanged(bool);
  void numberOfPointsOnCirclesChanged(int);
  void materialEffectsOnTrackScaleChanged(double);
  void propagatorChanged(Trk::IExtrapolator *);
  void propagationOptionsChanged(TrackSystemController::PropagationOptionFlags);
  void propMaxRadiusChanged(float);
  void extrapolateToThisVolumeChanged(void);
  // void ignoreMeasurementEffectsOnTrackInPropChanged(bool);
  void trackFitterChanged(Trk::ITrackFitter *);
  void selectionModeChanged(TrackCommonFlags::SELECTIONMODE);
  void showTruthAscObjsChanged(bool);
  void assocObjDetailLevelChanged( TrackCommonFlags::DETAILLEVEL );
  void cutAllowedPtChanged(const VP1Interval&);
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);
  void cutRequiredNHitsChanged(const QList<unsigned>&);
  void cutTruthFromIROnlyChanged(bool);
  void cutExcludeBarcodeZeroChanged(bool);
  void cutTruthExcludeNeutralsChanged(bool);
  void cutOnlyVertexAssocTracksChanged(bool);
  void showTotMomentumOnMultiTrackSelectionChanged(bool);
  
private:

  class Imp;
  Imp * m_d;

  //For verbose output:
  template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); }//unhide base methods
  static QString toString( const TrackCommonFlags::SELECTIONMODE& par ) { return TrackCommonFlags::toString(par); }
  static QString toString( const TrackCommonFlags::DETAILLEVEL& par ) { return TrackCommonFlags::toString(par); }
  static QString toString( const QList<unsigned>& par );
  static QString toString( const QList<int>& par );

private slots:
  void emitRerandomise();
  void emitRefit();
  // void updateTrackDrawStyle();
  // void updateTrackLightModel();
  void updateAscObjDrawStyle();
  void updateAscObjComplexity();
  void availableExtrapolatorsChanged(const QStringList&);
  void availableFittersChanged(const QStringList&);
  void emitExtrapolateToHereChanged(int);
  void possibleChange_shownTrackParts();
  void possibleChange_vertexProjectionAngle();
  // void possibleChange_trackTubeRadius();
  void possibleChange_trackLabels();
  void possibleChange_trackLabelTrkOffset();
  void possibleChange_trackLabelPosOffset();
  void possibleChange_shownTSOSParts();
  void possibleChange_customColouredTSOSParts();
  void possibleChange_useShortTRTMeasurements();
  void possibleChange_drawMeasGlobalPositions(); 
  void possibleChange_useShortMDTMeasurements();
  void possibleChange_measurementsShorttubesScale();
  void possibleChange_nStdDevForParamErrors();
  void possibleChange_parTubeErrorsDrawCylinders();
  void possibleChange_numberOfPointsOnCircles();
  void possibleChange_materialEffectsOnTrackScale();
  void possibleChange_propagator();
  void possibleChange_propagationOptions();
  void possibleChange_propMaxRadius();
  // void possibleChange_ignoreMeasurementEffectsOnTrackInProp();
  void possibleChange_trackFitter();
  void possibleChange_selectionMode();
  void possibleChange_assocObjDetailLevel();
  void possibleChange_showTruthAscObjs();
  void possibleChange_cutAllowedPt();
  void possibleChange_cutAllowedEta();
  void possibleChange_cutAllowedPhi();
  void possibleChange_cutRequiredNHits();
  void possibleChange_cutTruthFromIROnly();
  void possibleChange_cutExcludeBarcodeZero();
  void possibleChange_cutTruthExcludeNeutrals();
  void possibleChange_cutOnlyVertexAssocTracks();
  void possibleChange_showTotMomentumOnMultiTrackSelection();
  void objectBrowserClicked(QTreeWidgetItem * item, int);
  void updateFitPRDButtonState();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TrackSystemController::TrackLabelModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(TrackSystemController::PropagationOptionFlags)

#endif
