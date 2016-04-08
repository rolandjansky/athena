#!/usr/bin/env python

import sys          
import argparse

from PerformanceReports.ObjectCategories import obj_category

 
parser = argparse.ArgumentParser(description='Updates categorization of algorithms and stored collections in AMI db. To use you also need atlas grid certificate.')

parser.add_argument('login', type=str, help='AMI username')
parser.add_argument('password', type=str, help='AMI password')
parser.add_argument('--debug',action='store_true', help='print without uploading')
args = vars(parser.parse_args())


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

import PmbUtils.AlgDomains

if not args["debug"]:
    print "printout and upload!"    
else:
    print "--debug : only printout, no upload!"


print '************ Updating AOD/ESD Collections Classification *****************'
DAT={}
print 'Geting distinct collections from AMI\n------------------------------------------------------------------------------------'
mySql='sql=SELECT distinct object.objectname, object.category, object.datatype FROM object'
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
# print 'Ami Command: ', mySql, '\n---------------------------------------------------'

result=amiclient.execute(amiCommand)
res=result.getDict()

oldAOD={} # key - algoname value-category
oldESD={}
oldDPD={}
for k in res.keys():
    v=res[k]
    for k1 in v.keys():
        rowv=v[k1]
        if rowv['datatype']=='AOD':
            oldAOD[rowv['objectname']]=rowv['category']
        if rowv['datatype']=='ESD':
            oldESD[rowv['objectname']]=rowv['category']                
        if rowv['datatype']=='DPD':
            oldDPD[rowv['objectname']]=rowv['category']
        
print 'total collections in: AOD -', len(oldAOD),'\tESD -', len(oldESD),'\tDPD -', len(oldDPD)

allCategories=[]
print 'Geting  categories from AMI\n------------------------------------------------------------------------------------'
mySql='sql=SELECT categoryname FROM categoryref '
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
result=amiclient.execute(amiCommand)
res=result.getDict()['Element_Info']

for v in res.values():
    allCategories.append( v['categoryname'] );

print 'all defined categories:', allCategories
        



nobj=nobjchanged=0
for obj,cat in oldAOD.iteritems():
    nobj+=1
    newCat=obj_category(obj,'AOD')
    if newCat not in  allCategories:
        print 'AOD NEW CATEGORY APPEARED:'+newCat+'. EXITING'
        exit(0)
    if newCat!=oldAOD[obj]:
        nobjchanged+=1
        print 'obj',obj, 'old cat:', oldAOD[obj], 'new category: ', newCat
        if not args["debug"]:
            amiCommand=['UpdateElement', 'AMIUser='+args['login'], 'AMIPass='+args['password'], 'updateField=category','updateValue='+ newCat, "objectName="+obj, "dataType=AOD", 'entity=object', 'processingStep=utility','project=coll_sizes_tier0']
            result=amiclient.execute(amiCommand)
print nobjchanged, " AOD collections changed categories out of ",nobj

nobj=nobjchanged=0
for obj,cat in oldESD.iteritems():
    nobj+=1
    newCat=obj_category(obj,'ESD')
    if newCat not in  allCategories:
        print 'ESD NEW CATEGORY APPEARED:'+newCat+'. EXITING'
        exit(0)
    if newCat!=oldESD[obj]:
        nobjchanged+=1
        print 'obj',obj, 'old cat:', oldESD[obj], 'new category: ', newCat
        if not args["debug"]:
            amiCommand=['UpdateElement', 'AMIUser='+args['login'], 'AMIPass='+args['password'], 'updateField=category','updateValue='+ newCat, "objectName="+obj, "dataType=ESD", 'entity=object', 'processingStep=utility','project=coll_sizes_tier0']
            result=amiclient.execute(amiCommand)
print nobjchanged, " ESD collections changed categories out of ",nobj

nobj=nobjchanged=0
for obj,cat in oldDPD.iteritems():
    nobj+=1
    newCat=obj_category(obj,'DPD')
    if newCat not in  allCategories:
        print 'DPD NEW CATEGORY APPEARED:'+newCat+'. EXITING'
        exit(0)
    if newCat!=oldDPD[obj]:
        nobjchanged+=1
        print 'obj',obj, 'old cat:', oldDPD[obj], 'new category: ', newCat
        if not args["debug"]:
            amiCommand=['UpdateElement', 'AMIUser='+args['login'], 'AMIPass='+args['password'], 'updateField=category','updateValue='+ newCat, "objectName="+obj, "dataType=DPD", 'entity=object', 'processingStep=utility','project=coll_sizes_tier0']
            result=amiclient.execute(amiCommand)
print nobjchanged, " DPD collections changed categories out of ",nobj



print '****************** Updating Algorithm Classification *********************'

DAT={}
print 'Geting distinct algos from AMI\n------------------------------------------------------------------------------------'
mySql='sql=SELECT distinct algoref.algoname,algoref.algocategoryname FROM algoref'
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
# print 'Ami Command: ', mySql, '\n---------------------------------------------------'

result=amiclient.execute(amiCommand)
res=result.getDict()

old={} # key - algoname value-category
for k in res.keys():
    v=res[k]
    for k1 in v.keys():
        rowv=v[k1]
        old[rowv['algoname']]=rowv['algocategoryname']
        

allCategories=[]
print 'Geting  categories from AMI\n------------------------------------------------------------------------------------'
mySql='sql=SELECT algocategoriesname FROM algocategoryref '
amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
result=amiclient.execute(amiCommand)
res=result.getDict()['Element_Info']

for v in res.values():
    allCategories.append( v['algocategoriesname'] );

print 'all defined categories:', allCategories



nalg=0
nalgchanged=0
for alg,cat in old.iteritems():
    nalg+=1
    newCat=PmbUtils.AlgDomains.filter_alg_to_public_plot_domain(alg)
    if newCat not in  allCategories:
        print 'NEW CATEGORY APPEARED:'+newCat+'. EXITING'
        exit(0)
    
    if newCat!=old[alg]:
        nalgchanged+=1
        print 'algo',alg, 'old cat:', old[alg], 'new category: ', newCat
        if not args["debug"]:
            amiCommand=['UpdateElement', 'AMIUser='+args['login'], 'AMIPass='+args['password'], 'updateField=algoCategoryName','updateValue='+ newCat, "algoName="+alg, 'entity=algoRef', 'processingStep=utility','project=coll_sizes_tier0']
            result=amiclient.execute(amiCommand)

print nalgchanged, " algorithm changed categories out of ",nalg
