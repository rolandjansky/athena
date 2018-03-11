# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def firstSimCreatedBarcode():
    "Return the simulation barcode offset for G4 particles from metadata"
    offset = 200e3

    # Don't try to run the input peeker for a generator job; it will fail
    # without a proper input file.
    # Is there a better way of doing this test?
    from AthenaCommon.AppMgr import theApp
    if theApp.EvtSel == 'McEventSelector/EventSelector':
        print 'Generator job: leaving SimBarcodeOffset at 200k'

    else:
        from RecExConfig.InputFilePeeker import inputFileSummary
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
