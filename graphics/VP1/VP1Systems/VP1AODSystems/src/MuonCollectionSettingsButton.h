/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_MUONCOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_MUONCOLLSETTINGSBUTTON_H

#include "VP1Base/VP1MaterialButton.h"
#ifndef Q_MOC_RUN
#include "xAODMuon/Muon.h"
#endif 

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
  SoMaterial* parameterMaterial( xAOD::ParameterPosition ) const;
    
  // Quality cuts
  unsigned int minimumQuality() const;
  
  enum ShownAssociatedObject {
      Nothing = 0x0,
      TrackParticlesPrimary = 0x1,
      TrackParticlesCB = 0x2,
      TrackParticlesID = 0x3,
      TrackParticlesMS = 0x4,
      TrackParticlesME = 0x5,
      CaloClusters = 0x6,
      Segments = 0x7
  };
  Q_DECLARE_FLAGS(ShownAssociatedObjects, ShownAssociatedObject)
  ShownAssociatedObjects shownAssociatedObjects() const;
  
  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );
  
  signals:
  void trackTubeRadiusChanged(const double&);
  void lastAppliedChanged(); // emitted when something related to material changes
  void hideActualTrackPathChanged(bool);
   
  void cutAllowedPtChanged(const VP1Interval&);
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);
  
  void minimumQualityChanged(unsigned int);
  
  void shownAssociatedObjectsChanged(MuonCollectionSettingsButton::ShownAssociatedObjects);
   
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
  void possibleChange_minimumQuality();
  void possibleChange_shownAssociatedObjects();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MuonCollectionSettingsButton::ShownAssociatedObjects)

#endif


