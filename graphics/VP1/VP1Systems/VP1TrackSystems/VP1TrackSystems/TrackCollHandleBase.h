/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackCollHandleBase                 //
//                                                            //
//  Description: Base class for collection handles            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOLLHANDLEBASE_H
#define TRACKCOLLHANDLEBASE_H

#include "VP1TrackSystems/TrackTypes.h"
#include "VP1TrackSystems/TrackCommonFlags.h"
#include "VP1TrackSystems/TrackSysCommonData.h"//Not strictly needed
                 //here, but since all
                 //derived classes will
                 //need it we put it
                 //here.
#include "VP1Base/VP1Interval.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include <QColor>
#include <iostream>


class TrackSystemController;
class TrackHandleBase;
class SoSeparator;
class SoMaterial;
class VP1ExtraSepLayerHelper;
class TrackSysCommonData;
namespace Trk {
  class Track;
  class Segment;
  class IExtrapolator;
}

#include "VP1Base/VP1StdCollection.h"

class TrackCollHandleBase : public VP1StdCollection
{

  Q_OBJECT

public:

  //Each derived class should implement a "static QStringList
  //availableCollections();" method, which the collection widget will
  //use to determine the available collections.

  TrackCollHandleBase(TrackSysCommonData *,
          const QString& name,
          TrackType::Type);
  virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
  virtual ~TrackCollHandleBase();

   //Called just after creation. Should set current cut values and
  //connections with controller to monitor future changes. Reimplement
  //the ..Specific method to setup subsystem specific settings.
  void setupSettingsFromController(TrackSystemController*);
protected:
  virtual void setupSettingsFromControllerSpecific(TrackSystemController*) {};
public:

  ///////////////////////////////////////////////////////////
  //  For loading the data and resetting after each event: //
  ///////////////////////////////////////////////////////////

  //For use by the handles:
  QString name() const;
  TrackSysCommonData * common() const { return m_commonData; }
  //
  VP1ExtraSepLayerHelper * sephelper() const { return m_sephelper; }//For attaching/detaching. FIXME: TO BASE!!

  virtual bool canUsePropagator() { return true; }

  double trackTubeRadius() const { return m_trackTubeRadius; }

  virtual bool mightHaveSubSystemHitInfo() const { return false; }

  virtual QByteArray persistifiableState() const;//!<Provide specific implementation 
  virtual void setState(const QByteArray&);//!<Provide specific implementation 
  
protected:

  //Must be called from derived classes when filling in new track handles.
  void hintNumberOfTracksInEvent(unsigned);
  void addTrackHandle(TrackHandleBase*);

  //For iterating over track handles:
  void trackHandleIterationBegin();
  TrackHandleBase* getNextTrackHandle(); //Returns 0 when done.

  //Override if special cuts. Remember to call base class implementation also for common cuts.
  virtual bool cut(TrackHandleBase*);//Return true if should be shown (based on various cuts), false otherwise.

  //Utility (fixme: put in utility class elsewhere).
  template <class T> void cleanupPtrContainer(T&) const;//Delete pointers and calls .clear()
  //   template <class T> void cleanupNodeContainer(T&) const;//unref's pointers and calls .clear()

  void recheckCutStatus(TrackHandleBase*);//Call in derived classes for handles that might be effected by a change in cuts.

  //Convenience methods which can be called from derived classes or
  //the trackcollwidget (but specialised procedures might be more
  //optimal)
public:
  void recheckCutStatusOfAllVisibleHandles();
  void recheckCutStatusOfAllNotVisibleHandles();
  void recheckCutStatusOfAllHandles();
  void update3DObjectsOfAllHandles(bool onlythosetouchingmuonchambers = false, bool invalidatePropagatedPoints = false, float maxR = 0.0 ); // maxR being set will override the prop max value from the controller, e.g. for animations
  void updateMaterialOfAllHandles();
  void updateInDetProjectionsOfAllHandles();
  void updateMuonProjectionsOfAllHandles();

  int nShownHandles() { return m_nshownhandles; }
  //For use only by TrackHandleBase::setVisible(..):
  void incrementNShownHandles() { ++m_nshownhandles; }
  void decrementNShownHandles() { --m_nshownhandles; }

  virtual bool mayHaveAssociatedObjects() const { return false; }

  // FIXME - really not sure any of the prop stuff belongs here. Property of commondata/controller, not coll handle. EJWM
  Trk::IExtrapolator * propagator() const { return m_propagator; }
  TrackSystemController::PropagationOptionFlags propagationOptions() { return m_propagationOptions; }
  
  bool  ignoreMEOTinProp() const { return m_propagationOptions.testFlag(TrackSystemController::IgnoreMEOT); }
  bool  extendTracks() const { return m_propagationOptions.testFlag(TrackSystemController::ExtendTrack); }
  TrackCommonFlags::TrackPartsFlags parts() const { return m_parts; }

  enum COLOURBY { COLOUR_PERCOLLECTION, COLOUR_RANDOM, COLOUR_BYPID, COLOUR_MOMENTUM, COLOUR_CHARGE, COLOUR_DISTANCE, COLOUR_VERTEX };
  static QString toString(const COLOURBY&);

  COLOURBY colourBy() const { return m_colourby; }

  virtual COLOURBY defaultColourBy () const { return COLOUR_PERCOLLECTION; }
  virtual bool allowColourByPID() const { return false; }
  virtual bool allowColourByMomentum() const { return true; }
  virtual bool allowColourByCharge() const { return true; }
  virtual bool allowColourByDistanceFromSelectedTrack() const {return VP1QtUtils::environmentVariableIsOn("VP1_TRKSYS_COLOURBYDIST"); } 
  virtual bool allowColourByVertex() const { return false;  }
  virtual void collMaterialTransparencyAndBrightnessChanged();
  
  // Cuts
  bool useDefaultCuts() const; //!< Return true if this collection uses the default (i.e. controller) cuts
  bool cutOnlyVertexAssocTracks() const; //!< Return true if this collection should only should tracks associated to a vertex
  //General cuts:
  VP1Interval cutAllowedPt() const;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
  //Cuts for number of hits per subsystem:
  QList<unsigned> cutRequiredNHits() const;//Empty list: no requirements. Otherwise a list
                                           // with a length of four (pixel,sct,trt,muon)

  void vertexCutsAllowed(bool); // called if vertex system is available.
  //Truth cuts:
  bool cutTruthFromIROnly() const;
  bool cutExcludeBarcodeZero() const;
  bool cutTruthExcludeNeutrals() const;
  
protected:

  qint32 provideCollTypeID() const;
  virtual QString provideSection() const { return TrackType::typeToSectionHeader(m_type); };//FIXME
  virtual QString provideSectionToolTip() const { return TrackType::typeToSectionHeaderToolTip(m_type); };//FIXME
  virtual QString provideText() const { return name(); };//FIXME
  virtual void assignDefaultMaterial(SoMaterial*) const;
  virtual QColor defaultColor() const = 0;//Will be used in assignDefaultMaterial
  QList<QWidget*> provideExtraWidgetsForGuiRow() const;
  QByteArray extraWidgetsState() const;
  void setExtraWidgetsState(const QByteArray&);
private slots:
  void colourByComboBoxItemChanged();
public slots:
  void rerandomiseRandomTrackColours();
  void trackSelectionChanged();
  void setShownTSOSParts(TrackCommonFlags::TSOSPartsFlags);
  void setLabels(TrackSystemController::TrackLabelModes);
  void setLabelTrkOffset(float);
  void setLabelPosOffsets(QList<int>);
  void setCustomColouredTSOSParts(TrackCommonFlags::TSOSPartsFlags);
  void setUseShortTRTMeasurements(bool);
  void setUseShortMDTMeasurements(bool);
  void setDrawMeasGlobalPositions(bool);
  void setMeasurementsShorttubesScale(double);
  void setNStdDevForParamErrors(const double&);
  void setNumberOfPointsOnCircles(int);
  void setMaterialEffectsOnTrackScale(double);
  void setParTubeErrorsDrawCylinders(bool);
  void setRequiredNHits(const QList<unsigned>&);
  void setOnlyVertexAssocTracks(bool);
  void extrapolateToThisVolumeChanged(void);
  void clipVolumeChanged(double);
private:

  class Imp;
  Imp * m_d;

  int m_nshownhandles;
  TrackType::Type m_type;
  TrackSysCommonData * m_commonData;

  VP1ExtraSepLayerHelper * m_sephelper;
  Trk::IExtrapolator * m_propagator;
  TrackSystemController::PropagationOptionFlags m_propagationOptions;
  float m_propMaxRadius;
  TrackCommonFlags::TrackPartsFlags m_parts;
  TrackSystemController::TrackLabelModes m_labels;
  float m_labelsTrkOffset;
  QList<int> m_labelsPosOffsets;
  double m_trackTubeRadius;
  COLOURBY m_colourby;

  VP1Interval m_cut_allowedPtSq;//We store the allowed interval for pt squared - to avoid sqrt's.
  VP1Interval m_cut_allowedEta;
  QList<VP1Interval> m_cut_allowedPhi;
  QList<unsigned> m_cut_requiredNHits;
  bool m_cut_pt_allowall;
  bool m_cut_eta_allowall;
  bool m_cut_phi_allowall;
  bool m_cut_etaptphi_allwillfail;
  void actualSetShownTSOSPartsOnHandles();
  void actualSetCustomColouredTSOSPartsOnHandles();
  void fillObjectBrowser();
  void updateObjectBrowserVisibilityCounts();

private slots:
  void collVisibilityChanged(bool);
  void setPropagator(Trk::IExtrapolator *);
  void setPropagationOptions(TrackSystemController::PropagationOptionFlags);
  void setPropMaxRadius(float);
  void setParts(TrackCommonFlags::TrackPartsFlags);
  void setParts(int);
  void setTrackTubeRadius(const double&);
  void setColourBy(COLOURBY);
  void setUseDefaultCuts(bool);
  void setCutAllowedPt(const VP1Interval&);
  void setCutAllowedEta(const VP1Interval&);
  void setCutAllowedPhi(const QList<VP1Interval>&);
  void setActualPathShown(bool);

};

//Fixme: Move elsewhere:
template <class T> void TrackCollHandleBase::cleanupPtrContainer(T&t) const
{
  typename T::iterator it(t.begin()), itE(t.end());
  for (;it!=itE;++it)
    delete *it;
  t.clear();
}

#endif




