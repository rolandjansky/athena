# $Id: TrigCostD3PD_prodJobOFragment.py tamartin
#
# Top jobO fragment setting up the common Trig Cost (NTUP_TRIG_COST).

# This jobO should not be included more than once:
include.block( "TrigCost3PDMaker/TrigRateD3PDMaker_prodJobOFragment.py" )

from AthenaCommon.Logging import logging
TrigRateD3PDStream_msg = logging.getLogger( 'TrigRateD3PDMaker_prodJobOFragment' )

costConfigL2 = False
costConfigEF = False
costConfigHLT = False
if ('costD3PD_doL2' in dir() or 'costD3PD_doEF' in dir() or 'costD3PD_doHLT' in dir()):
    TrigRateD3PDStream_msg.info( "Configuring via cosD3PD flags" );
    if ('costD3PD_doL2' in dir() and bool(costD3PD_doL2) == True):   costConfigL2 = True
    if ('costD3PD_doEF' in dir() and bool(costD3PD_doEF) == True):   costConfigEF = True
    if ('costD3PD_doHLT' in dir() and bool(costD3PD_doHLT) == True): costConfigHLT = True
else:
    TrigRateD3PDStream_msg.info( "Configuring via TriggerFlags" );
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.doLVL2(): costConfigL2 = True
    if TriggerFlags.doEF():   costConfigEF = True
    if TriggerFlags.doHLT():  costConfigHLT = True

from TrigCostD3PDMaker.addTrigCostData import addTrigCostData
addTrigCostData("RATE", "trig_rate.root", costConfigL2, costConfigEF, costConfigHLT);