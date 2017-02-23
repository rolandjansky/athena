/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4CODE_SIMPLESDTOOL_H
#define LARG4CODE_SIMPLESDTOOL_H

/// @file SimpleSDTool.h
/// @brief Defines the SimpleSDTool class
/// @author Steve Farrell <Steven.Farrell@cern.ch>
/// @date 2016-03-26

// System includes
#include <string>
#include <vector>

// G4Atlas includes
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Local includes
#include "LArG4SimpleSD.h"

// Forward declarations
class ILArCalculatorSvc;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class LArG4SimpleSD;

namespace LArG4
{

  /// @class SimpleSDTool
  /// @brief A base class for tools that manage LArG4SimpleSDs.
  ///
  /// @todo Add more details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class SimpleSDTool : public SensitiveDetectorBase
  {

    public:

      /// Constructor
      SimpleSDTool(const std::string& type, const std::string& name,
                   const IInterface* parent);

      /// Initialize the tool
      StatusCode initialize() override final;

      /// Calls down to all the SDs to pack their hits into one collection
      StatusCode Gather() override final;

    protected:

      /// Initialize Calculator Services
      virtual StatusCode initializeCalculators() { return StatusCode::SUCCESS; }

      /// Helper method to create one SD
      std::unique_ptr<LArG4SimpleSD>
      makeOneSD(const std::string& name, ILArCalculatorSvc* calc,
                const std::vector<std::string>& volumes) const;

      /// Are we handling frozen shower fast sim?
      bool useFrozenShowers() const { return m_useFrozenShowers; }

    protected:

      /// @name configuration properties
      /// @{

      /// What time binning type for regular hits?
      std::string m_timeBinType;
      /// What time bin width for regular hits?
      float m_timeBinWidth;
      /// Is there going to be a fast simulation coming into this SD?
      G4bool m_useFrozenShowers;

      /// @}

      /// @name Calo identifier helpers
      /// @{
      const LArEM_ID*       m_larEmID;
      const LArFCAL_ID*     m_larFcalID;
      const LArHEC_ID*      m_larHecID;
      const LArMiniFCAL_ID* m_larMiniFcalID;
      /// @}

  }; // class SimpleSDTool

} // namespace LArG4

#endif
