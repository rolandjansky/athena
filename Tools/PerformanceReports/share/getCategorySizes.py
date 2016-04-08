#!/usr/bin/env python

import sys          
import argparse
 
parser = argparse.ArgumentParser(description='Shows collection sizes of collections in the file categorized according to AMI db.')
parser.add_argument('INPUTFILE', type=str, help='an input AOD or ESD file. (Filename has to have AOD or ESD in the name)')
parser.add_argument('-c','--clasification', action='store_const', const='1', help='also prints current clasification of all the object in this data format')

args = vars(parser.parse_args())

fName = args['INPUTFILE'];
fType='ESD'
if fName.count('AOD')>0:
    fType='AOD'
else:
    if fName.count('ESD')>0:
        fType='ESD'
    else:
        print 'WARNING: could not determine file type from the filename. Assuming it is ESD.'
    
print 'checking file ...'
ne=0
collectionSize={}
try:
    import PyUtils.PoolFile as PF
    PF.PoolOpts.FAST_MODE = True
    poolFile = PF.PoolFile( fName , False)
    poolFile.checkFile( )
    ne=poolFile.dataHeader.nEntries
    if ne>0:
         for collection in poolFile.data:
             collectionSize[collection.name] = collection.diskSize / ne
            
except Exception, e:
    print "## Caught exception [%s] !!" % str(e.__class__)
    print "## What:",e
    print sys.exc_info()[0]
    print sys.exc_info()[1]
    sys.exit(1)

# print collectionSize

     
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

print 'Geting object categories from AMI ...\n------------------------------------------------------------------------------------'
mySql="sql=SELECT UNIQUE object.objectName, object.category FROM objectSize,object WHERE objectSize.datatype='"+fType+"'"

amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
#print 'Ami Command: ', mySql, '\n---------------------------------------------------'

result=amiclient.execute(amiCommand)
cat=result.getDict()

allcategories={};

if args['clasification']!=None:
    print ' current clasification '
    print '        category          collection name'
    print '------------------------------------------------------------------------------------------------------------------------------------------------'

v=cat['Element_Info']
for k1 in v.keys():
    rowv=v[k1]
    if args['clasification']!=None: print rowv['category'].ljust(15), '    ',  rowv['objectName']
    allcategories[rowv['category']]=0
    DAT.append( [rowv['category'], rowv['objectName']])

if args['clasification']!=None: print '---------------------------------------------------------'

print allcategories

for ob in collectionSize:
    print ob
    
for ob in collectionSize:
    found=0
    for da in DAT:
        if da[1]==ob:
            found=1;
            allcategories[da[0]]+=collectionSize[ob]
    if found==0:
        print 'WARNING: collection not found in the DB. ', ob
        
for c in allcategories:
    print c.rjust(20),"\t" , '  %9.3f'% round(float(allcategories[c]),6) 

            
    
print 'DONE'
