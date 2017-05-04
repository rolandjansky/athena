# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from model.COOL.COOLQuery import COOLQuery
from model.COOL.COOLTime import COOLTime

from model.basics.DataSelection import DataSelection
from model.basics.DataRange import DataRange

from model.physics.filters.FilterRunDuration import FilterRunDuration



from datetime import datetime
from time import mktime



startDate = datetime(2011,9,24)
stopDate = datetime(2011,10,4)

startTime = mktime(startDate.timetuple())*(10**9)
stopTime = mktime(stopDate.timetuple())*(10**9)


fillsTimeRanges = {}

q = COOLQuery()
q.SetIoV(startTime, stopTime)
for obj, payload in q.GetFolder("/LHC/DCS/FILLSTATE"):
	#print obj.since(), obj.until(), payload["FillNumber"]

	fillNo = payload["FillNumber"]

	if not fillsTimeRanges.has_key(fillNo):
		fillsTimeRanges[fillNo] = DataRange(obj.since())

	fillsTimeRanges[fillNo].SetLast(obj.until() - 1.0*10**9)

print "[+] Fill info loaded"



fillsRunLbRanges = {}


for fillNo, timeRange in fillsTimeRanges.items():
	print fillNo,
	coolRange = DataRange()

	q.SetIoV(timeRange.GetFirst(), timeRange.GetLast())
	for obj, payload in q.GetFolder("/TRIGGER/LUMI/LBTIME"):

		runLb = COOLTime(payload["Run"], payload["LumiBlock"])

		if not coolRange.GetFirst(): coolRange.SetFirst(runLb)

		coolRange.SetLast(runLb-1)
	
	if not coolRange.IsEmpty():
		fillsRunLbRanges[fillNo] = coolRange


	print coolRange.GetFirst(), coolRange.GetLast()



for fillNo, coolRange in fillsRunLbRanges.items():
	selection = DataSelection()
	selection.SetListOfRanges([coolRange])

	selection = FilterRunDuration().FilterValues(selection)

	for subrange in selection.GetListOfRanges():
		print fillNo, subrange
	
"""
timeSelection = DataSelection()
timeSelection.SetListOfRanges( fillsTimeRanges.values() )
q.SetIoVSelection(timeSelection)


fillsRunLb = {}

for obj, payload in q.GetFolder("/TRIGGER/LUMI/LBTIME"):
	print COOLTime(payload["Run"], payload["LumiBlock"])
"""














#import multiprocessing
#q = multiprocessing.Queue()

#periodK = [186873, 186877, 186878, 186923, 186933, 186934, 186965, 187014, 187196, 187219, 187453, 187457, 187501, 187543, 187552, 187763, 187811, 187812, 187815]

"""
from model.basics.TRPFile import TRPFile
trp = TRPFile(186877)
trp.make_local()
"""    


#from model.basics.TRPFile import TRPFile
#print TRPFile(187219).get_path()

#from model.COOL.COOLQuery import COOLQuery
#q = COOLQuery()
#print q.GetConnectionString("/TRIGGER/HLT/Menu")
#q.GetFolderHandle("/TRIGGER/HLT/EFRates", "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=MONP200;")

"""
#!/bin/env python
#
# ----------------------------------------------------------------
# Script : AtlRunQueryAMI.py
# Project: AtlRunQuery
# Purpose: Utility to retrieve information from AMI
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 10, 2009
# ----------------------------------------------------------------
#

import re

class ARQ_AMI:

    _amiclient = None
    store = {}
    all_periods = None

    @classmethod
    def amiclient(cls):
        if cls._amiclient == None:
            cls._amiclient = cls.OpenAMIConnection()
        return cls._amiclient

    @classmethod
    def OpenAMIConnection(cls):
        try:
            from pyAMI.pyAMI import AMI
            amiclient = AMI(False)
            return amiclient
        except ImportError:
            print 'ERROR: could not load pyAMI'
            return None


    @classmethod
    def get_periods_for_run(cls, run):
        try:    run = int(run)
        except: return []
        if not run in cls.store:
            try:
                print 'GetDataPeriodsForRun', '-runNumber=%i' % run
                result = cls.amiclient().execute(['GetDataPeriodsForRun', '-runNumber=%i' % run])
                cls.store[run] = sorted([ (int(e['periodLevel']),e['period'],e['project']) for e in result.getDict()['Element_Info'].values() ])
            except:
                print "Exception"
                cls.store[run] = []
        return [x[1] for x in cls.store[run]]


    @classmethod
    def get_periods(cls, year, level):
        try:
            cmd = ['ListDataPeriods', '-createdSince=2009-01-01 00:00:00' ]
            if year>2000:
                cmd += [ '-projectName=data%02i%%' % (year-2000) ]
            if level in [1,2,3]:
                cmd += [ '-periodLevel=%i' % level ]
            result = cls.amiclient().execute(cmd)
            rows = [ (e['period'], e['projectName']) for e in result.getDict()['Element_Info'].values() ]
            return sorted(rows)
        except Exception , e:
            print e
            return []


    @classmethod
    def get_all_periods(cls):
        if cls.all_periods != None: return cls.all_periods
        cls.all_periods = []
        p = re.compile("(?P<period>(?P<periodletter>[A-Za-z]+)(?P<periodnumber>\d+)?)$")
        try:
            result = cls.get_periods(0, 0)
            for period, projectName in result:
                m = p.match(period)
                if not m: continue
                year = int(projectName[4:6])
                period_letter = m.group('periodletter')
                period_number = int(m.group('periodnumber')) if m.group('periodnumber') else 0
                if len(period_letter)!=1: pc = 0
                else: pc = 10000*year + 100*(ord(period_letter.upper())-65) + period_number
                cls.all_periods += [ ((year, period, pc), projectName+".period" + period) ]
            cls.all_periods.sort()
        except Exception, e:
            print e
            pass
        return cls.all_periods
    

    @classmethod
    def get_runs(cls, period, year):
        try:
            cmd = ['GetRunsForDataPeriod', '-period=%s' % period]
            if year>2000:
                cmd += [ '-projectName=data%02i%%' % (year-2000) ]
            result = cls.amiclient().execute(cmd)
            #print "amiCommand",' '.join(cmd)        
            r = sorted([ int(e['runNumber']) for e in result.getDict()['Element_Info'].values() ])
            return r
        except: pass



# for testing purpose
if __name__ == "__main__":

    choice = 1
    while choice != 0:
        print "\n1 - periods for run"
        print "2 - runs for period (and year)"
        print "3 - periods (by year and/or level)"
        print "4 - all periods (different format)"
        print "\n0 - exit\n"

        choice = raw_input("  enter your choice: ")
        choice = int(choice) if choice.isdigit() else 0
        if choice==1:
            run = int(raw_input("  run number: "))
            print ARQ_AMI.get_periods_for_run( [run] )
        elif choice==2:
            period = raw_input("  period           : ")
            year   = raw_input("  year <RET> = all : ")
            year   = int(year) if year.isdigit() else 0
            print ', '.join([str(x) for x in ARQ_AMI.get_runs(period, year)])
        elif choice==3:
            year   = raw_input("  year <RET> = all           : ")
            year   = int(year) if year.isdigit() else 0
            period = raw_input("  period [1|2|3] <RET> = all : ")
            period = int(period) if period.isdigit() else 0
            print ARQ_AMI.get_periods(year, period)
        elif choice==4:
            print ARQ_AMI.get_all_periods()
"""
