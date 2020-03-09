from JetRecConfig.JetDefinition import JetModifier

########################################################################
# Special modifier setups used by jet trigger, but not in offline

# No need for the special momentum scales, just copy the basic four-vec
# to "DetectorEtaPhi", because we're not doing origin correction
# and certainly not with the offline collection names
from JetMomentTools import JetMomentToolsConf
def getConstitFourMomTool_copy():
    cfourmom = JetMomentToolsConf.JetConstitFourMomTool("constitfourmom_copy",
                                                        JetScaleNames = ["DetectorEtaPhi"],
                                                        AltConstitColls = [""],
                                                        AltConstitScales = [0],
                                                        AltJetScales = ["JetConstitScaleMomentum"])
    return cfourmom

ConstitFourMom_copy = JetModifier("JetConstitFourMomTool", "constitfourmom_copy",
                                   helperfn=getConstitFourMomTool_copy)
