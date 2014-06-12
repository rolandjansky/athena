#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger executor to call base transforms
# @details: Based on athenaExecutor with some modifications
# @author: Mark Stockton

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfUtils import asetupReport

import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigTransform.trigUpdateArgs import swapArgs
     
class trigExecutor(athenaExecutor):
    # Trigger executor
    # different from standard athena as runs athenaMT/PT  from the command line rather than via JOs
    def __init__(self, name = 'athena', trf = None, inData = set(), outData = set(), exe = 'athena.py', exeArgs = ['athenaopts']):
        #don't need any of the JO file creation so set to None every time
        skeletonFile=None
        #remainder of init comes from parent
        super(trigExecutor, self).__init__(name=name, trf=trf, inData=inData, outData=outData, skeletonFile=skeletonFile, exe=exe, exeArgs=exeArgs)
        
    def preExecute(self, input = set(), output = set()):
        msg.info('Run athena preExecute, note will recreate command line from scratch after this is complete')
        super(trigExecutor, self).preExecute(input=input, output=output)
             
        msg.info('athena preExecute complete, will now create command line from scratch')
        
        #Swap the remaining arguments into the athenaMT/PT variable names
        swapArgs(self.conf.argdict)
        
        ##TODO expect arg order important i.e. want exeArgs at end
        #      so code restarts command from scratch (below) then in loop it skips exeArgs
        self._cmd = [self.exe, ]
        
        ## Code below is based on that from scriptExecutor in trfExe
        ## Build up the appropriate list for execution (need something rather more general...)
        for arg in self.conf.argdict:
            #skip exeArgs
            if arg in self.exeArgs:
                continue
             
            #skip any runargs (code from trfJobOptions)
            _isRunTimeArg=False
            if isinstance(self.conf.argdict[arg], trfArgClasses.argument): 
                if self.conf.argdict[arg].isRunarg == True:
                    _isRunTimeArg=True
            
            if _isRunTimeArg == False:
                msg.debug('Argument %s is not a runarg so skipped' % arg)
                continue

            # Add the arg and its value to the command
            # all args are given the -- option infront of their name
            # TODO currently passes a list object, need to check this is read correctly
            elif isinstance(self.conf.argdict[arg].value, list):
                self._cmd.append(str('--' + arg))
                #TODO if list object will be fine...
                self._cmd.append(str(self.conf.argdict[arg].value))
                #if not use below to create a single string
                #temp = ''
                #for v in self.conf.argdict[arg].value:
                #    temp+=v+' '
                #self._cmd.append(temp)
            #for substep arguments need to get the specific value for the step
            elif isinstance(self.conf.argdict[arg], trfArgClasses.argSubstep):
                self._cmd.append(str('--' + arg))
                #TODO for now it is set to first substep, will need to get the step currently in
                #For info this was added to fix the maxEvents which has a default of first
                self._cmd.append(str(self.conf.argdict[arg].returnMyValue(name=self._name, substep=self._substep, first=True)))
            else:
                self._cmd.append(str('--' + arg))
                self._cmd.append(str(self.conf.argdict[arg].value))

        #now add exeArgs to end of command
        for arg in self.exeArgs:
            if arg in self.conf.argdict:
                # If we have a list then add each element to our list, else just str() the argument value
                # Note if there are arguments which need more complex transformations then
                # consider introducing a special toExeArg() method.
                if isinstance(self.conf.argdict[arg].value, list):
                    self._cmd.extend([ str(v) for v in self.conf.argdict[arg].value])
                else:
                    self._cmd.append(str(self.conf.argdict[arg].value))
        
        msg.info('Updated script arguments: %s' % self._cmd)
        #TODO output in human readable form, using something like:
        
        #from graeme
        #' '.join([str(item) for item in argdict])
        #add if type list...
        
        #testing in python standalone
        #test=['athenaMT.py', '--postcommand', 'L2_PROCESS_postCommands.py', '--run-number', '202798', '--event-modifier', 'TrigTransform.PreloadL2', '--use-database', 'DBServer=TRIGGERDBATN:DBSMKey=1111:DBHLTPSKey=3333:DBLV1PSKey=2222', '--number-of-events', '10', '--file', "['test3', 'test6']", '--joboptionsvc-type', 'TrigConf::HLTJobOptionsSvc', '--save-output', "['test4']", '-f', 'test1', '-D']
        #' '.join(test)
        #for item in test:
        #    print item
        #    print type(item)
        
        test1=' '.join(self._cmd)
        #test1=' '.join([str(item) for item in self._cmd])
        msg.info('Mark test joining: %s' % (test1))
        #for testitem in self._cmd:
        #    #test2=' '.join(item)
        #    ##test2=' '.join([str(item) for item in testitem])
        #    #msg.info('Mark test joining: %s %s %s' % (testitem, type(testitem),test2))
        #    msg.info('Mark test joining: %s %s' % (testitem, type(testitem)))
                