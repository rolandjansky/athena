/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_MISSINGETCOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_MISSINGETCOLLSETTINGSBUTTON_H

//#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1CollectionSettingsButtonBase.h"

#include "xAODTracking/TrackingPrimitives.h"

class SoDrawStyle;
class SoLightModel;

//class MissingEtCollectionSettingsButton : public VP1MaterialButtonBase {
class MissingEtCollectionSettingsButton : public VP1CollectionSettingsButtonBase {

  Q_OBJECT

public:
  
  MissingEtCollectionSettingsButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
  virtual ~MissingEtCollectionSettingsButton();
  
  void setDimension(int dim);
  
  // The below are necessary to fulfill the interface, but will just be passed onto the VP1MaterialButton owned by the MissingEtCollectionSettingsButton form
  virtual bool setMaterial(SoMaterial*);
  virtual void copyValuesFromMaterial(SoMaterial*);
  virtual double lastAppliedTransparency() const ;
  virtual double lastAppliedShininess() const  ;
  virtual double lastAppliedBrightness() const;
  
  // GUI for cuts
  VP1Interval cutAllowedR() const;
  VP1Interval cutAllowedZ() const;
   
  //Overall drawstyles, complexities and light models:
//  SoDrawStyle *  vertexDrawStyle() const;
//  SoLightModel * vertexLightModel() const;

  // Getters
  float            metLength() const;
  float            metThickness() const;
  QList<VP1Interval> cutAllowedPhi() const;
      
  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );
  
signals:
  void lastAppliedChanged(); // emitted when something related to material changes
  void cutAllowedPhiChanged(const QList<VP1Interval>&);

//  void cutAllowedRChanged(const VP1Interval&);
//  void cutAllowedZChanged(const VP1Interval&);
  void metSizeChanged(int);
	 
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
  void updateVertexDrawStyle();
  void updateVertexLightModel(bool);
//  void possibleChange_vertexSize();
//  void possibleChange_cutAllowedR();
//  void possibleChange_cutAllowedZ();
    
};

#endif


