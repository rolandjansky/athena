#!/usr/bin/env python

__author__ = "Callum Kilby"

import sys
import getopt

def main(filePath) :
    """A script that takes in and parses a GRL xml file, and outputs a list of good lumi blocks.
The lines are formatted: RUNNUMBER, STARTLUMIBLOCK, ENDLUMIBLOCK."""
    GRLlist = open(filePath)
    runNum = 0
    for line in GRLlist :
        if '<Run>' in line :
            splitLine = line.split('<Run>')
            splitLine = splitLine[1].split('</Run>')
            runNum = splitLine[0]
            
        if '<LBRange Start="' in line :
            line = line.replace('<LBRange Start="', runNum+', ')
            line = line.replace('" End="', ', ')
            line = line.replace('"/>', '')
            line = line.strip()
            print line

if __name__ == "__main__" :
    usage =  "Usage: parseGRL.py [-h] <pathToGRLxmlFile>"
    usage += "\n-h --help\tprints this help message"
    
    try :
        opts, args = getopt.getopt(sys.argv[1:], 'h', ['help'])
    except getopt.GetoptError:
        print usage
        sys.exit()

    if args == []:
        print usage
        sys.exit()

    for opt, arg in opts :
        if opt == ('-h' or '--help') :
            print usage
            sys.exit()

    filePathIn = args[0]
    main(filePathIn)
