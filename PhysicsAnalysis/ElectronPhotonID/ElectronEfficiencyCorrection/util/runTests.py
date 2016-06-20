# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import glob
import sys

def writeOut(runnoList):
    f.write( '\\section{'+file.replace('_','\_')+'}\n')
    allcount = 0
    for type in types:
        f.write( '\\subsection{'+type+'}\n')
        for runno in runnoList:
            f.write( '\\subsubsection{run number = '+runno+'}\n')
            f.write( '\\begin{tabularx}{\linewidth}{'+tabstring+'}\n' )
            for i in range(len(pts)):
                f.write( ' & '+pts[i])
            f.write( '\\\\\hline\n')

            #f.write( ' & '+f.write( pts[i] for i in len(pts))
            for eta in etas:
                f.write( '$\\eta$ = '+eta+'')
                for pt in pts:
                    allcount = allcount+1
                    print str(allcount)+'/'+str(len(pts)*len(etas)*len(types)*len(runnoList)) 
                    result = os.popen('testEgEfficiencyCorrWithoutFile -f '+file+' -t '+type+' -r '+runno+' -e '+eta+' -p '+pt+' 2>&1 |grep pm').read().strip()
	 	    print ('testEgEfficiencyCorrWithoutFile -f '+file+' -t '+type+' -r '+runno+' -e '+eta+' -p '+pt+' 2>&1 |grep pm')
		    #print ('bin/sf_testbed -f '+file+' -t '+type+' -r '+runno+' -e '+eta+' -p '+pt+' 2>&1 |grep pm')	
                    print result
                    f.write(  '&'+result )
                f.write( '\\\\\hline\n')
            f.write( '\\end{tabularx}\n' )

outfile = 'sf_test.tex'
os.system('rm -f '+outfile)
f = open(outfile, 'w')

#RCDir =  os.environ["PWD"]
#print RCDir+'/../data/*.root'
#fileList = glob.glob(RCDir+'/../data/*')

if os.path.isfile('filelist.txt'): 
    file = open('filelist.txt', 'r')
    fileList = file.readlines()

else:
    RCDir =  os.environ["PWD"]
    print RCDir+'/triggtest/*.root'
    fileList = glob.glob(RCDir+'/triggtest/*')

fileList.sort()
print fileList
runnos = [ '300805' ]
#runnos2015 = [ '300805' ]

#runnos.append('12345')
runnosTrig2011 = ['177987','185354', '186874','188903']
runnosTrig2012 = ['200805']
runnos2015 = ['267904','300805']

types = ['AtlFast2','FullSim']
#types = ['FullSim']
etas = ['0','0.1','1.37','1.52' ,'2.5']
pts = [ '8000','10000','13000','18000','250001']
#7,9,12,17,24
#ipts = ['20001', '30001','40001','50001', '50000000']
#types = ['FullSim']
#etas = ['0.1']
#pts = ['25001']
tabstring = "X|"
for pt in pts:
    tabstring=tabstring+'X|'
tabstring = tabstring[:-1]

f.write( '\documentclass[8pt,oneside,a4paper,BCOR0mm,DIV45,landscape,pdftex]{scrartcl}\n')
f.write( '\usepackage{hyperref}\n')
f.write( '\usepackage{tabularx}\n')

f.write( '\\begin{document}\n')
f.write( '\\tableofcontents\clearpage\n')
counter = 0
for file in fileList:
    counter=counter+1
    print file+' ('+str(counter)+'/'+str(len(fileList))+')'
#    if 'offline' in file:
#        runnoList = runnos
    if '2011' in file:
        runnoList = runnosTrig2011
    elif '2012' in file:
        runnoList = runnosTrig2012
    elif '2015' in file:
        runnoList = runnos2015
    elif '2016' in file:
        runnoList = runnos2015
    if len(sys.argv) > 1:
        for arg in sys.argv:
            if arg in file:
                writeOut(runnoList)
    else:
        writeOut(runnoList)

f.write( '\end{document}\n')
f.close()
os.system('perl -pi -e \'s/Info in <testEgEfficiencyCorrWithoutFile>: //g;\' sf_test.tex')

