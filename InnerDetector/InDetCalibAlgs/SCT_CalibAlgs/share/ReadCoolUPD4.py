############################################################################################
# @file: Script to extract information for the uploaded in the COOL DB runs with UPD4 Tag
#       
# @Author: Peter Vankov <peter.vankov@cern.ch>
###########################################################################################

#!/bin/env python
import os,sys,time,glob,fnmatch, string, datetime
from PyCool import cool,coral
from time import gmtime, strftime

#############################################################################################
def formDbString(technology,server,schema,dbname):
  theString=technology+'://'+server+';schema='+schema+';dbname='+dbname
  return theString
  
def openDatabase(dbstring):
  dbSvc=cool.DatabaseSvcFactory.databaseService()
  technology='oracle'
  server='ATLAS_COOLPROD'
  [schema,dbname]=dbstring.split('/')
  myDbString=formDbString(technology, server, schema, dbname)
  try:
      db=dbSvc.openDatabase(myDbString)
  except Exception as e:
      print('Problem opening %s in GetRunList'%myDbString)
      print(e)
      sys.exit(-1)
  return db

def formIov(runNumber):
  s,u=((runNumber) << 32), ((runNumber + 1) << 32) - 1
  print ("formIOV: ", s, u)
  return s,u

def formIovFromTo(runNumberStart, runNumber):
  s,u=((runNumberStart ) << 32), ((runNumber + 1) << 32) - 1
  print ("formIOV: ", s, u)
  return s,u
  
#CS this modification checks which was the k-last run before the processed one
def getRunNumberStart(runNumber, k):
    runlistfile = open("/afs/cern.ch/user/s/sctcalib/scratch0/lastRuns.txt", "r")
    content = runlistfile.read()
    runlistfile.close()
    content_list = content.split("\n")
    content_list = [line for line in content_list if line.strip()]
    integer_int_list = list(map(int, content_list))
    if (runNumber >= integer_int_list[-1]):
        RNS = integer_int_list[len(integer_int_list)-(k+5)]
    else:
        RNS = integer_int_list[integer_int_list.index(runNumber)-(k+5)]
    return RNS
  
#############################################################################################
def GetRunList(dbstring, folder, tag, runNumber, k):
    db=openDatabase(dbstring)
    print('TAG   ',tag,'   TAG')
    folderlist=db.listAllNodes()
    print('    folderlist=db.listAllNodes()')

    for i in folderlist: print(i)

    myfolder=db.getFolder(folder)
    taglist=myfolder.listTags()
    print('    taglist=myfolder.listTags()')
    for i in taglist: print(i)
    
    runNumberStart = getRunNumberStart(runNumber, k)
    
    iovSince, iovUntil = formIovFromTo(runNumberStart,runNumber)
 
    temp=[]
    objs=myfolder.browseObjects(iovSince,iovUntil,cool.ChannelSelection.all(),tag)
    for obj in objs:
        mypayload=obj.payload()
        dfl=mypayload['DefectList']
        temp.append(obj.since()>>32)
    objs.close()
    db.closeDatabase()
    
    Temp=sorted(temp)
    #print('GetRunList: array is', Temp)
    ls=[]
    runtmp = -999
    if ( Temp[0] != runNumber ):
        ls.append(Temp[0])
        runtmp = Temp[0]
    for i in range(len(Temp)-1):
        if (Temp[i] != runtmp):
             if ( Temp[i] != runNumber ):
                 ls.append(Temp[i])
                 runtmp = Temp[i]
   
    print(ls)

    mylist=[]
    for i in range(k): mylist.append( ls[len(ls)-i-1] )
        
    return mylist

#############################################################################################
def GetNumNoisyMods(dbstring, folder, tag, runNumber):
    db=openDatabase(dbstring)

    myfolder=db.getFolder(folder)
    iovSince, iovUntil = formIov(runNumber)

    NumNoisyMods=0
    objs=myfolder.browseObjects(iovSince,iovUntil,cool.ChannelSelection.all(),tag)
    for obj in objs:
        mypayload=obj.payload()
        dfl=mypayload['DefectList']
        if not dfl:
            continue
        NumNoisyMods=NumNoisyMods+1
    objs.close()
    db.closeDatabase()
    
    return NumNoisyMods

#############################################################################################
def GetNumNoisyStrips(dbstring, folder, tag, runNumber):
    db=openDatabase(dbstring)
    myfolder=db.getFolder(folder)
    iovSince, iovUntil = formIov(runNumber) 

    Array_numNoisyStrips, numNoisyStrips = [], 0
    objs=myfolder.browseObjects(iovSince,iovUntil,cool.ChannelSelection.all(),tag)
    for obj in objs:
        mypayload=obj.payload()
        dfl=mypayload['DefectList']
        if not dfl:
            continue
        # line = str(obj.since()>>32)+"  "+str(dfl)
        line = str(dfl)
        words = line.split()
        numNoisyStripsAdds=0
        for j in range(len(words)):
            jk=words[j]
            if jk.find("-")>=0:
                sep=jk.replace ( '-', ' ' )
                sep1=sep.split()
                numNoisyStripsAdds=numNoisyStripsAdds+int(sep1[1])-int(sep1[0])
        numNoisyStrips=numNoisyStrips+len(words)+numNoisyStripsAdds
    objs.close()
    db.closeDatabase()

    return numNoisyStrips

#############################################################################################
### --> Execution phase: Called in SCTCalib_topOptions.py
###     (Uncomment below if want to use as standalone script)
#############################################################################################

### In order to get the run number we need:
# from SCTCalibConfig import RunNumber
##########################################
# # # RunNumber=161520

# k=5 
# dbstring="ATLAS_COOLOFL_SCT/COMP200"
# folder='/SCT/Derived/Monitoring'
# tag="SctDerivedMonitoring-UPD4-001"
    
# RunList=GetRunList(dbstring, folder, tag, RunNumber, k)

# sumNoisyModulesInDB=0
# print("------------> Runs to compare in COOL DB <--------------------------------------")
# for i in range(len(RunList)):

    # numNoisyMods=GetNumNoisyMods(dbstring, folder, tag, RunList[i])
    # numNoisyStrips=GetNumNoisyStrips(dbstring, folder, tag, RunList[i])

    # if (i==0): NoisyStripLastRunInDB = numNoisyStrips

    # print("", "Run: ", RunList[i], "Noisy Modules: ", numNoisyMods, "Noisy Strips: ", numNoisyStrips)
        
    # sumNoisyModulesInDB = sumNoisyModulesInDB + numNoisyMods

# if (len(RunList) != 0):
    # NoisyModuleAverageInDB = float(sumNoisyModulesInDB) / float(len(RunList))               
# else:
    # NoisyModuleAverageInDB=-999.

# print("")
# print("--> The average value of the number of noisy modules over the above runs is ", NoisyModuleAverageInDB)
# print("--> The number of nosiy strips in the last uploaded run, ", RunList[0],", is ", NoisyStripLastRunInDB)
# print("--------------------------------------------------------------------------------")

