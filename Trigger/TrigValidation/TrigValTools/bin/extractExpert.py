#!/usr/bin/env python

#####################################
#                                   #
# Extracting expert-monitoring.root #
#     from an output root file      #
#    	obtained when running       #
#   runHLTMPPU with OH monitoring   #
#                                   #
#     Author: Leonardo Badurina     #
#          Date: 21/08/2018         #
#                                   #
#####################################

if __name__ == "__main__":

    import os
    import sys
    import ROOT
    import subprocess

    print("=== extractExpert.py")

    # Finding non expert-monitoring.root file
    for rootfile in os.listdir(os.getcwd()):
	# Root file obtained using OH monitoring
    	if rootfile.endswith(".root") and rootfile.startswith("r"):
	    print("Rootfile: {}".format(rootfile))
	    break
        elif rootfile.endswith(".root") and rootfile.startswith("expert-monitoring"):
    	    subprocess.call(['mv','expert-monitoring.root', 'child_1'])
	    print("Found an expert-monitoring.root file. Exiting!")
    	    exit(0)

    # Excluding root files that do not correspond to expected output names
    if not rootfile.endswith(".root") and (not rootfile.startswith("r") or not rootfile.startswith("expert-monitoring")):
	print("No suitable file found. Exiting!")
	exit(2)

    # Reading root file
    tfile = ROOT.TFile(rootfile)
    tDir = tfile.GetDirectory("")
    bDirs = tDir.GetListOfKeys()
    baseList = []

    # Finding root number from upper level directory
    for bdir in bDirs:
	baseList.append(bdir.GetName())

    if len(baseList)==1:
        pass
    else:
        print("Too many run numbers. Exiting!")
        sys.exit(2)

    # Finding number of children and child subdirectories
    nDir = tfile.GetDirectory(("{}/lb_-1/HLT-Histogramming".format(baseList[0])))
    nDirs = nDir.GetListOfKeys()
    botList = []
    for bdir in nDirs:
        botList.append(bdir.GetName())
    
    # Copying EXPERT folder from each child directory to expert-monitoring.root
    for i in range(len(botList)):
	print("Extracting expert-monitoring file and saving in child_{}".format(i+1))
	subprocess.call(['rootcp', "-r","{}:{}/lb_-1/HLT-Histogramming/{}/EXPERT/*".format(rootfile,baseList[0],botList[i]),"child_{}/expert-monitoring.root".format(i+1)])
	

