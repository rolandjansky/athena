from __future__ import print_function

def argsToFlags(configFlags=None, listOfArgs=None):

    if configFlags is None:
        from AthenaConfiguration.AllConfigFlags import ConfigFlags        
        configFlags=ConfigFlags

    if listOfArgs is None:
        import sys
        listOfArgs=sys.argv[1:]


    for arg in listOfArgs:
        #Safety check on arg: Contains exactly one '=' and left side is a valid flag
        argsplit=arg.split("=")
        if len(argsplit)!=2:
            raise ValueError("Can't interpret argument %s, expected a key=value format" % arg)

        key=argsplit[0].strip()
        if not configFlags.hasFlag(key):
            raise KeyError("%s is not a known configuration flag" % key)
            
        #Arg looks good enough, just exec it:
        argToExec="configFlags."+arg

        exec(argToExec)

    return configFlags

        
if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags        
    ConfigFlags.Input.Files=["yourfile",]

    #Overwrite flags from the command-line
    argsToFlags() 
    ConfigFlags.dump()
    
