#!/usr/bin/env python

"""SimCoreFileGrepper - runs simCoreFileGrepper.py. RTT Postprocessing class.
vlaidates inputs, calls fileGrepper for each file matching input file pattern.
Based on FileGrepper.py.
"""

######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannot shelve the objects                #
# Instead, unload the necessary variables in from    #
# more complicated classes in __init__               #
#                                                    #
######################################################

import os
import glob

from ExeRunnerBase import ExeRunnerBase


def fixPatterns(pList, pString):
    """PList is a list of comma separated strings or ''
    pString is a list of strings or ''For

    """
    if not isinstance(pString, list):
        if pString:
            pString = [pString]
        else:
            pString = []

    if not isinstance(pList, list):
        if pList:
            pList = [pList]
        else:
            pList = []

    l = []
    l.extend(pList)
    l.extend(pString)

    l = ','.join(l)
    l = "'%s'" % l  # escape the regexes as will be seen by the shell

    return l


class SimCoreFileGrepper(ExeRunnerBase):
    """Grep files with using two lists of regexes"""

    def __init__(self, paramDict):
        """Parameters are passed in via a parameter dictionary.
        Read these, and use the veto and serach regexes lists
        to set up the corresponding matcher objects. These
        will find matching lines, and place the results in
        a {regex:[line]} dictionary"""

        testIdentifier = paramDict['testIdentifierObj']

        # find the name of the output file.
        # new-style tests: the output file is passed in via an
        # optional xml tag. If the tag is missing, the value defaults to ''.
        # For old style tests (and new style tests using old style xml tags),
        # the name is sometimes passed in using the <arg> tag, with
        # <argName>outputFile</argName> as a sub tag.
        #
        # The old-style name is used preferentially to the new-style name
        # to avoid abliging users to change there package files.
        #

        outputFile = paramDict['testOutputFile']  # pick up new-style name
        try:  # use old-style name is present.
            outputFile = paramDict['outputFile']
        except:
            pass

        ExeRunnerBase.__init__(self,
                               cmd='',    # set in run
                               testIdentifier=testIdentifier,
                               outputFile=outputFile)

        # argument passing gone mad....

        fList = paramDict.get('searchList', '')
        fStrings = paramDict.get('searchStrings', '')
        self.fPatterns = fixPatterns(fList, fStrings)

        vList = paramDict.get('vetoList', '')
        vStrings = paramDict.get('vetoStrings', '')
        self.vPatterns = fixPatterns(vList, vStrings)

        mList = paramDict.get('multipleInstanceList', '')
        mStrings = paramDict.get('multipleInstanceStrings', '')
        self.mPatterns = fixPatterns(mList, mStrings)

        self.iFilePat = paramDict['inputFile']

        ok, msg = self.configurationIsOK()
        if not ok:
            raise RuntimeError(
                '%s.__init__, bad configuration:\n %s' % (
                    self.__class__.__name__,
                    msg
                    )
                )

        self.iFilePat = os.path.abspath(self.iFilePat)

    def configurationIsOK(self):
        """Check the user has supplied all the necessary parameters."""

        if not self.iFilePat:
            m = '%s ==> no input file pattern to match was given. Abandoning.'\
                % self.__class__.__name__
            self.logger.error(m)
            return (False, m)

        return (True, '')

    def getInFiles(self):
        """Find the input files with the  user supplied file name pattern"""

        matches = [m for m in glob.glob(self.iFilePat) if os.path.isfile(m)]
        if not matches:
            m = '%s => No matches found for the input file pattern: %s\n' % \
                (self.__class__.__name__, self.iFilePat)
            self.logger.warning(m)
        return matches

    def run(self):
        """Check the user has supplied necessary parameters. For files
        with names matching the user supplied file name pattern,
        store any lines mathcing the user supplied regexes.
        Print out a file of results containing name
        of file checked, the regexes checked for and the correponding lines
        found.

        Return 1 if any matching file has any line matching a veto pattern.
        Return 0 otherwise."""

        iFiles = self.getInFiles()

        for iFile in iFiles:

            self.cmd = 'simCoreFileGrepperScript.py '
            if self.fPatterns != "''":
                self.cmd += '-f %s ' % self.fPatterns
            if self.vPatterns != "''":
                self.cmd += '-v %s ' % self.vPatterns
            if self.mPatterns != "''":
                self.cmd += '-m %s ' % self.mPatterns

            self.cmd += iFile

            ExeRunnerBase.run(self)
