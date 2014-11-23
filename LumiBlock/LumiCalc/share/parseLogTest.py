#!/usr/bin/env python

from LumiCalc.LumiCalcHtml import LumiCalc

lc = LumiCalc()
lc.workdir = '.'
lc.verbose = True
lc.uselar = True
lc.subdir='35b3e2'
lc.grlfn=''
lc.f = open('./working.html', 'w')

lc.parseOutput()
lc.printTable()
lc.printWarnings()
lc.printLinks()
lc.printFinish()
lc.cleanUp()
