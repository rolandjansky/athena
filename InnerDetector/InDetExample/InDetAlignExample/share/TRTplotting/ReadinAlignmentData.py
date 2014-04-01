#=======================================================#
#==========Reading in the data==========================#
#=======================================================#

#====get the current directory===
currentdir = os.getcwd()

isOk(dirHead)
os.chdir(dirHead)
print "Reading", dirHead

Chi2Method = getChi2Method()

for iter in range(numIter):
    if (iter < numIter): 
        isOk(dirPrefix + str(iter+skipIter))
        os.chdir(dirPrefix + str(iter+skipIter))
        isOk(alignlogfilename)
        file = open(alignlogfilename)
        print "Reading From Directory ", dirPrefix+str(iter+skipIter)
        totChi2[iter] = 0
        readhalf = False
        for line in file:
            words = string.split(line)
            if words.__len__() >0:
                if( words.__len__()>4 and words[3] == "processed" and words[4] == "tracks:" and not numSubJobs): 
                    procTracks[iter] = int(words[5])
                if( words.__len__()>4 and words[3] == "processed" and words[4] == "hits:" and not numSubJobs): 
                    procHits[iter] = int(words[5])
                if( words.__len__()>4 and words[1] == "total" and words[2] == "chisquare" and not numSubJobs):
                    totChi2[iter] = float(words[5])
                if( not Chi2Method and words.__len__()>4 and words[1] == "chisquare/ndof" ):#change in chi2 in the local method
                    chngChi2[iter] = float(words[6].split('/')[0])
                if ( Chi2Method and words[0] == 'chisquare/ndof' and words[5] == "(global-method):" ):#change in chi2 in the global method
                    chngChi2[iter] = float(words[6])
                if( Chi2Method and words.__len__() > 2 and words[1] == "Aligning" and words[3] == "global"):
                    readhalf = True
                if( not Chi2Method or readhalf):#Stupid Hack b/c in global method, both global and local solutions are written
                    if len(words) == 5 and words[0] == "new" and words[1] == "alignment" and words[2] == "parameters":
                        keys =  words[4].split("_")
                        bec = int(keys[1])
                        phi = int(keys[2])
                        lay = 0
                        if len(keys) > 3:
                            lay = int(keys[3])
                        moduleNum = getModuleNumber(bec,lay,phi)

                    if ( words[0] == "number" and words[2] == "hits" and moduleNum in modDict):
                        modDict[moduleNum].AddNHits(words[4])
                    if ( words[0] == "dx" and moduleNum in modDict):
                        modDict[moduleNum].AddDxIter(words[1],words[3])
                    if ( words[0] == "dy" and moduleNum in modDict):
                        modDict[moduleNum].AddDyIter(words[1],words[3])
                    if ( words[0] == "dz" and moduleNum in modDict):
                        modDict[moduleNum].AddDzIter(words[1],words[3])
                    if ( words[0] == "rotx" and moduleNum in modDict):
                        modDict[moduleNum].AddRotxIter(words[1],words[3])
                    if ( words[0] == "roty" and moduleNum in modDict):
                        modDict[moduleNum].AddRotyIter(words[1],words[3])
                    if ( words[0] == "rotz" and moduleNum in modDict):
                        modDict[moduleNum].AddRotzIter(words[1],words[3])
                    if ( not Chi2Method and words[0] == 'chisquare/dof' and words[3] == "alignment" and moduleNum in modDict ): #module only have a change in chi2 for the local method
                        modDict[moduleNum].AddChi2Iter(words[5])
                    
        file.close()
        os.chdir("..")

print "================== Reading the subjobs ==========================="
if numSubJobs:
    os.chdir(dirHead)
    print os.getcwd()
    for iter in range(numIter):
        procTracks[iter] = 0
        procHits[iter] = 0
        totChi2[iter] = 0
        print "Reading From Sub-Directories in ", dirPrefix+str(iter+skipIter)
        for subJob in range(numSubJobs):
            if isOkJustGiveWarning(dirPrefix + str(iter+skipIter)+"_"+str(subJob)):
                os.chdir(dirPrefix + str(iter+skipIter)+"_"+str(subJob))
                if isOkJustGiveWarning(alignlogfilename):
                    file = open(alignlogfilename)
                    for line in file:
                        words = string.split(line)
                        if words.__len__() >0:
                            if( words.__len__()>4 and words[3] == "processed" and words[4] == "tracks:"): 
                                procTracks[iter] += int(words[5])
                            if( words.__len__()>4 and words[3] == "processed" and words[4] == "hits:"): 
                                procHits[iter] += int(words[5])
                            if( words.__len__()>4 and words[1] == "total" and words[2] == "chisquare"):
                                totChi2[iter] += float(words[5])
                        
                    file.close()
                os.chdir("..")

print "================== Getting the EigenValues  ==========================="
if Chi2Method:
    os.chdir(dirHead+dirPrefix + str(numIter-1))
    file = open(alignlogfilename)
    readDataFlag = False
    for line in file:
        words = string.split(line)
        #Trun off the reading of eigen values
        if words.__len__() == 3 and words[0] == "End" and words[1] == "Eigenvalue" :
            readDataFlag = False

        #read the eigen values
        if readDataFlag:
            if words[0] == "solveLapack:":
                eigenValues.append(float(words[3]))
            else:
                eigenValues.append(float(words[0]))

        #Trun on the reading of eigen values
        if words.__len__() ==3 and words[0] == "The" and words[1] == "Eigenvalue" :
            readDataFlag = True
    eigenValues.sort()
    file.close()
    
    
#===return to the original directory====
os.chdir(currentdir)
print "We've read in all the data ============= Exiting ReadinAlignmentData.py"


