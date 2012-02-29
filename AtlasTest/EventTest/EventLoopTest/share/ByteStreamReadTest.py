#**************************************************************
#
# jopOptions file for reading ByteStream 
#
#==============================================================
# Input 
#include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
#include( "ByteStreamCnvSvc/ByteStreamSelector_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

# if you need DetDescr. 
# #include "TDRDetDescrAthenaRoot/TDRDetDescrAthenaRoot_jobOptions.txt"
# Specify input 
# Directory 
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

# specify Dir, Prefix,Run 
#ByteStreamInputSvc.InputDirectory =["/castor/cern.ch/atlas/P1commisioning/calo/cosmic/"]
# Prefix 
#ByteStreamInputSvc.FilePrefix  = ["daq_EB-1_Cosmic"]  
# Run number 
#ByteStreamInputSvc.RunNumber = [8035] 

# alternatively specify th full file name
ByteStreamInputSvc.FullFileName=["/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data"]
# ByteStreamInputSvc.FullFileName=["/castor/cern.ch/atlas/P1commisioning/phaseIII/daq_SFI-1_m3_combined_0011000_file01.data"]
#ByteStreamInputSvc.FullFileName=["/castor/cern.ch/user/i/idcosmic/real_data_endcap/combined/daq_EB_RCD_0007870_file17.data"]
# ByteStreamInputSvc.FullFileName=["/tmp/hma/daq_SFI-1_m3_combined_0011000_file01.data"]
#ByteStreamInputSvc.FullFileName=["/afs/cern.ch/user/b/battagli/public/Production/daq.NoTag.1183533026...LB0000.SFO-1._0001.data"]
#ByteStreamInputSvc.FullFileName=["/afs/cern.ch/user/b/battagli/public/data/daq.NoTag.540697965.No.Streaming.LB0020.SFO-1._0001.data"]
#ByteStreamInputSvc.FullFileName=["/afs/cern.ch/user/b/battagli/public/data/daq.NoTag.540697965.Physics.Electron.LB0020.SFO-1._0001.data"]


ByteStreamInputSvc.OutputLevel = DEBUG
ByteStreamInputSvc.MaxBadEvents = 20
# ByteStreamInputSvc.DumpFlag = True

MessageSvc.OutputLevel = DEBUG
theApp.EvtMax = 1000
