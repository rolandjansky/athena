#
for i in range(nFiles):    

    PIXBAR_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "measurements")
    DrawHitMaps(PIXBAR_HitMaps, outputDir+"/"+"PIXBAR_HitMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi stave", "Hits", 
                0.2, 0.97, "#mum",canvasText,makeOutput)

    PIXECA_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "ECA")
    DrawHitMaps(PIXECA_HitMaps, outputDir+"/"+"PIXECA_HitMaps_File_"+str(i+1)+"."+oFext, "", "", "Hits", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECA")

    PIXECC_HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "ECC")
    DrawHitMaps(PIXECC_HitMaps, outputDir+"/"+"PIXECC_HitMaps_File_"+str(i+1)+"."+oFext, "", "", "Hits", 
                0.2, 0.96, "#mum",canvasText,makeOutput,"PIX","ECC")

    # SCT barrel side 0 and side 1
    SCTBAR_s0HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "SCT", "BAR", True, 0)
    DrawHitMaps(SCTBAR_s0HitMaps, outputDir+"/"+"SCTBAR_s0HitMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi sector", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR")

    SCTBAR_s1HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "BAR", True, 1)
    DrawHitMaps(SCTBAR_s1HitMaps, outputDir+"/"+"SCTBAR_s1HitMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi sector", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR")

    # SCT ECA side 0 and sie 1
    SCTECA_s0HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECA", True, 0)
    DrawHitMaps(SCTECA_s0HitMaps, outputDir+"/"+"SCTECA_s0HitMaps_File_"+str(i+1)+"."+oFext, "", "", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")

    SCTECA_s1HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECA", True, 1)
    DrawHitMaps(SCTECA_s1HitMaps, outputDir+"/"+"SCTECA_s1HitMaps_File_"+str(i+1)+"."+oFext, "", "", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")

    # SCT ECC side 0 and sie 1
    SCTECC_s0HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECC", True, 0)
    DrawHitMaps(SCTECC_s0HitMaps, outputDir+"/"+"SCTECC_s0HitMaps_File_"+str(i+1)+"."+oFext, "", "", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")

    SCTECC_s1HitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "ECC", True, 1)
    DrawHitMaps(SCTECC_s1HitMaps, outputDir+"/"+"SCTECC_s1HitMaps_File_"+str(i+1)+"."+oFext, "", "", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","ECA")

    #
    # HIT EFFICIENCY PER MODULE AND PLANE
    #
    # SCT barrel side 0 and side 1
    SCTBAR_s0HitEffMaps = MakeHitEffMaps(hitEffDir, legendTitles, rootFiles, i, "SCT", "BAR", True, 0)
    DrawHitMaps(SCTBAR_s0HitEffMaps, outputDir+"/"+"SCTBAR_s0HitEffMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi sector", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR", 3)

    SCTBAR_s1HitEffMaps = MakeHitEffMaps(hitEffDir, legendTitles, rootFiles, i, "sct", "BAR", True, 1)
    DrawHitMaps(SCTBAR_s1HitEffMaps, outputDir+"/"+"SCTBAR_s1HitEffMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi sector", "Hits", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"SCT","BAR", 3)

    # pixel barrel hit efficiency map
    PIXBAR_HitEffMaps = MakeHitEffMaps(hitEffDir, legendTitles, rootFiles, i, "PIX", "BAR", True, 0)
    DrawHitMaps(PIXBAR_HitEffMaps, outputDir+"/"+"PIXBAR_HitEffMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi sector", "Hit eff.", 
                0.2, 0.96, "#mum",canvasText, makeOutput,"PIX","BAR", 3)

    if (userExtended):
        print " <MakeHitMapsPlots> Extended = True "
        PIXBAR_ExpectedHitMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "hits")
        DrawHitMaps(PIXBAR_ExpectedHitMaps, outputDir+"/"+"PIXBAR_ExpectedHitMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi stave", "Expected hits", 
                    0.2, 0.97, "#mum",canvasText,makeOutput)
        
        PIXBAR_HoleMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "holes")
        DrawHitMaps(PIXBAR_HoleMaps, outputDir+"/"+"PIXBAR_HoleMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi stave", "Holes", 
                    0.2, 0.97, "#mum",canvasText,makeOutput)

        PIXBAR_OutliersMaps = MakeHitMaps(hitEffDir, legendTitles, rootFiles, i, "pixels", "BAR", True, 0, "outliers")
        DrawHitMaps(PIXBAR_OutliersMaps, outputDir+"/"+"PIXBAR_OutliersMaps_File_"+str(i+1)+"."+oFext, "#eta ring", "#phi stave", "Outliers", 
                0.2, 0.97, "#mum",canvasText,makeOutput)
    

# reset the plots style
execfile("AtlasStyle.py")
