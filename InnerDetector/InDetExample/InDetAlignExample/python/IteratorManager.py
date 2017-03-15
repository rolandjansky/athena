class IteratorManager:
    #--------------------------------------------------------------------------------------------------------------
    def __init__(self,
                 config,
                 dataName,
                 iter,
                 part,
                 JOBNAME,
                 ATHENACFG,
                 RecoOptions      ={},
                 RecoScript       = "InDetAlignExample/NewTopOptions.py",
                 ConditionsScript = "InDetAlignExample/jobOption_ConditionsOverrider.py",
                 AlignmentScript  = "InDetAlignExample/NewInDetAlignAlgSetup.py",
                 AlignmentLevels  = "InDetAlignExample/NewInDetAlignLevels.py",
                 inputCoolFiles   = "",
                 nCPUs            = 1,
                 data             = None
                 ):
        
        self.config           = config
        self.dataName         = dataName
        self.iter             = iter
        self.part             = part
        self.ATHENACFG        = ATHENACFG
        self.inputCoolFiles   = inputCoolFiles
        self.JOBNAME          = JOBNAME
        self.RecoOptions      = RecoOptions
        self.ConditionsScript = ConditionsScript
        self.RecoScript       = RecoScript
        self.AlignmentScript  = AlignmentScript
        self.AlignmentLevels  = AlignmentLevels
        self.nCPUs            = nCPUs
        self.inputPoolFiles   = []
        self.data             = data
        
        self.RunPath="%s/%s" % (self.config.OutputPath, self.config.iter_dir( 0 ) )
        
        if self.part==-1:
            self.SCRIPTNAME = "%s_%s_Solve_%s.lsf" % (self.config.preName, self.dataName, self.config.folderSuffix)
        else:
            self.SCRIPTNAME = "%s_%s_%s_Part%02d.lsf" % (self.config.preName,self.dataName, self.config.folderSuffix, self.part)

    
    #--------------------------------------------------------------------------------------------------------------
    def createDirectories(self):
        import os
        if self.part == -1:
            return
        
        #os.system('mkdir -p %s/%s/%s/%02d' % ( self.config.OutputPath, self.config.common_dir(), self.dataName, self.part ) )
        
        for iteration in range(self.config.FirstIteration, self.config.FirstIteration+self.config.Iterations):
            os.system('mkdir -p %s/%s/%s/%02d' % ( self.config.OutputPath, self.config.iter_dir( iteration ), self.dataName, self.part ) )
        
    #--------------------------------------------------------------------------------------------------------------
    def writeJO(self):
        for iteration in range(self.config.FirstIteration, self.config.FirstIteration+self.config.Iterations) :
            
            if self.part == -1:
                jobfile = ("%s/%s" % (self.RunPath, self.JOBNAME ) ).replace("Iter0", "Iter{0}".format( iteration ) )
            else:
                jobfile = ("%s/%s/%02d/%s" % (self.RunPath, self.dataName, self.part, self.JOBNAME ) ).replace("Iter0", "Iter{0}".format( iteration ) )
            
            PrefixName="Iter%d%s_" % (iteration, self.config.folderSuffix)
            
            job=open(jobfile, 'w')
            job.write('##-------- Alignment Configuration --------------------\n')
            
            job.write("outputPoolFile        = \"%s/Iter%d_AlignmentConstants.root\"\n" %(self.RunPath.replace("Iter0", "Iter{0}".format( iteration )), iteration) )
            
            AlignmentOptions = configure_alignment_options( self.config, iteration, self.part, self.data )
                
            self.inputPoolFiles.append( AlignmentOptions["inputPoolFiles"] )
            
            for key, value in sorted( AlignmentOptions.items() ):
                if key == "inputPoolFiles":
                    if iteration == 0:
                        job.write( "{0:50s} = {1}\n".format( key, value ) )
                    else:
                        poolfile = "%s/%s_Iter%d_%s/Iter%d_AlignmentConstants.root" % (self.config.OutputPath.replace("Iter0", "Iter{0}".format( iteration-1 ) ), self.config.preName, iteration-1, self.config.folderSuffix, iteration-1 )
                        job.write( "{0:50s} = [\"{1}\"]\n".format( key, poolfile ) )
                elif key == "inputTFiles":
                    job.write( "{0:50s} = [ INPUTFILELIST ]\n".format( key ) )
                elif type(value) is str:
                    job.write( "{0:50s} = \"{1}\"\n".format( key, value ) )
                else:
                    job.write( "{0:50s} = {1}\n".format( key, value ) )
                    
            
            job.write("\n")
            job.write('##-------- Loading the Alignment Levels --------------------\n')
            job.write('include("'+str(self.AlignmentLevels)+'") \n')
            job.write("\n")
            job.write('##-------- Reconstruction Configuration --------------------\n')
        
            for key, value in sorted( self.RecoOptions.items() ):
                if type(value) is str:
                    job.write( "{0:50s} = \"{1}\"\n".format( key, value ) )
                else:
                    job.write( "{0:50s} = {1}\n".format( key, value ) )
                
            if len(self.config.extraOptions):
                job.write("\n")
                job.write('##-------- Extra Configuration --------------------\n')
        
                for key, value in sorted( self.config.extraOptions.items() ):
                    if type(value) is str:
                        job.write( "{0:50s} = \"{1}\"\n".format( key, value ) )
                    else:
                        job.write( "{0:50s} = {1}\n".format( key, value ) )

                job.write("\n")
                job.write('##-------- End of custom options -------------\n')

            # Need to write the InDetRec Options Here:
            job.write("\n")
            job.write('##-------- Load Reconstruction --------------------\n')
            job.write('include("'+str(self.RecoScript)+'") \n')
            job.write("\n")
            job.write('##-------- Load Conditions Overrider --------------------\n')
            job.write('include("'+str(self.ConditionsScript)+'") \n')
            job.write("\n")
            job.write('##-------- Load Alignment --------------------\n')
            job.write('include("'+str(self.AlignmentScript)+'") \n')
            job.write("\n")
            job.close()
                       
    #--------------------------------------------------------------------------------------------------------------
    def writeScript(self):
        import os
        
        workdir="%s/%s" % (self.config.OutputPath, self.config.common_dir() )
        
        if self.part == -1 :
            self.SCRIPTNAME = "%s/%s" % ( workdir, self.SCRIPTNAME ) 
        else:
            self.SCRIPTNAME = "%s/%s/%s" % ( workdir, self.dataName, self.SCRIPTNAME ) 
            
        script=open(self.SCRIPTNAME,'w')
        if self.part!=-1:
            script.writelines(
                [ "#BSUB -J %s_%s_%s_Part%02d -C 1\n"         % ( self.config.preName, self.dataName, self.config.folderSuffix, self.part),
                  "#BSUB -o %s/logs/%s_%s_%s_Part%02d.log \n" % ( workdir, self.config.preName, self.dataName, self.config.folderSuffix, self.part),
                  "#BSUB -e %s/logs/%s_%s_%s_Part%02d.err \n" % ( workdir, self.config.preName, self.dataName, self.config.folderSuffix, self.part)   ]   )
        else:
            script.writelines(
                [ "#BSUB -J %s_%s_Solve_%s -C 1\n" % ( self.config.preName, self.dataName, self.config.folderSuffix ),
                  "#BSUB -o %s/logs/%s_%s_Solve_%s.log \n" % ( workdir, self.config.preName, self.dataName, self.config.folderSuffix ),
                  "#BSUB -e %s/logs/%s_%s_Solve_%s.err \n" % ( workdir, self.config.preName, self.dataName, self.config.folderSuffix ),  ]   )
            
        script.writelines( 
            [ "#BSUB -q %s \n\n" % self.config.QUEUE,
              "#   setup the environment \n"           ] )
        
        
        if self.ATHENACFG.atlasSetup() == "CMTHOME":
            script.write("source %s/setup.sh -tag=%s,%s \n" % (self.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))
            script.write("source %s/build/x86_64-slc6-gcc49-opt/setup.sh\n" % (self.ATHENACFG.TestArea()) )
            script.write("export CORAL_DBLOOKUP_PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.0/AtlasCore/21.0.8/InstallArea/x86_64-slc6-gcc49-opt/XML/AtlasAuthentication\n")
        elif "single" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source %s/scripts/asetup.sh %s \\\n--testarea=%s \\\n--tags=%s \\\n--single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
            script.write("source %s/build/x86_64-slc6-gcc49-opt/setup.sh\n" % (self.ATHENACFG.TestArea()) )
            script.write("export CORAL_DBLOOKUP_PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.0/AtlasCore/21.0.8/InstallArea/x86_64-slc6-gcc49-opt/XML/AtlasAuthentication\n")


        elif "nightlies" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 20.7.X rel1 \\\n--testarea=%s \\\n--single \n" % (self.ATHENACFG.TestArea()))

        else:
            script.write("source %s/scripts/asetup.sh %s \\\n--testarea=%s \\\n--tags=%s \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
            script.write("source %s/build/x86_64-slc6-gcc49-opt/setup.sh\n" % (self.ATHENACFG.TestArea()) )
            script.write("export CORAL_DBLOOKUP_PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.0/AtlasCore/21.0.8/InstallArea/x86_64-slc6-gcc49-opt/XML/AtlasAuthentication\n")
                         
        for file in self.inputPoolFiles:
            if "atlasdatadisk" in file:
                script.write("export STAGE_SVCCLASS=atldata\n")
                break
            if "DAQ" in file or "tzero" in file:
                script.write("export STAGE_SVCCLASS=atlcal\n") 
                break
            if "atlasgroupdisk" in file:
                script.write("export STAGE_SVCCLASS=atlasgroupdisk\n") 
                break
            if "atlasscratchdisk" in file:
                script.write("export STAGE_SVCCLASS=atlasscratchdisk\n")
                break
            if "atlasdatadisk" in file:
                script.write("export STAGE_SVCCLASS=atldata\n")
                break
            if "atlascerngroupdisk" in file:
                script.write("export STAGE_SVCCLASS=atlascerngroupdisk\n")
                script.write("export STAGE_HOST=castoratlast3\n")
                break
        
        for file in self.inputPoolFiles:
            #if 'ESD' in file or 'AlignmentConstants' in file:
            if 'AlignmentConstants' in file or 'ESD' in file:
                script.write("pool_insertFileToCatalog "+ file + " \n")
        
        script.writelines( [
                "\n",
                "#----------------------------------------------------------\n",
                "basedir={0}\n".format( self.config.OutputPath ),
                "preName={0}\n".format( self.config.preName ),
                "dataName={0}\n".format( self.dataName ),
                "folderSuffix={0}\n".format( self.config.folderSuffix ),
                ] )
            
        if self.JOBNAME.find( "_Part" )>0 :
            script.writelines( [
                    "part={0:02d}\n".format( self.part ),
                    "logfile=athena_log.txt\n",
                    "\n",
                    "#----------------------------------------------------------\n",
                    "for iteration in `seq {0} {1}`; do\n" .format( self.config.FirstIteration, self.config.FirstIteration + self.config.Iterations -1 ),
                    "  workdir=${basedir}/${preName}_Iter${iteration}_${folderSuffix}/${dataName}/${part}\n",
                    "  jobfile=${preName}_${dataName}_${folderSuffix}_Part${part}.py\n",
                    "  \n",
                    "  cd ${workdir}\n",
                    "  \n",
                    "  if [ ${iteration} == 0 ] ; then\n",
                    "    while true; do\n",
                    "      athena ${jobfile} &> ${logfile}\n",
                    "      head -n1 ${logfile} > is_finished\n",
                    "      grep -e FATAL -e CRITICAL >> is_finished\n",
                    "      lines=`cat is_finished | wc -l`\n",
                    "      if [ \"${lines}\" -eq 1 ] ; then\n",
                    "        rm ${logfile}\n",
                    "        break\n",
                    "      fi\n",
                    "      \n",
                    "    done\n",
                    "    \n",
                    "  else\n",
                    "    prev_iter=`expr ${iteration} - 1`\n",
                    "    inputPoolFile=${basedir}/${preName}_Iter${prev_iter}_${folderSuffix}/Iter${prev_iter}_AlignmentConstants.root\n",
                    "    \n",
                    "    while true; do\n",
                    "      if [ -e ${inputPoolFile} ] ; then\n",
                    "        athena ${jobfile} &> ${logfile}\n",
                    "        head -n1 ${logfile} > is_finished\n",
                    "        grep -e FATAL -e CRITICAL >> is_finished\n",
                    "        lines=`cat is_finished | wc -l`\n",
                    "        if [ \"${lines}\" -eq 1 ] ; then\n",
                    "          rm ${logfile}\n",
                    "          break\n",
                    "        fi\n",
                    "        \n",
                    "      else\n",
                    "        sleep 60\n",
                    "      fi\n",
                    "    done\n",
                    "    \n",
                    "  fi\n",
                    "done\n" ] )
            
        elif self.JOBNAME.find( "Solve" ) > 0:
            script.writelines( [
                    "logfile=athena_solver_log.txt\n",
                    "\n",
                    "#----------------------------------------------------------\n",
                    "for iteration in `seq {0} {1}`; do\n" .format( self.config.FirstIteration, self.config.FirstIteration + self.config.Iterations -1 ),
                    "  workdir=${basedir}/${preName}_Iter${iteration}_${folderSuffix}\n",
                    "  \n",
                    "  while true; do\n",
                    "    n_finished=`ls ${workdir}/${dataName}/*/is_finished | wc -l`\n",
                    ("    if [ $n_finished == %d ] ; then\n" % self.nCPUs) if self.config.doTruncate == False else ("    if [ $n_finished -ge %d ] ; then\n" % int(self.nCPUs*(1.0-self.config.TruncateThreshold)) ),
                    "      cd ${workdir}\n",
                    "      solver={0}\n".format( self.JOBNAME ),
                    "      \n",
                    "      # evaluate the list of alignment TFiles with lazy evaluation\n",
                    "      array=()\n",
                    "      for file in `ls ${dataName}/*/AlignmentTFile.root`; do\n",
                    "        array=( \"${array[@]}\" \" '${file}'\" )\n",
                    "      done\n",
                    "      str=\"$(IFS=, ; echo \"${array[*]}\")\"\n",
                    "      sed -e \"s@INPUTFILELIST@${str}@g\" ${solver} > tmp; mv tmp ${solver}\n",
                    "      \n",
                    "      athena ${solver} &> ${logfile}\n",
                    "      head -n1 ${logfile} > is_finished\n",
                    "      break\n",
                    "      \n",
                    "    else\n",
                    "      sleep 60\n",
                    "    fi\n",
                    "  done\n",
                    "  \n",
                    "done\n"  ] )
            
        
        #Cleaning the core in case of errors
        script.close()
        os.system("chmod +x %s" % self.SCRIPTNAME)

    #--------------------------------------------------------------------------------------------------------------
    def send(self,runmode):
        import os
        print "----------------------------------------------"
        if runmode == "batch":
            os.system("bsub <%s" % self.SCRIPTNAME)
           
            if self.part==-1:
                print "  Sending %s_Solve_%s job " % (self.config.preName, self.config.folderSuffix)
            else:
                print "  Sending %s_%s_%s_Part%02d job to LxBatch" % (self.config.preName, self.config.folderSuffix, self.dataName, self.part)
                  
        elif runmode == "local":
            if self.part!=-1:
                print "  Running %s_%s_%s_Part%02d job" % (self.config.preName,self.config.folderSuffix,self.dataName,self.part)
                os.system("sh %s | tee %s/%s/logs/%s_%s_Part%02d.log \n" % (self.SCRIPTNAME, self.config.OutputPath, self.config.common_dir(), self.iter,self.config.preName,self.dataName,self.part))
            else:
                print "  Running %s_Iter%dSolve job" % (self.config.preName,self.iter)
                os.system("sh %s | tee %s/%s/logs/%s_Solve.log \n" % (self.SCRIPTNAME, self.config.OutputPath,self.iter,self.config.common_dir(), self.iter,self.config.preName))
            
        print "----------------------------------------------"

    #--------------------------------------------------------------------------------------------------------------
    def wait(self):
        print "Processing in lxbatch... look for jobs with ", self.config.preName, " and ", self.config.folderSuffix  
        # Wait for signal
        #print " wait for signal .salva. "
        time.sleep(30)
        while (os.popen('bjobs -w').read().find(self.config.preName)!=-1 and os.popen('bjobs -w').read().find(self.config.folderSuffix)!=-1):
            time.sleep(30)
            

# For the merging of the monitoring Files
class MergingManager:
    def __init__(self,
                 config,
             iter,
             dataName,
             nCPUs,
             ATHENACFG,
             SCRIPTNAME,
             JOBNAME
             ):
        
        self.config = config
        self.i=iter
        self.dataName = dataName
        self.nCPUs = nCPUs
        self.ATHENACFG=ATHENACFG
        self.SCRIPTNAME=SCRIPTNAME
        self.JOBNAME=JOBNAME


    def write(self):
        import os
        workdir="%s/%s" % (self.config.OutputPath, self.config.common_dir() )

        self.SCRIPTNAME = workdir + '/' + self.SCRIPTNAME
        
        script=open(self.SCRIPTNAME,'w')
        
        script.write("#BSUB -J %s_%s_Merge_%s -C 1\n" % (self.config.preName, self.dataName, self.config.folderSuffix))
        script.write("#BSUB -o %s/logs/%s_%s_Merge_%s.log \n" % (workdir, self.config.preName, self.dataName, self.config.folderSuffix))
        script.write("#BSUB -e %s/logs/%s_%s_Merge_%s.err \n" % (workdir, self.config.preName, self.dataName, self.config.folderSuffix))
        
        script.write("#BSUB -q %s \n" % self.config.QUEUE)
        script.write("\n")
        script.write("#   setup the environment \n")
        if self.ATHENACFG.atlasSetup() == "CMTHOME":
            script.write("source %s/setup.sh -tag=%s,%s \n" % (self.config.CMTDIR, self.ATHENACFG.Release(), self.ATHENACFG.Tags()))
        elif "single" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source %s/scripts/asetup.sh %s \\\n----testarea=%s \\\n--tags=%s \\\n--single \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))
        elif "nightlies" in self.ATHENACFG.AtlasSetupOptions():
            script.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 19.1.X rel1 \\\n--testarea=%s \\\n--single \n" % (self.ATHENACFG.TestArea()))
        else:
            script.write("source %s/scripts/asetup.sh %s \\\n--testarea=%s \\\n--tags=%s \n" % (self.ATHENACFG.AtlasSetupPath(),self.ATHENACFG.Release(),self.ATHENACFG.TestArea(), self.ATHENACFG.Tags()))

        # list of Files to be merged
        mergeFilesName = 'merge_'+self.dataName+'_Files.txt'
        
        script.writelines( [
                "\n",
                "#----------------------------------------------------------\n",
                "basedir={0}\n".format( self.config.OutputPath ),
                "preName={0}\n".format( self.config.preName ),
                "dataName={0}\n".format( self.dataName),
                "folderSuffix={0}\n".format( self.config.folderSuffix ),
                "\n",
                "#---------------------------------------------------\n",
                "for iteration in `seq {0} {1}`; do\n".format( self.config.FirstIteration, self.config.FirstIteration+self.config.Iterations-1 ),
                "  \n",
                "  workdir=${basedir}/${preName}_Iter${iteration}_${folderSuffix}\n",
                "  \n",
                "  while true; do\n",
                "    n_finished=`ls ${workdir}/${dataName}/*/is_finished | wc -l`\n",
                ("    if [ $n_finished == %d ] ; then\n" % self.nCPUs) if self.config.doTruncate == False else ("    if [ $n_finished -ge %d ] ; then\n" % int(self.nCPUs*(1.0-self.config.TruncateThreshold)) ),
                "      cd ${workdir}\n",
                "      \n",
                "      # evaluate the list of monitoring files with lazy evaluation\n",
                "      ls ${workdir}/${dataName}/*/monitoring.root > ${workdir}/%s\n" % ( mergeFilesName ),
                "      DQHistogramMerge.py %s TotalMonitoring.root True &>merger_log.txt\n" %( mergeFilesName ),
                "      \n",
                "      if [ -e ${workdir}/TotalMonitoring.root ] ; then\n",
                "        rm ${workdir}/${dataName}/*/monitoring.root \n",
                "      fi\n",
                "      \n",
                "      break\n",
                "      \n",
                "    else\n",
                "      sleep 60\n",
                "    fi\n",
                "  done\n\n"
                "done\n\n"     ] )
        script.close()
        os.system("chmod +x %s" % self.SCRIPTNAME)
        

    def send(self,runmode):
        import os
        print "----------------------------------------------"
        if runmode == "batch":
            os.system("bsub <%s" % self.SCRIPTNAME)
            
            print "  Sending %s_Merge_%s%s job " % (self.config.preName, self.dataName, self.config.folderSuffix)
            
        elif runmode == "local":
            print "  Running %s_Merge_%s%s job" % (self.config.preName, self.dataName, self.config.folderSuffix)
            os.system("sh %s | tee %s/Iter%d%s/logs/Iter%d%s_%s_Merge.log \n" % (self.SCRIPTNAME, self.config.OutputPath,self.i, self.config.folderSuffix, self.i, self.config.folderSuffix, self.dataName))
            
        print "----------------------------------------------"
                       

#----------------------------------------------------------------------------------------------------
def configure_alignment_options( config, iteration, part, data=None ):
    
    # Get the Alignment Options
    AlignmentOptions = {}
    if config.AlignmentMethod == "GX2":
        AlignmentOptions["runLocal"]      = False
        AlignmentOptions["solveLocal"]    = False
        AlignmentOptions["solvingOption"] = 1
    else:
        AlignmentOptions["runLocal"]      = True
        AlignmentOptions["solvingOption"] = 0
        
    AlignmentOptions["readConstantsFromPool"] = config.readConstantsFromPool
    
    AlignmentOptions["inputPoolFiles"] = []
    if config.readConstantsFromPool:
        if iteration == 0:
            AlignmentOptions["inputPoolFiles"] = [ config.inputAlignmentPoolFile ]
        else:
            constantsFile                      = "%s/%s_Iter%d_%s/Iter%d_AlignmentConstants.root" %( config.OutputPath, config.preName, iteration-1, config.folderSuffix, iteration-1 )
            AlignmentOptions["inputPoolFiles"] = [constantsFile]
    else:
        if iteration != 0:
            constantsFile                      = "%s/%s_Iter%d_%s/Iter%d_AlignmentConstants.root" %( config.OutputPath, config.preName, iteration-1, config.folderSuffix, iteration-1 )
            AlignmentOptions["inputPoolFiles"] = [constantsFile]
            
    if config.readBowingFromCool:
        if iteration == 0:
            bowingdb                                = config.inputBowingCoolFile
            AlignmentOptions["inputBowingDatabase"] = config.inputBowingCoolFile
        else:
            bowingdb                                = "%s/%s_Iter%d_%s/mycool.db" %( config.OutputPath, config.preName, iteration-1, config.folderSuffix )
            AlignmentOptions["inputBowingDatabase"] = bowingdb
    else:
        if iteration != 0:
            bowingdb                                = "%s/%s_Iter%d_%s/mycool.db" %( config.OutputPath, config.preName, iteration-1, config.folderSuffix )
            AlignmentOptions["inputBowingDatabase"] = bowingdb
            
    if config.readDynamicGlobalFoldersFromCool:
        if iteration == 0:
            dynamicdb                                      = config.inputDynamicGlobalCoolFile
            AlignmentOptions["inputDynamicGlobalDatabase"] = config.inputDynamicGlobalCoolFile
        else:
            dynamicdb                                = "%s/%s_Iter%d_%s/mycool.db" %( config.OutputPath, config.preName, iteration-1, config.folderSuffix )
            AlignmentOptions["inputDynamicGlobalDatabase"] = dynamicdb
    else:
        if iteration != 0:
            dynamicdb                                = "%s/%s_Iter%d_%s/mycool.db" %( config.OutputPath, config.preName, iteration-1, config.folderSuffix )
            AlignmentOptions["inputDynamicGlobalDatabase"] = dynamicdb
    
    AlignmentOptions["alignTRT"] = config.AlignTRT[iteration]
    if config.AlignTRT[iteration]:
        AlignmentOptions["trtAlignmentLevel"]        = config.TRTAlignmentLevel        [iteration]
        AlignmentOptions["trtAlignBarrel"]           = config.AlignTRTBarrel           [iteration]
        AlignmentOptions["trtAlignEndcaps"]          = config.AlignTRTEndcaps          [iteration]
        AlignmentOptions["trtAlignmentLevel"]        = config.TRTAlignmentLevel        [iteration]
        AlignmentOptions["trtAlignmentLevelBarrel"]  = config.TRTAlignmentLevelBarrel  [iteration]
        AlignmentOptions["trtAlignmentLevelEndcaps"] = config.TRTAlignmentLevelEndcaps [iteration]
        
    
    AlignmentOptions["alignSCT"]                   = config.AlignSCT                   [iteration]
    AlignmentOptions["sctAlignmentLevel"]          = config.SCTAlignmentLevel          [iteration]
    AlignmentOptions["sctAlignBarrel"]             = config.AlignSCTBarrel             [iteration]
    AlignmentOptions["sctAlignmentLevelBarrel"]    = config.SCTAlignmentLevelBarrel    [iteration]
    AlignmentOptions["sctAlignEndcaps"]            = config.AlignSCTEndcaps            [iteration]
    AlignmentOptions["sctAlignmentLevelEndcaps"]   = config.SCTAlignmentLevelEndcaps   [iteration]
    AlignmentOptions["alignPixel"]                 = config.AlignPixel                 [iteration]
    AlignmentOptions["pixelAlignmentLevel"]        = config.PixelAlignmentLevel        [iteration]
    AlignmentOptions["pixelAlignBarrel"]           = config.AlignPixelBarrel           [iteration]
    AlignmentOptions["pixelAlignmentLevelBarrel"]  = config.PixelAlignmentLevelBarrel  [iteration]
    AlignmentOptions["pixelAlignEndcaps"]          = config.AlignPixelEndcaps          [iteration]
    AlignmentOptions["pixelAlignmentLevelEndcaps"] = config.PixelAlignmentLevelEndcaps [iteration]
        
    if part == -1 and data != None :
        
        # Get Vectors, Matricies and Hitmaps
        if not config.useTFiles:
            tdata = [data]
            matrices, vectors, hitmaps = mergeMatrix_new(config.OutputPath, config.FirstIteration, tdata)
            AlignmentOptions["inputMatrixFiles"] =  matrices
            AlignmentOptions["inputVectorFiles"] = vectors
            AlignmentOptions["inputHitmapFiles"] = hitmaps
            AlignmentOptions["WriteTFile"] = False
            if len(hitmaps) == 0:
                AlignmentOptions["readHitmaps"] = False
        else:
            tfiles = [] # set an empty list
            tdata = [data]
            tfiles = mergeTFiles_new(config.OutputPath, config.FirstIteration, config.folderSuffix, tdata) + tfiles
            AlignmentOptions["inputTFiles"] = tfiles
            AlignmentOptions["WriteTFile"] = True           
            
        AlignmentOptions["runSolving"]    = True
        AlignmentOptions["runAccumulate"] = False
        AlignmentOptions["WriteTFile"]    = config.useTFiles
        
    else:
        AlignmentOptions["runSolving"]    = False
        AlignmentOptions["runAccumulate"] = True
        AlignmentOptions["WriteTFile"]    = config.useTFiles
        
    
    return AlignmentOptions



#----------------------------------------------------------------------------------------------------
def mergeMatrix_new(OutputPath, iter, DataToRun):

    print "------------------------------------------"
    print "  Setting Matrices list" 
    print "------------------------------------------"

    matrixlist = []
    vectorlist = []
    hitmaplist = []
    for data in DataToRun:
        for j in range(0, data.getCPUs(iter)):
            workdir = "%s/Iter%d/%s/%02d/" %  (OutputPath,iter,data.getName(),j)   
            matrixlist.append("%s/%02d/matrix.bin" % (data.getName(),j))
            vectorlist.append("%s/%02d/vector.bin" % (data.getName(),j))
            
            hitmaplist.append("%s/%02d/hitmap.bin" % (data.getName(),j))

    return matrixlist,vectorlist,hitmaplist
    

#----------------------------------------------------------------------------------------------------
def mergeTFiles_new(OutputPath, iter, folderSuffix, DataToRun):

    print "------------------------------------------"
    print "  Setting TFiles list" 
    print "------------------------------------------"

    tfilelist = []
    for data in DataToRun:
        for j in range(0, data.getCPUs(iter)):
            workdir = "%s/Iter%d%s/%s/%02d/" %  (OutputPath,iter, folderSuffix, data.getName(),j)  
            tfilelist.append("%s/%02d/AlignmentTFile.root" % (data.getName(),j))

    return tfilelist
        
             
#----------------------------------------------------------------------------------------------------
def write_status_checker( config, dataName ):
    
    script_name = "check_status_{0}_{1}.sh".format( config.preName, config.folderSuffix )
    
    script=open(script_name, 'w')
    
    script.writelines( [
            "#!/bin/sh\n",
            "\n",
            "workdir={0}\n".format( config.OutputPath ),
            "preName={0}\n".format( config.preName ),
            "dataName={0}\n".format( dataName ),
            "folderSuffix={0}\n".format( config.folderSuffix ),
            "\n\n",
            "date\n",
            "started=`date -d\"\`stat ${workdir}/${preName}_Iter0_${folderSuffix}/${dataName}/00/${preName}_${dataName}_${folderSuffix}_Part00.py | grep -i -i change | cut -d: -f2-20\`\"`\n",
            "echo\n",
            "echo \"Process started ${started}\"\n",
            "echo \"\"\n",
            "echo \"[ Accumulation Status ]\"\n",
            "for iter in `seq %d %d`\n" % ( config.FirstIteration, config.FirstIteration + config.Iterations - 1 ),
            "do\n",
            "    total=`printf %4d \`ls ${workdir}/${preName}_Iter${iter}_${folderSuffix}/${dataName} | wc -l\``\n",
            "    process=`printf %4d \`ls ${workdir}/${preName}_Iter${iter}_${folderSuffix}/${dataName}/*/athena_log.txt 2>/dev/null | wc -l\``\n",
            "    finished=`printf %4d \`ls ${workdir}/${preName}_Iter${iter}_${folderSuffix}/${dataName}/*/is_finished 2>/dev/null | wc -l\``\n",
            "    \n",
            "    status=\"\"\n",
            "    if [ ${total} == ${finished} ] ; then\n",
            "        end_time=`date -d\"\`stat ${workdir}/${preName}_Iter${iter}_${folderSuffix}/${dataName}/*/is_finished | grep -i change | sort | tail -n1 | cut -d: -f2-20\`\"`\n",
            "        status=\"done at ${end_time}\"\n",
            "    elif [ ${process} == 0 ] ; then\n",
            "        status=\"not started\"\n",
            "    else\n",
            "        start_time=`date -d\"\`stat ${workdir}/${preName}_Iter${iter}_${folderSuffix}/${dataName}/*/athena_log.txt | grep -i change | sort | head -n1 | cut -d: -f2-20\`\"`\n",
            "        status=\"ongoing (started at ${start_time} )\"\n",
            "    fi\n",
            "    \n",
            "    echo \"Iter${iter} | total = ${total} / in process = ${process} / finished = ${finished} | ${status}\"\n",
            "done\n",
            "\n\n",
            "echo \"\"\n",
            "echo \"[ Solver Status ]\"\n",
            "for iter in `seq %d %d`\n" % ( config.FirstIteration, config.FirstIteration + config.Iterations - 1 ),
            "do\n",
            "    if [ -e ${workdir}/${preName}_Iter${iter}_${folderSuffix}/Iter${iter}_AlignmentConstants.root ]; then\n",
            "        end_time=`date -d\"\`stat ${workdir}/${preName}_Iter${iter}_${folderSuffix}/Iter${iter}_AlignmentConstants.root | grep -i change | cut -d: -f2-20\`\"`\n",
            "        solver_status=\"done at date ${end_time}\"\n",
            "    elif [ -e ${workdir}/${preName}_Iter${iter}_${folderSuffix}/athena_solver_log.txt ]; then\n",
            "        start_time=`date -d\"\`stat ${workdir}/${preName}_Iter${iter}_${folderSuffix}/athena_solver_log.txt | grep -i change | sort | head -n1 | cut -d: -f2-20\`\"`\n",
            "        solver_status=\"ongoing (started at ${start_time} )\"\n",
            "    else\n",
            "        solver_status=`printf %4s \"not started\"`\n",
            "    fi\n",
            "    echo \"Iter${iter} | ${solver_status}\"\n",
            "done\n",
            "\n\n",
            "echo \"\"\n",
            "echo \"[ Monitoring Merger Status ]\"\n",
            "for iter in `seq %d %d`\n" % ( config.FirstIteration, config.FirstIteration + config.Iterations - 1 ),
            "do\n",
            "  if [ -e ${workdir}/${preName}_Iter${iter}_${folderSuffix}/TotalMonitoring.root ]; then\n",
            "    end_time=`date -d\"\`stat ${workdir}/${preName}_Iter${iter}_${folderSuffix}/TotalMonitoring.root | grep -i change | cut -d: -f2-20\`\"`\n",
            "    solver_status=\"done at date ${end_time}\"\n",
            "  elif [ -e ${workdir}/${preName}_Iter${iter}_${folderSuffix}/merger_log.txt ]; then\n",
            "    end_time=`date -d\"\`stat ${workdir}/${preName}_Iter${iter}_${folderSuffix}/merger_log.txt | grep -i change | cut -d: -f2-20\`\"`\n",
            "    merger_status=\"ongoing (started at ${end_time})\"\n",
            "  else\n",
            "    merger_status=`printf %4s \"not started\"`\n",
            "  fi\n",
            "  echo \"Iter${iter} | ${merger_status}\"\n",
            "done\n",
            "\n\n",
            "user=${USER}\n",
            "verbose=0\n",
            "\n\n",
            "while [ \"$1\" != \"\" ]\n",
            "do\n",
            "    if [ \"$1\" == \"-u\" ] ; then\n",
            "        if [ \"$2\" != \"\" ]; then user=$2; fi\n",
            "        shift\n",
            "    elif [ \"$1\" == \"-v\" ] ; then\n",
            "        verbose=1\n",
            "    fi\n",
            "    shift\n",
            "done\n",
            "\n\n",
            "echo \"\"\n",
            "string=`bjobs -w -u ${user} 2>/dev/null | grep \"${preName}\" | grep \"${folderSuffix}\" | grep -v JOBID`\n",
            "\n\n",
            "if [ $verbose == 1 ] ; then\n",
            "    echo \"${string}\"\n",
            "    echo \"\"\n",
            "fi\n",
            "\n\n",
            "run=`   echo \"${string}\" | grep RUN      | wc -l`\n",
            "pend=`  echo \"${string}\" | grep PEND     | wc -l`\n",
            "ususp=` echo \"${string}\" | grep USUSP    | wc -l`\n",
            "unkwn=` echo \"${string}\" | grep UNKWN    | wc -l`\n",
            "\n",
            "if [ \"${string}\" == \"\" ]; then\n",
            "    tot=\"0\"\n",
            "else\n",
            "    tot=`   echo \"${string}\" | grep -v JOBID | wc -l`\n",
            "fi\n",
            "\n\n",
            "echo \"[LSF Job Stats for this RunIterator]\"\n",
            "echo \"RUN:   $run\"\n",
            "echo \"USUSP: $ususp\"\n",
            "echo \"PEND:  $pend\"\n",
            "echo \"UNKWN: $unkwn\"\n",
            "echo \"TOTAL: $tot\"\n",
            "echo \"\"\n",
            "echo \"[Count by queue]\"\n",
            "queues=`echo \"${string}\" | awk '{print $4;}' | sort | uniq`\n",
            "for queue in `echo $queues`\n",
            "do\n",
            "    echo ${queue}: `echo \"${string}\" | grep ${queue} | wc -l`\n",
            "done\n",
            "\n\n",
            "echo \"\"\n" ] )
    
    script.close()
    
    import os
    os.system("chmod +x %s" % script_name)
