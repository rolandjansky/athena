# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("DigitizationTests.DigitizationTestsConfig.McEventCollectionTestTool", "McEventCollectionTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.PileUpEventInfoTestTool", "PileUpEventInfoTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.PixelRDOsTestTool", "PixelRDOsTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.SCT_RDOsTestTool", "SCT_RDOsTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.TRT_RDOsTestTool", "TRT_RDOsTestTool")

#For HepMcParticleLink InDetSimData checks
addTool("DigitizationTests.DigitizationTestsConfig.InDetSDOMapTestTool",   "InDetSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.PixelSDOMapTestTool",   "PixelSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.PixelPUSDOMapTestTool", "PixelPUSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.SCTSDOMapTestTool",   "SCTSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.SCTPUSDOMapTestTool", "SCTPUSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.TRTSDOMapTestTool",   "TRTSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.TRTPUSDOMapTestTool", "TRTPUSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.BCMSDOMapTestTool",   "BCMSDOMapTestTool")
addTool("DigitizationTests.DigitizationTestsConfig.BCMPUSDOMapTestTool", "BCMPUSDOMapTestTool")
