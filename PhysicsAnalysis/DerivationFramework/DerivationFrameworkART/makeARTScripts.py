import os

makeDataDAODs=True
makeMCDAODs=True
makeTruthDAODs=True
makeTrains=True

formatList = ['PHYSVAL','PHYS','PHYSLITE',
              'LLP1',
              'HDBS2',
              'HIGG1D1', 'HIGG2D1',
              'STDM6', 'STDM7', 'STDM11','STDM13',
              'TAUP1', 'TAUP2', 'TAUP3', 'TAUP4', 'TAUP5',
              'SUSY6', 'SUSY15',
              'EXOT2', 'EXOT13', 'EXOT15', 'EXOT17', 'EXOT20', 'EXOT21', 'EXOT23', 'EXOT28', 'EXOT29',
              'JETM1', 'JETM3', 'JETM4', 'JETM5', 'JETM6', 'JETM8', 'JETM9', 'JETM10', 'JETM11', 'JETM12', 'JETM13','JETM14','JETM15',
              'IDTR1', 'IDTR2',
              'EGAM1', 'EGAM2', 'EGAM3', 'EGAM4', 'EGAM5', 'EGAM7', 'EGAM8', 'EGAM9',
              'FTAG1', 'FTAG2', 'FTAG3', 'FTAG4', 'FTAG5',
              'BPHY1', 'BPHY2', 'BPHY3', 'BPHY4', 'BPHY5', 'BPHY6', 'BPHY7', 'BPHY8', 'BPHY9', 'BPHY10', 'BPHY11', 'BPHY12', 'BPHY14','BPHY15','BPHY16', 'BPHY17', 'BPHY18','BPHY19','BPHY20','BPHY21',
              'TCAL1',
              'HION2', 'HION3','HION4','HION5','HION6', 'HION7', 'HION8', 'HION9', 'HION10', 'HION11',
]

truthFormatList = ['TRUTH0', 'TRUTH1', 'TRUTH3']

trainList = [
              ["HDBS2"],
              ["TAUP5"],
              ["EGAM3","EGAM4","STDM14","JETM12","FTAG3","EGAM2"],
              ["JETM10","MUON2","JETM14"],
              ["EGAM9","EGAM7","STDM11","EXOT17","EGAM8","JETM3"],
              ["EGAM1","TAUP1","MUON6","JETM11","EXOT15","EXOT2","JETM4","EXOT21"],
              ["HIGG1D1","STDM7","FTAG2"],
              ["SUSY6","EXOT13","EXOT28","JETM6","TAUP3","EGAM5","MUON1","HIGG2D1"],
              ["MUON0","FTAG1","JETM9","JETM1"],
              ["FTAG4"],
              ["STDM6"]
            ]


mcLabel = "mc20"
dataLabel = "data18"
truthLabel = "mc15"
delayedStreamLabel = "data16DELAYED"
blsStreamLabel = "data17BPHYSLS"

mcFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc20_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3681_r13167/AOD.27312826._000061.pool.root.1"
mcFileBPHY21 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.16767830._000007.pool.root.1"
mcFileBPHY18 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.16278878._000048.pool.root.1"
mcFileBPHY20 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.16471215._000010.pool.root.1"
mcFileBPHY8 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11705353._000001.pool.root.1"
mcFileBPHY19 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.15110756._002435.pool.root.1"
mcFileBPHY14 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.13151497._000097.pool.root.1"
mcFileRPVLL = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14859811._000014.pool.root.1"
mcFileEXOT23 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/DAOD_RPVLL.15268048._000134.pool.root.1"

dataFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00357772.physics_Main.recon.AOD.r13286/AOD.27654050._000557.pool.root.1"
dataFileRPVLL = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/DAOD_RPVLL.13725673._000089.pool.root.1"
dataFileDelayed = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11270451._000007.pool.root.1"
dataFileBLS = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00337491.physics_BphysLS.merge.AOD.f873_m1885._lb0100._0001.1"
dataFileZeroBias = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00339070.physics_ZeroBias.merge.AOD.f887_m1892._lb0998-lb1007._0001.1"
heavyIonFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.17356768._000726.pool.root.1"#AOD.15763788._016518.pool.root.1"
heavyIon4File = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_hi.00365602.physics_UPC.merge.AOD.f1022_m2037._lb0203._0001.1"#data15_hi.00287866.physics_UPC.merge.AOD.f984_m2025._lb0257._0001.1"
heavyIonHPFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_hi.00365602.physics_HardProbes.merge.AOD.f1021_m2037._lb0203._0001.1"
truthFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/EVNT.17955321._002707.pool.root.1"

dataPreExec = " --preExec \'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff();\' "
mcPreExec   = " --preExec \'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff();\' "
dataPreExecLLP = " --preExec \'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); DetFlags.detdescr.pixel_setOn(); DetFlags.detdescr.SCT_setOn(); InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doR3LargeD0.set_Value_and_Lock(True);\' "
mcPreExecLLP   = " --preExec \'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); DetFlags.detdescr.pixel_setOn(); DetFlags.detdescr.SCT_setOn(); InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doR3LargeD0.set_Value_and_Lock(True);\' "
dataPreExecHION = " --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"BTaggingFixer\", Containers=[\"BTagging_AntiKt4HIAux.\"] ); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"InDetTrackParticlesFixer\", Containers = [ \"InDetTrackParticlesAux.\" ] )\' "
dataPreExecHION4 = " --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"BTaggingFixer\", Containers=[\"BTagging_AntiKt4HIAux.\", \"BTagging_AntiKt4EMTopoAux.\"] )\' "
dataPreExecHION5 = " --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True); from HIRecExample.HIRecExampleFlags import jobproperties; jobproperties.HIRecExampleFlags.doHIAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"BTaggingFixer\", Containers=[\"BTagging_AntiKt4HIAux.\"] )\' "
dataPreExecHION7 = " --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True); from HIRecExample.HIRecExampleFlags import jobproperties; jobproperties.HIRecExampleFlags.doHIAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"BTaggingFixer\", Containers=[\"BTagging_AntiKt4HIAux.\"] ); from HIJetRec.HIJetRecFlags import HIJetFlags; HIJetFlags.DoHIBTagging.set_Value_and_Lock(True)\' "
dataMergePreExec = " --preExec \'rec.doHeavyIon.set_Value_and_Lock(False)\' "

def generateText(formatName,label,inputFile,isTruth,isMC,nEvents):
   outputFileName = "test_"+label+formatName+".sh"
   outputFile = open(outputFileName,"w")
   outputFile.write("#!/bin/sh"+"\n")
   outputFile.write("\n")
   outputFile.write("# art-include: master/Athena"+"\n")
   outputFile.write("# art-description: DAOD building "+formatName+" "+label+"\n")
   outputFile.write("# art-type: grid"+"\n")
   outputFile.write("# art-output: *.pool.root"+"\n")
   outputFile.write("# art-output: checkFile.txt"+"\n")
   outputFile.write("# art-output: checkxAOD.txt"+"\n")
   outputFile.write("# art-output: checkIndexRefs.txt"+"\n")
   outputFile.write("\n")
   outputFile.write("set -e"+"\n")
   outputFile.write("\n")
   if (not isTruth and not isMC):
      if formatName[0:4] == "HION":# and not formatName == "HION3":
         if formatName == "HION4":
            outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExecHION4+"\n")
         elif formatName in ["HION5"]:
            outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExecHION5+"\n")
         elif formatName in ["HION7","HION8"]:
            outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExecHION7+"\n")
         else:
            outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExecHION+"\n")
      elif formatName in ["LLP1","IDTR2"]:
         outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExecLLP+"\n")
      else:
         outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExec+"\n")
   if (not isTruth and isMC):
      if formatName in ["LLP1","IDTR2"]:
         outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+mcPreExecLLP+"\n")
      else:
         outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+mcPreExec+"\n")
   if isTruth: outputFile.write("Reco_tf.py --inputEVNTFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+"\n")
   outputFile.write("\n")
   outputFile.write("echo \"art-result: $? reco\""+"\n")
   outputFile.write("\n")
   if not isTruth: 
      if formatName[0:4] == "HION" and not formatName == "HION3":
         outputFile.write("DAODMerge_tf.py --inputDAOD_"+formatName+"File DAOD_"+formatName+".art.pool.root --outputDAOD_"+formatName+"_MRGFile art_merged.pool.root"+dataMergePreExec+"\n")
      else:
         outputFile.write("DAODMerge_tf.py --inputDAOD_"+formatName+"File DAOD_"+formatName+".art.pool.root --outputDAOD_"+formatName+"_MRGFile art_merged.pool.root"+"\n")
   if isTruth: outputFile.write("DAODMerge_tf.py --inputDAOD_"+formatName+"File DAOD_"+formatName+".art.pool.root --outputDAOD_"+formatName+"_MRGFile art_merged.pool.root"+" --autoConfiguration ProjectName RealOrSim BeamType ConditionsTag DoTruth InputType BeamEnergy LumiFlags TriggerStream --athenaopts=\"-s\" "+"\n")
   outputFile.write("\n")
   outputFile.write("echo \"art-result: $? merge\""+'\n')
   outputFile.write("\n")
   outputFile.write("checkFile.py DAOD_"+formatName+".art.pool.root > checkFile.txt"+"\n")
   outputFile.write("\n")
   outputFile.write("echo \"art-result: $?  checkfile\""+'\n')
   outputFile.write("\n")
   outputFile.write("checkxAOD.py DAOD_"+formatName+".art.pool.root > checkxAOD.txt"+"\n")
   outputFile.write("\n")
   outputFile.write("echo \"art-result: $?  checkxAOD\""+'\n')
   outputFile.write("\n")
   outputFile.write("checkIndexRefs.py DAOD_"+formatName+".art.pool.root > checkIndexRefs.txt"+"\n")
   outputFile.write("\n")
   outputFile.write("echo \"art-result: $?  checkIndexRefs\""+'\n')
   outputFile.close()
   os.system("chmod +x "+outputFileName)

def generateTrains(formatList,label,inputFile,isMC):
   if (isMC and ("BPHY8" in formatList)): formatList.remove("BPHY8")
   if (isMC and ("BPHY19" in formatList)): formatList.remove("BPHY19")
   outputFileName = "test_"+label+"_".join(formatList)+".sh"
   outputFile = open(outputFileName,"w")
   outputFile.write("#!/bin/sh"+"\n")
   outputFile.write("\n")
   outputFile.write("# art-include: master/Athena"+"\n")
   outputFile.write("# art-description: DAOD building "+" ".join(formatList)+" "+label+"\n")
   outputFile.write("# art-type: grid"+"\n")
   outputFile.write("# art-output: *.pool.root"+"\n")
   outputFile.write("\n")
   outputFile.write("set -e"+"\n")
   outputFile.write("\n")
   if not isMC: outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+" ".join(formatList)+" --maxEvents 500 "+dataPreExec+" --passThrough True "+"\n")
   if isMC: outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+" ".join(formatList)+" --maxEvents 500 "+mcPreExec+" --passThrough True "+"\n")
   os.system("chmod +x "+outputFileName)

if (makeDataDAODs or makeMCDAODs):
   for formatName in formatList:
      if makeDataDAODs: 
         if formatName in ["EXOT23","SUSY15","SUSY6","EXOT15"]:
            generateText(formatName,dataLabel+"RPVLL",dataFileRPVLL,False,False,"-1")
            if formatName == "SUSY6": generateText(formatName,dataLabel,dataFile,False,False,"-1") 
         elif formatName=="BPHY3":
            generateText(formatName,dataLabel,dataFile,False,False,"500")
         elif formatName in ['BPHY7']:
            generateText(formatName,dataLabel,dataFile,False,False,"-1")
            generateText(formatName,delayedStreamLabel,dataFileDelayed,False,False,"-1")
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"500")
         elif formatName in ['BPHY8']:
            generateText(formatName,dataLabel,dataFile,False,False,"-1")
            generateText(formatName,delayedStreamLabel,dataFileDelayed,False,False,"-1")
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"1000") 
         elif formatName in ['BPHY18','BPHY21']:
            generateText(formatName,dataLabel,dataFile,False,False,"-1")
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"1000") 
         elif formatName in ['BPHY10','BPHY19']:
            generateText(formatName,dataLabel,dataFile,False,False,"-1")
            generateText(formatName,delayedStreamLabel,dataFileDelayed,False,False,"-1")
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"-1")
         elif formatName in ['STDM7']:
            generateText(formatName,dataLabel,dataFile,False,False,"-1")
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"-1")
         elif formatName=='BPHY20':
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"-1")
         elif formatName=='JETM5':
            generateText(formatName,dataLabel,dataFileZeroBias,False,False,"-1")
         elif formatName=='HION4':
            generateText(formatName,dataLabel,heavyIon4File,False,False,"-1")
         elif formatName in ['HION5','HION7','HION8']:
            generateText(formatName,dataLabel,heavyIonHPFile,False,False,"-1")
         elif formatName[0:4]=='HION':
            generateText(formatName,dataLabel,heavyIonFile,False,False,"-1")
         else: generateText(formatName,dataLabel,dataFile,False,False,"-1")
      if makeMCDAODs:
         if formatName[0:4]=='HION' and not formatName=='HION3': continue # only HION3 runs on MC
         if formatName in ["SUSY15","SUSY6","EXOT15"]:
            generateText(formatName,mcLabel+"RPVLL",mcFileRPVLL,False,True,"-1")
            if formatName == "SUSY6":generateText(formatName,mcLabel,mcFile,False,True,"-1") 
         elif formatName == "EXOT23":
            generateText(formatName,mcLabel+"RPVLL",mcFileEXOT23,False,True,"-1")
         elif formatName=="BPHY3":
            generateText(formatName,mcLabel,mcFile,False,True,"500")
         elif formatName=="BPHY8":
            generateText(formatName,mcLabel,mcFileBPHY8,False,True,"-1")
         elif formatName=="BPHY19":
            generateText(formatName,mcLabel,mcFileBPHY19,False,True,"5000")
         elif formatName=="BPHY14":
            generateText(formatName,mcLabel,mcFileBPHY14,False,True,"-1")
         elif formatName=="BPHY18":
            generateText(formatName,mcLabel,mcFileBPHY18,False,True,"5000")
         elif formatName=="BPHY20":
            generateText(formatName,mcLabel,mcFileBPHY20,False,True,"5000")
         elif formatName=="BPHY21":
            generateText(formatName,mcLabel,mcFileBPHY21,False,True,"5000")
         else: generateText(formatName,mcLabel,mcFile,False,True,"-1")

if makeTruthDAODs:
   for formatName in truthFormatList:
      generateText(formatName,truthLabel,truthFile,True,False,"1000")

if makeTrains:
   for train in trainList:
      generateTrains(train,dataLabel,dataFile,False)
      generateTrains(train,mcLabel,mcFile,True)
   generateTrains(["SUSY15","EXOT23","EXOT15","SUSY6"],mcLabel,mcFileEXOT23,True)
   generateTrains(["SUSY15","EXOT23","EXOT15","SUSY6"],dataLabel,dataFileRPVLL,False)
