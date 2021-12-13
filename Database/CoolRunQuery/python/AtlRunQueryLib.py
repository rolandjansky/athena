#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryLib.py
# Project: AtlRunQuery
# Purpose: Library with the actual query classes
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 13, 2008
# ----------------------------------------------------------------
#

from __future__ import with_statement, print_function

from CoolRunQuery.utils.AtlRunQueryTimer import timer

import re
import os
from time import time

from CoolRunQuery.utils.AtlRunQueryUtils              import coolDbConn, runsOnServer

from CoolRunQuery.selector.AtlRunQuerySelectorRuntime import RunTimeSelector
from CoolRunQuery.selector.AtlRunQuerySelectorBase    import Selector, DataKey

from CoolRunQuery.AtlRunQueryRun         import Run
from CoolRunQuery.AtlRunQueryQueryConfig import QC


# -----------------------------------------------------------------------------------------
# class: AtlRunQuery
# -----------------------------------------------------------------------------------------

class AtlRunQuery:

    def __init__(self, options, readoracle=False, loglevel=1, html="AUTO", origQuery="", datapath='data', parsedstring=""):

        with timer('total'):

            with timer('config'):
                self.config(options, readoracle, loglevel, html, origQuery, datapath, parsedstring)

            with timer('run all'):
                runlist = self.run()

            with timer('evaluate all'):
                (dic, dicsum, xmlhtmlstr, roothtmlstr) = self.evaluate(runlist)

            with timer('output all'):
                self.output(runlist, dic, dicsum, xmlhtmlstr, roothtmlstr)

            with timer('finalize'):
                self.finalize()

        from CoolRunQuery.utils.AtlRunQueryTimer import TimerStats as TS
        TS.printTimeSummary()
        TS.printTimeFlat()



    def config(self, options, readoracle, loglevel, html, origQuery, datapath, parsedstring):

        self.cmdlineOptions = options
        self.origQuery = origQuery
        QC.datapath = datapath
        self.parsedstring = parsedstring
        self.maxNumOfRuns = -1    # in case a maximum number of to be selected runs is given
        self.makeSummary = 'summary' in options.show  # in case a summary shall be printed
        self.makeDQSummary = 'dqsummary' in options.show  # in case a DQ efficiency summary shall be printed
        self.makeDQPlots = 'dqplots' in options.show  # in case only DQ plots shall be printed 
        self.makeDQeff = 'dqeff' in options.show  # in case a DQ efficiency summary shall be printed
        self.xmlFileName  = 'MyLBCollection.xml'
        self.xmlFileLabel = 'MyLBCollection'
        self.querystart = time()
        self.selectionOutput = []

        # ensure the output directory exists
        if QC.datapath=='':
            QC.datapath='.'
        else:
            try:
                os.makedirs(QC.datapath, exist_ok = True)
                print("Using output directory: %s" % QC.datapath)
            except (PermissionError, FileExistsError) as err:
                print("Could not create data output directory: %s" % QC.datapath)
                print("Reason: %s" % err)
                print("Will write to local directory")
                QC.datapath = "."

        # if no show argument is specified, we use the minimal version
        if not self.cmdlineOptions.show:
            self.cmdlineOptions.show = ['run','time']

        if self.cmdlineOptions.xmlfile is not None:
            # format 'file.xml:runlist' or 'runlist:file.xml'
            str1,str2 = self.cmdlineOptions.xmlfile.strip().split(':',1)
            if   '.xml' in str1:
                self.xmlFileName = str1
                if str2 != '':
                    self.xmlFileLabel = str2
            elif '.xml' in str2:
                self.xmlFileName = str2
                if str1 != '':
                    self.xmlFileLabel = str1            
                
        if self.cmdlineOptions.html:
            self.html = self.cmdlineOptions.html
        else:
            if html=="AUTO":
                self.html = runsOnServer()
            elif html=="YES":
                self.html = True
            elif html=="NO":
                self.html = False
            else:
                raise RuntimeError ("Unknown argument for option 'html': %s" % html)
        Run.writehtml = self.html

        if self.cmdlineOptions.prodgrl is not None : # happens only if --nogrl is specified
            self.prodgrl = self.cmdlineOptions.prodgrl
        elif self.cmdlineOptions.xmlfile is not None :
            self.prodgrl = True # if an xml file is specified it will be produced
        else:
            self.prodgrl = not runsOnServer()

        if self.cmdlineOptions.dictroot is not None : # happens if --root or --noroot is specified
            self.dictroot = self.cmdlineOptions.dictroot
        else:
            self.dictroot = not runsOnServer()
            

        if self.cmdlineOptions.database and self.cmdlineOptions.database.upper()=='MC':
            Selector.setCondDBMC()

        if self.cmdlineOptions.condtag:
            Selector.condtag = self.cmdlineOptions.condtag

        QC.localtime = (self.cmdlineOptions.utc is None)
        QC.settimezone()

        # check which runs currently participate in prompt calibration
        # we need this for the web display only
        if self.html:
            nemo_dir = '/afs/cern.ch/user/a/atlcond/scratch0/nemo/prod/web/'
            try:
                f = open( nemo_dir + 'calibruns.txt' )
                for line in f:
                    try:
                        if line:
                            Run.PromptCalibRuns.append( int(line.strip()) )
                    except ValueError:
                        pass
                # read list of NEMO tasks
                # do we need this for each query, or just for the web??
                fname = os.listdir( nemo_dir + 'tasks' )
                i = 0
                for taskname in fname:
                    m = re.match(r'.*(task_(\d+)_\d+.txt)',taskname)
                    if m:
                        i+=1
                        fname, runnr = m.groups()
                        Run.NemoTasks[int(runnr)] = fname
            except IOError as err:
                print (err)
                print ("Because of this I can't mark runs that are currently in the calibration loop")
        
        #DQ summary relative to GRL
        if 'cosmics' in options.show:
            self.dqsumgrl = "PHYS_Cosmics_AllGood"
        elif 'heavyions' in options.show:
            self.dqsumgrl = "PHYS_HeavyIonP_All_Good"
        elif self.cmdlineOptions.dqsumgrl:
            self.dqsumgrl = self.cmdlineOptions.dqsumgrl
        else:
            self.dqsumgrl = 'PHYS_StandardGRL_All_Good_25ns'
            
        #defect and logic tag of defect database
        if self.cmdlineOptions.defecttag and self.cmdlineOptions.logictag:
            self.dbbtag = (self.cmdlineOptions.defecttag, self.cmdlineOptions.logictag)
        else:
            self.dbbtag = ("HEAD", "HEAD") 


    def run(self):
        # is last run still open ?
        from CoolRunQuery.AtlRunQuerySFO import GetSFO_lastNruns
        sfoconnection = coolDbConn.GetSFODBConnection()
        sfocursor     = sfoconnection.cursor()
        retlist       = GetSFO_lastNruns( sfocursor, 1 )
        if 'OPENED' in retlist[0][1]:
            Run.runnropen = retlist[0][0]

        if self.cmdlineOptions.runlist is not None:
            runlist = self.cmdlineOptions.runlist
            if any('last' in x for x in self.cmdlineOptions.runlist):
                for idx in range(len(runlist)):
                    rrange = runlist[idx]
                    if 'last' not in rrange:
                        continue
                    rrange = rrange.replace('last','')
                    self.maxNumOfRuns = int(rrange)
                    # list of last n runs run numbers, where at least one stream is not calibration
                    retlist    = GetSFO_lastNruns( sfocursor, 1.1*self.maxNumOfRuns )
                    runlist[idx] = '%i+' % (retlist[-1][0])

        elif self.cmdlineOptions.timelist is not None:
            # translate time into run-range
            if 'last' in self.cmdlineOptions.timelist[0]:
                # contains 'last'
                from CoolRunQuery.utils.AtlRunQueryUtils import get_runs_last_dt
                rlist = get_runs_last_dt(self.cmdlineOptions.timelist[0])

                runlist = ['%i+' % rlist[-1]] if (len(rlist)>0) else ['99999999+']

            else:
                # translate time into run-range
                from CoolRunQuery.utils.AtlRunQueryUtils import timeStringToSecondsUTC,secondsToTimeStringUTC,get_run_range2,GetTimeRanges
                timelist = ','.join(self.cmdlineOptions.timelist)
                timeranges,timerangesHR = GetTimeRanges(timelist, intRepFnc=timeStringToSecondsUTC, maxval=time())
                timerangesAsString = [ list(map(secondsToTimeStringUTC, tr)) for tr in timeranges ]
                runranges = [ ( "%s-%s" % get_run_range2(tr[0],tr[1]) ) for tr in timerangesAsString]
                runlist = [','.join(runranges)]


        # the run selector
        rtSel = RunTimeSelector(name = 'runtime', runlist = runlist)

        # find the begin and end of each interesting run
        runlist = rtSel.select()

        # if Selector.condDB()=="CONDBR2":
        #     self.cmdlineOptions.show = [x for x in self.cmdlineOptions.show if x not in ["lhc","olclumi","olcfillparams","olclbdata"]]
        #     self.cmdlineOptions.partition=None
        #     self.cmdlineOptions.projecttag=None
        #     print ("Pre-run2 selection removed partition and projecttag from querying")
        #     print ("Pre-run2 selection removed lhc, olc from showing. Modified show list: %r" % self.cmdlineOptions.show)
            
        self.selectionOutput += ["%s" % rtSel]


        from CoolRunQuery.AtlRunQuerySelectorWorker import SelectorWorker

        # create all selectors that are actively select
        SelectorWorker.parseSelectorOptions(self.cmdlineOptions)

        # parse show option:
        SelectorWorker.parseShowOption(self.cmdlineOptions)

        # call setApply for all selectors that actively select
        SelectorWorker.setApplySelection()

        # call setShowOutput for all selectors that show
        SelectorWorker.setShowOutput()

        SelectorWorker.getRetrieveSelector('trigkey','TrigKeySelector')

        # Selectors can implement an initialize function which runs after any constructor and setShow 
        for s in SelectorWorker.getOrderedSelectorList():
            s = s.selector
            if hasattr(s, 'initialize'):
                with timer("initializing selector '%s'" % s.name):
                    s.verbose = True
                    s.initialize()

        # apply the selectors to initial run list
        for s in SelectorWorker.selectors():
            with timer("run selector '%s'" % s.name):
                s.verbose = True
                runlist = s.select(runlist)
                self.selectionOutput += ["%s" % s.__str__()]
            with timer("run AfterQuery for selector '%s'" % s.name):
                s.runAfterQuery(runlist)

        # reverse list for presentation: newest run on top
        runlist.reverse()

        # if "last" option used, the runlist has to be truncated
        if self.maxNumOfRuns > 0:
            print ('SELOUT Selecting the %i most recent runs' % self.maxNumOfRuns)
            runlist = runlist[0:self.maxNumOfRuns]
            self.selectionOutput += ['SELOUT Selecting the %i most recent runs' % self.maxNumOfRuns]
            
        return runlist


    def evaluate(self, runlist):

        # provide pickled dictionary of query result
        with timer('CreateResultDict'):
            from CoolRunQuery.output.AtlRunQuerySave import CreateResultDict
            dic, dicsum = CreateResultDict( runlist )


        # create XML file (and return pretty html output for print)
        if self.prodgrl:
            with timer('CreateXMLFile'):
                print ("Producing XML file")
                from CoolRunQuery.output.AtlRunQueryXML import CreateXMLFile
                from CoolRunQuery.AtlRunQueryVersion import SvnVersion
                xmlhtmlstr = CreateXMLFile( runlist, self.cmdlineOptions, self.origQuery, QC.datapath,
                                            self.xmlFileName, self.xmlFileLabel, SvnVersion )
        else:
            xmlhtmlstr = None
            print ("Creation of GRL disabled")

        if self.dictroot and len(runlist) > 0:
            with timer('CreateRootFile'):
                # create root file from dictionary - and make plots
                from CoolRunQuery.output.AtlRunQueryRoot import CreateRootFile
                rootfilename, roothtmlstr = CreateRootFile( dic )
        else:
            roothtmlstr = None
            print ("Creation of root file disabled")

        return (dic, dicsum, xmlhtmlstr, roothtmlstr)
        

    def output(self, runlist, dic, dicsum, xmlhtmlstr, roothtmlstr):

        if self.html:

            with timer('GetOKS link info'):
                from CoolRunQuery.AtlRunQuerySFO import SetOKSLinks
                SetOKSLinks(runlist)

            # create web page
            from CoolRunQuery.html.AtlRunQueryHTML import ResultPageMaker
            #try:
            pageinfo = { 'datapath'      : QC.datapath,
                         'origQuery'     : self.origQuery,
                         'fullQuery'     : self.parsedstring,
                         'runlist'       : runlist,
                         'dic'           : dic,
                         'dicsum'        : dicsum,
                         'makeSummary'   : self.makeSummary,
                         'makeDQeff'     : self.makeDQeff,
                         'makeDQSummary' : self.makeDQSummary,
                         'makeDQPlots'   : self.makeDQPlots,
                         'dqsumgrl'      : self.dqsumgrl,
                         'dbbtag'        : self.dbbtag,
                         'roothtmlstr'   : roothtmlstr,
                         'xmlfilename'   : self.xmlFileName,
                         'xmlhtmlstr'    : xmlhtmlstr,
                         'querytime'     : time() - self.querystart,
                         'selout'        : self.selectionOutput,
                         }
            with timer("run ResultPageMaker makePage"):
                ResultPageMaker.makePage(pageinfo)
            #except ImportError, ie:
            #    print ("Can't import pagemaker, no web page made",ie)

        else:
            print ('---------------------------------------------------------------------')
            print (Run.header())
            runlist.reverse()
            for r in runlist:
                print (r)

            from CoolRunQuery.utils.AtlRunQueryUtils import addKommaToNumber, filesize
            print ('Summary:')
            for data_key, summary in dicsum.items():
                if data_key.Type==DataKey.STREAM:
                    key = data_key.ResultKey.replace('STR:','')
                    val = '%s (%s)' % ( addKommaToNumber(summary[0]), filesize(summary[1]) )
                else:
                    key = data_key.ResultKey
                    val = addKommaToNumber(summary)
                print ('%20s : %s' % (key,val))
            duration = time() - self.querystart
            print ("%20s : %g sec" % ('Total execution time',duration))
            print ('---------------------------------------------------------------------')
        
    def finalize(self):
        coolDbConn.CloseAll()
            
if __name__ == '__main__':
    from CoolRunQuery.AtlRunQueryOptions import AtlRunQueryOptions 
    (options, args) = AtlRunQueryOptions().parse()    
    rsq = AtlRunQuery(options)

