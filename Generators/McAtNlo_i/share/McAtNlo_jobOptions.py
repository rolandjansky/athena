###############################################################
# Job options fragment which runs only with Generate_trf.py
#==============================================================

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

runnumber=-1
maxevents=-1
randomseed=-1
if hasattr(runArgs, 'runNumber'):
   runnumber=runArgs.runNumber
if hasattr(runArgs, 'maxEvents'):
   maxevents=runArgs.maxEvents
if hasattr(runArgs, "randomSeed"):
   randomseed=runArgs.randomSeed

#for every sample the corresponding production jobOptions defines the string, which has to be included in the name of input files.
#the input file names are checked by evgenConfig.inputfilecheck for the existence of the mandatory string
prefix='ttbar'  # user depended setting 
#rough efficiency of the Athena event filters corresponding to the sample being generated has to be specified
#0.5 is approximate filter efficiency in production of the 105200 sample
evgeneff=0.5  # user depended setting

if maxevents > -1 :
   maxevents=int(maxevents/evgeneff)
elif 'EvtMax' in locals()  and EvtMax>-1 :
   #e.g., Generate_trf.py -c "EvtMax=500" ...
   maxevents=int(EvtMax/evgeneff)
else:
   print 'Number of events to be generated was not determined. Retreat.'
   quit()

# debug
#print 'runnumber=',runnumber, 'maxevents=',maxevents, 'randomseed=',randomseed

#dictionary of MC@NLO parameters, which have to be changed to the provided values for generation of events
pardict={ 'ECM':'8000', 'HVQMASS':'172.5', 'TWIDTH':'1.32', 'WMASS':'80.399', 'WWIDTH':'2.085', 'ZMASS':'91.1876', 'ZWIDTH':'2.4952', 'BRTOPTOLEP':'0.108', 'BRTOPTOHAD':'0.338', 'BRWTOLEP':'0.108', 'BRWTOHAD':'0.338', 'IPROC':'-1706', 'IL1CODE':'0', 'IL2CODE':'0', 'TOPDECAY':'ALL', 'PDFSET':'10800', 'HERPDF':'EXTPDF', 'FPREFIX':prefix, 'EVPREFIX':prefix, 'EXEPREFIX':prefix, 'NEVENTS':'1000', 'RNDEVSEED':randomseed }  # user depended settings

import McAtNlo_i.McAtNlo_iUtils
McAtNlo_i.McAtNlo_iUtils.mcatnloversion='3.42'
McAtNlo_i.McAtNlo_iUtils.herwigversion='6.520.2'
McAtNlo_i.McAtNlo_iUtils.generateinput(runnumber, maxevents, 1, pardict)

#initialize the inputGeneratorFile option in order to link the on-the-fly generated input files to the default input files.
runArgs.inputGeneratorFile=prefix+'._00001'

#run example.
#remark: the empty argument inputGeneratorFile=NONE has to be included in the command string. do not omit it.
#
# Generate_trf.py  -t --omitvalidation=testEventMinMax preInclude=McAtNlo_jobOptions.py randomSeed=1 runNumber=105200 maxEvents=100 /
#                 jobConfig=MC12.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.py firstEvent=1 ecmEnergy=8000 outputEVNTFile=test.pool.root inputGeneratorFile=NONE


