# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class CosmicsLumiConfig(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "CosmicsLumiConfig"
    self.listname        = "CosmicsLumiConfig.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## All selection criteria at: Database/CoolRunQuery/python/AtlRunQueryParser.py
    ## possible dq flag colors : 'n.a.', 'unknown', 'green', 'yellow', 'red'
    ## possible detectors (incomplete): pix,sct,em,til,lar,any,... 

    ## See http://atlas-runquery.cern.ch for more user examples.

    dqflagsquery = [ #'em   SHIFTOFL g',
                     #'pix  SHIFTOFL g',
                   ]

    ## Set the dqflags query
    self.querydict['dq'] = dqflagsquery

