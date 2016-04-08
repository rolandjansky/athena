#!/usr/bin/env python

__version__ = "$Revision: 1.1.1.1 $"
__author__      = "Ilija Vukotic <ivukotic@cern.ch>"


import sys
import os
import argparse
from subprocess import *
from xml.dom.minidom import parse  

parser = argparse.ArgumentParser(description='Shows some of the performance information contained in AMI db.')

parser.add_argument('-fs','--fractionsSize', action='store_const', const='1', help='fraction of the events seen from OBJECTSIZE' )
parser.add_argument('-fj','--fractionsJobs', action='store_const',  const='1', help='fraction of the events seen from JOBS')
parser.add_argument('-o','--objectsCategorization' , action='store_const',  const='1', help='objects and their categorisation')
parser.add_argument('-a','--algosCategorization' , action='store_const',  const='1', help='algorithms and their categorisation')
parser.add_argument('-l','--luminosity' , action='store_const',  const='1', help='print luminosity')
parser.add_argument('-r','--runnumber', type=str, help='run number examples 13  13,14  13-18,15 )')
parser.add_argument('-u','--updateRunTable', type=str , nargs=2,  help='update RunTable with info: goodforphysics,luminosity,mu',metavar=('AMIusername', 'AMIpassword'))


args = vars(parser.parse_args())

print args
# exit(0)

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

cond=''
if args['runnumber']!=None:
    cond+= ' WHERE ('
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

if args['objectsCategorization']!=None:
    DAT={}
    a=set()
    mySql='sql=SELECT distinct object.objectName, object.category FROM object'
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    print 'Ami Command: ', mySql, '\n---------------------------------------------------'

    result=amiclient.execute(amiCommand)
    cat=result.getDict()

    print 'objects per category\n----------------------------------'
    c=cat['Element_Info']
    for i in c.keys():
        DAT[c[i]['objectName']]=c[i]['category']
        a.add(c[i]['category'])

    for cat in a:
        print "\n===============\n",cat,"\n----------------------------------------"
        for g in DAT.keys():
            if DAT[g]==cat:
                print g



if args['algosCategorization']!=None:    

    print '\n\n============================ ALGORITHMS ===============================\n'
    
    mySql='sql=SELECT distinct algoref.algocategoryname FROM algoref'
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    print 'Ami Command: ', mySql, '\n---------------------------------------------------'
    result=amiclient.execute(amiCommand)
    cat=result.getDict()['Element_Info']
    for i in cat.values():
        ca=i['algocategoryname']
        print '---------- category: ', ca ,'--------------'
    
        mySql1="sql=SELECT distinct algoref.algoname FROM algoref WHERE algoref.algocategoryname='"+ca+"'"
        amiCommand=['SearchQuery', mySql1, 'processingStep=utility','project=coll_sizes_tier0']
    
        result=amiclient.execute(amiCommand).getDict()['Element_Info']
        for r in result.values():
            print r['algoname']



if  args['fractionsSize']!=None:
        
    mySql='sql=SELECT distinct runnumber, stream FROM objectsize'
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    print 'Ami Command: ', mySql, '\n---------------------------------------------------'
    
    result=amiclient.execute(amiCommand)
    cat=result.getDict()
    
    c=cat['Element_Info']
    runs=[]
    All={}
    for i in c.keys():
        run=int(c[i]['runnumber'])
        st=c[i]['stream']
        # print run,st
        if run not in All.keys():
            runs.append(run)
            All[run]={}
        All[run][st]=0
        
    runs.sort()
    
    mySql='sql=SELECT events, runnumber, stream FROM objectSize where objectfk=4784 '
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    result=amiclient.execute(amiCommand)
    cat=result.getDict()
    c=cat['Element_Info']
    
    for i in c.keys():
        run=int(c[i]['runnumber'])
        st=c[i]['stream']
        ev=int(c[i]['events'])
        All[run][st]=ev
    
    
    
    #load event numbers

    for run in runs:
        print run, "\n------------------------------"
        for stream in All[run].keys():
            amiCommand=["SearchQuery", "-processingStep=real_data", "-project=data11_001"]
            amiCommand+=["-sql=SELECT dataset.totalEvents FROM dataset WHERE dataset.dataType='ESD' AND  dataset.streamName='physics_"+stream+"' AND dataset.version like 'f%' AND dataset.runNumber='"+str(run)+"'"]
            result=amiclient.execute(amiCommand) 
            res=result.getDict()
            
            print "\t\t", stream.ljust(15),'\t', str(All[run][stream]).rjust(10),
            if 'Element_Info' in res.keys():
                am=res['Element_Info']['row_1']['totalEvents']
                fr=str(round(All[run][stream]/float(res['Element_Info']['row_1']['totalEvents'])*100))
                
                print '  ami:', am.rjust(8) ,"  fraction seen: ",fr.rjust(4)
                
                if 'row_2' in res['Element_Info'].keys():
                    print '* not a unique set in AMI. all the values are:', 
                    for r in res['Element_Info'].values(): 
                        print int(r['totalEvents']),
                    print
            else:
                print '  ami:', '---'.rjust(8) ,"  fraction seen: ",'---'.rjust(4)
                
                

if  args['fractionsJobs']!=None:

    mySql='sql=SELECT distinct runnumber, stream, jobprocessingstep FROM jobperformance'+cond
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    print 'Ami Command: ', mySql, '\n---------------------------------------------------'

    result=amiclient.execute(amiCommand)
    cat=result.getDict()

    c=cat['Element_Info']
    runs=[]
    All={}
    for i in c.keys():
        run=int(c[i]['runnumber'])
        st=c[i]['stream']
        ps=c[i]['jobprocessingstep']
        # print run,st,ps
        if run not in All.keys():
            runs.append(run)
            All[run]={}
        All[run][st]={}
        All[run][st][ps]=0

    runs.sort()

    mySql='sql=SELECT events, runnumber, stream, jobprocessingstep FROM jobperformance'+cond
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    result=amiclient.execute(amiCommand)
    cat=result.getDict()
    c=cat['Element_Info']

    for i in c.keys():
        run=int(c[i]['runnumber'])
        st=c[i]['stream']
        ps=c[i]['jobprocessingstep']
        ev=int(c[i]['events'])
        All[run][st][ps]=ev

    #load event numbers

    for run in runs:
        print run, "\n---------------"
        for stream in All[run].keys():
            for step in All[run][stream].keys():
                amiCommand=["SearchQuery", "-processingStep=real_data", "-project=data11_001"]
                if step=='RAWtoESD':
                    amiCommand+=["-sql=SELECT dataset.totalEvents FROM dataset WHERE dataset.dataType='ESD' AND  dataset.streamName='physics_"+stream+"' AND dataset.version like 'f%' AND dataset.runNumber='"+str(run)+"'"]
                elif step=='ESDtoAOD' or step=='ESDtoDPD':
                    amiCommand+=["-sql=SELECT dataset.totalEvents FROM dataset WHERE dataset.dataType='AOD' AND  dataset.streamName='physics_"+stream+"' AND dataset.version like 'f%' AND dataset.runNumber='"+str(run)+"'"]
                result=amiclient.execute(amiCommand) 
                res=result.getDict()

                print "\tstream: ", stream.ljust(15),'\t', step, '\t events:', str(All[run][stream][step]).rjust(10),'\t',
                if 'Element_Info' in res.keys():
                    am=res['Element_Info']['row_1']['totalEvents']
                    fr=str(round(All[run][stream][step]/float(res['Element_Info']['row_1']['totalEvents'])*100))

                    print '  ami:', am.rjust(8) ,"  fraction seen: ",fr.rjust(4)

                    if 'row_2' in res['Element_Info'].keys():
                        print '* not a unique set in AMI. all the values are:', 
                        for r in res['Element_Info'].values(): 
                            print int(r['totalEvents']),
                        print
                else:
                    print '  ami:', '---'.rjust(8) ,"  fraction seen: ",'---'.rjust(4)      
                

if  args['luminosity']!=None or args['updateRunTable']!=None :

    mySql='sql=SELECT distinct runnumber FROM jobperformance'+cond+' order by runnumber '
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    print 'Ami Command: ', mySql, '\n---------------------------------------------------'

    result=amiclient.execute(amiCommand)
    cat=result.getDict()
    c=cat['Element_Info']
    
    
    mySql='sql=SELECT run FROM runinfo order by run'
    amiCommand=['SearchQuery', mySql, 'processingStep=utility','project=coll_sizes_tier0']
    print 'Ami Command: ', mySql, '\n---------------------------------------------------'

    result=amiclient.execute(amiCommand)
    cat=result.getDict()
    cduplicate=cat['Element_Info']
    
    runs=[]
    for i in c.keys():
        run=int(c[i]['runnumber'])
        skip=0
        for j in cduplicate.keys():
            if run==int(cduplicate[j]['run']): skip=1
        # print run
        if skip==0: runs.append(run)
        else: print 'Data for run',run,'already exist in the db. Skipping.'

    if len(runs)==0: 
        print 'no new runs found. exiting.'
        exit(0)
         
    runs.sort()
    
    sruns=set()
    runrow={}
    for i in runs:
        sruns.add(i)
        runrow[i]=[str(i),'0.0','0.0','0.0','0','0','0']
        
    searchString='find run '+str(min(sruns))+'-'+str(max(sruns))+' and partition ATLAS and ptag "data12*" / show run and events and readyforphysics'
    print searchString
    
        
    
    p1 = Popen(["AtlRunQuery.py", searchString], stdout=PIPE)
    p2 = Popen(["awk", ' BEGIN{n=0;star=0;sto=0;}{ if ($1=="run") star=1; if (star==0 || sto==1) next; if($1=="Summary:") sto=1; if ($1=="run"){ print n;n=0; print $2 }else{if ($NF=="1") n=1;}}END{print n} '], stdin=p1.stdout, stdout=PIPE)#if ($1=="run")
    output = p2.communicate()[0]
    # print "---\n",output,'\n---'
    ret = [] # combines output in lines
    cl=''
    for l in output:
        if ord(l)==10:
            ret.append(int(cl))
            cl=''
        else:
            cl+=l
    ret.pop(0)
    # print ret 
    goodones=[]
    for i in range(0, len(ret), 2):
        if ret[i+1]==0: continue
        if ret[i] in sruns:
            goodones.append(ret[i])
            runrow[ret[i]][5]='1'

    
      
    dir='/afs/cern.ch/atlas/www/GROUPS/DATAPREPARATION/DataSummary/2012/rundata/'
    print 'Run,Luminosity,maxEv/bunch,mu'
    for r in goodones:
        rn='run'+str(r)
        fn   = dir+rn+'.xml'
        if os.access(fn,os.F_OK):
            doc  = parse(fn)
            d    = doc.getElementsByTagName("delivered")[7]
            n    = d.childNodes
            lumi = float(n[0].data)
            d    = doc.getElementsByTagName("livetime")[7]
            n    = d.childNodes
            live = float(n[0].data)
            d    = doc.getElementsByTagName("maxavgevtsperbx")[7]
            n    = d.childNodes
            maxev = n[0].data
            epsfn=dir+rn+'/'+rn+'_bcidmu.eps'
            out=os.popen('grep Mean: '+epsfn).readlines()
            l=out[0]
            l=l[l.index('Mean:')+6:]
            mu=l[:l.index(')')]
            if (live): tl=lumi/live; 
            else: tl=0;
            print rn, '\tlumi: ',lumi,'\tlive:',live, "\tinst. lumi:", tl,'\tmaxev:', maxev,'\tmu:',mu
            runrow[r][0]=str(r)
            runrow[r][1]=str(tl)
            runrow[r][2]=str(live)
            runrow[r][3]=mu
            runrow[r][4]='1'
            if (lumi==0): runrow[r][4]='0'
            runrow[r][6]=str(int(float(maxev)*100))
    
    if args['updateRunTable']!=None:
        for r in range(300000):
            if r in runrow.keys():
                print runrow[r]
                comm = ['AddElement', 'AMIUser='+args['updateRunTable'][0] , 'AMIPass='+args['updateRunTable'][1], 'Run='+runrow[r][0], 'Lumi='+runrow[r][1], 'Time='+runrow[r][2], 'mu='+runrow[r][3],'Use='+runrow[r][4],'ReadyForPhysics='+runrow[r][5],'Ext='+runrow[r][6], 'entity=runInfo', 'processingStep=utility', 'project=coll_sizes_tier0']
                #print comm
                result=amiclient.execute(comm)
                
        
        
