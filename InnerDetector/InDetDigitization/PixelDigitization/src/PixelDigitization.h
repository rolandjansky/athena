/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file PixelDigitization/PixelDigitization.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief Interface of pixel digitization
 */

#ifndef PIXELDIGITIZATION_PIXELDIGITIZATION_H
#define PIXELDIGITIZATION_PIXELDIGITIZATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "PileUpTools/IPileUpTool.h"

class PixelDigitization: public AthAlgorithm {
public:
  PixelDigitization(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PixelDigitization() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final {return true;}
private:
  ToolHandle<IPileUpTool> m_pixelDigitizationTool {
    this, "DigitizationTool", "PixelDigitizationTool", "PixelDigitizationTool name"
  };
};

#endif // PIXELDIGITIZATION_PIXELDIGITIZATION_H
