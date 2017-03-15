from math import log
#=======================================================#
#Make the Root file by calling the afore defined functions==========
#=======================================================#
rootfile = TFile(Outputfile, "RECREATE")

# If we do L2
if LevelOfAlignment == 1:
    
    dir = rootfile.mkdir("TRT_Barrel", "TRT Barrel")
    dir.cd()
    drawL2Barrel()

    dir = rootfile.mkdir("TRT_EndcapC", "TRT Endcap C")
    dir.cd()
    drawL2Endcap(-2)

    dir = rootfile.mkdir("TRT_EndcapA", "TRT Endcap A")
    dir.cd()
    drawL2Endcap(2)
    
    print "|=================================================================================================================================|"
    print "|==========================================     New L2 Alignment Constants    ====================================================|"
    print "|=================================================================================================================================|"
    print "|      Module       |",
    if "Dx" not in DofNotAligned:
        print "       Dx           |",
    if "Dz" not in DofNotAligned:
        print "       Dz           |",
    if "Rotx" not in DofNotAligned:
        print "       Rotx         |",
    if "Roty" not in DofNotAligned:
        print "       Roty         |",
    if "Rotz" not in DofNotAligned:
        print "       Rotz         |",

    print ""
    print "|---------------------------------------------------------------------------------------------------------------------------------|"    

    for modId in modDict:
        m_lay = modDict[modId].lay
        m_phi = modDict[modId].phi
        m_bec = modDict[modId].bec

        if m_bec == -1:
            rootfile.cd("TRT_Barrel")
        elif m_bec == -2:
            rootfile.cd("TRT_EndcapC")
        elif m_bec == 2:
            rootfile.cd("TRT_EndcapA")

        dirname = "Module_"+str(m_bec)+"_"+str(m_lay)+"_"+str(m_phi)
        gDirectory.mkdir(dirname,"TRT_/"+dirname).cd()
        drawModule(modDict[modId])

    print "|=================================================================================================================================|"
    print "|=================================================================================================================================|"
    print "|=================================================================================================================================|"

if LevelOfAlignment == 0:
    drawStats()

    print "|=================================================================================================================================|"
    print "|==========================================     New L1 Alignment Constants    ====================================================|"
    print "|=================================================================================================================================|"
    print "|      Module       |",

    if "Dx" not in DofNotAligned:
        print "       Dx           |",
    if "Dy" not in DofNotAligned:
        print "       Dy           |",
    if "Dz" not in DofNotAligned:
        print "       Dz           |",
    if "Rotx" not in DofNotAligned:
        print "       Rotx         |",
    if "Roty" not in DofNotAligned:
        print "       Roty         |",
    if "Rotz" not in DofNotAligned:
        print "       Rotz         |",

    print ""
    print "|---------------------------------------------------------------------------------------------------------------------------------|" 


    if -2 in modDict:
        dir = rootfile.mkdir("TRT_EndCapC", "TRT EndCap C")
        dir.cd()
        drawModule(modDict[-2])
    if -1 in modDict:
        dir = rootfile.mkdir("TRT_Barrel", "TRT Barrel")
        dir.cd()
        drawModule(modDict[-1])
    if +2 in modDict:
        dir = rootfile.mkdir("TRT_EndCapA", "TRT EndCap A")
        dir.cd()
        drawModule(modDict[2])


rootfile.cd()
if Chi2Method:
    c0 = TCanvas("Eigen Value Spectrum")
    c0.SetGridx()
    c0.SetGridy()
    c0.SetLogy(1)
    eigenValueHist = TH1F("m_eigenValueHist","Eigen value spectrum",100,-20,20)
    for i in range(eigenValues.__len__()):
        eigenValueHist.Fill(log(eigenValues[i],10))
    eigenValueHist.SetFillColor(kGray+1)
    
    eigenValueHist.Draw()
    c0.Write()

    print "|=================================================================================================================================|"
    print "|=================================================================================================================================|"
    print "|=================================================================================================================================|"
    

rootfile.Close()

print "|=================================================================================================================================|"
