# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function
import re,sys
from time import gmtime

from CoolRunQuery.AtlRunQueryRun import Run
from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn, GetRanges
from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector, RunLBBasedCondition, TimeBasedCondition, DataKey


class FilenameSelector(RunLBBasedCondition):
    def __init__(self, name, projecttag=None):
        self.filenametag = projecttag
        self.fntpatterns = None
        if projecttag:
            self.fntpatterns = [re.compile(pt.strip().replace('*','.*').replace('?','.'),re.I) for pt in projecttag.split(',')]
        super(FilenameSelector,self).__init__(name=name,
                                              dbfolderkey='COOLONL_TDAQ::/TDAQ/RunCtrl/SOR%s' % ("_Params" if Selector.condDB() == "COMP200" else ""),
                                              channelKeys = [(0, 'Project tag','FilenameTag' if Selector.condDB() == "COMP200" else 'T0ProjectTag')])

    def __str__(self):
        if self.applySelection:
            return 'SELOUT Checking if the filename tag matches "%s"' % self.filenametag
        else:
            return "Retrieving filenametag"

    def passes(self,value,key):
        for p in self.fntpatterns:
            if p.match(value):
                return True
        return False

class PartitionSelector(RunLBBasedCondition):
    def __init__(self, name, partition=None):
        self.partition = partition
        super(PartitionSelector,self).__init__(name=name,
                                               dbfolderkey='COOLONL_TDAQ::/TDAQ/RunCtrl/EventCounters',
                                               channelKeys = [(0,'Partition','PartitionName')])

    def __str__(self):
        if self.applySelection:
            return 'SELOUT Checking if partition name matches "%s"' % self.partition
        else:
            return "Retrieving partition name"

    def passes(self,value,key):
        if value=='n.a.':
            self.selDataMissing = True
            return True
        return value==self.partition

class ReadyForPhysicsSelector(RunLBBasedCondition):
    def __init__(self, name, readyforphysics=None):
        self.readyforphysics = readyforphysics
        super(ReadyForPhysicsSelector,self).__init__(name=name,
                                                     dbfolderkey='COOLONL_TDAQ::/TDAQ/RunCtrl/DataTakingMode',
                                                     channelKeys = [(0,'Ready for physics','ReadyForPhysics')])

    def __str__(self):
        if self.applySelection:
            return 'SELOUT Checking if ReadyForPhysics flag matches "%s"' % self.readyforphysics
        else:
            return "Retrieving ReadyForPhysics flag"

    def passes(self,value,key):
        if value=='n.a.':
            self.selDataMissing = True
            return True
        return value==self.readyforphysics


class DurationSelector(Selector):
    def __init__(self,name,duration):
        self.geReq = (duration[-1]!='-')
        self.duration = int(duration.rstrip('+-'))
        super(DurationSelector,self).__init__(name)
    def __str__(self):
        return "SELOUT Checking if duration of run is %s than %i seconds" % ("more" if self.geReq else "less",self.duration)
    def select(self, runlist):
        print (self, end='')
        sys.stdout.flush()
        if self.geReq:
            rmlist = [r for r in runlist if (r.eor-r.sor)/1E9<self.duration]
        else:
            rmlist = [r for r in runlist if (r.eor-r.sor)/1E9>self.duration]
        for r in rmlist:
            runlist.remove(r)
        if self.applySelection:
            print (" ==> %i runs selected" % len(runlist))
        return runlist


class DetectorSelector(RunLBBasedCondition):
    def __init__(self, name, dmin=None, dmout=None):
        self.bm = self.bp = self.bmany = 0
        if not dmin:
            dmin = []
        if not dmout:
            dmout = []
        for m in dmin+dmout:
            if 'A' in m:
                continue
            self.bm |= int(m)
        for m in dmin:
            if 'A' in m:
                continue
            self.bp |= int(m)
        for m in dmin:
            if 'A' not in m:
                continue
            self.bmany |= int(m.rstrip('A'))

        dbfolder = ""
        if Selector.condDB() == "COMP200":
            dbfolder = "/TDAQ/RunCtrl/SOR_Params"
        if Selector.condDB() == "CONDBR2":
            dbfolder = "/TDAQ/RunCtrl/SOR"

        super(DetectorSelector,self).__init__(name=name,
                                              dbfolderkey='COOLONL_TDAQ::%s' % dbfolder,
                                              channelKeys = [(0, 'Detector systems', 'DetectorMask')])

        self.data_keys[0]._type = DataKey.DETECTOR



    def __str__(self):
        if self.applySelection:
            if self.bm!=0 and self.bmany!=0:
                return "SELOUT Checking if [detector mask & %i] matches %i and [detector mask & %i] is greater 0" % (self.bm,self.bp,self.bmany)
            elif self.bm==0:
                return "SELOUT Checking if [detector mask & %i] is greater 0" % self.bmany
            else:
                return "SELOUT Checking if [detector mask & %i] matches %i" % (self.bm,self.bp)
        else:
            return "Retrieving detector mask"

    def passes(self,values,key):
        try:
            val = int(values,16)
        except ValueError:
            self.selDataMissing = True
            return True
        retval = True
        if self.bm!=0:
            retval &= ((val & self.bm) == self.bp)
        if self.bmany!=0:
            retval &= ((val & self.bmany) != 0)
        return retval

    def prettyValue(self, value, key):
        """
        run 1: run 211541 has DetectorMask 281474976710647
        run 2: run 252233 has DetectorMask 0d00069fffffffff0
        """
        return  value




class BFieldCondition(TimeBasedCondition):
    def __init__(self, name, condition=None, channel=0, resDictKey=''):
        super(BFieldCondition,self).__init__(name,
                                             dbfolderkey='COOLOFL_DCS::/EXT/DCS/MAGNETS/SENSORDATA',
                                             channelKeys = [(channel,resDictKey,'value')])
        if condition:
            self.cutRange = GetRanges(condition)
    def __str__(self):
        if self.applySelection:
            if self.cutRange[0][1] > 1000000:
                txt = '[%.0f,+inf]' % (self.cutRange[0][0])
            else:
                txt = '[%.0f,%0.f]' % (self.cutRange[0][0],self.cutRange[0][1])
            return "SELOUT Checking if the %s is within %s" % (', '.join(self.ResultKey()), txt)
        else:
            return "Retrieving magnet currents %s" % self.ResultKey()

    def passes(self,values,key):
        try:
            val = abs(float(values))
        except ValueError:
            self.selDataMissing = True
            return True            
        for cr in self.cutRange:
            if val>=cr[0] and val<=cr[1]:
                return True
        return False
    
    def prettyValue(self, value, key):
        if value=='n.a.':
            return value
        return round(float(value),2)


class BFieldSelector(Selector):
    def __init__(self, name, bf=None):
        super(BFieldSelector,self).__init__(name)
        self.conditions = []
        if bf:
            if 'solenoidon' in bf or 'solenoidoff' in bf:
                tmp = []
                if   'solenoidon' in bf:
                    tmp += ['7700+']
                elif 'solenoidoff' in bf:
                    tmp += ['10-']
                self.conditions += [ BFieldCondition('csolcur', ','.join(tmp), channel = 1, resDictKey = 'SolCurrent') ]
            if 'toroidon' in bf or 'toroidoff' in bf:
                tmp = []
                if   'toroidon'  in bf:
                    tmp += ['20000+']
                elif 'toroidoff' in bf:
                    tmp += ['10-']
                self.conditions += [ BFieldCondition('ctorcur', ','.join(tmp), channel = 3, resDictKey = 'TorCurrent') ]
        else:
            self.conditions += [ BFieldCondition('csolcur', channel = 1, resDictKey = 'SolCurrent') ]
            self.conditions += [ BFieldCondition('ctorcur', channel = 3, resDictKey = 'TorCurrent') ]
            for sel in self.conditions:
                sel.setShowOutput()
                sel.applySelection = False

    def __str__(self):
        s = ""
        for sel in self.conditions:
            s += "\n%s" % sel
        return s

    def setShowOutput(self):
        for cond in self.conditions:
            cond.setShowOutput()

    def addShowSelector(self):
        sol = tor = None
        for s in self.conditions:
            if s.name == 'csolcur':
                sol = s
            if s.name == 'ctorcur':
                tor = s
        if not sol:
            sol = BFieldCondition('csolcur', channel = 1, resDictKey = 'SolCurrent')
            sol.applySelection = False
            self.conditions += [ sol ]
        if not tor:
            tor = BFieldCondition('ctorcur', channel = 3, resDictKey = 'TorCurrent')
            tor.applySelection = False
            self.conditions += [ tor ]
        sol.setShowOutput()
        tor.setShowOutput()
            
    def select(self, runlist):
        for sel in self.conditions:
            runlist = sel.select(runlist)
        return runlist


class BPMSelector(Selector):
    def __init__(self, name, events=None):
        self.name = name
        self.selpattern = []
        super(BPMSelector,self).__init__(name)

    def __str__(self):
        return "Retrieving Beam Position Monitor values from PVSS archive"
    def passes(self,values,key):
        return True
    def setShowOutput(self):
        pass
    def select(self, runlist):
        return runlist
    
    def runAfterQuery(self,runlist):
        # do show selection here
        from CoolRunQuery.AtlRunQueryPVSS import GetPVSS_BPMs
        pvssdb = coolDbConn.GetPVSSDBConnection()
        cursor = pvssdb.cursor()
        cursor.arraysize = 1000
        for r in runlist:
            t = gmtime(r.sor/1.E9)
            sor = "%02i-%02i-%4i %02i:%02i:%02i" % (t[2], t[1], t[0], t[3], t[4], t[5])
            t = gmtime(r.eor/1.E9)
            eor = "%02i-%02i-%4i %02i:%02i:%02i" % (t[2], t[1], t[0], t[3], t[4], t[5])
            
            # retrieve values
            res = GetPVSS_BPMs( cursor, sor, eor )
            r.addResult('BPM', res )

        Run.AddToShowOrder(DataKey('BPM'))



class DatasetsSelector(Selector):
    def __init__(self, name, events=None):
        self.name = name
        self.selpattern = []
        self.showCAFLinks = False
        super(DatasetsSelector,self).__init__(name)

    def __str__(self):
        return "Retrieving datasets from Tier-0 DB"
    def passes(self,values,key):
        return True
    def setShowOutput(self):
        pass
    def select(self, runlist):
        return runlist
    def addShowDatasetPattern(self, pattern = ''):
        # format: '*NTUP*,*dESD*,... [caf]'
        if pattern.lower() == 'caf':
            self.showCAFLinks = True
        elif pattern:
            # first check if 'caf'
            sp = pattern.split()
            if len(sp)>1:
                if 'caf' == sp[0].lower():
                    self.showCAFLinks = True
                    pattern = sp[1]
                elif 'caf' == sp[1].lower():
                    self.showCAFLinks = True
                    pattern = sp[0]
                # sanity check
                if len(sp) != 2 or not self.showCAFLinks:
                    print ('ERROR: wrong format in "show dataset". Usage: "show dataset [pattern] [caf]"')
                    sys.exit(1)
                                       
            self.selpattern = pattern.split(',')
    
    def runAfterQuery(self,runlist):
        # do show selection here
        from CoolRunQuery.AtlRunQueryTier0 import GetTier0_allDatasets
        runnrlist = [r.runNr for r in runlist]
        tier0connection  = coolDbConn.GetTier0DBConnection()
        tier0retdico     = GetTier0_allDatasets( tier0connection.cursor(), runnrlist, self.selpattern )

        for run in runlist: # go through old runlist and see
            if run.runNr in tier0retdico:
                run.addResult('Datasets', tier0retdico[run.runNr])
            else:
                run.addResult('Datasets', {})

        Run.AddToShowOrder(DataKey('Datasets'))
        Run.showCAFLinks = self.showCAFLinks



class LArcondSelector(RunLBBasedCondition):
    def __init__(self, name, larcond=[]):
        super(LArcondSelector,self).__init__(name=name,
                                             dbfolderkey='COOLONL_LAR::/LAR/Configuration/RunLog',
                                             channelKeys = [(0,'lar:runtype',  'runType'),
                                                            (0,'lar:nsamples', 'nbOfSamples'),
                                                            (0,'lar:format',   'format')])
        # define arguments (after initialising bas class!)
        self.larcond = {}
        if larcond:
            for c in larcond:
                # format: 'nsamples 7' or 'runtype 1', etc
                larcondargs = c.split()
                if len(larcondargs) == 2:
                    key = 'lar:' + larcondargs[0].strip().lower()
                    # does it exist?
                    if key not in self.ResultKey():
                        print ('ERROR: unknown larcond variable "%s"' % key)
                        sys.exit(1)                
                    self.larcond[key] = larcondargs[1].strip()
                else:
                    print ('ERROR: unknown condition format for larcond: "%s" -> need two arguments separated by blank' % larcondargs)
                    sys.exit(1)                
                    
                    
    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if LAr condition matches %s" % self.larcond
        else:
            return "Retrieving LAr run conditions"

    def passes(self,values,key):
        if key.lower().strip() in self.larcond:
            if values.strip() == self.larcond[key.lower().strip()]:
                return True
            else:
                return False
        return True

