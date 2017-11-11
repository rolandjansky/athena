#!/bin/env python
import sys

class progressBar:
    def __init__(self, minValue = 0, maxValue = 10, totalWidth=12, prefix="", suffix=""):
        self.progBar = "[]"   # This holds the progress bar string
        self.min = minValue
        self.max = maxValue
        self.span = maxValue - minValue
        self.amount = 0       # When amount == max, we are 100% done
        self.nextUpdate = minValue+float(self.span/100.0)
        if len(prefix): self.prefix=prefix+" ["
        else: self.prefix="["
        if len(suffix): self.suffix="] " + suffix
        else: self.suffix="]"
        self.width = totalWidth - len(self.prefix) - len(self.suffix)
        self.update(0)  # Build progress bar string

    def update(self, newAmount = 0):
        if newAmount < self.min: newAmount = self.min
        if newAmount > self.max: newAmount = self.max

        #=== only continue if something new to draw
        if newAmount < self.nextUpdate and newAmount<self.max : return
        self.nextUpdate = self.nextUpdate+float(self.span/100.0)
        self.amount = newAmount

        # Figure out the new percent done, round to an integer
        diffFromMin = float(self.amount - self.min)
        percentDone = (diffFromMin / float(self.span)) * 100.0 if self.span>0 else 100.
        percentDone = round(percentDone)
        percentDone = int(percentDone)

        # Figure out how many hash bars the percentage should be
        numHashes = (percentDone / 100.0) * self.width
        numHashes = int(round(numHashes))

        # build a progress bar with hashes and spaces
        self.progBar = self.prefix + '#'*numHashes + ' '*(self.width-numHashes) + self.suffix

        # figure out where to put the percentage, roughly centered
        percentPlace = (len(self.progBar) / 2) - len(str(percentDone))
        percentString = str(percentDone) + "%"

        # slice the percentage into the bar
        self.progBar = self.progBar[0:percentPlace] + percentString + self.progBar[percentPlace+len(percentString):]

        sys.stdout.write('\r' + self.progBar)
        sys.stdout.flush()# force updating of screen

    def done(self):
        self.update(self.max)
        sys.stdout.write('\r' + self.progBar + '\n')
        sys.stdout.flush()# force updating of screen

    def __str__(self):
        return str(self.progBar)
