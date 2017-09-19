import os

formatList = ['PHYSVAL', 
              'TOPQ1', 'TOPQ2', 'TOPQ3', 'TOPQ4', 'TOPQ5', 
              'HIGG1D1', 'HIGG1D2',  
              'HIGG2D1', 'HIGG2D2', 'HIGG2D4', 'HIGG2D5', 
              'HIGG3D1', 'HIGG3D2', 'HIGG3D3', 
              'HIGG4D1', 'HIGG4D2', 'HIGG4D3', 'HIGG4D4', 'HIGG4D5', 
              'HIGG5D1', 'HIGG5D2', 'HIGG5D3', 
              'HIGG6D1', 'HIGG6D2', 
              'HIGG8D1',  
              'STDM2', 'STDM3', 'STDM4', 'STDM5', 'STDM6', 'STDM7', 'STDM8', 'STDM9', 
              'TAUP1', 'TAUP2', 'TAUP3', 'TAUP4', 
              'SUSY1', 'SUSY2', 'SUSY3', 'SUSY4', 'SUSY5', 'SUSY6', 'SUSY7', 'SUSY8', 'SUSY10', 'SUSY11', 'SUSY12', 'SUSY13', 'SUSY14', 'SUSY15', 'SUSY16', 'SUSY17', 'SUSY18', 
              'EXOT0', 'EXOT2', 'EXOT3', 'EXOT4', 'EXOT5', 'EXOT6', 'EXOT7', 'EXOT8', 'EXOT9', 'EXOT10', 'EXOT12', 'EXOT13', 'EXOT14', 'EXOT15', 'EXOT17', 'EXOT18', 'EXOT19', 'EXOT20', 'EXOT21', 
              'JETM1', 'JETM2', 'JETM3', 'JETM4', 'JETM5', 'JETM6', 'JETM7', 'JETM8', 'JETM9', 'JETM10', 'JETM11', 
              'IDTR1', 
              'EGAM1', 'EGAM2', 'EGAM3', 'EGAM4', 'EGAM5', 'EGAM6', 'EGAM7', 'EGAM8', 'EGAM9', 
              'FTAG1', 'FTAG2', 'FTAG3', 'FTAG4',  
              'BPHY1', 'BPHY2', 'BPHY3', 'BPHY4', 'BPHY5', 'BPHY6', 'BPHY7', 'BPHY8', 'BPHY9', 'BPHY10', 'BPHY11', 'BPHY12', 
              'MUON0', 'MUON1', 'MUON2', 'MUON3', 'MUON4', 
              #'TCAL1', 
              #'TRUTH0', 'TRUTH1', 'TRUTH2', 'TRUTH3', 'TRUTH4', 'TRUTH5', 
              #'DAPR0', 'DAPR1', 'DAPR2', 
              #'HION1', 'HION2', 'HION3', 'HION4', 'HION5', 'HION6', 'HION7', 'HION8', 'HION9', 'HION10', 
              #'TRIG1', 'TRIG2', 'TRIG3', 'TRIG4', 'TRIG5'
]
mcLabel = "mc16"
dataLabel = "data17"
mcFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1"
dataFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327342.physics_Main.merge.AOD.f838_m1824._lb0300._0001.1"
daodBuild = True
doadMerge = False

def generateText(formatName,label,inputFile):
   outputFileName = "test_"+label+formatName+".sh" 
   outputFile = open(outputFileName,"w")
   outputFile.write("#!/bin/sh"+"\n")
   outputFile.write("\n")
   outputFile.write("# art-description: DAOD building "+formatName+" "+label+"\n")
   outputFile.write("# art-type: grid"+"\n")
   outputFile.write("# art-output: *.pool.root"+"\n")
   outputFile.write("\n")
   outputFile.write("Reco_tf.py --inputAODFile "+inputFile+" --outputDAODFile art.pool.root --reductionConf "+formatName+" --maxEvents 5000"+"\n")
   outputFile.write("\n")
   outputFile.write("DAODMerge_tf.py --maxEvents 5 --inputDAOD_"+formatName+"File DAOD_"+formatName+".art.pool.root --outputDAOD_"+formatName+"_MRGFile art_merged.pool.root"+"\n")
   outputFile.close()
   os.system("chmod +x "+outputFileName)

for formatName in formatList:
   generateText(formatName,dataLabel,dataFile)
   generateText(formatName,mcLabel,mcFile)


