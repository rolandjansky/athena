from ROOT import *
include("InDetAlignExample/RootSetup.py")
import sys, string, os


def drawModulePlots( posfile ):
    m_error_p_tot = TH1F("m_error_p_tot"," \hat{#phi} error for all layers ", 50, 0,0.05)
    m_error_p_tot.SetLineWidth(2)
    m_error_p_tot.SetFillColor(kGray+1)
    m_error_p_0 = TH1F("m_error_p_0"," \hat{#phi} error for modules in layer 0", 50, 0,0.05)
    m_error_p_0.SetLineWidth(2)
    m_error_p_0.SetFillColor(kGray+1)
    m_error_p_1 = TH1F("m_error_p_1"," \hat{#phi} error for modules in layer 1", 50, 0,0.05)
    m_error_p_1.SetLineWidth(2)
    m_error_p_1.SetFillColor(kGray+1)
    m_error_p_2 = TH1F("m_error_p_2"," \hat{#phi} error for modules in layer 2", 50, 0,0.05)
    m_error_p_2.SetLineWidth(2)
    m_error_p_2.SetFillColor(kGray+1)
    
    m_errorN_p_tot = TH2F("m_errorN_p_tot"," \hat{#phi} error  times sqrt(N) vs N for all layers ", 50, 100, 1e5, 50,0,10)
    m_errorN_p_tot.SetMarkerStyle(7)
    m_errorN_p_0 = TH2F("m_errorN_p_0"," \hat{#phi} error  times sqrt(N) vs N for modules in layer 0", 50, 100,1e5,50,0,10)
    m_errorN_p_0.SetMarkerStyle(7)
    m_errorN_p_1 = TH2F("m_errorN_p_1"," \hat{#phi} error  times sqrt(N) vs N for modules in layer 1", 50, 100,1e5,50,0,10)
    m_errorN_p_1.SetMarkerStyle(7)
    m_errorN_p_2 = TH2F("m_errorN_p_2"," \hat{#phi} error  times sqrt(N) vs N for modules in layer 2", 50, 100,1e5,50,0,10)
    m_errorN_p_2.SetMarkerStyle(7)
    
    m_error_r_tot = TH1F("m_error_r_tot"," \hat{r} error for all layers   ", 50, 0,0.1)
    m_error_r_tot.SetLineWidth(2)
    m_error_r_tot.SetFillColor(kGray+1)
    m_error_r_0 = TH1F("m_error_r_0"," \hat{r} error for modules in layer 0", 50, 0,0.1)
    m_error_r_0.SetLineWidth(2)
    m_error_r_0.SetFillColor(kGray+1)
    m_error_r_1 = TH1F("m_error_r_1"," \hat{r} error for modules in layer 1", 50, 0,0.1)
    m_error_r_1.SetLineWidth(2)
    m_error_r_1.SetFillColor(kGray+1)
    m_error_r_2 = TH1F("m_error_r_2"," \hat{r} error for modules in layer 2", 50, 0,0.1)
    m_error_r_2.SetLineWidth(2)
    m_error_r_2.SetFillColor(kGray+1)
    
    m_errorN_r_tot = TH2F("m_errorN_r_tot"," \hat{r} error  times sqrt(N) for all layers   ", 50, 100, 1e5,50, 0,5)
    m_errorN_r_tot.SetMarkerStyle(7)
    m_errorN_r_0 = TH2F("m_errorN_r_0"," \hat{r} error  times sqrt(N) for modules in layer 0", 50, 100, 1e5,50, 0,5 )
    m_errorN_r_0.SetMarkerStyle(7)
    m_errorN_r_1 = TH2F("m_errorN_r_1"," \hat{r} error  times sqrt(N) for modules in layer 1", 50, 100, 1e5, 50, 0,5)
    m_errorN_r_1.SetMarkerStyle(7)
    m_errorN_r_2 = TH2F("m_errorN_r_2"," \hat{r} error  times sqrt(N) for modules in layer 2", 50, 100, 1e5, 50, 0,5)
    m_errorN_r_2.SetMarkerStyle(7)
    
    m_N_tot = TH1F("m_N_tot"," sqrt(N) for all layers   ", 50, 0,1e3)
    m_N_tot.SetLineWidth(2)
    m_N_tot.SetFillColor(kGray+1)
    m_N_0 = TH1F("m_N_0"," sqrt(N) for modules in layer 0", 50, 0,1e3)
    m_N_0.SetLineWidth(2)
    m_N_0.SetFillColor(kGray+1)
    m_N_1 = TH1F("m_N_1"," sqrt(N) for modules in layer 1", 50, 0,1e3)
    m_N_1.SetLineWidth(2)
    m_N_1.SetFillColor(kGray+1)
    m_N_2 = TH1F("m_N_2"," sqrt(N) for modules in layer 2", 50, 0,1e3)
    m_N_2.SetLineWidth(2)
    m_N_2.SetFillColor(kGray+1)

    
    m_modDict={}

    #==== Initialize all the modules which are readout
    for i in range(3):
        for j in range(32):
            if 3*j+i in readOut:
                mod = Module(-1,i,j)
                m_modDict[3*mod.phi+mod.lay] = mod
    #================================================
    
    file = open(posfile)

    nlinesAfterModInitialize = 99 #start value > 0
    modId = 99 # start value of modId different from all others
   
    for line in file:
        nlinesAfterModInitialize += 1
        words = string.split(line)
        if words[0] == "new" and words[4].split('_')[0].split('/')[1] == "Barrel" :
            modId = 3* int(words[4].split('_')[2])+ int(words[4].split('_')[3])
            if modId in readOut:
                nlinesAfterModInitialize = 0
   
        if nlinesAfterModInitialize == 1 and modId in readOut:
            m_modDict[modId].AddNHits( words[4] )
     

        if nlinesAfterModInitialize == 3 and modId in readOut:
            m_modDict[modId].SetErrDx( words[3] )
     
        
        #Commment out the following for 1 dof  
        if nlinesAfterModInitialize == 4 and modId in readOut:
            m_modDict[modId].SetErrDz( words[3] ) 
            
        #Reading in the Cog-displacement and rotation from the alignlog
        if words[0] == "Total" and words[1]== "alignment":
            m_id = words[3].split('_')
            if m_id[1] == "-1":
                modId = 3*int(m_id[2]) + int(m_id[3])
                if modId in readOut:
                    m_dis = words[7].split(',')
                    m_modDict[modId].SetDx( m_dis[0].lstrip('(') )
                    m_modDict[modId].SetDy( m_dis[1] )
                    m_modDict[modId].SetDz( m_dis[2].rstrip(')') )
                    m_rot = words[10].split(',')
                    m_modDict[modId].SetDrotx( m_rot[0].lstrip('(') )
                    m_modDict[modId].SetDroty( m_rot[1] )
                    m_modDict[modId].SetDrotz( m_rot[2].rstrip(')') )
                
        #Reading in the Local to Global transformations
        if words.__len__() > 1:
            if words[1] == "trans=":
                m_id = words[0].split('_')
                if m_id[1] == "-1":
                    m_cog = words[2].split(',')
                    modId = 3*int(m_id[2])+int(m_id[3])
                    if modId in readOut:
                        m_modDict[modId].SetCogX(m_cog[0].lstrip('('))
                        m_modDict[modId].SetCogY(m_cog[1])
                        m_modDict[modId].SetCogZ(m_cog[2].rstrip(')'))
                        m_modDict[modId].SetRotx(words[4].lstrip('(').rstrip(','))
                        m_modDict[modId].SetRoty(words[5].rstrip(','))
                        m_modDict[modId].SetRotz(words[6].rstrip(')'))
                    
    file.close()
    
    
       
    # ======================
    # Fill the plots
    # ======================

    for j in range(readOut.__len__()):
        i = readOut[j]
        if m_modDict[i].bec == -1:
            
            errorp = m_modDict[i].ErrDx
            errorr = m_modDict[i].ErrDz

            max = modDict[i].NHitsIter.__len__()
            sqN = pow( modDict[i].NHitsIter[max-1] , 0.5)
            m_N_tot.Fill( sqN )
            
            # Filling the total residuals
            if errorr != 0 and errorp != 0:
                m_error_p_tot.Fill( errorp)
                m_error_r_tot.Fill( errorr)
                m_errorN_p_tot.Fill( modDict[i].NHitsIter[max-1],errorp*sqN)
                m_errorN_r_tot.Fill( modDict[i].NHitsIter[max-1], errorr*sqN)
                
                  
                # Filling the residuals at the layer level
                if m_modDict[i].lay == 0:
                    m_error_p_0.Fill( errorp)
                    m_error_r_0.Fill( errorr)
                    m_errorN_p_0.Fill( modDict[i].NHitsIter[max-1], errorp*sqN)
                    m_errorN_r_0.Fill( modDict[i].NHitsIter[max-1], errorr*sqN)
                    m_N_0.Fill( sqN )

                if m_modDict[i].lay == 1:
                    m_error_p_1.Fill( errorp)
                    m_error_r_1.Fill( errorr)
                    m_errorN_r_1.Fill( modDict[i].NHitsIter[max-1] , errorr*sqN)
                    m_errorN_p_1.Fill( modDict[i].NHitsIter[max-1],errorp*sqN)
                    m_N_1.Fill( sqN )
                    
                if m_modDict[i].lay == 2:
                    m_error_p_2.Fill( errorp)
                    m_error_r_2.Fill( errorr)
                    m_errorN_r_2.Fill( modDict[i].NHitsIter[max-1], errorr*sqN)
                    m_errorN_p_2.Fill( modDict[i].NHitsIter[max-1], errorp*sqN)
                    m_N_2.Fill( sqN )
                    
    c9 = TCanvas("Number of hits")
    c9.Divide(2,2)
    c9.cd(1)
    m_N_tot.Draw()
    c9.cd(2)
    m_N_0.Draw()
    c9.cd(3)
    m_N_1.Draw("")
    c9.cd(4)
    m_N_2.Draw("")
    c9.Write()

    if "Dx" not in DofNotAligned:
        c3 = TCanvas("Error on module phi positions" )
        c3.Divide(2,2)
        c3.cd(1)
        m_error_p_tot.Draw()
        c3.cd(2)
        m_error_p_0.Draw()
        c3.cd(3)
        m_error_p_1.Draw("")
        c3.cd(4)
        m_error_p_2.Draw("")
        c3.Write()
        
        c7 = TCanvas("Error on module phi positions times sqrt(N)" )
        c7.Divide(2,2)
        c7.cd(1)
        m_errorN_p_tot.Draw()
        c7.cd(2)
        m_errorN_p_0.Draw()
        c7.cd(3)
        m_errorN_p_1.Draw("")
        c7.cd(4)
        m_errorN_p_2.Draw("")
        c7.Write()
        
    if "Dz" not in DofNotAligned:
        c4 = TCanvas("Error on module r positions" )
        c4.Divide(2,2)
        c4.cd(1)
        m_error_r_tot.Draw()
        c4.cd(2)
        m_error_r_0.Draw()
        c4.cd(3)
        m_error_r_1.Draw("")
        c4.cd(4)
        m_error_r_2.Draw("")
        c4.Write()
    
        c8 = TCanvas("Error on module r positions times sqrt(N)" )
        c8.Divide(2,2)
        c8.cd(1)
        m_errorN_r_tot.Draw()
        c8.cd(2)
        m_errorN_r_0.Draw()
        c8.cd(3)
        m_errorN_r_1.Draw("")
        c8.cd(4)
        m_errorN_r_2.Draw("")
        c8.Write()
    
        
    
