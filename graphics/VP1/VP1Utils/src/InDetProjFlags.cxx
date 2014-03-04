/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class InDetProjFlags                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/InDetProjFlags.h"

void InDetProjFlags::projectionPartsGivenUsedDetectors( InDetProjPartsFlags& proj_pixel,
							InDetProjPartsFlags& proj_sct,
							InDetProjPartsFlags& proj_trt,
							const DetTypeFlags& useddets )
{
  InDetProjPartsFlags not_allbarrel = ~(BarrelPositive|BarrelNegative|BarrelCentral);
  InDetProjPartsFlags not_allendcap = ~(EndCapOuterPositive|EndCapOuterNegative
					|EndCapInnerPositive|EndCapInnerNegative
					|TRT_EndCapZToRPositive|TRT_EndCapZToRNegative
					|TRT_EndCapZToRCentral);
  InDetProjPartsFlags not_allendcappos = ~(EndCapOuterPositive|EndCapInnerPositive|TRT_EndCapZToRPositive);
  InDetProjPartsFlags not_allendcapneg = ~(EndCapOuterNegative|EndCapInnerNegative|TRT_EndCapZToRNegative);

  if (proj_pixel!=NoProjections) {
    if (useddets&Pixel_all) {
      if (!(useddets&(Pixel_brlpos|Pixel_brlneg))) {
	proj_pixel &= not_allbarrel;//No pixel barrel at all:
      } else {
	if (!(useddets&Pixel_brlpos)) proj_pixel &= ~BarrelPositive;//No pixel pos barrel
	if (!(useddets&Pixel_brlneg)) proj_pixel &= ~BarrelNegative;//No pixel neg barrel
      }
      if (!(useddets&(Pixel_ecpos|Pixel_ecneg))) {
	proj_pixel &= not_allendcap;//No pixel endcaps at all.
      } else {
	if (!(useddets&Pixel_ecpos)) proj_pixel &= not_allendcappos;//No pixel pos endcap
	if (!(useddets&Pixel_ecneg)) proj_pixel &= not_allendcapneg;//No pixel neg endcap
      }
    } else {
      proj_pixel = NoProjections;
    }
  }

  if (proj_sct!=NoProjections) {
    if (useddets&SCT_all) {
      if (!(useddets&(SCT_brlpos|SCT_brlneg))) {
	proj_sct &= not_allbarrel;//No sct barrel at all:
      } else {
	if (!(useddets&SCT_brlpos)) proj_sct &= ~BarrelPositive;//No sct pos barrel
	if (!(useddets&SCT_brlneg)) proj_sct &= ~BarrelNegative;//No sct neg barrel
      }
      if (!(useddets&(SCT_ecpos|SCT_ecneg))) {
	proj_sct &= not_allendcap;//No sct endcaps at all.
      } else {
	if (!(useddets&SCT_ecpos)) proj_sct &= not_allendcappos;//No sct pos endcap
	if (!(useddets&SCT_ecneg)) proj_sct &= not_allendcapneg;//No sct neg endcap
      }
    } else {
      proj_sct = NoProjections;
    }
  }

  if (proj_trt!=NoProjections) {
    if (useddets&TRT_all) {
      if (!(useddets&(TRT_brlpos|TRT_brlneg))) {
	proj_trt &= not_allbarrel;//No trt barrel at all:
      } else {
	if (!(useddets&TRT_brlpos)) proj_trt &= ~BarrelPositive;//No trt pos barrel
	if (!(useddets&TRT_brlneg)) proj_trt &= ~BarrelNegative;//No trt neg barrel
      }
      if (!(useddets&(TRT_ecpos|TRT_ecneg))) {
	proj_trt &= not_allendcap;//No trt endcaps at all.
      } else {
	if (!(useddets&TRT_ecpos)) proj_trt &= not_allendcappos;//No trt pos endcap
	if (!(useddets&TRT_ecneg)) proj_trt &= not_allendcapneg;//No trt neg endcap
      }
    } else {
      proj_trt = NoProjections;
    }
  }
}
