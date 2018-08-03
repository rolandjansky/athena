#!/usr/bin/env python

__author__ = "Will Buttinger"
__doc__ = """

List configurables (algorithms, tools, services) available. You can inspect the properties with the "-p" option. The main argument is a string to filter which algorithms are shown

Example: listConfigurables -ap CP__

"""

from AthenaCommon.Configurable import ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool

from AthenaCommon import ConfigurableDb

import textwrap

def main():
    import argparse
    from argparse import RawTextHelpFormatter
    
    parser = argparse.ArgumentParser(description=__doc__,formatter_class=RawTextHelpFormatter)
    parser.add_argument('-p',help="Print properties",action="store_true")
    parser.add_argument('-a',help="Show algorithms",action="store_true")
    parser.add_argument('-t',help="Show tools",action="store_true")
    parser.add_argument('-s',help="Show services",action="store_true")

    parser.add_argument('filter',nargs="*",help="filter string")
    
    args = parser.parse_args()


    showAll = not (args.a or args.t or args.s)

    ConfigurableDb.loadConfigurableDb()
    
    configurables = {}

    skipProperties = ['OutputLevel','Enable','ErrorMax','ErrorCounter','ExtraInputs','ExtraOutputs','AuditAlgorithms','AuditInitialize','AuditReinitialize','AuditRestart','AuditExecute','AuditFinalize','AuditTools','AuditBeginRun','AuditEndRun','AuditStart','AuditStop','Timeline','MonitorService','RegisterForContextService','IsClonable','Cardinality','NeededResources','EvtStore','DetStore','UserStore','THistSvc']

    for c in ConfigurableDb.cfgDb.keys():
        if len(args.filter)>0:
            ok=False
            for f in args.filter:
                if f in c: ok=True
            if not ok: continue
        
        #skip anything with gaudi in the name, boring
        pkg = ConfigurableDb.cfgDb[c]['package']
        if "GaudiExample" in pkg: continue
        instance = ConfigurableDb.getConfigurable(c)()
        if isinstance(instance,ConfigurableService):
            if not (showAll or args.s): continue
            if "Services" not in configurables:
                configurables["Services"] = dict()
            myDict = configurables["Services"]
        elif isinstance(instance,ConfigurableAlgorithm):
            if not (showAll or args.a): continue
            if "Algorithms" not in configurables:
                configurables["Algorithms"] = dict()
            myDict = configurables["Algorithms"]
        elif isinstance(instance,ConfigurableAlgTool):
            if not (showAll or args.t): continue
            if "Tools" not in configurables:
                configurables["Tools"] = dict()
            myDict = configurables["Tools"]
        
        if pkg not in myDict:
            myDict[pkg] = list() #list of configurables

        propDict = {}
        propDescr = instance._propertyDocDct if hasattr(instance,"_propertyDocDct") else {}
        for propName in instance.__slots__.keys():
            propDict[propName] = propDescr.get(propName," <NO DESCRIPTION> ")

        myDict[pkg] += [(str(c), propDict  )]

    #print the results
    for a,b in configurables.iteritems():
        print "%s:" % a
        for pkgName,cs in b.iteritems():
            print "  %s:" % pkgName
            for (cName,pList) in cs:
                printString = "    %s" % cName
                if args.p: printString += ":"
                print printString
                if args.p:
                    pLength = 0
                    for pName,pDescr in pList.iteritems():
                        if pName in skipProperties: continue
                        if len(pName) > pLength: pLength = len(pName)
                    padString = "      %" + str(pLength) + "s :%s"
                    padString2 ="      %" + str(pLength) + "s   %s"
                    for pName,pDescr in pList.iteritems():
                        if pName in skipProperties: continue
                        myString = textwrap.wrap(pDescr)
                        if len(myString)==0: print padString % (pName," <NO DESCRIPTION>")
                        else:
                            print padString % (pName,myString[0])
                            for f in myString[1:]:
                                print padString2 % ("",f)


    return 0

import os
if __name__ == "__main__":
    
    os._exit(main())
