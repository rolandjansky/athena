# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def firstSimCreatedBarcode():
    "Return the simulation barcode offset for G4 particles from metadata"
    from RecExConfig.InputFilePeeker import inputFileSummary
    offset = 200e3
    try:
        offset = int(inputFileSummary['metadata']['/Simulation/Parameters']['SimBarcodeOffset'])
    except:
        print 'Could not retrieve SimBarcodeOffset from /Simulation/Parameters, leaving at 200k'
    return int(offset + 1)


def getMCTruthClassifier(name="MCTruthClassifier", **kwargs):
    kwargs.setdefault("barcodeG4Shift", firstSimCreatedBarcode())
    return CfgMgr.MCTruthClassifier(name, **kwargs)

def getJetMCTruthClassifier(name="JetMCTruthClassifier", **kwargs):
    kwargs.setdefault("ParticleCaloExtensionTool","")
    return getMCTruthClassifier(name, **kwargs)
