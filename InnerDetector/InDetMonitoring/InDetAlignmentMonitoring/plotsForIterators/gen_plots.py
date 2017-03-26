#!/usr/bin/python

import sys,os,subprocess

dirname=sys.argv[1]
prename=sys.argv[2]
suffix=sys.argv[3]
runnum=sys.argv[4]

proc = subprocess.Popen("ls " + dirname + "/" + prename+"*/TotalMonitoring.root | wc -l", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
n_iters = int( proc.stdout.readline() )

os.system( "rm -rf ../plots_" + prename )

monfiles = [ dirname + "/" + prename + "_Iter" + str(i) + "_" +suffix+ "/TotalMonitoring.root" for i in range(0, n_iters) ]
logfiles = [ dirname + "/" + prename + "_Iter" + str(i) + "_" +suffix+ "/alignlogfile.txt"     for i in range(0, n_iters) ]

colors   = [ "kGray+1", "kRed+2", "kBlue+2", "kOrange+1", "kRed-6", "kAzure", "kCyan+1" ]
markers  = [ "24", "20", "21", "23", "22", "25", "24", "20", "21" ]

string=""
for monfile,color,marker,i in zip( monfiles, colors, markers, range(0, n_iters) ):
    string+= "SetNextInputFile('"+monfile+"', 'Iter"+str(i)+"', " + marker + ", " + color + ", 'AlignTracks_all', 'run_"+ str(runnum) +"')\\n"


os.system( "cat userConfig_meta.py | sed -e \"s@FILELIST@" + string + "@g\" > userConfig.py" )
os.system( "python MakeMajorAlignMonPlots.py --ALL 2>/dev/null" )
os.system( "mv ../plots {0}/plots_{1}_{2}".format(dirname, prename, suffix) )

