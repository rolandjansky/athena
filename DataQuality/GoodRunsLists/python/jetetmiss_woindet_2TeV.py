# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class jetetmiss_woindet_2TeV(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "jetetmiss_woindet_2TeV"
    self.listname        = "jetetmiss_woindet_2TeV.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## All selection criteria at: Database/CoolRunQuery/python/AtlRunQueryParser.py
    ## possible dq flag colors : 'n.a.', 'unknown', 'green', 'yellow', 'red'
    ## possible detectors (incomplete): pix,sct,em,til,lar,any,... 

    ## See http://atlas-runquery.cern.ch for more user examples.

    self.querydict['run']  = ["140541+"]
    #self.querydict['ptag'] = ["data09_2TeV"]
    self.querydict['st']   = ["physics_MinBias"]

    dqflagsquery = [ 'ATLGL  LBSUMM#DetStatusLBSUMM-December09-01 g',
                     'L1CTP  LBSUMM#DetStatusLBSUMM-December09-01 g',
                     'atlsol LBSUMM#DetStatusLBSUMM-December09-01 g',
                     'lar    LBSUMM#DetStatusLBSUMM-December09-01 g',
                     'tile   LBSUMM#DetStatusLBSUMM-December09-01 g',
                     ##'jetb yellow+  ',
                     ##'jetea yellow+ ',
                     ##'jetec yellow+ ', 
                     ##'met yellow+   ',
                   ]

    self.querydict['lhc']  = ["beamenergy 1170+"]

    ## Set the dqflags query
    self.querydict['dq'] = dqflagsquery

