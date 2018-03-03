asetup asetup 21.2,AthAnalysis,latest
lsetup PyAMI

# for susy_bkg_datasets_2018-XX-XX.txt 
getMetadata.py --timestamp="2018-03-03 08:00:00" --physicsGroups="SUSY,PMG,MCGN" --fields="dataset_number,physics_short,crossSection_pb,kFactor,genFiltEff,crossSectionTotalRelUncertUP" --inDsTxt="susy_bkg_datasets_2018-03-02.txt" > Backgrounds_2018-03-02.txt

# for central page: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgCentralPage#Reporting_issues
#getMetadata.py --fields="ldn,dataset_number,crossSection,kFactor,genFiltEff,processGroup,crossSectionRef" --inDsTxt="mydatasets.txt" --outFile="metadata.txt" --delim=";"

