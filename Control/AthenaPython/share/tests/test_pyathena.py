##
import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.SystemOfUnits as Units
import AthenaCommon.Constants as Lvl

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

from AthenaPython.tests.PyTestsLib import MyAlg, MySvc, MyTool, MyNameAud
job += MyAlg( "alg1", eta = 2.5 )
job.alg1.pt = 42.

job += MyAlg( name="alg2", eta = 5.1, pt = 20.*Units.GeV )
job += MyAlg( "alg3", eta = 5.1, pt = 20.*Units.GeV )
job.alg3 += MyTool( "mytool", counter = 50, parent="alg3" )

job += MyAlg( eta = 5.2,
              px  = 20.*Units.GeV,
              mytool = MyTool( "mytool", counter = 30,
                               parent="MyAlg",
                               OutputLevel = Lvl.VERBOSE ),
              )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += MySvc()

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 2

theApp.CreateSvc += [ svcMgr.MySvc.getFullName() ]
theApp.AuditAlgorithms = True
theApp.AuditServices   = True
theApp.AuditTools      = True
svcMgr.AuditorSvc += MyNameAud(name="MyNameAuditor", OutputLevel = Lvl.VERBOSE)

## now, add a sequencer: it will only execute seqalg1 and 2 and never 3
job += AthSequencer("MySeq")
job.MySeq += MyAlg( "seqalg1", filterPassed=True )
job.MySeq += MyAlg( "seqalg2", filterPassed=False )
job.MySeq += MyAlg( "seqalg3", filterPassed=True )

## another sequencer
job += AthSequencer("MultSeq")
job.MultSeq += AthSequencer("SubSeq1")
job.MultSeq.SubSeq1 += MyAlg("sub1alg1")
job.MultSeq.SubSeq1 += MyAlg("sub1alg2",filterPassed=False)
job.MultSeq.SubSeq1 += MyAlg("sub1alg3")

job.MultSeq += AthSequencer("SubSeq2")
job.MultSeq.SubSeq2 += MyAlg("sub2alg1")
job.MultSeq.SubSeq2 += MyAlg("sub2alg2")
job.MultSeq.SubSeq2 += MyAlg("sub2alg3")
##
