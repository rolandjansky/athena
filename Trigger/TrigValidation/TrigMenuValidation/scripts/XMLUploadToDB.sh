#!/usr/bin/env zsh

#export NICOS_ATLAS_RELEASE=15.4.0
#release=$NICOS_ATLAS_RELEASE
release=$2

echo "Release: $release"
#if [[ "$release" = "" ]]; then
#    release=$AtlasVersion
#    echo "Release: $release"
#fi
sqliteFile=/tmp/TriggerMenuSQLiteFile.sqlite

if [[ "$AtlasProject" != "AtlasP1HLT" && "$AtlasProject" != "AtlasCAFHLT" ]]; then
    #This is for AtlasProduction
    menu_names=(
    ## physics pp v2
	MC_pp_v2_no_prescale
	MC_pp_v2_loose_mc_prescale
	MC_pp_v2_tight_mc_prescale
    ## physics pp v1
	MC_pp_v1_no_prescale
	MC_pp_v1_loose_mc_prescale
	MC_pp_v1_tight_mc_prescale
    )
else
    #This is for P1HLT
    menu_names=(
    ## physics pp v2
	MC_pp_v2
	Physics_pp_v2
    ## physics pp v1
	Physics_pp_v1
	MC_pp_v1
    ## HI v1 menu
	InitialBeam_HI_v1
	MC_InitialBeam_HI_v1
    )

fi


#remove old files
if [[ -e TriggerTool.jar ]]; then
    echo "Removing old TriggerTool.jar"
    rm -f TriggerTool.jar
fi
if [[ -e $sqliteFile ]]; then
    echo "Removing old $sqliteFile "
    rm -f $sqliteFile
fi
if [[ -e MenusKeys.txt ]]; then
    echo "Removing old MenusKeys.txt "
    rm -f MenusKeys.txt
fi

source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
echo "Getting TriggerTool"
#ln -s /afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool.jar .
get_files -data -symlink TriggerTool.jar
export _JAVA_OPTIONS="-Xms256m -Xmx1048m"

for menu in $menu_names; do
    
    echo "Doing" $menu
    echo "------------------"
    echo $1
    
    LVL1file=$1"LVL1config_${menu}_${release}.xml"
    HLTfile=$1"HLTconfig_${menu}_${release}.xml"
    
    if [[ -s $HLTfile ]]; then
	echo "Uploading the xml files to the DB"

	    #uploads to the DB
	java -jar TriggerTool.jar -up -release $release -l1_menu $LVL1file -hlt_menu $HLTfile -name $menu -offlineconfig -dbConn "sqlite_file:$sqliteFile"

	echo "Listing"
	ls
	echo "Listing MenusKeys"
	cat MenusKeys.txt

                #downloads form the DB as a cross-check
	smk=`tail -4 MenusKeys.txt | head -1 | tail -1 |awk '{print $3}' | tr -d ":"`
	l1smk=`tail -4 MenusKeys.txt | head -2 | tail -1 |awk '{print $4}' | tr -d ":"`
	hltsmk=`tail -4 MenusKeys.txt | head -3 | tail -1 |awk '{print $4}' | tr -d ":"`
	
	echo "smk: $smk , l1smk: $l1smk , hltsmk: $hltsmk"
	
	java -jar TriggerTool.jar -down -key $smk -L1PSkeys $l1smk -HLTPSkeys $hltsmk -dbConn "sqlite_file:$sqliteFile"
	
	cp $HLTfile HLTconfig.xml
	cp HLTMenu_${smk}_${hltsmk}.xml HLTMenu.xml 
	echo "Comparing two xml files: HLTconfig_${menu}_${release}.xml and  HLTMenu_${smk}_${hltsmk}.xml"
	    
	CompMenus.py HLTconfig.xml HLTMenu.xml -e rtt > CompMenu-$menu.txt

    fi
done

if [[ -s $sqliteFile ]]; then
    mv $sqliteFile ./TriggerMenuSQLiteFile_$release.sqlite
    mv MenusKeys.txt ./MenusKeys_$release.txt    
    exit
else
    echo 'ERROR SQLite file has not been produced as xml files do not exist. Most likely this nightly is broken!!!'
    exit 1
fi
