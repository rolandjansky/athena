# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import datetime

class WarnData:

    def __init__(self):
        self.vars={}
        self.dates=[]
        self.unit=None

    def hasVariable(self,name):
        return name in self.vars.keys()

    def defineMonitoredVariable(self,name,warn_threshold,log_threshold,log_negativethreshold='auto'):
        #put warn/log thresholds to None to disable and to 0 to let everything pass
        assert name and not name in self.vars.keys()
        if log_negativethreshold=='auto':
            log_negativethreshold=-log_threshold
        assert warn_threshold==None or warn_threshold>=0
        assert log_threshold==None or log_threshold>=0
        assert log_negativethreshold==None or log_negativethreshold<=0
        self.vars[name]={'points':{},'thresholds':{'warn':warn_threshold,'log':log_threshold,'neglog':log_negativethreshold}}

    def addPoint(self,varname,thedate,thevalue):
        assert varname and varname in self.vars.keys()
        assert not thedate in self.vars[varname]['points'].keys()
        self.vars[varname]['points'][thedate]=thevalue
        if not thedate in self.dates:
            self.dates+=[thedate]
            self.dates.sort()

    def getUnit(self):
        return self.unit

    def getLastDate(self):
        if self.dates:
            return self.dates[-1]

    def getAllDates(self):
        return self.dates

    def getChanges(self,thedate=None,dontWarnIfPreviousNegative=True):
        if not thedate: thedate=self.getLastDate()
        changes={}
        for var,info in self.vars.items():
            if not thedate in info['points'].keys(): continue
            theval=info['points'][thedate]
            #find previous value:
            pre_date=None
            history_dates=[thedate]
            for d in info['points'].keys():
                if d<thedate:
                    history_dates+=[d]
                    if (pre_date==None or pre_date<d):
                        pre_date=d
            if not pre_date:
                continue
            history_dates.sort()
            history_vals=[]
            for hd in history_dates:
                history_vals+=[info['points'][hd]]
            pre_val=info['points'][pre_date]
            delta=theval-pre_val
            #if the value is actually a
            #ValuePerDomainCalcUtils.Measurement instance, the
            #values might have measurement errors, and we can see
            #if the difference is actually significant:
            error=None
            unit=None
            pre_val_value=pre_val
            try:
                error=delta.getError()
                unit=delta.getUnit()
                delta=delta.getValue()
                pre_val_value=pre_val_value.getValue()
            except:
                unit=None
                error=None
                pass
            if unit:
                if self.unit==None: self.unit=unit
                else: assert unit==self.unit
            if error and error>0:
                delta_significant = (abs(delta)/error>2.0)
                preval_significantly_negative=pre_val_value<-2.0*error
            else:
                delta_significant=(str(delta)!=str(0.0))
                preval_significantly_negative=pre_val_value<0.0

            #NB: If previous value
            #look at warn thresholds to see if we should warn:
            thr_log=info['thresholds']['log']
            thr_neglog=info['thresholds']['neglog']
            thr_warn=info['thresholds']['warn']
            shouldlog=False
            shouldwarn=False
            if thr_warn==0:
                shouldwarn=True
            elif thr_warn!=None and delta_significant:
                shouldwarn = (delta>=thr_warn)
            if shouldwarn and preval_significantly_negative and dontWarnIfPreviousNegative:
                shouldwarn=False
            #see if we should log:
            if delta<0:
                thr_log=-thr_neglog
            if thr_log==0:
                shouldlog=True
            elif thr_log!=None and delta_significant:
                shouldlog = (abs(delta)>=thr_log)
            unitstr=''
            if unit: unitstr=' '+str(unit)
            change_id=':'.join([str(var),str(pre_date),str(pre_val)+unitstr,str(thedate),str(theval)+unitstr])
            n=10
            if len(history_vals)>n:
                history_vals=history_vals[len(history_vals)-n:]
                assert len(history_vals)==n
            changes[var]={'pre_date':pre_date,'pre_val':pre_val,'date':thedate,'val':theval,'log':(shouldlog or shouldwarn),
                          'warn':shouldwarn,'change_id':change_id,'history':history_vals}
        return changes

    def getConciseChanges(self,thedate=None,blockfile=None):
        #Ignore non-loggable changes, find all date transitions, sort changes by delta, figure out if changes warrent a warning
        previousreports=[]
        if blockfile and os.path.isfile(blockfile):
            for l in open(blockfile):
                previousreports+=[l.strip()]
        haswarning=False
        repeat_reports=False
        new_reports=False
        date_transitions=[]
        changes={}
        for var,info in self.getChanges(thedate).items():
            if not info['log']:
                continue
            prev_report = ( info['change_id'] in previousreports)
            if prev_report: repeat_reports = True
            else: new_reports=True
            dt=(info['pre_date'],info['date'])
            if not dt in date_transitions: date_transitions+=[dt]
            if info['warn']: haswarning=True
            d={'var':var,'reported_previously':prev_report}
            for k,v in info.items():
                if k!='log': d[k]=v
            try: sortkey=((d['val']-d['pre_val']).getValue(),var)
            except: sortkey=(d['val']-d['pre_val'],var)
            assert sortkey not in changes
            changes[sortkey] = d
        date_transitions.sort()
        sortedchanges=[]
        sortkeys=changes.keys()[:]
        sortkeys.sort()
        sortkeys.reverse()
        for sortkey in sortkeys:
            sortedchanges+=[changes[sortkey]]
        return {'haswarning':haswarning,'repeat_reports':repeat_reports,'new_reports':new_reports,'date_transitions':date_transitions,'changes':sortedchanges}

#Method for tying the above together with the other PMB classes:

def constructWarnData(joblist,measname,build,cache,cmtconfig,domains=True):
    assert domains#for now...
    calc=joblist.getCalculator()
    w=WarnData()
    for time in joblist.getTimes():
        for var,value in joblist.getDomainResults(measname,time).items():
            try: invalid=value.invalid()
            except: invalid=False
            if invalid: continue
            if not w.hasVariable(var):
                #Fixme: Ask the domaincalc here for the actual thresholds!!
                (log_thr,warn_thr)=calc.getLogAndWarningThresholds_Domains(measname,var,build,cache,cmtconfig)
                w.defineMonitoredVariable(var,warn_threshold=warn_thr,log_threshold=log_thr)
            w.addPoint(var,time.getDate(),value)
    return w

def actualVal(v):
    try: actualval=v.getValue()
    except: actualval=v
    return actualval
def valToStr(v,showPlus=False):
    try: unit=v.getUnit()
    except: unit=''
    #if unit: unit=' '+unit
    v=actualVal(v)
    s=('%.1f'%v)+unit
    if v>=0 and showPlus: s='+'+s
    return s

def formatConciseChanges(conciseChanges,blockfile=None):
    lines =[]
    newreportids=[]
    for change in conciseChanges['changes']:
        if lines: lines+=['']
        prev_rep=change['reported_previously']
        lines+=['Domain  : '+change['var']+('*' if prev_rep else '')]
        if not prev_rep:
            newreportids+=[change['change_id']]
        #u=change['val'].getUnit()
        #if u: u=' '+u
        lines+=['Change  : %s (%s -> %s)'%(valToStr(change['val']-change['pre_val'],showPlus=True),change['pre_date'],change['date']) ]
        #lines+=['Dates  : %s -> %s'%(change['pre_date'],change['date'])]
        hist=[]
        for hv in change['history']:
            hist+=[valToStr(hv)]
        lines+=['History : '+', '.join(hist)]
    if conciseChanges['repeat_reports']:
        lines+=['']
        lines+=['*: Reported previously']

    if newreportids and blockfile:
        #append new lines
        fh = open ( blockfile, 'a' )
        for line in newreportids:
            assert not '\n' in line
            fh.write ( line+'\n' )
        fh.close()

    return lines

if __name__=='__main__':
    w=WarnData()

    #Some bogus data to test the class:

    w.defineMonitoredVariable('var1',warn_threshold=10.0,log_threshold=5.0)
    w.defineMonitoredVariable('var2',warn_threshold=10.0,log_threshold=5.0)
    w.defineMonitoredVariable('var3',warn_threshold=20.0,log_threshold=10.0)

    day1=datetime.date(2010, 1, 5)
    day2=datetime.date(2010, 1, 8)
    day3=datetime.date(2010, 1, 9)
    day4=datetime.date(2010, 1, 10)

    w.addPoint('var1',day1,17.0)
    w.addPoint('var1',day2,18.0)
    w.addPoint('var1',day4,10.0)

    w.addPoint('var2',day1,10.0)
    w.addPoint('var2',day2,18.0)
    w.addPoint('var2',day3,38.0)
    w.addPoint('var2',day4,48.1)

    w.addPoint('var3',day2,-10.0)
    w.addPoint('var3',day4,20.0)

    print w.getConciseChanges()
