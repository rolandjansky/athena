from JetRec.JetRecStandardToolManager import jtm
import DerivationFrameworkJetEtMiss.JetCommon as JetCommon
import DerivationFrameworkJetEtMiss.ExtendedJetCommon as ExtendedJetCommon
from JetRec.JetRecConf import JetAlgorithm
# Create a logger for this stream
import AthenaCommon
from BTagging.BTaggingConfiguration import getConfiguration
ConfInst=getConfiguration()
logger = AthenaCommon.Logging.logging.getLogger("EXOT27Utils")

#####
# Here put the trigger lists that we depend on
# This is preferable to the trigger API because that selects many extra
# triggers. We are now outside of running so new triggers can be added!
#####
met_triggers = [
        "HLT_xe70_mht",
        "HLT_xe90_mht_L1XE50",
        "HLT_xe100_mht_L1XE50",
        "HLT_xe110_mht_L1XE50",
        "HLT_xe110_pufit_L1XE50",
        "HLT_xe110_pufit_L1XE55",
        "HLT_xe110_pufit_xe65_L1XE50",
        "HLT_xe110_pufit_xe70_L1XE50"
]
single_electron_triggers = [
        "HLT_e24_lhmedium_L1EM20VH",  # data 2015 only, no MC
        "HLT_e60_lhmedium",  # 2015
        "HLT_e120_lhloose",  # 2015
        "HLT_e24_lhtight_nod0_ivarloose",  # 2016 A, B-D3
        "HLT_e60_lhmedium_nod0",  # 2016 A, B-D3, D4-F, G-, 2017 B-, 2018
        "HLT_e60_medium",  # 2016 A, B-D3, D4-F, G-
        "HLT_e300_etcut",  # 2016 A, B-D3, D4-F, G-, 2017 B-, 2018
        "HLT_e140_lhloose_nod0",  # 2016 A, B-D3, D4-F, G-, 2017 B-, 2018
        "HLT_e26_lhtight_nod0_ivarloose"  # 2016 D4-F, G-, 2017 B-, 2018
]
single_muon_triggers = [
        "HLT_mu20_iloose_L1MU15",  # 2015
        "HLT_mu50",  # 2016 A, B-D3, D4-E, F-G2, G3-I3, I4-, 2017 B-, 2018
        "HLT_mu24_iloose_L1MU15",  # MC only
        "HLT_mu24_iloose",  # data 2016 A only, no MC
        "HLT_mu24_ivarloose",  # MC only -> this trigger was not included
        "HLT_mu24_ivarloose_L1MU15",  # data 2016 A only, no MC -> this trigger was not included
        "HLT_mu40",  # 2015, 2016 A
        "HLT_mu24_ivarmedium",  # 2016 B-D3, D4-E
        "HLT_mu24_imedium",  # 2016 B-D3, D4-E -> this trigger was not included
        "HLT_mu26_ivarmedium",  # 2016 D4-E, F-G2, G3-I3, I4-, 2017 B-, 2018
        "HLT_mu26_imedium",  # 2016 D4-E, F-G2 -> this trigger was not included
]
single_photon_triggers = [
        "HLT_g140_loose",
        "HLT_g300_etcut",
        "HLT_g120_loose",
        "HLT_g200_etcut"
]
