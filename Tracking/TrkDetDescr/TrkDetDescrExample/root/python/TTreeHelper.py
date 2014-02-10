"""
TreePlotter.py

Created by Andreas Salzburger on 2009-04-16.
Copyright (c) 2009 __MyCompanyName__. All rights reserved.
"""

import sys
import os
from ROOT import TFile

class TTreeSelector(object):
    """TreePlotter helper class in python"""
    def __init__(self, *args):
        super(TTreeSelector, self).__init__()
        # open the file
        try :
            self.file = TFile.Open(args[0])
        except IndexError :
            print '[!] You need to specify a file name in the argument list'
        # get the hashlist and entries
        self.hashlist = self.file.GetListOfKeys()
        self.entries  = self.hashlist.GetEntries()
        print '[>] File [',args[0],'] sucessfully loaded'
        
    def loadTrees(self, arg):                
        """load Trees method: loads trees that match one of the arguments in name"""
        treedict = {}
        for itree in range(0,self.entries):
            treename = self.hashlist.At(itree).GetName()
            if treename.find(arg) >= 0 :
                tree = self.file.Get(treename)
                treedict[treename] = tree
        print '[>] ',len(treedict),'Trees loaded matching the selection:', arg
        return treedict
            
        
        
