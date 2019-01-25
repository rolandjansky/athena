# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF_FastCaloSimServices
KG Tan, 04/12/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon import CfgMgr
from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getAdditionalParticleParametrizationFileNames():
    return [
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
# definitions taken from file:
#   FastSimulationJobTransforms/jobConfig.FastCaloSim_DB.py
def setAdditionalParticleParametrizationFileNames( FastShowerCellBuilderTool ):
    FastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames = getAdditionalParticleParametrizationFileNames()

def getPunchThroughTool(name="ISF_PunchThroughTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
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
    kwargs.setdefault("BarcodeSvc"              , simFlags.TruthStrategy.BarcodeServiceName()        )
    kwargs.setdefault("EnvelopeDefSvc"          , getService('AtlasGeometry_EnvelopeDefSvc')         )
    kwargs.setdefault("BeamPipeRadius"          , 500.                                               )

    from ISF_PunchThroughTools.ISF_PunchThroughToolsConf import ISF__PunchThroughTool
    return ISF__PunchThroughTool(name, **kwargs )

def getEmptyCellBuilderTool(name="ISF_EmptyCellBuilderTool", **kwargs):
    from FastCaloSim.FastCaloSimConf import EmptyCellBuilderTool
    return EmptyCellBuilderTool(name, **kwargs )

def getNIMatEffUpdator(name="ISF_NIMatEffUpdator", **kwargs):
    from TrkExTools.TrkExToolsConf import Trk__NIMatEffUpdator as NIMatEffUpdator
    return NIMatEffUpdator(name, **kwargs )

def getNIPropagator(name="ISF_NIPropagator", **kwargs):
    kwargs.setdefault("MaterialEffects" , False )

    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as STEP
    return STEP(name, **kwargs )

def getNITimedExtrapolator(name="ISF_NITimedExtrapolator", **kwargs):
    kwargs.setdefault("MaterialEffectsUpdators" , [ 'ISF_NIMatEffUpdator' ])
    kwargs.setdefault("ApplyMaterialEffects"    , False )
    kwargs.setdefault("STEP_Propagator"    , 'ISF_NIPropagator' )

    from TrkExTools.TrkExToolsConf import Trk__TimedExtrapolator as TimedExtrapolator
    return TimedExtrapolator(name, **kwargs )

def getTimedExtrapolator(name="TimedExtrapolator", **kwargs):
    kwargs.setdefault("MaterialEffectsUpdators" , [ 'ISF_NIMatEffUpdator' ])
    kwargs.setdefault("ApplyMaterialEffects"    , False )
    kwargs.setdefault("STEP_Propagator"    , 'ISF_NIPropagator' )

    from TrkExTools.TrkExToolsConf import Trk__TimedExtrapolator as TimedExtrapolator
    return TimedExtrapolator(name, **kwargs )
## FastShowerCellBuilderTool

def getDefaultFastShowerCellBuilderTool(name, **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from ISF_FastCaloSimServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
    kwargs.setdefault("RandomService", simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName", ISF_FastCaloSimFlags.RandomStreamName() )
    kwargs.setdefault("DoSimulWithInnerDetectorTruthOnly", True )
    kwargs.setdefault("ID_cut_off_r",                      [1150] )
    kwargs.setdefault("ID_cut_off_z",                      [ 3490 ] )
    kwargs.setdefault("DoSimulWithInnerDetectorV14TruthCuts", True )
    kwargs.setdefault("DoNewEnergyEtaSelection",              True )
    kwargs.setdefault("DoEnergyInterpolation",                True )
    kwargs.setdefault("use_Ekin_for_depositions",             True )
    kwargs.setdefault("McLocation",                           ISF_FastCaloSimFlags.FastShowerInputCollection() )
    kwargs.setdefault("ParticleParametrizationFileName",      '')
    kwargs.setdefault("Extrapolator",                         'ISF_NITimedExtrapolator' )
    from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory
    return FastCaloSimFactory(name, **kwargs)

def getFastShowerCellBuilderTool(name="ISF_FastShowerCellBuilderTool", **kwargs):
    localFileNameList = getAdditionalParticleParametrizationFileNames()
    localFileNameList.insert(0,"L1_L2_egamma_corr.config20.root")
    kwargs.setdefault('AdditionalParticleParametrizationFileNames', localFileNameList)
    from IOVDbSvc.CondDB import conddb
    conddb.addFolder('GLOBAL_OFL','/GLOBAL/AtlfastII/FastCaloSimParam <tag>FastCaloSim_v2</tag>')
    return getDefaultFastShowerCellBuilderTool(name, **kwargs)

def getLegacyFastShowerCellBuilderTool(name="ISF_LegacyFastShowerCellBuilderTool", **kwargs):
    FastShowerCellBuilderTool = getFastShowerCellBuilderTool(name,**kwargs)
    FastShowerCellBuilderTool.Invisibles += [ 13 ]
    return FastShowerCellBuilderTool

def getPileupFastShowerCellBuilderTool(name="ISF_PileupFastShowerCellBuilderTool", **kwargs):
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

    kwargs.setdefault("sampling_energy_reweighting", weightsfcs )
    return getFastShowerCellBuilderTool(name, **kwargs)

def getFastHitConvertTool(name="ISF_FastHitConvertTool", **kwargs):
    mergeable_collection_suffix = "_FastCaloSim"

    EMB_hits_bare_collection_name = "LArHitEMB"
    EMB_hits_merger_input_property = "LArEMBHits"
    EMB_hits_collection_name = generate_mergeable_collection_name(
        EMB_hits_bare_collection_name,
        mergeable_collection_suffix,
        EMB_hits_merger_input_property)

    EMEC_hits_bare_collection_name = "LArHitEMEC"
    EMEC_hits_merger_input_property = "LArEMECHits"
    EMEC_hits_collection_name = generate_mergeable_collection_name(
        EMEC_hits_bare_collection_name,
        mergeable_collection_suffix,
        EMEC_hits_merger_input_property)

    FCAL_hits_bare_collection_name = "LArHitFCAL"
    FCAL_hits_merger_input_property = "LArFCALHits"
    FCAL_hits_collection_name = generate_mergeable_collection_name(
        FCAL_hits_bare_collection_name,
        mergeable_collection_suffix,
        FCAL_hits_merger_input_property)

    HEC_hits_bare_collection_name = "LArHitHEC"
    HEC_hits_merger_input_property = "LArHECHits"
    HEC_hits_collection_name = generate_mergeable_collection_name(
        HEC_hits_bare_collection_name,
        mergeable_collection_suffix,
        HEC_hits_merger_input_property)

    tile_hits_bare_collection_name = "TileHitVec"
    tile_hits_merger_input_property = "TileHits"
    tile_hits_collection_name = generate_mergeable_collection_name(
        tile_hits_bare_collection_name,
        mergeable_collection_suffix,
        tile_hits_merger_input_property)

    kwargs.setdefault('embHitContainername', EMB_hits_collection_name)
    kwargs.setdefault('emecHitContainername', EMEC_hits_collection_name)
    kwargs.setdefault('fcalHitContainername', FCAL_hits_collection_name)
    kwargs.setdefault('hecHitContainername', HEC_hits_collection_name)
    kwargs.setdefault('tileHitContainername', tile_hits_collection_name)
    
    from FastCaloSimHit.FastCaloSimHitConf import FastHitConvertTool
    return FastHitConvertTool(name,**kwargs)

def getCaloNoiseTool(name="ISF_FCS_CaloNoiseTool", **kwargs):
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    return CaloNoiseToolDefault(name, **kwargs )

def getAddNoiseCellBuilderTool(name="ISF_AddNoiseCellBuilderTool", **kwargs):
    kwargs.setdefault("CaloNoiseTool" , getPublicTool('ISF_FCS_CaloNoiseTool').getFullName())

    from FastCaloSim.FastCaloSimConf import AddNoiseCellBuilderTool
    return AddNoiseCellBuilderTool(name, **kwargs )

def getCaloCellContainerFCSFinalizerTool(name="ISF_CaloCellContainerFCSFinalizerTool", **kwargs):
    from FastCaloSim.FastCaloSimConf import CaloCellContainerFCSFinalizerTool
    return CaloCellContainerFCSFinalizerTool(name, **kwargs )

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
