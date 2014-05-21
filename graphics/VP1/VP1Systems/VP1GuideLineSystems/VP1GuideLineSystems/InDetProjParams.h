/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class InDetProjParams                     //
//                                                            //
//  Description: This is our "database" of parameters for the //
//               inner detector surfaces".  In the future we  //
//               might wish to get this information from      //
//               geomodel instead.                            //
//                                                            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef INDETPROJPARAMS_H
#define INDETPROJPARAMS_H

class InDetProjParams {
public:

  static double surfacethickness() { return 0.3; }

  //Fixme: The pixel/sct parameters are not used at the moment, and
  //       are simply set to some silly values!

  static double pixel_barrel_inner_radius() { return 10; }
  static double pixel_barrel_outer_radius() { return 20; }
  static double pixel_barrel_posneg_z() { return 10; }
  static double pixel_endcap_surface_z() { return 100; }
  static double pixel_endcap_surface_length() { return 50; }
  static double pixel_endcap_inner_radius() { return 20; }
  static double pixel_endcap_outer_radius() { return 40; }
  static double pixel_endcap_zasr_disttobarrelcyl() { return 10; }//Not used
  static double pixel_endcap_zasr_squeezefact() { return 2.5; }//Not used
  static double pixel_data_disttosurface_epsilon() { return 0.5; }

  static double sct_barrel_inner_radius() { return 20; }
  static double sct_barrel_outer_radius() { return 40; }
  static double sct_barrel_posneg_z() { return 20; }
  static double sct_endcap_surface_z() { return 200; }
  static double sct_endcap_surface_length() { return 100; }
  static double sct_endcap_inner_radius() { return 40; }
  static double sct_endcap_outer_radius() { return 80; }
  static double sct_endcap_zasr_disttobarrelcyl() { return 20; }//Not used
  static double sct_endcap_zasr_squeezefact() { return 2.5; }//Not used
  static double sct_data_disttosurface_epsilon() { return 0.5; }

  static double trt_barrel_inner_radius() { return 540; }
  static double trt_barrel_outer_radius() { return 1100; }
  static double trt_barrel_posneg_z() { return 712.5+6; }
  static double trt_endcap_surface_z() { return 1778.5; }
  static double trt_endcap_surface_length() { return 2.0*(936.25+10); }
  static double trt_endcap_inner_radius() { return 643.7-4; }
  static double trt_endcap_outer_radius() { return 1004-9; }
  static double trt_endcap_zasr_disttobarrelcyl() { return 20.0; }
  static double trt_endcap_zasr_squeezefact() { return 2.5; }
  static double trt_data_disttosurface_epsilon() { return 11.0; }


  //Convenience:
  static double pixel_endcap_zasr_innerradius()
  {
    return pixel_barrel_outer_radius()+pixel_endcap_zasr_disttobarrelcyl();
  }
  static double sct_endcap_zasr_innerradius()
  {
    return sct_barrel_outer_radius()+sct_endcap_zasr_disttobarrelcyl();
  }
  static double trt_endcap_zasr_innerradius()
  {
    return trt_barrel_outer_radius()+trt_endcap_zasr_disttobarrelcyl();
  }

  static double pixel_endcap_zasr_endcapz_begin() { return pixel_endcap_surface_z()-0.5*pixel_endcap_surface_length(); }
  static double sct_endcap_zasr_endcapz_begin() { return sct_endcap_surface_z()-0.5*sct_endcap_surface_length(); }
  static double trt_endcap_zasr_endcapz_begin() { return trt_endcap_surface_z()-0.5*trt_endcap_surface_length(); }

private:
  InDetProjParams();
  ~InDetProjParams();
};

#endif

