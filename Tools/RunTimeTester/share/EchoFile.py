######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

from Logger            import Logger

class EchoFile:
    def __init__(self, argDict):

        self.inFile   = argDict.get('datafile')
        # self.logger = params['logger']
        self.logger   = Logger()

    def run(self):
        inF  = open(self.inFile, 'r').readlines()
        outF = open('echoedFile.txt', 'w')

        for line in inF:
            outF.write(line)

        outF.close()
        
