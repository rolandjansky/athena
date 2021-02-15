# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: CaloD3PD.py 521798 2012-10-16 21:37:02Z ssnyder $
#
# @file CaloD3PDMaker/python/CaloD3PD.py
# @author sss, from code from Hong Ma.
# @date Oct, 2012
# @brief Construct a calorimeter D3PD.
#


from EventCommonD3PDMaker.EventInfoD3PDObject     import EventInfoD3PDObject
from CaloD3PDMaker.CaloCellD3PDObject             import SelCaloCellD3PDObject
from CaloSysD3PDMaker.CaloInfoD3PDObject          import CaloInfoD3PDObject
from CaloD3PDMaker.xAODClusterD3PDObject          import xAODClusterD3PDObject
from CaloSysD3PDMaker.LArDigitD3PDObject          import LArDigitD3PDObject
from D3PDMakerCoreComps.resolveSGKey              import testSGKey


def CaloD3PD (alg = None,
              file = 'calo.root',
              tuplename = 'calo'):
    # Core algorithm
    # By default the user of this function should have created an algorithm
    # already.  But for backwards compatibility let the function create its own
    # algorithm if needed...
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )
    

        alg += EventInfoD3PDObject (10)

        # alg += AllCaloCellD3PDObject (10)
        alg += SelCaloCellD3PDObject (10)
        # alg += EMCellD3PDObject (10)
        # alg += HECCellD3PDObject (10)
        # alg += FCALCellD3PDObject (10)
        # alg += TileCellD3PDObject (10)

        alg += CaloInfoD3PDObject (10)

        alg += xAODClusterD3PDObject (10)

        lardigit_sgkey = 'LArDigitContainer_Thinned'
        if testSGKey ('LArDigitContainer', lardigit_sgkey):
            alg += LArDigitD3PDObject (2, sgkey = lardigit_sgkey)

        return alg
    
