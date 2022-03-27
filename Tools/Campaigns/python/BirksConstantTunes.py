# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.Enums import ProductionStep

def MC21(flags, cfg):
    """This method overrides the values of Birks Constant to the values
expected to be used in MC21 and sets the sampling fractions
consistently. (Based on using G4 10.6.3.3.)"""
    if flags.Common.ProductionStep in [ProductionStep.Simulation, ProductionStep.FastChain]:
        ### LAr - Birks' law variation
        if flags.Detector.EnableLAr:
            EMBPresamplerCalculator = cfg.getService("EMBPresamplerCalculator")
            EMBPresamplerCalculator.BirksLaw = True
            EMBPresamplerCalculator.Birksk = 0.0486 * 1.2

            EMBCalculator = cfg.getService("EMBCalculator")
            EMBCalculator.BirksLaw = True
            EMBCalculator.Birksk = 0.0486 * 1.2

            EMECPosInnerWheelCalculator = cfg.getService("EMECPosInnerWheelCalculator")
            EMECPosInnerWheelCalculator.BirksLaw = True
            EMECPosInnerWheelCalculator.Birksk = 0.0486 * 1.2

            EMECNegInnerWheelCalculator = cfg.getService("EMECNegInnerWheelCalculator")
            EMECNegInnerWheelCalculator.BirksLaw = True
            EMECNegInnerWheelCalculator.Birksk = 0.0486 * 1.2

            EMECPosOuterWheelCalculator = cfg.getService("EMECPosOuterWheelCalculator")
            EMECPosOuterWheelCalculator.BirksLaw = True
            EMECPosOuterWheelCalculator.Birksk = 0.0486 * 1.2

            EMECNegOuterWheelCalculator = cfg.getService("EMECNegOuterWheelCalculator")
            EMECNegOuterWheelCalculator.BirksLaw = True
            EMECNegOuterWheelCalculator.Birksk = 0.0486 * 1.2

            EMECPresamplerCalculator = cfg.getService("EMECPresamplerCalculator")
            EMECPresamplerCalculator.BirksLaw = True
            EMECPresamplerCalculator.Birksk = 0.0486 * 1.2

            EMECPosBackOuterBarretteCalculator = cfg.getService("EMECPosBackOuterBarretteCalculator")
            EMECPosBackOuterBarretteCalculator.BirksLaw = True
            EMECPosBackOuterBarretteCalculator.Birksk = 0.0486 * 1.2

            EMECNegBackOuterBarretteCalculator = cfg.getService("EMECNegBackOuterBarretteCalculator")
            EMECNegBackOuterBarretteCalculator.BirksLaw = True
            EMECNegBackOuterBarretteCalculator.Birksk = 0.0486 * 1.2

        ### Tile - Birks' law variation
        if flags.Detector.EnableTile:
            import AthenaCommon.SystemOfUnits as Units
            gramsPerMeVcmSq = Units.g/(Units.MeV * Units.cm2)
            TileGeoG4SDCalc = cfg.getService("TileGeoG4SDCalc")
            TileGeoG4SDCalc.birk1 = 0.0130 * gramsPerMeVcmSq * 1.54
            TileGeoG4SDCalc.birk2 = 0.0

    if flags.Common.ProductionStep is not ProductionStep.Simulation:
        ### LAr Sampling Fraction Overrides
        if flags.Detector.EnableLAr:
            from IOVDbSvc.IOVDbSvcConfig import addOverride
            cfg.merge(addOverride(flags, "/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-G4106-v1-FTFP_BERT_BIRK"))

        ### Tile Sampling Fraction Overrides
        if flags.Detector.EnableTile:
            tileInfoLoader = cfg.getService("TileInfoLoader")
            tileInfoLoader.EmScaleA = 33.432914
