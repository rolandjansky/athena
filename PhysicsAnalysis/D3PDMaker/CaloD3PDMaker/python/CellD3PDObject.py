# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject

CellD3PDObject = make_SG_D3PDObject( "CaloCellContainer", "AllCalo", "cells_",
                                     "CellD3PDObject" )

CellD3PDObject.defineBlock( 0, 'Kinematics',
                            CaloD3PDMaker.CellFillerTool )
