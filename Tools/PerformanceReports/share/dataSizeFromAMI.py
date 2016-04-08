#!/usr/bin/env python

import sys          
import argparse
 
parser = argparse.ArgumentParser(description='Shows objects sizes from AMI db.')
parser.add_argument('-t','--reprotag', type=str, help='repro tag')
parser.add_argument('-r','--runnumber', type=str, help='run number examples 13  13,14  13-18,15 )')
parser.add_argument('-s','--stream', type=str, help='stream')
parser.add_argument('-o','--object', type=str, help='object name')
parser.add_argument('-c','--category', type=str, help='category')
parser.add_argument('-p','--pickle', type=str, help='pickle file name')
parser.add_argument('-f','--fraction', action='store_const', const='1', help='shows fraction of the total number of events')
parser.add_argument('FORMAT', type=str, help='data format: AOD or ESD')

args = vars(parser.parse_args())

cond = " and objectSize.datatype='" + args['FORMAT'] + "'"
if args['reprotag']!=None:  cond += " and objectSize.reprotag='" + args['reprotag'] +"'"
if args['stream']!=None:    cond += " and objectSize.stream='"   + args['stream'] +"'"
if args['object']!=None:    cond += " and objectSize.objectName='"   + args['object'] +"'"
if args['category']!=None:    cond += " and object.category='"   + args['category'] +"'"

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
    print 'WARNING: ', e
    print sys.exc_info()[0]
    print sys.exc_info()[1]
    sys.exit(0)

amiclient=AMI()

DAT=[]

print 'Geting object sizes from AMI\n------------------------------------------------------------------------------------'
mySql='sql=SELECT object.objectName, object.category, objectSize.events, objectSize.runnumber, objectSize.objectsize, objectSize.datatype, objectSize.stream, objectSize.reprotag FROM objectSize,object where objectSize.objectFK=object.objectID'
mySql+=cond
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
print 'Ami Command: ', mySql, '\n---------------------------------------------------'

result=amiclient.execute(amiCommand)
cat=result.getDict()

print 'reprotag     run       events     type     size[kb]        stream      category         collection name'
print '------------------------------------------------------------------------------------------------------------------------------------------------'
for k in cat.keys():
    v=cat[k]
    for k1 in v.keys():
        rowv=v[k1]
        print rowv['reprotag'],'    ', rowv['runnumber'],'   ', rowv['events'].rjust(8),'    ', rowv['datatype'],'     %9.3f'% round(float(rowv['objectsize']),6) ,'    ', rowv['stream'],'    ', rowv['category'].ljust(15), '    ',  rowv['objectName']
        DAT.append([rowv['reprotag'],int(rowv['runnumber']),int(rowv['events']),rowv['datatype'],round(float(rowv['objectsize']),6), rowv['stream'], rowv['category'], rowv['objectName']])
print '---------------------------------------------------------'



if args['fraction']!=None:
    #load event numbers
    r=set()
    s=set()
    t=set()
    for row in DAT:
        t.add(row[0])
        r.add(row[1])
        s.add(row[5])
    l=[]
    for run in r:
        l.append(run)
    l.sort()
    for run in l:
        for stream in s:
            for tag in t:
                amiCommand=["SearchQuery", "-processingStep=real_data", "-project=data11_001"]
                #amiCommand+=["-sql=SELECT dataset.totalEvents, dataset.identifier, dataset.logicalDatasetName,  dataset.version  FROM dataset WHERE dataset.logicalDatasetName LIKE 'data11_7TeV.%'  AND dataset.dataType='"+args['FORMAT']+"' AND  dataset.streamName='"+stream+"' AND dataset.runNumber='"+str(run)+"'"]
                amiCommand+=["-sql=SELECT dataset.totalEvents FROM dataset WHERE dataset.dataType='"+args['FORMAT']+"' AND  dataset.streamName='physics_"+stream+"' AND dataset.runNumber='"+str(run)+"' AND dataset.version='"+tag+"'"]
                result=amiclient.execute(amiCommand) 
                res=result.getDict()
                am='---'
                if 'Element_Info' in res.keys():
                    am=res['Element_Info']['row_1']['totalEvents']
                for d in DAT:
                    if d[1]==run and d[5]==stream and d[0]==tag:
                        print 'run:',run, '  stream',stream, '  tag',tag, '  ami:', (am).rjust(8),"    from my db: ",str(d[2]).rjust(8) ,"  fraction seen: ",
                        if am!='---':
                            print d[2]/float(am)
                        else:
                            print '---'



if args['pickle']!=None:
    try:
        import cPickle as pickle
    except:
        import pickle
    import gzip
    fil = gzip.GzipFile(args['pickle'], 'wb')    
    pickle.dump(DAT, fil, -1)
    
print 'DONE'
