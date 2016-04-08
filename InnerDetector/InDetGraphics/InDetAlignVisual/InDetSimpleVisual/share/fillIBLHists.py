# ================================
# Fills the Ibl Histograms
# Author John Alison <johnda@hep.upenn.edu>

# In the text file, the followint is the order of the information
# m_ibl_barrel_ec, m_ibl_layer_disk, m_ibl_phi_module, m_ibl_eta_module, m_ibl_x, m_ibl_y, m_ibl_z);

for i in range(len(iblElements1)):
    ibl_x1 = iblElements1[i][4] 
    ibl_y1 = iblElements1[i][5] 
    ibl_z1 = iblElements1[i][6]

    ibl_x2 = iblElements2[i][4]
    ibl_y2 = iblElements2[i][5]
    ibl_z2 = iblElements2[i][6]

    if iblElements1[i][0] == 0:
        fillBarrelHists(ibl_x1,ibl_y1,ibl_z1
                        ,ibl_x2,ibl_y2,ibl_z2
                        ,iblHists
                        )

        # Fill the Ibl Trans cans        
        iblTransCan[int(iblElements1[i][3]+10)].cd()
        line.SetLineColor(kMagenta+1)
        line.DrawArrow(ibl_x1,ibl_y1,ibl_x1+TRASL_FACTOR*(ibl_x2-ibl_x1),ibl_y1+TRASL_FACTOR*(ibl_y2-ibl_y1),0.01,"") 

        if int(iblElements1[i][3]) == 0:
            IDTransCan.cd()
            line.SetLineColor(kMagenta+1)
            line.DrawArrow(ibl_x1,ibl_y1,ibl_x1+TRASL_FACTOR*(ibl_x2-ibl_x1),ibl_y1+TRASL_FACTOR*(ibl_y2-ibl_y1),0.01,"") 


        # Fill 3D plot lists
        iblPoly = TPolyLine3D(2)
        iblPoly.SetLineWidth(1)
        iblPoly.SetLineColor(51 + 3*int(iblElements1[i][2]))
        iblPoly.SetPoint(0,
                         ibl_x1,
                         ibl_y1,
                         ibl_z1)
        iblPoly.SetPoint(1,
                         ibl_x1+TRASL_FACTOR*(ibl_x2-ibl_x1),
                         ibl_y1+TRASL_FACTOR*(ibl_y2-ibl_y1),
                         ibl_z1+TRASL_FACTOR*(ibl_z2-ibl_z1))
        iblLines[int(iblElements1[i][2])].append(iblPoly)

        iblMark = TPolyMarker3D(1)
        iblMark.SetMarkerSize(4)
        #iblMark.SetMarkerColor(kMagenta-9+int(iblElements1[i][2]))
        iblMark.SetMarkerColor(51 +3*int(iblElements1[i][2]))
        iblMark.SetMarkerStyle(7)
        iblMark.SetPoint(0,
                         ibl_x1,
                         ibl_y1,
                         ibl_z1)
        iblPoints[int(iblElements1[i][2])].append(iblMark)

    # DBM A
    if iblElements1[i][0] == 4:
        fillEndcapHists(ibl_x1,ibl_y1,ibl_z1
                        ,ibl_x2,ibl_y2,ibl_z2
                        ,iblHists
                        ,2
                        )
        

    # DBM C
    if iblElements1[i][0] == -4:
        fillEndcapHists(ibl_x1,ibl_y1,ibl_z1
                        ,ibl_x2,ibl_y2,ibl_z2
                        ,iblHists
                        ,0
                        )

# Draw 3D Plots
ibl3DCan.cd()
fullIblAxis = make_axes()
fullIblAxis.Draw()
fullIblTitle = make_title("IBL Visualization")
fullIblTitle.Draw()

for i in range(len(ibl3DStaves)):
    ibl3DStaves[i].cd()
    iblAxes[i].Draw()
    iblTitles[i].Draw()
    for j in range(len(iblLines[i])):
        ibl3DCan.cd()
        iblLines[i][j].Draw()
        iblPoints[i][j].Draw()
        ibl3DStaves[i].cd()
        iblLines[i][j].Draw()
        iblPoints[i][j].Draw()




