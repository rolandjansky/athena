# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.MenuConfObj")

class TopoMenuDef:

    __slots__ = [ "algoname", "outputlines", "outputbits" ]
    def __init__(self, algoname, outputlines = None, outputbits = None):

        if outputlines and len(outputlines) == 1 and algoname == outputlines[0]:
            log.info("... outputlines can be ommitted for topo algo %s", algoname)

        if outputlines is None:
            outputlines = [ algoname ]
        # check that there is no ',' in any of the output names
        if any([',' in x for x in outputlines]):
            raise RuntimeError("One name in these outputlines contains a ',', which is not allowed: %r" % outputlines)
        self.algoname = algoname
        self.outputlines = outputlines
        self.outputbits = outputbits

    def __str__(self):
        return self.algoname

    def __repr__(self):
        return f"name = {self.algoname}, outputlines = {self.outputlines}, outputbits = {self.outputbits}"

    def isMultiplicityDefinition(self):
        return self.algoname.startswith("Mult")
