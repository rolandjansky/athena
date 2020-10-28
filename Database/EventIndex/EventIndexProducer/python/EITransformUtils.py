# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @brief Module with EventIndex transform options and substep

import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfExe import athenaExecutor

### Add Argument Methods
def addEI_arguments(parser):
    from EventIndexProducer.eiTrfArgs import addEI_tfArgs
    addEI_tfArgs(parser)

def addEI_MRG_arguments(parser):
    from EventIndexProducer.eiTrfArgs import addEI_MRG_tfArgs
    addEI_MRG_tfArgs(parser)

### Add Sub-step Methods
## @brief Add EI transform substep
def addEI_Substep(executorSet):
    executorSet.add(athenaExecutor(name = 'EventIndexTf', skeletonFile = 'EventIndexProducer/skeleton.POOLtoEI_tf.py',
                                   substep = 'EI', tryDropAndReload = False,
                                   inData=['POOL', 'AOD', 'ESD', 'EVNT', 'HITS', 'RDO'],
                                   outData=['NULL','EI'] ))

def addEI_MRG_Substep(executorSet):
    executorSet.add(athenaExecutor(name = 'EventIndexTf', skeletonFile = 'EventIndexProducer/skeleton.POOLtoEI_tf.py',
                                   substep = 'EI', tryDropAndReload = False,
                                   inData=['AOD_MRG'],
                                   outData=['NULL','EI']))

### Append Sub-step Methods
def appendEI_Substep(trf):
    executor = set()
    addEI_Substep(executor)
    trf.appendToExecutorSet(executor)

