#!/usr/bin/env python

import sys          
import argparse
 
parser = argparse.ArgumentParser(description='Shows job performance from AMI db.')
parser.add_argument('-t','--reprotag', type=str, help='repro tag (f356,...)')
parser.add_argument('-r','--runnumber', type=str, help='run number examples 13  13,14  13-18,15 )')
parser.add_argument('-s','--stream', type=str, help='stream')
parser.add_argument('PROCESSINGSTEP', type=str, help='processing step (RAWtoESD,ESDtoAOD,...)')
parser.add_argument('-p','--pickle', type=str, help='pickle file name')

args = vars(parser.parse_args())

cond = " WHERE jobPerformance.jobprocessingstep='" + args['PROCESSINGSTEP'] + "'"
if args['reprotag']!=None:  cond += " and jobPerformance.reprotag='" + args['reprotag'] +"'"
if args['stream']!=None:    cond += " and jobPerformance.stream='"   + args['stream'] +"'"


if args['runnumber']!=None:
    cond+= ' AND ('
    op=args['runnumber']
    f=1
    ranges=op.split(',')
    for r in ranges:
        rns=r.split('-')
        if f==0:
            cond += " OR"
        if len(rns)==1:
            cond += " runnumber=" + rns[0]
        if len(rns)==2:
            cond += " ( runnumber BETWEEN "+ rns[0]+" AND "+ rns[1]+")"
        f=0
    cond +=' )'        

#Get pyAMI client
try:
    from pyAMI.pyAMI import AMI
except ImportError:
    print "WARNING unable to import AMI from pyAMI with standard $PYTHONPATH."
    sys.exit(0)
except Exception, e:
    print 'WARNING: other problem ', e
    print sys.exc_info()[0]
    print sys.exc_info()[1]
    sys.exit(0)
    
amiclient=AMI()

DAT=[]

print 'Geting object sizes from AMI'
print '---------------------------------------------------------------------'
mySql='sql=SELECT jobprocessingstep, reprotag, runnumber, events,  stream, CPU_BMIPS, EVTLOOP_CPU, OVERHEAD_CPU, LEAK_11_50_MALLOC, EVTLOOP_WALL, OVERHEAD_WALL, VMEM_MEAN, JOBCFG_WALL, LEAK_51_MALLOC, VMEM_PEAK, RSS_MEAN, LEAK_51_VMEM FROM jobPerformance'
mySql+=cond
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
result=amiclient.execute(amiCommand)
cat=result.getDict()

print 'Ami command: ',mySql
print '---------------------------------------------------------------------'
print 'reprotag      run         events       processing step     stream    '
print '---------------------------------------------------------------------'

for k in cat.keys():
    v=cat[k]
    for k1 in v.keys():
        rowv=v[k1]
        print rowv['reprotag'],'     ', rowv['runnumber'],'     ', rowv['events'].rjust(8),'     ', rowv['jobprocessingstep'],'           ', rowv['stream']
        print 'CPU_BMIPS: %12.3f'% round(float(rowv['CPU_BMIPS']),3), '\tJOBCFG_WALL: %12.3f'% round(float(rowv['JOBCFG_WALL']),3)
        print 'EVTLOOP_CPU:  %12.3f'% round(float(rowv['EVTLOOP_CPU']),3),
        print '\tOVERHEAD_CPU:  %12.3f'% round(float(rowv['OVERHEAD_CPU']),3)
        print 'EVTLOOP_WALL: %12.3f'% round(float(rowv['EVTLOOP_WALL']),3),
        print '\tOVERHEAD_WALL: %12.3f'% round(float(rowv['OVERHEAD_WALL']),3)
        print 'VMEM_MEAN: %12.3f'% round(float(rowv['VMEM_MEAN']),3),
        print '\tVMEM_PEAK: %12.3f'% round(float(rowv['VMEM_PEAK']),3),
        print '\tRSS_MEAN:%12.3f'% round(float(rowv['RSS_MEAN']),3)
        print 'LEAK_11_50_MALLOC: %12.3f'% round(float(rowv['LEAK_11_50_MALLOC']),3),
        print '\tLEAK_51_MALLOC: %12.3f'% round(float(rowv['LEAK_51_MALLOC']),3),
        print '\tLEAK_51_VMEM: %12.3f'% round(float(rowv['LEAK_51_VMEM']),3)
        print '---------------------------------------------------------------------'
        L=[rowv['reprotag'], int(rowv['runnumber']), int(rowv['events']), rowv['jobprocessingstep'], rowv['stream']]
        L.append( round(float(rowv['CPU_BMIPS']),3) )
        L.append( round(float(rowv['EVTLOOP_CPU']),3) )
        L.append( round(float(rowv['OVERHEAD_CPU']),3) )
        L.append( round(float(rowv['LEAK_11_50_MALLOC']),3) )
        L.append( round(float(rowv['EVTLOOP_WALL']),3) )
        L.append( round(float(rowv['OVERHEAD_WALL']),3) )
        L.append( round(float(rowv['VMEM_MEAN']),3) )
        L.append( round(float(rowv['JOBCFG_WALL']),3) )
        L.append( round(float(rowv['LEAK_51_MALLOC']),3) )
        L.append( round(float(rowv['VMEM_PEAK']),3) )
        L.append( round(float(rowv['RSS_MEAN']),3) )
        L.append( round(float(rowv['LEAK_51_VMEM']),3) )
        DAT.append( L )

if args['pickle']!=None:
    try:
        import cPickle as pickle
    except:
        import pickle
    import gzip
    fil = gzip.GzipFile(args['pickle'], 'wb')    
    pickle.dump(DAT, fil, -1)


print 'DONE'



