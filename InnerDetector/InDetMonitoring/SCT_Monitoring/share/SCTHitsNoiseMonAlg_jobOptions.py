#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTHitNoiseMonAlg_jobOptions.py
@author Ken Kreul
@date 2019-10-03
@brief Old style configuration of SCTHitNoiseMonAlg
'''

def Title( i, isub):
    m_element = i
    m_layerStr = i / 2
    m_sideStr = i % 2
    m_region = isub
    if m_region == 1 :
        return "Layer " + str(m_layerStr) + " Side " + str(m_sideStr)
    else: 
        return "Disk " + str(m_layerStr) + " Side " + str(m_sideStr)
  


from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "SCTHitsNoiseMonitor")

from SCT_Monitoring.SCT_MonitoringConf import SCTHitsNoiseMonAlg
myMonAlg = helper.addAlgorithm(SCTHitsNoiseMonAlg, "SCTHitsNoiseMonAlg")

myMonAlg.TriggerChain = ''



# Add a generic monitoring tool (a "group" in old language). The returned 
# object here is the standard GenericMonitoringTool.
N_REGIONS = 3
dimension = [N_REGIONS]

MonGroupArray = helper.addArray(dimension,myMonAlg,"SCTHitsNoiseMonitor","SCT") #This puts SCTHitsNoiseMonitor_3 on index 0 !!



myMonGroupGeneral = helper.addGroup(
    myMonAlg,
    "SCTHitsNoiseMonitorGeneral",
    "SCT/GENERAL/"
)

### STEP 5 ###
# Configure histograms
#####

N_REGIONS = 3
N_DISKS    =   9
N_BARRELS  =   4
NBINS_LBs = 3000

N_ETA_BINS =  13
FIRST_ETA_BIN = -6
LAST_ETA_BIN  = 6
N_ETA_BINS_EC =   3 
FIRST_ETA_BIN_EC =  0
LAST_ETA_BIN_EC = N_ETA_BINS_EC-FIRST_ETA_BIN_EC-1
n_layers = [N_DISKS, N_BARRELS, N_DISKS, 2 * N_DISKS + N_BARRELS]
n_etabins = [N_ETA_BINS_EC, N_ETA_BINS, N_ETA_BINS_EC]
f_etabin = [FIRST_ETA_BIN_EC, FIRST_ETA_BIN, FIRST_ETA_BIN_EC]
l_etabin = [LAST_ETA_BIN_EC, LAST_ETA_BIN, LAST_ETA_BIN_EC]

N_PHI_BINS_EC =  52
FIRST_PHI_BIN_EC =    0
LAST_PHI_BIN_EC = N_PHI_BINS_EC-FIRST_PHI_BIN_EC-1
  
N_PHI_BINS =  56
FIRST_PHI_BIN =  0
LAST_PHI_BIN  = N_PHI_BINS-FIRST_PHI_BIN-1


n_phibins = [N_PHI_BINS_EC, N_PHI_BINS, N_PHI_BINS_EC]
f_phibin = [FIRST_PHI_BIN_EC, FIRST_PHI_BIN, FIRST_PHI_BIN_EC]
l_phibin = [LAST_PHI_BIN_EC, LAST_PHI_BIN, LAST_PHI_BIN_EC]


abbreviations = ["ECp", "", "ECm"]
names = ["Endcap A", "Barrel", "Endcap C"]
path = ["SCTEA", "SCTB", "SCTEC"]
noiseAbbreviations = ["ECA","BAR","ECC"]
titleAbbreviations = ["ECp","BAR","ECm"]
layerDisk = [ "layer","disk","layer"]
limits = [ N_DISKS*2, N_BARRELS*2, N_DISKS*2 ]
nBins = 8
tbinsNames = ["000", "001", "010", "011", "100", "101", "110", "111"]
for isub in range(N_REGIONS):
    for i in range(limits[isub]):

  
        HitsMapName = "hitsmap" + abbreviations[isub] +  "_" + str(i/2) + "_" + str(i%2)
        HitsMapTitle = "SCT Hitmap for " + names[isub] + ": " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+HitsMapName+",phi_"+HitsMapName+";"+HitsMapName,
                    type= "TH2F", 
                    title= HitsMapTitle + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/hits",
                    xbins=n_etabins[isub], xmin=f_etabin[isub]-0.5, xmax=l_etabin[isub]+0.5,
                    ybins=n_phibins[isub], ymin=f_phibin[isub]-0.5 , ymax=l_phibin[isub]+0.5,
                    weight="numberOfStrips_"+HitsMapName )
                    
                    
        streamhitmap = "mapsOfHitsOnTracks" + abbreviations[isub] + "_" + "trackhitsmap_" + str(i/2) + "_" + str(i%2)
        histotitle = "SCT hits on tracks for " + names[isub] + " " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+streamhitmap + ",phi_"+streamhitmap + ";"+streamhitmap,
                    type= "TH2F", 
                    title= histotitle + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/hits/mapsOfHitsOnTracks/",
                    xbins=n_etabins[isub], xmin=f_etabin[isub]-0.5, xmax=l_etabin[isub]+0.5,
                    ybins=n_phibins[isub], ymin=f_phibin[isub]-0.5 , ymax=l_phibin[isub]+0.5 )
                    
        streamhitmapR = "hitoccupancymap" + abbreviations[isub] + "_" + str(i/2) + "_" + str(i%2)
        histotitleR  = "SCT Hit Occupancy map for " + names[isub] + ": " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+streamhitmapR + ",phi_"+streamhitmapR + ",occ_"+streamhitmapR+";" + streamhitmapR,
                    type= "TProfile2D", 
                    title= histotitleR + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/Noise",
                    xbins=n_etabins[isub], xmin=f_etabin[isub]-0.5, xmax=l_etabin[isub]+0.5,
                    ybins=n_phibins[isub], ymin=f_phibin[isub]-0.5, ymax=l_phibin[isub]+0.5 ) #filled in postproccesing, right now filled with dummy
        
        noiseoccupancy = "noiseoccupancymaptrigger" + abbreviations[isub] + "_" + str(i/2) + "_" + str(i%2)
        m_NOTriggerItem =  "L1_RD0_EMPTY"
        histotitletrigger = "SCT Noise Occupancy map for " + m_NOTriggerItem + " Trigger and " + names[isub] + ": " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+noiseoccupancy + ",phi_"+noiseoccupancy + ",occ_"+noiseoccupancy+";" + noiseoccupancy,
                    type= "TProfile2D", 
                    title= histotitletrigger + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/Noise",
                    xbins=n_etabins[isub], xmin=f_etabin[isub]-0.5, xmax=l_etabin[isub]+0.5,
                    ybins=n_phibins[isub], ymin=f_phibin[isub]-0.5, ymax=l_phibin[isub]+0.5 ) #filled in postproccesing, right now filled with dummy
        
        #End i Loop
    
    
        
    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB_HO_vsLB,HO_HO_vsLB;"+ noiseAbbreviations[isub] + "HO_vsLB",
                type= "TProfile", 
                title= "HO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB_HOTrigger_vsLB,HO_HOTrigger_vsLB;"+ noiseAbbreviations[isub] + "HOTrigger_vsLB",
                type= "TProfile", 
                title= "HO with trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB_NO_vsLB,NO_NO_vsLB;"+ noiseAbbreviations[isub] + "NO_vsLB",
                type= "TProfile", 
                title= "NO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB_NOTrigger_vsLB,NO_NOTrigger_vsLB;"+ noiseAbbreviations[isub] + "NOTrigger_vsLB",
                type= "TProfile", 
                title= "NO with Trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "lbh_HitsTrigger_vsLB,numberOfHitsFromSPsh_HSPHitsTrigger_vsLB;" + "h_HSPHitsTrigger"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of SP Hits in " + titleAbbreviations[isub] + " with trigger vs LB" + ";LumiBlock;Average number of SP Hits",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5)

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "lbh_Hits_vsLB,numberOfHitsFromSPsh_HSPHits_vsLB;" + "h_HSPHits"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of SP Hits in " + titleAbbreviations[isub] + " vs LB" + ";LumiBlock;Average number of SP Hits",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5)

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "lbh_HitsTrigger_vsLB,numhitsh_HallHitsTrigger_vsLB;" + "h_HallHitsTrigger"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of all Hits in " + titleAbbreviations[isub] + " with trigger vs LB" + ";LumiBlock;Average number of SP Hits",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5)

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "lbh_Hits_vsLB,numhitsh_HallHits_vsLB;" + "h_HallHits"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of all Hits in " + titleAbbreviations[isub] + " vs LB" + ";LumiBlock;Average number of SP Hits",
                path= path[isub] + "/Noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5)

#GENERAL
#clu_size
myMonGroupGeneral.defineHistogram(varname= "clu_size",
                type= "TH1F", 
                title= "SCT Cluster Size" + ";Cluster Size;Num of Events",
                path=  "/hits",
                xbins=200, xmin = 0, xmax = 200)

    
myMonGroupGeneral.defineHistogram(varname= "LB_HO_vsLB,HO_HO_vsLB;"+ "HO_vsLB",
                type= "TProfile", 
                title= "HO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= "/noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

myMonGroupGeneral.defineHistogram(varname= "LB_HOTrigger_vsLB,HO_HOTrigger_vsLB;"+  "HOTrigger_vsLB",
                type= "TProfile", 
                title= "HO with trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path=  "/noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
myMonGroupGeneral.defineHistogram(varname= "LB_NO_vsLB,NO_NO_vsLB;"+ "NO_vsLB",
                type= "TProfile", 
                title= "NO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= "/noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
myMonGroupGeneral.defineHistogram(varname= "LB_NOTrigger_vsLB,NO_NOTrigger_vsLB;"+ "NOTrigger_vsLB",
                type= "TProfile", 
                title= "NO with Trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= "/noise",
                xbins=NBINS_LBs, xmin = 0.5, xmax = NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

myMonGroupGeneral.defineHistogram(varname= "Bec_TBinFracAll,TBin_TBinFracAll;" + "TBinFracAll",
                type= "TProfile", 
                title= "fraction of 01X for each region" + "; ;Fraction of 01X",
                path= "/tbin",
                xbins= N_REGIONS, xmin = 0., xmax = N_REGIONS,
                xlabels= names)
    
    



topSequence += helper.result()

