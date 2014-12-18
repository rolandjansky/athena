# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import bisect

class TileCellHashMgr():
    """
    This class ised to convert cell hash to cell name
    """
    #____________________________________________________________________
    def __init__(self):

        #=== initialize all arrays
        self.__cellNames=[ 
            ["A-1","B-1","A-2","B-2","A-3","B-3","D-1","A-4","B-4","A-5","B-5","D-2","A-6","B-6","A-7","B-7","D-3","A-8","B-8","A-9","B-9","A-10"],
            ["A+1","B+1","D*0","A+2","B+2","A+3","B+3","D+1","A+4","B+4","A+5","B+5","D+2","A+6","B+6","A+7","B+7","D+3","A+8","B+8","A+9","B+9","A+10"],
            ["B-11","D-5","A-12","B-12","A-13","B-13","D-6","A-14","B-14","A-15","B-15","A-16"],
            ["B+11","D+5","A+12","B+12","A+13","B+13","D+6","A+14","B+14","A+15","B+15","A+16"],
            ["D-4","C-10","E-1","E-2","E-3","E-4","spD-4","spC-10","spD-40"],
            ["D+4","C+10","E+1","E+2","E+3","E+4","spD+4","spC+10","spD+40"]
            ]

        self.__partNames=["LBC","LBA","EBC","EBA","EBC","EBA"]
        self.__nCellsModule = [22,23,12,12,6,6]
        self.__nModules = 64
        self.__nCellsPart = [0]
        self.__nHashMax=0
        for p in xrange(6):
            self.__nHashMax+=self.__nCellsModule[p]*self.__nModules
            self.__nCellsPart.append(self.__nHashMax)

    #____________________________________________________________________
    def getNames(self, hash):
        """
        return module name and cell name for given hash
        """
        if hash>=self.__nHashMax or hash<0:
            return (None,None)

        part=bisect.bisect(self.__nCellsPart,hash)-1
        module=(hash-self.__nCellsPart[part])/self.__nCellsModule[part]+1
        modName="%s%02d" % (self.__partNames[part],module)
        index=(hash-self.__nCellsPart[part])%self.__nCellsModule[part]
        if part>=4:
            if index==0 and ((part==4 and module==18) or (part==5 and module==15)): index+=8
            if index==0 and (module>=15 and module<=18): index+=6
            if index==1 and ((module>=39 and module<=42) or (module>=55 and module<=58)): index+=6

        return (modName,self.__cellNames[part][index])

    #____________________________________________________________________
    def cellName(self, hash):
        """
        return cell name for given hash
        """
        if hash>=self.__nHashMax or hash<0:
            return None

        part=bisect.bisect(self.__nCellsPart,hash)-1
        module=(hash-self.__nCellsPart[part])/self.__nCellsModule[part]+1
        index=(hash-self.__nCellsPart[part])%self.__nCellsModule[part]
        if part>=4:
            if index==0 and ((part==4 and module==18) or (part==5 and module==15)): index+=8
            if index==0 and (module>=15 and module<=18): index+=6
            if index==1 and ((module>=39 and module<=42) or (module>=55 and module<=58)): index+=6

        return self.__cellNames[part][index]

    #____________________________________________________________________
    def moduleName(self, hash):
        """
        return module name for given hash
        """
        if hash>=self.__nHashMax or hash<0:
            return None

        part=bisect.bisect(self.__nCellsPart,hash)-1
        module=(hash-self.__nCellsPart[part])/self.__nCellsModule[part]+1
        modName="%s%02d" % (self.__partNames[part],module)

        return modName
