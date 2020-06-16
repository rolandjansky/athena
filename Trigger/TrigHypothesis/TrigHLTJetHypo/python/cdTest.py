# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from testChainDictMaker import testChainDictMaker


from chainDict2jetLabel import chainDict2jetLabel

cds = testChainDictMaker()
for cd in cds:
    print(cd[0])
    print(chainDict2jetLabel(cd[1]))
