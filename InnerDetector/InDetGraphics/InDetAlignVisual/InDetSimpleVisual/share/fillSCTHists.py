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
        line.DrawArrow(sct_x1,sct_y2,sct_x1+TRASL_FACTOR*(sct_x1-sct_x2),sct_y1+TRASL_FACTOR*(sct_y1-sct_y2),0.01,"") 

        if etaMod == 6:
            IDTransCan.cd()
            line.SetLineColor(kBlue+1)
            line.DrawArrow(sct_x1,sct_y2,sct_x1+TRASL_FACTOR*(sct_x1-sct_x2),sct_y1+TRASL_FACTOR*(sct_y1-sct_y2),0.01,"") 



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



