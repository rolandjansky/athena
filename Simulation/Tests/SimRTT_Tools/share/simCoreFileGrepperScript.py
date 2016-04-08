#!/usr/bin/env python

"""
fileGrepper - function to look for strings or veto strings in a file.

classes:
FileGrepResult  Holds grep results for a file
Matcher         Applies the regexes to the lines of a file. Returns the
                results in a dictionary {regex:[matching lines]}

functions:
resultsDictToString formatting helper function.
fileGrepper
"""
import getopt
import re
import sys
import collections
import os


def resultsDictToString(ddict):
    """Convert a dictionary of regex: matching lines to a string"""

    s = []
    for k, matches in ddict.items():
        s.append('  %s:' % k)
        for v in matches:
            s.append('    ' + v)
    return '\n'.join(s)


class FileGrepResult(object):
    """Class that holds a file name and dictionaries
    of regex:matching lines for two lists of regexes"""

    def __init__(self, fn, searchResults, vetoResults, multInstanceResults):

        self.fn = fn
        self.searchResults = searchResults  # {pattern: [lines]}
        self.vetoResults = vetoResults  # {pattern: [lines]}
        self.multInstanceResults = multInstanceResults # {pattern: [[lines],nInstances] }
        self.nMultInstanceFailures = 0
        self.multInstanceResultString = self.multInstances()

    def vetoIsPresent(self):
        'return True if there is a non-empty vetoResult dictionary'

        if self.vetoResults:
            return True

        return False

    def multInstances(self):
        'return False if number of instances is too high'

        s = []
        if len(self.multInstanceResults)>0:
            for res,nInstances in self.multInstanceResults:
                for k, matches in res.items():
                    if len(matches) > int(nInstances):
                        self.nMultInstanceFailures += 1
                        s.append('  %s ( >%s instances ):' % (k,nInstances))
                        for v in matches:
                            s.append('    ' + v)
        return '\n'.join(s)

    def __str__(self):
        """Present data as a formatted string"""

        s = ['File: %s' % self.fn]
        s.append('Search Results [%d]' % len(self.searchResults))
        s.append(resultsDictToString(self.searchResults))
        s.append('')
        s.append('Veto Results [%d]' % len(self.vetoResults))
        s.append(resultsDictToString(self.vetoResults))
        s.append('')
        s.append('Multiple Instance results [%d]' % self.nMultInstanceFailures)
        s.append(self.multInstanceResultString)
        return '\n'.join(s)


class Matcher(object):
    """A class that performs regex matches on lines in a file"""

    def __init__(self, patternList,checkNInstances=False):
        """Converts an input list of regexes to a list of
        raw and compiled regexes. The raw regexes are needed
        for presenting results"""


        self.patternList = []
        self.nInstancesList = []
        self.checkNInstances = checkNInstances

        if checkNInstances:
            for i in range(0,len(patternList),2):
                p = patternList[i]
                self.patternList += [(p,re.compile(p))] 
                self.nInstancesList.append(patternList[i+1])

        else:
            
            self.patternList = [(p, re.compile(p)) for p in patternList]


    def match(self, fn):
        """Opens file with supplied file name, reads all it lines
        checking the against the regexes. If a line matches, store it
        in the {raw regex:[lines]} dictionary"""


        if not self.checkNInstances:
            matchDict = collections.defaultdict(list)
            with open(fn) as ifile:
                for l in ifile:
                    for  ori, comp in self.patternList:
                        if comp.search(l):
                            matchDict[ori].append(l)
            return matchDict

        else:
            result = []
            for i in range(len(self.patternList)):
                ori=self.patternList[i][0]
                comp=self.patternList[i][1]
                nInstances=self.nInstancesList[i]

                matchDict = collections.defaultdict(list)                
                with open(fn) as ifile:
                    for l in ifile:
                        if comp.search(l):
                            matchDict[ori].append(l)
                result.append([matchDict,nInstances])
            return result

def usage():
    print 'NAME '
    print '  fileGrepper'
    print
    print 'SYNOPSIS'
    print '  fileGrepper [option] [FILE]'
    print
    print 'DESCRIPTION'
    print ' Look for  lines in FILE that match search and veto '
    print 'fPattern vPattern are split at the commas. The input file'
    print 'is read line by line. Lines matching any of the fPattern or '
    print 'vPattern regexes are written to the output file.'
    print 'If a veto pattern matches any line, the script exits with '
    print 'exit code 1. Otherwise it exits with exit code 0'
    print
    print '  -f'
    print '     a comma separated string of regexes'
    print '  -v'
    print '     a comma separated string of regexes'
    print
    print '  -o'
    print '     name of output file if present. Otherwise output written to'
    print '     standard out unless -s option is present'
    print
    print '  -s '
    print '     silent mode'
    print ''

def fileGrepper(fList, vList, mList, inFile, outFile, silent=True):
    """Check the user has supplied necessary parameters. For files
    with names matching the user supplied file name pattern,
    store any lines mathcing the user supplied regexes.
    Print out a file of results containing name
    of file checked, the regexes checked for and the correponding lines
    found.

    Return 1 if any matching file has any line matching a veto pattern.
    Return 0 otherwise."""

    # run the Matchers. Store results in a FileGrepResult object.
    # make the results an attribute for unit tests.
    

    grepResult = FileGrepResult(inFile,
                                Matcher(fList).match(inFile),
                                Matcher(vList).match(inFile),
                                Matcher(mList,True).match(inFile)
                                )

    if not silent:
        text = [(str(grepResult))]
        text.append('')
        text = '\n'.join(text)

        print text

    if grepResult.vetoIsPresent():
        return 1

    if grepResult.multInstances():
        return 1

    return 0

if __name__ == '__main__':

    opts, args = getopt.getopt(sys.argv[1:], 'f:v:o:s:m:', [])

    fList = []
    vList = []
    mList = []
    outFile = ''
    silent = False
    nInstances = 0

    for o, v in opts:
        if o == '-f':
            fList = v.split(',')
        if o == '-v':
            vList = v.split(',')
        if o == '-o':
            outFile = v
        if o == '-s':
            silent = True
        if o == '-m':
            mList = v.split(',')

    if len(args) != 1:
        print 'Supply only one input file'
        print args
        usage()
        sys.exit(1)

    inFile = args[0]

    status = fileGrepper(fList=fList,
                         vList=vList,
                         mList=mList,
                         inFile=inFile,
                         outFile=outFile,
                         silent=silent)

    sys.exit(status)
