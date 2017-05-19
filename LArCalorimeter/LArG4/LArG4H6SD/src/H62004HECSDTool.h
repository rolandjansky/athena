/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004SD_H62004HECSDTOOL_H
#define LARG4H62004SD_H62004HECSDTOOL_H

// System includes
#include <string>

// Local includes
#include "H62004SimpleSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"
namespace LArG4
{

  /// @class H62004HECSDTool
  /// @brief Tool for constructing H62004 SDs for HEC.
  ///
  /// Based on the previous LArG4H62004HECSDTool implementation.
  ///
  /// This implementation uses the LAr SD wrapper design for managing multiple
  /// SDs when running multi-threaded. See ATLASSIM-2606 for discussions.
  ///
  class H62004HECSDTool : public H62004SimpleSDTool
  {

    public:

      /// Constructor
      H62004HECSDTool(const std::string& type, const std::string& name,
                      const IInterface* parent);

    private:

      StatusCode initializeCalculators() override final;

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// Hit collection name
      std::string m_hitCollName;
    ServiceHandle<ILArCalculatorSvc> m_calculator;
  }; // class H62004HECSDTool

} // namespace LArG4

#endif
