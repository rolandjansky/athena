/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_H62004SIMPLESDTOOL_H
#define LARG4H6SD_H62004SIMPLESDTOOL_H

// LAr G4 includes
#include "LArG4Code/SimpleSDTool.h"

namespace LArG4
{

  /// @class H62004SimpleSDTool
  /// @brief Tool base class for LAr H6 2004 simple SD tools.
  ///
  /// This tool overrides the SD creation from SimpleSDTool to create
  /// the custom LArG4H62004SimpleSD objects.
  ///
  /// See LArG4::SimpleSDTool documentation for more details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class H62004SimpleSDTool : public SimpleSDTool
  {
    public:
      /// Constructor
      H62004SimpleSDTool(const std::string& type, const std::string& name,
                         const IInterface* parent);
    protected:
      /// Override helper method to create one SD
      std::unique_ptr<LArG4SimpleSD>
      makeOneSD(const std::string& name, ILArCalculatorSvc* calc,
                const std::vector<std::string>& volumes) const;
  };

} // namespace LArG4

#endif
