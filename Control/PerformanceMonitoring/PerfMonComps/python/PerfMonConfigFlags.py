# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createPerfMonConfigFlags():
    pcf = AthConfigFlags()

    # Two basic flags for PerfMonMT
    pcf.addFlag('PerfMon.doFastMonMT', False)
    pcf.addFlag('PerfMon.doFullMonMT', False)
    
    return pcf
