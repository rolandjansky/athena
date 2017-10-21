/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class InDetProjHelper                     //
//                                                            //
//  Description: Helper class for calculating projections     //
//               of data to inner detector surfaces.          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef INDETPROJHELPER_H
#define INDETPROJHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Utils/InDetProjFlags.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"


#include <vector>
#include <set>




class InDetProjHelper : public VP1HelperClassBase {
public:

  //Use one of the following to create an appropriate helper:
  static InDetProjHelper * createPixelHelper(IVP1System* sys = 0);
  static InDetProjHelper * createSCTHelper(IVP1System* sys = 0);
  static InDetProjHelper * createTRTHelper(IVP1System* sys = 0);
  //(non-zero system pointers to let error messages appear in gui)

  virtual ~InDetProjHelper();

  InDetProjFlags::InDetProjPartsFlags setParts(InDetProjFlags::InDetProjPartsFlags);//returns old value
  InDetProjFlags::InDetProjPartsFlags parts() const;

  //Clip path to volumes of the enabled detector parts, as given by the parts flag.
  void clipPath( const std::vector<Amg::Vector3D >& path, Amg::SetVectorVector3D& resulting_subpaths ) const;

  void clipPath( const std::vector<Amg::Vector3D >& path,
		 Amg::SetVectorVector3D& resulting_subpaths_barrelA,
		 Amg::SetVectorVector3D& resulting_subpaths_barrelC,
		 Amg::SetVectorVector3D& resulting_subpaths_endcapA,
		 Amg::SetVectorVector3D& resulting_subpaths_endcapC ) const;

  //Clip path as above, and then project the resulting subpaths to surfaces as given by the current parts flag.
  void projectPath( const std::vector<Amg::Vector3D >& path, Amg::SetVectorVector3D& resulting_projections ) const;
  void projectPath( const std::vector<Amg::Vector3D >& path,
		    Amg::SetVectorVector3D& resulting_projections_barrelA,
		    Amg::SetVectorVector3D& resulting_projections_barrelC,
		    Amg::SetVectorVector3D& resulting_projections_endcapA,
		    Amg::SetVectorVector3D& resulting_projections_endcapC ) const;

  //To figure out which parts a given track crosses (this does NOT depend on the current InDetProjPartsFlags setting).
  enum PartsFlag { NoParts = 0x00, BarrelA = 0x01, BarrelC = 0x02, EndCapA = 0x04, EndCapC = 0x08 };
  Q_DECLARE_FLAGS(PartsFlags, PartsFlag)
  PartsFlags touchedParts( const std::vector<Amg::Vector3D >& path ) const;

  //Fixme: Projection of points? Projection of driftcircle (get normal and point? Or just get transform?)

  static void transformECPointToZPlane_specialZtoR(Amg::Vector3D& p,
						   const double& planeZ,
						   const double& planeRBegin,
						   const double& endcapZBegin,
						   const double& squeezeFactor );

private:

  InDetProjHelper( double surfacethickness,
		   double data_disttosurface_epsilon,
		   double barrel_inner_radius,
		   double barrel_outer_radius,
		   double barrel_posneg_z,
		   double endcap_surface_z,
		   double endcap_surface_length,
		   double endcap_inner_radius,
		   double endcap_outer_radius,
		   double endcap_zasr_innerradius,
		   double endcap_zasr_endcapz_begin,
		   double endcap_zasr_squeezefact,
		   IVP1System* sys );



  class Imp;
  Imp * m_d;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(InDetProjHelper::PartsFlags)

#endif
