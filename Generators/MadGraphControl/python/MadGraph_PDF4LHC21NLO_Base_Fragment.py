# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import MadGraphControl.MadGraphUtils
MadGraphControl.MadGraphUtils.MADGRAPH_PDFSETTING={
    'central_pdf':93300, # PDF4LHC21
    'pdf_variations':[93300,260000], # PDF4LHC21 and NNPDF30_nlo_as_0118 PDF4LHC21
    'alternative_pdfs':[266000,265000,303200,27400,27100,14000,14400,304400,304200,331500,331100,14200,14300,14100], # NNPDF30_nnlo_as_0118_hessian, MSHT20nnlo_as118, MSHT20nlo_as118, CT18NNLO, CT18NLO, NNPDF31_nnlo_as_0118_hessian, NNPDF31_nlo_as_0118_hessian, NNPDF40_nnlo_as_01180_hessian, NNPDF40_nlo_as_01180, CT18ANNLO, CT18XNNLO, CT18ZNNLO
    'scale_variations':[0.5,1.,2.],
}
