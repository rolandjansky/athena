# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"Use MethodTimer as a base class to profile an objects methods."


import types, time

def timing(f, name, self, tdict, ndict, cname):
    def timed_f(*a, **k):
        t0 = time.time()
        result = f(self, *a, **k)
        duration = time.time()-t0
        key = '%s.%s' % (cname, name)
        tdict[key]=tdict.get(key, 0)+duration
        ndict[key]=ndict.get(key, 0)+1
        # print 'inside wrapper for ',key,tdict[key],ndict[key]
        return result
    return timed_f

def getAllBases(c, gen=0):
    ngen = gen
    # print c.__name__, gen
    all       = [(gen,c)]
    thisGener = [c]
    parents   = [c.__bases__ for c in thisGener] # list of tuples

    # print all
    
    if parents:
        nextGener = []
        ngen += 1
        for tup in parents:
            [all.append((ngen,e)) for e in tup]
            [nextGener.append(e) for e in tup]

        for c in nextGener:
            all.extend(getAllBases(c, ngen))

    return all
                      
        
    
    

class MethodTimer:

    def __init__(self):
        """
        calls 'timing' to obtain a function that wraps
        the methods of this object between timing calls.
        It also passes a dictionary tDict that stores the
        cumulated time in each method.
        This dictionery is an instance variable, not a class
        variable
        """
        self.tDict = {}
        self.nDict = {}
        # print self.__class__.__dict__
        classes = getAllBases(self.__class__)

        # print classes
        
        ctor = [(g,c) for (g,c) in classes if c.__name__ == 'MethodTimer']
        [classes.remove(c) for c in ctor]

        temp = {}
        for (a, b) in classes: temp[a]=b
        classes = temp.items()

        classes.sort()
        classes.reverse()
        # print classes
        
        for (g,c) in classes:
            for f in c.__dict__.keys():
                m= c.__dict__[f]
                if isinstance(m, types.FunctionType):
                    # print 'Wrapping method %s.%s' % (c.__name__, f)
                    self.__dict__[f]=timing(m,
                                            str(f),
                                            self,
                                            self.tDict,
                                            self.nDict,
                                            c.__name__
                                            )

    def printTimes(self): print self.tDict, self.nDict
    def getMethodTime(self, methodName): return self.tDict.get(methodName, -1.)
    def getTimedMethods(self): return self.tDict.keys()
    def methodTimerReset(self):
        for key in self.tDict.keys():
            self.tDict[key] = 0

    def formatMethodTimes(self):

        # sort by time
        temp = [(item[1], item) for item in self.tDict.items()]
        temp.sort()

        # report also the number of calls
        text = ''
        for i in temp:
            item = i[1]
            method = item[0]
            time   = item[1]
            ncalls = self.nDict[method]
            method = method.ljust(50)
            text += '%50s %15g %3d\n' % (method, time, ncalls)

        return text

    def printMethodTimes(self):
        text=  self.formatMethodTimes()
        print text
        return text 

        
if __name__ == '__main__':
    class TimedZ(MethodTimer):
        def __init__(self, scale):
            self.scale = scale
            MethodTimer.__init__(self)
            
        def fz(self):
            time.sleep(0.5*self.scale)
        def fa(self):
            print 'Z.a'
        
    class TimedY(TimedZ):
        def __init__(self, scale):
            TimedZ.__init__(self, scale)
            
        def fy(self):time.sleep(0.5*self.scale)

    class TimedA(TimedY):
        def __init__(self, scale):
            TimedZ.__init__(self, scale)
        def fa(self):
            z = 1/0
            time.sleep(1*self.scale)
            print 'A.a'
        def fb(self):time.sleep(2*self.scale)
        def fc(self):time.sleep(3*self.scale)

        def printSelf(self):
            print self.printMethodTimes()
   # print 'Execising Z'
   # z = TimedZ(0.1)
   # z.fz()
   # z.printTimes()
    
    print 'Execising A'
    a = TimedA(0.1)
    a.fa()
    a.fb()
    a.fz()
    a.fy()
    a.fy()
    a.printTimes()

    a.printSelf()
#    b= TimedA(0.3)
#    b.fa()


#    print 'Times for a (scale factor 0.1) for fa, fb'
#    a.printTimes()
#    print 'Times for b (scale factor 0.3) for fa'
#    b.printTimes()#

#    print 'second call to fa, 1st to fc'
#    a.fa()
#    a.fc()
#    a.printTimes()#

#    print 'zero, then fa,fc'
#    a.zero()
#    a.fa()
#    a.fc()
#    a.printTimes()
