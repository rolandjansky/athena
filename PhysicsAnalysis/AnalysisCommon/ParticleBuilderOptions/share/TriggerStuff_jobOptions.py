# Pool Converter
include("TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py")

# LVL1 config
include("TrigT1Config/TrigT1ConfigJobOptions.py")
#L1Config = Service( "L1Config" )
#L1Config.triggerMenuFileLocation   = "LVL1triggermenuDC2.xml"
#L1Config.thresholdListFileLocation = "LVL1triggerthresholdsDC2.xml"

# for RecMuonRoI svc
include ( "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.py" )
include ( "TrigT1RPCRecRoiSvc/TrigT1RPCRecRoiSvcJobOptions.py" )
