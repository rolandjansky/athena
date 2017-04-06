#
doIBL = False
doSCT = True

if (doIBL):
    # IBL 
    IBLStave = []
    nstaves = [5] # 14 
    nrings = [5] # 20
    outputDirIBL = outputDir + '/ModulesIBL'
    if not os.path.isdir(outputDirIBL):
        print " -- MakeResidualsPerModule -- Producing output in new directory:", outputDirIBL
        os.mkdir(outputDirIBL)
        
    for stave in range(nstaves[0]):
        for ring in range(nrings[0]):
            logicalRing = ring-10
            IBLStaveTemp = MakeModuleResPlotsFrom3D (residualsDir, legendTitles, markerColors, markerStyles, "pix_b0_xresvsmodetaphi_3d", "noFit", 
                                                     rootFiles, nFiles, True, stave+1, logicalRing)
            IBLStave.append(IBLStaveTemp)
            #print " == IBLStave[stave=",stave," ring=",ring,"] = ",IBLStaveTemp 
            #print " == IBLStave[stave=",stave," ring=",ring,"] = ",IBLStave[ring]
            outputFileName =  outputDirIBL+"/"+"IBL_xRES_Module_stave"+str(stave)+"_ring"+str(logicalRing)+"."+oFext
            outputFileName.replace(" ","_")    
            outputFileName.replace("(","-")    
            outputFileName.replace(")","-")    
            DrawPlots(IBLStaveTemp, outputFileName, "IBL stave "+str(stave)+" ring "+str(logicalRing), 
                      "Hits", "Local x residual [mm]", 0.17, 0.89, "#mum", canvasText, makeOutput, 0.62, 0.89, False)
        DrawResPerStave(IBLStave,"IBL_xRESperModule_stave"+str(stave),outputDir+"/"+"IBL_xRESperModule_stave"+str(stave)+"."+oFext, nFiles)

if (doSCT):
    nlayers = 4#4
    nsectors = [32, 40, 48, 56]# [32, 40, 48, 56]
    nrings =  [12, 12, 12, 12]
    firstRing = -6

    outputDirSCT = outputDir + '/ModulesSCT'
    if not os.path.isdir(outputDirSCT):
        print " -- MakeResidualsPerModule -- Producing output in new directory:", outputDirSCT
        os.mkdir(outputDirSCT)
    #for layer in range(nlayers):
    layer = 3
    if (layer==3):
         print " -- MakeResidualsPerModule -- SCT module residuals for layer === ", layer, ' === '
         for sector in range(nsectors[layer]):
             for ring in range(nrings[layer]):
                 logicalRing = ring+firstRing
                 if (logicalRing >= 0): logicalRing = logicalRing+1
                 print " -- MakeResidualsPerModule -- SCT module: layer ", layer, ' sector:', sector, ' ring:', ring, "(logical ring:",logicalRing,")"
                 targetHisto = "sct_b"+str(layer)+"_xresvsmodetaphi_3d"
                 SCTModuleXRes = MakeModuleResPlotsFrom3D (residualsDir, legendTitles, markerColors, markerStyles, targetHisto, "noFitWithStats", 
                                                           rootFiles, nFiles, True, sector+1, logicalRing)
                 outputFileName =  outputDirSCT+"/"+"SCT_xRES_Module_layer"+str(layer)+"_sector"+str(sector)+"_ring"+str(logicalRing)+"."+oFext
                 outputFileName.replace(" ","_")    
                 outputFileName.replace("(","-")    
                 outputFileName.replace(")","-")    
                 print " -- MakeResidualsPerModule -- SCT module: output file -> ", outputFileName
                 DrawPlots(SCTModuleXRes, outputFileName, "SCT L"+str(layer)+" sector "+str(sector)+" ring "+str(logicalRing), 
                      "Hits", "Local x residual [mm]", 0.17, 0.89, "#mum", canvasText, makeOutput, 0.62, 0.89, False)
    
