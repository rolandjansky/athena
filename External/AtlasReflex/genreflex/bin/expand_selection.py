#!/usr/bin/env python
#+
# Title:
#   expandselection.py
#
# Description:
#   Python script to expand the selection xml files to include datalinks etc.
#
# Argument list:
#   expandselection.py --inputfile <inputfile> --outputfile <outputfile>
#   --navigables="<navigables> --dataLinks="<datalinks> --elementlinks="<elementlinks>
#   --elvectors="Nelvectors> --templates=<template-dir>
#
# Authors:
#   David Quarrie (David.Quarrie@cern.ch)
#-

#+
# Globals
#-

import os
import sys
import argparse

#+
# Main Entrypoint
#-

def main():

    inputfile    = ""
    outputfile   = ""
    navigables   = []
    datalinks    = []
    elementlinks = []
    elvectors    = []
    templates    = ""
    printmode    = False

    myPath = os.path.abspath(__file__)
    myDir  = os.path.dirname(myPath)
    sys.path.append(myDir)

    parser = argparse.ArgumentParser(description='Expand selection.xml file')
    parser.add_argument("--inputfile", "-i", dest="inputfile", default="selection.xml", metavar="<file>", help="Input XML file (default selection.xml)")
    parser.add_argument("--outputfile", "-o", dest="outputfile", default="selection_expanded.xml", metavar="<file>", help="Output XML file (default selection_expanded.xml)")
    parser.add_argument("--navigables", "-n", dest="navigables", default=[], metavar="<navigables>", help="List of navigables (default none)")
    parser.add_argument("--datalinks", "--dlinks", "-d", dest="datalinks", default=[], action='append', metavar="<datalinks>", help="List of datalinks (default none)")
    parser.add_argument("--elementlinks", "--elinks", "-e", dest="elementlinks", default=[], action='append', metavar="<elementlinks>", help="List of elementlinks (default none)")
    parser.add_argument("--elvectors", "--vectors", "-v", dest="elvectors", default=[], action='append', metavar="<elvectors>", help="List of elementlink vectors (default none)")
    parser.add_argument("--templates", "-t", dest="templates", default="", required=True, metavar="<dir>", help="Directory containing template XML files")
    parser.add_argument("--print", "-p", dest="printmode", default=False, action="store_true", help="Print information (default none)")
    parser.add_argument("--noprint", dest="printmode", action="store_false", help="Don't print information (default none)")
    args = parser.parse_args()
    inputfile    = args.inputfile
    outputfile   = args.outputfile
    navigables   = buildArg(args.navigables)
    datalinks    = buildArg(args.datalinks)
    elementlinks = buildArg(args.elementlinks)
    elvectors    = buildArg(args.elvectors)
    navigables   = buildArg(args.navigables)
    printmode    = args.printmode
    templates    = args.templates
    if os.path.isdir(templates):        
        print "inputfile   : %s" % inputfile
        print "outputfile  : %s" % outputfile
        print "navigables  : %s" % navigables
        print "datalinks   : %s" % datalinks
        print "elementlinks: %s" % elementlinks
        print "elvectors   : %s" % elvectors
        print "templates   : %s" % templates
        print "printmode   : %s" % printmode
        result = False
        
        # Check that the input file exists and is readable
        if os.path.exists(inputfile) and not os.path.isdir(inputfile):
            input = open(inputfile, 'r')
            if input != None:
            
                # Input file is readable - attempt to create output file
                output = open(outputfile, 'w')
                if output != None:
            
                    # Copy the contents of the input file to the output file
                    for l in input:
                        if not l.strip().startswith('</lcgdict>'):
                            output.write(l)
                    input.close()
            
                    # Append the Navigables fragments
                    result = True
                    result = appendFragment(result, navigables, "Navigable", templates, output)
                    result = appendFragment(result, navigables, "ElementLink", templates, output)
                    result = appendFragment(result, navigables, "ElementLinkVector", templates, output)
                    result = appendFragment(result, navigables, "DataLink", templates, output)
                    
                    # Append the DataLink fragments
                    result = appendFragment(result, datalinks, "DataLink", templates, output)

                    # Append the ElementLink fragments
                    result = appendFragment(result, elementlinks, "ElementLink", templates, output)
                    result = appendFragment(result, elementlinks, "ElementLinkVector", templates, output)
                    result = appendFragment(result, elementlinks, "DataLink", templates, output)

                    # Append the ElementLinkVector fragments
                    result = appendFragment(result, elvectors, "ElementLinkVector", templates, output)
                    result = appendFragment(result, elvectors, "DataLink", templates, output)
                    
                    # Append the closing tag
                    output.write('</lcgdict>\n')
                    output.close
                else:
                    print "expandselection: Unable to open output file (%s)" % outputfile
                    input.close()
                    result = False    
            else:
                print "expandselection: Unable to open input file (%s)" % inputfile
                result = False  
        else:
            print "expandselection: input file (%s) doesn't exist" % inputfile
            result = False  
    else:
        print "Templates directory (%s) doesn't exist" % templates
    return result

def buildArg(args):
    result = []
    for a in args:
        items = a.strip('"').strip("'").split(' ')
        for i in items:
            result.append(i)
    return result

def appendFragment(result, items, name, dir, output):
    print "appendFragment: result:%s, items:%s, name:%s, dir:%s" % (result, items, name, dir)
    if result:
        if len(items) > 0:
            result = False
            file   = os.path.join(dir, name+'_selection.xml')
            f      = open(file, 'r')
            if f != None:
                lines = []
                for l in f:
                    lines.append(l)
                f.close()
                for it in items:
                    for l in lines:
                        l = l.replace('${class}', it)
                        output.write(l)
                result = True
            else:
                print "Unable to open fragment file %s" % file
    return result
        
if __name__ == '__main__':
    status = 1
    try:
        result = main()
        if result:
            status = 0
    except KeyboardInterrupt:
        print "Setup aborted and possibly incomplete"
        status = 1
    except SystemExit:
        pass	    
    except Exception, e:
        print "Fatal exception: %s" % str(e)
        status = 2
    sys.exit(status)
