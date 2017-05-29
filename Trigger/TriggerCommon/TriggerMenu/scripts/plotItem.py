#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, commands
import matplotlib.pyplot as plt
import TriggerClasses

class plotter:
    """
    Class containing some basic style elements for visualing prescales
    """

    colors = [ 'black', 'blue', 'red', 'green']
    styles = ['o-', 'x-', 'v-.', '*--' ]
    widths = [1, 1, 2, 2]
    xtitle = "Luminosity Points"
    ytitle = "Prescale Value"
    L1Name = 'L1'
    L2Name = 'L2'
    EFName = 'EF'

    class plotterErr:
        def __init__(self, value):
            self.value = value
        def __str__(self):
            return repr(self.value)

    def setValues(self, PS):
        #nLumi = len(PS['Lumi'])
        
        try:
            self.EFPS = PS['EFPS']
            self.L2PS = PS['L2PS']
            self.L1PS = PS['L1PS']
            self.TotalPS = PS['TotalPS']
        except KeyError:
            err = "plotter.setValues expects entries for 'EFPS', 'L2PS', and 'L1PS'"
            raise NameError(err)

        if 'EFName' in PS:
            self.EFName = PS['EFName']
        if 'L2Name' in PS:
            self.L2Name = PS['L2Name']
        if 'L1Name' in PS:
            self.L1Name = PS['L1Name']

        self.Lumi = PS['Lumi']

    def plotAll(self):
        """
        Make a plot overlaying L1, L2, EF PS, and the total PS.
        """
        #total = [max([x*y*z,-1]) for x,y,z in zip(self.L1PS, self.L2PS, self.EFPS)]
        total = self.TotalPS

        #Make a new canvas, twice as long as it is wide
        w,h = plt.figaspect(0.4)
        fig = plt.figure(figsize=(w*1.,h*1.))

        #Set the plotting area
        ax = fig.add_axes([0.1, 0.47, 0.85, 0.42])
        ax.plot(self.Lumi , self.L1PS , self.styles[0] , color=self.colors[0] , linewidth=self.widths[0] , label=self.L1Name)
        ax.plot(self.Lumi , self.L2PS , self.styles[1] , color=self.colors[1] , linewidth=self.widths[1] , label=self.L2Name)
        ax.plot(self.Lumi , self.EFPS , self.styles[2] , color=self.colors[2] , linewidth=self.widths[2] , label=self.EFName)
        ax.plot(self.Lumi , total     , self.styles[3] , color=self.colors[3] , linewidth=self.widths[3] , label='Total'    )
        
        #Set the label text
        plt.ylabel(self.ytitle)

        maxL1 = max(self.L1PS)
        maxL2 = max(self.L2PS)
        maxEF = max(self.EFPS)
        maxT  = max(total)

        #Set the x axis range, and tick points
        axis = [0, max(self.Lumi) + 1000, -10, max(maxL1, maxL2, maxEF, maxT)*1.1]
        plt.axis(axis)
        plt.xticks(self.Lumi)

        #Turn on the grid marks
        plt.grid(True)

        #Put the legend above the plot
        plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)

        ax_log = fig.add_axes([0.1, 0.05, 0.85, 0.42])
        ax_log.plot(self.Lumi , self.L1PS , self.styles[0] , color=self.colors[0] , linewidth=self.widths[0] , label=self.L1Name)
        ax_log.plot(self.Lumi , self.L2PS , self.styles[1] , color=self.colors[1] , linewidth=self.widths[1] , label=self.L2Name)
        ax_log.plot(self.Lumi , self.EFPS , self.styles[2] , color=self.colors[2] , linewidth=self.widths[2] , label=self.EFName)
        ax_log.plot(self.Lumi , total     , self.styles[3] , color=self.colors[3] , linewidth=self.widths[3] , label='Total'    )

        #Set the label text
        plt.xlabel(self.xtitle)
        plt.ylabel(self.ytitle)
        plt.yscale('log')
        
        #Set the x axis range, and tick points
        axis = [0, max(self.Lumi) + 1000, 0.5, max(total)*2]
        plt.axis(axis)
        plt.xticks(self.Lumi)

        #Turn on the grid marks
        plt.grid(True)

        #Print the plot to screen
        plt.show()

def readRuleBook(file_path):
    output = {}
    
    badlines = []
    duplicates = []
    
    file = open(file_path, 'r')
    for line in file: 
                
        if line.count('#') or not line.count(';'):
            continue

        rule = TriggerClasses.TriggerRule()
        
        if not rule.ReadLine(line):
            badlines.append(line.rstrip('\n'))
            continue
        
        if rule.GetTriggerName() in output:
            duplicates.append(rule.GetTriggerName())

        if len(badlines) != 0 or len(duplicates) != 0:
            err = "Failed to read %s lines: \n" % len(badlines)
            for line in badlines:
                err += "\t%s\n" % line
            err += "Found %s duplicate entries: \n" % len(duplicates)
            for dup in duplicates:
                err += "\t%s\n" % dup

            raise NameError(err)

        output[rule.GetTriggerName()] = rule

    return output

def main(name, options):

    hlt = options.hlt
    lvl1 = options.lvl1
    rules = options.rules

    print "Running with:"
    print "Chain name :", name
    print "HLT XML    :", hlt
    print "LVL1 XML   :", lvl1
    print "Rulebook   :", rules
        
    trigger_rules = {}
    trigger_rules = readRuleBook(rules)

    myPlotter = plotter()
    
    myChain = TriggerClasses.Chain()
    #Extrapolate rules out to a minimum and maximum value
    (myChain.minLumi, myChain.maxLumi) = (options.minL, options.maxL)
    
    myChain.buildChainFromMenu(name,  hlt, lvl1)
    myChain.buildRules(trigger_rules)
    PS =  myChain.getPS()
    
    myPlotter.setValues(PS)
    myPlotter.plotAll()
    
if __name__ == "__main__":
    AtlasArea = os.environ.get("AtlasArea")
    L1_XML  = commands.getoutput("ls %s/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v1_*xml" % AtlasArea)
    HLT_XML = commands.getoutput("ls %s/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v1_*xml" % AtlasArea)
    rules   = commands.getoutput("ls %s/Trigger/TriggerCommon/TriggerMenu/rules/Physics_pp_rulebook_PT.txt" % AtlasArea)

    from optparse import OptionParser
    p = OptionParser("usage: %prog [options] <chain name>")
    
    p.add_option('--rulebook' , type = "str" , default=rules   , dest="rules" , help="location of the rulebook - default [%default]")
    p.add_option('--hlt'      , type = "str" , default=L1_XML  , dest="hlt"   , help="location of the HLT menu - default [%default]")
    p.add_option('--lvl1'     , type = "str" , default=HLT_XML , dest="lvl1"  , help="location of the L1 menu - default [%default]")
    p.add_option('--minLumi'  , type = "int" , default=1000    , dest="minL"  , help="Force a minimum luminosity - default [%default]")
    p.add_option('--maxLumi'  , type = "int" , default=100000  , dest="maxL"  , help="Force a maximum luminosity - default [%default]")

    (options, args) = p.parse_args()

    if (len(args) < 1):
        print p.get_usage()
        
    else:
        name = args[0]

        main(name, options)
