evgenConfig.tune = "NNPDF3.0 NNLO"

import os 
if os.environ["SHERPAVER"].startswith('3.'):

  ## Nominal PDF settings
  genSeq.Sherpa_i.BaseFragment += """
  PDF_LIBRARY: LHAPDFSherpa
  USE_PDF_ALPHAS: 1
  PDF_SET: NNPDF30_nnlo_as_0118
  """

  ## Enable scale and PDF variations by default
  genSeq.Sherpa_i.BaseFragment += """
  HEPMC_USE_NAMED_WEIGHTS: true
  CSS_REWEIGHT: true
  REWEIGHT_SPLITTING_PDF_SCALES: true
  REWEIGHT_SPLITTING_ALPHAS_SCALES: true
  CSS_REWEIGHT_SCALE_CUTOFF: 5.0
  HEPMC_INCLUDE_ME_ONLY_VARIATIONS: true
  VARIATIONS_INCLUDE_CV: true
  VARIATIONS:
    - ScaleFactors:
        MuF2: 0.25
        MuR2: 0.25
    - ScaleFactors:
        MuF2: 1.0
        MuR2: 0.25
    - ScaleFactors:
        MuF2: 0.25
        MuR2: 1.0
    - ScaleFactors:
        MuF2: 1.0
        MuR2: 1.0
    - ScaleFactors:
        MuF2: 4.0
        MuR2: 1.0
    - ScaleFactors:
        MuF2: 1.0
        MuR2: 4.0
    - ScaleFactors:
        MuF2: 4.0
        MuR2: 4.0
    - PDF: NNPDF30_nnlo_as_0118_hessian*
    - PDF: NNPDF30_nnlo_as_0117
    - PDF: NNPDF30_nnlo_as_0119
    - PDF: MSHT20nnlo_as118
    - PDF: CT18NNLO_as_0118
    - PDF: PDF4LHC15_nnlo_30_pdfas*
    - PDF: NNPDF31_nnlo_as_0118_hessian
  """
else:
  genSeq.Sherpa_i.Parameters += [
      "PDF_LIBRARY=LHAPDFSherpa",
      "USE_PDF_ALPHAS=1",
      "PDF_SET=NNPDF30_nnlo_as_0118_hessian",
      "PDF_VARIATIONS=NNPDF30_nnlo_as_0118_hessian[all] NNPDF30_nnlo_as_0117 NNPDF30_nnlo_as_0119 MSHT20nnlo_as118 CT18NNLO_as_0118 PDF4LHC15_nnlo_30_pdfas[all] NNPDF31_nnlo_as_0118_hessian",
      ]
