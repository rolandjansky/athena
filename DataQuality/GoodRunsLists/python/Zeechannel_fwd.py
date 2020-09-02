# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class Zeechannel_fwd(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "Zeechannel_fwd"
    self.listname        = "Zeechannel_fwd.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## All selection criteria at: Database/CoolRunQuery/python/AtlRunQueryParser.py
    ## possible dq flag colors : 'n.a.', 'unknown', 'green', 'yellow', 'red'
    ## possible detectors (incomplete): pix,sct,em,til,lar,any,... 

    ## See http://atlas-runquery.cern.ch for more user examples.
    self.querydict['run']  = ["152166+"]
    self.querydict['ptag'] = ["data10_7TeV"]

    ## See http://atlas-runquery.cern.ch for more user examples.
    dqflagsquery = [    'ATLGL LBSUMM#DetStatus-v03-repro04-01 g',
                        'L1CTP LBSUMM#DetStatus-v03-repro04-01 g',
                        'ATLSOL LBSUMM#DetStatus-v03-repro04-01 g',
                        'L1CAL LBSUMM#DetStatus-v03-repro04-01 g',
                        'cp_eg_electron_endcap LBSUMM#DetStatus-v03-repro04-01 g',
                        'cp_eg_electron_barrel LBSUMM#DetStatus-v03-repro04-01 g',
                        'cp_eg_electron_forward LBSUMM#DetStatus-v03-repro04-01 g',
                   ]

    ## Set the dqflags query
    self.querydict['dq'] = dqflagsquery

    ## magnete currents
    self.querydict['mag']  = [ 's > 6000' ]

    ## lhc beam settings
    self.querydict['lhc']  = [ "stablebeams T",
                               "beamenergy 3400+",
                             ]

    ## ready for physics flag
    self.querydict['ready'] = [ '1' ]

