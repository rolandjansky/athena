#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTHitEffMonAlg_jobOptions.py
@author Ken Kreul
@date 2019-10-03
@brief Old style configuration of SCTHitEffMonAlg
'''
def dedicatedTitle(i, isub):
    m_element = i
    m_layerStr = i / 2
    m_sideStr = i % 2
    m_region = isub
    if m_region == 1 :
        return "Layer " + str(m_layerStr) + " Side " + str((m_element % 2 + 1) % 2)
    else:
        return "Disk " + str(m_layerStr) + " Side " + str((m_element % 2 + 1) % 2)


from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "SCTHitEffMonitor")

from SCT_Monitoring.SCT_MonitoringConf import SCTHitEffMonAlg
myMonAlg = helper.addAlgorithm(SCTHitEffMonAlg, "SCTHitEffMonAlg")

myMonAlg.TriggerChain = ''

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
myMonAlg.BunchCrossingTool = BunchCrossingTool()


# Add a generic monitoring tool (a "group" in old language). The returned 
# object here is the standard GenericMonitoringTool.

# SCTEC     = 0
# SCTB      = 1
# SCTEA     = 2
N_REGIONS = 3
GENERAL   = 3

myMonGroup = [
    helper.addGroup(
        myMonAlg,
        "SCTHitEffMonitorEC",
        "SCT/SCTEC/"
    ),
    helper.addGroup(
        myMonAlg,
        "SCTHitEffMonitorB",
        "SCT/SCTB/"
    ),
    helper.addGroup(
        myMonAlg,
        "SCTHitEffMonitorEA",
        "SCT/SCTEA/"
    ),
    helper.addGroup(
        myMonAlg,
        "SCTHitEffMonitor",
        "SCT/GENERAL/"
    )
]

# Configure histograms
    
N_DISKS = 9
N_BARRELS = 4
N_ETA_BINS = 13
FIRST_ETA_BIN = -6
LAST_ETA_BIN  = 6
N_ETA_BINS_EC = 3 
FIRST_ETA_BIN_EC = 0
LAST_ETA_BIN_EC = N_ETA_BINS_EC-FIRST_ETA_BIN_EC-1
n_layers = [N_DISKS, N_BARRELS, N_DISKS, 2 * N_DISKS + N_BARRELS]
n_etaBins = [N_ETA_BINS_EC, N_ETA_BINS, N_ETA_BINS_EC]
f_etaBin = [FIRST_ETA_BIN_EC, FIRST_ETA_BIN, FIRST_ETA_BIN_EC]
l_etaBin = [LAST_ETA_BIN_EC, LAST_ETA_BIN, LAST_ETA_BIN_EC]

N_PHI_BINS_EC = 52
FIRST_PHI_BIN_EC = 0
LAST_PHI_BIN_EC = N_PHI_BINS_EC-FIRST_PHI_BIN_EC-1

N_PHI_BINS = 56
FIRST_PHI_BIN = 0
LAST_PHI_BIN = N_PHI_BINS-FIRST_PHI_BIN-1

n_phiBins = [N_PHI_BINS_EC, N_PHI_BINS, N_PHI_BINS_EC]
f_phiBin = [FIRST_PHI_BIN_EC, FIRST_PHI_BIN, FIRST_PHI_BIN_EC]
l_phiBin = [LAST_PHI_BIN_EC, LAST_PHI_BIN, LAST_PHI_BIN_EC]

NBINS_LBs = 3000
N_MOD_ENDCAPS = 988
N_MOD_BARREL = 2112
n_mod = [N_MOD_ENDCAPS, N_MOD_BARREL, N_MOD_ENDCAPS, N_MOD_BARREL + 2 * N_MOD_ENDCAPS]

layerName = [" disk ", " layer ", " disk "]
subDetName = ["Endcap C", "Barrel", "Endcap A"]
mapName = ["m_eff_", "eff_", "p_eff_"] 
ineffmapName = ["ineffm_", "ineff_", "ineffp_"] 

sumEff = ["summaryeffm", "summaryeff", "summaryeffp"]
sumEffTitle = ["Summary Module Efficiency in Endcap C",
               "Summary Module Efficiency in Barrel",
               "Summary Module Efficiency in Endcap A"]

limit = [N_DISKS*2, N_BARRELS*2, N_DISKS*2]

# GENERAL
myMonGroup[GENERAL].defineHistogram(varname= "isub, eff;" + "SctTotalEff",
                                    type= "TProfile",
                                    title= "SCT Total Efficiency",
                                    path="eff",
                                    xbins=N_REGIONS,
                                    xmin=0.,
                                    xmax=N_REGIONS,
                                    xlabels=subDetName)

myMonGroup[GENERAL].defineHistogram(varname= "isub, eff;" + "SctTotalEffBCID",
                                    type= "TProfile",
                                    title= "SCT Total Efficiency for First BCID",
                                    path="eff",
                                    xbins=N_REGIONS,
                                    xmin=0.,
                                    xmax=N_REGIONS,
                                    xlabels=subDetName,
                                    cutmask="isFirstBCID")

myMonGroup[GENERAL].defineHistogram(varname= "sideHash, eff;" + "effHashCode",
                                    type= "TProfile",
                                    title= "Efficiency vs module Hash code" + ";Module Hash Code;Efficiency",
                                    path="eff",
                                    xbins=n_mod[GENERAL] * 2,
                                    xmin=-0.5,
                                    xmax=n_mod[3] * 2 -0.5)
                
myMonGroup[GENERAL].defineHistogram(varname= "LumiBlock, eff;" + "effLumiBlock",
                                    type= "TProfile",
                                    title= "Efficiency vs Luminosity block"+";;Efficiency",
                                    path="eff",
                                    xbins=NBINS_LBs,
                                    xmin=0.5,
                                    xmax=NBINS_LBs + 0.5)

### This histogram should be filled by post processing ###
# myMonGroup[GENERAL].defineHistogram(varname= "eff;" + "SctEffDistribution",
#                                     type= "TH1F",
#                                     title= "SCT Efficiency Distribution"+";Efficiency;Links",
#                                     path="eff",
#                                     xbins= 500,
#                                     xmin=0.,
#                                     xmax=1.)

# SCTEC, SCTB, SCTEA
for isub in range(N_REGIONS):
    profileLabels = range(limit[isub])
    for k in range(limit[isub]):
        profileLabels[k] = dedicatedTitle(k, isub)
    # Efficiency
    myMonGroup[isub].defineHistogram(varname= "layerPlusHalfSide, eff;" + sumEff[isub],
                                    type= "TProfile",
                                     title= sumEffTitle[isub]+ ";;Efficiency",
                                     path="eff",
                                     xbins=2*n_layers[isub],
                                     xmin=0.,
                                     xmax=n_layers[isub],
                                     xlabels=profileLabels)
    # Efficiency for first BCIDs
    myMonGroup[isub].defineHistogram(varname= "layerPlusHalfSide, eff;" + sumEff[isub] + "BCID",
                                     type= "TProfile",
                                     title= sumEffTitle[isub]+" for First BC" + ";;Efficiency", 
                                     path="eff",
                                     xbins=2*n_layers[isub],
                                     xmin=0.,
                                     xmax=n_layers[isub],
                                     xlabels=profileLabels,
                                     cutmask="isFirstBCID")
    # Efficiency as a function of LB
    myMonGroup[isub].defineHistogram(varname= "LumiBlock, eff;"+"effLumiBlock", #different names for fill
                                     type= "TProfile",
                                     title= "Efficiency vs Luminosity block in "+subDetName[isub]+";Luminosity block"+";Efficiency",
                                     path="eff",
                                     xbins=NBINS_LBs,
                                     xmin=0.5,
                                     xmax=NBINS_LBs + 0.5)

    # Disks for SCTEC and SCTEA and layers for SCTB
    for layer_disk in range(n_layers[isub]):
        for side in range(2):
            etaPhiSuffix = "_" + str(layer_disk) + "_" + str(side)
            effName = mapName[isub] + str(layer_disk) + "_" + str(side)
            ineffName = ineffmapName[isub] + str(layer_disk) + "_" + str(side)
            # Efficiency
            myMonGroup[isub].defineHistogram(varname= "ieta" + etaPhiSuffix + ",iphi" + etaPhiSuffix + ",eff;" + effName,
                                             type= "TProfile2D",
                                             title="Hit efficiency of" + layerName[isub] + str(layer_disk) + " / side " + str(side) + " in " + subDetName[isub]+ ";Index in the direction of #eta;Index in the direction of #phi",
                                             path="eff",
                                             xbins=n_etaBins[isub], xmin=f_etaBin[isub] - .5, xmax=l_etaBin[isub] + .5,
                                             ybins=n_phiBins[isub], ymin=f_phiBin[isub] - .5, ymax=l_phiBin[isub] + .5)
            # Efficiency for first BCIDs
            myMonGroup[isub].defineHistogram(varname= "ieta" + etaPhiSuffix + ",iphi" + etaPhiSuffix + ",eff;" + effName + "_bcid",
                                             type= "TProfile2D",
                                             title="Hit efficiency of" + layerName[isub] + str(layer_disk) + " / side " + str(side) + " in " + subDetName[isub] +  " for first BCID" + ";Index in the direction of #eta;Index in the direction of #phi",
                                             path="eff",
                                             xbins=n_etaBins[isub], xmin=f_etaBin[isub] - .5, xmax=l_etaBin[isub] + .5,
                                             ybins=n_phiBins[isub], ymin=f_phiBin[isub] - .5, ymax=l_phiBin[isub] + .5,
                                             cutmask="isFirstBCID")
            # Inefficiency
            myMonGroup[isub].defineHistogram(varname= "ieta" + etaPhiSuffix + ",iphi" + etaPhiSuffix  + ",ineff;" + ineffName,
                                             type= "TProfile2D",
                                             title="Hit inefficiency of" + layerName[isub] + str(layer_disk) + " / side " + str(side) + " in " + subDetName[isub] + ";Index in the direction of #eta;Index in the direction of #phi",
                                             path="eff",
                                             xbins=n_etaBins[isub], xmin=f_etaBin[isub] - .5, xmax=l_etaBin[isub] + .5,
                                             ybins=n_phiBins[isub], ymin=f_phiBin[isub] - .5, ymax=l_phiBin[isub] + .5)

topSequence += helper.result()

