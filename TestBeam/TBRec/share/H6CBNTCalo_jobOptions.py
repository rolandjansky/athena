from TBRec.TBRecConf import CBNT_CaloH6
# CBNT_H6_root_det, HEC
HEC_root = CBNT_CaloH6( "HEC_root" )
HEC_root.Det_Type="HEC"
HEC_root.Max_Chan=242
CBNT_AthenaAware += HEC_root

# CBNT_H6_root_det, EMEC
EMEC_root = CBNT_CaloH6( "EMEC_root" )
EMEC_root.Det_Type="EMEC"
EMEC_root.Max_Chan=113
CBNT_AthenaAware += EMEC_root

# CBNT_H6_root_det, FCAL
FCAL_root = CBNT_CaloH6( "FCAL_root" )
FCAL_root.Det_Type="FCAL"
FCAL_root.Max_Chan=347
CBNT_AthenaAware += FCAL_root
