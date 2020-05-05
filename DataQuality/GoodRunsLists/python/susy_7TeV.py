# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from GoodRunsLists.GoodRunsListConfig import GoodRunsListConfig

class susy_7TeV(GoodRunsListConfig):
  def __init__(self):
    GoodRunsListConfig.__init__(self)

    ## name in GRLB object, and below the xml filename
    self.name            = "susy_7TeV"
    self.listname        = "susy_7TeV.xml"

    ## Specify each selection of dq flags here.
    ## The query is interpreted as the logical AND of all elements in the array.

    ## See http://atlas-runquery.cern.ch for more user examples.
    self.querydict['run']  = ["152166+"]
    self.querydict['ptag'] = ["data10_7TeV"]

    dqflagsquery = [    'ATLGL LBSUMM#DetStatus-v03-repro04-01 g',
                        'L1CTP LBSUMM#DetStatus-v03-repro04-01 g',
                        'CP_MU_MMUIDCB LBSUMM#DetStatus-v03-repro04-01 g',
                        'cp_eg_electron_barrel LBSUMM#DetStatus-v03-repro04-01 g',
                        'cp_eg_electron_endcap LBSUMM#DetStatus-v03-repro04-01 g',
                        #'cp_jet_jetb  g',
                        #'cp_jet_jetea  g',
                        #'cp_jet_jetec  g',
                        #'cp_met_metcalo  g',
                        #'cp_met_metmuon  g',
                        #'trmet  g',
                        #'trjet  g',
                        #'trmuo  g',
                        #'trele  g',
                        'bs LBSUMM#DetStatus-v03-repro04-01 g',
                        'lumi LBSUMM#DetStatus-v03-repro04-01 g',
                   ] 

    ## Set the dqflags query
    self.querydict['dq'] = dqflagsquery

    ## lhc beam settings
    self.querydict['lhc']  = [ "stablebeams T",
                               "beamenergy 3400+",
                             ]

    ## ready for physics flag
    self.querydict['ready'] = [ '1' ]

