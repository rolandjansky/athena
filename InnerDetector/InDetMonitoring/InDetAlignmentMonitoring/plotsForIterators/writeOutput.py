#===========================================================================
#===========================================================================
# Description:
#  This File contains the functions to determine where to write the output histogram
#  and actually do it
#
# Author:
#  John Alison <johnda@hep.upenn.edu>
#===========================================================================

def getBEC(histName):
    if histName.find("_b") > -1:
        return "Barrel"
    elif (histName.find("Endcap_A") > -1 or histName.find("_eca") > -1):
        return "Endcap_A"
    elif (histName.find("Endcap_C") > -1 or histName.find("_ecc") > -1):
        return "Endcap_C"
    else:
        return "General"

def getSide(histName):
    if histName.find("Side_A") > -1:
        return "Side_A"
    elif histName.find("Side_C") > -1:
        return "Side_C"
    else:
        return 0
    
def getSubSystem(histName):
    if histName.find("pix") > -1:
        return "Pixel"
    elif histName.find("sct") > -1:
        return "SCT"
    elif histName.find("trt") > -1:
        return "TRT"
    else:
        return "General"

def getDir(histName,moduleName,trackCollectionName):
    tagName = getSubSystem(histName)
    if tagName == "TRT":
        becName = getBEC(histName)
        if becName == "Barrel":
            sideName = getSide(histName)
            if sideName:
                thisDir = rootfile.GetDirectory(tagName+"/"+becName+"/"+sideName+"/"+trackCollectionName+"/"+moduleName)
            else:
                thisDir = rootfile.GetDirectory(tagName+"/"+becName+"/"+trackCollectionName+"/"+moduleName)
                #print tagName+"/"+becName+"/"+trackCollectionName+"/"+moduleName
        else:
            thisDir = rootfile.GetDirectory(tagName+"/"+becName+"/"+trackCollectionName+"/"+moduleName)
            
    else:
        thisDir = rootfile.GetDirectory(tagName+"/"+trackCollectionName+"/"+moduleName)

    if not thisDir:
        print "ERROR: the directory ",tagName+"/"+trackCollectionName+"/"+moduleName, "does noe exist!"
        #print histName
        #print becName
        #print sideName
        print "exiting...."
        sys.exit()

    return thisDir

def writeCan(can,histName,moduleName,trackCollectionName):
    correctDir = getDir(histName,moduleName,trackCollectionName)
    correctDir.cd()
    can.Write()
    return

