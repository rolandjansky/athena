#!/afs/cern.ch/sw/lcg/external/Python/2.5.4/slc4_ia32_gcc34/bin/python
#################################################################
#         Script to run obtain the events per file              #
#                 only runs with root files                     #
#################################################################
import os
import PyUtils.PoolFile as PF

#inputPath = 'castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/FDR2_IDCalibStream/ESD/'
inputPath = '/castor/cern.ch/grid/atlas/atlasdatadisk/data09_900GeV/DESD_COLLCAND/r988_p62/'

subpaths = ["data09_900GeV.00141994.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102061_00",
            "data09_900GeV.00142065.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102089_00",
            "data09_900GeV.00142065.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid105339_00",
            "data09_900GeV.00142166.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102148_00",
            "data09_900GeV.00142171.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102155_00",
            "data09_900GeV.00142189.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102169_00",
            "data09_900GeV.00142189.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid105234_00",
            "data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102040_00",
            "data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102054_00",
            "data09_900GeV.00142165.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102141_00",
            "data09_900GeV.00142193.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102194_00",
            "data09_900GeV.00142383.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102217_00",
            "data09_900GeV.00142191.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102182_00",
            "data09_900GeV.00142195.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102208_00",
            "data09_900GeV.00142195.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid105216_00",
            "data09_900GeV.00142149.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102103_00",
            "data09_900GeV.00142154.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102110_00",
            "data09_900GeV.00142174.physics_MinBias.merge.DESD_COLLCAND.r988_p62_tid102162_00"]

ReadNEvents = False
PoolFileCatalog = True
numline = 1


OutputFile = 'GoodRunList_900GeV.txt'

if PoolFileCatalog:
   OutputFile = 'FileToCatalog_GoodRunList_900GeV.xml'
   
outputText = open(OutputFile, "w")
paths = []
if "subpaths" in dir():
   for SP in subpaths:
      paths.append(inputPath+SP+"/")
else:
   paths.append(inputPath)
	
for Path in paths:
   if "castor" in Path:
      print "Reading castor directory. Please wait..."
      inputfiles = os.popen("rfdir "+ Path).read().splitlines()
   else:
      print "Reading directory. Please wait..."
      inputfiles = os.popen("ls -l "+ Path).read().splitlines()

   for line in inputfiles:
      filename = line.split()[8]
      if "root" in filename or "ESD" in filename or "data" in filename:
         fullFilename = Path+filename
         try:
            if ReadNEvents:
               poolFile = PF.PoolFile(fullFilename)
               numEvents = int(poolFile.dataHeader.nEntries)
               print filename
               print numEvents
                  
               if numEvents > EventCut:
                  outputText.write(fullFilename + '\t' + str(numEvents) + '\n')
               else:
                  print "File with few events, skipping..."
            else:
               if not PoolFileCatalog :
                  outputText.write(fullFilename + '\n') 
               if PoolFileCatalog :
                  print " Creating as PoolFileToCatalogue Please wait..."
                  print "  pool_insertFileToCatalog ", fullFilename
                  if numline==1:
                     outputText.write(" pool_insertFileToCatalog "+ fullFilename + ' ')
                  else:
                     outputText.write( fullFilename + ' ')
                             
               numline=numline+1 
				
         except IOError:
            print "problem reading file " + filename + ", skipping..."
      else:
         print filename + " is not a root file, skipping... "
            
outputText.close()



####################################
##    POOL FILE CATALOGUE         ##
####################################
if PoolFileCatalog:
  outputText = open(OutputFile, "r")
  command = outputText.read()  # store the content in a variable
  print "command: ", command
  os.system(command)
  outputText.close()

