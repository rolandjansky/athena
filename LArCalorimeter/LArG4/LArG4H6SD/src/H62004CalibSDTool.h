/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_H62004CALIBSDTOOL_H
#define LARG4H6SD_H62004CALIBSDTOOL_H

// LAr G4 includes
#include "LArG4Code/CalibSDTool.h"

namespace LArG4
{

  /// @class H62004CalibSDTool
  /// @brief Tool base class for LAr H6 2004 calibration SD tools.
  ///
  /// This tool overrides the SD creation from CalibSDTool to create
  /// the custom LArG4H62004CalibSD objects.
  ///
  /// See LArG4::CalibSDTool documentation for more details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class H62004CalibSDTool : public CalibSDTool
  {
    public:
      /// Constructor
      H62004CalibSDTool(const std::string& type, const std::string& name,
                         const IInterface* parent);
    protected:
      /// Override helper method to create one SD
      std::unique_ptr<LArG4CalibSD>
      makeOneSD(const std::string& name, ILArCalibCalculatorSvc* calc,
                const std::vector<std::string>& volumes) const;
  };

} // namespace LArG4

#endif
