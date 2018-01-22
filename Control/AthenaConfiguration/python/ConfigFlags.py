from copy import deepcopy
from AthenaConfiguration.CfgLogMsg import cfgLogMsg

class ConfigFlag(object):
    def __init__(self,initialValue=None):
        self._value=initialValue
        self._alreadySet=False
        pass

    @staticmethod
    def getDefault(prevContainer):
        pass

    def set_Value(self,newvalue):
        if not self._alreadySet:
            self._value=newvalue
            self._alreadySet=True
        else:
            print "Config Flag '%s' already set" % self.__class__.__name__

    def get_Value(self):
        return deepcopy(self._value)

    def __call__(self):
        return self.get_Value()  
    
    def wasSet(self):
        return self._alreadySet


class pseudomodule(object):
    def __init__(self,flagsContainer,upstream):
        self._fc=flagsContainer
        self._location=upstream
    
    def __getattr__(self,nextLevel):
        newloc=self._location+"."+nextLevel
        #print "NewLoc=",newloc
        if len(newloc.split("."))==3:
            return self._fc.get(newloc)
        else:
            return pseudomodule(self._fc,newloc)



class ConfigFlagContainer(object):
    def __init__(self,otherFlags=None):
        if otherFlags is None:
            self._flagdict=dict()
        else:
            self._flagdict=deepcopy(otherFlags._flagdict)

    def importFlag(self,fullname):
        if not len(fullname.split(".")):
            raise KeyError("Flag name format is <package>.<module>.<flag>")
        (p,m,f)=fullname.split(".")
        cfgLogMsg.debug("Importing flag %s from module %s.%s" % (f,p,m))
        exec "import %s.%s as newmodule" % (p,m)
        if not hasattr(newmodule,f):
            raise ImportError("Module %s.%s has no class called %s" % (p,m,f))

        flagclass=getattr(newmodule,f)
        if not issubclass(flagclass,ConfigFlag):
            raise TypeError("Class %s is not a subclass of ConfigFlag" % f)
        return flagclass


    def get(self,name):
        if not self._flagdict.has_key(name):
            newFlag=self.importFlag(name)
            self._flagdict[name]=newFlag()
            self._flagdict[name]._value=newFlag.getDefault(self)
            pass
        f=self._flagdict[name]
        return f.get_Value()

    def set(self,name,value):
        if not self._flagdict.has_key(name):
            newFlag=self.importFlag(name)
            self._flagdict[name]=newFlag()
        return self._flagdict[name].set_Value(value)

    def dump(self):
        print "Content of ConfigFlagContainer:"
        for n,f in self._flagdict.iteritems():
            state = "(  set  )" if f.wasSet() else "(default)"
            print "  ",n,state,":",f.get_Value()



#Decorator to emulate pass-by-value behavior of flags
def cloneflags( configFunc ):
    def copied_flags( *args ):
        cargs =  [ deepcopy( a ) if issubclass(a,ConfigFlagContainer) else a for a in args ]

        return configFunc( *cargs )
    return copied_flags
