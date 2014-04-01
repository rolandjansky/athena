import os, string

#========================================
def getFileListFromCastor(runNumber,inputdirectory,maxNFiles=-1,skipNFiles=0):
   """ This takes a runNumber and an inputdirectory and returns a list of the inputFile files.
       It can deal with a maximum number of files and skipping the first skipNFiles files.
   """
   collection = []
   badList = ["data09_cos.00121330.physics_IDCosmic.merge.DPD_IDCOMM.f118_m106._0009.1\n",
              "data09_cos.00121330.physics_IDCosmic.merge.DPD_IDCOMM.f118_m106._0373.1\n"]
   for m_run in runNumber:
      filelist = os.popen("nsls "+inputdirectory+" |grep "+m_run).readlines()
      for i in filelist:
         if (string.find(i,"pool.root")>0 or string.find(i,"IDCosmic")>0 or string.find(i,".root") )>0:
            if i not in badList:
               collection.append(inputdirectory+"/"+string.strip(i)) 

      
   inputFiles=[]
   max = maxNFiles
   if maxNFiles == -1:
      max=len(collection)

   for i in range(0,max):
      inputFiles.append(collection[i+skipNFiles])
   
   print "|====================================================================================================================|"
   print "The input files are:"
   for n in range(inputFiles.__len__()):
     print inputFiles[n]
   print "|====================================================================================================================|"

   return inputFiles



#========================================
def getInputFileToNumEventMapping(m_list):
   """ Returns a dictionary of the file names
        and the number of events in each file
   """
   print "|====================================================================================================================|"
   print "|                                         Doing the mapping from file to number of events                            |"
   print "|====================================================================================================================|"
   m_numEventsPerFile = {}

   import PyUtils.PoolFile as PF

   failedFiles = ["/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91890_lb7.root","/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91891_lb13.root","/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91891_lb24.root"]

   for i in range(m_list.__len__()):
      if m_list[i] not in failedFiles:
         poolFile = PF.PoolFile( m_list[i] )
         m_numEventsPerFile[m_list[i]] = int(poolFile.fileInfos().split()[6])
   
   print m_numEventsPerFile
   return m_numEventsPerFile



#===========================================
# Creates the input files to be used and
#  determines the num of sub jobs and returns it
#===========================================
def createInputFiles(m_list, m_numEventsPerFile, m_nEvents, m_outputDir, m_maxNumSubJobs, readFromCastor=True):
   """ Returns the number of subjobs needed.
   """
   failedFiles = ["/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91890_lb7.root","/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91891_lb13.root","/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91891_lb24.root"]

   
   # Get the number of subjobs
   totalNumEvents = 0
   for i in range( m_list.__len__() ):
      if m_list[i] not in failedFiles:
         totalNumEvents += m_numEventsPerFile[m_list[i]]

   numSubJobs = int(totalNumEvents/m_nEvents)
   
   if numSubJobs > m_maxNumSubJobs:
      numSubJobs = m_maxNumSubJobs

   print "|====================================================================================================================|"
   print "|  There are "+str(totalNumEvents)+" total number of events"
   print "|  We'll do "+str(numSubJobs)+" sub jobs, with "+str(m_nEvents)+" events each"
   print "|====================================================================================================================|"

   failedFiles = ["/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91890_lb7.root","/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91891_lb13.root","/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91891_lb24.root"]

   # for the subjobs
   m_usedFiles = 0
   m_numberEventsUsed =0
   for i in range(numSubJobs):
      print "Making input file "+ str(i)
      subJobInputFile=open(m_outputDir+"/inputFiles_"+str(i)+".py","w")
      subJobInputFile.write("theApp.EvtMax="+str(m_nEvents)+" \n")

      # add the inputFiles 
      m_eventsNeeded = m_nEvents
      m_addedInputFile = False
      while(m_eventsNeeded != 0 and m_usedFiles < m_list.__len__()):
         if m_list[m_usedFiles] in failedFiles:
            m_usedFiles+=1
            continue
         
         # Used for DeBugging
         #print "m_usedFiles",m_usedFiles,"m_numberEventsUsed",m_numberEventsUsed,"m_eventsNeeded",m_eventsNeeded,m_list[m_usedFiles][0],m_numEventsPerFile[ m_list[m_usedFiles][0] ]
         if m_eventsNeeded <= m_numEventsPerFile[ m_list[m_usedFiles] ]:
            if not m_addedInputFile:
               subJobInputFile.write("ServiceMgr.EventSelector.SkipEvents = "+str(m_numberEventsUsed)+"\n")
               m_addedInputFile = True

            m_numberEventsUsed += m_eventsNeeded
            m_numEventsPerFile[ m_list[m_usedFiles] ] -= m_eventsNeeded
            if readFromCastor:
               subJobInputFile.write("ServiceMgr.EventSelector.InputCollections += ['castor:"+m_list[m_usedFiles]+"'] \n")
            else:
               subJobInputFile.write("ServiceMgr.EventSelector.InputCollections += ['"+m_list[m_usedFiles]+"'] \n")
            m_eventsNeeded = 0
            
         else:
            if not m_addedInputFile:
               subJobInputFile.write("ServiceMgr.EventSelector.SkipEvents = "+str(m_numberEventsUsed)+"\n")
               m_addedInputFile = True

            if readFromCastor:
               subJobInputFile.write("ServiceMgr.EventSelector.InputCollections += ['castor:"+m_list[m_usedFiles]+"'] \n")
            else:
               subJobInputFile.write("ServiceMgr.EventSelector.InputCollections += ['"+m_list[m_usedFiles]+"'] \n")
            m_eventsNeeded -= m_numEventsPerFile[ m_list[m_usedFiles] ]
            m_usedFiles += 1
            m_numberEventsUsed = 0
         
         
      subJobInputFile.write("iterationN=1 \n")#needed? historical
      subJobInputFile.write('StoreGateSvc = Service ("StoreGateSvc" ) \n')
      subJobInputFile.close()

   # for the combinations
   comboInputFile=open(m_outputDir+"/comboInput.py","w")
   comboInputFile.write("theApp.EvtMax=1 \n")
   if readFromCastor:
      comboInputFile.write("ServiceMgr.EventSelector.InputCollections += ['castor:"+m_list[0]+"' ]\n")
   else:
      comboInputFile.write("ServiceMgr.EventSelector.InputCollections += [ '"+m_list[0]+"' ]\n")
   comboInputFile.write("iterationN=911 \n") #needed ? historical.
   comboInputFile.write('StoreGateSvc = Service ("StoreGateSvc" ) \n')
   comboInputFile.close()

   return numSubJobs
