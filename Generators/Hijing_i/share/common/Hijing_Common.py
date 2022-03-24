from Hijing_i.Hijing_iConf import Hijing
genSeq += Hijing()

## evgenConfig setup -- use reduced number of events for HI
evgenConfig.generators += ["Hijing"]

## Set default HIJING random number seeds
# TODO: Isn't this reset anyway in EvgenJobTransforms/share/Generate_randomseeds.py?
if not hasattr(svcMgr, 'AtRndmGenSvc'):
    from RngComps.RngCompsConf import AtRndmGenSvc
    svcMgr += AtRndmGenSvc()
svcMgr.AtRndmGenSvc.Seeds = \
    ["HIJING 327213897 979111713", "HIJING_INIT 31452781 78713307"]

## Extra stream persistency
evgenConfig.doNotSaveItems += ["McEventCollection#*"]
evgenConfig.extraSaveItems += ["McEventCollection#GEN_EVENT"] 
evgenConfig.extraSaveItems += ["HijingEventParams#*"] 
