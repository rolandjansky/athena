## JobOption fragment for Herwig 7.1.3
assert hasattr(genSeq, "Herwig7")

Herwig7Config.add_commands("""
cd /Herwig/Shower
do ShowerHandler:AddVariation MUR2_MUF2_SHOWER_HARD     2 2     Hard
do ShowerHandler:AddVariation MUR2_MUF1_SHOWER_HARD     2 1     Hard
do ShowerHandler:AddVariation MUR1_MUF2_SHOWER_HARD     1 2     Hard
do ShowerHandler:AddVariation MUR1_MUF0.5_SHOWER_HARD   1 0.5   Hard
do ShowerHandler:AddVariation MUR0.5_MUF1_SHOWER_HARD   0.5 1   Hard
do ShowerHandler:AddVariation MUR0.5_MUF0.5_SHOWER_HARD 0.5 0.5 Hard
do ShowerHandler:AddVariation MUR2_MUF2_SHOWER_SEC      2 2     Secondary
do ShowerHandler:AddVariation MUR2_MUF1_SHOWER_SEC      2 1     Secondary
do ShowerHandler:AddVariation MUR1_MUF2_SHOWER_SEC      1 2     Secondary
do ShowerHandler:AddVariation MUR1_MUF0.5_SHOWER_SEC    1 0.5   Secondary
do ShowerHandler:AddVariation MUR0.5_MUF1_SHOWER_SEC    0.5 1   Secondary
do ShowerHandler:AddVariation MUR0.5_MUF0.5_SHOWER_SEC  0.5 0.5 Secondary
set SplittingGenerator:Detuning 2.0
""")
