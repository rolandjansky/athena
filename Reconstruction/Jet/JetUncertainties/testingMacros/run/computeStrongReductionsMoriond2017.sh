python makeStrongReductionTests.py --dir JES_2016/Moriond2017 --npars 3 --ref JES2016_AllNuisanceParameters.config --tmp generateConfigs/JES2016_SR_template.config --batch --redo-configs
python makeStrongReductionTests.py --dir JES_2016/Moriond2017 --npars 3 --ref JES2016_AllNuisanceParameters.config --tmp generateConfigs/JES2016_SR_template.config --batch --matrixStage --ntestsmatrices 300
python makeStrongReductionTests.py --dir JES_2016/Moriond2017 --npars 3 --ref JES2016_AllNuisanceParameters.config --tmp generateConfigs/JES2016_SR_template.config --batch --comparisonStage --ntestscomparisons 1000
# For investigating around a promising combination:
python makeStrongReductionTests.py --dir JES_2016/Moriond2017 --npars 3 --ref JES2016_AllNuisanceParameters.config --tmp generateConfigs/JES2016_SR_template.config --batch --comparisonStage --ntestscomparisons 300 --investigateComb 116061,36885,33399,64587

# Later, to finally test the selected ones (now renamed)
jetDefinition="AntiKt4EMTopo,AntiKt4LCTopo" configFiles="/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/share/JES_2016/Moriond2017/JES2016_AllNuisanceParameters.config;/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/share/JES_2016/Moriond2017/JES2016_SR_Scenario1.config" outFile="matrices_1.pdf" outHistFile="matrices_1.root" bash runMakeCorrelationMatrixPlots.sh Flexible FineGridFixedEta false
# for each of them, and then....
python ../python_scripts/Make4DCorrelationMatrix.py "AntiKt4EMTopo" SR-4D-Moriond2017.png "NONE" false matrices_1.root matrices_2.root matrices_3.root matrices_4.root

# Re-run the full 5 above for each of EM, LC.
