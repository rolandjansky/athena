# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("DigitizationTests.DigitizationTestsConfig.McEventCollectionTestTool",
        "McEventCollectionTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.EventInfoTestTool",
        "EventInfoTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.PixelRDOsTestTool",
        "PixelRDOsTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.SCT_RDOsTestTool",
        "SCT_RDOsTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.TRT_RDOsTestTool",
        "TRT_RDOsTestTool")
