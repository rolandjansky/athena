/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceTester.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAINTERFACETESTER_H
#define ANALYSISCALIBRATIONDATAINTERFACETESTER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceTool.h"
#include <string>

class AtlasDetectorID;
class Identifier;

namespace Analysis 
{

  /** @class CalibrationDataInterfaceTester

      Minimal test of the CalibrationDataInterfaceTool functionality.
      
      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  class CalibrationDataInterfaceTester : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       CalibrationDataInterfaceTester(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~CalibrationDataInterfaceTester();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      
       /** @brief jet collection name */
       std::string  m_jetCollection;

       /** @brief tagger name */
       std::string  m_tagger;

       /** @brief tagger operating point */
       std::string  m_operatingPoint;

       /** @brief calibration uncertainty result */
       std::string  m_calibrationUncertainty;
       CalibrationDataInterfaceTool::Uncertainty m_uncertaintyType;

       /** @brief pointer to the CalibrationDataInterfaceTool */
       ToolHandle<ICalibrationDataInterfaceTool>  m_calibrationInterface;

    }; 
} // end of namespace

#endif 
