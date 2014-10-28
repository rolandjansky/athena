# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class TopoOutput(object):
    def __init__(self, algoname, module, fpga, clock, firstbit):
        self.algo = None
        self.sortingAlgos = []
        self.algoname = algoname
        self.module = module
        self.fpga = fpga
        self.clock = clock
        self.firstbit = firstbit

    def xml(self, ind, step=2):
        s = (' '*ind*step) + '<Output algname="%s" algoId="%i" module="%i" fpga="%i" firstbit="%i" clock="%i"/>\n' % (self.algoname,
                                                                                                                      self.algo.algoId,
                                                                                                                      self.module,
                                                                                                                      self.fpga,
                                                                                                                      self.firstbit,
                                                                                                                      self.clock)
        return s
