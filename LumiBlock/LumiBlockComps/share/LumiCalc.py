#!/bin/env python
# lumiCalc.py
# calculation of integrated lumi based on input list of LB and
# COOL trigger/lumi instance
# Richard Hawkings, May 2007
# Akira Shibata, June 2009
#
# syntax: lumiCalc.py {<options>} {<files>}
# do lumiCalc.py --help for instructions


import sys,getopt
from LumiBlockComps.LumiCalculator import coolLumiCalc,lumiResult,RLBRange

class lumiCalcDriver:

    def __init__(self):
        # defaults for optional parameters
        self.dbname='COOLONL_TRIGGER/COMP200'
        self.statusdbname='COOLOFL_GLOBAL/COMP200'
        self.readoracle=False
        self.runmin=0
        self.runmax=0
        self.lumimin=0
        self.readxml=False
        self.lumimax=(1 << 32)-2
        self.trigger=''
        self.level=0
        self.loglevel=1
        self.statusreq=""
        self.statustag=""
        # process command line options and switches
        try:
            longopts=['trigger=',"level=","database=","detstatus=","detstatustag=","detstatusdb=","r=","rs=","ru=","ls=","lu=","l=","readoracle","mc","xml","debug","quiet","help"]
            opts,args=getopt.getopt(sys.argv[1:],'',longopts)
        except getopt.GetoptError,e:
            print e
            self.usage()
            sys.exit(1)
        self.filelist=args
        self.procopts(opts)
        self.execute()

    def usage(self):
        print ""
        print "usage: LumiCalc.py {<options>} { file1.root { file2.root ... } }"
        print "Options are:"
        print "--trigger=<trigger name> or ALL: set trigger name"
        print "--level=1,2,3   : restrict trigger=ALL to given level"
        print "--database=<COOL databaseID>  : set COOL DB"
        print "--detstatusdb=<COOL databaseID> : set COOL DB for detector status"
        print "--detstatus=<status string> :make detector status cuts"
        print "--detstatustag=<tagval>: set tag for detector status information"
        print "--readoracle : Force queries to use Oracle replica"
        print "--mc : Use Monte Carlo database (OFLP200) for FDRs"
        print "--xml : Use XML input"
        print "--rs=<first run>"
        print "--ru=<last run> (inclusive)"
        print "--r=<run> (set limits for single run)"
        print "--ls=<first lumi block>"
        print "--lu=<last lumi block> (inclusive)"
        print "--l=<lumi block> (single lumiblock)"
        print "--quiet : produce minimum output"
        print "--debug : produce maximum/debug output"
        print "--help : display this help text"
        print
        print "Calculates and prints integrated luminosity for the specified trigger"
        print "using either the LB meta-data information on the specified files"
        print "or the range of runs/LBs specified with -r,-rs,-ls etc options"
        print "If no trigger is specified, print the list of available triggers for each run"
        

    def procopts(self,opts):
        "Process the command line parameters"
        for o,a in opts:
            if (o=='--trigger'): self.trigger=str(a)
            if (o=='--level'): self.level=int(a)
            if (o=='--database'): self.dbname=str(a)
            if (o=='--detstatusdb'): self.statusdbname=str(a)
            if (o=='--detstatus'): self.statusreq=str(a)
            if (o=='--detstatustag'): self.statustag=str(a)
            if (o=='--rs'): self.runmin=int(a)
            if (o=='--ru'): self.runmax=int(a)
            if (o=='--r'):
                self.runmin=int(a)
                self.runmax=int(a)
            if (o=='--ls'): self.lumimin=int(a)
            if (o=='--lu'): self.lumimax=int(a)
            if (o=='--l'):
                self.lumimin=int(a)
                self.lumimax=int(a)
            if (o=="--debug"): self.loglevel=2
            if (o=="--quiet"): self.loglevel=0
            if (o=="--readoracle"): self.readoracle=True
            if (o=="--xml"): self.readxml=True
            if (o=="--mc"):
                self.dbname=self.dbname.replace('COMP200','OFLP200')
                self.statusdbname=self.statusdbname.replace('COMP200','OFLP200')
            if (o=='--help'):
                self.usage()
                sys.exit(0)
        

    def execute(self):
        # setup the tool
        lumitool=coolLumiCalc(self.dbname,self.statusdbname,self.readoracle,self.loglevel,self.statusreq,self.statustag)
        # get the list of run/lumiblocks
        if len(self.filelist)==0:
            rlblist=[]
            for run in range(self.runmin,self.runmax+1):
                rlblist+=[RLBRange(run,self.lumimin,self.lumimax)]
        else:
            rlblist=[]
            for file in self.filelist:
                rlblist+=lumitool.rlbFromFile(file, self.readxml)
        if (self.trigger==''):
            # loop over all runs in LB, and print the trigger configuration
            runlist=[]
            for rlb in rlblist:
                if (rlb.run not in runlist): runlist+=[rlb.run]
            for run in runlist:
                print "Triggers defined for run",run,":"
                print lumitool.listTriggers(run,self.level)
        else:
            # process for all or a single trigger
            if (self.trigger.upper()=='ALL'):
                # have to loop over all LBs individually and look for defined
                # triggers, as the setup may change each run
                # accumulate dictionary of intL for each trigger
                lumibytrig={}
                for rlb in rlblist:
                    triggerlist=lumitool.listTriggers(rlb.run,self.level)
                    for trigger in triggerlist:
                        lumiresult=lumitool.calcFromList(trigger,[rlb])
                        if (lumiresult is not None):
                            if (trigger in lumibytrig.keys()):
                                lumibytrig[trigger]+=lumiresult
                            else:
                                lumibytrig[trigger]=lumiresult
                # now print accumulated results
                print "Trigger name            L1-acc   L2-acc   L3-acc  intL(nb^-1)     Xsec(nb)"
                # present in alphabetical order of triggers
                sortlist=lumibytrig.keys()
                sortlist.sort()
                for trigger in sortlist:
                    lumiresult=lumibytrig[trigger]
                    level=lumitool.triggerLevel(trigger,True)
                    if (lumiresult.intL>0):
                        xsec=[lumiresult.l1acc,lumiresult.l2acc,lumiresult.l3acc][level-1]/lumiresult.intL
                    else:
                        xsec=0.
                    print "%-21s %8i %8i %8i %12.2f %12.2f" % (trigger,lumiresult.l1acc,lumiresult.l2acc,lumiresult.l3acc,lumiresult.intL,xsec)
                    
            else:
                # single trigger
                lumiresult=lumitool.calcFromList(self.trigger,rlblist)
                print ">== Trigger  :",self.trigger
                print "IntL (nb^-1) : %12.2f" % lumiresult.intL
                print "L1/2/3 accept: %12i %8i %8i" % (lumiresult.l1acc,lumiresult.l2acc,lumiresult.l3acc)
                print "Livetime     : %12.4f" % lumiresult.livetime
                print "Good LBs     : %12i" % lumiresult.ngood
                print "BadStatus LBs: %12i" % lumiresult.nbadstat
        

# main code
if __name__=='__main__':
    lumiCalcDriver()


