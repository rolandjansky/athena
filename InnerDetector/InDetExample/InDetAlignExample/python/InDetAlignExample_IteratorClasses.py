# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# =====================================================================
#
#  Class for local processing
#
# =====================================================================

from threading import Thread
import os
import time
class runProcess(Thread):
    def __init__ (self,
                  jobOptions,
                  iter,
                  part,
                  OutputPath,
                  OutputLevel):
        Thread.__init__(self)
        self.jobOptions = jobOptions
        self.i = iter
        self.j = part
        self.OutputPath = OutputPath
        self.OutputLevel = OutputLevel
        self.status = -1
    def run(self):
        print ("----------------------------------------------")
        print ("  Running Iter%d - Part%02d in local machine" % (self.i, self.j))
        print ("----------------------------------------------")
        print (" - output path: %s/Iter%d/%02d" % (self.OutputPath, self.i, self.j))

        if self.OutputLevel == 'INFO':
            print ("Running...")
            process=os.popen("athena.py %s >& %s/Iter%d/logs/Iter%dPart%02d.log" % (self.jobOptions, self.OutputPath, self.i, self.i ,self.j))
        else:
            process=os.popen("athena.py %s | tee %s/Iter%d/logs/Iter%dPart%02d.log" % (self.jobOptions, self.OutputPath, self.i, self.i ,self.j))
        while 1:
            line = process.readline()
            if self.OutputLevel == 'DEBUG':
                print (line, end='')
            if not line:
                self.status=1
                break
            
class SortCpus:
        def __init__(self, TOTALCPUS, LOCALDIR, FILELIST, OutputLevel):
                def sort_by_value(d):
                        """ Returns the keys of dictionary d sorted by their values """
                        items=d.items()
                        backitems=[ [v[1],v[0]] for v in items]
                        backitems.sort()
                        backitems.reverse()
                        return [ backitems[i][1] for i in range(0,len(backitems))]
                self.OutputLevel = OutputLevel
                inputfiles = open(FILELIST, "r")
                filelist = inputfiles.read().split()
                inputfiles.close()
                if not LOCALDIR:
                        print ("Reading Custom File")
                        FinalListSorted = []
                        for line in filelist:
                                if line and line[0] != '#':
                                        FinalListSorted.append(line)
                        print (FinalListSorted)

                elif "castor" in LOCALDIR:
                        print ("Reading castor directory. Please wait...")
                        extendedFileList = os.popen("rfdir "+ LOCALDIR[7:]).read().splitlines()
                else:
                        print ("Reading directory. Please wait...")
                        extendedFileList = os.popen("ls -l "+ LOCALDIR).read().splitlines()

                if LOCALDIR:
                        i = 0
                        SizeList = {}
                        for line in extendedFileList:
                                curr = line.split()
                                SizeList[i] = {}
                                SizeList[i][0] = curr[8]
                                SizeList[i][1] = curr[4]
                                i = i+1
                        FinalList = {}
                        for i in range(0,len(SizeList)):
                                if SizeList[i][0] in filelist:
                                        #print (SizeList[i][0], " size:", SizeList[i][1])
                                        FinalList[SizeList[i][0]] = int(SizeList[i][1])

                        #SizeListSorted = [ (k,SizeList[k]) for k in sorted(SizeList.values())]
                        FinalListSorted = sort_by_value(FinalList)
                        #print ("Sorted list" )
                        #for i in range(0,len(FinalListSorted)):
                        #       print (FinalListSorted[i], "\tsize:\t", FinalList[FinalListSorted[i]])
                currCPU = 0
                reverse = False
                self.CPUsFiles = {}
                for i in range(0,len(FinalListSorted)):
                        #print (FinalListSorted[i], "CPU: ", currCPU)
                        if currCPU in self.CPUsFiles:
                                self.CPUsFiles[currCPU].append(LOCALDIR+FinalListSorted[i])
                        else:
                                self.CPUsFiles[currCPU] = [LOCALDIR+FinalListSorted[i]]
                        if(not reverse):
                                currCPU = currCPU + 1
                                if(currCPU == TOTALCPUS):
                                        #currCPU = currCPU - 1
                                        reverse = not reverse
                        if(reverse):
                                currCPU = currCPU - 1
                                if(currCPU == -1):
                                        currCPU = currCPU + 1
                                        reverse = not reverse
                    
                                        
        def getCPU(self,CURRENTCPU):
                if self.OutputLevel=='DEBUG':
                        print ("|",40*"-"," CPU #: ", CURRENTCPU, 40*"-", "|")
                        for line in self.CPUsFiles[CURRENTCPU]:
                                print ("|  - ",line)
                        print ("|",93*"-","|")
                return self.CPUsFiles[CURRENTCPU]
           
                

class writeJob:
    def __init__(self,
                 OutputPath,
                 Events,
                 CosmicsNoBField,
                 CosmicsBField,
                 BeamHalo,
                 BeamGas,
                 Challenge,
                 Nominal,
                 Aligned,
                 Files,
                 ReadAlignmentConstants,
                 PreviousIteration,
                 RunGlobalChi2Align,
                 RunRobustAlign,
                 SolveOption,
                 AlignLevel,
                 iter,
                 part,
                 JOBNAME
                 ):
        self.OutputPath = OutputPath
        self.Events = Events
        self.CosmicsNoBField = CosmicsNoBField
        self.CosmicsBField = CosmicsBField
        self.BeamHalo = BeamHalo
        self.BeamGas = BeamGas
        self.Nominal = Nominal
        self.Aligned = Aligned
        self.Challenge = Challenge
        self.Files = Files
        self.ReadAlignmentConstants = ReadAlignmentConstants
        self.PreviousIteration = PreviousIteration
        self.RunGlobalChi2Align = RunGlobalChi2Align
        self.RunRobustAlign     = RunRobustAlign    
        self.SolveOption = SolveOption
        self.AlignLevel = AlignLevel
        self.i = iter
        self.j = part
        self.JOBNAME = JOBNAME

    def write(self):
        PrefixName="Iter%d_" % self.i
        if self.SolveOption==3:
            TempPath="%s/Iter%d" % (self.OutputPath, self.i)
        else:
            if self.CosmicsNoBField:
                TempPath="%s/Iter%d/CosmicsNoBField/%02d" % (self.OutputPath, self.i, self.j)
            elif self.CosmicsBField:
                TempPath="%s/Iter%d/CosmicsBField/%02d" % (self.OutputPath, self.i, self.j)
            elif self.BeamHalo:
                TempPath="%s/Iter%d/Halo/%02d" % (self.OutputPath, self.i, self.j)
            elif self.BeamGas:
                TempPath="%s/Iter%d/Gas/%02d" % (self.OutputPath, self.i, self.j)
            else:
                TempPath="%s/Iter%d/Collision/%02d" % (self.OutputPath, self.i, self.j)

        os.system('mkdir -p %s' % TempPath)
        topOptionFileName = ''
        if self.CosmicsNoBField or self.CosmicsBField:
            topOptionFileName  = 'InDetAlignExample_Cosmic_topOptions.py'
        elif self.BeamHalo:
            topOptionFileName = 'InDetAlignExample_Halo_topOptions.py'
        elif self.BeamGas:
            topOptionFileName = 'InDetAlignExample_Gas_topOptions.py'
        else:
            topOptionFileName = 'InDetAlignExample_topOptions.py'
        os.system("get_files -jo InDetAlignExample/"+topOptionFileName+" >/dev/null")
        topOptions=open(topOptionFileName,'r')
            
        #job=open(TempPath+"/"+self.JOBNAME,'w')
        job=open(self.JOBNAME,'w')
        job.write(topOptions.readline())
        job.write(topOptions.readline())
        job.write("\n")
        job.write("InDetAlignExampleFlags.Iteration              = %d \n" % self.i)
        job.write("InDetAlignExampleFlags.EvtMax                 = %d \n" % self.Events)
        job.write("InDetAlignExampleFlags.challenge              = \"%s\" \n" % (self.Challenge))
        job.write("InDetAlignExampleFlags.Magnet                 = %s \n" % (self.CosmicsBField))
        job.write("InDetAlignExampleFlags.Cosmics                = %s \n" % (self.CosmicsBField or self.CosmicsNoBField)  )
        job.write("InDetAlignExampleFlags.BeamHalo               = %s \n" % self.BeamHalo)
        job.write("InDetAlignExampleFlags.BeamGas                = %s \n" % self.BeamGas)
        job.write("InDetAlignExampleFlags.SiNominal              = %s \n" % self.Nominal)
        job.write("InDetAlignExampleFlags.Aligned                = %s \n" % self.Aligned)
        job.write("InDetAlignExampleFlags.ReadAlignmentConstants = %s \n" % self.ReadAlignmentConstants)
        job.write("InDetAlignExampleFlags.AlignConstsPath        = \"%s/Iter%d/\" \n" % (self.OutputPath, (self.i-1)))
        job.write("InDetAlignExampleFlags.WritePrefixName        = \"%s\" \n" % PrefixName)
        job.write("InDetAlignExampleFlags.ReadPrefixName         = \"%s\" \n" % self.PreviousIteration)
        job.write("InDetAlignExampleFlags.doGlobalChi2Align      = %s \n" % self.RunGlobalChi2Align)
        job.write("InDetAlignExampleFlags.doRobustAlign          = %s \n" % self.RunRobustAlign)
        job.write("InDetAlignExampleFlags.OutputPath             = \"%s/\" \n" % TempPath)
        job.write("InDetAlignExampleFlags.Files       = %s \n" % self.Files)
        
        if self.RunRobustAlign:
            if self.SolveOption==0: # for RA only
                job.write("InDetAlignExampleFlags.TextFileWriteIndex     = %d \n" % self.j)
                job.write("InDetAlignExampleFlags.TextFileReadEndIndex   = 1 \n")
            if self.SolveOption==3: # for RA only
                job.write("InDetAlignExampleFlags.TextFileWriteIndex     = 999 \n") # for RA
                job.write("InDetAlignExampleFlags.TextFileReadEndIndex   = %d \n" % (self.j-1)) # dummy for RA
        job.write("InDetAlignExampleFlags.AlignLevel             = %d \n" % self.AlignLevel)        
        job.write("InDetAlignExampleFlags.SolveOption            = %d \n" % self.SolveOption)
        job.write("printfunc (' Outputpath = ', InDetAlignExampleFlags.OutputPath) \n")
        job.write("\n")
        
        for line in topOptions.readlines():
            job.write(line)
        job.close()
        os.system("rm "+topOptionFileName)

class writeScript:        
    def __init__(self,
                 OutputPath,
                 preName,
                 iter,
                 part,
                 CosmicsNoBField,
                 CosmicsBField,
                 BeamHalo,
                 BeamGas,
                 QUEUE,
                 CMTDIR,
                 ATHENAREL,
                 TAGS,
                 RUNPATH,
                 SCRIPTNAME,
                 JOBNAME
                 #files 
                 ):
        self.OutputPath = OutputPath
        self.preName = preName
        self.i=iter
        self.j=part
        self.CosmicsNoBField = CosmicsNoBField
        self.CosmicsBField = CosmicsBField
        self.BeamHalo = BeamHalo
        self.BeamGas = BeamGas
        self.QUEUE=QUEUE
        self.CMTDIR=CMTDIR
        self.ATHENAREL=ATHENAREL
        self.TAGS = TAGS
        self.RUNPATH=RUNPATH
        self.SCRIPTNAME=SCRIPTNAME
        self.JOBNAME=JOBNAME
        #self.files = files
        #self.temppath = ""
    def write(self):
        script=open(self.SCRIPTNAME,'w')
        ##   if self.j!=-1: 
        ##             if self.CosmicsNoBField:
        ##                 temppath = "%s/Iter%d/CosmicsNoBField/%02d/" % (self.OutputPath,self.i,self.j)
        ##             elif self.CosmicsBField:
        ##                 temppath = "%s/Iter%d/CosmicsBField/%02d/" % (self.OutputPath,self.i,self.j)
        ##             elif self.BeamHalo:
        ##                 temppath = "%s/Iter%d/Halo/%02d/" % (self.OutputPath,self.i,self.j)
        ##             elif self.BeamGas:
        ##                 temppath = "%s/Iter%d/Gas/%02d/" % (self.OutputPath,self.i,self.j) 
        ##             else:
        ##                 temppath = "%s/Iter%d/Collision/%02d/" % (self.OutputPath,self.i,self.j)
        ##         else:
        ##             temppath = "%s/Iter%d/" % (self.OutputPath,self.i)
            
        ##         script=open(temppath+self.SCRIPTNAME,'w')
        if self.j!=-1:
            if self.CosmicsNoBField:
                script.write("#BSUB -J %s_Iter%dPart%02d_CosmicNoBField \n" % (self.preName,self.i, self.j))
                script.write("#BSUB -o %s/Iter%d/logs/Iter%dPart%02d_CosmicNoBField.log \n" % (self.OutputPath,self.i,self.i,self.j))
            elif self.CosmicsBField:
                script.write("#BSUB -J %s_Iter%dPart%02d_CosmicBField \n" % (self.preName,self.i, self.j))
                script.write("#BSUB -o %s/Iter%d/logs/Iter%dPart%02d_CosmicBField.log \n" % (self.OutputPath,self.i,self.i,self.j))    
            elif self.BeamHalo:
                script.write("#BSUB -J %s_Iter%dPart%02d_Halo \n" % (self.preName,self.i, self.j))
                script.write("#BSUB -o %s/Iter%d/logs/Iter%dPart%02d_Halo.log \n" % (self.OutputPath,self.i,self.i,self.j))
            elif self.BeamGas:
                script.write("#BSUB -J %s_Iter%dPart%02d_Gas \n" % (self.preName,self.i, self.j))
                script.write("#BSUB -o %s/Iter%d/logs/Iter%dPart%02d_Gas.log \n" % (self.OutputPath,self.i,self.i,self.j))
            else:
                script.write("#BSUB -J %s_Iter%dPart%02d_Collision \n" % (self.preName,self.i, self.j))
                script.write("#BSUB -o %s/Iter%d/logs/Iter%dPart%02d_Collision.log \n" % (self.OutputPath,self.i,self.i,self.j))
        else:
            script.write("#BSUB -J %s_Iter%dSolve \n" % (self.preName, self.i))
            script.write("#BSUB -o %s/Iter%d/logs/Iter%dSolve.log \n" % (self.OutputPath,self.i,self.i))
        
        script.write("#BSUB -q %s \n" % self.QUEUE)
        script.write("\n")
        script.write("#   setup the environment \n")
        script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENAREL, self.TAGS))
        script.write("source %s/../InnerDetector/InDetExample/InDetAlignExample/cmt/setup.sh \n" % self.CMTDIR)

#        script.write("source %s/../%s/InnerDetector/InDetExample/InDetAlignExample/cmt/setup.sh \n" % (self.CMTDIR,self.ATHENAREL))

#        script.write("cd %s \n" % temppath)
        script.write("cd %s \n" % self.RUNPATH)

       ##  for file in self.files:
##             script.write("pool_insertFileToCatalog %s\n" % file)
        script.write("athena.py %s \n" % self.JOBNAME)
        script.close()
      #  self.temppath = temppath
    def send(self):
        
        print ("----------------------------------------------")
        if self.j==-1:
            print ("  Sending %s_Solve_Iter%d job to LxBatch " % (self.preName, self.i))
        else:
            if self.CosmicsNoBField:
                print ("  Sending %s_Iter%d_Part%02d_CosmicsNoBField job to LxBatch" % (self.preName,self.i,self.j))
            elif self.CosmicsBField:
                print ("  Sending %s_Iter%d_Part%02d_CosmicsBField job to LxBatch" % (self.preName,self.i,self.j))
            elif not self.BeamHalo and not self.BeamGas:
                print ("  Sending %s_Iter%d_Part%02d_Collision job to LxBatch" % (self.preName,self.i,self.j))
            elif self.BeamHalo:
                print ("  Sending %s_Iter%d_Part%02d_Halo job to LxBatch" % (self.preName,self.i,self.j))
            elif self.BeamGas:
                print ("  Sending %s_Iter%d_Part%02d_Gas job to LxBatch" % (self.preName,self.i,self.j))
        print ("----------------------------------------------")

        os.system("chmod +x %s" % self.SCRIPTNAME)
        os.system("bsub <%s" % self.SCRIPTNAME)

        #os.system("chmod +x %s" % self.temppath+self.SCRIPTNAME)
        #os.system("bsub <%s" % self.temppath+self.SCRIPTNAME)

    def wait(self):
        print ("Processing in lxbatch...")
        # Wait for signal
        time.sleep(30)
        while os.popen('bjobs -w').read().find(self.preName)!=-1:
            time.sleep(30)


class collectRAmodules:        
    def __init__(self,
                 OutputPath,
                 iter,
                 CPUs,
                 RUNPATH,
                 info
                 ):
        self.OutputPath=OutputPath
        self.i=iter
        self.CPUs=CPUs
        self.RUNPATH=RUNPATH
        self.info=info

    def run(self):
        print ()
        print ("------------------------------------------")
        print ("  Collecting Iter%d RA module files" % self.i)
        print ("------------------------------------------")
        os.chdir("%s/Iter%d" % (self.OutputPath,self.i))
        os.mkdir("moduleRA")
        
        firstMatrix = 0
        TempPath = "%s/Iter%d/Collision/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
             TempPath = "%s/Iter%d/CosmicsNoBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
             TempPath = "%s/Iter%d/CosmicsBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
             TempPath = "%s/Iter%d/Halo/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
            TempPath = "%s/Iter%d/Gas/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        print ("checking %s/Iter%d_AlignmentRAmodule_%dPIX/SCT.txt" % (TempPath,self.i,firstMatrix))
        while not ( os.path.isfile( "%s/Iter%d_AlignmentRAmodule_%dPIX.txt" % (TempPath,self.i,firstMatrix) ) or os.path.isfile( "%s/Iter%d_AlignmentRAmodule_%dSCT.txt" % (TempPath,self.i,firstMatrix) ) ):
            print ("%s/Iter%d_AlignmentRAmodule_%dPIX/SCT.txt" % (TempPath,self.i,firstMatrix))
            print ("WARNING: Initial Check: RA module file with index %02d is missing" % firstMatrix)
            self.info.write("WARNING: Initial Check: RA module file with index %02d is missing\n" % firstMatrix)
            firstMatrix += 1
            if firstMatrix == self.CPUs:
                print ('ERROR: Initial Check: No RA module files found')
                break
            TempPath = "%s/Iter%d/Collision/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsNoBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Halo/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Gas/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        
        # looping over CPUs
        for j in range(firstMatrix, self.CPUs):
            TempPath = "%s/Iter%d/Collision/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsNoBField/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsBField/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Halo/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Gas/%02d/" %  (self.OutputPath,self.i,j)
            if os.path.isfile("%s/Iter%d_AlignmentRAmodule_%dPIX.txt" % (TempPath,self.i,j)) or os.path.isfile("%s/Iter%d_AlignmentRAmodule_%dSCT.txt" % (TempPath,self.i,j)):
                os.system("cp %s/Iter%d_AlignmentRAmodule_*txt %s/Iter%d/moduleRA/." % (TempPath,self.i,self.OutputPath,self.i))
                self.info.write("RA module file with index %02d exists\n" % j)
                print ("RA module file with index %02d exists" % j      )
            else:
                print ("WARNING: RA module file with index %02d is missing" % j)
                print ("file name: %s/Iter%d_AlignmentRAmodule_%dPIX/SCT.txt" % (TempPath,self.i,j))
                self.info.write("WARNING: RA module file with index %02d is missing\n" % j)
        os.chdir(self.RUNPATH)


class mergeMatrix:        
    def __init__(self,
                 OutputPath,
                 iter,
                 CPUs,
                 RUNPATH,
                 addbigPath,
                 info
                 ):
        self.OutputPath=OutputPath
        self.i=iter
        self.CPUs=CPUs
        self.RUNPATH=RUNPATH
        self.addbigPath=addbigPath
        self.info=info
        
        
    def run(self):
        print ()
        print ("------------------------------------------")
        print ("  Merging Iter%d GX2 Matrices" % self.i)
        print ("------------------------------------------")
        if os.environ['HOSTNAME'] != 'tst01.ific.uv.es':
            os.chdir(self.addbigPath)
            print (self.addbigPath)
            print ("compiling matrix merger...")
            os.system("g++  -Wno-deprecated big_add.C -o big_add.exe")

        os.chdir("%s/Iter%d" % (self.OutputPath,self.i))
        os.mkdir("temp")
        firstMatrix = 0
        TempPath = "%s/Iter%d/Collision/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
             TempPath = "%s/Iter%d/CosmicsNoBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
             TempPath = "%s/Iter%d/CosmicsBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)   
        if not os.path.isdir(TempPath):
             TempPath = "%s/Iter%d/Halo/%02d/" %  (self.OutputPath,self.i,firstMatrix)
        if not os.path.isdir(TempPath):
            TempPath = "%s/Iter%d/Gas/%02d/" %  (self.OutputPath,self.i,firstMatrix)
             
        while not os.path.isfile( "%s/Iter%d_bigmatrix.bin" % (TempPath,self.i)):
            print ("%s/Iter%d_bigmatrix.bin" % (TempPath,self.i))
            print ("WARNING: Matrix number %02d is missing" % firstMatrix)
            self.info.write("WARNING: Matrix number %02d is missing\n" % firstMatrix)
            firstMatrix += 1
            if firstMatrix == self.CPUs:
                print ('ERROR: No matrices found')
                break
            TempPath = "%s/Iter%d/Collision/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsNoBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsBField/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Halo/%02d/" %  (self.OutputPath,self.i,firstMatrix)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Gas/%02d/" %  (self.OutputPath,self.i,firstMatrix)
                
        #self.info.write("Matrix number %02d exists\n" % firstMatrix)
        print ("Matrix number %02d exists" % firstMatrix            )
        
        os.system("cp %s/Iter%d_bigmatrix.bin %s/Iter%d/temp/bigmatrix.bin" % (TempPath,self.i,self.OutputPath,self.i))
        os.system("cp %s/Iter%d_bigvector.bin %s/Iter%d/temp/bigvector.bin" % (TempPath,self.i,self.OutputPath,self.i))
        os.system("cp %s/Iter%d_hitmap.txt %s/Iter%d/temp/hitmap.txt" % (TempPath,self.i,self.OutputPath,self.i))

        # looping over CPUs
        for j in range(firstMatrix+1, self.CPUs):
            TempPath = "%s/Iter%d/Collision/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsNoBField/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/CosmicsBField/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Halo/%02d/" %  (self.OutputPath,self.i,j)
            if not os.path.isdir(TempPath):
                TempPath = "%s/Iter%d/Gas/%02d/" %  (self.OutputPath,self.i,j)
            if os.path.isfile("%s/Iter%d_bigmatrix.bin" % (TempPath,self.i)):
                os.system("cp %s/Iter%d_bigmatrix.bin %s/bigmatrix.bin" % (TempPath,self.i,TempPath))
                os.system("cp %s/Iter%d_bigvector.bin %s/bigvector.bin" % (TempPath,self.i,TempPath))
                os.system("cp %s/Iter%d_hitmap.txt %s/hitmap.txt" %       (TempPath,self.i,TempPath))
                self.info.write("Matrix number %02d exists\n" % j)
                print ("Matrix number %02d exists" % j      )
                print ("merging matrix...")
                m = os.popen("time %s/big_add.exe" % self.addbigPath,'w')
                m.write("%s \n" % TempPath)
                m.write("%s/Iter%d/temp \n" % (self.OutputPath,self.i))
                m.close()
        
                os.system("rm %s/bigmatrix.bin" % TempPath)
                os.system("rm %s/bigvector.bin" % TempPath)
                os.system("rm %s/hitmap.txt" % TempPath)
                          
                os.system("mv %s/Iter%d/bigmatrix.bin %s/Iter%d/temp/" % (self.OutputPath,self.i, self.OutputPath, self.i))
                os.system("mv %s/Iter%d/bigvector.bin %s/Iter%d/temp/" % (self.OutputPath,self.i, self.OutputPath, self.i))
                os.system("mv %s/Iter%d/hitmap.txt %s/Iter%d/temp/" % (self.OutputPath,self.i, self.OutputPath, self.i))
            else:
                print ("WARNING: Matrix number %02d is missing" % j)
                self.info.write("WARNING: Matrix number %02d is missing\n" % j)
    
        #for j in range(0, self.CPUs):
        #    if os.path.isfile("%s/Iter%d/Collision/%02d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,j,self.i)):
        #        os.system("rm %s/Iter%d/Collision/%02d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,j,self.i))
        #    if os.path.isfile("%s/Iter%d/CosmicsNoBField/%02d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,j,self.i)):
        #        os.system("rm %s/Iter%d/CosmicsNoBField/%02d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,j,self.i))
        #    if os.path.isfile("%s/Iter%d/CosmicsBField/%02d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,j,self.i)):
        #        os.system("rm %s/Iter%d/CosmicsBField/%02d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,j,self.i))
               
        os.system("mv %s/Iter%d/temp/bigmatrix.bin %s/Iter%d/Iter%d_bigmatrix.bin" % (self.OutputPath,self.i,self.OutputPath,self.i,self.i))
        os.system("mv %s/Iter%d/temp/bigvector.bin %s/Iter%d/Iter%d_bigvector.bin" % (self.OutputPath,self.i,self.OutputPath,self.i,self.i))
        os.system("mv %s/Iter%d/temp/hitmap.txt %s/Iter%d/Iter%d_hitmap.txt" % (self.OutputPath,self.i,self.OutputPath,self.i,self.i))
        os.rmdir("%s/Iter%d/temp" % (self.OutputPath,self.i))
        os.chdir(self.RUNPATH)


class COG:
    def __init__(self,
                 OutputPath,
                 iter,
                 Nominal,
                 Challenge,
                 JOBNAME,
                 SCRIPTNAME,
                 preName
                 ):
        self.OutputPath=OutputPath
        self.iter=iter
        self.Nominal = Nominal
        self.Challenge = Challenge
        self.JOBNAME = JOBNAME
        self.SCRIPTNAME = SCRIPTNAME
        self.preName=preName
    def writeJob(self):
        TempPath="%s/Iter%d/" % (self.OutputPath, self.iter)
        InputConstants = TempPath+"Iter%d_AlignmentConstantsPreCog.root" % (self.iter)
        OutputConstants =  TempPath+"Iter%d_AlignmentConstants.root" % (self.iter)
        OutputDbConstants =  TempPath+"Iter%d_AlignmentConstants.db" % (self.iter)
        OutputTxtConstants = TempPath+"Iter%d_AlignmentConstants.txt" % (self.iter)
        topOptions=open("InDetAlignCog.py",'r')
        job=open(self.JOBNAME,'w')
        job.write(topOptions.readline())
        job.write(topOptions.readline())
        job.write("\n")
        
        job.write("InputConstants         = \"%s\" \n" % InputConstants) 
        job.write("OutputConstants        = \"%s\" \n" % OutputConstants)
        job.write("OutputDbConstants      = \"%s\" \n" % OutputDbConstants)
        job.write("OutputTxtConstants     = \"%s\" \n" % OutputTxtConstants)
        job.write("onlySilicon            = True \n")
        job.write("ReadAlignmentConstants = True \n")
        job.write("\n")
        
        for line in topOptions.readlines():
            job.write(line)
        job.close()
        
    def moveConstants(self):
        os.system("mv %s/Iter%d/Iter%d_AlignmentConstants.root %s/Iter%d/Iter%d_AlignmentConstantsPreCog.root" % (self.OutputPath, self.iter, self.iter, self.OutputPath, self.iter, self.iter))
        os.system("mv %s/Iter%d/Iter%d_AlignmentConstants.db %s/Iter%d/Iter%d_AlignmentConstantsPreCog.db" % (self.OutputPath, self.iter, self.iter, self.OutputPath, self.iter, self.iter))
        os.system("mv %s/Iter%d/Iter%d_AlignmentConstants.txt %s/Iter%d/Iter%d_AlignmentConstantsPreCog.txt" % (self.OutputPath, self.iter, self.iter, self.OutputPath, self.iter, self.iter))
        
    def writeScript(self,
                    QUEUE,
                    CMTDIR,
                    ATHENAREL,
                    TAGS,
                    RUNPATH):
        script=open(self.SCRIPTNAME,'w')
        script.write("#BSUB -J %s_Iter%dCog \n" % (self.preName,self.iter))         
        script.write("#BSUB -o %s/Iter%d/logs/Iter%dCog.log \n" % (self.OutputPath,self.iter,self.iter))
        script.write("#BSUB -q %s \n" % QUEUE)
        script.write("\n")
        script.write("#   setup the environment \n")
        script.write("source %s/setup.sh -tag=val,%s,%s \n" % (CMTDIR, ATHENAREL, TAGS))
        script.write("source %s/../cmt/setup.sh \n" % RUNPATH)

        script.write("cd %s \n" % RUNPATH)
        script.write("athena.py %s \n" % self.JOBNAME)
        script.close()

    def sendScript(self):
        print ("----------------------------------------------")
        print ("  Sending Iter%d - COG job to LxBatch " % self.iter)
        print ("----------------------------------------------")
        os.system("chmod +x %s" % self.SCRIPTNAME)
        os.system("bsub <%s" % self.SCRIPTNAME)

    def wait(self):
        print ("Processing in lxbatch...")
        # Wait for signal
        time.sleep(30)
        while os.popen('bjobs -w').read().find(self.preName)!=-1:
            time.sleep(30)


    def run(self,
            OutputLevel):
        print ("----------------------------------------------")
        print ("  Running %s_Iter%d - COG in local machine" % (self.preName,self.iter))
        print ("----------------------------------------------")
        print (" - output path: %s/Iter%d/" % (self.OutputPath, self.iter))

        if OutputLevel == 'INFO':
            process=os.popen("athena.py %s > %s/Iter%d/logs/Iter%dCog.log" % (self.JOBNAME, self.OutputPath, self.iter, self.iter))
        else:
            process=os.popen("athena.py %s" % self.JOBNAME)
        while 1:
            line = process.readline()
            if OutputLevel == 'DEBUG':
                print (line, end='')
            if not line:
                self.status=1
                break          
