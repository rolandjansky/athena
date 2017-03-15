from ROOT import *
include("InDetAlignExample/RootSetup.py")
import sys, string, os


def drawCSCTruth( posfile ):
    m_pull_p_tot = TH1F("m_pull_p_tot"," \hat{#phi} pulls for all layers ", 50, -20,20)
    m_pull_p_tot.SetLineWidth(2)
    m_pull_p_0 = TH1F("m_pull_p_0"," \hat{#phi} pulls for modules in layer 0", 50, -20,20)
    m_pull_p_0.SetLineWidth(2)
    m_pull_p_1 = TH1F("m_pull_p_1"," \hat{#phi} pulls for modules in layer 1", 50, -20,20)
    m_pull_p_1.SetLineWidth(2)
    m_pull_p_2 = TH1F("m_pull_p_2"," \hat{#phi} pulls for modules in layer 2", 50, -20,20)
    m_pull_p_2.SetLineWidth(2)
    
    m_res_p_tot = TH1F("m_res_p_tot"," \hat{#phi} res for all layers ", 50, -0.1,0.1)
    m_res_p_tot.SetLineWidth(2)
    m_res_p_0 = TH1F("m_res_p_0"," \hat{#phi} res for modules in layer 0", 50, -0.1,0.1)
    m_res_p_0.SetLineWidth(2)
    m_res_p_1 = TH1F("m_res_p_1"," \hat{#phi} res for modules in layer 1", 50, -0.1,0.1)
    m_res_p_1.SetLineWidth(2)
    m_res_p_2 = TH1F("m_res_p_2"," \hat{#phi} res for modules in layer 2", 50, -0.1,0.1)
    m_res_p_2.SetLineWidth(2)
    
    m_resN_p_tot = TH2F("m_resN_p_tot"," \hat{#phi} res times sqrt(N) for all layers ", 50, 100,10000, 50,-10,10)
    m_resN_p_tot.SetLineWidth(2)
    m_resN_p_0 = TH2F("m_resN_p_0"," \hat{#phi} res times sqrt(N) for modules in layer 0", 50,100, 10000, 50, -10,10)
    m_resN_p_0.SetLineWidth(2)
    m_resN_p_1 = TH2F("m_resN_p_1"," \hat{#phi} res  times sqrt(N) for modules in layer 1", 50,100,10000, 50, -10,10)
    m_resN_p_1.SetLineWidth(2)
    m_resN_p_2 = TH2F("m_resN_p_2"," \hat{#phi} res  times sqrt(N) for modules in layer 2", 50, 100, 10000, 50, -10,10)
    m_resN_p_2.SetLineWidth(2)
    
    m_pull_r_tot = TH1F("m_pull_r_tot"," \hat{r} pulls for all layers   ", 50, -20,20)
    m_pull_r_tot.SetLineWidth(2)
    m_pull_r_0 = TH1F("m_pull_r_0"," \hat{r} pulls for modules in layer 0", 50, -20,20)
    m_pull_r_0.SetLineWidth(2)
    m_pull_r_1 = TH1F("m_pull_r_1"," \hat{r} pulls for modules in layer 1", 50, -20,20)
    m_pull_r_1.SetLineWidth(2)
    m_pull_r_2 = TH1F("m_pull_r_2"," \hat{r} pulls for modules in layer 2", 50, -20,20)
    m_pull_r_2.SetLineWidth(2)
    
    m_res_r_tot = TH1F("m_res_r_tot"," \hat{r} res for all layers   ", 50, -1,1)
    m_res_r_tot.SetLineWidth(2)
    m_res_r_0 = TH1F("m_res_r_0"," \hat{r} res for modules in layer 0", 50, -1,1)
    m_res_r_0.SetLineWidth(2)
    m_res_r_1 = TH1F("m_res_r_1"," \hat{r} res for modules in layer 1", 50, -1,1)
    m_res_r_1.SetLineWidth(2)
    m_res_r_2 = TH1F("m_res_r_2"," \hat{r} res for modules in layer 2", 50, -1,1)
    m_res_r_2.SetLineWidth(2)
    
    m_resN_r_tot = TH2F("m_resN_r_tot"," \hat{r} res  times sqrt(N) vs N for all layers   ", 50, 100, 10000,50,-100,100)
    m_resN_r_tot.SetLineWidth(2)
    m_resN_r_0 = TH2F("m_resN_r_0"," \hat{r} res  times sqrt(N) vs N for modules in layer 0", 50, 100,10000,50,-100,100)
    m_resN_r_0.SetLineWidth(2)
    m_resN_r_1 = TH2F("m_resN_r_1"," \hat{r} res  times sqrt(N) vs N for modules in layer 1", 50, 100,10000,50,-100,100)
    m_resN_r_1.SetLineWidth(2)
    m_resN_r_2 = TH2F("m_resN_r_2"," \hat{r} res  times sqrt(N) vs N for modules in layer 2", 50, 100,10000,50,-100,100)
    m_resN_r_2.SetLineWidth(2)
    
    m_cor_tot = TH2F("m_cor_tot"," res correlation (phi vs R) for all layers   ", 50, -0.1,0.1, 50, -2, 2)
    m_cor_tot.SetMarkerStyle(7)
    m_cor_0 = TH2F("m_cor_0"," res correlation (phi vs R) for modules in layer 0", 50, -0.1,0.1, 50, -2, 2)
    m_cor_0.SetMarkerStyle(7)
    m_cor_1 = TH2F("m_cor_1"," res correlation (phi vs R) for modules in layer 1", 50, -0.1,0.1, 50, -2, 2)
    m_cor_1.SetMarkerStyle(7)
    m_cor_2 = TH2F("m_cor_2"," res correlation (phi vs R) for modules in layer 2", 50, -0.1,0.1, 50, -2, 2)
    m_cor_2.SetMarkerStyle(7)

    m_modDict={}

    #==== Initialize all the modules which are readout
    for i in range(3):
        for j in range(32):
            if 3*j+i in readOut:
                mod = Module(-1,i,j)
                m_modDict[3*mod.phi+mod.lay] = mod
    #================================================
                
    # Read the referncefile next,  Here we get the local-to-global position.
    file = open(referencefile)

    for line in file:
        words = string.split(line)
        if words.__len__() > 1:
            if words[1] == "trans=":
                m_id = words[0].split('_')
                modId = 3*int(m_id[2])+int(m_id[3])
                if m_id[1] == "-1" and modId in readOut:
                    m_cog = words[2].split(',')
                    m_modDict[modId].SetRefX(m_cog[0].lstrip('('))
                    m_modDict[modId].SetRefY(m_cog[1])
                    m_modDict[modId].SetRefZ(m_cog[2].rstrip(')'))
                    m_modDict[modId].SetRefRotx(words[4].lstrip('(').rstrip(','))
                    m_modDict[modId].SetRefRoty(words[5].rstrip(','))
                    m_modDict[modId].SetRefRotz(words[6].rstrip(')'))
                    

    file.close()
    
    # Read the positionfile next,  Here we get the local-to-global position plus 
    # the displacements recommended by the alignment.
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
            m_modDict[modId].SetNHits( words[4] )
            

        if nlinesAfterModInitialize == 3 and modId in readOut:
            m_modDict[modId].SetErrDx( words[3] )
              
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
            x1 = m_modDict[i].RefX 
            y1 = m_modDict[i].RefY 
            
            x2 = m_modDict[i].CogX 
            y2 = m_modDict[i].CogY 
            
            dx = x2-x1
            dy = y2-y1
            
            # the reference point
            v1 = TVector2(x1,y1)
            # the change
            v2 = TVector2(dx,dy)
            
            # - Projection
            perp2r = v2.Norm(v1)
            
            dxpj = perp2r.Px() 
            dypj = perp2r.Py() 
            dxr  = dx - perp2r.Px()
            dyr  = dy - perp2r.Py()

            #residuals of the r and phi direction
            res_p = pow( pow( dxpj, 2) + pow( dypj, 2) , 0.5)
            res_r = pow( pow( dxr, 2) + pow( dyr, 2) , 0.5)

            #errors on the r and phi direction need to get the error
            errorp = m_modDict[i].ErrDx
            errorr = m_modDict[i].ErrDz
            
            #Setting the signs of the residuals
            sign = v2.DeltaPhi(v1)

            if sign >0:
                res_p *= -1
            else:
                res_p *= 1

            if y1 > 0 and dyr > 0:
                res_r *=1
            elif y1<0 and dyr <0:
                res_r *=1
            else:
                res_r *= -1
            sqN = pow( m_modDict[i].Nhits , 0.5)
            
            # Filling the total residuals
            if errorr != 0 and errorp != 0:
                m_res_p_tot.Fill( res_p)
                m_res_r_tot.Fill( res_r)
                m_resN_p_tot.Fill( m_modDict[i].Nhits,res_p*sqN)
                m_resN_r_tot.Fill( m_modDict[i].Nhits,res_r*sqN)
                m_cor_tot.Fill(res_p, res_r )
                m_pull_p_tot.Fill( res_p/errorp)
                m_pull_r_tot.Fill( res_r/errorr)

                  

                # Filling the residuals at the layer level
                if m_modDict[i].lay == 0:
                    m_res_p_0.Fill( res_p)
                    m_res_r_0.Fill( res_r)
                    m_resN_p_0.Fill( m_modDict[i].Nhits,res_p*sqN)
                    m_resN_r_0.Fill( m_modDict[i].Nhits,res_r*sqN)
                    m_cor_0.Fill(res_p, res_r )
                    m_pull_p_0.Fill( res_p/errorp)
                    m_pull_r_0.Fill( res_r/errorr)
                    
                if m_modDict[i].lay == 1:
                    m_res_p_1.Fill( res_p)
                    m_res_r_1.Fill( res_r)
                    m_resN_p_1.Fill( m_modDict[i].Nhits,res_p*sqN)
                    m_resN_r_1.Fill( m_modDict[i].Nhits,res_r*sqN)
                    m_cor_1.Fill(res_p, res_r )
                    m_pull_p_1.Fill( res_p/errorp)
                    m_pull_r_1.Fill( res_r/errorr)

                if m_modDict[i].lay == 2:
                    m_res_p_2.Fill( res_p)
                    m_res_r_2.Fill( res_r)
                    m_resN_p_2.Fill( m_modDict[i].Nhits,res_p*sqN)
                    m_resN_r_2.Fill( m_modDict[i].Nhits,res_r*sqN)
                    m_cor_2.Fill(res_p, res_r )
                    m_pull_p_2.Fill( res_p/errorp)
                    m_pull_r_2.Fill( res_r/errorr)
    

  
  
    if "Dx" not in DofNotAligned:
        c1 = TCanvas("Residual Misalignments in Phi")
        c1.Divide(2,2)
        c1.cd(1)
        m_res_p_tot.Draw()
        gStyle.SetOptFit()
        c1.cd(2)
        m_res_p_0.Draw()
        c1.cd(3)
        m_res_p_1.Draw("")
        c1.cd(4)
        m_res_p_2.Draw("")
        c1.Write()

        c5 = TCanvas("Residual Misalignments in Phi times sqrt(N)")
        c5.Divide(2,2)
        c5.cd(1)
        m_resN_p_tot.Draw()
        gStyle.SetOptFit()
        c5.cd(2)
        m_resN_p_0.Draw()
        c5.cd(3)
        m_resN_p_1.Draw("")
        c5.cd(4)
        m_resN_p_2.Draw("")
        c5.Write()

        c3 = TCanvas("Phi Pulls")
        c3.Divide(2,2)
        c3.cd(1)
        m_pull_p_tot.Draw()
        gStyle.SetOptFit()
        m_pull_p_tot.Fit("gaus","q")
        c3.cd(2)
        m_pull_p_0.Draw()
        c3.cd(3)
        m_pull_p_1.Draw("")
        c3.cd(4)
        m_pull_p_2.Draw("")
        c3.Write()
        
    if "Dz" not in DofNotAligned:
        c2  = TCanvas("Residual Misalignments in R")
        c2.Divide(2,2)
        c2.cd(1)
        m_res_r_tot.Draw()
        c2.cd(2)
        m_res_r_0.Draw()
        c2.cd(3)
        m_res_r_1.Draw("")
        c2.cd(4)
        m_res_r_2.Draw("")
        c2.Write()
        

        c6 = TCanvas("Residual Misalignments in Phi times sqrt(N)")
        c6.Divide(2,2)
        c6.cd(1)
        m_resN_r_tot.Draw()
        c6.cd(2)
        m_resN_r_0.Draw()
        c6.cd(3)
        m_resN_r_1.Draw("")
        c6.cd(4)
        m_resN_r_2.Draw("")
        c6.Write()

        c4 = TCanvas("R Pulls")
        c4.Divide(2,2)
        c4.cd(1)
        m_pull_r_tot.Draw()
        m_pull_r_tot.Fit("gaus","q")
        c4.cd(2)
        m_pull_r_0.Draw()
        c4.cd(3)
        m_pull_r_1.Draw("")
        c4.cd(4)
        m_pull_r_2.Draw("")
        c4.Write()

    if "Dz" not in DofNotAligned and "Dx" not in DofNotAligned:    
        c10 = TCanvas("Correlation between the phi and the r residual")
        c10.Divide(2,2)
        c10.cd(1)
        m_cor_tot.Draw()
        c10.cd(2)
        m_cor_0.Draw()
        c10.cd(3)
        m_cor_1.Draw("")
        c10.cd(4)
        m_cor_2.Draw("")
        c10.Write()
