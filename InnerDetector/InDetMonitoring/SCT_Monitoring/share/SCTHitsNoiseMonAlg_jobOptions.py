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

# Trigger histogram will be made only for data.
doTrigger = False
if globalflags.DataSource == "data":
  from RecExConfig.RecFlags import rec
  if rec.doTrigger():
    doTrigger = True
myMonAlg.doTrigger = doTrigger

from ROOT import SCT_Monitoring as sctMon #import SCT_MonitoringNumbers.h

# Add a generic monitoring tool (a "group" in old language). The returned 
# object here is the standard GenericMonitoringTool.
dimension = [sctMon.N_REGIONS]

MonGroupArray = helper.addArray(dimension,myMonAlg,"SCTHitsNoiseMonitor","SCT") # SCTHitsNoiseMonitor_3 on index 0 !!

myMonGroupGeneral = helper.addGroup(
    myMonAlg,
    "SCTHitsNoiseMonitorGeneral",
    "SCT/GENERAL/"
)

# Configure histograms

abbreviations = ["ECp", "", "ECm"]
names = ["Endcap A", "Barrel", "Endcap C"]
path = ["SCTEA", "SCTB", "SCTEC"]
noiseAbbreviations = ["ECA","BAR","ECC"]
titleAbbreviations = ["ECp","BAR","ECm"]
layerDisk = [ "layer","disk","layer"]
limits = [ sctMon.N_DISKS*2, sctMon.N_BARRELS*2, sctMon.N_DISKS*2 ]
tbinsNames = ["000", "001", "010", "011", "100", "101", "110", "111"]

for isub in range(sctMon.N_REGIONS):
    for i in range(limits[isub]):

        HitsMapName = "hitsmap" + abbreviations[isub] +  "_" + str(i/2) + "_" + str(i%2)
        HitsMapTitle = "SCT Hitmap for " + names[isub] + ": " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+HitsMapName+",phi_"+HitsMapName+";"+HitsMapName,
                    type= "TH2F", 
                    title= HitsMapTitle + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/hits",
                    xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub]-0.5, xmax=sctMon.l_etabin[isub]+0.5,
                    ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub]-0.5 , ymax=sctMon.l_phibin[isub]+0.5,
                    weight="numberOfStrips_"+HitsMapName )
                    
                    
        streamhitmap = "mapsOfHitsOnTracks" + abbreviations[isub] + "_" + "trackhitsmap_" + str(i/2) + "_" + str(i%2)
        histotitle = "SCT hits on tracks for " + names[isub] + " " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+streamhitmap + ",phi_"+streamhitmap + ";"+streamhitmap,
                    type= "TH2F", 
                    title= histotitle + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/hits/mapsOfHitsOnTracks/",
                    xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub]-0.5, xmax=sctMon.l_etabin[isub]+0.5,
                    ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub]-0.5 , ymax=sctMon.l_phibin[isub]+0.5 )
                    
        occMap = "occupancymap" + abbreviations[isub] + "_" + str(i/2) + "_" + str(i%2)
        hitoccupancy = "hitoccupancymap" + abbreviations[isub] + "_" + str(i/2) + "_" + str(i%2)
        histotitleR  = "SCT Hit Occupancy map for " + names[isub] + ": " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+occMap + ",phi_"+occMap + ",HO_"+occMap+";" + hitoccupancy,
                    type= "TProfile2D", 
                    title= histotitleR + ";Index in the direction of #eta;Index in the direction of #phi",
                    path= path[isub] + "/Noise",
                    xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub]-0.5, xmax=sctMon.l_etabin[isub]+0.5,
                    ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub]-0.5, ymax=sctMon.l_phibin[isub]+0.5 ) #filled in postproccesing, right now filled with dummy
        
        noiseoccupancy = "noiseoccupancymaptrigger" + abbreviations[isub] + "_" + str(i/2) + "_" + str(i%2)
        m_NOTriggerItem =  "L1_RD0_EMPTY"
        histotitletrigger = "SCT Noise Occupancy map for " + m_NOTriggerItem + " Trigger and " + names[isub] + ": " + Title(i,isub)
        MonGroupArray.__getitem__(isub).defineHistogram(varname= "eta_"+occMap + ",phi_"+occMap + ",NO_"+occMap+";" + noiseoccupancy,
                    type= "TProfile2D", 
                    title= histotitletrigger + ";Index in the direction of #eta;Index in the direction of #phi",
                    cutmask= "IsSelectedTrigger_"+occMap,
                    path= path[isub] + "/Noise",
                    xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub]-0.5, xmax=sctMon.l_etabin[isub]+0.5,
                    ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub]-0.5, ymax=sctMon.l_phibin[isub]+0.5 ) #filled in postproccesing, right now filled with dummy
        
        #End i Loop
    
    
        
    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB,HO;"+ noiseAbbreviations[isub] + "HO_vsLB",
                type= "TProfile", 
                title= "HO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB,HO;"+ noiseAbbreviations[isub] + "HOTrigger_vsLB",
                type= "TProfile", 
                title= "HO with trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                cutmask= "IsSelectedTrigger",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB,NO;"+ noiseAbbreviations[isub] + "NO_vsLB",
                type= "TProfile", 
                title= "NO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LB,NO;"+ noiseAbbreviations[isub] + "NOTrigger_vsLB",
                type= "TProfile", 
                title= "NO with Trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                cutmask= "IsSelectedTrigger",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LBHits,numberOfHitsFromSPs;" + "h_HSPHitsTrigger"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of SP Hits in " + titleAbbreviations[isub] + " with trigger vs LB" + ";LumiBlock;Average number of SP Hits",
                cutmask= "isSelectedTriggerHits",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5)

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LBHits,numberOfHitsFromSPs;" + "h_HSPHits"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of SP Hits in " + titleAbbreviations[isub] + " vs LB" + ";LumiBlock;Average number of SP Hits",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5)

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LBHits,numberOfHitsFromAllRDOs;" + "h_HallHitsTrigger"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of all Hits in " + titleAbbreviations[isub] + " with trigger vs LB" + ";LumiBlock;Average number of SP Hits",
                cutmask= "isSelectedTriggerHits",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5)

    MonGroupArray.__getitem__(isub).defineHistogram(varname= "LBHits,numberOfHitsFromAllRDOs;" + "h_HallHits"+titleAbbreviations[isub]+"_vsLB",
                type= "TProfile", 
                title= "Average num of all Hits in " + titleAbbreviations[isub] + " vs LB" + ";LumiBlock;Average number of SP Hits",
                path= path[isub] + "/Noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5)

#GENERAL
myMonGroupGeneral.defineHistogram(varname= "clu_size",
                type= "TH1F", 
                title= "SCT Cluster Size" + ";Cluster Size;Num of Events",
                path=  "/hits",
                xbins=200, xmin = 0, xmax = 200)

    
myMonGroupGeneral.defineHistogram(varname= "LB,HO;"+ "HO_vsLB",
                type= "TProfile", 
                title= "HO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= "/noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

myMonGroupGeneral.defineHistogram(varname= "LB,HO;"+  "HOTrigger_vsLB",
                type= "TProfile", 
                title= "HO with trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                cutmask= "IsSelectedTrigger",
                path=  "/noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
myMonGroupGeneral.defineHistogram(varname= "LB,NO;"+ "NO_vsLB",
                type= "TProfile", 
                title= "NO vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                path= "/noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy
                
myMonGroupGeneral.defineHistogram(varname= "LB,NO;"+ "NOTrigger_vsLB",
                type= "TProfile", 
                title= "NO with Trigger vs LB for all region (SP noise)" + ";LumiBlock;Hit Occupancy [10^{-5}]",
                cutmask= "IsSelectedTrigger",
                path= "/noise",
                xbins=sctMon.NBINS_LBs, xmin = 0.5, xmax = sctMon.NBINS_LBs + 0.5) #filled in postproccesing, right now filled with dummy

myMonGroupGeneral.defineHistogram(varname= "Bec_TBinFracAll,TBin_TBinFracAll;" + "TBinFracAll",
                type= "TProfile", 
                title= "fraction of 01X for each region" + "; ;Fraction of 01X",
                path= "/tbin",
                xbins= sctMon.N_REGIONS, xmin = 0., xmax = sctMon.N_REGIONS,
                xlabels= names)
   
myMonGroupGeneral.defineHistogram(varname= "sct_hits",
                type= "TH1F",
                title= "Total SCT Hits;Total SCT Hits;Entries",
                path= "/hits/summary",
                xbins= sctMon.N_NOISE_HIT_BINS, xmin = sctMon.FIRST_NOISE_HIT_BIN, xmax = sctMon.LAST_NOISE_HIT_BIN)

topSequence += helper.result()

