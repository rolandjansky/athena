import os

makeDataDAODs=True
makeMCDAODs=True
makeTruthDAODs=False
makeTrains=False

formatList = ['PHYSVAL',
              'TOPQ1', 'TOPQ2', 'TOPQ4', 'TOPQ5',
              'HIGG1D1', 'HIGG1D2',
              'HIGG2D1', 'HIGG2D4', 'HIGG2D5',
              'HIGG3D1', 'HIGG3D3',
              'HIGG4D1', 'HIGG4D2', 'HIGG4D3', 'HIGG4D4', 'HIGG4D5','HIGG4D6',
              'HIGG5D1', 'HIGG5D2', 'HIGG5D3',
              'HIGG6D1', 'HIGG6D2',
              'HIGG8D1',
              'STDM2', 'STDM3', 'STDM4', 'STDM5', 'STDM6', 'STDM7', 'STDM8', 'STDM9',
              'TAUP1', 'TAUP2', 'TAUP3', 'TAUP4',
              'SUSY1', 'SUSY2', 'SUSY3', 'SUSY4', 'SUSY5', 'SUSY6', 'SUSY7', 'SUSY8', 'SUSY9', 'SUSY10', 'SUSY11', 'SUSY12', 'SUSY13', 'SUSY14', 'SUSY15', 'SUSY16', 'SUSY17', 'SUSY18',
              'EXOT0', 'EXOT2', 'EXOT3', 'EXOT4', 'EXOT5', 'EXOT6', 'EXOT7', 'EXOT8', 'EXOT9', 'EXOT10', 'EXOT12', 'EXOT13', 'EXOT15', 'EXOT17', 'EXOT19', 'EXOT20', 'EXOT21', 'EXOT22', 'EXOT23','EXOT24','EXOT25','EXOT26',
              'JETM1', 'JETM2', 'JETM3', 'JETM4', 'JETM5', 'JETM6', 'JETM7', 'JETM8', 'JETM9', 'JETM10', 'JETM11', 'JETM12', 'JETM13',
              'IDTR1',
              'EGAM1', 'EGAM2', 'EGAM3', 'EGAM4', 'EGAM5', 'EGAM6', 'EGAM7', 'EGAM8', 'EGAM9',
              'FTAG1', 'FTAG2', 'FTAG3', 'FTAG4', 'FTAG5',
              'BPHY1', 'BPHY2', 'BPHY3', 'BPHY4', 'BPHY5', 'BPHY6', 'BPHY7', 'BPHY8', 'BPHY9', 'BPHY10', 'BPHY11', 'BPHY12',
              'MUON0', 'MUON1', 'MUON2', 'MUON3', 'MUON4',
              'TCAL1'
              #'HION1', 'HION2', 'HION3', 'HION4', 'HION5', 'HION6', 'HION7', 'HION8', 'HION9', 'HION10'
]

truthFormatList = ['TRUTH0', 'TRUTH1', 'TRUTH3']

trainList = [ ["TCAL1","HIGG2D5","EXOT10"], # 0.02-0.03%
              ["EGAM3","JETM12","EGAM4"], # 0.08-0.12%
              ["STDM5","EXOT12","EGAM2","EXOT9"], # 0.17-0.23%
              ["BPHY5","HIGG4D4","SUSY9","EXOT17"], # 0.3-0.32%
              ["EXOT0","SUSY12","JETM7","HIGG1D2"], # 0.35-0.38%
              ["MUON2","EGAM7","EGAM9","BPHY1"], # 0.39-0.46%
              ["BPHY4","EXOT6","HIGG4D1","SUSY2","SUSY11"], # 0.5-0.53%
              ["STDM3","EXOT19","TOPQ5","TOPQ2","JETM4","JETM3" ], # 0.69-0.76%
              ["EXOT15","SUSY7","SUSY16","HIGG4D6"], # 0.8-0.9%
              ["STDM2","SUSY18","EXOT7","EGAM1","HIGG4D5"], # 0.98-10.8%
              ["EXOT3","SUSY3","HIGG4D3","EXOT22"], # 1.1-1.2%
              ["HIGG6D1","SUSY1","TAUP1","SUSY4"], # 1.28-1.41%
              ["EXOT2","JETM11","STDM7","EXOT21"], # 1.45-1.58%
              ["JETM1","JETM9","EXOT5"], # 1.62-1.65%
              ["SUSY10","SUSY8","SUSY5"], # 1.7-1.75%
              ["EXOT13","EGAM5","EXOT8","EXOT4"], # 2.09.2.4%
              ["HIGG4D2","HIGG8D1","JETM6"], #2.7-2.9%
              ["SUSY6","HIGG2D1"], # 3.3-3.7%
              ["MUON1","TOPQ1"], # 3.9-4.5%
              ["STDM4","MUON0"], # 5.2-5.9%
              ["HIGG6D2","TOPQ4","TAUP3"], # 7.1-8.4%
              ["FTAG2","FTAG4"] # FTAG train
              # Following run as singletons: FTAG1, FTAG3, JETM10, HIGG1D1 
              # Special streams (BLS/ZB) not run as trains
            ]

mcLabel = "mc16"
dataLabel = "data18"
truthLabel = "mc15"
delayedStreamLabel = "data16DELAYED"
blsStreamLabel = "data17BPHYSLS"
mcFileBPHY8 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11705353._000001.pool.root.1"
mcFileEXOT23 = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/user.ctreado.11717804.EXT0._000056.DAOD_RPVLL.pool.root"
mcFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12169019._004055.pool.root.1"
dataFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00348403.physics_Main.merge.AOD.f920_m1947._lb0829._0001.1"
dataFileRPVLL = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/DAOD_RPVLL.13679930._004714.pool.root.1"
dataFileDelayed = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11270451._000007.pool.root.1"
dataFileBLS = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00337491.physics_BphysLS.merge.AOD.f873_m1885._lb0100._0001.1"
dataFileZeroBias = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00339070.physics_ZeroBias.merge.AOD.f887_m1892._lb0998-lb1007._0001.1"
heavyIonFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_hi.00338037.physics_MinBias.merge.AOD.f900_m1912._lb0292._0009.1"
truthFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/EVNT.05192704._020091.pool.root.1"
dataPreExec = " --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = \"BTagCalibRUN12Onl-08-40\"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( \"InDetTrackParticlesFixer\", Containers = [ \"InDetTrackParticlesAux.\" ] )\' "
mcPreExec = " --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = \"BTagCalibRUN12-08-40\" \' "

def generateText(formatName,label,inputFile,isTruth,isMC,nEvents):
   outputFileName = "test_"+label+formatName+".sh"
   outputFile = open(outputFileName,"w")
   outputFile.write("#!/bin/sh"+"\n")
   outputFile.write("\n")
   outputFile.write("# art-description: DAOD building "+formatName+" "+label+"\n")
   outputFile.write("# art-type: grid"+"\n")
   outputFile.write("# art-output: *.pool.root"+"\n")
   outputFile.write("# art-output: checkFile.txt"+"\n")
   outputFile.write("# art-output: checkxAOD.txt"+"\n")
   outputFile.write("\n")
   outputFile.write("set -e"+"\n")
   outputFile.write("\n")
   if ((isTruth==False) and (isMC==False) ): outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+dataPreExec+"\n")
   if ((isTruth==False) and (isMC==True) ): outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+mcPreExec+"\n")
   if (isTruth==True): outputFile.write("Reco_tf.py --inputEVNTFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents "+nEvents+"\n")
   outputFile.write("\n")
   outputFile.write("echo \"art-result: $? reco\""+"\n")
   outputFile.write("\n")
   if (isTruth==False): outputFile.write("DAODMerge_tf.py --inputDAOD_"+formatName+"File DAOD_"+formatName+".art.pool.root --outputDAOD_"+formatName+"_MRGFile art_merged.pool.root"+"\n")
   if (isTruth==True): outputFile.write("DAODMerge_tf.py --inputDAOD_"+formatName+"File DAOD_"+formatName+".art.pool.root --outputDAOD_"+formatName+"_MRGFile art_merged.pool.root"+" --autoConfiguration ProjectName RealOrSim BeamType ConditionsTag DoTruth InputType BeamEnergy LumiFlags TriggerStream --athenaopts=\"-s\" "+"\n")
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
   outputFile.close()
   os.system("chmod +x "+outputFileName)

def generateTrains(formatList,label,inputFile,isMC):
   if (isMC == True) and ("BPHY8" in formatList): formatList.remove("BPHY8")
   outputFileName = "test_"+label+"_".join(formatList)+".sh"
   outputFile = open(outputFileName,"w")
   outputFile.write("#!/bin/sh"+"\n")
   outputFile.write("\n")
   outputFile.write("# art-description: DAOD building "+" ".join(formatList)+" "+label+"\n")
   outputFile.write("# art-type: grid"+"\n")
   outputFile.write("# art-output: *.pool.root"+"\n")
   outputFile.write("\n")
   outputFile.write("set -e"+"\n")
   outputFile.write("\n")
   if (isMC == False): outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+" ".join(formatList)+" --maxEvents 500 "+dataPreExec+" --passThrough True "+"\n")
   if (isMC == True): outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+" ".join(formatList)+" --maxEvents 500 "+mcPreExec+" --passThrough True "+"\n")
   os.system("chmod +x "+outputFileName)


if (makeDataDAODs or makeMCDAODs):
   for formatName in formatList:
      if (makeDataDAODs): 
         if formatName=="EXOT23":
            generateText(formatName,dataLabel,dataFileRPVLL,False,False,"-1")
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
         elif formatName=='BPHY10':
            generateText(formatName,dataLabel,dataFile,False,False,"-1")
            generateText(formatName,delayedStreamLabel,dataFileDelayed,False,False,"-1")
            generateText(formatName,blsStreamLabel,dataFileBLS,False,False,"-1")
         elif formatName=='JETM5':
            generateText(formatName,dataLabel,dataFileZeroBias,False,False,"-1")
         elif formatName[0:4]=='HION':
            generateText(formatName,dataLabel,heavyIonFile,False,False,"-1")
         else: generateText(formatName,dataLabel,dataFile,False,False,"-1")
      if (makeMCDAODs):
         if formatName=="EXOT23":
            generateText(formatName,mcLabel,mcFileEXOT23,False,True,"-1")
         elif formatName=="BPHY3":
            generateText(formatName,mcLabel,mcFile,False,True,"500")
         elif formatName=="BPHY8":
            generateText(formatName,mcLabel,mcFileBPHY8,False,True,"-1")
         else: generateText(formatName,mcLabel,mcFile,False,True,"-1")

if (makeTruthDAODs):
   for formatName in truthFormatList:
      generateText(formatName,truthLabel,truthFile,True,False,"1000")

if (makeTrains):
   for train in trainList:
      generateTrains(train,dataLabel,dataFile,False)
      #generateTrains(train,mcLabel,mcFile,True)
   generateTrains(['TOPQ1','TOPQ2','TOPQ4','TOPQ5'],mcLabel,mcFile,True) # special train, not run in production but needed for testing purposes
