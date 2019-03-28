# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Flags to control the L1 calo upgrade simulation  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

# get logger for this file
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigT1CaloFexSim.L1SimulationControlFlags' )
log.setLevel(logging.DEBUG)



_caloflags = list()
_ctpflags = list()
_glflags = list()


class SCellType(JobProperty):
    """ String which contains the chosen approach to supercell creation: Pulse, Emulated, BCID """
    statusOn = True
    allowedType = ['str']
    StoredValue = "Pulse"
    def _do_action(self):
        if self.get_Value()=="Emulated":
            from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags
            simflags.Calo.ApplySCQual=False 
_caloflags.append(SCellType)


class QualBitMask(JobProperty):
    """ int bitmask to be used for quality requirements """
    statusOn = True
    allowedType = ['int']
    StoredValue = 0x200
_caloflags.append(QualBitMask)


class ComputeEFexClusters(JobProperty):
    """ ComputeClusters """
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_caloflags.append(ComputeEFexClusters)


class ApplySCQual(JobProperty):
    """ ComputeClusters """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_caloflags.append(ApplySCQual)



class RunFexAlgorithms(JobProperty):
    """ ComputeClusters """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_caloflags.append(RunFexAlgorithms)



class RunCTPEmulation(JobProperty):
    """ Run the CTP Emulation """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_ctpflags.append(RunCTPEmulation)


# global

class OutputHistFile(JobProperty):
    """ Location of output root file """
    statusOn = True
    allowedType = ['str']
    StoredValue = "L1Sim#l1simulation.root"
_glflags.append(OutputHistFile)

class EnableDebugOutput(JobProperty):
    """ To enable DEBUG or VERBOSE output for specific algorithms """
    statusOn = True
    allowedType = ['bool']
    StoredValue = True
_glflags.append(EnableDebugOutput)


# end of flag definitions


class Calo(JobPropertyContainer):
    """ L1Calo Phase I simulation flags """
    pass

class CTP(JobPropertyContainer):
    """ CTP Phase I simulation flags """
    pass

class L1Phase1Sim(JobPropertyContainer):
    """ L1 Phase I simulation flags for L1Calo, L1Muon, CTP"""
    pass

from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.add_Container( L1Phase1Sim )
tf.L1Phase1Sim.add_Container( Calo )
tf.L1Phase1Sim.add_Container( CTP )

CTPPhase1SimFlags  = tf.L1Phase1Sim.CTP
L1CaloPhase1SimFlags = tf.L1Phase1Sim.Calo
L1Phase1SimFlags = tf.L1Phase1Sim

for flag in _glflags:
    L1Phase1SimFlags.add_JobProperty(flag)

for flag in _caloflags:
    L1Phase1SimFlags.Calo.add_JobProperty(flag)

for flag in _ctpflags:
    L1Phase1SimFlags.CTP.add_JobProperty(flag)


del _caloflags
del _ctpflags
del log
