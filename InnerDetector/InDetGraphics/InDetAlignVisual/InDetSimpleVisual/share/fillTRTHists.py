# ================================
# Fills the TRT Histograms
# Author John Alison <johnda@hep.upenn.edu>

# In the text file, the following is the order of the information 
# m_trt_barrel_ec, m_trt_layer_or_wheel, m_trt_phi_module, m_trt_straw_layer, (m_trt_straw), m_trt_x, m_trt_y, m_trt_z);
# The m_trt_straw is only filled if the detailed TRT txt file was created
for i in range(len(trtElements1)):

    # the index allows us to readin files produced with and without the detailed TRT setting with the same code

    # Get the position of the first geometry 
    xIndex = 4
    yIndex = 5
    zIndex = 6
    if len(trtElements1[i]) == 8:    
        xIndex = 5
        yIndex = 6
        zIndex = 7

    trt_wheel = trtElements1[i][1]
    trt_strawlayer = trtElements1[i][3] 

    trt_x1 = trtElements1[i][xIndex] 
    trt_y1 = trtElements1[i][yIndex] 
    trt_z1 = trtElements1[i][zIndex]

    trt_x2 = trtElements2[i][xIndex]
    trt_y2 = trtElements2[i][yIndex]
    trt_z2 = trtElements2[i][zIndex]

    if abs(trtElements1[i][0]) == 1:
        fillBarrelHists(trt_x1,trt_y1,trt_z1
                        ,trt_x2,trt_y2,trt_z2
                        ,trtHists
                        )

        # Fill TRT Trans can
        if trtElements1[i][3] == 15:
            if trtElements1[i][0] == -1:
                trtTransCan[0].cd()
            else:
                trtTransCan[1].cd()
            line.SetLineColor(kGreen+1)
            line.DrawArrow(trt_x1,trt_y1,trt_x1+TRASL_FACTOR*(trt_x2-trt_x1),trt_y1+TRASL_FACTOR*(trt_y2-trt_y1),0.01,"") 

            IDTransCan.cd()
            if trt_z1 < 0:
                line.SetLineColor(kGreen-2)
            else:
                line.SetLineColor(kGreen+2)
            line.DrawArrow(trt_x1,trt_y1,trt_x1+TRASL_FACTOR*(trt_x2-trt_x1),trt_y1+TRASL_FACTOR*(trt_y2-trt_y1),0.01,"") 

            # Save values for later drawing
            trt_lay = trtElements1[i][1] 
            trt_phi = trtElements1[i][2]

            # C-Side
            if trtElements1[i][0] == -1:

                x1_Cside[(trt_lay,trt_phi)] = trt_x1
                y1_Cside[(trt_lay,trt_phi)] = trt_y1

                x2_Cside[(trt_lay,trt_phi)] = trt_x2
                y2_Cside[(trt_lay,trt_phi)] = trt_y2

            # A-Side
            else:

                x1_Aside[(trt_lay,trt_phi)] = trt_x1
                y1_Aside[(trt_lay,trt_phi)] = trt_y1
                z1_Aside[(trt_lay,trt_phi)] = trt_z1

                x2_Aside[(trt_lay,trt_phi)] = trt_x2
                y2_Aside[(trt_lay,trt_phi)] = trt_y2
            

        if len(drawTRTModules) > 0:
            trt_bec = trtElements1[i][0] 

            # Only Barrel for the moment
            if abs(trt_bec) == 1:
                trt_lay = trtElements1[i][1] 
                trt_phi = trtElements1[i][2]

                # Only draw the modules we care about
                if (trt_lay, trt_phi) in drawTRTModules:
                    
                    #fill the hists
                    thisTRTHist = trtModuleHists[(trt_lay),(trt_phi)]
                    fillBarrelHists(trt_x1,trt_y1,trt_z1
                                    ,trt_x2,trt_y2,trt_z2
                                    ,thisTRTHist)
                                    
                                    
                    # Draw the Trans can
                    theseCans = trtModTransCan[(trt_lay),(trt_phi)]

                    if trt_bec == -1:
                        theseCans[0].cd()
                    else:
                        theseCans[1].cd()

                    line.SetLineColor(kGreen+1)
                    line.DrawArrow(trt_x1,trt_y1,trt_x1+TRASL_FACTOR*(trt_x2-trt_x1),trt_y1+TRASL_FACTOR*(trt_y2-trt_y1),0.01,"")    


    # Endcap A
    if trtElements1[i][0] == 2:
        fillEndcapHists(trt_x1,trt_y1,trt_z1
                        ,trt_x2,trt_y2,trt_z2
                        ,trtHists
                        ,2
                        )

        
        drawStrawPlaneTransCan(2,trt_wheel,trt_strawlayer,trt_x1,trt_y1,trt_x2,trt_y2)
    

    # Endcap C
    if trtElements1[i][0] == -2:
        fillEndcapHists(trt_x1,trt_y1,trt_z1
                        ,trt_x2,trt_y2,trt_z2
                        ,trtHists
                        ,0
                        )

        drawStrawPlaneTransCan(-2,trt_wheel,trt_strawlayer,trt_x1,trt_y1,trt_x2,trt_y2)

