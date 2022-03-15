/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/src/HGTD_FrontEndTool.cxx
 *
 * @author Tao Wang <tao.wang@cern.ch>
 *
 * @date September, 2021
 */

#include "HGTD_FrontEndTool.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandPoisson.h"

#include <algorithm>
#include <iostream>

HGTD_FrontEndTool::HGTD_FrontEndTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : AthAlgTool(type, name, parent) {
  declareInterface<IFrontEnd>(this);
}

void HGTD_FrontEndTool::process(
    SiChargedDiodeCollection& /*collection*/,
    CLHEP::HepRandomEngine* /*rndmEngine*/) const {

  // FIXME only commented out for now, Tao will use this when adding his code
  // if (collection.empty()) {
  // ATH_MSG_DEBUG(
  // "HGTD_FrontEndTool::process() Empty Charged Diode Collection");
  // return;
  // }
  // for each of the charged diodes, set the time after creating the time pulse
  // SiChargedDiodeIterator i_chargedDiode = collection.begin();
  // SiChargedDiodeIterator i_chargedDiode_end = collection.end();

  // for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
  // SiChargedDiode& diode = (*i_chargedDiode).second;
  // check if the charge is above threshold
  // I cut on minimal eloss in active region, using a value agreed on by HGTD
  // Sensor as well as ASIC group

  // generate the pulse

  // apply the discriminator

  // set the new time
  // }
}
