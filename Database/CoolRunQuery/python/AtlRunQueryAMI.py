#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryAMI.py
# Project: AtlRunQuery
# Purpose: Utility to retrieve information from AMI
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 10, 2009
# ----------------------------------------------------------------
#

from __future__ import print_function
import re

class ARQ_AMI:

    _amiclient = None
    store = {}
    all_periods = None


    @classmethod
    def getAmiClient(cls, amiconf = None, verbose = False):
        """get ami client
        param amiconf: name of file with AMI user and pw

        If a valid filename is specified, it tries to read the
        username and password from there. If that does not succeed or
        no filename is specified, voms based authentication is tried.
        """
        from pyAMI.pyAMI import AMI,pyAMIEndPoint
        from os import stat,path

        useConfigFile = False
        if amiconf:
            if not path.exists(amiconf):
                if verbose:
                    print ("WARNING: AMI config file", amiconf, "does not exist. Need to rely on valid voms proxy.")
            elif stat(amiconf).st_mode & path.stat.S_IRUSR == 0:
                if verbose:
                    print ("WARNING: AMI config file", amiconf, "exists but is not readable. Need to rely on valid voms proxy.")
            else:
                useConfigFile = True
                
        if useConfigFile:
            pyAMIEndPoint.setType("replica")
            ami=AMI()
            ami.readConfig(amiconf)
            if ami.checkAuth():
                print ("... connecting to CERN AMI replica with user+pw")
                return ami
            pyAMIEndPoint.setType("main")
            ami.certAuth()
            if ami.checkAuth():
                print ("... connecting to AMI main server with user+pw")
                return ami

            print ("WARNING: Authentication in config file",amiconf,"not valid, check format, user, pw. Need to rely on valid voms proxy.")


        pyAMIEndPoint.setType("replica")
        ami=AMI()
        if ami.checkAuth():
            print ("... connecting to CERN replica using voms-proxy")
            return ami


        pyAMIEndPoint.setType("main")
        ami.certAuth()
        if ami.checkAuth():
            print ("... connecting to main server using voms-proxy")
            return ami


        if verbose:
            print ("WARNING voms-proxy authentication not valid. No access to AMI.")
        return None


    @classmethod
    def amiclient(cls):
        if cls._amiclient is None:
            from os import getenv
            from CoolRunQuery.utils.AtlRunQueryUtils import runsOnServer
            if runsOnServer():
                home = "/data/atrqadm/data/"
            else:
                home = getenv('HOME')
                if not home:
                    home = '~'
            conffilename = home + "/private/AMIConf.txt"
            cls._amiclient = cls.getAmiClient(conffilename)
            if cls._amiclient is None:
                print ("ERROR: voms-proxy authentication not valid and no AMI configuration file",conffilename,"supplied. No access to AMI!")
                cls._amiclient="No AMI"
        return cls._amiclient

    @classmethod
    def OpenAMIConnection(cls):
        try:
            from pyAMI.pyAMI import AMI
            amiclient = AMI()
            amiclient.readConfig("./AMIConf.txt")
            return amiclient
        except ImportError:
            print ('ERROR: could not load pyAMI')
            return None


    @classmethod
    def amiexec(cls, cmdList):
        try:
            result = cls.amiclient().execute(cmdList)
            return result.getDict()
        except Exception as ex:
            print ("AMI exception '",type(ex),"' occured")
            import traceback
            traceback.print_exc()
        return {}


    @classmethod
    def get_periods_for_run(cls, run):
        try:
            run = int(run)
        except ValueError:
            return []
        if run not in cls.store:
            try:
                print ('GetDataPeriodsForRun', '-runNumber=%i' % run)
                #result = cls.amiclient().execute(['GetDataPeriodsForRun', '-runNumber=%i' % run])
                result = cls.amiexec(['GetDataPeriodsForRun', '-runNumber=%i' % run])
                #cls.store[run] = sorted([ (int(e['periodLevel']),e['period'],e['project']) for e in result.getDict()['Element_Info'].values() ])
                cls.store[run] = sorted([ (int(e['periodLevel']),e['period'],e['project']) for e in result['Element_Info'].values() ])
            except Exception:
                print ("Exception")
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
            #result = cls.amiclient().execute(cmd)
            result = cls.amiexec(cmd)
            #rows = [ (e['period'], e['projectName']) for e in result.getDict()['Element_Info'].values() ]
            rows = [ (e['period'], e['projectName']) for e in result['Element_Info'].values() ]
            return sorted(rows)
        except Exception:
            import traceback
            traceback.printExc()
            return []


    @classmethod
    def get_all_periods(cls):
        if cls.all_periods is not None:
            return cls.all_periods
        cls.all_periods = []
        p = re.compile(r"(?P<period>(?P<periodletter>[A-Za-z]+)(?P<periodnumber>\d+)?)$")
        try:
            result = cls.get_periods(0, 0)
            for period, projectName in result:
                m = p.match(period)
                if not m:
                    continue
                year = int(projectName[4:6])
                period_letter = m.group('periodletter')
                period_number = int(m.group('periodnumber')) if m.group('periodnumber') else 0
                if len(period_letter)!=1:
                    pc = 0
                else:
                    pc = 10000*year + 100*(ord(period_letter.upper())-65) + period_number
                cls.all_periods += [ ((year, period, pc), projectName+".period" + period) ]
            cls.all_periods.sort()
        except Exception:
            import traceback
            traceback.print_exc()
            pass
        return cls.all_periods
    

    @classmethod
    def get_runs(cls, period, year):
        try:
            cmd = ['GetRunsForDataPeriod', '-period=%s' % period]
            if year>2000:
                cmd += [ '-projectName=data%02i%%' % (year-2000) ]
            #result = cls.amiclient().execute(cmd)
            result = cls.amiexec(cmd)
            #print ("amiCommand",' '.join(cmd)        )
            #r = sorted([ int(e['runNumber']) for e in result.getDict()['Element_Info'].values() ])
            r = sorted([ int(e['runNumber']) for e in result['Element_Info'].values() ])
            return r
        except (ValueError, KeyError):
            pass



# for testing purpose
if __name__ == "__main__":

    choice = 1
    while choice != 0:
        print ("\n1 - periods for run")
        print ("2 - runs for period (and year)")
        print ("3 - periods (by year and/or level)")
        print ("4 - all periods (different format)")
        print ("5 - test AMI authentication")
        print ("\n0 - exit\n")

        choice = input("  enter your choice: ")
        choice = int(choice) if choice.isdigit() else 0
        if choice==1:
            run = int(input("  run number: "))
            print (ARQ_AMI.get_periods_for_run( [run] ))
        elif choice==2:
            period = input("  period           : ")
            year   = input("  year <RET> = all : ")
            year   = int(year) if year.isdigit() else 0
            print (', '.join([str(x) for x in ARQ_AMI.get_runs(period, year)]))
        elif choice==3:
            year   = input("  year <RET> = all           : ")
            year   = int(year) if year.isdigit() else 0
            period = input("  period [1|2|3] <RET> = all : ")
            period = int(period) if period.isdigit() else 0
            print (ARQ_AMI.get_periods(year, period))
        elif choice==4:
            print (ARQ_AMI.get_all_periods())
        elif choice==5:
            ami = ARQ_AMI.amiclient()
            if ami!="No AMI":
                print ("Successfully connected to AMI")
            else:
                print ("Failed to connect to AMI")
