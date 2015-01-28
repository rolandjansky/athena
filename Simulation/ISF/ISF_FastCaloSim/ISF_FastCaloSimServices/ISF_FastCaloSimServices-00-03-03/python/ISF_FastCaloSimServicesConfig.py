# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF_FastCaloSimServices
KG Tan, 04/12/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved
from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags

# definitions taken from file:
#   FastSimulationJobTransforms/jobConfig.FastCaloSim_DB.py
def setAdditionalParticleParametrizationFileNames( FastShowerCellBuilderTool ):
  FastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames = [
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_10000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_10000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_10000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_10000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_10000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_100000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_100000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_100000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_100000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_100000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_1000000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_2000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_2000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_2000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_2000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_2000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_20000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_20000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_20000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_20000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_20000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_200000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_5000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_5000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_5000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_5000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_5000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_50000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_50000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_50000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_50000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_50000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_500000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_500000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_500000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_500000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_500000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_400/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_400/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_400/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_400/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_400/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_600/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_600/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_600/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_600/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_600/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_800/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_800/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_800/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_800/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_211/EN_800/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_10000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_10000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_10000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_10000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_10000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_100000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_1000000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_2000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_2000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_2000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_2000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_2000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_20000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_20000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_20000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_20000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_20000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_200000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_5000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_5000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_5000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_5000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_5000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_50000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_500000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_500000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_500000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_500000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_500000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_400/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_400/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_400/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_400/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_400/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_600/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_600/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_600/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_600/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_600/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_800/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_800/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_800/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_800/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_22/EN_800/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_10000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_10000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_10000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_10000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_10000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_100000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_1000000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_2000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_2000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_2000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_2000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_2000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_20000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_20000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_20000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_20000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_20000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_200000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_5000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_5000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_5000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_5000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_5000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_50000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_500000/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_500000/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_500000/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_500000/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_500000/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_400/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_400/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_400/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_400/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_400/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_600/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_600/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_600/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_600/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_600/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_800/eta_central",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_800/eta_crack",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_800/eta_endcap1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_800/eta_endcap2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:EnergyResults/pdgid_11/EN_800/eta_fcal",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_5000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_2000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_10000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_1000000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_500000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_20000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_100000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_200000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_22/EN_50000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_5000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_2000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_10000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_1000000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_500000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_20000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_100000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_200000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_11/EN_50000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_5000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_2000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_10000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_1000000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_500000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_20000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_100000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_4",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_200000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_14",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_12",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_8",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_13",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_20",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_9",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_11",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_10",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_5",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_6",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_1",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_7",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_2",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_19",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_0",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_3",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_18",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_15",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_16",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_17",
    "DB=/GLOBAL/AtlfastII/FastCaloSimParam:2:ShapeResults/pdgid_211/EN_50000/calosample_4", 
  ]

def getPunchThroughTool(name="ISF_PunchThroughTool", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService"     , simFlags.RandomSvc()                               )
    kwargs.setdefault("RandomStreamName"        , ISF_FastCaloSimFlags.RandomStreamName()            )
    kwargs.setdefault("FilenameLookupTable"     , "CaloPunchThroughParametrisation.root"             )
    kwargs.setdefault("PunchThroughInitiators"  , [ 211 ]                                            )
    kwargs.setdefault("PunchThroughParticles"   , [    2212,     211,      22,      11,      13 ]    )
    kwargs.setdefault("DoAntiParticles"         , [   False,    True,   False,    True,    True ]    )
    kwargs.setdefault("CorrelatedParticle"      , [     211,    2212,      11,      22,       0 ]    )
    kwargs.setdefault("FullCorrelationEnergy"   , [ 100000., 100000., 100000., 100000.,      0. ]    )
    kwargs.setdefault("MinEnergy"               , [   938.3,   135.6,     50.,     50.,   105.7 ]    )
    kwargs.setdefault("MaxNumParticles"         , [      -1,      -1,      -1,      -1,      -1 ]    )
    kwargs.setdefault("EnergyFactor"            , [      1.,      1.,      1.,      1.,      1. ]    )
    kwargs.setdefault("BarcodeSvc"              , ISF_Flags.BarcodeService()                         )
    kwargs.setdefault("EnvelopeDefSvc"          , getService('AtlasGeometry_EnvelopeDefSvc')         )
    kwargs.setdefault("BeamPipeRadius"          , 500.						     )

    from ISF_PunchThroughTools.ISF_PunchThroughToolsConf import ISF__PunchThroughTool
    return ISF__PunchThroughTool(name, **kwargs )

def getEmptyCellBuilderTool(name="ISF_EmptyCellBuilderTool", **kwargs):
    from FastCaloSim.FastCaloSimConf import EmptyCellBuilderTool
    return EmptyCellBuilderTool(name, **kwargs )

def getFastShowerCellBuilderTool(name="ISF_FastShowerCellBuilderTool", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory
    #return FastCaloSimFactory(name, **kwargs )
    #HACK: For some reason, FastCaloSimFactory() doesn't take arguments! - KG
    FastShowerCellBuilderTool = FastCaloSimFactory()
    FastShowerCellBuilderTool.RandomService    = simFlags.RandomSvc()
    FastShowerCellBuilderTool.RandomStreamName = ISF_FastCaloSimFlags.RandomStreamName()
    FastShowerCellBuilderTool.DoSimulWithInnerDetectorTruthOnly    = True
    FastShowerCellBuilderTool.ID_cut_off_r                         = [ 1150 ]
    FastShowerCellBuilderTool.ID_cut_off_z                         = [ 3490 ]
    FastShowerCellBuilderTool.DoSimulWithInnerDetectorV14TruthCuts = True
    FastShowerCellBuilderTool.DoNewEnergyEtaSelection              = True
    FastShowerCellBuilderTool.DoEnergyInterpolation                = True
    FastShowerCellBuilderTool.use_Ekin_for_depositions             = True
    FastShowerCellBuilderTool.McLocation                           = ISF_FastCaloSimFlags.FastShowerInputCollection()
    FastShowerCellBuilderTool.ParticleParametrizationFileName      = ''
    setAdditionalParticleParametrizationFileNames( FastShowerCellBuilderTool )
    FastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames.insert(0,"L1_L2_egamma_corr.config20.root")
    from IOVDbSvc.CondDB import conddb
    conddb.addFolder('GLOBAL_OFL','/GLOBAL/AtlfastII/FastCaloSimParam <tag>FastCaloSim_v2</tag>')
    return FastShowerCellBuilderTool

def getPileupFastShowerCellBuilderTool(name="ISF_PileupFastShowerCellBuilderTool", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory
    #
    # weights from:
    # http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/FastShower/FastCaloSim/FastCaloSim/FastCaloSim_CaloCell_ID.h
    weightsfcs = [
      ### LAr presampler
      #FirstSample=CaloCell_ID::PreSamplerB,
      2.0,
      ### LAr barrel
      #PreSamplerB=CaloCell_ID::PreSamplerB,
      #EMB1=CaloCell_ID::EMB1,
      #EMB2=CaloCell_ID::EMB2,
      #EMB3=CaloCell_ID::EMB3,  
      2.0, 2.0, 2.0, 2.0, 
      ### LAr EM endcap
      #PreSamplerE=CaloCell_ID::PreSamplerE,
      #EME1=CaloCell_ID::EME1,
      #EME2=CaloCell_ID::EME2,
      #EME3=CaloCell_ID::EME3,  
      2.0, 2.0, 2.0, 2.0, 
      ### Hadronic end cap cal.
      #HEC0=CaloCell_ID::HEC0,
      #HEC1=CaloCell_ID::HEC1,
      #HEC2=CaloCell_ID::HEC2,
      #HEC3=CaloCell_ID::HEC3,  
      2.0, 2.0, 2.0, 2.0, 
      ### Tile barrel
      #TileBar0=CaloCell_ID::TileBar0,
      #TileBar1=CaloCell_ID::TileBar1,
      #TileBar2=CaloCell_ID::TileBar2, 
      1.0, 1.0, 1.0, 
      ### Tile gap (ITC & scint)
      #TileGap1=CaloCell_ID::TileGap1,
      #TileGap2=CaloCell_ID::TileGap2,
      #TileGap3=CaloCell_ID::TileGap3, 
      1.0, 1.0, 1.0, 
      ### Tile extended barrel
      #TileExt0=CaloCell_ID::TileExt0,
      #TileExt1=CaloCell_ID::TileExt1,
      #TileExt2=CaloCell_ID::TileExt2, 
      1.0, 1.0, 1.0, 
      ### Forward EM endcap
      #FCAL0=CaloCell_ID::FCAL0,
      #FCAL1=CaloCell_ID::FCAL1,
      #FCAL2=CaloCell_ID::FCAL2,       
      1.0, 1.0, 1.0, 
      ### Beware of MiniFCAL! We don't have it, so different numbers after FCAL2
      #LastSample = CaloCell_ID::FCAL2,
      #MaxSample = LastSample+1
      1.0, 1.0,
    ]
    #return FastCaloSimFactory(name, **kwargs )
    #HACK: For some reason, FastCaloSimFactory() doesn't take arguments! - KG
    FastShowerCellBuilderTool = FastCaloSimFactory()
    FastShowerCellBuilderTool.RandomService    = simFlags.RandomSvc()
    FastShowerCellBuilderTool.RandomStreamName = ISF_FastCaloSimFlags.RandomStreamName()
    FastShowerCellBuilderTool.DoSimulWithInnerDetectorTruthOnly    = True
    FastShowerCellBuilderTool.ID_cut_off_r                         = [ 1150 ]
    FastShowerCellBuilderTool.ID_cut_off_z                         = [ 3490 ]
    FastShowerCellBuilderTool.DoSimulWithInnerDetectorV14TruthCuts = True
    FastShowerCellBuilderTool.DoNewEnergyEtaSelection              = True
    FastShowerCellBuilderTool.DoEnergyInterpolation                = True
    FastShowerCellBuilderTool.use_Ekin_for_depositions             = True
    FastShowerCellBuilderTool.McLocation                           = ISF_FastCaloSimFlags.FastShowerInputCollection()
    FastShowerCellBuilderTool.ParticleParametrizationFileName      = ''
    # MB : set weights for parameter segmentation
    FastShowerCellBuilderTool.sampling_energy_reweighting          = weightsfcs
    # 
    setAdditionalParticleParametrizationFileNames( FastShowerCellBuilderTool )
    FastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames.insert(0,"L1_L2_egamma_corr.config20.root")
    from IOVDbSvc.CondDB import conddb
    conddb.addFolder('GLOBAL_OFL','/GLOBAL/AtlfastII/FastCaloSimParam <tag>FastCaloSim_v2</tag>')
    return FastShowerCellBuilderTool

def getFastHitConvertTool(name="ISF_FastHitConvertTool",**kwargs):
    from FastCaloSimHit.FastCaloSimHitConf import FastHitConvertTool 
    return FastHitConvertTool(name,**kwargs)

def getCaloNoiseTool(name="ISF_FCS_CaloNoiseTool", **kwargs):
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    return CaloNoiseToolDefault(name, **kwargs )

def getAddNoiseCellBuilderTool(name="ISF_AddNoiseCellBuilderTool", **kwargs):
    kwargs.setdefault("CaloNoiseTool" , getPublicTool('ISF_FCS_CaloNoiseTool').getFullName())

    from FastCaloSim.FastCaloSimConf import AddNoiseCellBuilderTool
    return AddNoiseCellBuilderTool(name, **kwargs )

def getCaloCellContainerFinalizerTool(name="ISF_CaloCellContainerFinalizerTool", **kwargs):
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool     
    return CaloCellContainerFinalizerTool(name, **kwargs )

#### FastCaloSimSvc
def getFastCaloSimSvc(name="ISF_FastCaloSimSvc", **kwargs):
    from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
    kwargs.setdefault("BatchProcessMcTruth"              , False                                             )
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False                                             )
    kwargs.setdefault("Identifier"                       , 'FastCaloSim'                                     )
    kwargs.setdefault("CaloCellsOutputName"              , ISF_FastCaloSimFlags.CaloCellsName()              )
    kwargs.setdefault("PunchThroughTool"                 , getPublicTool('ISF_PunchThroughTool')             )
    kwargs.setdefault("DoPunchThroughSimulation"         , False                                             )
    kwargs.setdefault("ParticleBroker"                   , getService('ISF_ParticleBrokerSvc')               )
    kwargs.setdefault("CaloCellMakerTools_setup"         , [
                                                             getPublicTool('ISF_EmptyCellBuilderTool'),
                                                           ])
    kwargs.setdefault("CaloCellMakerTools_simulate"      , [
                                                             getPublicTool('ISF_FastShowerCellBuilderTool'),
                                                           ])
    kwargs.setdefault("CaloCellMakerTools_release"       , [
                                                             #getPublicTool('ISF_AddNoiseCellBuilderTool'),
                                                             getPublicTool('ISF_CaloCellContainerFinalizerTool'),
                                                             getPublicTool('ISF_FastHitConvertTool')
                                                           ])
    # let the ISF FCS flags know that FCS is being used
    ISF_FastCaloSimFlags.FastCaloSimIsActive.set_Value_and_Lock(True)

    # register the FastCaloSim random number streams
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    simFlags.RandomSeedList.addSeed( ISF_FastCaloSimFlags.RandomStreamName(), 98346412, 12461240 )

    from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConf import ISF__FastCaloSimSvc
    return ISF__FastCaloSimSvc(name, **kwargs )


#### Pileup FastCaloSim
def getFastCaloSimPileupSvc(name="ISF_FastCaloSimPileupSvc", **kwargs):
    from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
    kwargs.setdefault("BatchProcessMcTruth"              , False                                             )
    kwargs.setdefault("SimulateUndefinedBarcodeParticles", False                                             )
    kwargs.setdefault("Identifier"                       , 'FastCaloSim'                                     )
    kwargs.setdefault("CaloCellsOutputName"              , ISF_FastCaloSimFlags.CaloCellsName()              )
    kwargs.setdefault("PunchThroughTool"                 , getPublicTool('ISF_PunchThroughTool')             )
    kwargs.setdefault("DoPunchThroughSimulation"         , False                                             )
    kwargs.setdefault("ParticleBroker"                   , getService('ISF_ParticleBrokerSvc')               )
    kwargs.setdefault("CaloCellMakerTools_setup"         , [
                                                             getPublicTool('ISF_EmptyCellBuilderTool'),
                                                           ])
    kwargs.setdefault("CaloCellMakerTools_simulate"      , [
                                                             getPublicTool('ISF_PileupFastShowerCellBuilderTool'),
                                                           ])
    kwargs.setdefault("CaloCellMakerTools_release"       , [
                                                             #getPublicTool('ISF_AddNoiseCellBuilderTool'),
                                                             getPublicTool('ISF_CaloCellContainerFinalizerTool'),
                                                             getPublicTool('ISF_FastHitConvertTool')
                                                           ])
    # let the ISF FCS flags know that FCS is being used
    ISF_FastCaloSimFlags.FastCaloSimIsActive.set_Value_and_Lock(True)

    # register the FastCaloSim random number streams
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    simFlags.RandomSeedList.addSeed( ISF_FastCaloSimFlags.RandomStreamName(), 98346412, 12461240 )

    from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConf import ISF__FastCaloSimSvc
    return ISF__FastCaloSimSvc(name, **kwargs )


#### Legacy FastCaloSim
def getLegacyAFIIFastCaloSimSvc(name="ISF_LegacyAFIIFastCaloSimSvc", **kwargs):
    from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
    kwargs.setdefault("BatchProcessMcTruth" , True )
    return getFastCaloSimSvc(name, **kwargs)

def getFastHitConvAlgFastCaloSimSvc(name="ISF_FastHitConvAlgFastCaloSimSvc",**kwargs):
    kwargs.setdefault("CaloCellMakerTools_release", [
                                                           #getPublicTool('ISF_AddNoiseCellBuilderTool'),
                                                            getPublicTool('ISF_CaloCellContainerFinalizerTool')
                                                    ] )
    # setup FastCaloSim hit converter and add it to the alg sequence:
    # -> creates HITS from reco cells
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence=AlgSequence()
    topSequence+=getAlgorithm('ISF_FastHitConvAlg')
    return getFastCaloSimSvc(name,**kwargs)

def getFastHitConvAlgLegacyAFIIFastCaloSimSvc(name="ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc", **kwargs):
    from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
    kwargs.setdefault("BatchProcessMcTruth" , True )
    return getFastHitConvAlgFastCaloSimSvc(name, **kwargs)

def getFastHitConvAlg(name="ISF_FastHitConvAlg", **kwargs):
    from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
    kwargs.setdefault("CaloCellsInputName"  , ISF_FastCaloSimFlags.CaloCellsName() )
    # TODO: do we need this?
    #from AthenaCommon.DetFlags import DetFlags
    #if DetFlags.pileup.LAr_on() or DetFlags.pileup.Tile_on():
    #  kwargs.setdefault("doPileup", True)
    #else:
    #  kwargs.setdefault("doPileup", False)
    from FastCaloSimHit.FastCaloSimHitConf import FastHitConv
    return FastHitConv(name, **kwargs )

