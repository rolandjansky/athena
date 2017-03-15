#===========================================================================
#===========================================================================
# Description:
#  File create the subdirectories in the output file
#
# Author:
#  John Alison <johnda@hep.upenn.edu>
#===========================================================================

madeDirectories = False
def makeDirectories(moduleNames,trackCollectionNames):
    subSystemDirs = ["General","TRT","SCT","Pixel"]
    becNames = ["Barrel","Endcap_A","Endcap_C","General"]
    sideNames = ["Side_A","Side_C"]

    for sub in subSystemDirs:
        thisSubDir = rootfile.mkdir(sub)
        if sub == "TRT":
            for bec in becNames:
                if bec == "Barrel":
                    thisBecDir = thisSubDir.mkdir(bec)
                    for track in trackCollectionNames:
                        thisTrakDir = thisBecDir.mkdir(track)
                        for mod in moduleNames:
                            thisTrakDir.mkdir(mod)

                    for side in sideNames:
                        thisSideDir = thisBecDir.mkdir(side)
                        for track in trackCollectionNames:
                            thisTrakDir = thisSideDir.mkdir(track)
                            for mod in moduleNames:
                                thisTrakDir.mkdir(mod)
                            
                else:
                    thisBecDir = thisSubDir.mkdir(bec)
                    for track in trackCollectionNames:
                        thisTrakDir = thisBecDir.mkdir(track)
                        for mod in moduleNames:
                            thisTrakDir.mkdir(mod)
        else:
            for track in trackCollectionNames:
                thisTrakDir = thisSubDir.mkdir(track)
                for mod in moduleNames:
                    thisTrakDir.mkdir(mod)
        rootfile.cd()
                
    return

