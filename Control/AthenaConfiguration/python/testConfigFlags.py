from AthenaConfiguration.ConfigFlags import ConfigFlagContainer


if __name__=="__main__":
    cont=ConfigFlagContainer()


    #get a 'simple' flag:
    mc1=cont.get("AthenaConfiguration.GlobalConfigFlags.isMC")

    print "Default value of isMC=",mc1

    #set it to someting different:
    rv=cont.set("AthenaConfiguration.GlobalConfigFlags.isMC",not mc1)
    print "Attempt to set isMC:",rv


    mc2=cont.get("AthenaConfiguration.GlobalConfigFlags.isMC")
    print "New value of isMC=",mc2


    #Try to set anohter time:
    rv=cont.set("AthenaConfiguration.GlobalConfigFlags.isMC",mc1)
    print "Attempt to set isMC:",rv
    print "New value of isMC=",mc2



    #get a complicated flag:
    el1=cont.get("AthenaConfiguration.GlobalConfigFlags.estimatedLuminosity")
    
    print "Default value of estimatedLuminosity:",el1


    rv=cont.set("AthenaConfiguration.GlobalConfigFlags.estimatedLuminosity",42)
    print "Attempt to set estimatedLuminosity:",rv


    el2=cont.get("AthenaConfiguration.GlobalConfigFlags.estimatedLuminosity")
    print "New value of estimatedLuminosity:",el2

    rv=cont.set("AthenaConfiguration.GlobalConfigFlags.estimatedLuminosity",142)
    print "Attempt to set estimatedLuminosity a:",rv
    el2=cont.get("AthenaConfiguration.GlobalConfigFlags.estimatedLuminosity")
    print "New value of estimatedLuminosity:",el2


    #Get a flag via shorthand description

    be=cont.get("beamEnergy")
    print "beam energy is",be


    cont.dump()
