#!/bin/bash

if [ $# -eq 1 ]; then
    TriggerRateTools_PATH=$1
fi;

. $TriggerRateTools_PATH/config/config_common.sh

#OLD_IFS=$IFS
#IFS=:
#for cmd in $TRT_ENV; do
#    IFS=$OLD_IFS
#    echo $cmd
#    $cmd
#    IFS=:
#done
#IFS=$OLD_IFS

#DO_BATCH=false

if [ `echo $DO_BATCH | grep -c -i "true"` == 1 ]; then
    . $TriggerRateTools_PATH/config/plugins/$BATCH_SYS/setup.sh
fi


mkdir failed_jobs
for file in `ls | grep ".failed"`; do
    mv $file failed_jobs
done

iter=0

num_files=0
for file in `ls | grep ".root"`; do
	num_files=$((num_files+1))
done


while [ $num_files -gt 1 ]; do

    max_per_job=5
    num_jobs=0

    echo "Merge step $iter starting"

    curr_file=0

    for file in `ls | grep ".root"`; do
        if [ $(((curr_file/max_per_job)*max_per_job)) -eq $curr_file ]; then
    	mkdir temp_combine_$((curr_file/max_per_job))
    	cp CombineJobs.C temp_combine_$((curr_file/max_per_job))
        fi

        mv $file temp_combine_$((curr_file/max_per_job))
        curr_file=$((curr_file+1))
    done

    work_dir=`pwd`

    for folder in `ls | grep "temp_combine_"`; do

        mkdir $folder/job_details
        mkdir $folder/job_details/log
        mkdir $folder/job_details/out
        export OUTPUT_PATH="$work_dir/$folder/job_details"
        export JOB_NAME=`echo $folder | awk -F "_" '{print "rates_merge_" $3}'`

        echo "#!/bin/bash" > $folder/merge.sh

        echo ". $TriggerRateTools_PATH/config/config_common.sh" >> $folder/merge.sh
        echo "OLD_IFS=\$IFS" >> $folder/merge.sh
        echo "IFS=:" >> $folder/merge.sh
        echo "for cmd in \$TRT_ENV; do" >> $folder/merge.sh
        echo "    IFS=\$OLD_IFS" >> $folder/merge.sh
        echo "    echo \$cmd" >> $folder/merge.sh
        echo "    \$cmd" >> $folder/merge.sh
        echo "    IFS=:" >> $folder/merge.sh
        echo "done" >> $folder/merge.sh
        echo "IFS=\$OLD_IFS" >> $folder/merge.sh

        echo "cd $work_dir/$folder" >> $folder/merge.sh
        echo "folder=$folder" >> $folder/merge.sh
        echo "curr_folder=`echo $folder | awk -F \"_\" '{print $3}'`" >> $folder/merge.sh

        echo "root -b -q -l CombineJobs.C; root_ec=$?" >> $folder/merge.sh
        echo "rm -f core.*" >> $folder/merge.sh

        echo "if [ \$root_ec -eq 0 ]; then" >> $folder/merge.sh
        echo "    mv TriggerRates.root ../temp.\$curr_folder.TD.AAN.root" >> $folder/merge.sh
        echo "else" >> $folder/merge.sh
        echo "    echo \"CombineJobs.C failed!\"" >> $folder/merge.sh
        echo "fi" >> $folder/merge.sh
        echo "touch merge_done" >> $folder/merge.sh
        
        chmod a+rx $folder/merge.sh

        if [ `echo $DO_BATCH | grep -c -i "true"` == 1 ]; then
            export JOB_EXEC="$work_dir/$folder/merge.sh"
            . $TriggerRateTools_PATH/config/plugins/$BATCH_SYS/submit.sh 3
        else
            ./$folder/merge.sh
        fi

        num_jobs=$((num_jobs+1))

    done

    merged=0

    echo "Monitoring merging status..."
    while [ $merged -lt $num_jobs ]; do
        sleep 30
        if [ $BATCH_SYS == "condor" ]; then
            condor_release `whoami` > /dev/null
        fi
        merged=0
        for folder in `ls | grep "temp_combine_"`; do
    	if [ -e $folder/merge_done ]; then
    	    merged=$((merged+1))
    	fi
        done
        echo ">>> Merged: $merged"
    done

    mkdir iter_$iter
    for folder in `ls | grep "temp_combine_"`; do
        rm -f $folder/merge_done
        rm -fr $folder/job_details/
        rm -f $folder/merge.sh
        mv $folder iter_$iter
    done

    echo "Merge step $iter done"

    num_files=0
    for file in `ls | grep ".root"`; do
    	num_files=$((num_files+1))
    done

    iter=$((iter+1))

done

if [ -e temp.0.TD.AAN.root ]; then
    mv temp.0.TD.AAN.root ../TriggerRates.root
    echo "Job merging finished successfully"
else
    echo "Job merging failed at some point, TriggerRates.root not found!"
fi

echo "Cleaning up..."
mkdir original_data
if [ -e iter_0 ]; then
    for folder in `ls iter_0/ | grep "temp_combine_"`; do
        mv iter_0/$folder/* original_data
    done
    mv failed_jobs/* original_data
    rm -fr failed_jobs
    rm -fr iter_0
fi

for folder in `ls | grep "iter_"`; do
    rm -fr $folder
done

for iter_folder in `ls | grep "temp_combine_"`; do
    rm -fr $iter_folder
done

for file in `ls original_data`; do
    mv original_data/$file .
done

rm -fr original_data
