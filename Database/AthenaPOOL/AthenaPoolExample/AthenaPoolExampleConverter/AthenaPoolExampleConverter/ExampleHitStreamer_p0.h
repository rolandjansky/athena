/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHITSTREAMER_P0_H
#define ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHITSTREAMER_P0_H

/** @file ExampleHitStreamer_p0.h
 *  @brief This file contains the class definition for the ExampleHitStreamer_p0 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  @author Marcin Nowak <Marcin.Nowak@cern.ch>
 *  $Id: ExampleHitStreamer_p0.h,v 1.3 2006-06-23 18:55:36 gemmeren Exp $
 **/

#include "AthenaPoolServices/T_AthenaRootConverter.h"
#include "AthenaPoolExampleData/ExampleHit.h"
#include "ExampleHit_p0.h"

/** @class ExampleHitStreamer_p0
 *  @brief This class provides an example for a ROOT streamer.
 **/
class ExampleHitStreamer_p0 : public T_AthenaRootConverter<ExampleHit, ExampleHit_p0> {
public:
   ExampleHitStreamer_p0() {
      SetClassNameAndChecksum("ExampleHit", 306442982U);
   }
   void CopyOldToNew(const ExampleHit_p0& old_obj, ExampleHit& new_obj) {
      new_obj.setX(old_obj.m_x);
      new_obj.setY(old_obj.m_y);
      new_obj.setZ(old_obj.m_z);
      new_obj.setDetector( old_obj.m_detector );
   }
};
#endif
