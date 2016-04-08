#!/usr/bin/env python

import sys          
import argparse
 
parser = argparse.ArgumentParser(description='Shows algorithm timings from AMI db.')
parser.add_argument('PROCESSINGSTEP', type=str, help='processing step (RAWtoESD,ESDtoAOD,...)')
parser.add_argument('-t','--reprotag', type=str, help='repro tag')
parser.add_argument('-r','--runnumber', type=str, help='run number examples 13  13,14  13-18,15 )')
parser.add_argument('-s','--stream', type=str, help='stream')
parser.add_argument('-a','--algo', type=str, help='algo name. accepts percentage sign as wildcard.')
parser.add_argument('-p','--pickle', type=str, help='pickle file name')

args = vars(parser.parse_args())

cond1=''
cond = " WHERE algo_proc_step='" + args['PROCESSINGSTEP'] + "'"
if args['reprotag']!=None:  cond += " and reprotag='" + args['reprotag'] +"'"
if args['stream']!=None:    cond += " and stream='"   + args['stream'] +"'"
if args['algo']!=None:   cond1 += " WHERE algoName like '"   + args['algo'] +"'"

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

print 'Geting algoRef data from AMI...'
mySql='sql=SELECT algoName, algoCategoryName, algoRefID FROM algoRef'
mySql+=cond1
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
result=amiclient.execute(amiCommand)
refnames={}
refcats={}
cat1=result.getDict()
for key1 in cat1.keys():
    v1=cat1[key1]
    for key2 in v1.keys():
        row=v1[key2]
        refnames[row['algoRefID']] = row['algoName']
        refcats[row['algoRefID']]  = row['algoCategoryFK']


print 'Geting algo data from AMI...\n------------------------------------------------------------------------------'
mySql='sql=SELECT events, runnumber, stream, reprotag, cputime, algorefFK FROM algoPerformance'
mySql+=cond
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
print 'Ami Command: ', mySql, '\n---------------------------------------------------'

result=amiclient.execute(amiCommand)
cat=result.getDict()

print 'reprotag     run        events      stream         CPUtime[ms]          algorithm name'
print '----------------------------------------------------------------------------------------------------------------------------------'

for k in cat.keys():
    v=cat[k]
    for k1 in v.keys():
        rowv=v[k1]
        if rowv['algorefFK'] in refnames:
            print rowv['reprotag'],'    ', rowv['runnumber'],'    ', rowv['events'].rjust(8),'    ', rowv['stream'],'    %15.3f'%round(float(rowv['cputime']),6), '    ', refnames[rowv['algorefFK']]
            DAT.append([rowv['reprotag'], int(rowv['runnumber']), int(rowv['events']), rowv['stream'], round(float(rowv['cputime']),3), refnames[rowv['algorefFK']] ])
            
print '----------------------------------------------------------------------------------------------------------------------------------'

if args['pickle']!=None:
    try:
        import cPickle as pickle
    except:
        import pickle
    import gzip
    fil = gzip.GzipFile(args['pickle'], 'wb')    
    pickle.dump(DAT, fil, -1)


print 'DONE'



