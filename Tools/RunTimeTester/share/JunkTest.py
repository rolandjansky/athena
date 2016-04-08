######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

from Logger import Logger

class JunkTest:
    def __init__(self, argDict):
        self.int = 1
        self.logger   = Logger()

    def run(self):
        # self.logger.error('JunkTest RTT test run method')
        print 'JunkTest RTT test run method'
        self.logger.critical('JunkTest: critical message')
        self.logger.error('JunkTest: error message')
        self.logger.warning('JunkTest: warning message')
        self.logger.info('JunkTest: info message')
        self.logger.debug('JunkTest: debug message')
        return 0
        

