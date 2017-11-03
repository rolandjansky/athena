#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#******************************************
#This script checks the reduction of derivations. It takes in input the log.AODtoDAOD file produced when running the command Reco_tf.py

__author__  = 'Francesco Guescini <Francesco.Guescini@cern.ch>'
__version__ = '0.2'

#******************************************
#import stuff
import argparse, os, PyUtils.PoolFile, re

#******************************************
def checkAODtoDAOD(args):

    print '\ncheck AOD to DAOD derivation'
	
    #------------------------------------------
    #read input file
    print
    lines = []
    if os.path.isfile(args.filename):
        print 'log: %s'%args.filename
        with open(args.filename, 'rb') as f:
            lines = f.readlines()
            f.close()
        outputdir = os.path.dirname(os.path.realpath(args.filename))
    else:
        raise SystemExit('\ncould not find log file: %s'%args.filename)
    outputdir+='/'
    print 'output directory: %s'%outputdir

    #------------------------------------------
    #get AOD info
    print
    for i, line in enumerate(lines):
        if 'Successfully filled inputFileSummary from file ' in line:
            aod = line.split('from file ')[1].rstrip()
            break

    for i, line in enumerate(lines):
        if 'RootCollection Info Root collection opened, size =' in line:
            aodevents = int(line.split('size = ')[1].rstrip())
            break

    aodsize = os.path.getsize(aod.replace("root://eosatlas/",""))
    
    print 'AOD: %s\t%.1f MB\t%s'%(aod, (aodsize/1024.), aodevents)
    
    #------------------------------------------
    #get DAODs info
    print

    #events processed
    for i, line in enumerate(lines):
        if 'INFO Events processed' in line:
            eventsprocessed = int(line.split('processed: ')[1].rstrip())
    if eventsprocessed < 0. or eventsprocessed > aodevents:
        eventsprocessed = aodevents
    print 'events processed: %s\n'%eventsprocessed
            
    #available derivations
    derivation = []
    for i, line in enumerate(lines):
        if 'Will attempt to make the following reduced formats' in line:
            derivation = line.split('[\'')[1].rstrip().rstrip('\']').split('\', \'')
            
    #DAOD files
    daod = []
    for d in derivation:
        for i, line in enumerate(lines):
            if 'Output file:' in line and 'DAOD_'+d+'.' in lines[i+1]:
                daod.append( lines[i+1].split(' ')[1].rstrip() )

    #size and events
    daodsize = []
    daodevents = []
    daodeventssize = []
    for d in daod:
        poolFile = PyUtils.PoolFile.PoolFile(outputdir+d)
        daodsize.append(os.path.getsize(outputdir+d))
        daodevents.append(poolFile.dataHeader.nEntries)

        totEventDiskSize = 0.
        for d in poolFile.data:
            #print "\t%s\t%-50s\t%10.3f kb"%(d.dirType,d.name,d.diskSize)
            totEventDiskSize+=d.diskSize
        #print "\ntot. events disk size = %.3f kb"%totEventDiskSize
        #print "event disk size = %.3f kb"%(totEventDiskSize/poolFile.dataHeader.nEntries)
        daodeventssize.append(totEventDiskSize)
        
    print '\nDAODs:'
    for i,d in enumerate(derivation):
        print '  %s  \t%s\t%.1f MB\t%s\t%.3f kb'%(d, daod[i], daodsize[i]/1024./1024., daodevents[i], daodeventssize[i]/daodevents[i])
        
    #------------------------------------------
    #results table
    if args.table:
        print ''
        print '\\begin{table}'
        print '  \\begin{tabular}{lrrr}'
        print '    \hline'
        print '    derivation&events reduction&size reduction&event size [kb]\\\\'
        print '    \hline'
        for i,d in enumerate(derivation):
            print '    %s & %.1f\\%% & %.1f\\%% & %.1f\\\\'%(d, (100.*daodevents[i]/eventsprocessed), (100.*(daodsize[i]*aodevents)/(aodsize*eventsprocessed)), daodeventssize[i]/daodevents[i])
        print '    \hline'
        print '  \end{tabular}'
        print '\end{table}'

#******************************************
if __name__ == '__main__':

    #------------------------------------------
    #parse input arguments
    parser = argparse.ArgumentParser(description='%prog [options]')
    parser.add_argument('-f', '--file', dest='filename', default='', required=False, help='AOD to DAOD log file')
    parser.add_argument('-t', '--table', dest='table', action='store_true', default=False, help='ouput table')
    args = parser.parse_args()
    
    #------------------------------------------
    #check hybrid jig calibration
    checkAODtoDAOD(args)
    print
