#!/bin/env python

import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf

from TriggerMenu.l1topo.TopoAlgos import SortingAlgo, DecisionAlgo

def instantiateExample():

    alg = AlgConf.DeltaPhiIncl1(name = 'JetSize1DeltaPhiIncl_12', inputs = ['SortedJetsSize1'], outputs = ['JetDeltaPhiNarrow','JetDeltaPhiWide'])
    alg.addgeneric('NumberLeading1', 4)
    alg.addgeneric('NumberLeading2', 4)
    alg.addvariable('MinET', 5)
    alg.addvariable('DeltaPhiMin', 20, 0)
    alg.addvariable('DeltaPhiMax', 27, 0)
    alg.addvariable('DeltaPhiMin', 27, 1)
    alg.addvariable('DeltaPhiMax', 32, 1)


def main():
    sortalg = []
    decalg = []
    for classname in dir(AlgConf):
        klass = getattr(AlgConf,classname)
        if type(klass)!=type(type): continue
        if issubclass(klass,SortingAlgo): sortalg += [klass]
        if issubclass(klass,DecisionAlgo): decalg += [klass]

    instantiateExample()

    return 0


if __name__=="__main__":
    import sys
    main()
    #try:
    #    sys.exit(main())
    #except Exception, e:
    #    print "Caught exception",e
    #    sys.exit(1)
