

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def HelloWorldCfg(ConfigFlags=None):
    result=ComponentAccumulator()
    
    from AthExHelloWorld.AthExHelloWorldConf import HelloAlg
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

    from AthExHelloWorld.AthExHelloWorldConf import HelloTool
    HelloWorld.MyPrivateHelloTool = HelloTool( "HelloTool" )
    HelloWorld.MyPrivateHelloTool.MyMessage = "A Private Message!"


    result.addEventAlgo(HelloWorld)
    return result
