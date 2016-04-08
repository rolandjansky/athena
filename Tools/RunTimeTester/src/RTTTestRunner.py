# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This is a wrapper class for RTT tests and actions.
The purpose of the class is to allow a test to be run by a computing element,
as opposed to the processor running the RTT. Instances of RTTT3estRunner have
a test as an attribute. The instance is to be "shelved": in the run
directory, "unshelved" on the computing node, run, reshelved, and examined
by the RTT to determine the outcome.
"""

import shelve, sys, string, traceback, types

class RTTTestRunner:
    def __init__(self, dbPath, test, isAction):
        # test is a 3-item tuple
        self.dbPath   = dbPath
        self.test     = test[0] # action/test instance
        self.testId   = test[1]
        self.userPos  = test[2]
        self.result   = -1   # Failure
        self.error    = False
        self.errmsg   = ''
        self.logger   = None # to be retrieved from the test
        self.isAction = isAction # test or action?
        
    def autoShelve(self):
        db = shelve.open(self.dbPath)
        key = "(%s, %s)" % (str(self.userPos), str(self.testId))
        db[key] = self
        db.close()
        
    def run(self):
        try:
            self.result = self.test.run()
        except:
            self.error = True
            list   = sys.exc_info()
            self.errmsg  = str(list[0])
            self.errmsg += str(list[1])
            self.errmsg += string.join(traceback.format_tb(list[2]))

        self.logger = self.test.logger
        self.autoShelve()

    def transferLog(self, logger):
        if not self.logger: return # log not yet available

        levels = {
            'error'   : (logger.error, self.logger.errorL),
            'warning' : (logger.warning, self.logger.warningL),
            'info'    : (logger.info, self.logger.infoL),
            'debug'   : (logger.debug, self.logger.debugL)
            }
       
        actionOrTest = {True:'action', False:'test'}.get(self.isAction)

        for level in levels.keys():
            to_, from_ = levels[level]

            for line in from_:
                if type(line) in [types.ListType, types.TupleType]:
                    for each_thing in line:
                        message = '%s => %s: %s' % (actionOrTest, self.testId, each_thing.strip())
                        to_(message)
                elif type(line) is types.DictType:
                    for k,v in line.items():
                        message = '%s => %s: Key: %s, Value: %s' % (actionOrTest, self.testId, k, v)
                        to_(message)
                else:
                    line = str(line)
                    message = '%s => %s: %s' % (actionOrTest, self.testId, line.strip())
                    to_(message)
