/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_IPARTICLECOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_IPARTICLECOLLSETTINGSBUTTON_H

#include "VP1Base/VP1MaterialButton.h"

class SoDrawStyle;
class SoLightModel;

class IParticleCollectionSettingsButton : public VP1MaterialButtonBase {

  Q_OBJECT

public:
  
  IParticleCollectionSettingsButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
  virtual ~IParticleCollectionSettingsButton();
  
  void setDimension(int dim);
  void initEditWindow();
  
  // The below are necessary to fulfill the interface, but will just be passed onto the VP1MaterialButton owned by the IParticleCollectionSettingsButton form
  virtual bool setMaterial(SoMaterial*);
  virtual void copyValuesFromMaterial(SoMaterial*);
  virtual double lastAppliedTransparency() const ;
  virtual double lastAppliedShininess() const  ;
  virtual double lastAppliedBrightness() const;
 

  // GUI for cuts
  VP1Interval cutAllowedPt() const;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
	
	//Overall drawstyles, complexities and light models:
	SoDrawStyle * drawStyle() const;
	SoLightModel * lightModel() const;
    
  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );
  
  signals:
    void lastAppliedChanged(); // emitted when something related to material changes
    
    void cutAllowedPtChanged(const VP1Interval&);
    void cutAllowedEtaChanged(const VP1Interval&);
    void cutAllowedPhiChanged(const QList<VP1Interval>&);
    
  public slots:
    void showEditMaterialDialog();
    virtual void setText(const QString&);

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
		void updateDrawStyle();
		void updateLightModel(bool);
    void possibleChange_cutAllowedPt();
    void possibleChange_cutAllowedEta();
    void possibleChange_cutAllowedPhi();
};

#endif


