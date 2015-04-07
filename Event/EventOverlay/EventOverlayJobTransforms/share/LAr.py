
#Useful for loading in LAr MC folders, needed when running digitization with data conditions

from IOVDbSvc.CondDB import conddb

conddb.blockFolder("/LAR/ElecCalibMC/AutoCorr")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/AutoCorr","LARElecCalibMCAutoCorr-CSC02-I2",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/DAC2uA")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/DAC2uA","LARElecCalibMCDAC2uA-CSC02-J",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/MinBias")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/MinBias","LARElecCalibMCMinBias-CSC02-E",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/MinBiasAverage")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/MinBiasAverage","LARElecCalibMCMinBiasAverage-CSC02-I",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/MphysOverMcal")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/MphysOverMcal","LARElecCalibMCMphysOverMcal-CSC02-I",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/Noise")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/Noise","LARElecCalibMCNoise-CSC02-I",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/OFC")
conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/OFC",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/Pedestal")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/Pedestal","LARElecCalibMCPedestal-DC3-B-IdFix7",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/Ramp")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/Ramp","LARElecCalibMCRamp-CSC02-K",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/Shape")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/Shape","LARElecCalibMCShape-CSC02-I",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/ShapeFullWave")
conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/ShapeFullWave",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/fSampl")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-CSC02-F-QGSP_BERT_BIRK",force=True,forceMC=True)
conddb.blockFolder("/LAR/ElecCalibMC/uA2MeV")
conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/uA2MeV","LARElecCalibMCuA2MeV-CSC02-I",force=True,forceMC=True)

