# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import namedtuple


OFCDefinitions=namedtuple("OFCDefinitions",
                          ["Algoname",
                           "Nsamples",
                           "Nphase",
                           "Dphase",
                           "PhysAutoCorr",
                           "useDelta",
                           "KeyOFC",
                           "KeyShape",
                           "FolderOFC",
                           "FolderShape",
                           "FolderOFCPicked",
                           "FolderShapePicked",
                           "ReadDSPConfig",
                           "DSPConfigFolder" 
                           ]
                          )


OFCDef_4Samples=OFCDefinitions("OFC4samples",
                               Nsamples=4,
                               Nphase=8,
                               # hack for shifted OFC
                               #Nphase=16,
                               Dphase=3, 
                               PhysAutoCorr=(False,True),
                               useDelta=(0,0),
                               KeyOFC=("LArOFC_4_0","LArOFC_4_0_mu"),
                               KeyShape="LArShape_4_0",
                               FolderOFC="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples3bins17phases",
                               FolderShape="/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases",
                               FolderOFCPicked="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples1phase",
                               FolderShapePicked="/LAR/ElecCalibOfl/Shape/RTM/4samples1phase",
                               ReadDSPConfig=True,
                               DSPConfigFolder="/LAR/Configuration/DSPConfiguration"
                               )


OFCDef_5Samples=OFCDefinitions("OFC5samples",
                               Nsamples=5,
                               Nphase=8,
                               # hack for shifted OFC
                               #Nphase=16,
                               Dphase=3, 
                               PhysAutoCorr=(False,True),
                               useDelta=(0,0),
                               KeyOFC=("LArOFC_5_0","LArOFC_5_0_mu"),
                               KeyShape="LArShape_5_0",
                               FolderOFC="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples3bins17phases",
                               FolderShape="/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases",
                               FolderOFCPicked="/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples1phase",
                               FolderShapePicked="/LAR/ElecCalibOfl/Shape/RTM/5samples1phase",
                               ReadDSPConfig=True,
                               DSPConfigFolder="/LAR/Configuration/DSPConfiguration"
                               )
                               


OFCDefs=(OFCDef_5Samples,OFCDef_4Samples)
#OFCDefsRepro=(OFCDef_4Samples,0)

