# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class all_7TeV(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "all_7TeV"
    self.listname        = "all_7TeV.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## All selection criteria at: Database/CoolRunQuery/python/AtlRunQueryParser.py
    ## possible dq flag colors : 'n.a.', 'unknown', 'green', 'yellow', 'red'
    ## possible detectors (incomplete): pix,sct,em,til,lar,any,... 

    ## See http://atlas-runquery.cern.ch for more user examples.
    self.querydict['run']  = ["152166+"]
    self.querydict['ptag'] = ["data10_7TeV"]
    self.querydict['st']   = ["physics_MinBias"]

    dqflagsquery = [ 'ATLGL      SHIFTOFL#HEAD   g',
                     #'L1CTP     SHIFTOFL#HEAD   g',
                     'atltor     SHIFTOFL#HEAD   g',
                     'atlsol     SHIFTOFL#HEAD   g',
                     'pix        SHIFTOFL#HEAD   g',
                     'sct        SHIFTOFL#HEAD   g',
                     'trtb,trte  SHIFTOFL#HEAD   y+',
                     'mdt        SHIFTOFL#HEAD   g',
                     'rpc        SHIFTOFL#HEAD   g',
                     'tgc        SHIFTOFL#HEAD   g',
                     'csc        SHIFTOFL#HEAD   g',
                     'lar        SHIFTOFL#HEAD   y+',
                     'tile       SHIFTOFL#HEAD   y+',
                   ]

    ## Set the dqflags query
    self.querydict['dq'] = dqflagsquery

    ## lhc beam settings
    self.querydict['lhc']  = ["stablebeams T"]

