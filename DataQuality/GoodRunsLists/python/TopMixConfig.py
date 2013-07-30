# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class TopMixConfig(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "TopMixConfig"
    self.listname        = "TopMixConfig.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## All selection criteria at: Database/CoolRunQuery/python/AtlRunQueryParser.py
    ## possible dq flag colors : 'n.a.', 'unknown', 'green', 'yellow', 'red'
    ## possible detectors (incomplete): pix,sct,em,til,lar,any 

    ## See http://atlas-runquery.cern.ch for more user examples.
    ## Example: "f r 108175 and db MC and dq pix LBSUMM g and dq EMBA lbsumm g and ctag DetStatusLBSumm-TopMix1 / sh r and dq LBSUMM / nodef"

    ## Reset default settings (= cosmics data related)
    self.querydict = {}
    self.querydict['run']  = ['108175']
    self.querydict['db']   = ['MC']
    self.querydict['ctag'] = ['DetStatusLBSumm-TopMix1']

    ## Set the dqflags query
    self.querydict['dq']   = [ 'sct LBSUMM g',
                             ]

