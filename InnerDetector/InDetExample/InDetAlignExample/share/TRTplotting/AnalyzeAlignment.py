#===========================================================================
#===========================================================================
# Author: John Alison (johnda) 
# June 17, 2008
#===========================================================================
#===========================================================================
# This is a script which creates a rootfile for displaying the results 
# of the L1 and L2 alignment from the TRTAlignAlg.  It assumes the different
# iterations of the algorithm are in seperate directories, located under
# a common directory head, and named with a common prefix which ends in 
# a number(starting with 0+skipIters) specifing the iteration.   
#
# Eg:
#  > ls TRTL2Alignment/
#    Iter0 Iter1 Iter2 Iter3 Iter4 ...
#
# To run the script change the options below and run,
#
# > athena AnalyzeTRTAlignment.py
#
# This should generate a table of the total alignment changes, and a rootfile
# containing changes vs iteration for the entire TRT and on a module by module
# basis and a visualization of the alignment changes between the two files
# specified below.

#============ Output file name ===========================
Outputfile = "AnalyzeAlignment.root"

#============= Path to the directory where the iterations are stored
dirHead = "/afs/cern.ch/user/a/atlidali/w1/users/johnda/testNewIterator"

#============= Prefix of the directory ===================
#(eg dirPrefix = Iter if iterations as given in the above example)
dirPrefix = "Iter"

#============= Number of iterations ======================
numIter = 10
skipIter = 10

#============= Are there sub jobs ? ======================
# This Needs fixed
numSubJobs = 0

#======================================
include("InDetAlignExample/makeAnalysis.py")
