# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class minbias_solon_2TeV(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "minbias_solon_2TeV"
    self.listname        = "minbias_solon_2TeV.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## All selection criteria at: Database/CoolRunQuery/python/AtlRunQueryParser.py
    ## possible dq flag colors : 'n.a.', 'unknown', 'green', 'yellow', 'red'
    ## possible detectors (incomplete): pix,sct,em,til,lar,any,... 

    ## See http://atlas-runquery.cern.ch for more user examples.

    self.querydict['run']  = ["140541+"]
    #self.querydict['ptag'] = ["data09_2TeV"] # one 2.4 TeV run in data_900GeV project tag
    self.querydict['st']   = ["physics_MinBias"]

    dqflagsquery = [ 'ATLGL  LBSUMM#DetStatusLBSUMM-December09-01 g',
                     'L1CTP  LBSUMM#DetStatusLBSUMM-December09-01 g',
                     'atlsol LBSUMM#DetStatusLBSUMM-December09-01 g',
                   ]

    ## Set the dqflags query
    self.querydict['dq'] = dqflagsquery

    ## lhc settings
    self.querydict['lhc']  = ["beamenergy 1170+"] # no stable beams for 2.4 TeV runs

