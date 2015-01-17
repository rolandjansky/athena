# Author: Karsten Koeneke
# CERN, February 2009


#============================= This algorithm =============================
from D2PDMaker.D2PDMakerConf import CheckD2PD
topSequence += CheckD2PD( "MyCheckD2PD",
                          OutputLevel = DEBUG,
                          #CompositeParticleContainer3 = "MyLooseZBosonOffShellCollection",
                          #CompositeParticleContainer2 = "MyLooseZBosonCollection",
                          CompositeParticleContainer1 = "MyZeeLooseZeeBosonCollection" )
#==========================================================================



##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
rootFileName = 'checkD2PD.root'
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='"+rootFileName+"' OPT='RECREATE'"]
#from AnalysisTools.AnalysisToolsConf import AANTupleStream ## deprecated?
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamDPD","Stream1" ]
AANTupleStream.OutputName = rootFileName
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING
