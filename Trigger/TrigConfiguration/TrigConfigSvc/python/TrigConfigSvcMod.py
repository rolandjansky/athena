

def TrigConfigSvcMod( flags ):
    from AthenaConfiguration.CfgLogMsg import cfgLogMsg
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
    from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH

    l1ConfigSvc = TrigConf__LVL1ConfigSvc( "LVL1ConfigSvc" )
    l1XMLFile = findFileInXMLPATH(flags.get("TrigConfigSvc.Flags.inputLVL1configFile"))
    cfgLogMsg.debug( "LVL1ConfigSvc input file:"+l1XMLFile  )

    l1ConfigSvc.XMLMenuFile = l1XMLFile
    l1ConfigSvc.ConfigSource = "XML"

    acc.addService( l1ConfigSvc )
    return acc

if __name__ == "__main__":
    from AthenaConfiguration.ConfigFlags import ConfigFlagContainer
    flags = ConfigFlagContainer()
    acc = TrigConfigSvcMod( flags )
    
    acc = TrigConfigSvcMod( flags )
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
    
