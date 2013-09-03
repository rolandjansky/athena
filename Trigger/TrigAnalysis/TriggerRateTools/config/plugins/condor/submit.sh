#!/bin/bash
########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        07 Sep, 2007                            #
#                                                      #
# Description: submits jobs to the "condor" system     #
########################################################

#creating script that will set job environment variables

if [ $1 != 3  ]; then
    if [ -e $CONDOR_SCRIPTS ]; then
        echo "nothing" >> /dev/null
    else
        mkdir -p $CONDOR_SCRIPTS
    fi

    echo "#!/bin/bash" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
    echo "export CONFIG_FILE=\"$CONFIG_FILE\"" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
    echo "export RUN_DIR_SPECIFIC=\"$RUN_DIR_SPECIFIC\"" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
    echo "export FILE=\"$FILE\"" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
    echo "export ROOT_FILE=\"$ROOT_FILE\"" >> $CONDOR_SCRIPTS/$JOB_NAME.sh

#    echo "output_folder=\`echo \$FILE | awk -F \"/\" '{ \$NF = \"\"; \$(NF-1) = \"\"; print }' | awk '{sub(/[ \t]+\$/, \"\");print}' | awk '{gsub(/ /,\"/\");print}'\`" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "folder_pt1=\`echo \$FILE | awk -F \"/\" '{ print \$(NF-2);}' | awk '{sub(/[ \t]+\$/, \"\");print}' | awk '{gsub(/ /,\"/\");print}'\`" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "folder_pt2=\`echo \$RUN_DIR_SPECIFIC | awk -F \".\" '{print \$NF;}'\`" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "folder=\"\$folder_pt1\"\"_\"\"\$folder_pt2\"" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "mkdir -p /tmp/\$folder" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#
#    echo "" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "echo \"Copying files locally...\"" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "for root_file in \`grep \"/tmp/\" \$FILE | awk -F \"\\\"\" '{print \$2}' | awk -F \"/\" '{print \$4}'\`; do" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "    orig_root_file=\`grep \"\$root_file\" \$output_folder/original_data | awk '{sub(/^[ \t]+/, \"\"); print}' | awk '{sub(/[ \t]+\$/, \"\");print}'\`" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "    dccp \$orig_root_file /tmp/\$folder/" >> $CONDOR_SCRIPTS/$JOB_NAME.sh
#    echo "done" >> $CONDOR_SCRIPTS/$JOB_NAME.sh

    chmod a+x $CONDOR_SCRIPTS/$JOB_NAME.sh
else
    export CONDOR_SCRIPTS="$OUTPUT_PATH/condor_scripts"
    if [ -e $CONDOR_SCRIPTS ]; then
        echo "nothing" >> /dev/null
    else
        mkdir -p $CONDOR_SCRIPTS
    fi
fi

#creating the job specifications files

echo "Universe        = $UNIVERSE" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Notification    = $NOTIFICATION" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Notify_user     = $NOTIFY_USER" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Executable      = $JOB_EXEC" >> $CONDOR_SCRIPTS/$JOB_NAME.job
if [ $1 != 3  ]; then
    echo "Arguments       = \"$CONDOR_SCRIPTS/$JOB_NAME.sh\"" >> $CONDOR_SCRIPTS/$JOB_NAME.job
else
    echo "Arguments       = \"$JOB_ARGS\"" >> $CONDOR_SCRIPTS/$JOB_NAME.job
fi
echo "Requirements    = $REQUIREMENTS" >> $CONDOR_SCRIPTS/$JOB_NAME.job
#echo "Rank            = $RANK" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Image_Size      = $IMAGE_SIZE" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Priority        = $PRIORITY" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "GetEnv          = $GETENV" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Initialdir      = $INITIALDIR" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Input           = $INPUT" >> $CONDOR_SCRIPTS/$JOB_NAME.job
#echo "Output          = $OUTPUT_PATH/out/$JOB_NAME.out" >> $CONDOR_SCRIPTS/$JOB_NAME.job
#echo "Output          = /dev/null" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Output          = $OUTPUT_PATH/out/$JOB_NAME.out" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Error           = $OUTPUT_PATH/log/$JOB_NAME.log" >> $CONDOR_SCRIPTS/$JOB_NAME.job
#echo "Log             = \"$OUTPUT_PATH/log/$JOB_NAME.condor.log\"" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "periodic_release = ((JobStatus==5) && (CurentTime - EnteredCurrentStatus) > 60)" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "+Experiment     = $EXPERIMENT" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "+Job_Type       = $JOB_TYPE" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "+RACF_Group     = $RACF_GROUP" >> $CONDOR_SCRIPTS/$JOB_NAME.job
echo "Queue" >> $CONDOR_SCRIPTS/$JOB_NAME.job
chmod a+x $CONDOR_SCRIPTS/$JOB_NAME.job

condor_submit $CONDOR_SCRIPTS/$JOB_NAME.job
