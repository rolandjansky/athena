# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject

CaloInfoD3PDObject = make_SG_D3PDObject( "CaloCellContainer", "AllCalo", "ccc_",
                                         "CaloInfoD3PDObject" )

CaloInfoD3PDObject.defineBlock( 0, 'ALL',
                                CaloD3PDMaker.CaloInfoFillerTool,
                                PosNeg = 0 )

CaloInfoEtD3PDObject = make_SG_D3PDObject( "CaloCellContainer", "AllCalo", "cccEt_",
                                           "CaloInfoEtD3PDObject" )

CaloInfoEtD3PDObject.defineBlock( 0, 'AllEt',
                                  CaloD3PDMaker.CaloInfoFillerTool,
                                  prefix = 'Et_',
                                  PosNeg = 0,
                                  DoEt = 1 )

# if the energies are summed for each side 
CaloInfoEtD3PDObject.defineBlock( 0, 'Pos', 
                                  CaloD3PDMaker.CaloInfoFillerTool,
                                  prefix = 'p_',
                                  PosNeg = 1,
                                  DoEt = 1 )

CaloInfoEtD3PDObject.defineBlock( 0, 'Neg',
                                  CaloD3PDMaker.CaloInfoFillerTool,
                                  prefix = 'n_',
                                  PosNeg = -1,
                                  DoEt = 1 )
