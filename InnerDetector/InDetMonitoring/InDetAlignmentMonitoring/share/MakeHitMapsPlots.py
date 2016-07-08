#
print " <MakeResidualPlots.MakeHitMapsPlots> userCollectOutput=",userCollectOutput
print "                                      userWebMonitoring=",userWebMonitoring

psname = "/Users/martis/projectes/atlas/alineament/webmonitoringtest/InDetAlignmentWebMonitor/trunk/WebPage/detailed_plots/2016/HitMaps.pdf"
if (userWebMonitoring):
    psname = "/var/vhost/atlas-alignment/secure/detailed_plots/2016/HitMaps.pdf"
#

def initPsFile():
    NullCanvas = TCanvas()
    NullCanvas.Print(psname+"[")
    return NullCanvas
	
def closePsFile(NullCanvas):
    NullCanvas.Print(psname+"]")

Canvases = []
if (userCollectOutput):    
    NullCanvas = initPsFile()

    
for i in range(nFiles):    

    PIXBAR_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "measurements")
    outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_HitMaps_File_"+str(i)+"."+oFext
    outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(PIXBAR_HitMaps, outputFileName, "#eta ring", "#phi stave", "Hits", 
                          0.2, 0.96, "#mum",canvasText,makeOutput)
    if (userCollectOutput): Canvases.append(thiscan)
    
    PIXECA_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "ECA")
    outputFileName =  outputDir+"/"+userPrefix+"PIXECA_HitMaps_File_"+str(i)+"."+oFext
    outputFileName =  outputDir+"/"+userPrefix+"PIXECA_HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(PIXECA_HitMaps, outputFileName, "", "", "Hits", 
                          0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECA")
    if (userCollectOutput): Canvases.append(thiscan)

    PIXECC_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "ECC")
    outputFileName =  outputDir+"/"+userPrefix+"PIXECC_HitMaps_File_"+str(i)+"."+oFext
    outputFileName =  outputDir+"/"+userPrefix+"PIXECC_HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")     
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(PIXECC_HitMaps, outputFileName, "", "", "Hits", 
                          0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECC")
    if (userCollectOutput): Canvases.append(thiscan)

    # SCT barrel side 0 and side 1
    SCTBAR_s0HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "SCT", "BAR", True, 0)
    outputFileName =  outputDir+"/"+userPrefix+"SCTBAR_s0HitMaps_File_"+str(i)+"."+oFext
    outputFileName =  outputDir+"/"+userPrefix+"SCTBAR_s0HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(SCTBAR_s0HitMaps, outputFileName, "#eta ring", "#phi sector", "Hits", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR")
    if (userCollectOutput): Canvases.append(thiscan)

    SCTBAR_s1HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "BAR", True, 1)
    outputFileName =  outputDir+"/"+userPrefix+"SCTBAR_s1HitMaps_File_"+str(i)+"."+oFext
    outputFileName =  outputDir+"/"+userPrefix+"SCTBAR_s1HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(SCTBAR_s1HitMaps, outputFileName, "#eta ring", "#phi sector", "Hits", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR")
    if (userCollectOutput): Canvases.append(thiscan)
    
    # SCT ECA side 0 and sie 1
    SCTECA_s0HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECA", True, 0)
    outputFileName = outputDir+"/"+userPrefix+"SCTECA_s0HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(SCTECA_s0HitMaps, outputFileName, "", "", "Hits", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")
    if (userCollectOutput): Canvases.append(thiscan)
    
    SCTECA_s1HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECA", True, 1)
    outputFileName = outputDir+"/"+userPrefix+"SCTECA_s1HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(SCTECA_s1HitMaps, outputFileName, "", "", "Hits", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")
    if (userCollectOutput): Canvases.append(thiscan)
    
    # SCT ECC side 0 and sie 1
    outputFileName = outputDir+"/"+userPrefix+"SCTECC_s0HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    SCTECC_s0HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECC", True, 0)
    thiscan = DrawHitMaps(SCTECC_s0HitMaps, outputFileName, "", "", "Hits", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")
    if (userCollectOutput): Canvases.append(thiscan)
        
    outputFileName = outputDir+"/"+userPrefix+"SCTECC_s1HitMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    SCTECC_s1HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECC", True, 1)
    thiscan = DrawHitMaps(SCTECC_s1HitMaps, outputFileName, "", "", "Hits", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")
    if (userCollectOutput): Canvases.append(thiscan)

    #
    # HIT EFFICIENCY PER MODULE AND PLANE
    #
    # pixel barrel hit efficiency map
    PIXBAR_HitEffMaps = MakeHitEffMaps(hitEffDir, legendTitles, rootFiles, i, "PIX", "BAR", True, 0)
    outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_HitEffMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(PIXBAR_HitEffMaps, outputFileName, "#eta ring", "#phi sector", "Hit eff.", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"PIX","BAR", 3)
    if (userCollectOutput): Canvases.append(thiscan)
    
    # SCT barrel side 0 and side 1
    SCTBAR_s0HitEffMaps = MakeHitEffMaps(hitEffDir, legendTitles, rootFiles, i, "SCT", "BAR", True, 0)
    outputFileName =  outputDir+"/"+userPrefix+"SCTBAR_s0HitEffMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(SCTBAR_s0HitEffMaps, outputFileName, "#eta ring", "#phi sector", "Hit eff.", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR", 3)
    if (userCollectOutput): Canvases.append(thiscan)

    SCTBAR_s1HitEffMaps = MakeHitEffMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "BAR", True, 1)
    outputFileName =  outputDir+"/"+userPrefix+"SCTBAR_s1HitEffMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    thiscan = DrawHitMaps(SCTBAR_s1HitEffMaps, outputFileName, "#eta ring", "#phi sector", "Hit eff.", 
                          0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR", 3)
    if (userCollectOutput): Canvases.append(thiscan)

    #
    # Extended
    #    
    if (userExtended):
        print " <MakeHitMapsPlots> Extended = True "
        PIXBAR_ExpectedHitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "hits")
        outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_ExpectedHitMaps_File_"+str(i)+"."+oFext
        outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_ExpectedHitMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXBAR_ExpectedHitMaps, outputFileName, "#eta ring", "#phi stave", "Expected hits", 
                    0.2, 0.96, "#mum",canvasText,makeOutput)
        
        PIXBAR_HoleMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "holes")
        outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_HoleMaps_File_"+str(i)+"."+oFext
        outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_HoleMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXBAR_HoleMaps, outputFileName, "#eta ring", "#phi stave", "Holes", 
                    0.2, 0.96, "#mum",canvasText,makeOutput)
        
        PIXBAR_OutliersMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "outliers")
        outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_OutlierMaps_File_"+str(i)+"."+oFext
        outputFileName =  outputDir+"/"+userPrefix+"PIXBAR_OutlierMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXBAR_OutliersMaps, outputFileName, "#eta ring", "#phi stave", "Outliers", 
                    0.2, 0.96, "#mum",canvasText,makeOutput)

if (userCollectOutput):
    print " <MakeResidualPlots> PsFile = ",psname
    for canvas in Canvases:
        print "  <MakeResidualPlots> storing canvas: ", canvas 
        PsFile = canvas.Print(psname)
    closePsFile(NullCanvas)
        
# reset the plots style
execfile("AtlasStyle.py")
