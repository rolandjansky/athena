# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os, time

class manageJobEoverPMaps:
        def __init__(self,
                     OutputPath,
                     dataName,
                     iter,
                     part,
                     JOBNAME,
                     preName,
             ATHENACFG,
                     RecoOptions={},
                     extraOptions={},
                     AlignmentOptions={},
                     RecoScript="InDetAlignExample/NewTopOptions.py",
                     AlignmentScript="InDetAlignExample/NewInDetAlignAlgSetup.py",
                     AlignmentLevels = "InDetAlignExample/NewInDetAlignLevels.py",
                     #MonitoringScript = "InDetRecExample/InDetMonitoringAlignment.py",
                     QUEUE = "1nh",
                     CMTDIR = "",
                     inputPoolFiles = "",
                     SCRIPTNAME = "",
                     RunPath = ""):
                self.OutputPath = OutputPath
                self.preName = preName
                self.i=iter
                self.j=part
                self.dataName = dataName
                self.QUEUE=QUEUE
                self.CMTDIR=CMTDIR
                self.ATHENACFG=ATHENACFG
                self.inputPoolFiles = inputPoolFiles
                self.JOBNAME=JOBNAME
                self.RecoOptions = RecoOptions
                self.extraOptions = extraOptions
                self.RecoScript = RecoScript
                self.AlignmentOptions = AlignmentOptions
                self.AlignmentScript = AlignmentScript
                self.AlignmentLevels = AlignmentLevels
                #self.MonitoringScript = MonitoringScript
                self.SCRIPTNAME = SCRIPTNAME
                self.RunPath = RunPath


        def createDirectories(self):
                os.system('mkdir -p %s' % self.RunPath)

        def writeJO(self):
                job=open(self.RunPath + self.JOBNAME,'w')


                job.write('##-------- Reconstruction Configuration --------------------\n')

                for option in self.RecoOptions:
                        if type(self.RecoOptions[option]) is str:
                                customoption = option + "\t=\t\"" +  self.RecoOptions[option]+'\"\n'
                                job.write(customoption)
                        else:
                                if option == "inputFiles":
                                        customoption = option + "\t=\t" +  str(self.RecoOptions[option])+'\n'
                                else:
                                        customoption = option + "\t=\t" +  str(self.RecoOptions[option])+'\n'
                                job.write(customoption)

                if len(self.extraOptions):
                        job.write("\n")
                        job.write('##-------- Extra Configuration --------------------\n')

                for option in self.extraOptions:
                        if type(self.extraOptions[option]) is str:
                                customoption = option + "\t=\t\"" +  self.extraOptions[option]+'\"\n'
                                job.write(customoption)
                        else:
                                customoption = option + "\t=\t" +  str(self.extraOptions[option])+'\n'
                                job.write(customoption)

                job.write("\n")
                job.write('##-------- End of custom options -------------\n')

                # Need to write the InDetRec Options Here:
                job.write("\n")
                job.write('##-------- Load Reconstruction --------------------\n')
                job.write('include("'+str(self.RecoScript)+'") \n')

                job.close()

        def writeScript(self):
                self.SCRIPTNAME = self.RunPath + '/' + self.SCRIPTNAME
                script=open(self.SCRIPTNAME,'w')
                script.write("#BSUB -J %s_%s_Part%02d \n" % (self.preName,self.dataName, self.j))
                script.write("#BSUB -o %s/logs/%s_Part%02d.log \n" % (self.OutputPath,self.dataName,self.j))
                script.write("#BSUB -e %s/logs/%s_Part%02d.err \n" % (self.OutputPath,self.dataName,self.j))

                script.write("#BSUB -q %s \n" % self.QUEUE)
                script.write("\n")
                script.write("#   setup the environment \n")
                if self.ATHENACFG.atlasSetup() == "CMTHOME":
                        script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))
                elif "single" in self.ATHENACFG.AtlasSetupOptions():
                        script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s --single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
                else:
                        script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
                script.write("export STAGE_SVCCLASS=atldata\n")


                script.write("cd %s \n" % self.RunPath)

                for file in self.inputPoolFiles:
                        #if 'ESD' in file or 'AlignmentConstants' in file:
                        if 'AlignmentConstants' in file or 'ESD' in file:
                                script.write("pool_insertFileToCatalog "+ file + " \n")

                script.write("athena %s \n" % self.JOBNAME)
                script.write("cd -")
                script.close()

        def send(self,runmode):
                os.system("chmod +x %s" % self.SCRIPTNAME)
                print ("----------------------------------------------")
                if runmode == "batch":
                        os.system("bsub <%s" % self.SCRIPTNAME)

                        print ("  Sending %s_%s_Part%02d job to LxBatch" % (self.preName,self.dataName, self.j))

                elif runmode == "local":
                        print ("  Running %s_%s_Part%02d job" % (self.preName,self.dataName,self.j))
                        os.system("sh %s | tee %s/logs/%s_Part%02d.log \n" % (self.SCRIPTNAME, self.OutputPath,self.dataName,self.j))


                print ("----------------------------------------------")

        def wait(self):
                print ("Processing in lxbatch...")
                # Wait for signal
                time.sleep(30)
                while os.popen('bjobs -w').read().find(self.preName)!=-1:
                        time.sleep(30)

class mergeScriptEoverPMaps:
        def __init__(self,
                     OutputPath,
                     preName,
                     QUEUE,
                     CMTDIR,
                     ATHENACFG,
                     SCRIPTNAME,
                     JOBNAME
                     ):
                self.OutputPath = OutputPath
                self.preName = preName
                self.QUEUE=QUEUE
                self.CMTDIR=CMTDIR
                self.ATHENACFG=ATHENACFG
                self.SCRIPTNAME=SCRIPTNAME
                self.JOBNAME=JOBNAME

        def write(self):
                TempPath = self.OutputPath
                self.SCRIPTNAME = TempPath + '/' + self.SCRIPTNAME

                # list of Files to be merged
                mergeFilesName = TempPath + '/merge_Files.txt'
                script=open(self.SCRIPTNAME,'w')

                script.write("#BSUB -J %s_Merge \n" % (self.preName))
                script.write("#BSUB -o %s/logs/%s_Merge.log \n" % (self.OutputPath,self.preName))
                script.write("#BSUB -e %s/logs/%s_Merge.err \n" % (self.OutputPath,self.preName))

                script.write("#BSUB -q %s \n" % self.QUEUE)
                script.write("\n")
                script.write("#   setup the environment \n")
                if self.ATHENACFG.atlasSetup() == "CMTHOME":
                        script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))

                elif "single" in self.ATHENACFG.AtlasSetupOptions():
                        script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s --single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))

                else:
                        script.write("source %s/scripts/asetup.sh %s --testarea=%s --tags=%s \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
                script.write("cd %s \n" % (self.OutputPath))
                script.write("DQHistogramMerge.py %s ./eoverpValidationMerge.root True\n" %(mergeFilesName))
                script.write("cd -")
                script.close()

                mergeFiles=open(mergeFilesName,"w")
                os.chdir(self.OutputPath)
                files = os.popen("find -name 'eoverpValidationOut.root'").readlines()
                for line in files:
                        mergeFiles.write("%s" % line)
                mergeFiles.close()


        def send(self,runmode):
                os.system("chmod +x %s" % self.SCRIPTNAME)
                print ("----------------------------------------------")
                if runmode == "batch":
                        os.system("bsub <%s" % self.SCRIPTNAME)

                        print ("  Sending %s_Merge job " % self.preName)

                elif runmode == "local":
                        print ("  Running %s_Merge job" % self.preName)
                        os.system("sh %s | tee %s/logs/Merge.log \n" % (self.SCRIPTNAME, self.OutputPath))

                print ("----------------------------------------------")
                       
