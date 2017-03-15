#==========================================================#
#===========Setup for Alignment Histograms=================#
#==========================================================#
include("InDetAlignExample/analyzeAlignmentTools.py")
doNew = True

# Getting the LevelOfAlignment 
if doNew:
    LevelOfAlignment = getLevelOfAlignmentNew()
else:
    LevelOfAlignment = getLevelOfAlignment()


if LevelOfAlignment == 0:

    print "|=================================================================================================================================|"
    print "|========================================== Reading the Global (L1) TRT alignment ================================================|"
    print "|=================================================================================================================================|"
        

elif LevelOfAlignment == 1:

    print "|=================================================================================================================================|"
    print "|========================================== Reading the Internal (L2) TRT alignment ==============================================|"
    print "|=================================================================================================================================|"

else:
    print "LevelOfAlignment is not set properly"
    sys.exit(0)

#==========================================================#
#===========Initializing the modules and containers========#
#==========================================================#

#Create modules that aren't masked and store those that are read out==========#
if doNew:
    modDict = getModulesNew()
    DofNotAligned = getDofNotAlignedNew()
else:
    modDict = getModules()
    DofNotAligned = getDofNotAligned()
    
print modDict
# Getting the DoF not aligned


#==========write out dofs which aren't being aligned=========
for i in range(DofNotAligned.__len__()):
    print "!========================== Warning, ",DofNotAligned[i], " is being ignored! ==================!"

if doCSCTruth:
    print "!========================== Warning, we are comparing to the CSC truth   ==================!"

#=============== Things needed for both the L1 and L2 alignment =================
#====total chi2====#
totChi2    = {}

#====proccessed Tracks====#
procTracks = {}

#====proccessed Hits====#
procHits   = {}

#====Change in Chi2====#
chngChi2   = {}

#====Eigen values for global ===#
eigenValues = []

#==========================================================#
#=============Initial CSC misalignments====================#
#==========================================================#
if doCSCTruth:
    path = str(os.environ['TestArea'])+"/InnerDetector/InDetAlignAlgs/TRT_AlignAlgs/share/"
    file = open(path+"CSCLocalL2InitialPhiMisAlignments.txt")
    for line in file:
        words = string.split(line)
        modId = 3*int(words[1])+int(words[0])
        if modId in readOut:
            modDict[modId].initialPositionDx = float(words[2])

    file.close()

    file = open(path+"CSCLocalL2InitialRadialMisAlignments.txt")
    for line in file:
        words = string.split(line)
        modId = 3*int(words[1])+int(words[0])
        if modId in readOut:
            modDict[modId].initialPositionDz = float(words[2])
    file.close()

#==============================================================#
# Setting up graphs/histograms all iterations (or just last one) 
#==============================================================#

#===Setting up the Chi2 graph===#
TotalChi2Graph = makeGraph("Total Chi2 vs Iteration",0,1e7)

#===Setting up the change in Chi2 graph===#
ChangeInChi2Graph = makeGraph("Change in Chi2 vs Iteration",1,4e6)

#===Setting up the #tracks graph===#
ProcTracksGraph = makeGraph("Tracks vs Iteration",0,35e3)

#===Setting up the #hits graph===#
ProcHitsGraph = makeGraph("Hits vs Iteration",0,1.5e6)

#=============Draw General Statistics from the alignment iterations=======
def drawStats():
    for i in range(numIter):
        try:
            totChi2[i]
            TotalChi2Graph.SetPoint(i,i,totChi2[i] )
        except:
            print "totChi2 not filled"

        try:
            chngChi2[i]
            ChangeInChi2Graph.SetPoint(i,i,chngChi2[i] )
        except:
            print "changeinchi2 not filled"

        try:
            procTracks[i]
            ProcTracksGraph.SetPoint(i,i,procTracks[i] )
        except:
            print "procTracks not filled"

        try:
            procHits[i]
            ProcHitsGraph.SetPoint(i,i,procHits[i] )
        except:
            print "procHits not filled"
        
    c0 = TCanvas("Statistics vs iteration","title",1050,750)
    c0.Divide(2,2)
    c0.cd(1)
    TotalChi2Graph.Draw("APL")
    c0.cd(2)
    gPad.SetLogy(1)
    ChangeInChi2Graph.Draw("APL")
    c0.cd(3)
    ProcTracksGraph.Draw("APL")
    c0.cd(4)
    ProcHitsGraph.Draw("APL")
    c0.Write()
    return c0

#=============Draw the information on a module by module basis=======
def drawModule(module):
    bec = module.bec
    lay = module.lay
    phi = module.phi

    printout = "|        "+str(bec)+"/"+str(lay)+"/"+str(phi)+"     "
    while printout.__len__() < 20:
        printout = printout + " "
    print printout+"|",

    
    m_c2 = TCanvas("Change in Chi2","Chi2 for module: Layer:"+str(lay)+" Phi "+str(phi),700,500)
    m_c1 = TCanvas("Change in Rotations","Change in Rotations for(Black=rotx, Red=roty, Blue=rotz) module: Layer:"+str(lay)+" Phi "+str(phi),700,500)
    m_c3 = TCanvas("Total Rotations","Total Rotations for(Black=rotx, Red=roty, Blue=rotz) module: Layer:"+str(lay)+" Phi "+str(phi),700,500)

    m_c0 = TCanvas("Change in Translations","Change in Translations (Black=dx, Red=dy, Blue=dz) for module: Layer:"+str(lay)+" Phi "+str(phi),700,500)
    m_c4 = TCanvas("Total Translations","Total Translations (Black=dx, Red=dy, Blue=dz) for module: Layer:"+str(lay)+" Phi "+str(phi),700,500)
    m_c5 = TCanvas("Number of Hits","Number of hits seen for module: Layer:"+str(lay)+" Phi "+str(phi),700,500)
    
    zeroLine.Draw("same")
    #Change in Translations
    m_c0.cd()

    #Dx
    if "Dx" not in DofNotAligned:
        module.drawDx("APL")
        printout =  "    "+printerror(module.tot["dx"], module.err["dx"])
        
        while printout.__len__() < 20:
            printout = printout + " "
        print printout+"|",

        
    #Dy
    if "Dy" not in DofNotAligned:

        if "Dx" in DofNotAligned:
            module.drawDy("APL")
        else:
            module.drawDy("PL")

        printout =  "    "+printerror(module.tot["dy"], module.err["dy"])
        
        while printout.__len__() < 20:
            printout = printout + " "
        print printout+"|",

    #Dz
    if "Dz" not in DofNotAligned:

        if "Dx" in DofNotAligned and "Dy" in DofNotAligned:
            module.drawDz("APL")
        else:
            module.drawDz("PL")

        printout =  "    "+printerror(module.tot["dz"], module.err["dz"])
        while printout.__len__() < 20:
            printout = printout + " "
        print printout+"|",
            
    zeroLine.Draw("same")
    #=======Total Translations
    m_c4.cd()

    #Dx
    if "Dx" not in DofNotAligned:
        module.drawTotDx("APL")

    #Dy
    if "Dy" not in DofNotAligned:
        if "Dx" in DofNotAligned:
            module.drawTotDy("APL")
        else:
            module.drawTotDy("PL")

    #Dz
    if "Dz" not in DofNotAligned:
        if "Dx" in DofNotAligned and "Dy" in DofNotAligned:
            module.drawTotDz("APL")
        else:
            module.drawTotDz("PL")

    zeroLine.Draw("same")
    #=======Change in Rotations
    m_c1.cd()

    #Rotx
    if "Rotx" not in DofNotAligned:

        module.drawRotx("APL")
        printout =  "  "+printerror(module.tot["rotx"], module.err["rotx"])
        while printout.__len__() < 20:
            printout = printout + " "
        print printout+"|",

    
    #Roty
    if "Roty" not in DofNotAligned:

        if "Rotx" in DofNotAligned:
            module.drawRoty("APL")
        else:
            module.drawRoty("PL")
            
        printout =  "  "+printerror(module.tot["roty"], module.err["roty"])
        while printout.__len__() < 20:
            printout = printout + " "
        print printout+"|",


    #Rotz
    if "Rotz" not in DofNotAligned:

        if "Rotx" in DofNotAligned and "Roty" in DofNotAligned:
            module.drawRotz("APL")
        else:
            module.drawRotz("PL")
        printout =  "  "+printerror(module.tot["rotz"], module.err["rotz"])
        while printout.__len__() < 20:
            printout = printout + " "
        print printout+"|",
        
    zeroLine.Draw("same")
    #=========Total Rotations
    m_c3.cd()

    #Rotx
    if "Rotx" not in DofNotAligned:
        module.drawTotRotx("APL")

    #Roty
    if "Roty" not in DofNotAligned:
        if "Rotx" in DofNotAligned:
            module.drawTotRoty("APL")
        else:
            module.drawTotRoty("PL")

    #Rotz
    if "Rotz" not in DofNotAligned:
        if "Rotx" in DofNotAligned and "Roty" in DofNotAligned:
            module.drawTotRotz("APL")
        else:
            module.drawTotRotz("PL")

    print ""

    zeroLine.Draw("same")
    #========The Chi2
    
    m_c2.cd()
    gPad.SetLogy(1)
    module.drawChi2("APL")

    #=========Total Rotations
    m_c5.cd()
    
    module.NHitGraph = TGraphErrors(module.NHitsIter.__len__())
    #module.tot["chi2"] = 0.0
    module.NHitGraph.SetTitle("N Hits vs Iterations for Layer"+str(module.lay)+"and Phi Module"+str(module.phi))
    module.NHitGraph.GetXaxis().SetTitle("Iteration")
    module.NHitGraph.GetXaxis().SetLimits(-0.2,(module.NHitsIter.__len__()+1)-0.8)
    module.NHitGraph.SetMaximum(1e6)
    module.NHitGraph.SetMinimum(0)
    module.NHitGraph.SetMarkerStyle(3)
    module.NHitGraph.SetMarkerSize(2)
    module.NHitGraph.SetMarkerColor(1)
    module.NHitGraph.SetLineColor(1)
    for i in range(module.NHitsIter.__len__()):
        module.NHitGraph.SetPoint(i,i,module.NHitsIter[i])
        #module.tot["chi2"] += module.Chi2Iter[i]
    module.NHitGraph.Draw("APL")

    if "Dx" not in DofNotAligned or "Dz" not in DofNotAligned: 
        m_c0.Write()
        m_c4.Write()
    if "Rotx" not in DofNotAligned or "Roty" not in DofNotAligned or "Rotz" not in DofNotAligned:
        m_c1.Write()
        m_c3.Write()
    if not Chi2Method:
        m_c2.Write()
    m_c5.Write()
    
    return m_c2, m_c1, m_c0, m_c4

#======Drawing the line at zero in all these plots
zeroLine = TLine(0,0,numIter-1+0.3,0)

#===============================
if LevelOfAlignment == 1:
    include("InDetAlignExample/ModulePlots.py")
    #===============================
    if doCSCTruth:
        include("InDetAlignExample/CSCTruthPlots.py")

print "We've loaded all the functions ======== Exiting AlignmentHistoSetup.py"
