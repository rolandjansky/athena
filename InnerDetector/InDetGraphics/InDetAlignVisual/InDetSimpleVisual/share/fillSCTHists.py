# ================================
# Fills the SCT Histograms
# Author John Alison <johnda@hep.upenn.edu>

# In the text file, the followint is the order of the information
# m_sct_barrel_ec, m_sct_layer_disk, m_sct_phi_module, m_sct_eta_module, m_sct_x, m_sct_y, m_sct_z);
for i in range(len(sctElements1)):
    sct_x1 = sctElements1[i][4] #+ 0.8
    sct_y1 = sctElements1[i][5] #- 0.2
    sct_z1 = sctElements1[i][6]

    sct_x2 = sctElements2[i][4] 
    sct_y2 = sctElements2[i][5]
    sct_z2 = sctElements2[i][6]

    if sctElements1[i][0] == 0:
        fillBarrelHists(sct_x1,sct_y1,sct_z1
                        ,sct_x2,sct_y2,sct_z2
                        ,sctHists
                        )

        # Fill SCT Trans Cans
        etaMod = int(sctElements1[i][3])
        if etaMod < 0:
            etaMod = etaMod + 6
        else:
            etaMod = etaMod + 5

        sctTransCan[etaMod].cd()
        line.SetLineColor(kBlue+1)
        line.DrawArrow(sct_x1,sct_y1,sct_x1+TRASL_FACTOR*(sct_x2-sct_x1),sct_y1+TRASL_FACTOR*(sct_y2-sct_y1),0.01,"") 

        if etaMod == 6:
            IDTransCan.cd()
            line.SetLineColor(kBlue+1)
            line.DrawArrow(sct_x1,sct_y1,sct_x1+TRASL_FACTOR*(sct_x2-sct_x1),sct_y1+TRASL_FACTOR*(sct_y2-sct_y1),0.01,"") 


        # Fill 3D plot lists
        color = 51 + int(sctElements1[i][2])
        if color > 100:
            color = 51 - (color- 100)

        sctPoly = TPolyLine3D(2)
        sctPoly.SetLineWidth(1)
        sctPoly.SetLineColor(color)
        sctPoly.SetPoint(0,
                         sct_x1,
                         sct_y1,
                         sct_z1)
        sctPoly.SetPoint(1,
                         sct_x1+TRASL_FACTOR*(sct_x2-sct_x1),
                         sct_y1+TRASL_FACTOR*(sct_y2-sct_y1),
                         sct_z1+TRASL_FACTOR*(sct_z2-sct_z1))
        sctLines[int(sctElements1[i][1])].append(sctPoly)

        sctMark = TPolyMarker3D(1)
        sctMark.SetMarkerSize(4)
        sctMark.SetMarkerColor(color)
        sctMark.SetMarkerStyle(7)
        sctMark.SetPoint(0,
                         sct_x1,
                         sct_y1,
                         sct_z1)
        sctPoints[int(sctElements1[i][1])].append(sctMark)


    if sctElements1[i][0] == 2:
        fillEndcapHists(sct_x1,sct_y1,sct_z1
                        ,sct_x2,sct_y2,sct_z2
                        ,sctHists
                        ,2
                        )

    if sctElements1[i][0] == -2:
        fillEndcapHists(sct_x1,sct_y1,sct_z1
                        ,sct_x2,sct_y2,sct_z2
                        ,sctHists
                        ,0
                        )

sct3DCan.cd()
fullSctAxes = make_axes()
fullSctAxes.Draw()
fullSctTitle = make_title("SCT Visualization Barrel")
fullSctTitle.Draw()
for i in range(len(sct3DLayers)):
    sct3DLayers[i].cd()    
    sctAxes[i].Draw()
    sctTitles[i].Draw()
    for j in range(len(sctLines[i])):
        sct3DCan.cd()
        sctLines[i][j].Draw()
        sctPoints[i][j].Draw()
        sct3DLayers[i].cd()
        sctLines[i][j].Draw()
        sctPoints[i][j].Draw()
