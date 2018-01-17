#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#File name: isf_sim.py
#Author:Bo Liu <boliu@cern.ch>

import os,sys
from optparse import OptionParser
###Options from command line
parser=OptionParser()

parser.add_option("--jobtype",dest="jobtype",help="Option for Job type,\n alternative:ATLFASTII(default),Simu,Merge,DigiMRec,ISF,ISF_sim",default="ATLFASTII")
parser.add_option("--eventtype",dest="eventtype",help="option for event type,\n alternative:ttbar(default),electron,muon,pion,minbias",default="ttbar")

parser.add_option("--EvtMax",dest="EvtMax",help="options for Max Event Number(default:5)",default=5)
parser.add_option("--skipEvents",dest="skipEvents",help="options for skip Event(default:0)",default=0)
parser.add_option("--Histfile",dest="Histfile",help="options for Monitoring hist file",default="HIST.root")
(options,args)=parser.parse_args()
jobtype=options.jobtype
eventtype=options.eventtype
EvtMax=int(options.EvtMax)
skipEvents=int(options.skipEvents)
Histfile=options.Histfile
import random
randomSeed=random.randint(1,100000000)
randomSeed1=random.randint(1,100000000)
randomSeed2=random.randint(1,100000000)
if jobtype.find("ISF")!=-1:
   Hitsfile_digi="Hits.pool.root"
else:
   Hitsfile_digi="Merge.pool.root"

pwd=os.getcwd()



###definition of command###
def setcommand(commandtype):
   if commandtype=="Simu":
      Simu_command='''AtlasG4_trf.py conditionsTag=OFLCOND-MC12-SIM-00 geometryVersion=ATLAS-GEO-20-00-01 randomSeed=%d maxEvents=%d inputEvgenFile=%s outputHitsFile=Hits.pool.root physicsList=QGSP_BERT preInclude=SimulationJobOptions/preInclude.MC12_AtlfastII_Hits.py''' % (randomSeed,EvtMax,eventfile)
      return Simu_command
   elif commandtype=="Merge":
      Merge_command='''Merging_trf.py inputHitsFile=%s outputHitsFile=Merge.pool.root maxEvents=%d skipEvents=%d geometryVersion=ATLAS-GEO-20-00-01 conditionsTag=OFLCOND-MC12-SIM-00 inputLogsFile=NONE preInclude=FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py,FastCaloSimHit/preInclude.AF2Hit.py postInclude=FastCaloSimHit/postInclude.AF2FilterHitItems.py,FastSimulationJobTransforms/jobConfig.FastCaloSim_ID_cuts.py,FastSimulationJobTransforms/jobConfig.egamma_lateral_shape_tuning.config19.py,G4AtlasTests/postInclude.DCubeTest.py postExec='from AthenaCommon.AppMgr import ServiceMgr as svcMgr;svcMgr.MessageSvc.setError+=["AtDSFMTGenSvc"]' '''% (Hitsfile,EvtMax,skipEvents,)
      return Merge_command
   elif commandtype=="DigiMRec":
      DigiMRec_command='''DigiMReco_trf.py inputHitsFile=%s outputESDFile=ESO.pool.root outputAODFile=AOD.pool.root outputHISTFile=%s outputRDOFile=ttbar_AtlfastII_RDO_50.pool.root maxEvents=%d preInclude_h2r=SimulationJobOptions/preInclude.PileUpBunchTrains2011Config8_DigitConfig.py,RunDependentSimData/configLumi_mc12_v1.py postInclude_h2r=FastSimulationJobTransforms/postInclude.AF2DigiSetContainerNames.py preExec_h2r='from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"]' postExec_h2r='ToolSvc.LArAutoCorrTotalToolDefault.NMinBias=0' preExec_r2e='rec.Commissioning.set_Value_and_Lock(True);jobproperties.Beam.energy.set_Value_and_Lock(3500*Units.GeV);muonRecFlags.writeSDOs=True;from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' preExec_e2a='TriggerFlags.AODEDMSet="AODSLIM";rec.Commissioning.set_Value_and_Lock(True);jobproperties.Beam.energy.set_Value_and_Lock(3500*Units.GeV);muonRecFlags.writeSDOs=True' RunNumber=158127 autoConfiguration=everything  conditionsTag=OFLCOND-MC12-AFII-07 geometryVersion=ATLAS-GEO-20-00-01 numberOfCavernBkg=0 DataRunNumber=-1 jobNumber=%d digiSeedOffset1=%d digiSeedOffset2=%d digiRndmSvc=AtDSFMTGenSvc cavernHitsFile=%s ''' % (Hitsfile_digi,Histfile,EvtMax,EvtMax,randomSeed1,randomSeed2,Hitsfile_digi)
      return DigiMRec_command
   elif commandtype=="ISF_sim":
      ISF_sim_command='''athena.py -c 'EvtMax=%d;Simulator="ATLFASTII";Input="%s"' ISF_Example/jobOptions_ConfGetter.py''' % (EvtMax,eventtype)
      return ISF_sim_command

####Check the existence of File###
def checkfile(filename):
   if os.path.exists(pwd+"/"+filename):
      print "File exist in this directory."
      return 1
   else:
      print "File does not exist in this directory"
      return 0

###Add EOS prefix###
def addeos(filename):
   argv=pwd.split('/')
   postfix='/'.join(argv[-4:])
   middle='/'.join(argv[-8:-5])
   prefix="root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod"
   path=prefix+'/'+middle+'/'+postfix+'/'+filename
   return path


###Run Job###
###main###
if __name__ =="__main__":
   if options.jobtype=="ATLFASTII":
      if options.eventtype=="ttbar":
         print "Run %s with %s" % (options.jobtype,options.eventtype)
         eventfile="/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root"
         simu_command=setcommand("Simu")
         log_Simu=os.system(simu_command)
         if log_Simu!=0:
            print "Error, Simulation job failed."
            sys.exit(1)
         else:
            if checkfile("Hits.pool.root")==1:
               Hitsfile="Hits.pool.root"
            else:
               Hitsfile=addeos("Hits.pool.root")
            merge_command=setcommand("Merge")
            log_Merge=os.system(merge_command)
            if log_Merge!=0:
               print "Error,Merge job failed."
               sys.exit(1)
            else:
               os.system("mv truth.root ATLFASTII_sim_ttbar.truth.root")
               if checkfile("Merge.pool.root")==1:
                  Hitsfile_digi="Merge.pool.root"
               else:
                  Hitsfile_digi=addeos("Merge.pool.root")
               digi_command=setcommand("DigiMRec")
               log_DigiMRec=os.system(digi_command)
               if log_DigiMRec!=0:
                  print "Error,Digi and Reco job failed."
                  sys.exit(1)
               else:
                  print "All Jobs run Ok"
                  sys.exit()


#  print simu_command
#  print merge_command
#  print digi_command
   elif options.jobtype=="Simu":
      print options.jobtype
#  log_Simu=os.system(Simu_command)
   elif options.jobtype=="Merge":
      print options.jobtype
#  log_Merge=os.system(Merge_command)
   elif options.jobtype=="DigiMRec":
      print options.jobtype
#  log_DigiMRec=os.system(DigiMRec_command)
   elif options.jobtype=="ISF":
      print options.jobtype
      isf_command=setcommand("ISF_sim")
      log_ISF_sim=os.system(isf_command)
      if log_ISF_sim!=0:
         print "Error,ISF Simulation job failed"
         sys.exit(1)
      else:
         if checkfile("Hits.pool.root")==1:
            Hitsfile_digi="Hits.pool.root"
         else:
            Hitsfile_digi=addeos("Hits.pool.root")
         digi_command=setcommand("DigiMRec")
         log_DigiMRec=os.system(digi_command)
         if log_DigiMRec!=0:
            print "Error,ISF Digi and Reco job failed"
            sys.exit(1)
         else:
            print "All Jobs run Ok"
            sys.exit()


   elif options.jobtype=="ISF_sim":
      print options.jobtype
     # log_ISF_sim=os.system("ISF_sim_command")
