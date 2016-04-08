# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ShellCommand import ShellCommand
class MemoryUsageDiagnostic:
    def __init__(self, logger):
        self.commands={
            '/usr/bin/vmstat':    [],
            '/usr/bin/vmstat -a': []
            }
        self.logger = logger

    def execute(self):
        for c in self.commands.keys():
            sc = ShellCommand([c], self.logger)
            reply = sc.getReply()
            # [self.logger.debug(r) for r in reply]
            self.commands[c].append(reply)
            

if __name__ == '__main__':

    from PseudoLogger import PseudoLogger
    md = MemoryUsageDiagnostic(PseudoLogger())
    md.execute()
