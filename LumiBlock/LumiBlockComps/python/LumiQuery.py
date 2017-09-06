# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# module LumiQuery
""" Simple module for interaction with the luminosity metadata in a file. """

global LumiRangeList
LumiRangeList = []

class PyRunLumiP1(long):
    """ Python (run,block)/(run,event) type object based on the LumiBlockRange_p1 persistency format. 

    LumiBlockRanges are stored as encoded run and (luminosity) block
    numbers.  Since the conventional run-based accessors for IOVTime
    are run() and event(), event() is provided *as a synonym* for block.
   
    This class is initialized with an IOVTime *as stored* in the 
    persistent object LumiBlockRange_p1 (Note, it does not work on the
    transient object!)

    Warning: although PyRunLumiP1 inherits from long, addition and
    subtraction are not well defined and should be treated with care!
    Nevertheless, *incrementing* a PyRunLumiP1 gives a basically intuitive
    result.) 

    """
    def run(self):
        return int(self >> 32)
    def block(self):
        return int(self & 0xFFFFFFFF)
    def event(self):
        return self.block()
    def __str__(self):
        return 'R='+ str(self.run()) + '; LB=' + str(self.event())
    def __repr__(self):
        return self.__hex__().__repr__() + '->(' + repr(self.run()) + ':' + repr(self.event()) + ')'    



def ListFromFile(filename="FewEventsLumi.pool.root", label = "LumiBlockCollection_p1_LumiBlocks", metadatatree="MetaData"):
    """ Returns a list of RunLumiBlock ranges (stored as python tuples of type PyRunLumiP1) from the named file.
    
    The file should have metadata stored internally as a ROOT tree
    (named 'Stream' by default). It can be a POOL file or other generic
    ntuple file.

    Note: This function does not access the MetaDataStore.  Please use
    a LumiBlock AlgTool for that.
    """    

    import ROOT
    a = ROOT.TFile.Open(filename)
    global LumiRangeList
    LumiRangeList = []
    try:
        T = a.Get(metadatatree)
        T.SetBranchStatus("*",0);
        T.SetBranchStatus(label,1)
        T.SetBranchStatus("vector<LumiBlockRange_p1>",1)
        T.SetBranchStatus("vector<LumiBlockRange_p1>.m_start",1)
        T.SetBranchStatus("vector<LumiBlockRange_p1>.m_stop",1)
        T.GetEntry(0)
    except AttributeError:
        print "There is no tree named", metadatatree, "in file" ,filename
        if a.__repr__() != 'None':
            print 8*'--', "Contents:", 8*'--'
            a.ls()
        return

    ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ###
    # Read the tree.
    # This must be updated if the storage method changes.
    ### ### ### ### ### ### ### ### ### ### ### ### ### ### ### ###
    try:    
        iterRange = getattr( T, label ).size() #T.GetLeaf("vector<LumiBlockRange_p1>.m_start").GetLen() 
    except AttributeError, a:
        print a
        print "The tree", metadatatree, "in file" ,filename, "has no vector<LumiBlockRange_p1>.m_start branch, so I don't understand its format."
        return    
    for i in xrange(iterRange): 
        LumiRangeList += [ (PyRunLumiP1( getattr( T, label )[i].m_start                           ),PyRunLumiP1( getattr( T, label )[i].m_stop                           ))]
    return [ x for x in LumiRangeList ]


def PrintList():
    """ An example function. It prints the list returned by ListFromFile. """
    global LumiRangeList
    for (x,y) in LumiRangeList: print x, 'to', y


if __name__ == '__main__':
    import sys
    ll1 = ListFromFile( sys.argv[1], "LumiBlockCollection_p1_LumiBlocks", "MetaData" )
    ll2 = ListFromFile( sys.argv[1], "LumiBlockCollection_p1_IncompleteLumiBlocks", "MetaData" )
