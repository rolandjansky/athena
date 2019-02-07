# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Express stream addition  """

__author__  = 'B.Demirkoz'
__version__="$Revision: 1.19 $"
__doc__="Addition of express stream tags and prescales to the list of chains"

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenu.Express.py')

def addExpressStream(triggerPythonConfig):
    Names = {}
    menu_name = TriggerFlags.triggerMenuSetup()
    if menu_name.startswith('L1_pp_v4') or menu_name.startswith('Physics_pp_v4') or menu_name.startswith('MC_pp_v4'):
        Names = ExpressNamesPPV4
    elif menu_name.startswith('L1_pp_v5') or menu_name.startswith('Physics_pp_v5') or menu_name.startswith('MC_pp_v5'):
        Names = ExpressNamesPPV5
    elif menu_name.startswith('L1_pp_v6') or menu_name.startswith('Physics_pp_v6') or menu_name.startswith('MC_pp_v6'):
        Names = ExpressNamesPPV6
    elif menu_name.startswith('L1_pp_v7') or menu_name.startswith('Physics_pp_v7') or menu_name.startswith('MC_pp_v7'):
        Names = ExpressNamesPPV7
    elif menu_name.startswith('L1_pp_v8') or menu_name.startswith('Physics_pp_v8') or menu_name.startswith('MC_pp_v8'):
        Names = ExpressNamesPPV8


    for chainName, chainPrescale in Names.iteritems():
        if chainName not in triggerPythonConfig.allChains.keys():
            log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
            continue

        if  len(triggerPythonConfig.allChains[chainName]) ==2:
            efchain = triggerPythonConfig.allChains[chainName][1]
            efchain.addStreamTag('express', type='express', obeyLB='yes', prescale=chainPrescale)
        elif len(triggerPythonConfig.allChains[chainName]) == 1: #this is needed for hi jets
            efchain = triggerPythonConfig.allChains[chainName][0]
            efchain.addStreamTag('express', type='express', obeyLB='yes', prescale=chainPrescale)



#   Item name            | Prescale
#----------------------------------------------------------

ExpressNamesPPV4= {
    'mu24_tight'               :   1,
    }

ExpressNamesPPV5 = {
}
ExpressNamesPPV6 = {
}
ExpressNamesPPV7 = {
}
ExpressNamesPPV8 = {
}

