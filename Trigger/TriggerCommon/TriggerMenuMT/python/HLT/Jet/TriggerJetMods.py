# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from JetRecConfig.JetDefinition import JetModifier

########################################################################
# Special modifier setups used by jet trigger, but not in offline
# We just extend the stdJetModifiers from the standard config.


# Many JetMoment tools need to know the name of the container they operate on.
# We set the function below as the 'JetContainer' property so the config system
# can assign the right name to the c++ tool.
def _jetname(jetdef,modspec):
    return jetdef.fullname()

from JetRecConfig.StandardJetMods import stdJetModifiers
stdJetModifiers.update(

    # No need for the special momentum scales, just copy the basic four-vec
    # to "DetectorEtaPhi", because we're not doing origin correction
    # and certainly not with the offline collection names    
    ConstitFourMom_copy  = JetModifier("JetConstitFourMomTool", "constitfourmom_copy",
                                       JetScaleNames = ["DetectorEtaPhi"],
                                       AltConstitColls = [""],
                                       AltConstitScales = [0],
                                       AltJetScales = ["JetConstitScaleMomentum"]                                       
                                       ),
    Cleaning = JetModifier("JetCleaningTool","jetcleaning_{modspec}",
                           # This allows to set the modifier using a string like
                           # "Cleaning:CLEAN_LEVEL" as defined in JetCleaningTool
                           # (example "Cleaning:LooseBad")
                           CutLevel=lambda _, modspec: str(modspec),
                           prereqs=[f"mod:{mod}" for mod in ['CaloQuality']],
                           JetContainer = _jetname
    ),
)
