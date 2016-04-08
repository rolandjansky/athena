######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

import string
from Logger            import Logger

class FileToDict:
    def __init__(self, argDict):

        self.inFile   = argDict.get('datafile')
        self.logger   = Logger()
        # self.logger = params['logger']

    def run(self):
        self.DataDict      = self.makeDict(self.inFile)

        return self.DataDict
        

    def retrieveFile(self, key, db):
        pass
    
    
    def linesCleanUp(self, lines):
        ls =[]
        for line in lines:
            line =string.strip(line)
            if len(line)>0:
                ls.append(line)

        return ls


    def makeDict(self,file):
        lines         = open(file, 'r').readlines()
        lines         = self.linesCleanUp(lines)
        
        self.dict={}
        for item in lines:
            keyValuePairs     = string.split(item, ":")
            for m in  keyValuePairs:
                keyValuePairs = self.linesCleanUp(keyValuePairs)
            self.dict[keyValuePairs[0]] = int( keyValuePairs[1])
            
        return self.dict




