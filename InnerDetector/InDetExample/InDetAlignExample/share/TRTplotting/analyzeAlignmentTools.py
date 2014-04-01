#======function for printing the errors in a decent manner
from math import *

def printerror(val, err):
    if err:
        prec = int(abs(round(log10(err))))
    else:
        prec = 3
    #print str(round(val, prec))+"("+"%.1g)" % err,
    return str(round(val, prec+1))+"("+"%.1g)" % err

#===error handling functions=====
def isOk(dirOrfile):
    if not os.access(dirOrfile,os.F_OK):
        print "The directory/file: "+dirOrfile+" is bogus, or cant be reached from where you are."
        print "Check the path and the spelling."
        sys.exit()
    return ""

def isOkJustGiveWarning(dirOrfile):
    if not os.access(dirOrfile,os.F_OK):
        print "WARNING: The file: "+dirOrfile+" is bogus, looks like failed job."
        return 0
    else:
        return 1

# returns a list of the dof not aligned
def getDofNotAligned():
    currentdir = os.getcwd()

    DofNotAligned = []

    didDx = False
    didDy = False
    didDz = False
    didRotx = False
    didRoty = False
    didRotz = False
    
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        if len(words) == 0:
            continue
        if words[0] == 'dx' and not didDx:
            didDx = True
            print words[0],"aligned"
        if words[0] == 'dy' and not didDy:
            didDy = True
            print words[0],"aligned"
        if words[0] == 'dz' and not didDz:
            didDz = True
            print words[0],"aligned"
        if words[0] == 'rotx' and not didRotx:
            didRotx = True
            print words[0],"aligned"
        if words[0] == 'roty' and not didRoty:
            didRoty = True
            print words[0],"aligned"
        if words[0] == 'rotz' and not didRotz:
            didRotz = True
            print words[0],"aligned"
    file.close()


    if not didDx:
        print "Dx NOT Aligned"
        DofNotAligned.append("Dx")
    if not didDy:
        print "Dy NOT Aligned"
        DofNotAligned.append("Dy")
    if not didDz:
        print "Dz NOT Aligned"
        DofNotAligned.append("Dz")
    if not didRotx:
        print "rotx NOT Aligned"
        DofNotAligned.append("Rotx")
    if not didRoty:
        print "roty NOT Aligned"
        DofNotAligned.append("Roty")
    if not didRotz:
        print "rotz NOT Aligned"
        DofNotAligned.append("Rotz")

    os.chdir(currentdir)

    return DofNotAligned


# returns a list of the dof not aligned
def getDofNotAlignedNew():
    currentdir = os.getcwd()

    DofNotAligned = []

    didDx = False
    didDy = False
    didDz = False
    didRotx = False
    didRoty = False
    didRotz = False
    
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        if len(words) == 0:
            continue
        if words[0] == 'TransX' and not didDx:
            didDx = True
            print words[0],"aligned"
        if words[0] == 'TransY' and not didDy:
            didDy = True
            print words[0],"aligned"
        if words[0] == 'TransZ' and not didDz:
            didDz = True
            print words[0],"aligned"
        if words[0] == 'RotX' and not didRotx:
            didRotx = True
            print words[0],"aligned"
        if words[0] == 'RotY' and not didRoty:
            didRoty = True
            print words[0],"aligned"
        if words[0] == 'RotZ' and not didRotz:
            didRotz = True
            print words[0],"aligned"
    file.close()


    if not didDx:
        print "Dx NOT Aligned"
        DofNotAligned.append("Dx")
    if not didDy:
        print "Dy NOT Aligned"
        DofNotAligned.append("Dy")
    if not didDz:
        print "Dz NOT Aligned"
        DofNotAligned.append("Dz")
    if not didRotx:
        print "rotx NOT Aligned"
        DofNotAligned.append("Rotx")
    if not didRoty:
        print "roty NOT Aligned"
        DofNotAligned.append("Roty")
    if not didRotz:
        print "rotz NOT Aligned"
        DofNotAligned.append("Rotz")

    os.chdir(currentdir)

    return DofNotAligned



# returns the chi2method used
def getChi2Method():
    # local is 0, gobal is 1
    Chi2Method = 0

    currentdir = os.getcwd()
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        if len(words) == 6 and words[1] == "Aligning" and words[3] == "global":
            Chi2Method = 1

    file.close()
    os.chdir(currentdir)

    if Chi2Method:
        print "Aligned using the global method"
    else:
        print "Aligned using the local method"


    return Chi2Method

def getChi2MethodNew():
    # local is 0, gobal is 1
    Chi2Method = 0

    currentdir = os.getcwd()
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        if len(words) == 6 and words[1] == "solving" and words[3] == "Global":
            Chi2Method = 1

    file.close()
    os.chdir(currentdir)

    if Chi2Method:
        print "Aligned using the global method"
    else:
        print "Aligned using the local method"


    return Chi2Method


# returns the chi2method used
def getLevelOfAlignment():
    # 0 L1, 1 is L2 
    LevelOfAlignment = 0

    currentdir = os.getcwd()
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        #['new', 'alignment', 'parameters', 'for', 'TRT/Endcap/Module_-2_0_0']
        if len(words) == 5 and words[0] == "new":
            if len(words[4].split("_")) == 4 and words[4].split("_")[3] != "0":
                LevelOfAlignment = 1
            

    file.close()
    os.chdir(currentdir)

    if LevelOfAlignment:
        print "Reading in the L2 alignment"
    else:
        print "Reading in the L1 alignment"


    return LevelOfAlignment

def getLevelOfAlignmentNew():
    # 0 L1, 1 is L2 
    LevelOfAlignment = 0

    currentdir = os.getcwd()
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        #['new', 'alignment', 'parameters', 'for', 'TRT/Endcap/Module_-2_0_0']
        if len(words) == 5 and words[0] == "Alignment" and words[1] == "parameters":
            if len(words[4].split("_")) == 4 and words[4].split("_")[3] != "0":
                LevelOfAlignment = 1
            

    file.close()
    os.chdir(currentdir)

    if LevelOfAlignment:
        print "Reading in the L2 alignment"
    else:
        print "Reading in the L1 alignment"


    return LevelOfAlignment





def getModuleNumber(bec,lay,phi):
    # Case of L1
    if not LevelOfAlignment:
        if bec == -2:
            return -2
        elif bec == -1:
            return -1
        elif bec == 2:
            return 2
    # Case of L2
    else:
        sign = 1
        if bec<0:
            sign = -1
        if bec == -1:
            #print "returning ",sign*(abs(bec)*1000+lay*100+phi), bec, lay, phi
            return sign*(abs(bec)*1000+lay*100+phi)
        else:
            #print "returning ",sign*(abs(bec)*10000+phi*100+lay), bec, lay, phi
            return sign*(abs(bec)*10000+phi*100+lay)

def getModules():
    modDict = {}

    currentdir = os.getcwd()
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        #['new', 'alignment', 'parameters', 'for', 'TRT/Endcap/Module_-2_0_0']
        if len(words) == 5 and words[0] == "new" and words[1] == "alignment" and words[2] == "parameters":
            keys =  words[4].split("_")
            bec = int(keys[1])
            phi = int(keys[2])
            lay = 0
            if len(keys) > 3:
                lay = int(keys[3])
            moduleNum = getModuleNumber(bec,lay,phi)
            mod = Module(bec,lay,phi)
            modDict[moduleNum] = mod    

    file.close()
    os.chdir(currentdir)
    return modDict

def getThisModule(words):

    if LevelOfAlignment == 0:
        if len(words) == 6 and words[4] == "TRT" and words[5] == "Barrel" :
            mod = Module(-1,0,0)
        elif len(words) == 7 and words[4] == "TRT" and words[5] == "EndCap" and words[6] == "A":
            mod = Module(2,0,0)
        elif len(words) == 7 and words[4] == "TRT" and words[5] == "EndCap" and words[6] == "C":
            mod = Module(-2,0,0)
    else:
        if len(words) == 5:
            ids = words[4].split("_")
            if len(ids) == 4:
                bec = int(ids[1])
                phi = int(ids[2])
                lay = int(ids[3])
                mod = Module(bec,lay,phi)
        
    return mod

def getThisModuleNumber(words):

    if LevelOfAlignment == 0:
        if len(words) == 6 and words[4] == "TRT" and words[5] == "Barrel" :
            moduleNum  = getModuleNumber(-1,0,0)
        elif len(words) == 7 and words[4] == "TRT" and words[5] == "EndCap" and words[6] == "A":
            moduleNum  = getModuleNumber(2,0,0)
        elif len(words) == 7 and words[4] == "TRT" and words[5] == "EndCap" and words[6] == "C":
            moduleNum  = getModuleNumber(-2,0,0)
    else:
        if len(words) == 5:
            ids = words[4].split("_")
            if len(ids) == 4:
                bec = int(ids[1])
                phi = int(ids[2])
                lay = int(ids[3])
                moduleNum = getModuleNumber(bec,lay,phi)
        
    return moduleNum

def getModulesNew():
    modDict = {}

    currentdir = os.getcwd()
    isOk(dirHead)
    os.chdir(dirHead)
    
    isOk(dirPrefix + str(0+skipIter))
    os.chdir(dirPrefix + str(0+skipIter))
    isOk(alignlogfilename)
    file = open(alignlogfilename)

    for line in file:
        words = line.split()
        
        #['Alignment', 'parameters', 'for', 'module:', 'TRT', 'EndCap', 'C']
        if len(words) > 4 and words[0] == "Alignment" and words[1] == "parameters" and words[3] == "module:":
            mod = getThisModule(words)
            moduleNum = getThisModuleNumber(words)
            #else:
            #    keys =  words[4].split("_")
            #    bec = int(keys[1])
            #    phi = int(keys[2])
            #    lay = 0
            #    if len(keys) > 3:
            #        lay = int(keys[3])
            #        moduleNum = getModuleNumber(bec,lay,phi)
            #        mod = Module(bec,lay,phi)
            modDict[moduleNum] = mod    

    file.close()
    os.chdir(currentdir)
    return modDict


def makeCanAndGraph(title,min,max,units):
    can = TCanvas(title,title,700,500)
    graph = TGraphErrors(numIter)
    graph.GetXaxis().SetLimits(-0.2,(numIter)+0.2)
    graph.SetTitle(title)
    graph.SetMaximum(max)
    graph.SetMinimum(min)
    graph.GetXaxis().SetTitle("Iteration")
    graph.GetYaxis().SetTitle(units)
    return can,graph

def makeGraph(title,min,max):
    graph = TGraphErrors(numIter)
    graph.SetTitle(title)
    graph.GetXaxis().SetTitle("Iteration")
    graph.GetXaxis().SetLimits(-0.2,(numIter+1)-0.8)
    graph.SetMaximum(max)
    graph.SetMinimum(min)
    graph.SetMarkerStyle(7)
    graph.SetMarkerSize(2)
    graph.SetLineWidth(2)

    return graph

def drawL2Barrel():
    drawStats()
    #drawModulePlots(newpositionfile)
    if doCSCTruth:
        gDirectory.mkdir("CSCTruth","TRT_Barrel/CSCTruth").cd()
        drawCSCTruth(newpositionfile)

    dxCanAndGraph = [makeCanAndGraph("Change in x Layer 0",-0.2,0.2,"mm")
                     ,makeCanAndGraph("Change in x Layer 1",-0.2,0.2,"mm")
                     ,makeCanAndGraph("Change in x Layer 2",-0.2,0.2,"mm")]

    totxCanAndGraph = [makeCanAndGraph("Total Change in x Layer 0",-0.6,0.6,"mm")
                     ,makeCanAndGraph("Total Change in x Layer 1",-0.6,0.6,"mm")
                     ,makeCanAndGraph("Total Change in x Layer 2",-0.6,0.6,"mm")]

    dyCanAndGraph = [makeCanAndGraph("Change in y Layer 0",-0.2,0.2,"mm")
                     ,makeCanAndGraph("Change in y Layer 1",-0.2,0.2,"mm")
                     ,makeCanAndGraph("Change in y Layer 2",-0.2,0.2,"mm")]

    totyCanAndGraph = [makeCanAndGraph("Total Change in y Layer 0",-0.6,0.6,"mm")
                     ,makeCanAndGraph("Total Change in y Layer 1",-0.6,0.6,"mm")
                     ,makeCanAndGraph("Total Change in y Layer 2",-0.6,0.6,"mm")]

    dzCanAndGraph = [makeCanAndGraph("Change in z Layer 0",-0.2,0.2,"mm")
                     ,makeCanAndGraph("Change in z Layer 1",-0.2,0.2,"mm")
                     ,makeCanAndGraph("Change in z Layer 2",-0.2,0.2,"mm")]

    totzCanAndGraph = [makeCanAndGraph("Total Change in z Layer 0",-0.6,0.6,"mm")
                     ,makeCanAndGraph("Total Change in z Layer 1",-0.6,0.6,"mm")
                     ,makeCanAndGraph("Total Change in z Layer 2",-0.6,0.6,"mm")]

    rotxCanAndGraph = [makeCanAndGraph("Change in Rotx Layer 0",-1e-3,1e-3,"rad")
                       ,makeCanAndGraph("Change in Rotx Layer 1",-1e-3,1e-3,"rad")
                       ,makeCanAndGraph("Change in Rotx Layer 2",-1e-3,1e-3,"rad")]

    totRotxCanAndGraph = [makeCanAndGraph("Total Change in Rotx Layer 0",-1e-2,1e-2,"rad")
                          ,makeCanAndGraph("Total Change in Rotx Layer 1",-1e-2,1e-2,"rad")
                          ,makeCanAndGraph("Total Change in Rotx Layer 2",-1e-2,1e-2,"rad")]

    rotyCanAndGraph = [makeCanAndGraph("Change in Roty Layer 0",-1e-3,1e-3,"rad")
                       ,makeCanAndGraph("Change in Roty Layer 1",-1e-3,1e-3,"rad")
                       ,makeCanAndGraph("Change in Roty Layer 2",-1e-3,1e-3,"rad")]

    totRotyCanAndGraph = [makeCanAndGraph("Total Change in Roty Layer 0",-1e-2,1e-2,"rad")
                          ,makeCanAndGraph("Total Change in Roty Layer 1",-1e-2,1e-2,"rad")
                          ,makeCanAndGraph("Total Change in Roty Layer 2",-1e-2,1e-2,"rad")]

    rotzCanAndGraph = [makeCanAndGraph("Change in Rotz Layer 0",-1e-3,1e-3,"rad")
                       ,makeCanAndGraph("Change in Rotz Layer 1",-1e-3,1e-3,"rad")
                       ,makeCanAndGraph("Change in Rotz Layer 2",-1e-3,1e-3,"rad")]

    totRotzCanAndGraph = [makeCanAndGraph("Total Change in Rotz Layer 0",-1e-2,1e-2,"rad")
                          ,makeCanAndGraph("Total Change in Rotz Layer 1",-1e-2,1e-2,"rad")
                          ,makeCanAndGraph("Total Change in Rotz Layer 2",-1e-2,1e-2,"rad")]

    chi2CanAndGraph = [makeCanAndGraph("Change in Chi2 Layer 0",1e-8,1e4,"#Delta #Chi^{2}")
                       ,makeCanAndGraph("Change in Chi2 Layer 1",1e-8,1e4,"#Delta #Chi^{2}")
                       ,makeCanAndGraph("Change in Chi2 Layer 2",1e-8,1e4,"#Delta #Chi^{2}")]


    for i in range(len(dxCanAndGraph)):
        dxCanAndGraph[i][0].cd()
        dxCanAndGraph[i][1].Draw("APL")
        totxCanAndGraph[i][0].cd()
        totxCanAndGraph[i][1].Draw("APL")

        dyCanAndGraph[i][0].cd()
        dyCanAndGraph[i][1].Draw("APL")
        totyCanAndGraph[i][0].cd()
        totyCanAndGraph[i][1].Draw("APL")

        dzCanAndGraph[i][0].cd()
        dzCanAndGraph[i][1].Draw("APL")
        totzCanAndGraph[i][0].cd()
        totzCanAndGraph[i][1].Draw("APL")

        rotxCanAndGraph[i][0].cd()
        rotxCanAndGraph[i][1].Draw("APL")
        totRotxCanAndGraph[i][0].cd()
        totRotxCanAndGraph[i][1].Draw("APL")

        rotyCanAndGraph[i][0].cd()
        rotyCanAndGraph[i][1].Draw("APL")
        totRotyCanAndGraph[i][0].cd()
        totRotyCanAndGraph[i][1].Draw("APL")

        rotzCanAndGraph[i][0].cd()
        rotzCanAndGraph[i][1].Draw("APL")
        totRotzCanAndGraph[i][0].cd()
        totRotzCanAndGraph[i][1].Draw("APL")

        chi2CanAndGraph[i][0].cd()
        gPad.SetLogy(1)
        chi2CanAndGraph[i][1].Draw("APL")


    for modId in modDict:
        if modDict[modId].bec == -1:
            lay = modDict[modId].lay

            dxCanAndGraph[lay][0].cd()
            modDict[modId].drawDx("PL")
            totxCanAndGraph[lay][0].cd()
            modDict[modId].drawTotDx("PL")

            dyCanAndGraph[lay][0].cd()
            modDict[modId].drawDy("PL")
            totyCanAndGraph[lay][0].cd()
            modDict[modId].drawTotDy("PL")

            dzCanAndGraph[lay][0].cd()
            modDict[modId].drawDz("PL")
            totzCanAndGraph[lay][0].cd()
            modDict[modId].drawTotDz("PL")

            rotxCanAndGraph[lay][0].cd()
            modDict[modId].drawRotx("PL")
            totRotxCanAndGraph[lay][0].cd()
            modDict[modId].drawTotRotx("PL")

            rotyCanAndGraph[lay][0].cd()
            modDict[modId].drawRoty("PL")
            totRotyCanAndGraph[lay][0].cd()
            modDict[modId].drawTotRoty("PL")

            rotzCanAndGraph[lay][0].cd()
            modDict[modId].drawRotz("PL")
            totRotzCanAndGraph[lay][0].cd()
            modDict[modId].drawTotRotz("PL")

            if not Chi2Method:
                chi2CanAndGraph[lay][0].cd()
                modDict[modId].drawChi2("PL")

    for i in range(len(dxCanAndGraph)):
        if "Dx" not in DofNotAligned:
            dxCanAndGraph[i][0].Write()
            totxCanAndGraph[i][0].Write()

        if "Dy" not in DofNotAligned:
            dyCanAndGraph[i][0].Write()
            totyCanAndGraph[i][0].Write()
            
        if "Dz" not in DofNotAligned:
            dzCanAndGraph[i][0].Write()
            totzCanAndGraph[i][0].Write()
            
        if "Rotx" not in DofNotAligned:
            rotxCanAndGraph[i][0].Write()
            totRotxCanAndGraph[i][0].Write()

        if "Roty" not in DofNotAligned:
            rotyCanAndGraph[i][0].Write()
            totRotyCanAndGraph[i][0].Write()

        if "Rotz" not in DofNotAligned:
            rotzCanAndGraph[i][0].Write()
            totRotzCanAndGraph[i][0].Write()

        if not Chi2Method:
            chi2CanAndGraph[i][0].Write()
        
    if not Chi2Method and False:
        for i in range(numIter):
            TotalChi2Graph.SetPoint(i,i,totChi2[i] )
            ctot.cd()
            TotalChi2Graph.Draw("APL")

    if makeVisualization:
        include("TRT_AlignAlgs/visualizeTRTAlignment.py")
        isOk(newpositionfile)
        isOk(referencefile)
        drawVisualization(newpositionfile, referencefile)


def drawL2Endcap(thisEndCap):
    side = "A"
    if thisEndCap == -2:
        side = "C"
        
    dxCanAndGraph = makeCanAndGraph("Change in x Endcap "+side,-0.2,0.2,"mm")
    totxCanAndGraph = makeCanAndGraph("Total Change in x Endcap "+side,-0.6,0.6,"mm")

    dyCanAndGraph = makeCanAndGraph("Change in y Endcap "+side,-0.2,0.2,"mm")
    totyCanAndGraph = makeCanAndGraph("Total Change in y Endcap "+side,-0.6,0.6,"mm")

    dzCanAndGraph = makeCanAndGraph("Change in z Endcap "+side,-0.2,0.2,"mm")
    totzCanAndGraph = makeCanAndGraph("Total Change in z Endcap "+side,-0.6,0.6,"mm")

    rotxCanAndGraph = makeCanAndGraph("Change in Rotx Endcap "+side,-1e-3,1e-3,"rad")
    totRotxCanAndGraph = makeCanAndGraph("Total Change in Rotx Endcap "+side,-1e-2,1e-2,"rad")
                         
    rotyCanAndGraph = makeCanAndGraph("Change in Roty Endcap "+side,-1e-3,1e-3,"rad")
    totRotyCanAndGraph = makeCanAndGraph("Total Change in Roty Endcap "+side,-1e-2,1e-2,"rad")
                         
    rotzCanAndGraph = makeCanAndGraph("Change in Rotz Endcap "+side,-1e-3,1e-3,"rad")
    totRotzCanAndGraph = makeCanAndGraph("Total Change in Rotz Endcap "+side,-1e-2,1e-2,"rad")
                         
    chi2CanAndGraph = makeCanAndGraph("Change in Chi2 Endcap "+side,1e-8,1e4,"#Delta #Chi^{2}")
                      

    dxCanAndGraph[0].cd()
    dxCanAndGraph[1].Draw("APL")
    totxCanAndGraph[0].cd()
    totxCanAndGraph[1].Draw("APL")
    
    dyCanAndGraph[0].cd()
    dyCanAndGraph[1].Draw("APL")
    totyCanAndGraph[0].cd()
    totyCanAndGraph[1].Draw("APL")
    
    dzCanAndGraph[0].cd()
    dzCanAndGraph[1].Draw("APL")
    totzCanAndGraph[0].cd()
    totzCanAndGraph[1].Draw("APL")
    
    rotxCanAndGraph[0].cd()
    rotxCanAndGraph[1].Draw("APL")
    totRotxCanAndGraph[0].cd()
    totRotxCanAndGraph[1].Draw("APL")
    
    rotyCanAndGraph[0].cd()
    rotyCanAndGraph[1].Draw("APL")
    totRotyCanAndGraph[0].cd()
    totRotyCanAndGraph[1].Draw("APL")
    
    rotzCanAndGraph[0].cd()
    rotzCanAndGraph[1].Draw("APL")
    totRotzCanAndGraph[0].cd()
    totRotzCanAndGraph[1].Draw("APL")
    
    chi2CanAndGraph[0].cd()
    gPad.SetLogy(1)
    chi2CanAndGraph[1].Draw("APL")
    
    for modId in modDict:
        if modDict[modId].bec == thisEndCap:

            dxCanAndGraph[0].cd()
            modDict[modId].drawDx("PL")
            totxCanAndGraph[0].cd()
            modDict[modId].drawTotDx("PL")

            dyCanAndGraph[0].cd()
            modDict[modId].drawDy("PL")
            totyCanAndGraph[0].cd()
            modDict[modId].drawTotDy("PL")

            dzCanAndGraph[0].cd()
            modDict[modId].drawDz("PL")
            totzCanAndGraph[0].cd()
            modDict[modId].drawTotDz("PL")

            rotxCanAndGraph[0].cd()
            modDict[modId].drawRotx("PL")
            totRotxCanAndGraph[0].cd()
            modDict[modId].drawTotRotx("PL")

            rotyCanAndGraph[0].cd()
            modDict[modId].drawRoty("PL")
            totRotyCanAndGraph[0].cd()
            modDict[modId].drawTotRoty("PL")

            rotzCanAndGraph[0].cd()
            modDict[modId].drawRotz("PL")
            totRotzCanAndGraph[0].cd()
            modDict[modId].drawTotRotz("PL")

            if not Chi2Method:
                chi2CanAndGraph[0].cd()
                modDict[modId].drawChi2("PL")

    if "Dx" not in DofNotAligned:
        dxCanAndGraph[0].Write()
        totxCanAndGraph[0].Write()

    if "Dy" not in DofNotAligned:
        dyCanAndGraph[0].Write()
        totyCanAndGraph[0].Write()
            
    if "Dz" not in DofNotAligned:
        dzCanAndGraph[0].Write()
        totzCanAndGraph[0].Write()
            
    if "Rotx" not in DofNotAligned:
        rotxCanAndGraph[0].Write()
        totRotxCanAndGraph[0].Write()

    if "Roty" not in DofNotAligned:
        rotyCanAndGraph[0].Write()
        totRotyCanAndGraph[0].Write()

    if "Rotz" not in DofNotAligned:
        rotzCanAndGraph[0].Write()
        totRotzCanAndGraph[0].Write()

    if not Chi2Method:
        chi2CanAndGraph[0].Write()
        

