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
        240308:1410308000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=80)
        240314:1410314000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=140)
        240320:1410320000, ##MC15c RUN-4 (ITk ExtBrl4 + sFCal (small LAr gap), mu=200)
        240400:1410400000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=0)
        240406:1410406000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=60)
        240408:1410408000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=80)
        240414:1410414000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=140)
        240420:1410420000, ##MC15c RUN-4 (ITk InclBrl4 + sFCal (small LAr gap), mu=200)
        240500:1410500000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + sFCal (small LAr gap), mu=0)
        240506:1410506000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + sFCal (small LAr gap), mu=60)
        240508:1410508000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + sFCal (small LAr gap), mu=80)
        240514:1410514000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + sFCal (small LAr gap), mu=140)
        240520:1410520000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + sFCal (small LAr gap), mu=200)
        240600:1410600000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + sFCal (small LAr gap), mu=0)
        240606:1410606000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + sFCal (small LAr gap), mu=60)
        240608:1410608000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + sFCal (small LAr gap), mu=80)
        240614:1410614000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + sFCal (small LAr gap), mu=140)
        240620:1410620000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + sFCal (small LAr gap), mu=200)
        240700:1410700000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + FCal, mu=0)
        240708:1410708000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + FCal, mu=80)
        240714:1410714000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + FCal, mu=140)
        240720:1410720000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.5) + FCal, mu=200)
        240800:1410800000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + FCal, mu=0)
        240808:1410808000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + FCal, mu=80)
        240814:1410814000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + FCal, mu=140)
        240820:1410820000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.5) + FCal, mu=200)
        240900:1410900000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.6) + FCal, mu=0)
        240906:1410906000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.6) + FCal, mu=60)
        240908:1410908000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.6) + FCal, mu=80)
        240914:1410914000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.6) + FCal, mu=140)
        240920:1410920000, ##MC15c RUN-4 (ITk ExtBrl4 (Step 1.6) + FCal, mu=200)
        241000:1411000000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.6) + FCal, mu=0)
        241006:1411006000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.6) + FCal, mu=60)
        241008:1411008000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.6) + FCal, mu=80)
        241014:1411014000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.6) + FCal, mu=140)
        241020:1411020000, ##MC15c RUN-4 (ITk InclBrl4 (Step 1.6) + FCal, mu=200)
        241100:1411100000, ##MC15c RUN-4 (ITk ExtBrl4_33mm (Step 1.6) + FCal, mu=0)
        241106:1411106000, ##MC15c RUN-4 (ITk ExtBrl4_33mm (Step 1.6) + FCal, mu=60)
        241108:1411108000, ##MC15c RUN-4 (ITk ExtBrl4_33mm (Step 1.6) + FCal, mu=80)
        241114:1411114000, ##MC15c RUN-4 (ITk ExtBrl4_33mm (Step 1.6) + FCal, mu=140)
        241120:1411120000, ##MC15c RUN-4 (ITk ExtBrl4_33mm (Step 1.6) + FCal, mu=200)
        241200:1411200000, ##MC15c RUN-4 (ITk IExtBrl4 (Step 1.6) + FCal, mu=0)
        241206:1411206000, ##MC15c RUN-4 (ITk IExtBrl4 (Step 1.6) + FCal, mu=60)
        241208:1411208000, ##MC15c RUN-4 (ITk IExtBrl4 (Step 1.6) + FCal, mu=80)
        241214:1411214000, ##MC15c RUN-4 (ITk IExtBrl4 (Step 1.6) + FCal, mu=140)
        241220:1411220000, ##MC15c RUN-4 (ITk IExtBrl4 (Step 1.6) + FCal, mu=200)
        241300:1411300000, ##MC15c RUN-4 (ITk InclBrl4_OptRing (Step 1.8) + FCal, mu=0)
        241306:1411306000, ##MC15c RUN-4 (ITk InclBrl4_OptRing (Step 1.8) + FCal, mu=60)
        241308:1411308000, ##MC15c RUN-4 (ITk InclBrl4_OptRing (Step 1.8) + FCal, mu=80)
        241314:1411314000, ##MC15c RUN-4 (ITk InclBrl4_OptRing (Step 1.8) + FCal, mu=140)
        241320:1411320000, ##MC15c RUN-4 (ITk InclBrl4_OptRing (Step 1.8) + FCal, mu=200)
        241400:1411400000, ##MC15c RUN-4 (ITk LoI-VF + FCal, mu=0)
        241406:1411406000, ##MC15c RUN-4 (ITk LoI-VF + FCal, mu=60)
        241408:1411408000, ##MC15c RUN-4 (ITk LoI-VF + FCal, mu=80)
        241414:1411414000, ##MC15c RUN-4 (ITk LoI-VF + FCal, mu=140)
        241420:1411420000, ##MC15c RUN-4 (ITk LoI-VF + FCal, mu=200)
        241500:1411500000, ##MC15c RUN-4 (ITk ExtBrl32 + FCal, mu=0)
        241506:1411506000, ##MC15c RUN-4 (ITk ExtBrl32 + FCal, mu=60)
        241508:1411508000, ##MC15c RUN-4 (ITk ExtBrl32 + FCal, mu=80)
        241514:1411514000, ##MC15c RUN-4 (ITk ExtBrl32 + FCal, mu=140)
        241520:1411520000, ##MC15c RUN-4 (ITk ExtBrl32 + FCal, mu=200)
        241600:1411600000, ##MC15c RUN-4 (ITk Inclined_Quads + FCal, mu=0)
        241606:1411606000, ##MC15c RUN-4 (ITk Inclined_Quads + FCal, mu=60)
        241608:1411608000, ##MC15c RUN-4 (ITk Inclined_Quads + FCal, mu=80)
        241614:1411614000, ##MC15c RUN-4 (ITk Inclined_Quads + FCal, mu=140)
        241620:1411620000, ##MC15c RUN-4 (ITk Inclined_Quads + FCal, mu=200)
        241700:1411700000, ##MC15c RUN-4 (ITk Inclined_Alternative + FCal, mu=0)
        241706:1411706000, ##MC15c RUN-4 (ITk Inclined_Alternative + FCal, mu=60)
        241708:1411708000, ##MC15c RUN-4 (ITk Inclined_Alternative + FCal, mu=80)
        241714:1411714000, ##MC15c RUN-4 (ITk Inclined_Alternative + FCal, mu=140)
        241720:1411720000, ##MC15c RUN-4 (ITk Inclined_Alternative + FCal, mu=200)
        241800:1411800000, ##MC15c RUN-4 (ITk Inclined_LightBarrel + FCal, mu=0)
        241806:1411806000, ##MC15c RUN-4 (ITk Inclined_LightBarrel + FCal, mu=60)
        241808:1411808000, ##MC15c RUN-4 (ITk Inclined_LightBarrel + FCal, mu=80)
        241814:1411814000, ##MC15c RUN-4 (ITk Inclined_LightBarrel + FCal, mu=140)
        241820:1411820000, ##MC15c RUN-4 (ITk Inclined_LightBarrel + FCal, mu=200)
        267599:1434123751, ##MC15c pp (low mu+LHCf)
        271516:1436762129, ##MC15c pp (50 ns bunch spacing, nominal mu)
        282420:1445073756, ##MC15c pp (beta*=90 m, ALFA)
        284500:1446539185, ##MC15c/MC16 pp (25 ns bunch spacing, nominal mu, 2015/2016)
        290000:1450000000, ##MC15c/MC16 pPb (2016)
        295000:1475000000, ##MC15c/MC16 pPb at 8.16 TeV (2016)
        300000:1500000000, ##MC16 pp (25 ns bunch spacing, nominal mu, 2017)
        305000:1525000000, ##MC16 pp (5 TeV 2017)
        309000:1540000000, ##MC16 XeXe (2017)
        310000:1550000000, ##MC16 pp (25 ns bunch spacing, nominal mu, 2018)
        311000:1551000000, ##MC16 pp (25 ns bunch spacing, low mu, 2018)
        312000:1552000000, ##MC16 pp (900 GeV high-beta ALFA, 2018)
        313000:1553000000  ##MC16 PbPb (2018)


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
