# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import types

import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
from PyCool import cool,coral

def ChooseOptionFromList(sTitle,sMessage,sPrompt,sChoiceList,iPlusMinus,bPlusMinus,optionDict):

    print ""
    print sTitle

    if bPlusMinus==False:
        for index,s in enumerate(sChoiceList):
            print "%3d : %-20s" % (index+iPlusMinus,s)
    else:
        for index,s in enumerate(sChoiceList):
            print "%3d/%3d : %-20s" % (index+iPlusMinus,-index,s)

    # option menu
    sMenu=""
    for s,v in optionDict:
        sMenu=sMenu+s+" "+v+"   "
    if sMenu!="": print "\n"+sMenu

    bValidResponse=False
    while bValidResponse==False:

        print " .. "+sPrompt+" > ",
        tty = open("/dev/tty", "r+")
        rep=tty.readline()
        rep=rep.strip()

        # response is in option list
        for s,v in optionDict:
            if rep==s: return rep

        # convert response to integer
        try:
            iResponse=int(rep)
            
            # reponse in indexes
            if bPlusMinus==False:
                if iResponse in range(iPlusMinus, len(sChoiceList)+iPlusMinus): return iResponse

            # reponse in indexes
            if bPlusMinus==True:
                if iResponse in range(iPlusMinus, len(sChoiceList)+iPlusMinus+1): return iResponse
                if iResponse in range(-len(sChoiceList)+iPlusMinus,iPlusMinus-1): return iResponse

            print "WARNING : response out of range"
                
        except:
            print "???"

        
def TypeEnterToContinue(sMessage):

    if sMessage!="": print sMessage
    print " .. Type ENTER to continue  > ",
    tty = open("/dev/tty", "r+")
    rep=tty.readline()


def YesNoQuestion(sMessage):

    print ""
    print sMessage+" [y/n] :",
    tty = open("/dev/tty", "r+")
    rep=tty.readline()
    rep=rep.strip()
    
    if rep=="y" or rep=="Y": return 1
    return 0



    from ROOT import TSQLServer
    def getNextAtlasRunNumber(self):
        """
        Returns the next free ATLAS run number
        """

        runNum    = -1
        server    = "oracle://localhost/atlr"
        statement = "select MAX(RUNNUMBER) FROM atlas_run_number.RUNNUMBER"
        db = TSQLServer.Connect(server,"atlas_run_number_r","07-Run.Num.rEaDeR")
        if not db or not db.IsConnected():
            raise(Exception("Problem connecting to run number server \"%s\"" % server))
        stmt = db.Statement(statement)
        if stmt.Process():
            stmt.StoreResult()
            stmt.NextResultRow()
            runNum = (stmt.GetUInt(0)+1)
        else:
            raise(Exception("Error in processing statement \"%s\"" % statement ))
        db.Close()
        return runNum
    
    

