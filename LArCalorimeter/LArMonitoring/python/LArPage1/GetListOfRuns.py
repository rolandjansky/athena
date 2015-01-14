#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import getopt
import urllib2
import xmlrpclib
import os,re,time,sys,string,fileinput
import xmlrpclib
import GetNumberFromDB as DB
import ConfigParser 
config = ConfigParser.ConfigParser()
config.read('config.ini')
dqmpass = config.get('dqm', 'pass')


# return sorted hash of selected runs
# runs are selected in the range [firstrun,lastrun]
# nruns -- max number of runs in runlist 
#          <=0 -- max numbner is infinity
# datatag -- select runs with that tag (example "data12_8TeV")
def makerunlist(data):

    s = xmlrpclib.ServerProxy('https://'+dqmpass+'@atlasdqm.cern.ch')
    run_spec = {'stream': 'physics_IDCosmic', 'source': 'reproc', 'low_run': data['firstrun'], 'high_run': data['lastrun']}
    # runinfo structure:
    # * Run type (string)
    # * Project tag (string)
    # * Partition name (string)
    # * Number of events passing Event Filter (integer)
    # * Run start (seconds after 1970) (integer)
    # * Run end (seconds after 1970, 0 if run ongoing) (integer)
    # * Number of luminosity blocks (-1 if run ongoing) (string)
    # * Data source (string) - this is only kept for backward
    # compatibility and should be IGNORED
    # * Detector mask (string)
    # * Recording enabled (integer)
    # * Number of events in physics streams (integer) 
    runinfo = s.get_run_information(run_spec)
    stablebeaminfo = s.get_run_beamluminfo(run_spec)
    list_of_defects = ["LAR_UNCHECKED","LAR_BULK_UNCHECKED"]    
    defectsDBname=DB.GetDefectsDBName(data['firstrun'],data['lastrun'])
    defectsmap = s.get_defects_lb(run_spec, list_of_defects, 'HEAD', False, False, defectsDBname)

    # endoflooptime structure:
    # * the scheduled end of the Tier-0 calibration period in seconds
    # past January 1 1970,
    # * is whether the loop has actually ended or not (1=ended, 0=still open) 
    endoflooptime=s.get_end_of_calibration_period(run_spec) 
    curenttime=time.time()

    ListOfRuns = runinfo.keys()
    ListOfRunsSorted = []

    # There are 2 types of dq status of interest to DQ shifter :
    # 1. runs for which the express data quality assesment has to be
    #   completed. There are 2 conditions : the calibration loop has not
    #   expired (i.e "current date"< "end of calibration loop") and the
    #   LAR_UNCHECKED defect is still present
    # 2. runs for which the bulk data quality assessment has to be
    #   completed. The LAR_UNCHECKED defect is absent and the
    #   LAR_BULK_UNCHECKED defect is present.
    for i in xrange(0,len(ListOfRuns)):
        run=str(ListOfRuns[i])
        if runinfo[run][0]=='Physics' and \
           (data['datatag']=="notag" or runinfo[run][1]==data['datatag']) and \
           (int(data['beamcondition'])==0 or \
            (int(data['beamcondition'])==1 and \
             run in stablebeaminfo.keys() and
             str(stablebeaminfo[run][1])=='True')) and \
           (int(data['dqstatus'])==0 or \
            (int(data['dqstatus'])==1 and 
             run in endoflooptime.keys() and \
             curenttime<endoflooptime[run][0] and 
             run in defectsmap.keys() and \
             'LAR_UNCHECKED' in defectsmap[run]) or
            (int(data['dqstatus'])==2 and \
             run in defectsmap.keys() and \
             'LAR_UNCHECKED' not in defectsmap[run] and \
             'LAR_BULK_UNCHECKED' in defectsmap[run])):
            ListOfRunsSorted.append(int(run))

    ListOfRunsSorted.sort()
    ListOfRunsSorted.reverse()
    runlist={}
    irun=0
    nruns=int(data['nruns'])
    for i in xrange(0,len(ListOfRunsSorted)):
        run=ListOfRunsSorted[i]
        if nruns>0 and irun > nruns-1: break
        number_events = DB.GetNumberOfCosmicCaloEvents(run)
        int_lumi = float(runinfo[str(run)][4])/1000.
        NLB = int(runinfo[str(run)][6])
        if number_events > int(data['minnevent']) and \
           NLB > int(data['minnlb']) and \
           int_lumi > float(data['minlum']):
            runlist[irun]=run
            irun=irun+1

    return runlist


if __name__ == "__main__":

# for test purpose
       
    data= {}
    data['firstrun'] = 215400
    data['lastrun'] = 215450
    data['datatag'] = "data12_8TeV"
    data['beamcondition'] =1

#    data['firstrun'] = 248000
#    data['lastrun'] = 248371 
#    data['firstrun'] = 248115
#    data['lastrun'] = 248115 
#    data['datatag'] = "data14_cos"
#    data['beamcondition'] =0

    data['nruns']=-1
    data['minnevent'] = 10000
    data['dqstatus'] =0 
    data['minnlb'] =0 
    data['minlum'] = 0.

    runlist=makerunlist(data)
    print runlist

