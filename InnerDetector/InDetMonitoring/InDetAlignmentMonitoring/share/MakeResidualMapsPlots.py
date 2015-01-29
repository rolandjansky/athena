# MakeResidualMapsPlots
#
MinEntriesPerModule = 15

for i in range(nFiles):
    print " -- MakeResidualMapsPlots -- file ", i
    PIXBAR_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 0, True, 300)
    DrawHitMaps(PIXBAR_xResMeanMaps, outputDir+"/"+"PIXBAR_xResMeanMaps_File_"+str(i+1)+"."+oFext, "#eta id", "#phi id", "Local x residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput)
    PIXBAR_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "BAR", 1, True, 250)
    DrawHitMaps(PIXBAR_yResMeanMaps, outputDir+"/"+"PIXBAR_yResMeanMaps_File_"+str(i+1)+"."+oFext, "#eta id", "#phi id", "Local y residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput)

for i in range(nFiles):
    PIXECA_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECA", 0)
    DrawHitMaps(PIXECA_xResMeanMaps, outputDir+"/"+"PIXECA_xResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECA")

    PIXECA_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECA", 1, True, 50)
    DrawHitMaps(PIXECA_yResMeanMaps, outputDir+"/"+"PIXECA_yResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local y residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECA")

    # PIX ECC doesn't work because histogrma is empty !
    PIXECC_xResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECC", 0)
    DrawHitMaps(PIXECC_xResMeanMaps, outputDir+"/"+"PIXECC_xResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECC")

    PIXECC_yResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "pixels", "ECC", 1, True, 50)
    DrawHitMaps(PIXECC_yResMeanMaps, outputDir+"/"+"PIXECC_yResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECC")

    # unbiased residual maps
    SCTBAR_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR")
    DrawHitMaps(SCTBAR_ResMeanMaps, outputDir+"/"+"SCTBAR_ResMeanMaps_File_"+str(i+1)+"."+oFext, "#eta id", "#phi id", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")

    SCTECAs0_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECA", 0)
    DrawHitMaps(SCTECAs0_ResMeanMaps, outputDir+"/"+"SCTECAs0_ResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECA")

    SCTECAs1_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECA", 1)
    DrawHitMaps(SCTECAs1_ResMeanMaps, outputDir+"/"+"SCTECAs1_ResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECA")

    SCTECCs0_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECC", 0)
    DrawHitMaps(SCTECCs0_ResMeanMaps, outputDir+"/"+"SCTECCs0_ResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECC")

    SCTECCs1_ResMeanMaps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "ECC", 1)
    DrawHitMaps(SCTECCs1_ResMeanMaps, outputDir+"/"+"SCTECCs1_ResMeanMaps_File_"+str(i+1)+"."+oFext, "", "", "Local x residual [#mu m]", 
                0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","ECC")

# reset the plots style
execfile("AtlasStyle.py")

