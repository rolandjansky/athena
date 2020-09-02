# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Official collection of GRL maintained and produced by DQ group.
## Last update: 2009-10-21, by Max Baak

## Add new GRL configurations bottom of the array
## Eg. 'GoodRunsLists.ExampleConfig' means: package = GoodRunsLists, GRL class = ExampleConfig
class OfficialGRLs:
  def __init__(self):

    self.collection = [
                        'GoodRunsLists.jetetmiss_windet_900GeV',
                        'GoodRunsLists.jetetmiss_woindet_900GeV',
                        'GoodRunsLists.jetetmiss_woindet_2TeV',
                        'GoodRunsLists.jetetmiss_jetmetok_woindet_2TeV',
                        'GoodRunsLists.jetetmiss_jetmetok_woindet_900GeV',
                        'GoodRunsLists.jetetmiss_jetmetok_windet_900GeV',
                        'GoodRunsLists.minbias_2TeV',
                        'GoodRunsLists.minbias_900GeV',
                        'GoodRunsLists.minbias_solon_900GeV',
                        'GoodRunsLists.minbias_solon_windet_900GeV',
                        'GoodRunsLists.minbias_solon_2TeV',
                        'GoodRunsLists.minbias_stable_900GeV',
                        'GoodRunsLists.minbias_windet_900GeV',
                      ]
