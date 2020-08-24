# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def HelloWorldCfg():
    result=ComponentAccumulator()
    
    HelloAlg=CompFactory.HelloAlg
    HelloWorld=HelloAlg("HelloWorld")
    
    HelloWorld.OutputLevel = 0

    # Set an int property
    HelloWorld.MyInt = 42

    # Set a boolean property (False, True, 0, 1)
    HelloWorld.MyBool = True

    # Set a double property
    HelloWorld.MyDouble = 3.14159

    # Set a vector of strings property ...
    HelloWorld.MyStringVec = [ "Welcome", "to", "Athena", "Framework", "Tutorial" ]

    # ... and add one more:
    HelloWorld.MyStringVec += [ "!" ]

    # Set a map of strings to strings property ...
    HelloWorld.MyDict = { 'Bonjour'      : 'Guten Tag',
                          'Good Morning' : 'Bonjour' , 'one' : 'uno' }

    # ... and add one more:
    HelloWorld.MyDict[ "Goeiedag" ] = "Ni Hao"

    # Set a table (a vector of pairs of doubles) ...
    HelloWorld.MyTable = [ ( 1 , 1 ) , ( 2 , 4 ) , ( 3 , 9 ) ]

    # ... and one more:
    HelloWorld.MyTable += [ ( 4, 16 ) ]

    # Set a matrix (a vector of vectors) ...
    HelloWorld.MyMatrix = [ [ 1, 2, 3 ],
                            [ 4, 5, 6 ] ]

    # ... and some more:
    HelloWorld.MyMatrix += [ [ 7, 8, 9 ] ]

    HelloTool=CompFactory.HelloTool
    ht=HelloTool( "HelloTool" )
    HelloWorld.MyPrivateHelloTool = ht #HelloTool( "HelloTool" )
    HelloWorld.MyPrivateHelloTool.MyMessage = "A Private Message!"

    pt=HelloTool( "PublicHello")
    pt.MyMessage="A public Message!"    

    result.addPublicTool(pt)
    print (pt)


    HelloWorld.MyPublicHelloTool=pt


    #print HelloWorld

    result.addEventAlgo(HelloWorld)
    return result


if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Exec.MaxEvents=10
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(HelloWorldCfg())
    cfg.run()

    #f=open("HelloWorld.pkl","wb")
    #cfg.store(f)
    #f.close()

