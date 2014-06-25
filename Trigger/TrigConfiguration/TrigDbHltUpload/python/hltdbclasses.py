#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
#
# Miroslav Nozicka 
#
# HLT base DB Classes
# ------------------------------------------------------------------------

from sys import stdout

class Column(object):
  def __init__( self, name, value, type, test = True):
    self.name  = name
    self.value = value
    self.type  = type
    self.test  = test

class DBTable(object):
  def __repr__(self):
    return "DBTable"


class progressBar:
    def __init__(self, minValue = 0, maxValue = 100, totalWidth=20):
        self.progBar = "[]"   # This holds the progress bar string
        self.min    = minValue
        self.max    = maxValue
        self.span   = maxValue - minValue
        self.width  = totalWidth
        self.amount = 0       # When amount == max, we are 100% done 
        self.oldNumHashes = -1 # number of hashes drawn last time
    
    def updateAmount(self, newAmount = 0):
        self.amount = newAmount

        # Figure out the new percent done, round to an integer
        diffFromMin = self.amount - self.min
        percentDone = (diffFromMin / float(self.span)) * 100.0
        percentDone = round(percentDone)

        # Figure out how many hash bars the percentage should be
        allFull = self.width
        numHashes = (percentDone / 100.0) * allFull
        numHashes = int(round(numHashes))

        self.updateString = 0
        if numHashes == self.oldNumHashes and self.amount!=self.max: return

        # update status
        self.oldNumHashes = numHashes
        self.updateString = 1

        # rebuild the status string
        self.progBar = "[" + '#'*numHashes + ' '*(allFull-numHashes) + "]" + str(int(percentDone)) + "%"

    def printbar(self, newAmount):
        if newAmount < self.min or newAmount > self.max: return
        prog.updateAmount(newAmount)
        if self.updateString:
            print str(self.progBar) + "\r",
            stdout.flush()
