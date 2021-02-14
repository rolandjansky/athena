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
  
#def formIov(runNumber):
#  s,u=(runNumber << 32), ((runNumber + 1) << 32) - 1
#  print ("formIOV: ", s, u)
#  return s,u
  
#CS: this version is needed if previous runs are processed (make sure runNumberSave is small enough)
def formIov(runNumber):
  #runNumberSave = 364160
  #runNumberSave = 364076
  runNumberSave = 359398
  s,u=(runNumberSave << 32), ((runNumber + 1) << 32) - 1
  return s,u

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
    
    iovSince, iovUntil = formIov(runNumber)
 
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
    for i in range(len(Temp)-1):
        if (Temp[i] != Temp[i+1]):
             ls.append(Temp[i])
   
    #ls.append(Temp[len(Temp)-1])
    print(ls)

    list=[]
    for i in range(k): list.append( ls[len(ls)-i-1] )
        
    return list

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

