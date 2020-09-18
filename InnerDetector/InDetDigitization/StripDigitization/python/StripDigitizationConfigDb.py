# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool,addService,addAlgorithm
addTool("StripDigitization.StripDigitizationConfig.getStripDigitizationTool"       , "StripDigitizationTool")
addTool("StripDigitization.StripDigitizationConfig.getStripAmp"                    , "StripAmp" )
addTool("StripDigitization.StripDigitizationConfig.getStripFrontEnd"               , "StripFrontEnd" )
addTool("StripDigitization.StripDigitizationConfig.getStripSurfaceChargesGenerator", "StripSurfaceChargesGenerator" )

