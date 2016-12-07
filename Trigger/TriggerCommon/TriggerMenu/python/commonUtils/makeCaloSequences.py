# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os

# stop noise from imports - output to /dev/null

oldout = sys.stdout
olderr = sys.stderr

f = open(os.devnull, 'w')
sys.stdout = f
sys.stderr = f

try:
    from TriggerMenu.commonUtils.LeptonIsoEDConfig import TrigHLTEnergyDensityCentral, TrigHLTEnergyDensityForward

    from TrigGenericAlgs.TrigGenericAlgsConf import \
        PESA__DummyUnseededAllTEAlgo as DummyAlgo
    
    from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_jet_fullcalo,
                                            TrigCaloClusterMaker_topo)

    from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_eGamma,
                                            TrigCaloTowerMaker_eGamma,
                                            TrigCaloCellMaker_eGamma_cells)
    from TrigEgammaRec.TrigEgammaToolFactories import TrigCaloClusterMaker_slw
    
    # import BadImportWillFail
except Exception, e:
    # reinstate output
    sys.stdout = oldout
    sys.stderr = olderr

    print 'makeCaloSequences: import failed'
    print e
    sys.exit(1)
else:
    # reinstate output
    sys.stdout = oldout
    sys.stderr = olderr
    
    

class SequenceSpecifier(object):
    def __init__(self, te_in, te_out, alglist):
        self.te_in = te_in
        self.te_out = te_out
        self.alglist = alglist

    def __str__(self):
        s = ['SequenceSpecifier: te_in %s te_out %s' % (self.te_in,
                                                        self.te_out)]
        s.append(' Algorithms:')
        [s.append('    ' + a.getName()) for a in self.alglist]
        return '\n'.join(s)

    def generate(self):
        s=[[self.te_in],self.alglist,self.te_out]
        return s

def EnergyDensitySequence(te_in):
    te_out = 'EF_FSTopoClustersED'
    alglist = [TrigHLTEnergyDensityCentral("TrigHLTEnergyDensityCentral"), TrigHLTEnergyDensityForward("TrigHLTEnergyDensityForward")]
    return SequenceSpecifier(te_in, te_out, alglist).generate()


def fullScanTopoClusterSequence():
    te_in = ''
    te_out = 'EF_FSTopoClusters'

    alglist = [DummyAlgo('RoiCreator'),
               TrigCaloCellMaker_jet_fullcalo('TriggerCaloCellMaker_FS',
                                              doNoise=0,
                                              AbsE=True,
                                              doPers=True),
               TrigCaloClusterMaker_topo('TopoCaloClusterMaker_topo_FS',
                                         doMoments=True,
                                         doLC=True)]
    return SequenceSpecifier(te_in, te_out, alglist).generate()


def getFullScanTopoClusterEDSequences():
    theFullScanTopoClusterSequence = fullScanTopoClusterSequence()
    theEnergyDensitySequence = EnergyDensitySequence(theFullScanTopoClusterSequence[-1])
    return [theFullScanTopoClusterSequence,theEnergyDensitySequence]

def EgammaSlwClusterSequence(te_in,te_out):
    
    alglist = [TrigCaloCellMaker_eGamma(),TrigCaloTowerMaker_eGamma(),TrigCaloClusterMaker_slw()]
    return SequenceSpecifier(te_in, te_out, alglist).generate()

if __name__ == '__main__':
    seq = fullScanTopoClusterSequence()
    print seq

    seq2 = EgammaSlwClusterSequence("EF_step0","EF_step1")
    print seq2
