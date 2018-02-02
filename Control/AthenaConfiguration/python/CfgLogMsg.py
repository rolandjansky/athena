import os, inspect

class CfgLogMsg():

    def __init__(self,level="info"):
        self._lvl=self._lvlFromString(level)
        pass

    
    def _lvlFromString(self,level):
        #No ERROR message: Errors reported via exceptions!
        if (level.lower()=="none"):
            return 0
        elif (level.lower()=="warning"):
            return 1
        elif (level.lower()=="info"):
            return 2
        elif (level.lower()=="debug"):
            return 3
        else:
            raise TypeError('Allowed message levels are only "none", "info" or "debug", got %s' % level)
        pass

    def setLevel(self,level):
        self._lvl=self._lvlFromString(level)
        pass


    def warning(self,message):
        if self._lvl>=1:
            (callfile,callfunc)=self.getCaller()
            print "%s:%s  WARN %s" % (callfile,callfunc,message)

    def info(self,message):
        if self._lvl>=2:
            (callfile,callfunc)=self.getCaller()
            print "%s:%s  INFO %s" % (callfile,callfunc,message)

    def debug(self,message):
        if self._lvl>=3:
            (callfile,callfunc)=self.getCaller()
            print "%s:%s  DEBUG %s" % (callfile,callfunc,message)

    def getCaller(self):
        try:
            #print inspect.stack()
            callfile=inspect.stack()[2][1].split("/")[-1]
            callfunc=inspect.stack()[2][3]
        except Exception, e:
            print e 
            (callfile,callfunc)=("","")
            pass
        
        return (callfile,callfunc)
        


_level=None
if 'ATH_CFG_MSG_LVL' in os.environ:
    _level=os.environ["ATH_CFG_MSG_LVL"]
else:
    _level="info"

cfgLogMsg=CfgLogMsg(_level)

del _level
