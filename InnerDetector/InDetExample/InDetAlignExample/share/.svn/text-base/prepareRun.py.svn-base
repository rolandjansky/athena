##########################################################
#             Auxiliary code                             #
##########################################################
def getLBrange (runNumber):
    firstLB = 1
    lastLB = 1200
    if (runNumber == 280464): firstLB = 401; lastLB = 900
        
    if (runNumber == 281411): firstLB = 1; lastLB = 1000
    if (runNumber == 282625): firstLB = 1; lastLB = 200
    if (runNumber == 282631): firstLB = 101; lastLB = 300
    if (runNumber == 282712): firstLB = 301; lastLB = 900
    if (runNumber == 282784): firstLB = 101; lastLB = 300
    if (runNumber == 282992): firstLB = 101; lastLB = 800
    if (runNumber == 283074): firstLB = 101; lastLB = 500
    if (runNumber == 283155): firstLB = 101; lastLB = 300
    if (runNumber == 283270): firstLB = 1; lastLB = 100
    if (runNumber == 283429): firstLB = 101; lastLB = 1500
    if (runNumber == 283608): firstLB = 1; lastLB = 200
    if (runNumber == 283780): firstLB = 201; lastLB = 1100
    if (runNumber == 284006): firstLB = 1; lastLB = 400
    if (runNumber == 284154): firstLB = 101; lastLB = 300
    if (runNumber == 284213): firstLB = 1; lastLB = 1100
    if (runNumber == 284285): firstLB = 201; lastLB = 1500
    if (runNumber == 284420): firstLB = 101; lastLB = 400
    if (runNumber == 284427): firstLB = 101; lastLB = 300
    if (runNumber == 284473): firstLB = 101; lastLB = 400
    if (runNumber == 284484): firstLB = 1; lastLB = 600
        
    lbList = [firstLB, lastLB]
    return lbList


##########################################################
#             Main code                                  #
##########################################################

if __name__ == '__main__':
    import shutil
    import os
    import sys
    from optparse import OptionParser
    
    parser = OptionParser()
    parser.add_option("--run", type="int", dest="num")
    (options, args) = parser.parse_args()

    print '###############################################'
    print '####### Setting up directory for Run', options.num
    print '###############################################'
    
    
    testarea = os.environ['TestArea']
    mypath = './Run_%i' % options.num
    run_command = '%s/run_command' % mypath
    if not os.path.isdir(mypath):
        os.makedirs(mypath)
        
    # find out LBranges
    lbrange = getLBrange(options.num)
    firstLB = lbrange[0]
    lastLB = lbrange[1]
    #print " LBrange: ", firstLB, " --> ", lastLB 
        
    f1=open(run_command, 'w+')
    f1.write("python TotalIterator.py --inputList \"/afs/cern.ch/work/m/martis/public/athena/BowTest/data15_13TeV/data/Run_%i_calibration_IDTracks.txt\" --eventType \"collisions\" --isData True --nEvents 100000 --nCpus 20 --ptmin 5000 --inputBowingDb \"%s/InnerDetector/InDetExample/InDetAlignExample/BowingDB/BowingNULL.db\" --useLBselector --FirstLB %i --LastLB %i --NLB 100 --suffix %i --nIter 2 --queue 8nh" % (options.num, testarea, firstLB, lastLB, options.num))
        
    iterFile = '%s/TotalIterator.py' % mypath
    getFile = '%s/InnerDetector/InDetExample/InDetAlignExample/share/TotalIterator.py' % testarea
    shutil.copy2(getFile, iterFile)

