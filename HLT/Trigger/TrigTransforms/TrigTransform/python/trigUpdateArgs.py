#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Argument altering for trigger transform
# @details: Code to convert trf args into the athenaMT/PT variable names
# @author: Mark Stockton

####
#Notes:
#     
#Input arguments need to be converted into the athenaMT/PT variable names
#
#However some are needed by the base transform and so the switch has to be done after preExecute
#this applies to the input and output argument names at least        
#
#Always use long names for the options to be used for athenaMT/PT
#due to how the athenaTExecutor is set up

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses

def trigUpdateArgs(inArgDict,parser):
     #Calls all functions defined below to update the arguments
     #EXCEPT for the swapArgs which is called from trigExe:preExecute
          
     msg.info('Updating input arguments into athenaMT/PT arguments')
     
     #edit the database options
     configDatabase(inArgDict,parser)
     
     #if have any post commands create the appropriate JO file
     if 'post_commands' in inArgDict:
        postCommand(inArgDict)

     msg.info('Finished updating input arguments into athenaMT/PT arguments')
     
def outputArgs(inArgDict,description):
     #debugging function useful for logging the status of args
     for arg in inArgDict:
       msg.debug('Checking args (%s): %s' % (description,arg))
        
def swapArgs(inArgDict):
     #swap any arguments that just change the key name

     msg.info('Swapping input argument names')
     outputArgs(inArgDict,'before swapArgs')
     
     #Create dictionary of arguments that are to be swapped
     
     swapDict = {}
     swapDict['inputBSFile']   = 'file'#can't swap until after athenaExectutor:preExecute
     swapDict['outputRAWFile'] = 'save-output'
     swapDict['outputBS_MTFile'] = 'save-output'#can't swap until after athenaExectutor:preExecute
     swapDict['run_number']    = 'run-number'
     swapDict['maxEvents']     = 'number-of-events'#TODO need to check not already set
     
     ##swapDict['version']   = ''
     #swapDict['doCTP']   = ''
     #swapDict['doMUCTPI']   = ''
     #swapDict['doCopyIn']   = ''
     #swapDict['doStreaming']   = ''
     #swapDict['doDummyStreaming']   = ''
     #swapDict['doRecording']   = ''
     #swapDict['doL2']   = ''
     #swapDict['doNotTruncateL2R']   = ''
     #swapDict['doEF']   = ''
     #swapDict['doPre']   = ''
     #swapDict['doPost']   = ''
     #swapDict['doHLTDebugWrite']   = ''
     ##swapDict['file_stream']   = ''
     #swapDict['output_dir']   = ''
     #swapDict['crash_dir']   = ''
     #swapDict['histo_dir']   = ''
     #swapDict['service_class']   = ''
     #swapDict['file_prefix']   = ''
     #swapDict['job_id']   = ''
     #swapDict['jobOptions']   = ''
     #swapDict['options']   = ''
     #swapDict['conditions']   = ''
     #swapDict['filters']   = ''
     #swapDict['stream_out']   = ''
     #swapDict['proc_session']   = ''
     #swapDict['doLVL1Remapping']   = ''
     #swapDict['enLVL1prescales']   = ''
     #swapDict['verbose']   = ''
        
     #swap simple item names
     tempDict = {}
     while inArgDict:
         key, value = inArgDict.popitem()
         if key in swapDict:
             tempkey=key
             key=swapDict.get(key)
             msg.debug('Swapping keys: %s to %s' % (tempkey,key))
         tempDict[key]=value

     #save back into inArgDict
     while tempDict:
         key, value = tempDict.popitem()
         inArgDict[key]=value

     outputArgs(inArgDict,'after swapArgs')
     msg.info('Finished swapping input argument names')
       
     
def configDatabase(inArgDict,parser):
    
     msg.info('Setting up the databse options')
     
     # Uses: connection, smk, hltps, lvl1ps
     # Sets: joboptionsvc-type, use-database, event-modifier

     # code below is to reproduce the simple transform output
     # needs to be altered later to match that from current transforms 
     if 'smk' in inArgDict and 'hltps' in inArgDict and 'lvl1ps' in inArgDict and 'connection' in inArgDict:

        #arguments remain in dict but wont be used in final command
        connection = inArgDict.get('connection')
        smk = inArgDict.get('smk')
        hltps = inArgDict.get('hltps')
        lvl1ps = inArgDict.get('lvl1ps')
        
        #Create the new arguments to be used based on the input above
        inArgDict['joboptionsvc-type']=trfArgClasses.argString('TrigConf::HLTJobOptionsSvc', runarg=True)
        temp='DBServer=%s:DBSMKey=%s:DBHLTPSKey=%s:DBLV1PSKey=%s' % (connection.value, smk.value, hltps.value, lvl1ps.value)
        inArgDict['use-database']=trfArgClasses.argString(temp, runarg=True)

        #TODO will need to check that when compiled this is picked up (may need to rename folder)
        #TODO will need to know if doing L2 or EF
        #TODO note this was always added in old transforms
        inArgDict['event-modifier']= trfArgClasses.argString('TrigTransform.PreloadL2', runarg=True)
        
        #Out put what keys are swapped
        msg.debug('Replacing keys: connection,smk,hltps,lvl1ps with joboptionsvc-type,use-database,event-modifier')
     
     #code below from current transforms not yet in use
     #TODO need to get strings from the input, eg smk isnt defined so needs to be inArgDict['smk']

     #config = {}
     #if 'jobOptions' in inArgDict:
     #   config = { 'type':'JOBOPTIONS',         \
     #              'joboptions':joboptions,     \
     #              'options'  :options,         \
     #              'post_commands':'',          \
     #              'conditions':conditions,     \
     #              'run_number':str(run_number),\
     #             }
     #   msg.info("  trigger configuration from JobOptions %s, with options %s"%(joboptions,options))
     #elif 'smk' in inArgDict and 'hltps' in inArgDict and 'lvl1ps' in inArgDict and 'connection' in inArgDict:
     #   config = { 'type':'DB',                  \
     #              'pairs':{                     \
     #                     'DBSMKey'   :inArgDict['smk'],      \
     #                     'DBHLTPSKey':inArgDict['hltps'],    \
     #                     'DBL1PSKey' :inArgDict['lvl1ps'],   \
     #                     },                     \
     #              'post_commands':post_commands,\
     #              'connection':connection,      \
     #              'conditions':conditions,      \
     #              'run_number':str(run_number), \
     #             }
     #   msg.info("  trigger configuration from %s, smk %s, hltps %s, lvl1ps %s"%(connection,smk,hltps,lvl1ps))
     #TODO add extra here and give proper exit
     #elif 'run_number' in inArgDict and 'connection' in inArgDict:
     #   trigconf = TrigConf.get(run_number)
     #   try:
     #       smkey  = trigconf.SMK()
     #       hltkey = trigconf.HLT_PS(lumi_block)
     #       l1key  = trigconf.LVL1_PS(lumi_block)
     #       msg.info("  FULL PS configuration %s" % trigconf.FULL_HLT_PS())
     #       if trigconf.FULL_HLT_PS(): 
     #           hltkey = trigconf.FULL_HLT_PS()
     #   except:
     #       msg.error("BatchHLTApps: cannot get the BD configuration for run %d" % run_number)
     #       msg.error(tigconf.str())
     #       sys.exit(1)
     #           
     #   config = { 'type':'DB',                  \
     #              'pairs':{                     \
     #                     'DBSMKey'   :smkey,    \
     #                     'DBHLTPSKey':hltkey,   \
     #                     'DBL1PSKey' :l1key,    \
     #                     },                     \
     #              'post_commands':post_commands,\
     #              'connection':connection,      \
     #              'conditions':conditions,      \
     #              'run_number':str(run_number), \
     #             }
     #   msg.info("  trigger configuration from %s, smk %s, hltps %s, lvl1ps %s"%(connection,smkey,hltkey,l1key))
     #else:
     #   Step().cancel("bad configuration for L2/EF")


     #TODO: currently disabled
       #need to check which names need saving
       #need to make sure that new args are listed as runTime args
     #save back into inArgDict
     #while config:
     #    key, value = config.popitem()
     #    inArgDict[key]=value
     
     msg.info('Finished setting up the database options for athenaMT/PT')
       
def postCommand(inArgDict):
    
     msg.info('Creating the PostCommands JO file')     
     
     # Uses: post_commands
     # Sets: postcommand
     # Makes: L2_PROCESS_postCommands.py
     
     #TODO need to pass process name correctly
     #post_command_name = '%s_postCommands.py' % self.name
     post_command_name = 'L2_PROCESS_postCommands.py'
     postCommands = file(post_command_name,"w")
                
                
     conUp_text = ['from GaudiPython.Bindings import iProperty\n','\n',]
                
     #TODO instead of writing the file should have it in the code already so can find on alxr easier
     #simon suggested pluggins on 30Aug   
     #--postcommand|-C            Optional python commands executed after jobOption
     #                        s script or database configuration (defaults to [
     #                        ])        
     
     #TODO can the file be put into output log?? maybe then doesnt need to be in alxr
                
     #TODO need to work out what to get from conditions in terms of my argDict
     #for c in self.config['conditions']:
     #        cond = c.split(',')
     #        string = "\"\'<prefix>%s</prefix> <tag>%s</tag>\'\"" % (cond[0],cond[1])
     #        conUp_text += ['iProperty(\'IOVDbSvc\').overrideTags.push_back(%s)\n'%string]
                
     dgbMU_text = ['from GaudiPython.Bindings import iProperty\n',
                   '\n',
                   'iProperty(\'ByteStreamAddressProviderSvc\').OutputLevel=DEBUG\n',
                   '\n',
                   'iProperty(\'ToolSvc.MuCTPIByteStreamTool\').OutputLevel=VERBOSE\n',
                   'if len(iProperty(\'TrigSteer_L2.Lvl1Converter\').properties())>0:\n',
                   '    iProperty(\'muFast_900GeV\').OutputLevel   = DEBUG\n',
                   '    iProperty(\'muFast_900GeV\').MUlvl1INFO    = True\n',
                   '    iProperty(\'muFast_900GeV\').MUtrackINFO   = True\n',    
                   '    iProperty(\'muFast_900GeV\').MUroadsINFO   = True\n',
                   '    iProperty(\'muFast_900GeV\').MUdecoINFO    = True\n',
                   '    iProperty(\'muFast_900GeV\').MUcontINFO    = True\n',
                   '    iProperty(\'muFast_900GeV\').MUfitINFO     = True\n',
                   '    iProperty(\'muFast_900GeV\').MUsagINFO     = True\n',
                   '    iProperty(\'muFast_900GeV\').MUptINFO      = True\n', 
                   '    iProperty(\'muFast_900GeV\').MUtgcDecoINFO = True\n',
                   '\n',
                   '    iProperty(\'muFast_Muon\').OutputLevel   = DEBUG\n',
                   '    iProperty(\'muFast_Muon\').MUlvl1INFO    = True\n',
                   '    iProperty(\'muFast_Muon\').MUtrackINFO   = True\n',    
                   '    iProperty(\'muFast_Muon\').MUroadsINFO   = True\n',
                   '    iProperty(\'muFast_Muon\').MUdecoINFO    = True\n',
                   '    iProperty(\'muFast_Muon\').MUcontINFO    = True\n',
                   '    iProperty(\'muFast_Muon\').MUfitINFO     = True\n',
                   '    iProperty(\'muFast_Muon\').MUsagINFO     = True\n',
                   '    iProperty(\'muFast_Muon\').MUptINFO      = True\n', 
                   '    iProperty(\'muFast_Muon\').MUtgcDecoINFO = True\n',
                   '\n',
                   '    iProperty(\'Lvl2EventLoopMgr\').OutputLevel = DEBUG\n',
                   '\n',
                   #'    iProperty(\'ByteStreamAddressProviderSvc\').MuCTPIModuleID=0x0\n',
                   '    iProperty(\'ServiceMgr.RoIBResultByteStreamTool\').OutputLevel=VERBOSE\n',
                   '\n',
                   '    iProperty(\'ServiceMgr.PtLUTSvc\').OutputLevel=DEBUG\n'
                  ]
                
     magDB_text = ['from GaudiPython.Bindings import iProperty\n',
                   '\n',
                   '# Add the DCS folder (note the double-use of quotes)\n',
                   'iProperty(\'IOVDbSvc\').Folders.push_back(\"\'<db>COOLOFL_DCS/COMP200</db> /EXT/DCS/MAGNETS/SENSORDATA\'\")\n',
                   'iProperty(\'MagFieldAthenaSvc\').UseDCS = True\n',
                   'iProperty(\'MagFieldAthenaSvc\').NameOfTheSource = \'COOL\'\n\n',
                  ]

     ignPR_text = ['from GaudiPython.Bindings import iProperty\n',
                   '\n',
                   '# This will work for L2\n',
                   'if len(iProperty(\'TrigSteer_L2.Lvl1Converter\').properties())>0:\n',
                   '    iProperty(\'TrigSteer_L2.Lvl1Converter\').ignorePrescales = True\n',
                   '\n',  
                   '# and this for EF\n',
                   'if len(iProperty(\'TrigSteer_EF.Lvl2Converter\').properties())>0:\n',
                   '    iProperty(\'TrigSteer_EF.Lvl2Converter\').ignorePrescales = True\n\n',   
                  ]

     noPs_check = ['from GaudiPython.Bindings import iProperty\n',
                   '\n',
                   '# This will work for L2\n',
                   '#if len(iProperty(\'TrigSteer_L2.Lvl1Converter\').properties())>0:\n',
                   '#    iProperty(\'TrigSteer_L2.Lvl1Converter\').checkConfig = False\n',
                   '\n',
                   '# and this for EF\n',
                   'if len(iProperty(\'TrigSteer_EF.Lvl2Converter\').properties())>0:\n',
                   '    iProperty(\'TrigSteer_EF.Lvl2Converter\').checkConfig = False\n\n',
                  ]

     hardT_text = ['from GaudiPython.Bindings import iProperty\n',
                   '\n',
                   '# This will work for L2\n',
                   'if len(iProperty(\'TrigSteer_L2\').properties())>0:\n',
                   '    iProperty(\'TrigSteer_L2\').hardEventTimeout = 1800000000000\n',
                   '\n',  
                   '# and this for EF\n',
                   'if len(iProperty(\'TrigSteer_EF\').properties())>0:\n',
                   #'    iProperty(\'TrigSteer_EF\').hardEventTimeout = 1800000000000\n\n',   
                   '    iProperty(\'TrigSteer_EF\').hardEventTimeout = 1800000000000\n\n',
                  ]
                
     costM_text = ['from GaudiPython.Bindings import iProperty\n',
                   '\n',
                   'if \'doOperationalInfo\' in iProperty(\'TrigSteer_L2\').properties():\n',
                   '    iProperty(\'TrigSteer_L2\').doOperationalInfo=1\n',
                   '\n',
                   'if \'writeAlways\' in iProperty(\'TrigSteer_L2.TrigCostExecL2\').properties():\n',
                   '    iProperty(\'TrigSteer_L2.TrigCostExecL2\').writeAlways=True',
                   '\n',  
                   'if \'doOperationalInfo\' in iProperty(\'TrigSteer_EF\').properties():\n',
                   '    iProperty(\'TrigSteer_EF\').doOperationalInfo=1\n',
                   '\n',
                   'if \'writeAlways\' in iProperty(\'TrigSteer_EF.TrigCostExecEF\').properties():\n',
                   '    iProperty(\'TrigSteer_EF.TrigCostExecEF\').writeAlways=True\n',
                  ]
     
     Hndlr_text = ['from GaudiPython.Bindings import iProperty\n',
                              '# Change core dump handler to avoid gdb hanging when stdout is redirected, see bug 87861\n',
                              'iProperty(\'CoreDumpSvc\').FatalHandler = 438\n\n',
                  ]
                
     file_text = []

     #if doMagFromDCS:      file_text.extend(magDB_text)
     #if doIgnorePrescales: file_text.extend(ignPR_text)
                
     conf = str(inArgDict['post_commands'])
             
     #TODO need to work out what to get from conditions in terms of my argDict
     #if len(inArgDict['conditions']):    file_text.extend(conUp_text)
     if conf.find('ignorePrescales') !=-1: file_text.extend(ignPR_text)
     if conf.find('ignoreOnlineKeys')!=-1: file_text.extend(noPs_check)
     if conf.find('confMagFromDCS')  !=-1: file_text.extend(magDB_text)
     if conf.find('setHardTimeout')  !=-1: file_text.extend(hardT_text)
     if conf.find('debugMuon')       !=-1: file_text.extend(dgbMU_text)
     if conf.find('costMonitor')     !=-1: file_text.extend(costM_text)
     if conf.find('CoreHandler')     !=-1: file_text.extend(Hndlr_text)

     print >> postCommands, "".join(file_text)
     postCommands.close()
         
     #swap item name now finished using input values
     value = inArgDict.pop('post_commands')
     key='postcommand'
     value.value=post_command_name
     msg.debug('Swapping keys: post_commands to %s' % (key))
     inArgDict[key]=value
     
     msg.info('Finished creating the PostCommands JO file')    
     
  