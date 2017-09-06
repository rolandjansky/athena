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

  ProjectionSurfacesHelper( const double& surfacethickness,
			    const double& barrel_inner_radius,
			    const double& barrel_outer_radius,
			    const double& barrel_posneg_z,
			    const double& endcap_surface_z,
			    const double& endcap_surface_length,
			    const double& endcap_inner_radius,
			    const double& endcap_outer_radius,
			    const double& endcap_zasr_disttobarrelcyl,
			    const double& endcap_zasr_squeezefact,
			    SoMaterial*, SoSeparator * attachsep, QObject * parent, IVP1System*sys );

  class Imp;
  Imp * d;

};

#endif
