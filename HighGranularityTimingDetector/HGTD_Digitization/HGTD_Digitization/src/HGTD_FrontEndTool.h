/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_FrontEndTool.h
 *
 * @author Tao Wang <tao.wang@cern.ch>
 *
 * @date September, 2021
 *
 * @brief Simulation of the HGTD front-end electronics
 *
 * TODO: this class doesn't do anything yet, Tao will add the pulse shape!
 */

#ifndef HGTD_DIGITIZATION_HGTD_FRONTEND_H
#define HGTD_DIGITIZATION_HGTD_FRONTEND_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SiDigitization/IFrontEnd.h"

#include "GaudiKernel/ToolHandle.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

class HGTD_FrontEndTool : public AthAlgTool, virtual public IFrontEnd {

public:
  /**  constructor */
  HGTD_FrontEndTool(const std::string& type, const std::string& name,
                    const IInterface* parent);
  /** Destructor */
  virtual ~HGTD_FrontEndTool() {}

  void process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine* rndmEngine) const;

};

#endif
