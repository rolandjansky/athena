# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time, os
from formatCollection  import formatCollection
from popen2            import popen4, Popen4
from exc2string2       import exc2string2
from string            import replace
from RTTSException     import RTTTimeOut
# -------------------------------------------------------------------------

class ShellCommand:
    def __init__(self, lines, logger, nLinesToReport=100, timeout=None, silent=False):


        self.silent = silent
        self.logger = logger
        assert(nLinesToReport >=-1) # -1 - do not limit the number of lines
        lines = [str(line.strip())+';' for line in lines]
        
        lastLine = lines[-1]
        lastLine = lastLine[:-1]

        self.nLinesToReport = nLinesToReport
        try:
            self.timeout = int(timeout)
        except:
            self.timeout = None
        
        cmd = reduce(lambda x, y: x+y, lines)
        cmd = cmd.replace(';;',';')
        self.cmd = cmd[:-1]
    
        if not self.silent: self.logger.debug('issuing command '+str(self.cmd))

        self.reply = []
        
        try:
            self.launch()
        except RTTTimeOut:
            msg = 'Timeout  %s exceeded, aborting command %s' % (str(self.timeout), str(self.cmd))
            self.reply = [msg]
        except:
            msg = 'Exception on cmd %s: \nTraceback: %s' % (str(self.cmd), exc2string2())
            self.reply = [msg]
        
    def launch(self):
        self.started = time.time()
        po = Popen4(self.cmd)
        self.pid = po.pid

        output = []
        while po.poll() == -1:
            time.sleep(3)
            output.extend(po.fromchild.readlines())

            if self.hasTimedOut():
                msg  = 'Time up for ShellCommand process with pid: ' + str(self.pid) + '. It will now be killed.'
                msg += 'The commands that were launched by this process were: \n' + str(self.cmd)
                self.logger.debug(msg)
                self.suicide()
                del po
                raise RTTTimeOut(msg)

        del po
        self.reply = output

    def hasTimedOut(self):
        if self.timeout:
            return (time.time() - self.started) > self.timeout
        return False

    def getRawReply(self):
        return self.reply
        
    def getReply(self):    
        # remove the EOL if present
        reply  = [replace(line, '\n', '' ) for line in self.reply]
        lenReply = len(reply)
        
        if not self.silent and self.nLinesToReport>-1:
            if lenReply < self.nLinesToReport:
                m = 'Command: %s Reply:\n' % (str(self.cmd))
                self.logger.debug(m)
                [self.logger.debug('%s' % str(l)) for l in reply]
            else:
                msg  = 'ShellCommand - '
                msg += 'total number of lines in reply  %d ' % lenReply
                msg += 'Truncating to %d lines\n' % self.nLinesToReport
                msg += formatCollection(reply[:self.nLinesToReport]
                                        )
                self.logger.debug(msg)

        return reply
            
    def suicide(self):
        self.logger.warning('Received request to terminate child process: ' + str(self.pid))
        os.system('kill -9 ' + str(self.pid))
        self.logger.warning('Child process ' + str(self.pid) + ' killed!')
