# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from os.path import join
import CMTtools

# -------------------------------------------------------------------------

class Commander:    

    """
    Commander - a base class that issues commands to the operating system.
    Derived classe overide the commands to allow for differences due to
    different mode values.
    
    Commands:  athena
"""
    def __init__(self):
        pass
    
    def workNodes(self):
        return ['lxb7513',
                'lxb7516',
                'lxb7517',
                'lxbrb2201',
                'lxbrb2202',
                'lxbrb2203',
                'lxbrb2204',
                'lxbrb2205',
                'lxbrb2206',
                'lxbrb2207',
                'lxbrb2208',
                'lxbrb2209',
                'lxbrb2210',
                'lxbrb2211',
                'lxbrb2212',
                'lxbrb2213',
                'lxbrb2214',
                'lxb7511',
                'lxb7512',
                'lxb7514',
                'lxb7515',
                'lxb7518',
                'lxb7519',
                'lxb7534',
                'lxb7535',
                'lxb7536',
                'lxb7537',
                'lxb7538',
                'lxb7539',
                'lxb8761',
                'lxb8762',
                'lxb8763',
                'lxb8764',
                'lxb8765',
                'lxb8766',
                'lxb8768',
                'lxb8769',
                'lxb8770',
                'lxb8773',
                'lxb8774',
                'lxb8775',
                'lxb8848',
                'lxb9019',
                'lxbra2707',
                'lxbra2708',
                'lxbrg3809',
                'lxbrg4004',
                'lxbrg4005',
                'lxbrg4010',
                'lxbrg4201',
                'lxbrg4204',
                'lxbrg4205',
                'lxbrg4206',
                'lxbrg4209',
                'lxbrg4210',
                'lxbrg4401',
                'lxbrg4406',
                'lxbrg4407',
                'lxb7520',
                'lxb7526',
                'lxb7527',
                'lxb7533',
                'lxb7566',
                'lxb8767',
                'lxb8771',
                'lxb8772',
                'lxbra2709',
                'lxbrg4001',
                'lxbrg4404',
                'lxbrg4405']


class LinuxInteractiveCommander(Commander):
    """ Commands for running interactively (foreground) on Linux"""

    def __init__(self):
        Commander.__init__(self)
    
    def submitCommand(self, runPath, log):        
        script  = join(runPath, "runScript.sh")
        logName = join(runPath, log)

        #!!! do not put in the background!!! joblog is present before job ends
        return "rm %s; nice %s>& %s" % (logName, script, logName)
    
    def postProcessorCommand(self, runPath):
        script   = join(runPath, 'postProcessorScript.sh')
        logName  = join(runPath, 'postProcessor.log')

        cmd =  "rm %s; source %s>& %s" % (logName, script, logName)

        return cmd
        
    

class BatchCommander(Commander):
    def __init__(self):
        Commander.__init__(self)

    def submitCommand(self, runPath, identifiedName, log, elog, queue, walltimelimit=''):
        log    = join(runPath, log)
        elog   = join(runPath, elog)
        script = join(runPath, "runScript.sh")

        return self.command(script, identifiedName, log, elog, queue, walltimelimit)

    def postProcessorCommand(self, runPath, identifiedName, queue):
        log    = join(runPath, 'postProcessor.log')
        elog   = join(runPath, 'postProcessor.elog')
        script = join(runPath, "postProcessorScript.sh")

        return self.command(script, identifiedName, log, elog, queue)

class LSFBatchCommander(BatchCommander):
    """ Commands for running jobs on the CERN LSF queues"""
    
    def __init__(self):
        BatchCommander.__init__(self)

    def command(self, script, identifiedName, log, elog, queue, walltimelimit=''):
        if walltimelimit:
            try:
                int(walltimelimit)
                walltimelimit='-W %s' % walltimelimit
            except:
                walltimelimit=''
        nodeSelectionOption = '-m "' + reduce(lambda x,y:x+' '+y, self.workNodes()) + '"'
        cmd = 'LSB_STDOUT_DIRECT=Y;/usr/bin/bsub %s -o %s %s -q %s -R "atlasrtt" %s -J "%s"' % (nodeSelectionOption, 
                                                                                                log,
                                                                                                walltimelimit,
                                                                                                queue,
                                                                                                script,
                                                                                                identifiedName)
        return cmd


            
class DummyCommander(Commander):

    def __init__(self):
        Commander.__init__(self)
    
    def command(self, identifiedName):
        return  'print Dummy submit'+ identifiedName

    def submitCommand(self, identifiedName): return self.command(identifiedName)

    def postProcessorCommand(self, identifiedName):  return self.command(identifiedName)



