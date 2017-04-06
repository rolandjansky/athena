#!/usr/bin/python

import sys,os,subprocess

dirname="/afs/cern.ch/work/k/krosbach/alignment/2015_5TeV_20.7_reprocessing/align01/output2"
project_name="template"
proc = subprocess.Popen("ls " + dirname + "/" + project_name+"*/TotalMonitoring.root | wc -l", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
runnum = 286367
n_iters = int( proc.stdout.readline() )

os.system( "rm -rf ../plots_" + project_name )

monfiles = [ dirname + "/" + project_name + "_Iter" + str(i) + "_suffix/TotalMonitoring.root" for i in range(0, n_iters) ]
logfiles = [ dirname + "/" + project_name + "_Iter" + str(i) + "_suffix/alignlogfile.txt"     for i in range(0, n_iters) ]

colors   = [ "kGray+1", "kRed+2", "kBlue+2", "kOrange+1" ]
markers  = [ "24", "20", "21", "23" ]

string=""
for monfile,color,marker,i in zip( monfiles, colors, markers, range(0, n_iters) ):
    string+= "SetNextInputFile('"+monfile+"', 'Iter"+str(i)+"', " + marker + ", " + color + ", 'AlignTracks_all', 'run_"+ str(runnum) +"')\\n"


os.system( "cat userConfig_meta.py | sed -e \"s@FILELIST@" + string + "@g\" > userConfig.py" )
os.system( "python MakeMajorAlignMonPlots.py --ALL 2>/dev/null" )

string=""
for logfile in logfiles:
    string += logfile + " "
os.system( "./drawEvolution.py " + string )
os.system( "mv *.pdf ../plots" )
os.system( "mv -f ../plots ../plots_" + project_name )

