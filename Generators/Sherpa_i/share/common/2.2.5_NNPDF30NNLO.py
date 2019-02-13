## Sherpa config with NNPDF3.0 NNLO PDF
include("Sherpa_i/2.2.5_Base_Fragment.py")

evgenConfig.tune = "NNPDF3.0 NNLO"

genSeq.Sherpa_i.Parameters += [
    "PDF_LIBRARY=LHAPDFSherpa",
    "USE_PDF_ALPHAS=1",
    "PDF_SET=NNPDF30_nnlo_as_0118",
    "PDF_VARIATIONS=NNPDF30_nnlo_as_0118[all] NNPDF30_nnlo_as_0117 NNPDF30_nnlo_as_0119 MMHT2014nnlo68cl CT14nnlo PDF4LHC15_nnlo_30_pdfas[all]",
    ]
