
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
    

    #PIX

    
    PIXX0vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsCLUSIZPHI,outputDir+"/"+"PIXX0vsCLUSIZEPHI."+oFext,"PIX Barrel layer 0","Local x residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


    PIXX1vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsCLUSIZPHI,outputDir+"/"+"PIXX1vsCLUSIZEPHI."+oFext,"PIX Barrel layer 1","Local x residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX2vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsCLUSIZPHI,outputDir+"/"+"PIXX2vsCLUSIZEPHI."+oFext,"PIX Barrel layer 2","Local x residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    PIXX3vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsCLUSIZPHI,outputDir+"/"+"PIXX3vsCLUSIZEPHI."+oFext,"PXI Barrel layer 3","Local x residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);




    PIXX0vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsCLUSIZZ,outputDir+"/"+"PIXX0vsCLUSIZEZ."+oFext,"PIX Barrel layer 0","Local x residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


    PIXX1vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsCLUSIZZ,outputDir+"/"+"PIXX1vsCLUSIZEZ."+oFext,"PIX Barrel layer 1","Local x residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXX2vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsCLUSIZZ,outputDir+"/"+"PIXX2vsCLUSIZEZ."+oFext,"PIX Barrel layer 2","Local x residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    PIXX3vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsCLUSIZZ,outputDir+"/"+"PIXX3vsCLUSIZEZ."+oFext,"PXI Barrel layer 3","Local x residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


    
    PIXY0vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY0vsCLUSIZPHI,outputDir+"/"+"PIXY0vsCLUSIZEPHI."+oFext,"PIX Barrel layer 0","Local y residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


    PIXY1vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualy_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY1vsCLUSIZPHI,outputDir+"/"+"PIXY1vsCLUSIZEPHI."+oFext,"PIX Barrel layer 1","Local y residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXY2vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualy_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY2vsCLUSIZPHI,outputDir+"/"+"PIXY2vsCLUSIZEPHI."+oFext,"PIX Barrel layer 2","Local y residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    PIXY3vsCLUSIZPHI = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualy_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY3vsCLUSIZPHI,outputDir+"/"+"PIXY3vsCLUSIZEPHI."+oFext,"PXI Barrel layer 3","Local y residual [mm]","Cluster size #phi",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);




    PIXY0vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY0vsCLUSIZZ,outputDir+"/"+"PIXY0vsCLUSIZEZ."+oFext,"PIX Barrel layer 0","Local y residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);


    PIXY1vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualy_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY1vsCLUSIZZ,outputDir+"/"+"PIXY1vsCLUSIZEZ."+oFext,"PIX Barrel layer 1","Local y residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);
    
    PIXY2vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualy_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY2vsCLUSIZZ,outputDir+"/"+"PIXY2vsCLUSIZEZ."+oFext,"PIX Barrel layer 2","Local y residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    PIXY3vsCLUSIZZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualy_clustersizeZ","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXY3vsCLUSIZZ,outputDir+"/"+"PIXY3vsCLUSIZEZ."+oFext,"PXI Barrel layer 3","Local y residual [mm]","Cluster size z",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);



    

    
    
    # SCT 
    SCTX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX0vsCLUSIZ,outputDir+"/"+"SCTX0vsCLUSIZE."+oFext,"SCT Barrel layer 0","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    SCTX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX1vsCLUSIZ,outputDir+"/"+"SCTX1vsCLUSIZE."+oFext,"SCT Barrel layer 1","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    SCTX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX2vsCLUSIZ,outputDir+"/"+"SCTX2vsCLUSIZE."+oFext,"SCT Barrel layer 2","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

    SCTX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX3vsCLUSIZ,outputDir+"/"+"SCTX3vsCLUSIZE."+oFext,"SCT Barrel layer 3","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.60, 0.88, False);

