/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSD_FCS_STEPINFOSDTOOL_H
#define ISF_FASTCALOSIMSD_FCS_STEPINFOSDTOOL_H

/// @file FCS_StepInfoSDTool.h
/// @brief Defines the FCS_StepInfoSDTool class
/// @author Steve Farrell <Steven.Farrell@cern.ch>
/// @date 2016-03-26

// System includes
#include <string>
#include <vector>

// G4Atlas includes
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Local includes
#include "FCS_StepInfoSD.h"

// Forward declarations
class ILArCalculatorSvc;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class FCS_StepInfoSD;

namespace FCS_Param
{

  /// @class FCS_StepInfoSDTool
  /// @brief A base class for tools that manage FCS_StepInfoSDs.
  ///
  /// @todo Add more details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class FCS_StepInfoSDTool : public SensitiveDetectorBase
  {

    public:

      /// Constructor
      FCS_StepInfoSDTool(const std::string& type, const std::string& name,
                   const IInterface* parent);

      /// Initialize the tool
      StatusCode initialize() override final;

      /// Calls down to all the SDs to pack their hits into one collection
      StatusCode Gather() override final;

    protected:

      /// Initialize Calculator Services
      virtual StatusCode initializeCalculators() { return StatusCode::SUCCESS; }

      /// Helper method to create one SD
      std::unique_ptr<FCS_StepInfoSD>
      makeOneSD(const std::string& name, ILArCalculatorSvc* calc,
                const std::vector<std::string>& volumes) const;

    protected:


      /// @name Calo identifier helpers
      /// @{
      const LArEM_ID*       m_larEmID;
      const LArFCAL_ID*     m_larFcalID;
      const LArHEC_ID*      m_larHecID;
      const LArMiniFCAL_ID* m_larMiniFcalID;
      /// @}
    FCS_StepInfoSD::Config m_config;

  }; // class FCS_StepInfoSDTool

} // namespace FCS_Param

#endif
