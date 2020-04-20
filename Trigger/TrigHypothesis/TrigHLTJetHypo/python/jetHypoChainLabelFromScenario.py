# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# test to obtain a jet hypo label from a jet hypo scenario

from __future__ import print_function


from chainDict2jetLabel import chainDict2jetLabel

# scenarios live in HLTMenuConfig/Menu/SignaturesDict

scenarios = ['dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass',
           'dijetSEP80j1etSEP0j1eta240SEP60j2etSEP320j2eta490SEP700djmass']

# skeleton chainDict

for sc in scenarios:
    chainDict = {
        'chainParts': [{'signature': 'Jet',
                        'hypoScenario': sc}]
}

    print (sc)
    print (chainDict2jetLabel(chainDict))


