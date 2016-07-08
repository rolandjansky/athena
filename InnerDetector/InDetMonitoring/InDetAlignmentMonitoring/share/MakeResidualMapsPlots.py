# MakeResidualMapsPlots
#
MinEntriesPerModule = 20

for i in range(nFiles):
    
    print " -- MakeResidualMapsPlots -- file ", i
    PIXBAR_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 0, True, 175)
    outputFileName = outputDir+"/"+userPrefix+"PIXBAR_xResMeanMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(PIXBAR_xResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput)
    
    PIXBAR_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 1, True, 100)
    outputFileName = outputDir+"/"+userPrefix+"PIXBAR_yResMeanMaps_File_"+str(i)+"."+oFext
    outputFileName = outputDir+"/"+userPrefix+"PIXBAR_yResMeanMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(PIXBAR_yResMeanMaps, outputFileName, "#eta id", "#phi id", "Local y residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput)

    if (userExtended):
        PIXECA_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECA", 0)
        outputFileName = outputDir+"/"+userPrefix+"PIXECA_xResMeanMaps_File_"+str(i)+"."+oFext
        outputFileName = outputDir+"/"+userPrefix+"PIXECA_xResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXECA_xResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                    0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECA")

        PIXECA_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECA", 1, True, 40)
        outputFileName =  outputDir+"/"+userPrefix+"PIXECA_yResMeanMaps_File_"+str(i)+"."+oFext
        outputFileName =  outputDir+"/"+userPrefix+"PIXECA_yResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXECA_yResMeanMaps, outputFileName, "", "", "Local y residual [#mu m]", 
                    0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECA")

    # PIX ECC doesn't work because histogrma is empty !
        PIXECC_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECC", 0)
        outputFileName =  outputDir+"/"+userPrefix+"PIXECC_xResMeanMaps_File_"+str(i)+"."+oFext
        outputFileName =  outputDir+"/"+userPrefix+"PIXECC_xResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXECC_xResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                    0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECC")

        PIXECC_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECC", 1, True, 40)
        outputFileName =  outputDir+"/"+userPrefix+"PIXECC_yResMeanMaps_File_"+str(i)+"."+oFext
        outputFileName =  outputDir+"/"+userPrefix+"PIXECC_yResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(PIXECC_yResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                    0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECC")

    # SCT Barrel: unbiased residual maps 
    SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR",0, True, 20)
    outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s0ResMeanMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")

    SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR", 1, True, 20)
    outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s1ResMeanMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")

    ###########################
    # pixel barrel residual width
    PIXBAR_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 0, True, 80, "width")
    outputFileName = outputDir+"/"+userPrefix+"PIXBAR_xResWidthMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(PIXBAR_xResMeanMaps, outputFileName, "#eta id", "#phi id", "Width of local x residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","BAR",4)
    
    PIXBAR_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 1, True, 350, "width")
    outputFileName = outputDir+"/"+userPrefix+"PIXBAR_yResWidthMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(PIXBAR_yResMeanMaps, outputFileName, "#eta id", "#phi id", "Width of local y residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","BAR", 4)
    
    # SCT barrel residual width
    SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR", 0, True, 80, "width")
    outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s0ResWidthMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Width of local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR", 4)

    SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR", 1, True, 80, "width")
    outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s1ResWidthMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Width of local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR", 4)

    if (userExtended):       
        SCTECAs0_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECA", 0, True, 10)
        outputFileName = outputDir+"/"+userPrefix+"SCTECA_s0ResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(SCTECAs0_ResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                    0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECC")

        SCTECAs1_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECA", 1, True, 10)
        outputFileName = outputDir+"/"+userPrefix+"SCTECA_s1ResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(SCTECAs1_ResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECC")

        SCTECCs0_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECC", 0, True, 10)
        outputFileName = outputDir+"/"+userPrefix+"SCTECC_s0ResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(SCTECCs0_ResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECC")
        
        SCTECCs1_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECC", 1, True, 10)
        outputFileName = outputDir+"/"+userPrefix+"SCTECC_s1ResMeanMaps_File_"+legendTitles[i]+"."+oFext
        outputFileName = outputFileName.replace(" ","_")    
        outputFileName = outputFileName.replace("(","_")    
        outputFileName = outputFileName.replace(")","_")    
        DrawHitMaps(SCTECCs1_ResMeanMaps, outputFileName, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECC")
        
        if (False):
            #########################
            # detailed residual maps
            PIXBAR_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 0, True, 40, "detailed")
            outputFileName = outputDir+"/"+userPrefix+"PIXBAR_xResMeanDetailedMaps_File_"+legendTitles[i]+"."+oFext
            outputFileName = outputFileName.replace(" ","_")    
            outputFileName = outputFileName.replace("(","_")    
            outputFileName = outputFileName.replace(")","_")    
            DrawHitMaps(PIXBAR_xResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                        0.2, 0.96, "#mum",canvasText,makeOutput)
        
            PIXBAR_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 1, True, 100, "detailed")
            outputFileName = outputDir+"/"+userPrefix+"PIXBAR_yResMeanDetailedMaps_File_"+legendTitles[i]+"."+oFext
            outputFileName = outputFileName.replace(" ","_")    
            outputFileName = outputFileName.replace("(","_")    
            outputFileName = outputFileName.replace(")","_")    
            DrawHitMaps(PIXBAR_yResMeanMaps, outputFileName, "#eta id", "#phi id", "Local y residual [#mu m]", 
                        0.2, 0.96, "#mum",canvasText,makeOutput)
            
            SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR", 0, True, 25, "detailed")
            outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s0ResMeanDetailedMaps_File_"+legendTitles[i]+"."+oFext
            outputFileName = outputFileName.replace(" ","_")    
            outputFileName = outputFileName.replace("(","_")    
            outputFileName = outputFileName.replace(")","_")    
            DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                        0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")
            
            SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR", 1, True, 25, "detailed")
            outputFileName = outputDir+"/"+userPrefix+"SCTBAR_s1ResMeanDetailedMaps_File_"+legendTitles[i]+"."+oFext
            outputFileName = outputFileName.replace(" ","_")    
            outputFileName = outputFileName.replace("(","_")    
            outputFileName = outputFileName.replace(")","_")    
            DrawHitMaps(SCTBAR_ResMeanMaps, outputFileName, "#eta id", "#phi id", "Local x residual [#mu m]", 
                        0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")
        
# reset the plots style
execfile("AtlasStyle.py")

