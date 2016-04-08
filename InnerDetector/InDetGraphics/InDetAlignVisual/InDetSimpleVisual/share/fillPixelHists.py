# ================================
# Fills the Pixel Histograms
# Author John Alison <johnda@hep.upenn.edu>

# In the text file, the followint is the order of the information
# m_pix_barrel_ec, m_pix_layer_disk, m_pix_phi_module, m_pix_eta_module, m_pix_x, m_pix_y, m_pix_z);
for i in range(len(pixelElements1)):
    pix_x1 = pixelElements1[i][4] 
    pix_y1 = pixelElements1[i][5] 
    pix_z1 = pixelElements1[i][6]

    pix_x2 = pixelElements2[i][4]
    pix_y2 = pixelElements2[i][5]
    pix_z2 = pixelElements2[i][6]

    if pixelElements1[i][0] == 0:
        fillBarrelHists(pix_x1,pix_y1,pix_z1
                        ,pix_x2,pix_y2,pix_z2
                        ,pixelHists
                        )
        

        # Fill the Pixel Trans cans        
        pixelTransCan[int(pixelElements1[i][3]+6)].cd()
        line.SetLineColor(kRed+1)
        line.DrawArrow(pix_x1,pix_y1,pix_x1+TRASL_FACTOR*(pix_x2-pix_x1),pix_y1+TRASL_FACTOR*(pix_y2-pix_y1),0.01,"") 

        if int(pixelElements1[i][3]) == 0:
            IDTransCan.cd()
            line.SetLineColor(kRed+1)
            line.DrawArrow(pix_x1,pix_y1,pix_x1+TRASL_FACTOR*(pix_x2-pix_x1),pix_y1+TRASL_FACTOR*(pix_y2-pix_y1),0.01,"") 


        # Fill 3D plot lists
        color = 51 + int(pixelElements1[i][2])
        if color > 100:
            color = 51 - (color- 100)

        pixPoly = TPolyLine3D(2)
        pixPoly.SetLineWidth(1)
        pixPoly.SetLineColor(color)
        pixPoly.SetPoint(0,
                         pix_x1,
                         pix_y1,
                         pix_z1)
        pixPoly.SetPoint(1,
                         pix_x1+TRASL_FACTOR*(pix_x2-pix_x1),
                         pix_y1+TRASL_FACTOR*(pix_y2-pix_y1),
                         pix_z1+TRASL_FACTOR*(pix_z2-pix_z1))
        pixLines[int(pixelElements1[i][1])-1].append(pixPoly)

        pixMark = TPolyMarker3D(1)
        pixMark.SetMarkerSize(4)
        pixMark.SetMarkerColor(color)
        pixMark.SetMarkerStyle(7)
        pixMark.SetPoint(0,
                         pix_x1,
                         pix_y1,
                         pix_z1)
        pixPoints[int(pixelElements1[i][1])-1].append(pixMark)

    # Endcap A
    if pixelElements1[i][0] == 2:
        fillEndcapHists(pix_x1,pix_y1,pix_z1
                        ,pix_x2,pix_y2,pix_z2
                        ,pixelHists
                        ,2
                        )
        

    # Endcap C
    if pixelElements1[i][0] == -2:
        fillEndcapHists(pix_x1,pix_y1,pix_z1
                        ,pix_x2,pix_y2,pix_z2
                        ,pixelHists
                        ,0
                        )

pix3DCan.cd()
fullPixAxes = make_axes()
fullPixAxes.Draw()
fullPixTitle = make_title("PIX Visualization Barrel")
fullPixTitle.Draw()
for i in range(len(pix3DLayers)):
    pix3DLayers[i].cd()
    pixAxes[i].Draw()
    pixTitles[i].Draw()
    for j in range(len(pixLines[i])):
        pix3DCan.cd()
        pixLines[i][j].Draw()
        pixPoints[i][j].Draw()
        pix3DLayers[i].cd()
        pixLines[i][j].Draw()
        pixPoints[i][j].Draw()
