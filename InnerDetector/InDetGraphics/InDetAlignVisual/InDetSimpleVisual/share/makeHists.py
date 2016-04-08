# ================================
# Creates the Histograms
# Author John Alison <johnda@hep.upenn.edu>

class geometryHists:
    def __init__(self,name,title,color):

        # Detector Geometries
        # ======================
        self.geo1 = makeSubSysGeometryHist(name+"1","Inner Detector Positions for Geometry 1 "+title,color)
        self.geo2 = makeSubSysGeometryHist(name+"2","Inner Detector Positions for Geometry 2 "+title,color)

        # Differences in X
        # ==================

        # Differences in X Vs R
        self.DxVsR = makeSubSysDiffHist(name+"XvsR"
                                        ,"Differences in X Position (IBL/Pix/SCT/TRT) vs R "+title
                                        ,"Radius [mm]",[0,0,0],[1200,1200,1200]
                                        ,"Difference in X Position [mm]",deltaX,color)
        
        # Differences in X Vs Z
        self.DxVsZ = makeSubSysDiffHist(name+"XvsZ"
                                        ,"Differences in X Position (IBL/Pix/SCT/TRT) vs Z "+title
                                        ,"Z [mm]",[-2900,-800,400],[-400,800,2900]
                                        ,"Difference in X Position [mm]",deltaX,color
                                        ,nBins=2400)
        # Differences in X 
        self.Dx = make1D_SubSysDiffHist(name+"X"
                                        ,"Differences in X Position (IBL/Pix/SCT/TRT) "+title
                                        ,"Difference in X Position",[-1*deltaX,-1*deltaX,-1*deltaX],[deltaX,deltaX,deltaX]
                                        ,"Entries",color)


        # Differences in Y
        # ==================

        # Differences in Y Vs R
        self.DyVsR = makeSubSysDiffHist(name+"YvsR"
                                        ,"Differences in Y Position (IBL/Pix/SCT/TRT) vs R "+title
                                        ,"Radius [mm]",[0,0,0],[1200,1200,1200]
                                        ,"Difference in Y Position",deltaY,color)

        # Differences in Y Vs Z
        self.DyVsZ = makeSubSysDiffHist(name+"YvsZ"
                                        ,"Differences in Y Position (IBL/Pix/SCT/TRT) vs Z "+title
                                        ,"Z [mm]",[-2900,-800,400],[-400,800,2900]
                                        ,"Difference in Y Position",deltaY,color
                                        ,nBins=2400)
        # Differences in Y 
        self.Dy = make1D_SubSysDiffHist(name+"Y"
                                        ,"Differences in Y Position (IBL/Pix/SCT/TRT) "+title
                                        ,"Difference in Y Position",[-5,-5,-5],[5,5,5]
                                        ,"Entries",color)


        # Differences in Z
        # ==================

        # Differences in Z vs R
        self.DzVsR = makeSubSysDiffHist(name+"ZvsR"
                                        ,"Differences in Z Position (IBL/Pix/SCT/TRT) vs R "+title
                                        ,"Radius [mm]",[0,0,0],[1200,1200,1200]
                                        ,"Difference in Z Position",deltaZ,color)

        # Differences in Z 
        self.Dz = make1D_SubSysDiffHist(name+"Z"
                                        ,"Differences in Z Position (IBL/Pix/SCT/TRT) "+title
                                        ,"Difference in Z Position",[-5,-5,-5],[5,5,5]
                                        ,"Entries",color)

        # Differences in Phi
        # ==================

        # Differences in Phi Vs Z
        self.DphiVsZ= makeSubSysDiffHist(name+"PhivsZ"
                                         ,"Differences in Phi Position (IBL/Pix/SCT/TRT) vs Z"+title
                                         ,"Z [mm]",[-2900,-800,400],[-400,800,2900]
                                         ,"Difference in Phi Position",deltaPhi,color
                                         ,nBins=2400)

        # Differences in Phi vs R
        self.DphiVsR = makeSubSysDiffHist(name+"PhivsR"
                                          ,"Differences in phi Position (IBL/Pix/SCT/TRT) vs R "+title
                                          ,"Radius [mm]",[0,0,0],[1200,1200,1200]
                                          ,"Difference in XY Position",deltaPhiTrans,color)

        # Differences in XY
        # ==================

        # Differences in XY vs R
        self.DxyVsR = makeSubSysDiffHist(name+"XYvsR"
                                    ,"Differences in X-Y Position (IBL/Pix/SCT/TRT) vs R"+title
                                    ,"Radius [mm]",[0,0,0],[1200,1200,1200]
                                    ,"Difference in XY Position",deltaXY,color)

        # Differences in XY vs Z
        self.DxyVsZ = makeSubSysDiffHist(name+"XYvsZ"
                                    ,"Differences in XY Position (IBL/Pix/SCT/TRT) vs Z "+title
                                    ,"Z [mm]",[-2900,0,400],[-400,800,2900]
                                    ,"Difference in XY Position",deltaXY,color)
        # Differences in R
        # ==================

        # Differences in R vs R
        self.DrVsR = makeSubSysDiffHist(name+"RvsR"
                                   ,"Differences in R Position (IBL/Pix/SCT/TRT) vs R "+title
                                   ,"Radius [mm]",[0,0,0],[1200,1200,1200]
                                   ,"Difference in R Position",deltaR,color)

        ## Differences in R vs Phi
        self.DrVsPhi = makeSubSysDiffHist(name+"RvsPhi"
                                     ,"Differences in R Position (IBL/Pix/SCT/TRT) vs phi"+title
                                     ,"Phi [rad]",[-3.14,-3.14,-3.14],[3.14,3.14,3.14]
                                     ,"Difference in R Position",deltaR,color)


pixelHists = geometryHists("Pix","Pixel",kRed)
sctHists = geometryHists("SCT","SCT",kBlue)
trtHists = geometryHists("TRT","TRT",kGreen)
iblHists = geometryHists("IBL","IBL",kMagenta)

trtModuleHists = {}
     
for thisMod in range(len(drawTRTModules)):
    thisLay = drawTRTModules[thisMod][0]
    thisPhi = drawTRTModules[thisMod][1]
    # Add the pair to our total list
    trtModuleHists[(thisLay,thisPhi)] = geometryHists("TRT_L_"+str(thisLay)+"_P_"+str(thisPhi)
                                                      ,"TRT Layer: "+str(thisLay)+" Phi: "+str(thisPhi)
                                                      ,kGreen)
