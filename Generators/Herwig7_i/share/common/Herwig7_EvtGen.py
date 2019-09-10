## Run EvtGen afterburner on top of Pythia 8
assert hasattr(genSeq, "Herwig7")
include("EvtGen_i/EvtGen_Fragment.py")
evgenConfig.auxfiles += ['HerwigppInclusiveP8.pdt']
genSeq.EvtInclusiveDecay.pdtFile = "HerwigppInclusiveP8.pdt"

Herwig7Config.add_commands("""
# Quick "fix" to the mismatch between Herwig 7 and EvtGen of the masses below
set /Herwig/Particles/B'_c1+:NominalMass 7.3
set /Herwig/Particles/B'_c1-:NominalMass 7.3
set /Herwig/Particles/B_c1+:NominalMass 7.3
set /Herwig/Particles/B_c1-:NominalMass 7.3
set /Herwig/Particles/B_c2+:NominalMass 7.35
set /Herwig/Particles/B_c2-:NominalMass 7.35
set /Herwig/Particles/B*_c0+:NominalMass 7.25
set /Herwig/Particles/B*_c0-:NominalMass 7.25
set /Herwig/Particles/B_c+:NominalMass 6.277
set /Herwig/Particles/B_c-:NominalMass 6.277
#
# This is a complete kludge.  Herwig7 has switched the mapping of names and pdgids for the D'(s1) and the D(s1)
# That means EvtGen misinterprets which particle it should be decaying, since the PDGID is what is written
# in the record.  So, we'll set things up to change the masses so that the HEPMC record lis OK.  That means
# we want:
#  Mass: 2.45960  ID: 20433 (Pythia8 calls this the D*_1s+ , EvtGen calls it D_s1-.  20433 is what Herwig calls 
#                 D'_s1+ and orginally gave it a mass of 2.53535)
#  Mass: 2.53512  ID: 10433 (Pythia8 calls this the D_1s+ , EvtGen calls it D'_s1+.  10433 is what Herwig calls
#                 D_s1+ and orginally gave it a mass of 2.4589
#
#  Since EvtGen will redecay everything anyway, we'll make these particles stable in Herwig
#
set /Herwig/Particles/D'_s1+:NominalMass 2.4595000e+00
set /Herwig/Particles/D'_s1+:Width 0.001
set /Herwig/Particles/D'_s1+:WidthCut 0.01
set /Herwig/Particles/D'_s1+:Width_generator:Initialize Yes
set /Herwig/Particles/D'_s1+:Mass_generator:Initialize Yes
set /Herwig/Particles/D'_s1-:NominalMass 2.4595000e+00
set /Herwig/Particles/D'_s1-:Width 0.001
set /Herwig/Particles/D'_s1-:WidthCut 0.01
set /Herwig/Particles/D'_s1-:Width_generator:Initialize Yes
set /Herwig/Particles/D'_s1-:Mass_generator:Initialize Yes
set /Herwig/Particles/D_s1+:NominalMass 2.5352800e+00
set /Herwig/Particles/D_s1+:Width 0.001
set /Herwig/Particles/D_s1+:WidthCut 0.01
set /Herwig/Particles/D_s1+:Width_generator:Initialize Yes
set /Herwig/Particles/D_s1+:Mass_generator:Initialize Yes
set /Herwig/Particles/D_s1-:NominalMass 2.5352800e+00
set /Herwig/Particles/D_s1+:Width 0.001
set /Herwig/Particles/D_s1+:WidthCut 0.01
set /Herwig/Particles/D_s1+:Width_generator:Initialize Yes
set /Herwig/Particles/D_s1+:Mass_generator:Initialize Yes
set /Herwig/Particles/D'_s1-:Stable Stable
set /Herwig/Particles/D'_s1+:Stable Stable
set /Herwig/Particles/D_s1-:Stable Stable
set /Herwig/Particles/D_s1+:Stable Stable

""")

