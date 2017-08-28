/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1MaterialButton                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: September 2007                        //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1MATERIALBUTTON_H
#define VP1MATERIALBUTTON_H

#include "VP1Base/VP1HelperClassBase.h"

#include <QPushButton>

class SoMaterial;
class IVP1System;

class VP1MaterialButtonBase : public QPushButton, public VP1HelperClassBase{
  Q_OBJECT
  public:
    VP1MaterialButtonBase(QWidget* parent, IVP1System * sys = 0, QString helpername = "")
     : QPushButton(parent),VP1HelperClassBase(sys,helpername){};

   virtual bool setMaterial(SoMaterial*)=0;
   virtual void copyValuesFromMaterial(SoMaterial*)=0;
   virtual double lastAppliedTransparency() const=0;
   virtual double lastAppliedShininess() const=0 ;
   virtual double lastAppliedBrightness() const=0;

   virtual QByteArray saveState() const =0; //!< fill out with the state of the object (used for drag and drop etc)
   virtual void restoreFromState( const QByteArray& )=0 ;

  virtual ~VP1MaterialButtonBase() {}
  signals:
    void lastAppliedChanged();
};


class VP1MaterialButton : public VP1MaterialButtonBase {

  Q_OBJECT

public:
  //////////////////////////////////////////////////////////////////////
  //
  // Static methods for creating/editing "simple materials" (those
  // edited in the simple mode of this button). Most typical use case
  // is to first call createSimpleMaterial(..) to create a new simple
  // material, and then subsequently connect it to some material
  // button by the setMaterial(..) method:

  static SoMaterial* createMaterial( const QColor&, const double& brightness = 0.0, const double& transp = 0.0 );
  static SoMaterial* createMaterial( const double& r, const double& g, double b, const double& brightness = 0.0,const double& transp = 0.0 );
  static void setMaterialParameters( SoMaterial * m, const QColor&, const double& brightness = 0.0, const double& transp = 0.0 );
  static void setMaterialParameters( SoMaterial * m, const double& r, const double& g, const double& b, const double& brightness = 0.0, const double& transp = 0.0 );

  //////////////////////////////////////////////////////////////////////

  VP1MaterialButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
  virtual ~VP1MaterialButton();

  bool handleMaterial(SoMaterial *); //Returns false if already handling or null.
  bool stopHandlingMaterial(SoMaterial *);//Returns false if didn't handle or null.
  void clearHandledMaterials();

  void copyValuesFromMaterial(SoMaterial*);

  bool setMaterial(SoMaterial*);//calls copyValuesFromMaterial and handleMaterial in that order:

  QList<SoMaterial*> handledMaterials() const;

  void setText ( const QString &  );//Forbidden!! Only here since Designer generated code needs it in public.

  void setDimension(int dim);

  QColor lastAppliedDiffuseColour() const;
  QColor lastAppliedAmbientColour() const;
  QColor lastAppliedSpecularColour() const;
  QColor lastAppliedEmissiveColour() const;
  double lastAppliedTransparency() const;
  double lastAppliedShininess() const;
  double lastAppliedBrightness() const;//Redundant

  QWidget& editWindow() ;

  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );

signals:
  void lastAppliedChanged(); // emitted when something changes

public slots:
  void showEditMaterialDialog();
  void setMaterialText(const QString&);//If set to XXX, title will display "Edit Material XXX"

protected slots:
  void updatePreview();
  void updatePreviewSceneAndBgd();
  void apply();
  void reset();
  void updateButton();
  void switchMode();
  void transparencyChanged();

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void dropEvent(QDropEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
private:

  class Imp;
  Imp * d;

};

#endif
