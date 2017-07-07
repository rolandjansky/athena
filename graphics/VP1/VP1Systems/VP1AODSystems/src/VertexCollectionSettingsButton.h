/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_VERTEXCOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_VERTEXCOLLSETTINGSBUTTON_H

#include "VP1Base/VP1MaterialButton.h"
#include "xAODTracking/TrackingPrimitives.h"

class SoDrawStyle;
class SoLightModel;

class VertexCollectionSettingsButton : public VP1MaterialButtonBase {

  Q_OBJECT

public:
  
  VertexCollectionSettingsButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
  virtual ~VertexCollectionSettingsButton();
  
  void setDimension(int dim);
  
  // The below are necessary to fulfill the interface, but will just be passed onto the VP1MaterialButton owned by the VertexCollectionSettingsButton form
  virtual bool setMaterial(SoMaterial*);
  virtual void copyValuesFromMaterial(SoMaterial*);
  virtual double lastAppliedTransparency() const ;
  virtual double lastAppliedShininess() const  ;
  virtual double lastAppliedBrightness() const;
  
  // GUI for cuts
  VP1Interval cutAllowedR() const;
  VP1Interval cutAllowedY() const;
  VP1Interval cutAllowedZ() const;
   
  //Overall drawstyles, complexities and light models:
  SoDrawStyle *  vertexDrawStyle() const;
  SoLightModel * vertexLightModel() const;
  int            vertexSize() const;
      
  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );
  
  signals:
  void lastAppliedChanged(); // emitted when something related to material changes
  	 
  void cutAllowedRChanged(const VP1Interval&);
  void cutAllowedYChanged(const VP1Interval&);
  void cutAllowedZChanged(const VP1Interval&);
  void vertexSizeChanged(int);
	 
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
  // void possibleChange_vertexSize();
  void possibleChange_cutAllowedR();
  void possibleChange_cutAllowedY();
  void possibleChange_cutAllowedZ();
    
};

#endif


