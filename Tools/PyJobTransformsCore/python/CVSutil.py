# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def CVSkeywords( listOfKeyWords ):
    """Take as input a list of strings containing CVS keywords of the form: \"$<name>:<value>$\"
    It will return a dictionary with <name>,<value> as key,value pairs.
    See cvs manual for possible keywords and their meaning."""
    kwDict = {}
    for kw in listOfKeyWords:
        # CVS keywords are embedded between 2 '$' signs
        dollar1 = kw.find('$')
        dollar2 = kw.find('$',dollar1+1)
        if dollar1 == -1 or dollar2 == -1:
            print "WARNING: %s is not a CVS keyword (it should have 2 '$' signs)" % kw
            continue
        # get part in between the 2 '$' signs
        cvskw = kw[dollar1+1:dollar2]
        # split name and value
        value = ''
        colon = cvskw.find(':')
        if colon == -1:
            # no value. Full string is name
            name = cvskw.strip()
        else:
            # name is part before colon
            name = cvskw[:colon].strip()
            # value is part after colon
            if colon + 1 < len(cvskw):
                value = cvskw[colon+1:].strip()
        if not name:
            print "WARNING: \"%s\" is not a CVS keyword (it should have a name after the first $" % kw
            continue
        kwDict[name] = value

    return kwDict


# Apply it to this module
CVSkeywordsMap = CVSkeywords( ["$Id: CVSutil.py,v 1.2 2009-01-29 12:04:16 ctan Exp $" ,
                               "$Name: not supported by cvs2svn $" ,
                               "$Revision: 285339 $"] )

__version__ = CVSkeywordsMap["Revision"]
__author__ = "clat@hep.ph.bham.ac.uk"

del CVSkeywordsMap
