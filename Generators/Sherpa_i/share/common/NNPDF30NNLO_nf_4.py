evgenConfig.tune = "NNPDF3.0 NNLO"

genSeq.Sherpa_i.Parameters += [
    "PDF_LIBRARY=LHAPDFSherpa",
    "USE_PDF_ALPHAS=1",
    "PDF_SET=NNPDF30_nnlo_as_0118_nf_4",
    "PDF_VARIATIONS=NNPDF30_nnlo_as_0118_nf_4[all] NNPDF30_nnlo_as_0117_nf_4 NNPDF30_nnlo_as_0119_nf_4 MMHT2014nnlo68cl_nf4 CT14nnlo_NF4 PDF4LHC15_nlo_nf4_30[all]",
    ]
