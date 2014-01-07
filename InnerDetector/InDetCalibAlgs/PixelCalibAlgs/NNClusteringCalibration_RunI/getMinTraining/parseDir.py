# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from fancyTab import *

def findMin(path):
    minimum=10000
    epochWithMinimum=-1
       
    import os
    command = "cp "+path+" ."
    os.system(command)
    os.system('source cleanFile.sh')
    
    
    for line in file('chron.txt'):
        line = line.split()
        epoch = int(line[0])
        trainingError = float(line[1])
        testError = float(line[2])
        if testError < minimum:
            minimum = testError
            epochWithMinimum = epoch

    return epochWithMinimum , minimum
        


path="/afs/cern.ch/user/g/giacinto/scratch0/Athena/rel_2/InnerDetector/InDetCalibAlgs/PixelCalibAlgs/NnClusteringCalibration/"

test_table = []


import os
print "-----------------------------------------------------------------------------------------------------------"

print "Now looking for the best current minimum of the training in directory: "
print path
print "-----------------------------------------------------------------------------------------------------------"
print " "
#print "Network type | \t parameters | \t epoch | \t minimum "
test_table.append(["Network Type", "Parameters", "Epoch", "Minimum"])

for dir in [ "number", "positions" , "errors" ]:
    wh = path+dir
    listed = [ name for name in os.listdir(wh) if os.path.isdir(os.path.join(wh, name)) ]
    
    resDir =  filter( lambda dirname: dirname[0]!="L" and  dirname[0]!="."  , listed)

    for weightsDir in resDir:
        weightPath = wh+"/"+weightsDir
        listedWeights = [ name for name in os.listdir(weightPath) if os.path.isdir(os.path.join(weightPath , name)) ]
        filteredListW=  filter( lambda dirname: dirname[0]!="L" and  dirname[0]!="."  , listedWeights)
        for w in filteredListW:
            chronFile = weightPath+"/"+w+"/trainingCronology.txt"
            epochAtMin =-1
            min =-1
            if os.path.exists(chronFile):
                epochAtMin , min = findMin(chronFile)
                test_table.append([dir,weightsDir ,str(epochAtMin) ,str(min) ])

    test_table.append(["------------","---------------------" ,"------" ,"----------"])

#            print dir+"\t"+weightsDir+"\t"+w+"\t"+str(epochAtMin)+"\t"+str(min)
#            print dir+" |\t| "+weightsDir+" \t \t "+str(epochAtMin)+" \t "+str(min)


print_table(test_table)

        
