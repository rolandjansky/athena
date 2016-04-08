# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time

class RunTimer:
    def __init__(self,maxTime, logger):
        "maxTime in seconds"
        
        self.maxProcessTime = maxTime
        self.startTime      = time.time()
        self.logger         = logger
        self.logger.debug("RunTimer constructor: maxtime (secs): %d" % self.maxProcessTime)

    #------------------------------------------------------
    def __str__(self):
        stime = time.strftime("%d %b %Y %H:%M:%S ",
                              time.localtime(self.startTime))
        s = 'start time: %s maxtime: %s' % (stime,  str(self.maxProcessTime))
        return s

    #------------------------------------------------------
    
    def remaining(self):
        remain = self.maxProcessTime - (time.time()-self.startTime)
        if remain<0: remain =0
        self.logger.debug('RunTimeTimer, remaining time %d' % remain)
        return remain

    #------------------------------------------------------
    
    def timeOut(self):
        now = time.time()
        to = False
        if (now-self.startTime)>self.maxProcessTime: to =  True
        self.logger.debug('RunTimeTimer, timed-out =  %s' % str(to))
        return to

    #------------------------------------------------------
    
    def wasNotStartedToday(self):
        """If the stamp has not come yet and it is past midnight, stop the RTT."""
        startDate = self.extractDate(self.startTime)
        nowDate = self.extractDate(time.time())
        answer = False
        if startDate != nowDate:
            answer = True
        return answer

    #------------------------------------------------------
    
    def extractDate(self,theTime):
        theList = time.gmtime(theTime)
        date = [theList[0], theList[1], theList[2]]
        return date

    #------------------------------------------------------
    
    def reset(self,maxTimeFromNow=60*60*20):
        'Resets the timer, as if it had been created now.'
        self.startTime = time.time()
        self.maxProcessTime = maxTimeFromNow
        self.logger.info('Reset the RunTimer to: ' + str(maxTimeFromNow) + ' seconds.')
        
    #------------------------------------------------------
    
    def maxTimeInHours(self):
        return int(float(self.maxProcessTime)/float(3600))

    #------------------------------------------------------
    
    def start(self):
        stringRT = str(self)
        toks = stringRT.split()
        theStart = toks[2] + ' ' + toks[3] + ' ' + toks[4] + ' ' + toks[5]
        return theStart

#=========================================================
#=========================================================
#=========================================================

class NX0NightlyRunTimer(RunTimer):
    def __init__(self, logger):
        RunTimer.__init__(self, 165*60*60, logger)

#=========================================================
#=========================================================
#=========================================================

class N0XNightlyRunTimer(RunTimer):
    def __init__(self, logger):
        RunTimer.__init__(self, 115*60*60, logger)

#=========================================================
#=========================================================
#=========================================================

class NumberedRelRunTimer(RunTimer):
    def __init__(self, logger):
        RunTimer.__init__(self, 750*60*60, logger)



#=========================================================
#=========================================================
#=========================================================

if __name__ == '__main__':
    pass

