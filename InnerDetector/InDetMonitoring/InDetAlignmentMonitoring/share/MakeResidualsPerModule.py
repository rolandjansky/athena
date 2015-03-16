IBLStave = []

nstaves = 14
nrings = 20
for stave in range(nstaves):
    for ring in range(nrings):
        logicalRing = ring-10
        IBLStaveTemp = MakeModuleResPlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", rootFiles, nFiles, True, 
                                            stave+1, logicalRing)
        IBLStave.append(IBLStaveTemp)
        print IBLStave[ring] 
        DrawPlots(IBLStave[ring], outputDir+"/"+"IBL_xRESvsETA_stave"+str(stave)+"."+oFext, "IBL stave "+str(stave), "Average local x residual [mm]",
        "ring along stave [#eta]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.65,0.89, False)

    DrawResPerStave(IBLStave,outputDir+"/"+"IBL_xRESvsETA_stave"+str(stave)+"."+oFext, nFiles)
    
