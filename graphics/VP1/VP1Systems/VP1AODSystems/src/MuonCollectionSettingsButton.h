/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_MUONCOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_MUONCOLLSETTINGSBUTTON_H

#include "VP1Base/VP1MaterialButton.h"
#include "xAODTracking/TrackingPrimitives.h"

class SoDrawStyle;
class SoLightModel;

class MuonCollectionSettingsButton : public VP1MaterialButtonBase {

  Q_OBJECT

public:
  
  MuonCollectionSettingsButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
  virtual ~MuonCollectionSettingsButton();
  
  void setDimension(int dim);
  
  // The below are necessary to fulfill the interface, but will just be passed onto the VP1MaterialButton owned by the MuonCollectionSettingsButton form
  virtual bool setMaterial(SoMaterial*);
  virtual void copyValuesFromMaterial(SoMaterial*);
  virtual double lastAppliedTransparency() const ;
  virtual double lastAppliedShininess() const  ;
  virtual double lastAppliedBrightness() const;
  
  // GUI for track lines...
  double trackTubeRadius() const;//0 => don't show tubes for tracks
  bool   hideActualTrackPath() const;
  
  // GUI for cuts
  VP1Interval cutAllowedPt() const;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
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
   
  void cutAllowedPtChanged(const VP1Interval&);
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);

   
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
};

#endif


