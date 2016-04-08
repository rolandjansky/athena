# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

import time

#NB1: It is considered an error if two TimeLabels are constructed with
#     the same TimePos but different Labels.
#
#NB2: Only if *all* TimeLabels have a timestamp will absolute time-mode
#     and interpolation be available.

class ColourUtils:
    #__fallbackcols = ['b','g','r','c','m','y']
    #quick hack
    __fallbackcols = [(0/255.01,204/255.01,255/255.01),(255/255.01,166/255.01,0/255.01),(24/255.01,86/255.01,255/255.01),(255/255.01,64/255.01,47/255.01),(97/255.01,118/255.01,255/255.01),(255/255.01,121/255.01,63/255.01)]
    @staticmethod
    def getFallBackColour(index):
        return ColourUtils.__fallbackcols[index%len(ColourUtils.__fallbackcols)]

class TimeLabel( object ):

    # Aaaaah... can't have overloaded constructors in python.
    #
    # So be nice and use in either of two ways:
    #
    # 1) TimeLabel(date=<dataobject>)
    #    (for e.g. rtt measurements that are only differentiated by their date)
    #
    # or
    #
    # 2) TimeLabel(label=...,sortvalue=...)
    #    (for e.g. measurements for a given release. Sortvalue should be a number
    #     which is used to find its relative position compared to other TimeLabels)

    def __init__(self,date=None,label=None,sortvalue=None):
        if date!=None:
            assert label==None and sortvalue==None
            #from time import gmtime, strftime
            self.date=date
            self.label=date.strftime('%Y-%m-%d')
            self.sortval=None
        else:
            assert label!=None and sortvalue!=None
            self.date=None
            self.label=str(label)
            self.sortval=sortvalue
        self.__checkconsistency__()
        self.hash=float(self.sortval if self.date==None else self.unixTime())

    def __str__(self):
        return str(self.label)

    def isTimeStamp(self):
        return self.date!=None

    def unixTime(self):
        return time.mktime(self.date.timetuple())

    def getLabel(self):
        return self.label

    def getDate(self):
        return self.date

    def __checkconsistency__(self):
        ok=True
        if self.label==None or self.label=="":
            ok=False
        if (0 if self.date==None else 1)+(0 if self.sortval==None else 1)!=1:
            ok=False
        if not ok:
            raise Exception("Error detected in definition of TimeLabels")

    #Hash and comparison implementations. "None" is considered lower than any hash
    def __hash__( self ):
        return self.hash
    def __eq__( self, other ):
        return self.hash==other.hash if other != None else False
    def __lt__( self, other ):
        return self.hash<other.hash if other != None else False
    def __ne__( self, other ):
        return self.hash!=other.hash if other != None else True
    def __le__( self, other ):
        return self.hash<=other.hash if other != None else False
    def __gt__( self, other ):
        return self.hash>other.hash if other != None else True
    def __ge__(self, other):
        return self.hash>=other.hash if other != None else True

class Measurement( object ):
    def __str__(self):
        return str(self.value) if not self.invalid() else '<invalid>'

    def __init__(self,value,unit=None,value_error=None):
        self.value=value
        self.unit=unit
        self.value_error=value_error

    def getUnit(self):
        return self.unit

    def getValue(self):
        return self.value

    def getError(self):
        return self.value_error

    def invalid(self):
        return self.value==None

    def valid(self):
        return not self.invalid()

    def __mul__(self,factor):
        if self.invalid() or factor==None:
            return Measurement(None,self.unit,None)
        outerr=factor*self.value_error if self.value_error!=None else None
        return Measurement(factor*self.value,self.unit,outerr)

    def __add__(self, other):
        #Figure out unit on result:
        outunit=self.getUnit()
        if self.getUnit()==None or other.getUnit()==None:
            outunit=None
        elif self.getUnit() != other.getUnit():
            raise Exception('Incompatible units on measurements ')
        #If at least one is invalid (value==None), output will be as well:
        if self.invalid() or other.invalid():
            return Measurement(None,outunit,None)
        #Figure out error on result (if both inputs have errors):
        outerror=None
        e1=self.getError()
        e2=other.getError()
        if e1!=None and e2!=None:
            from math import sqrt
            outerror=sqrt(e1*e1+e2*e2)
        return Measurement(self.getValue()+other.getValue(),outunit,outerror)

    def __sub__(self, other):
        #Figure out unit on result:
        outunit=self.getUnit()
        if self.getUnit()==None or other.getUnit()==None:
            outunit=None
        elif self.getUnit() != other.getUnit():
            raise Exception('Incompatible units on measurements')
        #If at least one is invalid (value==None), output will be as well:
        if self.invalid() or other.invalid():
            return Measurement(None,outunit,None)
        #Figure out error on result (if both inputs have errors):
        outerror=None
        e1=self.getError()
        e2=other.getError()
        if e1!=None and e2!=None:
            from math import sqrt
            outerror=sqrt(e1*e1+e2*e2)
        return Measurement(self.getValue()-other.getValue(),outunit,outerror)

class PlottableResult:
    def __init__(self,colnames,title=None,colname2colour=None,unit=None):
        import time
        self.__rightnow=time.time()
        self.__divTotal=False
        self.title=title
        #FIXME: Add self.coltitle2prettytitle={} and use in getColumnTitle()
        self.setAutoOffSetParameters()#To set defaults
        self.valueoffsets={}#To allow some columns to be drawn with an offset
        self.autooffset_cache=None
        self.colnames=[]
        for cn in colnames:
            self.colnames+=[cn]
        self.colname2colour = colname2colour if colname2colour!=None else {}
        self.unit = unit
        self.data={}#timepoint->{colname1:value, colname2:value}
        self.data_extrapolated=None
        self.colname2linestyle={}
        self.colname2markerstyle={}
        self.totalEstimateHasBeenAdded=False
        self.maxAllowedNegativeFraction=None

    def rightNow(self): return self.__rightnow

    #Returns name of new column
    def addTotalEstimateColumn(self,colname=None,colour=None):
        assert not self.totalEstimateHasBeenAdded, "Attempt to add total estimate twice!"
        allowextrapolation=self.allTimeLabelsAreTimeStamps()
        if colname==None:
            if self.__divTotal: colname = 'total/10'
            else: colname = 'total'
        #Calculate the new points:
        times=self.getTimes()
        pointstoadd=[]
        for t in times:
            val=self.__getTotal__(t,allowExtrapolation=allowextrapolation)
            if val!=None:
                pointstoadd+=[(val,t)]
        #Update the meta-data:
        self.colnames += [colname]
        self.colname2colour[colname] = colour if colour!=None else (1,0,0)
        #Add the points
        for pointtoadd in pointstoadd:
            self.addPoint(colname,pointtoadd[0],pointtoadd[1])
        self.colname2linestyle[colname]='--'
        self.colname2markerstyle[colname]=''
        self.totalEstimateHasBeenAdded=True
        return colname

    def setColumnOffset(self,colname,offset):#Put offset='auto', 0, or some number.
        self.autooffset_cache = None#invalidate
        self.valueoffsets[colname]= offset

    def setMaxAllowedNegativeFraction(self,f):
        self.maxAllowedNegativeFraction=f
        self.autooffset_cache = None#invalidate

    def getMaxAllowedNegativeFraction(self):
        return self.maxAllowedNegativeFraction

    def setAutoOffSetParameters(self,minNumberOfZeroes=2,prefersmalloffset=True):
        self.autooffset_cache = None#invalidate
        self.autooffset_minnzeroes=minNumberOfZeroes
        self.autooffset_prefersmalloffset=prefersmalloffset

    def __calculateAutoTotalOffset(self,intervals,#[(min0,max0),(min1,max1),...],
                                   totals,#[total0,total1,]
                                   minNumberOfZeroes,#We want pretty offsets with as many zeroes as possible
                                   prefersmalloffset=True):#If true, take the smallest possible offset, otherwise take the most central [however, offset==0 is ALWAYS preferred)
        n=len(intervals)
        assert n==len(totals) and n>0
        ###########################################################
        def valuesInterval(l):
            assert len(l)>0
            entriesAreIntervals = (type(l[0])==type((0,0)))
            (vmin,vmax)=(None,None)
            if entriesAreIntervals:
                for (imin,imax) in l:
                    assert imax>=imin, "Bad interval definition!"
                    if vmin==None or vmin>imin:
                        vmin=imin
                    if vmax==None or vmax<imax:
                        vmax=imax
            else:
                for i in l:
                    if i==None:
                        continue
                    if vmin==None or vmin>i:
                        vmin=i
                    if vmax==None or vmax<i:
                        vmax=i
            return (vmin,vmax)
        ###########################################################

        ###########################################################
        def possibleOffsets(totint,valsint,minNumberOfZeroes,nzeroes=8,lower=0):
            if totint[0]==None or totint[1]==None:
                return [0]
            if totint[0]>valsint[0]:
                #Make sure that the code below only need to look for positives offset:
                l=possibleOffsets((-totint[1],-totint[0]),(-valsint[1],-valsint[0]),minNumberOfZeroes,nzeroes,lower)
                l2=[]
                for v in l:
                    l2 += [-v]
                l2.sort()
                return l2

            ##extend interval slightly upwards (but not downwards unless we stay positive):
            #if valsint[1]>0.0:
            #    epsilon = 0.05*(valsint[1]-max(0.0,valsint[0]))
            #    assert epsilon>0.0
            #    ext_high = valsint[1]+epsilon
            #    ext_low = max(0.0,valsint[0]-epsilon) if valsint[0]>0 else valsint[0]
            #    valsint = (ext_low,ext_high)

            #Look for a positive offset.
            l=[]
            step = int(10**int(nzeroes))
            lastbelow=None
            firstabove=None
            #testvals = range(10) if minNumberOfZeroes!=0 else [0,5]
            #for i in testvals:
            for i in range(10):
                offset = lower+step*i
                if totint[0]+offset<valsint[0]:
                    #Offset too low:
                    lastbelow=offset
                    continue
                if totint[1]+offset>valsint[1]:
                    #Offset too high
                    firstabove=offset
                    break
                l+=[offset]
            if len(l)==0:
                #None found at this nzeroes resolution, meaning that
                #we need to skip one zero:
                nzeroes=nzeroes-1
                if nzeroes<minNumberOfZeroes or firstabove==None:
                    return l#Too bad!
                return possibleOffsets(totint,valsint,minNumberOfZeroes,nzeroes,lastbelow)
            l.sort()
            return l
        ###########################################################


        #Try to find the best range, starting by going as far
        #back in history as possible. If we can't find a useful result,
        #ignore one (more) entry at the back and try again. For nlast==1
        #we should per definition always get a useful result:

        for nlast in range(len(totals),0,-1):
            totint=valuesInterval(totals[n-nlast:])
            valsint=valuesInterval(intervals[n-nlast:])

            if totint[0]>=valsint[0] and totint[1]<=valsint[1]:
                return 0

            ltot = totint[1]-totint[0] if totint[1]!=totint[0] else 0
            lvals = valsint[1]-valsint[0] if valsint[1]!=valsint[0] else 0

            if ltot>lvals:
                continue

            offsets = possibleOffsets(totint,valsint,minNumberOfZeroes)
            if len(offsets)!=0:
                if 0 in offsets:
                    return 0
                if prefersmalloffset:
                    #Take the numerically smallest value:
                    smallestoffset=None
                    for o in offsets:
                        if smallestoffset==None or abs(o)<abs(smallestoffset):
                            smallestoffset=o
                    assert smallestoffset!=None
                    return smallestoffset
                #Take central value:
                return offsets[len(offsets)/2]
        print "Warning: Could not find a suitable offset with the required number of zeroes."
        return 0

    def __getautooffset(self,colname):
        assert self.valueoffsets[colname]=='auto'
        if self.autooffset_cache==None:
            self.autooffset_cache={}

            #Build one list of (min,max) values for the non-autooffset
            #columns (if no points available at a given time, use
            #-inf,+inf), and a list for each of the autooffset-columns
            othercols_intervals=[]
            autocols_vals={}

            #  -- Find all column names (auto and others) and times:
            autocols=[]
            othercols=[]
            for icol in range(self.getNColumns()):
                cn=self.getColumnName(icol)
                if self.valueoffsets.keys().count(cn) and self.valueoffsets[cn]=='auto':
                    autocols+=[icol]
                else:
                    othercols+=[icol]
            times=self.getTimes()

            # -- Get all values (*without* offsets) of the auto-columns:
            allowExtrapolation=self.allTimeLabelsAreTimeStamps()
            for icol in autocols:
                l=[]
                for t in times:
                    l+=[self.getValue(icol,t,applyOffset=False,allowExtrapolation=allowExtrapolation)]
                autocols_vals[self.getColumnName(icol)]=l

            # -- Get all values (*with* offsets) of the other-columns at each time, and find the range they span:
            vmin_alltimes=None
            vmax_alltimes=None
            for t in times:
                vmin=None
                vmax=None
                for icol in othercols:
                    v=self.getValue(icol,t,applyOffset=True,allowExtrapolation=allowExtrapolation)
                    if v==None:
                        continue
                    if vmin==None or v<vmin:
                        vmin=v
                    if vmax==None or v>vmax:
                        vmax=v
                if vmin!=None and (vmin_alltimes==None or vmin<vmin_alltimes):
                    vmin_alltimes=vmin
                if vmax!=None and (vmax_alltimes==None or vmax>vmax_alltimes):
                    vmax_alltimes=vmax
                if vmax==None or vmin==None or vmin==vmax:
                    vmin=-1.0e99
                    vmax=1.0e99
                othercols_intervals+=[(vmin,vmax)]

            #In case there is a maximum negative fraction allowed, we should clip any of the othercols_intervals to that:
            if self.maxAllowedNegativeFraction!=None and vmin_alltimes!=None and vmax_alltimes!=None and vmax_alltimes>0.0:
                assert self.maxAllowedNegativeFraction>=0.0
                if vmin_alltimes < -self.maxAllowedNegativeFraction*vmax_alltimes:
                    vmin_alltimes=-self.maxAllowedNegativeFraction*vmax_alltimes
                    new_othercols_intervals=[]
                    for vmin,vmax in othercols_intervals:
                        if vmin!=None and vmin<vmin_alltimes:
                            vmin=vmin_alltimes
                        new_othercols_intervals+=[(vmin,vmax)]
                    othercols_intervals=new_othercols_intervals

            #Call helper method to figure out the offset value, and store in cache:
            for cn,vals in autocols_vals.items():
                offset = self.__calculateAutoTotalOffset(intervals=othercols_intervals,
                                                         totals=vals,
                                                         minNumberOfZeroes=self.autooffset_minnzeroes,
                                                         prefersmalloffset=self.autooffset_prefersmalloffset)
                self.autooffset_cache[cn] = offset

        return self.autooffset_cache[colname]

    def getColumnOffset(self,colname):#Returns 'auto', 0, or some number
        if self.valueoffsets.keys().count(colname)==0:
            return 0
        else:
            o = self.valueoffsets[colname]
            return o if o!='auto' else self.__getautooffset(colname)

    def addPoint(self,colname,value,time=None):
        assert not self.totalEstimateHasBeenAdded, "Attempt to add points after total estimate!"
        self.autooffset_cache = None#invalidate
        self.data_extrapolated=None#invalidate
        if self.data.keys().count(time)==0:
            self.data[time]={}
        self.data[time][colname]=value

    #Getters, used by the various output classes to produce tables, plots, etc.
    def getTitle(self):
        return self.title

    def getUnit(self):
        return self.unit

    def getTitleWithUnit(self):
        return self.title+('' if self.unit==None else ' ['+self.unit+']')

    def getNColumns(self):
        return len(self.colnames)

    def getColumnName(self,icol):
        return self.colnames[icol]

    def getColumnTitle(self,icol,applyOffset=False):
        t=self.colnames[icol]
        o=self.getColumnOffset(self.getColumnName(icol)) if applyOffset else 0
        if o!=0:
            s=str(o)
            u=self.getUnit()
            if (abs(o)>500):
                if u=='mb' or u=="MB":
                    s=str(o/1000.0)
                    u='gb' if u=='mb' else 'GB'
                elif u=='ms' or u=="MS":
                    s=str(o/1000.0)
                    u='s' if u=='ms' else 'S'
            if s.endswith('.0'):
                s=s[0:len(s)-2]
            if o<0:
                t+=' '+s.replace('-','- ')+' '+u
            else:
                t+=' + '+s+' '+u
        return t

    def getColumnColour(self,icol):
        colname=self.getColumnName(icol)
        if self.colname2colour.keys().count(colname)==1:
            return self.colname2colour[colname]
        return ColourUtils.getFallBackColour(icol)

    def getColumnMarkerStyle(self,icol):
        cn=self.getColumnName(icol)
        if self.colname2markerstyle.keys().count(cn)==1:
            return self.colname2markerstyle[cn]
        return 'o'

    def getColumnLineStyle(self,icol):
        cn=self.getColumnName(icol)
        if self.colname2linestyle.keys().count(cn)==1:
            return self.colname2linestyle[cn]
        return '-'

    def prettyDate(self,t):
        from time import gmtime, strftime
        return strftime('%Y-%m-%d',gmtime(t))

    def getTimes(self):
        times = self.data.keys()
        times.sort()
        return times

    def allTimeLabelsAreTimeStamps(self):
        for t in self.getTimes():
            if not t.isTimeStamp():
                return False
        return True

    def getValue(self,icol,time,applyOffset=False,allowExtrapolation=False):
        #NB: Illegal to call with allowExtrapolation==True unless allTimeLabelsAreTimeStamps()
        colname=self.getColumnName(icol)
        v=None
        if self.data[time].keys().count(colname)==1:
            v=self.data[time][colname]
            if v!=None:
                if applyOffset:
                    v+=self.getColumnOffset(colname)
                return v
        if allowExtrapolation:
            if self.data_extrapolated==None:
                self.__updateExtrapolatedValCache__()
            if self.data_extrapolated[time].keys().count(colname)==1:
                v=self.data_extrapolated[time][colname]
                if applyOffset and v:
                    v+=self.getColumnOffset(colname)
            return v
        return None

    def __getTotal__(self,time,allowExtrapolation=True):
        tot=0.0
        for icol in range(self.getNColumns()):
            v=self.getValue(icol,time,allowExtrapolation=allowExtrapolation,applyOffset=False)
            if v==None:
                return None
            tot+=v
        if self.__divTotal and tot!=None:
            return tot/10.0
        return tot

    def __updateExtrapolatedValCache__(self):
        assert self.allTimeLabelsAreTimeStamps()
        self.data_extrapolated={}
        for icol in range(self.getNColumns()):
            colname=self.getColumnName(icol)
            x=[]
            y=[]
            for t in self.getTimes():
                x+=[t.unixTime()]
                y+=[self.getValue(icol,t,allowExtrapolation=False,applyOffset=False)]
            #We now have all (time,val) pairs for a given
            #column. Supposedly some of the values are None - for
            #those we need to extend/interpolate the remaining values.
            #interpolate missing points in xall/yall:
            #   -> extend values at front and back:
            def extendValuesAtFront(alist):
                if alist[0]==None:
                    firstval=None
                    for val in alist:
                        if val!=None:
                            firstval=val
                            break
                    if firstval==None:
                        return#No data at all in column...
                    for i in range(len(alist)):
                        if alist[i]==None:
                            alist[i]=firstval
                        else:
                            break
            extendValuesAtFront(y)
            y.reverse()
            extendValuesAtFront(y)
            y.reverse()
            #Now only internal elements in yval can be None. Interpolate those:
            lasty = None
            x_atlasty = None
            def getIndexOfNextNone(l,i):
                for i2 in range(i,len(l)):
                    if l[i2]!=None:
                        return i2
            for i in range(len(x)):
                assert x[i]!=None, "invalid time"
                if y[i]!=None:
                    lasty=y[i]
                    x_atlasty=x[i]
                else:
                    i2=getIndexOfNextNone(y,i)
                    if i2==None:
                        continue#This happens if a column has no entries
                    y[i]=lasty + (y[i2]-lasty)*(x[i]-x_atlasty)/(x[i2]-x_atlasty)

            i=0
            for t in self.getTimes():
                if self.data_extrapolated.keys().count(t)==0:
                    self.data_extrapolated[t]={}
                if self.data_extrapolated[t].keys().count(colname)==0:
                    self.data_extrapolated[t][colname]={}
                self.data_extrapolated[t][colname]=y[i]
                i+=1

    def getNewestValue(self,icol,applyOffset=False):
        times = self.getTimes()
        times.reverse()
        for t in times:
            val = self.getValue(icol,t,applyOffset=applyOffset)
            if val!=None:
                return val
        return None

class Job( object ):
    def __init__( self, jobidstr, meas_name2val = None, time = None, meas_name2error = None ):
        self.jobidstr=jobidstr
        self.time = time
        self.meas={}
        if meas_name2val!=None:
            for k,v in meas_name2val.items():
                e=None
                if meas_name2error!=None:
                    e = meas_name2error[k] if meas_name2error.keys().count(k)!=0 else None
                self.addMeasurement(k,v,e)

    def __str__(self):
        return "<Job "+self.jobidstr+" for "+(self.time.getLabel() if self.time!=None else '?')+">"

    def getIDStr( self ):
        return self.jobidstr

    def getTime( self ):
        return self.time

    def getValidMeasurementNames(self):
        l=[]
        for measname, meas in self.meas.items():
            if meas!=None and meas.valid():
                l += [measname]
        return l

    def getAllMeasurementNames(self):
        l=self.meas.keys()
        l.sort()
        return l

    def addMeasurement(self,measname,measval,measerror=None):
        l = measname.split(':')
        self.meas[l[0]] = Measurement(value=measval,unit=(None if len(l)<2 else l[1]),value_error=measerror)

    def getMeasurement(self,measname):
        if self.meas.keys().count(measname)==0:
            return Measurement(None)
        return self.meas[measname]

class DomainCalcBase(object):
    #necessary methods:
    def getName(self):
        assert False, "reimplement"
    def defineJobs(self):
        assert False, "reimplement"
    #optional methods:
    def getPrettyName(self):
        return self.getName()
    def defineDomains(self):
        return []
    def calcDomains(self,job2val):
        return {}
    def jobColour(self,job):
        return ColourUtils.getFallBackColour(self.defineJobs().index(job))
    def domainColour(self,domain):
        return ColourUtils.getFallBackColour(self.defineDomains().index(domain))
    def domainOffsets(self):
        return None
    #This one is only used in case the jobs are found in RTT:
    def getCDJobDefinitions(self,dbkey):
        return None
    #Where domain warnings should be send by default:
    def getDefaultRTTEmailList(self,build='dev',cache='AtlasProduction',cmtconfig='i686-slc4-gcc34-opt'):
        return None
    #And this one is used when jobs are available as logfiles:
    def getLogFileNamingSchemeDefinitions(self):
        return None
    #Don't reimplement the rest:
    def testvalid(self):
        try:
            #Test we got a reimplementation
            self.getName()
            self.defineJobs()
            doms=self.defineDomains()
            for dom in doms:
                self.domainColour(dom)#must be ok to call of any domain
            fake_job2val=dict((j,Measurement(117.0)) for j in self.defineJobs())
            dom2res=self.calcDomains(fake_job2val)
            if dom2res:
                for dom in dom2res.keys():
                    assert dom in doms,dom
                for dom in doms:
                    assert dom in dom2res,dom
            do=self.domainOffsets()
            if do:
                for dom in do.keys():
                    assert dom in doms,dom
        except Exception, err:
            print "WARNING: Problems validating domain calculator:",err
            raise
    #    for j in self.defineJobs():
    #
    #self.calcDomains(job2val)
    #def domainOffsets(self):
    #    return {'ShowerLibs':'auto','Core':'auto'}

    #convenience method - use in calcDomains(..)
    def sumCheck(self,val1,val2):
        sumcheck=val1-val2
        if not sumcheck.invalid() and abs(sumcheck.getValue())>1.0e-8:
            print 'WARNING: Failed consistency check ',sumcheck

    def getDomainCalcFileName(self):
        from inspect import getsourcefile
        from os.path import basename
        return basename(getsourcefile(self.defineJobs))

    def getSVNUrl(self,directToCalcDomainLine=False):
        from inspect import getsourcelines
        f=self.getDomainCalcFileName()
        if directToCalcDomainLine:
            (lines,lineno)=getsourcelines(self.calcDomains)
            f+='#L'+str(lineno)
        return 'https://svnweb.cern.ch/trac/atlasoff/browser/Tools/PmbUtils/trunk/python/'+f

    def getLogAndWarningThresholds_Domains(self,measname,domain,build,cache,cmtconfig):
        if measname=='vmem':
            return (5.0,30.0)
        return (None,None)

    def getLogAndWarningThresholds_Jobs(self,measname,jobname,build,cache,cmtconfig):
        if measname=='vmem':
            return (5.0,30.0)
        return (None,None)

class JobList( object ):

    def __init__ (self,domaincalculator):
        self.domaincalculator = domaincalculator
        self.jobs = {}

    def getCalculator(self):
        return self.domaincalculator

    def addJob(self,job):
        if self.domaincalculator.defineJobs().count(job.getIDStr())==0:
            raise Exception("Unknown job str "+str(job.getIDStr()))
        timelabel=job.getTime()
        if self.jobs.keys().count(timelabel)==0:
            self.jobs[timelabel] = [ job ]
        else:
            self.jobs[timelabel] += [ job ]

    def getTimes(self):
        times=self.jobs.keys()
        times.sort()
        return times

    def getTimeLabelByLabel(self,label):
        for t in self.jobs.keys():
            if t.getLabel()==label:
                return t
        return None

    def getJobs(self,time=None):
        if self.jobs.keys().count(time)==0:
            return None
        return self.jobs[time]

    def __getunit__(self,measname):
        for t in self.getTimes():
            for jobid in self.domaincalculator.defineJobs():
                unit = self.getMeasurement(jobid,measname,t).getUnit()
                if unit!=None:
                    return unit

    def getMeasurement(self,jobname,measname,time=None):
        if self.jobs.keys().count(time)==1:
            for job in self.jobs[time]:
                if job.getIDStr()==jobname:
                    return job.getMeasurement(measname)
        return Measurement(None)#NB: Can't call __getunit__ here!! (recursion)

    def getDomainResults(self,measname,time):
        if self.jobs.keys().count(time)==0:
            return None
        jobid2measval = {}
        for job in self.jobs[time]:
            if job!=None:
                val = job.getMeasurement(measname)
                cfg = job.getIDStr()
                if val!=None and cfg!=None:
                    jobid2measval[cfg]=val
        #fill up missing entries:
        jobids=self.domaincalculator.defineJobs()
        unit=self.__getunit__(measname)
        for jobid in jobids:
            if jobid2measval.keys().count(jobid)==0:
                jobid2measval[jobid]=Measurement(None,unit)
        results={}
        for k,v in self.domaincalculator.calcDomains(jobid2measval).items():
            results[k]=v
        return results

    #No function overloading in python?? Gah!!!:
    def getPlottableResult_SpecificMeasurementPerDomain(self,measname,title=None,domains=None,times=None):
        if domains==None:
            domains=self.getCalculator().defineDomains()
        else:
            knowndomains=self.getCalculator().defineDomains()
            for d in domains:
                assert d in knowndomains, "Requested domain unknown: "+str(d)
        colname2colour = {}
        for dom in domains:
            colname2colour[dom] = self.getCalculator().domainColour(dom)
        pr = PlottableResult(domains,
                             title=('Time development of '+measname if title==None else title),
                             colname2colour=colname2colour,unit=self.__getunit__(measname))
        for t in self.getTimes():
            if times!=None:
                found=False
                for allowedtime in times:
                    islabelstr=(type(allowedtime)==type(''))
                    if ((allowedtime==t.getLabel()) if islabelstr else (allowedtime==t)):
                        found=True
                        break
                if not found:
                    continue
            results=self.getDomainResults(measname,time=t)
            for dom in domains:
                pr.addPoint(dom,results[dom].getValue(),t)
        return pr

    def getPlottableResult_SpecificMeasurementPerJob(self,measname,title=None,jobids=None,times=None):
        if jobids==None:
            jobids=self.getCalculator().defineJobs()
        else:
            knownjobids=self.getCalculator().defineJobs()
            for j in jobids:
                assert j in knownjobids, "Requested jobid unknown: "+str(j)
        colname2colour = {}
        for jobid in jobids:
            colname2colour[jobid] = self.getCalculator().jobColour(jobid)
        pr = PlottableResult(jobids,
                             title=('Time development of '+measname if title==None else title),
                             colname2colour=colname2colour,unit=self.__getunit__(measname))
        for t in self.getTimes():
            if times!=None:
                found=False
                for allowedtime in times:
                    islabelstr=(type(allowedtime)==type(''))
                    if ((allowedtime==t.getLabel()) if islabelstr else (allowedtime==t)):
                        found=True
                        break
                if not found:
                    continue
            for jobid in jobids:
                pr.addPoint(jobid,self.getMeasurement(jobid,measname,t).getValue(),t)
        return pr

    def getAllMeasurementNames(self):
        (measurements,labels,unit)=self.__measurementlist__([])
        return measurements

    def __measurementlist__(self,measurements):
        if not isinstance(measurements,list):
            raise Exception('measurements has wrong type')
        #find all measurement names for all jobs:
        measinjob = []
        for t in self.getTimes():
            for job in self.getJobs(t):
                for measname in job.getAllMeasurementNames():
                    if measinjob.count(measname)==0:
                        measinjob += [measname]
        if len(measurements)==0:
            measurements = measinjob
        else:
            for measname in measurements:
                if measinjob.count(measname)==0:
                    raise Exception('Unknown measurement name: '+measname)
        unit=None
        units = []
        for measname in measurements:
            u=self.__getunit__(measname)
            if not u in units:
                units += [u]
        labels = []
        if len(units)>1:
            for measname in measurements:
                u=self.__getunit__(measname)
                labels += [ (measname + ('' if u==None else ' ['+str(u)+']')) ]
        else:
            unit = units[0] if len(units)!=0 else None
            labels += measurements
        return (measurements,labels,unit)

    def getPlottableResult_MeasurementsForSpecificJob(self,jobid,measurements=[],title=None):
        (measurements,labels,unit)=self.__measurementlist__(measurements)
        pr = PlottableResult(measurements,labels,
                             title=('Measurements from job '+jobid if title==None else title),
                             unit=unit)
        for t in self.getTimes():
            for measname in measurements:
                pr.addPoint(measname,self.getMeasurement(jobid,measname,t).getValue(),t)
        return pr

    def getPlottableResult_MeasurementsForSpecificDomain(self,domain,measurements=[],title=None):
        (measurements,labels,unit)=self.__measurementlist__(measurements)
        pr = PlottableResult(measurements,labels,
                             title=('Measurements from domain '+domain if title==None else title),
                             unit=unit)
        for t in self.getTimes():
            for measname in measurements:
                results=self.getDomainResults(measname,time=t)
                pr.addPoint(measname,results[domain].getValue(),t)
        return pr

class JobListPrinter(object):

    def __printTable__(self,table,colsepstr='  '):
        #table is a list of lines which are themselves lists, i.e. a 2D array.
        #
        #Special 'lines' accepted: And empty list denotes a blank
        #line, a list with one entry, '<line/>' gives a line, and a
        #list with one entry, <title>XXX gives a title, XXX.
        #
        #Return value is a list of strings (to be printed or written to a file)
        def isSpecialLine(line):
            return len(line)==0 or (len(line)==1 and (line[0]=='<line>' or line[0][0:7]=='<title>'))

        outlines=[]

        #run through data once to learn about number of columns and their widths
        ncols = 0
        for line in table:
            if not isSpecialLine(line):
                ncols = max(ncols,len(line))
        widths=[0]*ncols
        for line in table:
            if not isSpecialLine(line):
                for i in range(len(line)):
                    widths[i] = max(widths[i],len(str(line[i])))
        #Actual printing:
        totwidth=(sum(widths)+len(colsepstr)*(len(widths)-1))
        for line in table:
            if isSpecialLine(line):
                if len(line)==0:
                    outlines+=['']
                else:
                    if line[0]=='<line>':
                        outlines+=['-'*totwidth]
                    elif line[0][0:7]=='<title>':
                        title=str(line[0][7:])
                        l = long((totwidth - (len(title)+2*len(colsepstr)))/2)
                        if l<1:
                            outlines+=[title]
                        else:
                            outlines+=['-'*l+colsepstr+title+colsepstr+'-'*(totwidth-l-len(title)-2*len(colsepstr))]
                    else:
                        print "ERROR - should not happen!"
            else:
                tmp=''
                for i in range(len(line)):
                    tmp += colsepstr+line[i].rjust(widths[i])
                outlines+=[tmp[len(colsepstr):]]
        return outlines

    def printTable(self,plottableResult,toStdOut=True,outFile=None):
        times = plottableResult.getTimes()
        showdate = not ( len(times) == 1 and times[0] == None )
        ncols = plottableResult.getNColumns()
        #header:
        table = []
        table += [['<line>']]
        table += [['<title>'+plottableResult.getTitleWithUnit()]]
        table += [['<line>']]
        line=[]
        if showdate:
            line += ['date']
        for i in range(ncols):
            line += [plottableResult.getColumnTitle(i,applyOffset=False)]
        table += [line]
        #contents:
        maxval=0
        for t in times:
            for i in range(ncols):
                r=plottableResult.getValue(i,t,applyOffset=False)
                if r!=None:maxval=max(maxval,abs(r))
        ndigits=1
        if maxval<10: ndigits=2
        if maxval<1: ndigits=3
        for t in times:
            line=[]
            if showdate:
                line += [t.getLabel()]
            for i in range(ncols):
                r=plottableResult.getValue(i,t,applyOffset=False)
                format='%-6.'+str(ndigits)+'f'
                line += [str(format%r).strip()] if r!=None else '-'
            table += [line]
        #footer:
        table += [['<line>']]
        #print:
        outlines=self.__printTable__(table)
        if toStdOut:
            for outline in outlines:
                print outline
        if outFile!=None:
            file = open(outFile, 'w')
            for outline in outlines:
                file.write(outline+'\n')
            file.close()

class MatPlotLibInit(object):
    def setupMatPlotLib(self):
        import sys,matplotlib
        if not 'matplotlib.backends' in sys.modules:
            matplotlib.use( 'Agg' )

class JobListPlotter(object):

    matplotlib_setup = False

    def producePieChart(self,plottableResult,outfile,ignoreColumns=[]):
        MatPlotLibInit().setupMatPlotLib()
        from pylab import title,savefig,gca,axis,subplots_adjust,gcf,pie,clf,cla,figure
        pie_labels=[]
        pie_vals=[]
        pie_cols=[]

        fallbackcolsindex = 0
        somehadcolours=False
        somehadnocolours=False
        for i in range(plottableResult.getNColumns()):
            if ignoreColumns.count(plottableResult.getColumnName(i))!=0:
                continue
            val = plottableResult.getNewestValue(i)
            label = plottableResult.getColumnTitle(i,applyOffset=False)
            if val==None:
                print "producePieChart WARNING: No value found for "+label
                continue
            if val<0:
                print "producePieChart WARNING: Ignoring negative value for "+label
                continue
            if val==0:
                print "producePieChart WARNING: Ignoring zero value for "+label
                continue
            col = plottableResult.getColumnColour(i)
            if col==None:
                somehadnocolours=True
                col = ColourUtils.getFallBackColour(fallbackcolsindex)
                fallbackcolsindex += 1
            else:
                somehadcolours=True
            pie_cols   += [col]
            pie_vals   += [val]
            pie_labels += [label]
        if somehadnocolours and somehadcolours:
            print "producePieChart WARNING: Some, but not all, entries had a colour defined."
        clf()
        cla()
        figure(1, figsize=(6,6))
        subplots_adjust(left=0.15,right=0.85,bottom=0.0,top=0.9)
        gcf().set_size_inches( (7,7) )
        axis('equal')
        pieret = pie([v*1000000 for v in pie_vals],#the *1000000 is to work around matplotlib bug for small numbers
                     labels=pie_labels, autopct='%1.1f%%', shadow=True,colors=pie_cols,pctdistance=0.8)
        texts = pieret[1]
        for i in range(len(texts)):
            texts[i].set_color(pie_cols[i])
        totalstr=str(sum(pie_vals))+('' if plottableResult.getUnit()==None else ' '+str(plottableResult.getUnit()))
        title(plottableResult.getTitle()+' [total '+totalstr+']',
              bbox={'facecolor':'0.9', 'pad':12},fontsize=12)
        savefig(outfile)

    def produceTimeDevelopmentPlot(self,plottableResult,outfile,dolog=False,clipminval=None,clipmaxval=None,quietwarn=False,secondoutfile=None):
        #NB: dolog doesn't seem to always work!!
        maxAllowedNegativeFraction = plottableResult.getMaxAllowedNegativeFraction()
        if   maxAllowedNegativeFraction!=None and clipminval!=None:
            print "produceTimeDevelopmentPlot ERROR: Can't specify both maxAllowedNegativeFraction and clipminval"
            maxAllowedNegativeFraction=None

        MatPlotLibInit().setupMatPlotLib()

        ncols = plottableResult.getNColumns()

        from pylab import plot,xlabel,ylabel,title,grid,savefig,legend,xlim,ylim,text,axis,gca,subplots_adjust,gcf,semilogy,clf,cla,figure,axes
        clf()
        cla()
        rel_left_margin=0.08
        rel_right_margin=0.2
        subplots_adjust(left=rel_left_margin,right=1.0-rel_right_margin,bottom=0.15,top=0.9)
        dpi=100#FIXME: ASSUMPTION
        height_inches=7
        width_inches=10
        gcf().set_size_inches( (width_inches,height_inches) )
        default_fontsize=10
        fixrhmarginwidth_pixel=rel_right_margin*width_inches*dpi

        times = plottableResult.getTimes()
        if len(times)==0:
            if not quietwarn:
                print "WARNING: Can't produce time development plot '"+str(outfile)+"' since there are no data-points"
            return False
        #todo: make configurable:
        absolutetimepos = plottableResult.allTimeLabelsAreTimeStamps()
        rn=plottableResult.rightNow()
        def timepos(t,rightnow):
            if absolutetimepos:
                #rightnow=time()#fixme: "right now" should be part of the plottableresult
                oneday=3600*24
                return (t.unixTime()-rightnow)/oneday
            else:
                return times.index(t)-len(times)

        mintime=timepos(times[0],rn)
        maxtime=timepos(times[len(times)-1],rn)
        if len(times)==1:
            mintime-=0.5
            maxtime+=0.5
        minval=None
        maxval=None

        labelmargfact=0.02

        linew=2.0
        rhlabels=[]
        labels_ypos=[]
        for icol in range(ncols):
            #lastcol = (icol==ncols=1)
            #for defined points:
            x=[]
            y=[]
            for t in times:
                val=plottableResult.getValue(icol,t,applyOffset=True)
                thetime=timepos(t,rn)
                if val!=None and thetime!=None:
                    x += [thetime]
                    y += [val]
                    maxval = val if val>maxval or maxval==None else maxval
                    minval = val if val<minval or minval==None else minval

            if len(x)>0:
                c=plottableResult.getColumnColour(icol)
                plot(x,y, linewidth=linew,color=c,
                     marker=plottableResult.getColumnMarkerStyle(icol),
                     linestyle=plottableResult.getColumnLineStyle(icol))
                lastval=y[len(y)-1]
                labels_ypos+=[lastval]
                t=text(maxtime+labelmargfact*(maxtime-mintime),lastval,plottableResult.getColumnTitle(icol,applyOffset=True),fontsize=default_fontsize,verticalalignment='center',color=c)
                rhlabels+=[(t,lastval)]

        if plottableResult.getUnit()!=None:
            ylabel(plottableResult.getUnit())
        title(plottableResult.getTitleWithUnit())
        xlim(mintime,maxtime)
        if minval==None and maxval==None:
            minval=0.0
            maxval=1.0
        ylim_min = min(minval,0) if clipminval==None else clipminval
        ylim_max = maxval*1.05 if clipmaxval==None or clipmaxval<=0 else clipmaxval
        if maxAllowedNegativeFraction!=None and ylim_max>0:
            ylim_min = max(ylim_min,-maxAllowedNegativeFraction*ylim_max)
        if ylim_min==ylim_max:
            ylim_max = ylim_min + 1
        assert ylim_min<ylim_max, "Bad axis ranges"
        ylim(ylim_min,ylim_max)
        grid(True)
        gca().xaxis.labelpad = 10

        if (not absolutetimepos) or (maxtime-mintime)<40:
            #Turn off the default tick-labels and put in dates:
            ax = gca()
            xtl = ax.get_xticklabels()
            for atick in xtl:
                atick.set_visible(False)
            for t in times:
                s=t.getLabel()
                text(timepos(t,rn),ylim_min-0.02*(ylim_max-ylim_min),s,rotation='vertical',fontsize=default_fontsize,verticalalignment='top',horizontalalignment='center')
        else:
            xlabel('Days ago [0 is '+plottableResult.prettyDate(rn)+']')

        if dolog:
            semilogy()

        #Reduce font-size of long y-labels or of those close to each other:

        savefig(outfile)#To make sure all text objects have a renderer

        def widthOfArtist(a):
            p=a.get_window_extent().get_points()
            return p[1][0]-p[0][0]
        def heightOfArtist(a):
            p=a.get_window_extent().get_points()
            return p[1][1]-p[0][1]

        myinf=1.0e999
        labels_ypos.sort()
        maxwidth=fixrhmarginwidth_pixel*0.8
        def nearcount(labels_ypos,ypos,yspan):
            lower=ypos-yspan*0.025
            upper=ypos+yspan*0.025
            n=0
            for y in labels_ypos:
                if y>=lower and y<=upper:
                    n+=1
            return n-1

        for txt,ypos in rhlabels:
            widthnow=widthOfArtist(txt)
            iypos=labels_ypos.index(ypos)
            maxfsy=myinf
            if not dolog:
                nc=nearcount(labels_ypos,ypos,ylim_max-ylim_min)
                #disty=None
                #if iypos>0:
                #    disty=abs(ypos-labels_ypos[iypos-1])
                #if iypos<len(labels_ypos)-1:
                #    disty=min(disty,abs(ypos-labels_ypos[iypos+1]))
                #if disty!=None:
                #    rd=disty/(ylim_max-ylim_min)
                #    if rd<0.025:
                #        maxfsy=default_fontsize*rd/0.025
                #        if maxfsy<5:
                #            maxfsy=5
                if nc>0:
                    maxfsy=default_fontsize*max(0.6,(6-nc)/6.0)

            newfs=myinf
            if widthnow>maxwidth:
                newfs=maxwidth/widthnow*txt.get_fontsize()
            if newfs>maxfsy:
                newfs=maxfsy
            if newfs<0.5*myinf:
                txt.set_fontsize(newfs)

        #Final output:
        savefig(outfile)
        if secondoutfile:
            savefig(secondoutfile)
        return True

    def produceBarChart(self,plottableResult,outfile):
        times = plottableResult.getTimes()
        showdate = not ( len(times) == 1 and times[0] == None )
        assert showdate
        ntimes=len(times)
        showntimes=times#fixme: option for this
        allowedcolumns=None#['core','id','muon','calo'] #fixme: option for this
        titlebelowcols=False

        def timeToColour(t):
            return ColourUtils.getFallBackColour(showntimes.index(t))

        from RawBarChartUtils import RawBarPlotter, RawBarGroup, RawBarColumn, RawBarEntry
        rawbarplotter = RawBarPlotter()
        rawbarplotter.setTitle(plottableResult.getTitle())
        rawbarplotter.setAxisTitle(plottableResult.getUnit())
        i=0
        for t in showntimes:
            rawbarplotter.addLegendEntry(t.getLabel(),timeToColour(t))

        for i in range(plottableResult.getNColumns()):
            coltitle=plottableResult.getColumnTitle(i,applyOffset=True)
            if allowedcolumns!=None and not coltitle in allowedcolumns:
                continue
            #each result-column is a bar-group, and each result-time is a bar-column within:
            group=RawBarGroup(coltitle)
            for t in showntimes:
                column=RawBarColumn(coltitle=(t.getLabel() if titlebelowcols else None))
                r=plottableResult.getValue(i,t,applyOffset=True)
                if r!=None:
                    column.addEntry(RawBarEntry(r,timeToColour(t)))
                group.addColumn(column)
            rawbarplotter.addBarGroup(group)

        rawbarplotter.setDoShadow(True)
        rawbarplotter.producePlot(outfile)

class JobListMultiOutputHelper:

    @staticmethod
    def produceOutput(joblist,
                      #title=None gives a default title in all of the following entries
                      jobTables=[],#list of (showonstdout,filename,fieldname,title)
                      domainTables=[],#list of (showonstdout,filename,fieldname,dototal,title)
                      domainPieCharts=[],#List of (filename,label,fieldname,title) # label=None means a pie-chart using the most recent label for all domains
                      domainHistPlots=[],#list of (filename,fieldname,totaloffset,clipmin,clipmax,title,domainoffsets) #clipmin/clipmax==None => ignored
                      jobHistPlots=[],#list of (filename,fieldname,clipmin,clipmax,title) #clipmin/clipmax==None => ignored
                      singleFieldMultiLabelBarCharts=[],#list of (filename,fieldname,dojobs,totaloffset,timelabels,title)# timelabels=None for all, otherwise a list of timelabels. totaloffset==None => no total
                      ###############################
                      #'fin [mb]':      | : label1  #
                      #                 + : label2  #
                      #  +     |                    #
                      #  +|   +|                    #
                      #  +|   +|                    #
                      # core idex  ...              #
                      ###############################
                      displayMultiFieldSingleLabelDomainBarCharts=[],#list of (filename,label,[field1,field2,...],totaloffset,shownamesunderbars)
                                                                     #NB: All fields must have the same unit!
                      ########################################
                      #'label [mb]':                + : ini  #
                      #      |                      | : evt  #
                      #      |   -         |        - : fin  #
                      #  +   |   -         |   -             #
                      #  +   |   -     +   |   -             #
                      # ini evt fin   ini evt fin   ...      #
                      #    core          idex                #
                      ########################################
                      displayMultiFieldMultiLabelDomainBarCharts=[]#list of (filename,[field1,field2,...],[label1,label2,...])
                                                                   #NB: All fields must have the same unit!
                      ##########################################
                      #'[mb]':                        + : ini  #
                      #                               | : evt  #
                      #    -                          - : fin  #
                      #    -     -                             #
                      #    -     -                             #
                      #    |     -         -     -             #
                      #    |     |         -     |             #
                      #    |     |         |     |             #
                      #    |     |         |     |             #
                      #    +     +         |     +             #
                      #    +     +         +     +             #
                      # label1 label2   label1 label2  ...     #
                      #      core            idex              #
                      ##########################################
                      ):
        assert len(displayMultiFieldSingleLabelDomainBarCharts)==0, "displayMultiFieldSingleLabelDomainBarCharts not implemented!"
        assert len(displayMultiFieldMultiLabelDomainBarCharts)==0, "displayMultiFieldMultiLabelDomainBarCharts not implemented!"

        import os.path
        def fixPath(filename):
            return os.path.expanduser(filename) if filename!=None else None

        for showonstdout,filename,fieldname,title in jobTables:
            plotresults = joblist.getPlottableResult_SpecificMeasurementPerJob(fieldname,title=title)
            JobListPrinter().printTable(plotresults,toStdOut=showonstdout,outFile=fixPath(filename))

        for showonstdout,filename,fieldname,dototal,title in domainTables:
            plotresults = joblist.getPlottableResult_SpecificMeasurementPerDomain(fieldname,title=title)
            if dototal:
                total_colname=plotresults.addTotalEstimateColumn()
            JobListPrinter().printTable(plotresults,toStdOut=showonstdout,outFile=fixPath(filename))

        for filename,label,fieldname,title in domainPieCharts:
            tl = joblist.getTimeLabelByLabel(label) if label!=None and type(label)==type('') else label
            #assert tl!=None,"Unrecognised label: "+str(label)
            if title==None:
                title='Contribution to '+fieldname
            plotresults = joblist.getPlottableResult_SpecificMeasurementPerDomain(fieldname,
                                                                                  title=title,
                                                                                  times=([tl] if tl!=None else tl))
            JobListPlotter().producePieChart(plotresults,fixPath(filename))

        for filename,fieldname,totaloffset,clipmin,clipmax,title,domainoffsets in domainHistPlots:
            plotresults = joblist.getPlottableResult_SpecificMeasurementPerDomain(fieldname,title=title)
            plotresults.setMaxAllowedNegativeFraction(0.15)
            if totaloffset!=None:
                total_colname=plotresults.addTotalEstimateColumn()
                if totaloffset!=0:
                    plotresults.setColumnOffset(total_colname,totaloffset)
            if domainoffsets:
                for dom,offset in domainoffsets.items():
                    plotresults.setColumnOffset(dom,offset)
            JobListPlotter().produceTimeDevelopmentPlot(plotresults,fixPath(filename),dolog=False, clipminval=clipmin,clipmaxval=clipmax)

        for filename,fieldname,clipmin,clipmax,title in jobHistPlots:
            plotresults = joblist.getPlottableResult_SpecificMeasurementPerJob(fieldname,title=title)
            JobListPlotter().produceTimeDevelopmentPlot(plotresults,fixPath(filename),dolog=False, clipminval=clipmin,clipmaxval=clipmax)

        for filename,fieldname,dojobs,totaloffset,timelabels,title in singleFieldMultiLabelBarCharts:
            if title==None:
                title=fieldname+' contribution'
            if dojobs:
                plotresults = joblist.getPlottableResult_SpecificMeasurementPerJob(fieldname,title=title,times=timelabels)
            else:
                plotresults = joblist.getPlottableResult_SpecificMeasurementPerDomain(fieldname,title=title,times=timelabels)
            if totaloffset!=None:
                total_colname=plotresults.addTotalEstimateColumn()
                if totaloffset!=0:
                    plotresults.setColumnOffset(total_colname,totaloffset)
            JobListPlotter().produceBarChart(plotresults,fixPath(filename))

#auto-offset needs maxAllowedNegFraction=None
#clipping needs maxAllowedNegFraction:
