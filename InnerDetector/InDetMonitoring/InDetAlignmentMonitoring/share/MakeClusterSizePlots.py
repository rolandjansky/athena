
for i in range(nFiles):    
    SCTBAR_Maps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR",0, True, 3, "clustersize")
    outputFileName = outputDir+"/"+"SCTBAR_s0ClusterSizeMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(SCTBAR_Maps, outputFileName, "#eta id", "#phi id", "Average cluster size", 0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")

    SCTBAR_Maps = MakeResidualMaps(residualsDir, legendTitles, rootFiles, i, "sct", "BAR",1, True, 3, "clustersize")
    outputFileName = outputDir+"/"+"SCTBAR_s1ClusterSizeMaps_File_"+legendTitles[i]+"."+oFext
    outputFileName = outputFileName.replace(" ","_")    
    outputFileName = outputFileName.replace("(","_")    
    outputFileName = outputFileName.replace(")","_")    
    DrawHitMaps(SCTBAR_Maps, outputFileName, "#eta id", "#phi id", "Average cluster size", 0.2, 0.95, "#mum",canvasText,makeOutput,"SCT","BAR")

	# SCT 
    SCTX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX0vsCLUSIZ,outputDir+"/"+"SCTX0vsCLUSIZE."+oFext,"SCT Barrel layer 0","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    SCTX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX1vsCLUSIZ,outputDir+"/"+"SCTX1vsCLUSIZE."+oFext,"SCT Barrel layer 1","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    SCTX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX2vsCLUSIZ,outputDir+"/"+"SCTX2vsCLUSIZE."+oFext,"SCT Barrel layer 2","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    SCTX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX3vsCLUSIZ,outputDir+"/"+"SCTX3vsCLUSIZE."+oFext,"SCT Barrel layer 3","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

