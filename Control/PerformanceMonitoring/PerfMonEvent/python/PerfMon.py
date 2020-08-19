# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @author: Sebastien Binet <binet@cern.ch>
# @date:   April 2007
#
#

__version__ = "$Revision: 1.8 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

__all__ = [ 
            'CpuHdr',  'IoHdr',  'MemHdr',  'PersHdr',
            'CpuData', 'IoData', 'MemData', #'PersData',
            ]

import cppyy

cppyy.load_library( 'libPerfMonEventDict' )

PerfMon = cppyy.makeNamespace( 'PerfMon' )

CpuHdr  = PerfMon.CpuHdr
IoHdr   = PerfMon.IoHdr
MemHdr  = PerfMon.MemHdr
PersHdr = PerfMon.PersHdr

CpuData  = PerfMon.CpuData
IoData   = PerfMon.IoData
MemData  = PerfMon.MemData
#PersData = PerfMon.PersData
