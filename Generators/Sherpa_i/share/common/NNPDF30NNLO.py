evgenConfig.tune = "NNPDF3.0 NNLO"

genSeq.Sherpa_i.Parameters += [
    "PDF_LIBRARY=LHAPDFSherpa",
    "USE_PDF_ALPHAS=1",
    "PDF_SET=NNPDF30_nnlo_as_0118_hessian",
    "PDF_VARIATIONS=NNPDF30_nnlo_as_0118_hessian[all] NNPDF30_nnlo_as_0117 NNPDF30_nnlo_as_0119 MSHT20nnlo_as118 CT18NNLO_as_0118 PDF4LHC15_nnlo_30_pdfas[all] NNPDF31_nnlo_as_0118_hessian",
    ]
