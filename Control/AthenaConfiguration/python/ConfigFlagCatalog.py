from AthenaConfiguration.ConfigFlags import ConfigFlag, cfgLogMsg
import sys,os


def isGoodFile(pathname):
    result=False
    if pathname.endswith("Flags.py") and os.path.isfile(pathname):
        fp=open(pathname)
        for l in fp:
            if "AthenaConfiguration" in l: 
                result=True
                break
            pass
        fp.close()
    return result
        

def buildConfigFlagCatalog():

    result=dict()

    seenFiles=set()

    for p in sys.path:
        if not os.path.isdir(p): continue
        for p1 in os.listdir(p):
            if os.path.isdir(p+"/"+p1):
                for f in  os.listdir(p+"/"+p1):
                    if isGoodFile("/".join((p,p1,f))):
                        cfgFlagFileName=f[:-3] # drop trailing .py
                        cfgFlagMod=p1
                        if (cfgFlagMod,cfgFlagFileName) in seenFiles: continue
                        seenFiles.add((cfgFlagMod,cfgFlagFileName))
                        #print "Working on module %s.%s" % (cfgFlagMod,cfgFlagFileName)
                        try:
                            exec "import %s.%s as cfgMod" %(cfgFlagMod,cfgFlagFileName)
                        except ImportError:
                            cfgLogMsg.warning("Failed to import file %s" % "/".join((p,p1,f)))
                            continue
                        for cfgObjName in dir(cfgMod):
                            if not cfgObjName.startswith("_") and not cfgObjName is "ConfigFlag":
                                cfgObj=getattr(cfgMod,cfgObjName)
                                #print cfgObj
                                try:
                                    if issubclass(cfgObj,ConfigFlag):
                                        #found a ConfigFlag:
                                        cfgLogMsg.debug("Found ConfigFlag %s in module %s" % (cfgObjName,cfgMod.__name__))
                                        if result.has_key(cfgObjName):
                                            raise KeyError("Error, duplicate job config flag",cfgObjName)
                                        else:
                                            result[cfgObjName]=cfgObj
                                except TypeError: 
                                    del cfgObj
                                    pass
                        del cfgMod
    cfgLogMsg.info("Build catalog of configuration flags with %i entries" % len(result))
    return result


configFlagCatalog=buildConfigFlagCatalog()


if __name__=="__main__":
    print buildConfigFlagCatalog()
