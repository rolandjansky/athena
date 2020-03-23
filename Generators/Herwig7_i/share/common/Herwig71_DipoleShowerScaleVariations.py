## JobOption fragment for Herwig 7.1.3
assert hasattr(genSeq, "Herwig7")

Herwig7Config.add_commands("""
cd /Herwig/DipoleShower
do DipoleShowerHandler:AddVariation MUR2_MUF2_SHOWER_HARD     2 2     Hard
do DipoleShowerHandler:AddVariation MUR2_MUF1_SHOWER_HARD     2 1     Hard
do DipoleShowerHandler:AddVariation MUR1_MUF2_SHOWER_HARD     1 2     Hard
do DipoleShowerHandler:AddVariation MUR1_MUF0.5_SHOWER_HARD   1 0.5   Hard
do DipoleShowerHandler:AddVariation MUR0.5_MUF1_SHOWER_HARD   0.5 1   Hard
do DipoleShowerHandler:AddVariation MUR0.5_MUF0.5_SHOWER_HARD 0.5 0.5 Hard
do DipoleShowerHandler:AddVariation MUR2_MUF2_SHOWER_SEC      2 2     Secondary
do DipoleShowerHandler:AddVariation MUR2_MUF1_SHOWER_SEC      2 1     Secondary
do DipoleShowerHandler:AddVariation MUR1_MUF2_SHOWER_SEC      1 2     Secondary
do DipoleShowerHandler:AddVariation MUR1_MUF0.5_SHOWER_SEC    1 0.5   Secondary
do DipoleShowerHandler:AddVariation MUR0.5_MUF1_SHOWER_SEC    0.5 1   Secondary
do DipoleShowerHandler:AddVariation MUR0.5_MUF0.5_SHOWER_SEC  0.5 0.5 Secondary
set DipoleShowerHandler:Detuning 2.0
""")
