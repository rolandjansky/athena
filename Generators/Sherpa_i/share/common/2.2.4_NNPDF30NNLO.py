## Sherpa config with NNPDF3.0 NNLO PDF
include("Sherpa_i/2.2.4_Base_Fragment.py")

## NNPDF3.0 NNLO is Sherpa's default PDF/tune, thus no need to set anything up
evgenConfig.tune = "NNPDF3.0 NNLO"
