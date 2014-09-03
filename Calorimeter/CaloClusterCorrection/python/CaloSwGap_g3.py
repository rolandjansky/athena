# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloSwGap_g3.py,v 1.1 2006-11-10 03:47:27 ssnyder Exp $
#
# File: CaloClusterCorrection/python/CaloSwGap_g3.py
# Created: Nov 2006, sss
# Purpose: Gap correction, original G3 version.
#
# This is the original G3-based gap correction, translated from the
# original fortran version.
#


#############################################################################


class CaloSwGap_g3_parms:
    etamin_crack = 1.35
    etamax_crack = 1.6

    scint_weight = [
        1.65450, 1.44040, 1.22855, 1.38503, 1.35972,
        1.19340, 1.32854, 1.10760, 1.23266, 0.78814
        ]

    correction = [
        0.993927, 1.004908, 1.032988, 1.029174, 1.050922,
        1.133175, 0.968635, 0.990108, 1.007681, 1.013702
        ]
