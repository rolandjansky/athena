# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys
import traceback



def exc2string2():
    """Provide traceback ehen an exception has been raised"""
    llist = sys.exc_info()
    errmsg = str(llist[0])
    errmsg += str(llist[1])
    errmsg += ' '.join(traceback.format_tb(llist[2]))
    return errmsg



try:
    from TriggerMenu.commonUtils.LeptonIsoEDConfig import TrigHLTEnergyDensityCentral, TrigHLTEnergyDensityForward

    from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_jet_fullcalo,
                                            TrigCaloClusterMaker_topo)

    from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_eGamma,
                                            TrigCaloTowerMaker_eGamma)
    from TrigEgammaRec.TrigEgammaToolFactories import TrigCaloClusterMaker_slw
    
except Exception:
    print ('makeCaloSequences: import failed')
    traceback.print_exc()
    sys.exit(1)

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
    
    #   from TrigGenericAlgs.GenericDummyUnseededAllTEAlgoConfig import GenericDummyUnseededAllTEAlgo as DummyAlgp
    
    from TrigGenericAlgs.GenericDummyUnseededAllTEAlgoConfig import GenericDummyUnseededAllTEAlgo 

    alglist = [ GenericDummyUnseededAllTEAlgo('RoiCreator'),
                TrigCaloCellMaker_jet_fullcalo('TriggerCaloCellMaker_FS',
                                               doNoise=0,
                                               AbsE=True,
                                               doPers=True),
                TrigCaloClusterMaker_topo('TopoCaloClusterMaker_topo_FS',
                                          doMoments=True,
                                          doLC=True)]
    return SequenceSpecifier(te_in, te_out, alglist).generate()


def getFullScanCaloSequences():
    ''' 
    Common utility to configure full scan calo algorithms.
    To profit from caching, the sequence needs to be broken in steps.
    Therefore, this function provides a map where the keys are the
    outputTE names that are used amongst clients (and easily accesible to remap dict)
    In addition, provide the ED algorithm for isolation chains
    For isolation chains, these need to also add to sequence 
    the dummy merger, to merge full scan back to RoI sequence (not available here)
    '''
    import collections
    caloSeqMap = collections.OrderedDict()

    from TrigGenericAlgs.GenericDummyUnseededAllTEAlgoConfig import GenericDummyUnseededAllTEAlgo # as DummyAlgp

    theDummyRoiCreator = GenericDummyUnseededAllTEAlgo('RoiCreator')

    cellMaker=TrigCaloCellMaker_jet_fullcalo('TriggerCaloCellMaker_FS',
                                              doNoise=0,
                                              AbsE=True,
                                              doPers=True)
    topoMaker=TrigCaloClusterMaker_topo('TopoCaloClusterMaker_topo_FS',
                                         doMoments=True,
                                         doLC=True)
    ed_alg_list = [TrigHLTEnergyDensityCentral("TrigHLTEnergyDensityCentral"), 
                    TrigHLTEnergyDensityForward("TrigHLTEnergyDensityForward")]

    caloSeqMap['EF_full']=[theDummyRoiCreator]
    caloSeqMap['EF_full_cell']=[cellMaker]
    caloSeqMap['EF_FSTopoClusters']=[topoMaker]
    caloSeqMap['EF_FSTopoClustersED'] = ed_alg_list

    return caloSeqMap

def getFullScanTopoClusterEDSequences():
    theFullScanTopoClusterSequence = fullScanTopoClusterSequence()
    theEnergyDensitySequence = EnergyDensitySequence(theFullScanTopoClusterSequence[-1])
    return [theFullScanTopoClusterSequence,theEnergyDensitySequence]

def EgammaSlwClusterSequence(te_in,te_out):
    
    alglist = [TrigCaloCellMaker_eGamma(),TrigCaloTowerMaker_eGamma(),TrigCaloClusterMaker_slw()]
    return SequenceSpecifier(te_in, te_out, alglist).generate()

if __name__ == '__main__':
    seq = fullScanTopoClusterSequence()
    print (seq)

    seq2 = EgammaSlwClusterSequence("EF_step0","EF_step1")
    print (seq2)
