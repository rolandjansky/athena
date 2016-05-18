# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
logRunDMCFlags = logging.getLogger('RunDMCFlags')
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class RunToTimestampDict(JobProperty):
    """This is used to hold a dictionary of the form
    {152166:1269948352889940910, ...} to allow the
    timestamp to be determined from the run.
    """
    statusOn=True
    allowedTypes=['dict']
    StoredValue={
        152777:1270866809, ##2010 7 TeV p-p for period A (very low mu)
        154471:1272831478, ##2010 900 GeV p-p
        155697:1274722685, ##2010 7 TeV p-p for period C (mu~0.2)
        162623:1282673387, ##2010 7 TeV p-p for period F (36 single bunches, mu~2)
        167576:1287909620, ##2010 7 TeV p-p (150ns bunch trains)
        170002:1291063245, ##2010 Heavy Ion run
        178211:1301070988, ##2011 2.76 TeV
        180164:1303554669, ##2011 7 TeV p-p (B-D)
        183003:1306973316, ##2011 7 TeV p-p (E-H)
        186169:1311379706, ##2011 7 TeV p-p (I-K)
        189751:1316868299, ##2011 7 TeV p-p (L-M)
        193662:1322075751, ##2011 Heavy Ion run
        195847:1328040250, ##Upgrade samples from MC12
        210184:1347470747, ##2012 Heavy Ion - proton run
        212272:1349603811, ##2012 8 TeV p-p
        219114:1360457560, ##2013 Heavy Ion - proton run
        219305:1360689056, ##2013 2.76 TeV p-p
        222222:1376009561, ##DC14 (mu=30) 25ns, old R1 geo tag
        222250:1376075781, ##DC14 (mu=30) 50ns, old R1 geo tag
        222500:1376646281, ##MC15a 50ns, new R2 geo tag (mu=0)
        222506:1376656281, ##MC15a PbPb
        222508:1376661281, ##MC15a pp at 5.02 TeV
        222510:1376669101, ##MC15a 50ns, new R2 geo tag (nominal mu)
        222525:1376703331, ##MC15a 25ns, new R2 geo tag (nominal mu)
        222526:1377093769, ##MC15b 25ns, new R2 geo tag (nominal mu)
        226000:1394188626, ##MC15c PbPb
        228000:1398342135, ##MC15c pp at 5.02 TeV (25 ns, mu=1.6)
        240000:1410000000, ##MC15c RUN-4 (ITk LoI+FCal layout, mu=0)
        240008:1410008000, ##MC15c RUN-4 (ITk LoI+FCal layout, mu=80)
        240014:1410014000, ##MC15c RUN-4 (ITk LoI+FCal layout, mu=140)
        240020:1410020000, ##MC15c RUN-4 (ITk LoI+FCal layout, mu=200)
        240100:1410100000, ##MC15c RUN-4 (ITk LoI+sFCal (small LAr gap), mu=0)
        240108:1410108000, ##MC15c RUN-4 (ITk LoI+sFCal (small LAr gap), mu=80)
        240114:1410114000, ##MC15c RUN-4 (ITk LoI+sFCal (small LAr gap), mu=140)
        240120:1410120000, ##MC15c RUN-4 (ITk LoI+sFCal (small LAr gap), mu=200)
        240200:1410200000, ##MC15c RUN-4 (ITk LoI+sFCal (large/original LAr gap), mu=0)
        240208:1410208000, ##MC15c RUN-4 (ITk LoI+sFCal (large/original LAr gap), mu=80)
        240214:1410214000, ##MC15c RUN-4 (ITk LoI+sFCal (large/original LAr gap), mu=140)
        240220:1410220000, ##MC15c RUN-4 (ITk LoI+sFCal (large/original LAr gap), mu=200)
        240300:1410300000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=0)
        240306:1410306000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=60)
        240314:1410314000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=140)
        240320:1410320000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=200)
        240400:1410400000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=0)
        240406:1410406000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=60)
        240414:1410414000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=140)
        240420:1410420000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=200)
        267599:1434123751, ##MC15c pp (low mu+LHCf)
        271516:1436762129, ##MC15c pp (50 ns bunch spacing, nominal mu)
        282420:1445073756, ##MC15c pp (beta*=90 m, ALFA)
        284500:1446539185  ##MC15c pp (25 ns bunch spacing, nominal mu)

        }
    def addEntry( self, run, timestamp, force=False):
        if not type(run) == int:
            raise ValueError( 'run is not the expected type (int) for the key of RunToTimestampDict!' )
        if not type(timestamp) == int:
            raise ValueError( 'timestamp is not the expected type (int) for the key of RunToTimestampDict!' )
        run2tslist = self.get_Value()
        logRunDMCFlags.info("Attempting to add the following pair to RunToTimestampDict: %i:%i.", run, timestamp)
        if run2tslist.has_key(run):
            logRunDMCFlags.warning("RunToTimestampDict already has an entry for run %i with timestamp %i", run, run2tslist[run])
            if force:
                logRunDMCFlags.warning("Replacing existing entry in RunToTimestampDict!")
            else:
                logRunDMCFlags.warning("Not altering RunToTimestampDict!")
                return
        run2tslist[run] = timestamp
        return
    def getTimestampForRun(self, run):
        return self.get_Value()[run]
    def Print(self):
        """Print the contents of the RunToTimestampList
        """
        logRunDMCFlags.info( 'RunToTimestampList:' )
        run2tslist = self.get_Value()
        logRunDMCFlags.info('run\t\ttimestamp')
        for d in run2tslist.keys(): logRunDMCFlags.info('%i\t%i', d, run2tslist[d])
        return

## Definition and registration of the simulation flag container
class RunDMCFlags(JobPropertyContainer):
    """
    Container for the RunDMCFlags

    This JobPropertyContainer is intended to hold a dictionary to
    allow the correct timestamp to be inferred from the DataRunNumber
    specified on the transform command-line. This is intended to be a
    temporary fix for https://its.cern.ch/jira/browse/ATCONDDB-21
    A longer term solution will eventually be put in place.

    """

## Register and populate the RunDMCFlags container
jobproperties.add_Container(RunDMCFlags)
for jpname in dir():
    jp = eval(jpname)
    import inspect
    if inspect.isclass(jp):
        if issubclass(jp, JobProperty) and jp is not JobProperty:
            logRunDMCFlags.debug("Adding RunDMCFlag '%s' to RunDMCFlags container" % jpname)
            jobproperties.RunDMCFlags.add_JobProperty(jp)


## Short-cut alias (standard naming)
runDMCFlags = jobproperties.RunDMCFlags
