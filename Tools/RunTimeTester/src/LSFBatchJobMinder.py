# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from exc2string2     import exc2string2
from BatchJobMinder  import BatchJobMinder
from ShellCommand    import ShellCommand
from Querier         import LSF_bjobs1, LSF_bjobs2, LSF_bjobs3, LSF_bhist

import os
# -------------------------------------------------------------------------
from MethodTracer        import MetaTracer
# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer
            
class LSFBatchJobMinder(BatchJobMinder):
    
    def __init__ (self, argBag, jDescriptor):


        self.setQueue(jDescriptor.suggestedQueue)

        commander = argBag.commander

        argBag.submitCommand      = commander.submitCommand(argBag.runPath,
                                                            argBag.identifiedName,
                                                            argBag.log,
                                                            argBag.elog,
                                                            self.queue,
                                                            jDescriptor.suggestedBatchWallTime)
        argBag.postProcessCommand = commander.postProcessorCommand(argBag.runPath,
                                                                   argBag.identifiedName,
                                                                   self.queue
                                                                   )


        BatchJobMinder.__init__(self, argBag, jDescriptor)
        # total time from bjobs, spec int time from log file of samplejob
        self.spec2000Conv = 712.0/265.1 
        self.logger.debug('initialised LSFBatchJobMinder %s' % self.identifiedName)    
        self.postProcessingQueue = 'short'

    # ------------------------------------------------------------------------

    def setQueue(self, suggestedQueue):
        self.queue = {'short':  'atlasrttshort',
                      'medium': 'atlasrttmedium',
                      'long':   'atlasrttlong'}.get(suggestedQueue, 'atlasrttshort')

    # ------------------------------------------------------------------------

    def stopJob(self):
        if self.done: return
        
        try:
            cmd = ['bkill ' + str(self.pid)]
            sc       = ShellCommand(cmd, self.logger, 100, None, True)
            self.logger.debug(sc.getReply())
        except Exception, e:
            m  = 'Job: ' + str(self.identifiedName)
            m += ' threw an exception while issuing cmd: %s' % cmd[0] 
            self.logger.warning(m)


    # -------------------------------------------------------------------------

    def checkQueue(self):
        #check if queue exists

        querier = LSF_bjobs1(self.logger, self.queue)
        goodQueue, output = querier.statusAndReply()

        if not goodQueue:
            msg  = 'Bad queue: %s' % (output)
            self.logger.error(msg)
            return False

        return True

    # -------------------------------------------------------------------------
    
    def checkJobSubmissionWasOK(self):
        'Called by the StateEngine'

        for l in self.subLog:
            if l.find('is submitted to') == -1:
                self.logger.error('Batch machine refused job submission!')
                return False

        #see if we have a reasonable jobs PID
        try:
            pid = int(self.pid)
        except:
            return False
        
        return True
            
    # -------------------------------------------------------------------------
    
    def batchQueryReplyFormatOK(self, reply):
        if not reply: return False
        for thing in reply:
            if thing.find("batch system daemon not responding")!=-1: return False
        return True
    
    # -------------------------------------------------------------------------
    
    def batchReplyProvesJobIsInQueue(self, reply):
        # batch queue reply to query for a running job is a header line
        # followed by the status of each pid of the query
        if len(reply)<2:
            return False

        tokens = reply[1].split()
        if len(tokens)<3: return False
        tokens = [t.strip() for t in tokens]
        
        self.logger.debug('tokens %s' % str(tokens))
        if not tokens[0].startswith(str(self.pid)): return False
        if tokens[2].startswith('RUN'): return True
        if tokens[2].startswith('PEND'): return True
        self.logger.debug('Job is NOT in queue')
        return False # any other lsf state (eg DONE) - not in queue.

    # -------------------------------------------------------------------------
    
    def batchReplyProvesJobIsAbsentFromQueue(self, reply):
        """if the original bjobs command returned a 1 line reply
        it might be the case that the job is part of the history.
        check the history to see if the job is known.
        if so, assume the job ran."""
        
        if not reply: return False
        if len(reply) == 1:
            sc = ShellCommand(['bhist %s' % str(self.pid)], self.logger)
            if len(sc.getReply())>1:
                return True
            else:
                return False
            
        return not self.batchReplyProvesJobIsInQueue(reply)

    # -------------------------------------------------------------------------
    
    def batchQuery(self):
        return 'bjobs ' + str(self.pid)

    # -------------------------------------------------------------------------

    def batchQueryCommand(self): return 'bjobs'

    # -------------------------------------------------------------------------

    def audit(self):
        """
        ask LSF for audit information. The availablility if the audit information varies.
        If the job is running, cputime, mem, vmem, walltime and job status are available

        For jobs wich have finished, but which do not have state DONE (eg EXIT) there was an error
        and cputime and memory used are not available

        For finished jobs in state DONE (success), memory numbers are not available. If memory numbers
        were found durng the run, the highest value is recorded.
        """

        dict = {}

        def findValues0(output, dict):
            # self.logger.error('audit 10 '+output)
            sstring = 'The CPU time used is'            
            start = output.find(sstring)
            if start < 0: return
            output = output[start:]
            tokens = output.split()

            dict['cpuTime'] = int(round(float(tokens[5])))
            if 'MEM:' in tokens:  dict['mem']     = tokens[8]
            if 'SWAP:' in tokens: dict['vmem']    = tokens[11]

        # status, output = commands.getstatusoutput('bjobs -l %s' % self.pid)
        # if status:
        #    self.logger.warning('bad status on command bjobs -l %s' % status)

        querier = LSF_bjobs2(self.logger, self.pid)
        status, reply = querier.statusAndReply()

        if not status:
            self.logger.warning('bad reply on command bjobs -l %s' % reply)
        else:
            self.logger.debug('good reply on command bjobs -l %s' % reply)
            findValues0(reply, dict)


        querier = LSF_bjobs3(self.logger, self.pid)
        status, reply = querier.statusAndReply()
        
        if not status:
            self.logger.warning('bad reply on command bjobs -a %s' % reply)
            dict['batchStatus'] = 'unknown'
        else:
            dict['batchStatus'] = reply
                
        def findValues2(output, dict):
            try:
                tokens = output.split('\n')
                tokens = tokens[2].split()
            except:
                m  = 'audit.findValues1 threw exception. Strange batch reply was: %s\n' % str(output)
                m += 'Unexpected batch system reply result is being ignored. TraceBack:\n'
                m += exc2string2()
                self.logger.error(m)
                dict['wallTime'] = 'Unobtainable'
            else:   
                dict['wallTime'] = (tokens[-1]).strip()

        # status, output = commands.getstatusoutput('bhist %s' % self.pid)
        querier = LSF_bhist(self.logger, self.pid)
        status, reply = querier.statusAndReply()
        if not status:
            self.logger.warning('bad reply on  "bhist": %s' % reply)
        else:
            findValues2(reply, dict)

        def acceptIfGreater(nstring, ostring):
            try:
                nf = float(nstring)
            except:
                return None

            try:
                of = float(ostring)
            except:
                return nf

            if nf>of: return nf
            return of

        mem = acceptIfGreater(dict.get('mem', None), self.mem)
        if mem != None: dict['mem'] = str(int(round(mem))) + 'MB'

        mem = acceptIfGreater(dict.get('vmem', None), self.vmem)
        if mem != None: dict['vmem'] = str(int(round(mem))) + 'MB'

        try:
            cpuTime = float(dict['cpuTime'])
        except:
            dict['cpuTime2000'] = 'Unknown'
        else:
            dict['cpuTime2000'] = int(round(self.spec2000Conv*cpuTime))

        self.__dict__.update(dict)
        return

    # -------------------------------------------------------------------------

    def setPid(self):
        self.logger.debug('Calculating job pid from subLog %s' % str(self.subLog))
        try:
            self.pid = ((self.subLog[0].split())[1].strip())[1:-1] # submit cmd prints id to stdout
        except:
            self.logger.error('Could not evaluate job pid from %s' % str(self.subLog))
        else:
            self.logger.debug('Job pid: %s' % str(self.pid))


