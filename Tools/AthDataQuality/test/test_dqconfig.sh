#!/bin/bash

# art-description: Test hcfg file for top level hists, git hash
# art-type: build
# art-ci: 21.0
# art-include: 21.0/AthDataQuality

#Run a test webdisplay of the new config 
run_test_display()
{
    ref_file=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/AthDataQuality/data17_13TeV.00337491.physics_Main.merge.HIST.f873_h264.root

    DQWebDisplay.py $ref_file TestDisplay 2 0 "stream=physics_Main" > test_display.txt
    DQWebDisplay.py $ref_file TestDisplayCosmics 3 0 "stream=physics_Main" > test_cosmics.txt
    DQWebDisplay.py $ref_file TestDisplayHI 6 0 "stream=physics_Main" > test_HI.txt
}

run_test()
{

    hcfg_dir=$(python -c "from DataQualityConfigurations._resolve_data_path import resolve_data_path; hcfg_dir = resolve_data_path('DataQualityConfigurations'); print hcfg_dir")
    if [[ $hcfg_dir == "" ]]; then
	echo hcfg path not found
	echo Tests failed
	return 1
    fi

    cd $hcfg_dir
    echo $hcfg_dir

    hcfg_files=(collisions_run.hcfg collisions_minutes10.hcfg heavyions_run.hcfg heavyions_minutes10.hcfg) #cosmics_run.hcfg cosmics_minutes10.hcfg
    for f in "${hcfg_files[@]}"
    do
	han-config-print.exe $f > /tmp/hcfg_out.txt
	check_hcfg_hash $f
	if ! [ $? -eq 0 ]; then
            echo git hash missing from $f
	    echo Tests failed
            return 0
	fi
	check_hcfg_top_level $f
	if ! [ $? -eq 0 ]; then
            echo "Histograms found under \"Overall Status\" for $f (output was likely specified incorrectly)"
            echo Tests failed
            return 0
	fi
    done

    run_test_display
    if ! [ $? -eq 0 ]; then
	echo Web display failed
	echo update failed
	return 0
    fi
    
    check_for_git
    if ! [ $? -eq 0 ]; then
	echo git hash missing from webdisplays
	echo update failed
	return 0
    fi
    
    echo Tests passed
    return 0
}

run_test
