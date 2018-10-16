/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class ProjectionSurfacesHelper            //
//                                                            //
//  Description: Helper class used to display surfaces for    //
//               either pixel, sct or trt.                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PROJECTIONSURFACESHELPER_H
#define PROJECTIONSURFACESHELPER_H

class SoSeparator;
class SoMaterial;

#include <QObject>
#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Utils/InDetProjFlags.h"

class ProjectionSurfacesHelper : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:


  //Use one of the following to create an appropriate helper
  //(projection surfaces will be put under a second internal separator
  //under attachsep):
  static ProjectionSurfacesHelper * createPixelHelper(SoMaterial*,SoSeparator * attachsep,QObject*parent=0,IVP1System* sys = 0);
  static ProjectionSurfacesHelper * createSCTHelper(SoMaterial*,SoSeparator * attachsep,QObject*parent=0,IVP1System* sys = 0);
  static ProjectionSurfacesHelper * createTRTHelper(SoMaterial*,SoSeparator * attachsep,QObject*parent=0,IVP1System* sys = 0);
  //(non-zero system pointers to let error messages appear in gui)

  virtual ~ProjectionSurfacesHelper();

  //To see what parts are shown or to access (e.g. to edit) the
  //material of the projection surfaces, use these two methods:
  InDetProjFlags::InDetProjPartsFlags shownParts() const;
  SoMaterial * material();

public slots:
  //Use this slot to control what parts are shown (default is none):
  void setSurfaces(InDetProjFlags::InDetProjPartsFlags);

private:

  ProjectionSurfacesHelper( double surfacethickness,
			    double barrel_inner_radius,
			    double barrel_outer_radius,
			    double barrel_posneg_z,
			    double endcap_surface_z,
			    double endcap_surface_length,
			    double endcap_inner_radius,
			    double endcap_outer_radius,
			    double endcap_zasr_disttobarrelcyl,
			    double endcap_zasr_squeezefact,
			    SoMaterial*, SoSeparator * attachsep, QObject * parent, IVP1System*sys );

  class Imp;
  Imp * m_d;

};

#endif
