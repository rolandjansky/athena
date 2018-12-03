/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4CODE_CALIBSDTOOL_H
#define LARG4CODE_CALIBSDTOOL_H

/// @file CalibSDTool.h
/// @brief Defines the CalibSDTool class
/// @author Steve Farrell <Steven.Farrell@cern.ch>
/// @date 2016-03-26

// System includes
#include <string>
#include <vector>

// G4Atlas includes
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Local includes
#include "LArG4CalibSD.h"

// Forward declarations
class ILArCalibCalculatorSvc;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class CaloDM_ID;
class LArG4CalibSD;

namespace LArG4
{

  /// @class CalibSDTool
  /// @brief A base class for tools that manage LArG4CalibSDs.
  ///
  /// @todo Add more details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class CalibSDTool : public SensitiveDetectorBase
  {

    public:

      /// Constructor
      CalibSDTool(const std::string& type, const std::string& name,
                  const IInterface* parent);

      /// Initialize the tool
      StatusCode initialize() override final;

      /// Calls down to all the SDs to pack their hits into one collection
      StatusCode Gather() override final;

    protected:

      /// Initialize Calculator Services
      virtual StatusCode initializeCalculators() { return StatusCode::SUCCESS; }

      /// Helper method to create one SD
      std::unique_ptr<LArG4CalibSD>
      makeOneSD(const std::string& name, ILArCalibCalculatorSvc* calc,
                const std::vector<std::string>& volumes) const;

    protected:

      /// @name configuration properties
      /// @{

      /// Are we set up to run with PID hits?
      G4bool m_doPID;

      /// @}

      /// @name Calo identifier helpers
      /// @{
      const LArEM_ID*       m_larEmID;
      const LArFCAL_ID*     m_larFcalID;
      const LArHEC_ID*      m_larHecID;
      const LArMiniFCAL_ID* m_larMiniFcalID;
      const CaloDM_ID*      m_caloDmID;
      const AtlasDetectorID* m_id_helper;
      /// @}

  }; // class CalibSDTool

} // namespace LArG4

#endif
