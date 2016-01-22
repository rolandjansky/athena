/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_TRACKPARTICLECOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_TRACKPARTICLECOLLSETTINGSBUTTON_H

#include "VP1Base/VP1MaterialButton.h"
#include "xAODTracking/TrackingPrimitives.h"

class SoDrawStyle;
class SoLightModel;

class TrackParticleCollectionSettingsButton : public VP1MaterialButtonBase {

  Q_OBJECT

public:
  
  TrackParticleCollectionSettingsButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
  virtual ~TrackParticleCollectionSettingsButton();
  
  void setDimension(int dim);
  
  // The below are necessary to fulfill the interface, but will just be passed onto the VP1MaterialButton owned by the TrackParticleCollectionSettingsButton form
  virtual bool setMaterial(SoMaterial*);
  virtual void copyValuesFromMaterial(SoMaterial*);
  virtual double lastAppliedTransparency() const ;
  virtual double lastAppliedShininess() const  ;
  virtual double lastAppliedBrightness() const;
  
  // GUI for track lines...
  double trackTubeRadius() const;//0 => don't show tubes for tracks
  bool   hideActualTrackPath() const;

  // GUI for propagation
  bool useExistingParameters() const; /// Returns true if the propagation should use the parameters already on the particle, or false if we should extrapolate from the 1st parameter
  int numOfStepsForInterpolation() const; /// Returns the number of steps to use when interpolating between paramters (0 is no interpolation.)

	
  // GUI for parameters
  bool showParameters() const;
  bool colourParametersByType() const;
  

  // GUI for cuts
  bool        cutOnlyVertexAssocTracks() const; //!< Return true if this collection should only should tracks associated to a vertex
  VP1Interval cutAllowedPt() const;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
  //Cuts for number of hits per subsystem:
  QList<unsigned> cutRequiredNHits() const;//Empty list: no requirements. Otherwise a list
  // with a length of four (pixel,sct,trt,muon)
   
  //Overall drawstyles, complexities and light models:
  SoDrawStyle *  trackDrawStyle() const;
  SoLightModel * trackLightModel() const;
  
  // Parameters 
  SoMaterial* defaultParameterMaterial() const;
  SoMaterial* parameterMaterial( xAOD::ParameterPosition) const;
    
  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );
  
  signals:
  void trackTubeRadiusChanged(const double&);
  void lastAppliedChanged(); // emitted when something related to material changes
  void hideActualTrackPathChanged(bool);
  
  void showParametersChanged(bool);
  void colourParametersByTypeChanged(bool);
	 
  void cutAllowedPtChanged(const VP1Interval&);
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);
  void cutRequiredNHitsChanged(const QList<unsigned>&);
  void cutOnlyVertexAssocTracksChanged(bool);
  void propagationOptionsChanged();
	 
  public slots:
  void showEditMaterialDialog();
  void setMaterialText(const QString&);

  protected slots:
  void updateButton();

protected:
  // void initEditWindow();//!< Make sure that edit window is constructed
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void dropEvent(QDropEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
private:

  class Imp;
  Imp * d;
    
  private slots:
  void updateTrackDrawStyle();
  void updateTrackLightModel(bool);
  void updateTrackTubeRadius();
  void updateHideActualTrackPath(bool);
  void possibleChange_cutAllowedPt();
  void possibleChange_cutAllowedEta();
  void possibleChange_cutAllowedPhi();
  void possibleChange_cutRequiredNHits();
  void possibleChange_cutOnlyVertexAssocTracks();
  void possibleChange_showParameters();
  void possibleChange_parameterColours();
    
};

#endif


