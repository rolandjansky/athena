#
# MakeHitPlots.py
#

print " <MakeResidualPlots.MakeResidualPlots> userCollectOutput=",userCollectOutput
print "                                       userWebMonitoring=",userWebMonitoring
#
psname = "/Users/martis/projectes/atlas/alineament/webmonitoringtest/InDetAlignmentWebMonitor/trunk/WebPage/detailed_plots/2016/PlotsOfHits.pdf"
if (userWebMonitoring):
    psname = "/var/vhost/atlas-alignment/secure/detailed_plots/2016/PlotsOfHits.pdf"

def initPsFile():
    NullCanvas = TCanvas()
    NullCanvas.Print(psname+"[")
    return NullCanvas
	
def closePsFile(NullCanvas):
    NullCanvas.Print(psname+"]")

Canvases = []
if (userCollectOutput):    
    NullCanvas = initPsFile()

##########################
TrackNPIXHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
thiscan = DrawPlots(TrackNPIXHits, outputDir+"/"+userPrefix+"nPIXHits."+oFext, "", "N Tracks",
                    "N PIX Hits", 0.69, 0.6, "",
                    canvasText,makeOutput)
if (userCollectOutput): Canvases.append(thiscan)

TrackNSCTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
thiscan = DrawPlots(TrackNSCTHits, outputDir+"/"+userPrefix+"nSCTHits."+oFext, "", "N Tracks",
                    "N SCT Hits", 0.69, 0.6, "",
                    canvasText,makeOutput)
if (userCollectOutput): Canvases.append(thiscan)

TrackNTRTHits = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Ntrthits_per_track","noFit",rootFiles,nFiles,normaliseHistos)
thiscan = DrawPlots(TrackNTRTHits, outputDir+"/"+userPrefix+"nTRTHits."+oFext, "TRT hits on track", "N Tracks",
                    "N TRT Hits", 0.18, 0.68, "",
                    canvasText,makeOutput, 0.15,0.88)
if (userCollectOutput): Canvases.append(thiscan)

TrackNSCTHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsBarrel, outputDir+"/"+userPrefix+"nSCTHitsBarrel."+oFext, "", "N Tracks",
          "N SCT Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEca, outputDir+"/"+userPrefix+"nSCTHitsEca."+oFext, "", "N Tracks",
          "N SCT Hits Eca", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNSCTHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Nscthits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNSCTHitsEcc, outputDir+"/"+userPrefix+"nSCTHitsEcc."+oFext, "", "N Tracks",
          "N SCT Hits Ecc", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsBarrel = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_barrel","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsBarrel, outputDir+"/"+userPrefix+"nPIXHitsBarrel."+oFext, "", "N Tracks",
          "N PIX Hits Barrel", 0.69, 0.6, "",
          canvasText,makeOutput)

TrackNPIXHitsEca = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_eca","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEca, outputDir+"/"+userPrefix+"nPIXHitsEca."+oFext, "", "N Tracks",
          "N PIX Hits ECA", 0.69, 0.6, "",
          canvasText,makeOutput)


TrackNPIXHitsEcc = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"Npixhits_per_track_ecc","noFit",rootFiles,nFiles,normaliseHistos)
DrawPlots(TrackNPIXHitsEcc, outputDir+"/"+userPrefix+"nPIXHitsEcc."+oFext, "", "N Tracks",
          "N PIX Hits ECC", 0.69, 0.6, "",
          canvasText,makeOutput)

# Hit Efficiencies
SiBarrelHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_barrel","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelHitEff, outputDir+"/"+userPrefix+"SiBarrelHitEff."+oFext, "Si Barrel Hit Eff.", "Hit Efficiency",
          "Barrel Layer", 0.53, 0.37, "",
          canvasText,makeOutput,0.50,0.80,False)

SiBarrelSeenHits = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_vs_layer_barrel","noFit",rootFiles,nFiles,False) 
DrawPlots(SiBarrelSeenHits, outputDir+"/"+userPrefix+"SiBarrelSeenHits."+oFext, "Si Barrel measurements", "Seen hits",
          "Barrel Layer", 0.20, 0.87, "",
          canvasText,makeOutput,0.55,0.41,True)

SiBarrelExpectedHits = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_vs_layer_barrel","noFit",rootFiles,nFiles,False) 
DrawPlots(SiBarrelExpectedHits, outputDir+"/"+userPrefix+"SiBarrelExpectedHits."+oFext, "Si Barrel expected hits", "Expected hits",
          "Barrel Layer", 0.20, 0.87, "",
          canvasText,makeOutput,0.55,0.41,True)

SiEcaHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_eca","noFit",rootFiles,nFiles,False)
DrawPlots(SiEcaHitEff, outputDir+"/"+userPrefix+"SiEcaHitEff."+oFext, "Si Endcap A Hit Eff.", "Hit Efficiency",
          "ECA Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

SiEccHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"measurements_eff_vs_layer_ecc","noFit",rootFiles,nFiles,False)
DrawPlots(SiEccHitEff, outputDir+"/"+userPrefix+"SiEccHitEff."+oFext, "Si Endcap C Hit Eff.", "Hit Efficiency",
          "ECC Disk", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTB0HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB0HitEff, outputDir+"/"+userPrefix+"TRTB0HitEff."+oFext, "TRT Barrel Layer 0 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 0 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTB1HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB1HitEff, outputDir+"/"+userPrefix+"TRTB1HitEff."+oFext, "TRT Barrel Layer 1 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 1 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

TRTB2HitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_phiSector_trt_b2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB2HitEff, outputDir+"/"+userPrefix+"TRTB2HitEff."+oFext, "TRT Barrel Layer 2 Hit Eff.", "Hit Efficiency",
          "Barrel Layer 2 Phi Sector", 0.19, 0.41, "",
          canvasText,makeOutput,0.60,0.41,False)

#TRTECAHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_wheel_trt_ea","noFit",rootFiles,nFiles,False)
#DrawPlots(TRTECAHitEff, outputDir+"/"+userPrefix+"TRTECAHitEff."+oFext, "TRT Endcap A Hit Eff.", "Hit Efficiency",
#          "Endcap A Wheel", 0.19, 0.41, "",
#          canvasText,makeOutput,0.60,0.41,False)

#TRTECCHitEff = MakePlots(hitEffDir,legendTitles,markerColors,markerStyles,"hits_eff_vs_wheel_trt_ec","noFit",rootFiles,nFiles,False)
#DrawPlots(TRTECCHitEff, outputDir+"/"+userPrefix+"TRTECCHitEff."+oFext, "TRT Endcap C Hit Eff.", "Hit Efficiency",
#          "Endcap C Wheel", 0.19, 0.41, "",
#          canvasText,makeOutput,0.60,0.41,False)



if (userExtended):
    PIXHits = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"PIXHitsvEta","noFit",rootFiles,nFiles, False)
    DrawPlots(PIXHits,outputDir+"/"+userPrefix+"NPIXvsETA.png","","Average PIX hits", "#eta",0.2,0.9,"", canvasText,makeOutput, 0.60, 0.91, False);
    SCTHits = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"SCTHitsvEta","noFit",rootFiles,nFiles, False)
    DrawPlots(SCTHits,outputDir+"/"+userPrefix+"NSCTvsETA.png","","Average SCT hits", "#eta",0.2,0.9,"", canvasText,makeOutput, 0.60, 0.91, False);
    TRTHits = MakeProfPlots (genericTracksDir,legendTitles,markerColors,markerStyles,"TRTHitsvEta","noFit",rootFiles,nFiles, False)
    DrawPlots(TRTHits,outputDir+"/"+userPrefix+"NTRTvsETA.png","","Average TRT hits", "#eta",0.2,0.9,"", canvasText,makeOutput, 0.60, 0.91, False);


# SivsTRT
if siTRTDir[0] != "":
    TRTExtEffEta = MakePlots(siTRTDir,legendTitles,markerColors,markerStyles,"eff_trthits_eta0","noFit",rootFiles,nFiles,False)
    DrawPlots(TRTExtEffEta, outputDir+"/"+userPrefix+"TRTExtEffVsEta."+oFext, "TRT Extension Eff.", "TRT Extension Eff.",
              "Track Eta", 0.19, 0.41, "",
              canvasText,makeOutput,0.60,0.41,False)

    TRTExtEffPhi = MakePlots(siTRTDir,legendTitles,markerColors,markerStyles,"eff_trthits_phi0","noFit",rootFiles,nFiles,False)
    DrawPlots(TRTExtEffPhi, outputDir+"/"+userPrefix+"TRTExtEffVsPhi."+oFext, "TRT Extension Eff.", "TRT Extension Eff.",
              "Track Phi", 0.19, 0.41, "",
              canvasText,makeOutput,0.60,0.41,False)

#    
NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"NPixPerLumiBlock","noFit",rootFiles,nFiles,False)
thiscan = DrawPlots(NTracks, outputDir+"/"+userPrefix+"NPixPerLumiBlock.png", "Pixel hits vs LumiBlock", "#PIXEL hits", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)
if (userCollectOutput): Canvases.append(thiscan)

NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"NSCTPerLumiBlock","noFit",rootFiles,nFiles,False)
thiscan = DrawPlots(NTracks, outputDir+"/"+userPrefix+"NSCTPerLumiBlock.png", "SCT hits vs LumiBlock", "#SCT hits", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)
if (userCollectOutput): Canvases.append(thiscan)
    
NTracks = MakePlots(genericTracksDir,legendTitles,markerColors,markerStyles,"NTRTPerLumiBlock","noFit",rootFiles,nFiles,False)
thiscan = DrawPlots(NTracks, outputDir+"/"+userPrefix+"NTRTPerLumiBlock.png", "TRT hits vs LumiBlock", "#TRT hits", "LumiBlock", 0.19, 0.9, "#mum", canvasText,makeOutput)
if (userCollectOutput): Canvases.append(thiscan)

if (userCollectOutput):
    print " <MakeHitPlots> PsFile = ",psname
    for canvas in Canvases:
        print "  <MakeHitPlots> storing canvas: ", canvas 
        PsFile = canvas.Print(psname)
    closePsFile(NullCanvas)

