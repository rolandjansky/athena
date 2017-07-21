# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import bisect

class TileCellHashMgr():
    """
    This class ised to convert cell hash to cell name
    """
    #____________________________________________________________________
    def __init__(self, geometry='Default'):

        #=== initialize all arrays
        self._geometry=geometry
        if geometry == "UpgradeABC":
            self._cellNames=[
            ["A-1","A-2","A-3","B-1","C-1","A-4","A-5","A-6","A-7","B-2","C-2","A-8","A-9","D-1","A-10","A-11","B-3","C-3","A-12","A-13","A-14","A-15","B-4","C-4",
             "A-16","A-17","D-2","A-18","A-19","B-5","C-5","A-20","A-21","A-22","A-23","B-6","C-6","A-24","A-25","D-3","A-26","A-27","B-7","C-7","A-28","A-29","A-30",
             "A-31","B-8","C-8","A-32","A-33","A-34","A-35","B-9","A-36","A-37","A-38","A-39","A-40"],
            ["A+1","D*0","A+2","A+3","B+1","C+1","A+4","A+5","A+6","A+7","B+2","C+2","A+8","A+9","D+1","A+10","A+11","B+3","C+3","A+12","A+13","A+14","A+15","B+4","C+4",
             "A+16","A+17","D+2","A+18","A+19","B+5","C+5","A+20","A+21","A+22","A+23","B+6","C+6","A+24","A+25","D+3","A+26","A+27","B+7","C+7","A+28","A+29","A+30",
             "A+31","B+8","C+8","A+32","A+33","A+34","A+35","B+9","A+36","A+37","A+38","A+39","A+40"],
            ["D-5","A-42","B-11","A-44","A-45","A-46","A-47","B-12","A-48","A-49","D-6","A-50","A-51","B-13","A-52","A-53","A-54","A-55","B-14","A-56","A-57","A-58","A-59","B-15",
             "A-60","A-61","A-62","A-63","A-64"],
            ["D+5","A+42","B+11","A+44","A+45","A+46","A+47","B+12","A+48","A+49","D+6","A+50","A+51","B+13","A+52","A+53","A+54","A+55","B+14","A+56","A+57","A+58","A+59","B+15",
             "A+60","A+61","A+62","A+63","A+64"],
            ["D-4","C-10","E-1","E-2","E-3","E-4","spD-4","spC-10","spD-40","spE-1","mbE-1","e4E-1"],
            ["D+4","C+10","E+1","E+2","E+3","E+4","spD+4","spC+10","spD+40","spE+1","mbE+1","e4E+1"]
            ]
            self._nCellsModule = [60,61,29,29,6,6]

        elif geometry == "UpgradeBC":
            self._cellNames=[
            ["A-1","B-1","C-1","A-2","B-2","C-2","A-3","B-3","C-3","D-1","A-4","B-4","C-4","A-5","B-5","C-5","D-2","A-6","B-6","C-6","A-7","B-7","C-7",
             "D-3","A-8","B-8","C-8","A-9","B-9","A-10"],
            ["A+1","B+1","C+1","D*0","A+2","B+2","C+2","A+3","B+3","C+3","D+1","A+4","B+4","C+4","A+5","B+5","C+5","D+2","A+6","B+6","C+6","A+7","B+7","C+7",
             "D+3","A+8","B+8","C+8","A+9","B+9","A+10"],
            ["B-11","D-5","A-12","B-12","A-13","B-13","D-6","A-14","B-14","A-15","B-15","A-16"],
            ["B+11","D+5","A+12","B+12","A+13","B+13","D+6","A+14","B+14","A+15","B+15","A+16"],
            ["D-4","C-10","E-1","E-2","E-3","E-4","spD-4","spC-10","spD-40","spE-1","mbE-1","e4E-1"],
            ["D+4","C+10","E+1","E+2","E+3","E+4","spD+4","spC+10","spD+40","spE+1","mbE+1","e4E+1"]
           ]
            self._nCellsModule = [30,31,12,12,6,6]

        elif geometry == "UpgradeA":
            self._cellNames=[
            ["A-1","A-2","A-3","B-1","A-4","A-5","A-6","A-7","B-2","A-8","A-9","D-1","A-10","A-11","B-3","A-12","A-13","A-14","A-15","B-4","A-16","A-17","D-2","A-18",
             "A-19","B-5","A-20","A-21","A-22","A-23","B-6","A-24","A-25","D-3","A-26","A-27","B-7","A-28","A-29","A-30","A-31","B-8","A-32","A-33","A-34",
             "A-35","B-9","A-36","A-37","A-38","A-39","A-40"],
            ["A+1","D*0","A+2","A+3","B+1","A+4","A+5","A+6","A+7","B+2","A+8","A+9","D+1","A+10","A+11","B+3","A+12","A+13","A+14","A+15","B+4","A+16","A+17","D+2","A+18",
             "A+19","B+5","A+20","A+21","A+22","A+23","B+6","A+24","A+25","D+3","A+26","A+27","B+7","A+28","A+29","A+30","A+31","B+8","A+32","A+33","A+34",
             "A+35","B+9","A+36","A+37","A+38","A+39","A+40"],
            ["D-5","A-42","B-11","A-44","A-45","A-46","A-47","B-12","A-48","A-49","D-6","A-50","A-51","B-13","A-52","A-53","A-54","A-55","B-14","A-56","A-57","A-58","A-59","B-15",
             "A-60","A-61","A-62","A-63","A-64"],
            ["D+5","A+42","B+11","A+44","A+45","A+46","A+47","B+12","A+48","A+49","D+6","A+50","A+51","B+13","A+52","A+53","A+54","A+55","B+14","A+56","A+57","A+58","A+59","B+15",
             "A+60","A+61","A+62","A+63","A+64"],
            ["D-4","C-10","E-1","E-2","E-3","E-4","spD-4","spC-10","spD-40","spE-1","mbE-1","e4E-1"],
            ["D+4","C+10","E+1","E+2","E+3","E+4","spD+4","spC+10","spD+40","spE+1","mbE+1","e4E+1"]
            ]
            self._nCellsModule = [52,53,29,29,6,6]

        else:
            self._geometry="Default"
            self._cellNames=[
            ["A-1","B-1","A-2","B-2","A-3","B-3","D-1","A-4","B-4","A-5","B-5","D-2","A-6","B-6","A-7","B-7","D-3","A-8","B-8","A-9","B-9","A-10"],
            ["A+1","B+1","D*0","A+2","B+2","A+3","B+3","D+1","A+4","B+4","A+5","B+5","D+2","A+6","B+6","A+7","B+7","D+3","A+8","B+8","A+9","B+9","A+10"],
            ["B-11","D-5","A-12","B-12","A-13","B-13","D-6","A-14","B-14","A-15","B-15","A-16"],
            ["B+11","D+5","A+12","B+12","A+13","B+13","D+6","A+14","B+14","A+15","B+15","A+16"],
            ["D-4","C-10","E-1","E-2","E-3","E-4","spD-4","spC-10","spD-40","spE-1","mbE-1","e4E-1"],
            ["D+4","C+10","E+1","E+2","E+3","E+4","spD+4","spC+10","spD+40","spE+1","mbE+1","e4E+1"]
            ]
            self._nCellsModule = [22,23,12,12,6,6]

        self._partNames=["LBC","LBA","EBC","EBA","EBC","EBA"]
        self._nModules = 64
        self._nCellsPart = [0]
        self._nHashMax=0
        for p in xrange(6):
            self._nHashMax+=self._nCellsModule[p]*self._nModules
            self._nCellsPart.append(self._nHashMax)

    #____________________________________________________________________
    def getGeometry(self):
        """
        return geometry name
        """
        return self._geometry

    #____________________________________________________________________
    def getHashMax(self):
        """
        return max hash value
        """
        return self._nHashMax

    #____________________________________________________________________
    def getNames(self, hash):
        """
        return module name and cell name for given hash
        """
        if hash>=self._nHashMax or hash<0:
            return (None,None)

        part=bisect.bisect(self._nCellsPart,hash)-1
        module=(hash-self._nCellsPart[part])/self._nCellsModule[part]+1
        modName="%s%02d" % (self._partNames[part],module)
        index=(hash-self._nCellsPart[part])%self._nCellsModule[part]
        if part>=4:
            if index==0 and ((part==4 and module==18) or (part==5 and module==15)): index+=8
            if index==0 and (module==14 or module==15 or module==18 or module==19): index+=6
            if index==1 and ((module>=39 and module<=42) or (module>=55 and module<=58)): index+=6
            if index==2 and (module==7 or module==25 or module==44 or module==53): index+=7
            if index==2 and (module==8 or module==24 or module==43 or module==54): index+=8
            if part==4:
                if index==2 and (module==28 or module==31 or module==35 or module==38): index+=7
                if index==2 and (module==29 or module==32 or module==34 or module==37): index+=9

        return (modName,self._cellNames[part][index])

    #____________________________________________________________________
    def cellName(self, hash):
        """
        return cell name for given hash
        """
        if hash>=self._nHashMax or hash<0:
            return None

        part=bisect.bisect(self._nCellsPart,hash)-1
        module=(hash-self._nCellsPart[part])/self._nCellsModule[part]+1
        index=(hash-self._nCellsPart[part])%self._nCellsModule[part]
        if part>=4:
            if index==0 and ((part==4 and module==18) or (part==5 and module==15)): index+=8
            if index==0 and (module>=15 and module<=18): index+=6
            if index==1 and ((module>=39 and module<=42) or (module>=55 and module<=58)): index+=6
            if index==2 and (module==7 or module==8 or module==25 or module==24 or module==44 or module==43 or module==53 or module==54): index+=7
            if part==4:
                if index==2 and (module==28 or module==29 or module==31 or module==32 or module==35 or module==34 or module==38 or module==37): index+=7

        return self._cellNames[part][index]

    #____________________________________________________________________
    def moduleName(self, hash):
        """
        return module name for given hash
        """
        if hash>=self._nHashMax or hash<0:
            return None

        part=bisect.bisect(self._nCellsPart,hash)-1
        module=(hash-self._nCellsPart[part])/self._nCellsModule[part]+1
        modName="%s%02d" % (self._partNames[part],module)

        return modName
