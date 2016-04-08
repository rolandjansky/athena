#!/usr/bin/env zsh

#Eventually take list of menus from a text file


#This is a list of menus to upload - need to edit this over time!
#If you change this, you also need to change the smk and prescale set naming below 
menu_names=(
#    MC_lumi1E31_simpleL1CalibB
#    MC_lumi1E31_simpleL1Calib_no_prescale
#    MC_lumi1E31_simpleL1Calib_physics_prescale
    MC_InitialBeam_v1 	
    MC_InitialBeam_v1_no_prescale
#    MC_InitialBeam_v2 	
#    MC_InitialBeam_v2_no_prescale
    MC_InitialBeam_v3 	
    MC_InitialBeam_v3_no_prescale
    InitialBeam_HI_v1
    InitialBeam_HI_v1_no_prescale
    Physics_pp_v1
    MC_pp_v1_no_prescale
    MC_pp_v1_loose_mc_prescale
    MC_pp_v1_tight_mc_prescale
)

#We give three arguments $1 $2
#First specifies the new release number (eg 15.6.3.6)
#Second is the connection string 
#e.g. "Oracle://devdb10/ATLAS_PBELL;username=ATLAS_PBELL;password=xxx"
#e.g. "Oracle://atlr/ATLAS_CONF_TRIGGER_MC;username=ATLAS_CONF_TRIGGER_MC_W;password=xxx"

if [ $# -lt 2 ]; then
		echo "User must feed: "
		echo "release - the release to be *built*"
		echo "connection - the oracle connection string for the TriggerTool"
		exit 1
fi

release=$1
connection=$2

echo "*************************************************************"
echo "****** MCTriggerDB upload script by Paul Bell Feb 2010 ******"
echo "**** "
echo "**** Will upload the menus for the $release release with $connection"
echo "**** "


for menu in $menu_names; do
		
		echo "**** -- $menu"
done

echo "**** "
echo "**** If the above is OK then enter c to continue"
read -r key
if [[ $key != c ]]; then
		echo "**** Exiting with no upload "
		echo "*************************************************************"
		return
fi

echo "**** "

#get hold of a TT

#echo "**** Linking to the TriggerTool in /afs/cern.ch/user/a/attrgcnf/TriggerTool"
#rm TriggerTool.jar
#ln -s /afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool.jar
export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
echo "**** "

#we have the locations of the xmls, so for all the menus we want uploading we call the TT
#we mark each key as being for replication

XML_loc="${AtlasArea}/Trigger/TriggerCommon/TriggerMenuXML/data/"
for menu in $menu_names; do

		L1_loc=${XML_loc}/LVL1config_${menu}_${release}.xml
		HLT_loc=${XML_loc}/HLTconfig_${menu}_${release}.xml
		
		if [ -e ${L1_loc} ]; then
				cp ${L1_loc} .
		else
				echo "Could not find ${L1_loc}, please correct the location"
		fi
		
		if [ -e ${HLT_loc} ]; then
				cp ${HLT_loc} .
		else
				echo "Could not find ${HLT_loc}, please correct the location"
		fi
		
    echo "**** Calling TriggerTool upload for menu $menu:"
		
    java -jar TriggerTool.jar -up -release ${release} -l1_menu "LVL1config_${menu}_${release}.xml" -hlt_menu "HLTconfig_${menu}_${release}.xml" -offlineconfig -mr -dbConn ${connection}

done

#now we replicate if we asked to

if [[ $3 == "replicate" ]]; then

    echo "**** "
    echo "**** Now getting the sqlite file and replicating the Oracle DB there..."
    echo "**** "

#we could do the whole thing every time, but faster to keep adding to the file
#replicate mechanism only adds keys not already there

    #rm triggerDBMC.db
    cp /afs/cern.ch/user/a/attrgcnf/TriggerDBReplica/triggerDBMC.db .
    java -jar TriggerTool.jar -dbConn $3 -r triggerDBMC.db -offlineconfig

fi

echo "**** "
echo "**** All done "
echo "**** Now put the triggerDBMC.db file to /afs/cern.ch/user/a/attrgcnf/TriggerDBReplica, 
**** email the atlas-db-release list to inform them and update the MCTriggerDB twiki"
echo "**** "
echo "*************************************************************"



