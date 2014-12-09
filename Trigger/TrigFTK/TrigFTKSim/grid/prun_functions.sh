#!/bin/bash

# this file contains auxillary functions for FTK simulation on the grid

nLayersFinalFit=8
nLayersFinalFitOld=11

function checkReturn() {
    # function that should be run IMMEDIATELY after each ftk.py invocation
    # if ftk.py failed at the initialization stage, we exit with the same error code
    stcode="$?"
    if [ ! "${stcode}" -eq "0" ]; then
	echo "ERROR: fatal failure in ftk.py: the job failed with exit code ${stcode}."
	echo "This is sometimes caused by a temporary PNFS glitch - consider retrying the job."
	echo "HINT: check the last ftk.py LOG file below."
	echo "================================================"
	echo "================================================"
	# try to retrieve the last two log/error files
	echo " Retrieving the last two ftk.py log/error files: "
	rm -f dummy_file && touch dummy_file
	if [ -f ".ftk_jobinp" ]; then
	    echo "================================================"
	    echo "       JOB INPUTs:"
	    cat .ftk_jobinp
 	    cat `cat .ftk_jobinp` dummy_file
	fi
	if [ -f ".ftk_stdout" ]; then
	    echo "================================================"
	    echo "       STDOUTs:"
	    cat .ftk_stdout
 	    cat `cat .ftk_stdout` dummy_file
	fi
	if [ -f ".ftk_stderr" ]; then
	    echo "================================================"
	    echo "       STDERRs:"
	    cat .ftk_stderr
 	    cat `cat .ftk_stderr` dummy_file
	fi
	rm -f dummy_file
	# clean up to ensure that the job is failed by Panda
        rm -rf ${OUTPUTDIR}
	echo "Exiting..."
	exit ${stcode};
    else
	echo "INFO: job finished successfully with return code ${stcode}"
    fi;
}

function DEBUG() {
    [ "$_DEBUG" == "on" ] &&  $@
}

function isGridDS() {
    # Function that identifies grid datasets.
    # We accept either user10.AntonKapliy OR user.kapliy (new format using grid nickname)
    #   https://lcg-voms.cern.ch:8443/vo/atlas/vomrs
    #     [Member Info] -> [Edit Personal Info]
    [ `echo $1 | cut -b -6 | egrep --color=never '^user[0-9][0-9]|^user\.|^mc[0-9][0-9]|^group\.'` ];
}

function isGridOutputDS_old() {
    # Function that identifies grid datasets gerentated from ftk.py
    # They contain a bank generated with mode 10
    # this is for the old format: patterns are tar'ed up
    [ `echo $1 | egrep --color=never '^user[0-9][0-9]|^user\.|^mc[0-9][0-9]|^group\.' | grep NSubsPerRegion` ];
}

function isGridOutputDS() {
    # Function that identifies grid datasets gerentated from ftk.py
    # They contain a bank generated with mode 10
    # this is for the old format: patterns are tar'ed up
    [ `echo $1 | egrep --color=never '^user[0-9][0-9]|^user\.|^mc[0-9][0-9]|^group\.' | grep NLoops | grep -v unmerged` ];
}

function relayerRunstring() {
    # changes # of layers in the runstring (used for split arch)
    local runstr=$1  # raw_11L_50x64x144_v14indep_test
    local newval=$2  # 8
                     # output = raw_8L_50x64x144_v14indep_test
    echo ${runstr} | awk -v newval=${newval} 'BEGIN{FS="_"}{$2=sprintf("%sL",newval); for(i=1;i<=NF;i++) if(i==1) printf("%s",$(i)); else printf("_%s",$(i)); printf("\n");}'
}

function reSSRunstring() {
    # changes ss specifier of the runstring (used for split arch)
    local runstr=$1  # raw_11L_50x64x144_v14indep_test
    local newval=$2  # 50x16x144
                     # output = raw_11L_50x16x144_v14indep_test    
    echo ${runstr} | awk -v newval=${newval} 'BEGIN{FS="_"}{$3=sprintf("%sL",newval); for(i=1;i<=NF;i++) if(i==1) printf("%s",$(i)); else printf("_%s",$(i)); printf("\n");}'
}

function extractNsubsFromRunstring() {
    # extracts nsubs from runstring in the format raw_11L_50x64x144_v14indep_4M_othertags
    local nsubs=`echo $1 | awk 'BEGIN{FS="_";}; {print $5}' | sed -e 's#M##g'`
    if [ ! $(echo "$nsubs" | grep -E "^[0-9]+$") ]; then
	echo
	echo 'ERROR: wrong format of runstring - could not determine nsubs'
	echo 'Correct format: raw_11L_50x64x144_v14indep_4M_othertags'
	echo "Details: runstring=$1 but nsubs=[${nsubs}]"
	echo
	exit -1;
    fi;
    echo $nsubs
}

function extractNLoops() {
    if [ `echo $1 | grep NLoops` ]; then
	echo $1 | sed -e "s,.*_\([0-9]*\)NLoops.*,\1,";
    fi
}

function trim() {
    echo $1;
}

# checks if Panda created lists of staged input files (prun --writeInputToTxt option)
# these lists are expected to be called STREAM.files.dat for each input DS stream.
# In the future, this can be updated to handle xrootd-hosted files, too.
function prepareStaged () {
    rm -f $STAGED_PATHS $STAGED_FILES $STAGED_INPATH
    touch $STAGED_PATHS $STAGED_FILES $STAGED_INPATH
    nINfiles=`find . -maxdepth 1 -name IN\*files.dat | wc -l`
    if [ "${nINfiles}" -gt "0" ]; then
        # parse comma-separated input, remove dcap://server/ prefix,
	# remove whitespace; sort; convert to full linux path (if not on dcache)
	for f in IN*files.dat; do
	cat $f | awk 'function c(p){delete r;split(p,r,"/pnfs/");return r[2] ? sprintf("/pnfs/%s",r[2]) : p;}BEGIN{FS=","};{for(i=1;i<=NF;i++)print c($i);}' | sed 's/^[ \t]*//;s/[ \t]*$//' | sort | awk -v base=${ROOTDIR} 'BEGIN{FS="/"};NF>1{print};NF<=1{printf("%s/%s\n",base,$0)}' >> ${STAGED_PATHS}
    done;
	cat IN.files.dat 2>/dev/null | awk 'function c(p){delete r;split(p,r,"/pnfs/");return r[2] ? sprintf("/pnfs/%s",r[2]) : p;}BEGIN{FS=","};{for(i=1;i<=NF;i++)print c($i);}' | sed 's/^[ \t]*//;s/[ \t]*$//' | sort | awk -v base=${ROOTDIR} 'BEGIN{FS="/"};NF>1{print};NF<=1{printf("%s/%s\n",base,$0)}' > ${STAGED_INPATH}
	for ff in `cat $STAGED_PATHS`; do
	basename $ff >> $STAGED_FILES
    done
    echo "LIST OF STAGED FILE PATHS (RAW):"
    for f in IN*files.dat; do
	echo "========= ${f} =========="
	cat ${f}
	echo ""
    done
    echo "LIST OF STAGED FILE PATHS (POST-PROCESSING):"
    cat $STAGED_PATHS
    echo "LIST OF STAGED %IN% FILE PATHS (POST-PROCESSING):"
    cat $STAGED_INPATH
else
    echo "WARNING: didn't find any staged input files. Will fall back to manual dq2-get calls..."
fi;
}

# returns pythonpath WITHOUT any of the SITEROOT-located paths.
# This allows to use the default environment python (/usr/bin/python) more safely
function cleanedPythonpath() {
    python -c "import sys,re; newpath=[p for p in sys.path if not re.search('$SITEROOT',p)]; newp=':'.join(newpath); print newp[1:]"
}

function checkFiles() {
    for f in $@; do
	if [ ! -s $f ]; then
	    echo
	    echo ERROR: missing file $f
	    echo
	    echo
	    echo "Directory listing:"
	    echo
	    find . -maxdepth 2;
	    exit -1;
	fi
    done;
}

function checkFTKDS() {
    if [ ! -n "${ftkDS}" ]; then
	echo "ERROR: missing -o ftkDS=user.nickname.<..> or -o ftkDS=user10.UserName.<..>"
	echo "(ftkDS is the output dataset of a prior ftksim run)"
	exit -1;
    fi;
}

# extracts ftk.py option specified via "-o" flag
# since there could be multiple -o substrings, we have to scan them all
function extractOption() {
    oname=$1  # for example, MAXPATTERNS
    result=""
    ostrings=`echo $extra_args | awk 'BEGIN{z=0;}; {for(i=0;i<NF;i++) if($i=="-o") out[z++]=$(i+1);}; END{for(j=0;j<z;j++) print out[j]; }'`
    # loop over all -o arguments
    for ostring in ${ostrings}; do
        # split comma-separated list
        opts=`echo $ostring | awk 'BEGIN{FS=","}; {for(i=0;i<NF;i++) print $(i+1)}'`
        # finally, loop over actual options:
        for opt in ${opts}; do
            name=`echo $opt | awk 'BEGIN{FS="="}; {print $1; exit(0);};'`
            value=`echo $opt | awk 'BEGIN{FS="="};{print $2; exit(0);};'`
            # option with an explicitly given value
            if [ -n "${value}" ]; then
                if [ "${name}" == "${oname}" ]; then
                    result="${value}"
                fi;
            # option with a default value (1)
            else
                if [ "${name}" == "${oname}" ]; then
                    result="1";
                fi;
            fi;
        done;
    done;

    # if the result string is empty use the def
    if [ -z $result ]; then
	if [ -n $2 ]; then
	    result=$2
	fi
    fi

    echo ${result}
}

# An internal workhorse function that downloads a list of files from a dataset
# Do not use it - instead use a wrapper function getFilesFromDS
# IDEA:
# All files are downloaded at once using "dq2-get -f file1,file2,file3" to minimize oracle DB dq2 calls
# Failed transfers (and only those!) are re-attempted several times
function getFilesFromDS_internal() {
    local DQ2_LOCAL_SITE_ID=$1
    local DSname=$2
    local FILES_STAGED=$3
    local QUIT_ON_FAIL=$4  # leave undefined to allow non-fatal download failures
    
    local FILELIST_STAGED=
    local COUNT=0;
    for fname in ${FILES_STAGED}; do
	if [ $COUNT -eq 0 ]; then
	    FILELIST_STAGED=${fname};
	else
	    FILELIST_STAGED=${FILELIST_STAGED},${fname};
	fi
	((COUNT++));
    done
    FILELIST_STAGED=`trim ${FILELIST_STAGED}`
    NUMFILES_STAGED=${COUNT}
    echo "Number of files to be downloaded: ${NUMFILES_STAGED}"
    echo "Length of filelist string passed as -f parameter to dq2-get: ${#FILELIST_STAGED}"
    for ntrial in {0..5}; do
        # find the subset of files that haven't been downloaded yet
	FILES_STAGED=
	for fname in `echo ${FILELIST_STAGED} | awk 'BEGIN {FS=","} {for(i=1;i<=NF;i++) print $i}'`; do
	    local found=`find . -type f -name $fname`
	    if [ "$found" != "" ] && [ -s $found ]; then
		continue
	    fi
	    FILES_STAGED="${FILES_STAGED} ${fname}"
	done
	FILELIST_STAGED=
	COUNT=0
	for fname in ${FILES_STAGED}; do
 	    if [ $COUNT -eq 0 ]; then
 		FILELIST_STAGED=${fname};
 	    else
 		FILELIST_STAGED=${FILELIST_STAGED},${fname};
 	    fi
 	    COUNT=1;
	done;
	FILELIST_STAGED=`trim ${FILELIST_STAGED}`
	if [ "${ntrial}" -eq "5" ]; then
            # In the end, provide list of files that we couldn't download despite numerous attempts:
	    if [ -n "${FILELIST_STAGED}" ]; then
		echo
		echo "WARNING: unable to download these files from ${DSname}"
		echo ${FILELIST_STAGED}
		echo
		if [ -n "${QUIT_ON_FAIL}" ]; then
		    echo 'ERROR: exiting because some downloads failed ...'
		    exit -1
		fi;
	    else
		echo "Good, all ${NUMFILES_STAGED} files have been downloaded!"
	    fi;
	    break; 
	fi
	# actual download attempt
	if [ -n "${FILELIST_STAGED}" ]; then
	    let R=$RANDOM%50
	    if [ "${ntrial}" -gt "0" ]; then
		sltime=`expr $R + 10 \* ${ntrial}`;
		echo "Sleeping for ${sltime} seconds..."
 		sleep $sltime
	    fi;
	    echo "Trying to get the following files (attempt # ${ntrial}):"
	    echo DQ2_GET_CMD -L $DQ2_LOCAL_SITE_ID -f ${FILELIST_STAGED} ${DSname};
	    DQ2_GET_CMD -L $DQ2_LOCAL_SITE_ID -f ${FILELIST_STAGED} ${DSname};
            ds=`echo ${DSname} | sed -e 's#/##g'`
            find $ds.* -maxdepth 1 -type f -exec ln -s ../{} $ds/ \;
	else
	    echo "Good, all ${NUMFILES_STAGED} files have been downloaded!"
	    break;
	fi
    done;
}

# INTERNAL FUNCTION TO DOWNLOAD BANK FILES. DO NOT USE IT - INSTEAD USE getFilesFromDS
# Downloads a list of files from a dataset (with automatic retries)
# USAGE: the list of files (third argument) can be given as any iterable, such as:
# * space-separated list of files (e.g., "file1 file2 file3" or "${myarray[*]}")
# * newline-separated list of files (e.g. the result of `cat list_of_files.txt`)
function getFilesFromDS__dq2() {
    # define maximum number of files downloaded in one dq2 request
    # this is needed to prevent extra long dq2-get bash command lines (>32k characters)
    DOWNLOAD_BATCH_SIZE=50
    local DQ2_LOCAL_SITE_ID=$1
    local DSname=$2
    local FILES_REQUESTED=$3
    local QUIT_ON_FAIL=$4  # leave empty to allow non-fatal download failures

    # Now split files into batches of DOWNLOAD_BATCH_SIZE and call download function on each batch
    nfiles=0
    unset FILES_STAGED
    FILE_BATCH=
    for f in ${FILES_REQUESTED}; do
	FILE_BATCH[nfiles]=$f
	((nfiles++))
	if [ "${nfiles}" -eq "${DOWNLOAD_BATCH_SIZE}" ]; then
	    getFilesFromDS_internal $DQ2_LOCAL_SITE_ID $DSname "${FILE_BATCH[*]}" ${QUIT_ON_FAIL}
	    nfiles=0;
	    unset FILE_BATCH
	fi;
    done
    if [ "${nfiles}" -gt "0" ]; then
	getFilesFromDS_internal $DQ2_LOCAL_SITE_ID $DSname "${FILE_BATCH[*]}" ${QUIT_ON_FAIL}
	nfiles=0
	unset FILE_BATCH
    fi;
}

# Retrieves a list of files from a dataset (with automatic retries)
# But first, it checks if these files have already been staged by the pilot
# in which case it simply symlinks them to the right folder
function getFilesFromDS() {
    local DSname=$2
    local FILES_REQUESTED=$3
    local QUIT_ON_FAIL=$4  # leave empty to allow non-fatal download failures
    mkdir -p ${DSname}
    nleft=0
    unset FILES_LEFTOUT
    for f in ${FILES_REQUESTED}; do
	if [ -s $STAGED ]; then
	    filefound=`grep --color=never $f ${STAGED_PATHS} | head -n1`
	    outname=${DSname}/${f}
	    if [ -n "${filefound}" -a -s "${filefound}" ]; then
		if [ ! -f "${outname}" ]; then
		    ln -s $filefound ${outname}
		else
		    echo "File already exists: ${outname}"
		fi;
	    else
		echo "ERROR: cannot find file ${f}"
		echo "List of available files:"
		cat ${FILES_REQUESTED}
		exit 1;
	    fi;
	else
	    FILES_LEFTOUT[nleft]=$f
	    ((nleft++))
	fi;
    done
    # explicitly download those files that have not been staged
    if [ "$nleft" -gt "0" ]; then
	echo "Explicitly downloading these files via dq2-get:"
	echo "${FILES_LEFTOUT[*]}"
	getFilesFromDS__dq2 "$1" "$2" "${FILES_LEFTOUT[*]}" "$4"
    fi;
}

# INTERNAL FUNCTION TO DOWNLOAD INPUT WRAPPER FILES. DO NOT USE IT - INSTEAD USE getSomeWrapperFiles
# downloads filenum=[fmin..fmax] from a dataset, where bounds are gived to ftk.py -o fmin=10,fmax=20
# if no bounds are given, it downloads ALL wrapper files from a dataset
# NOTE - wrapper files are assumed to have an extension ".dat" or ".bz2" or ".gz"
function getSomeWrapperFiles__dq2() {
    local inpDS=$1
    DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f ${inpDS} > inpDS_allfiles
    cat inpDS_allfiles | grep --color=never '\[.\]' | cut -f 2 | /bin/egrep --color=never '\.dat' | sort > inpDS_filelist
    NUMFILES_AVAILABLE=`cat inpDS_filelist | wc -l`
    echo "Total number of wrapper files available in ${inpDS} = ${NUMFILES_AVAILABLE}"
    head -n10 inpDS_filelist
    echo "..."
    tail -n10 inpDS_filelist
    # if no bounds are given on the command line, we will download all available wrapper files
    if [ "${fmin}" == "" -o "${fmax}" == "" ]; then 
	fmin=0
	fmax=`expr ${NUMFILES_AVAILABLE} - 1`
    fi
    NUMFILES_EXPECTED=`expr ${fmax} - ${fmin} + 1`
    echo "Preparing to download up to ${NUMFILES_EXPECTED} files in the range [${fmin}..${fmax}] from dataset ${inpDS}"
    local LIST_OF_FILES=`awk -v fmin=${fmin} -v fmax=${fmax} '{fnum=NR-1; if(fnum>=fmin && fnum<=fmax) print $1};' inpDS_filelist`
    # download the files
    getFilesFromDS__dq2 $DQ2_LOCAL_SITE_ID ${inpDS} "${LIST_OF_FILES}" 1
    NUMFILES_DOWNLOADED=`echo ${LIST_OF_FILES} | xargs -n1 find . -type f -name | wc -l`;
    if [ ! "${NUMFILES_DOWNLOADED}" -eq "${NUMFILES_EXPECTED}" ]; then
	echo "ERROR: downloaded ${NUMFILES_DOWNLOADED}, but expected ${NUMFILES_EXPECTED} files from dataset ${inpDS}. Exiting..."
	exit -1;
    fi
}

# Retrieves input wrapper files to be processed.
# If they haven't been staged by the pilot, attempts to download them manually.
# If copyFromDcache==1, the pnfs-hosted files are copied, rather than symlinked
# (only use this for effcurve jobs!)
function getSomeWrapperFiles() {
    local inpDS=$1
    local copyFromDcache=$2
    if [ ! -s "${STAGED_INPATH}" ]; then
	getSomeWrapperFiles__dq2 $@
    else
	mkdir -p ${inpDS}
	input_list=`cat ${STAGED_INPATH}`
	echo "File: $input_list"
	for f in ${input_list}; do
	    if [ -s "$f" ]; then
		outname=${inpDS}/`basename $f`
		if [ ! -f "${outname}" ]; then
		    # copy dcache-hosted files from pnfs, if requested
		    if [ "${copyFromDcache}" == "1" ] && echo $f | grep -q pnfs ; then
			echo cp $f ${outname}
			cp $f ${outname}
		    else
			echo ln -s $f ${outname}
			ln -s $f ${outname}
		    fi;
		else
		    echo "File already exists: ${outname}"
		fi;
	    else
		echo 'ERROR: cannot find staged file:'
		echo $f
		exit -1;
	    fi
	done
    fi
}


# Special function to retrieve FTK bank files - either from a folder or a dataset
# Basically, it's a wrapper around getFilesFromDS and simple cp/dccp depending on the source of bank files
function retrieveBankFiles() {
    echo "date before retrieveBankFiles" `date`;
    echo "Retrieving: $@"
    mkdir -p patt gcon input lookup
    datadir=$1;
    DSname=${datadir}; # take dataset name from bank dir
    shift;

    # case 1: files from a dataset created by prun_job.sh in mode=10 (old version)
    if isGridOutputDS_old ${datadir}; then
	mkdir -p tmp
	rm -f tmp/retrieveBankFiles_list_dq2output tmp/retrieveListFilesOrig tmp/tmp/retrieveListFiles
	if [ -s $STAGED ]; then
	    touch tmp/retrieveBankFiles_list_dq2output
	else
	    DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -r $DSname
	    DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f $DSname > tmp/retrieveBankFiles_list_dq2output_all
	    cat tmp/retrieveBankFiles_list_dq2output_all | grep '\[.\]' | cut -f 2 > tmp/retrieveBankFiles_list_dq2output
	fi;
	nfiles=0
	for f in $@; do
	    echo "preparing to copy file $f from dir $datadir"
  	    # whatever the requested files ($f) are, extract the full content 
	    # of the out_files.tbz2 corresponding to this region and subregion
	    echo reg=$reg sub=$sub
	    NSubsPerRegion=$nsubs
	    MYJOBID=`echo $(( $reg * $NSubsPerRegion + $sub + 1 )) | awk '{ a=$1; printf("%05d\n",a); }' `
	    FileName_core=${MYJOBID}.out_files.tbz2
	    FileName=${datadir}._${FileName_core}   # compose file name (old format)
	    echo FileName is ${FileName_core};
	    # Actual file name pattern we are looking for (stripped the "DATASET._" part):
	    echo ${FileName_core} >> tmp/retrieveListFilesOrig
	    # In case we intend to dq2-get it (note that we take into account .1 and other similar modifications):
	    grep ${FileName_core} tmp/retrieveBankFiles_list_dq2output >> tmp/retrieveListFiles
	done
	unset FileNames
	FileNames=`cat tmp/retrieveListFiles`
	# If no files were matched, perhaps dq2 is not working on this node.
	# To recover, we revert to the unmodified file list and hope that these files have been staged by the pilot.
	# (That will be checked inside getFilesFromDS function)
	nFileNames=`cat tmp/retrieveListFiles | wc -l`
	nFileNamesOrig=`cat tmp/retrieveListFilesOrig | wc -l`
	if [ "${nFileNames}" -eq "0" -a "${nFileNamesOrig}" -gt "0" ]; then
	    FileNames=`cat tmp/retrieveListFilesOrig`
	fi;
	getFilesFromDS $DQ2_LOCAL_SITE_ID $DSname "${FileNames}" 1
	for FileName in $FileNames; do
	    if [ ! -s "${DSname}/${FileName}" ]; then
		echo "ERROR: failed to download ${DSname}/${FileName}"
		ls -l ${DSname}/
		echo "Exiting..."
		exit 1;
	    fi;
	    echo "Untarring file $FileName"
	    # cannot use tar directly on /pnfs files, so cat it first
	    cat ${DSname}/${FileName} | bzip2 -dc | tar xvf -
	    for myFile in output/*; do
   		# remove _run* from the name
		mv $myFile `echo $myFile | sed -e "s,_run[0-9],,"`
		mv `echo $myFile | sed -e "s,_run[0-9],,"` patt/
	    done;
	    echo list output dir
	    ls -l output/
	    echo list patt dir
	    ls -l patt/
	    rm output/*
	    rm ${DSname}/*
	done

    # case 2: files from a dataset that was manually created and uploaded via dq2-put,
    # or from a dataset created using the new grid mode=pconst
    elif isGridDS ${datadir}; then
	echo "CASE 2"
	unset FileNames
	nfiles=0
	mkdir -p tmp
	if [ -s $STAGED ]; then
	    cat ${STAGED_FILES} > tmp/retrieveBankFiles_list_dq2output
	else
	    DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -r $DSname
	    DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f $DSname > tmp/retrieveBankFiles_list_dq2output_all
	    cat tmp/retrieveBankFiles_list_dq2output_all | grep '\[.\]' | cut -f 2 > tmp/retrieveBankFiles_list_dq2output
	fi;
        # compose the list of file names
	for f in $@; do
	    basef=`basename $f`
	    echo "BASENAME ${basef}"
	    FileNames[$nfiles]=`grep --color=never $basef tmp/retrieveBankFiles_list_dq2output | head -n1`
	    if [ ! -n "${FileNames[$nfiles]}" ]; then
		echo "ERROR: Unable to find file $basef"
		echo "List of available files:"
		cat tmp/retrieveBankFiles_list_dq2output
		exit 1
	    fi;
	    ((nfiles++))
	done
	getFilesFromDS $DQ2_LOCAL_SITE_ID $DSname "${FileNames[*]}" 1
	i=0
	for f in $@; do
	    echo running mv ${FileNames[$i]} $f
	    find . -maxdepth 2 -name ${FileNames[$i]} -exec  mv {} $f \;
	    ((i++))
	done

    # case 3: local (or pnfs-hosted) files that can be copied directly
    else
	for f in $@; do
	    src=${datadir}/${f}
	    CP=cp
	    echo $src | grep -q pnfs && CP=dccp
	    ${CP} ${src} ${f}
	done;
    fi

    # Final check: were all files actually copied?
    for f in $@; do
	if [ ! -s $f ]; then
	    pwd
	    ls -lrta $f
	    echo
	    echo "ERROR: unable to copy file ${datadir}/${f} into $f exiting..."
	    echo
	    exit -1;
	fi
    done;

    echo "date after retrieveBankFiles" `date`;
    echo
}

# This function checks that bank directories are specified consistently
# between local directories and grid datasets
# run this every time after you modify bankdir or baseDS
function setBankDirs () {
    echo "Default bank directory: ${bankdir}"
    if [ "$baseDS" == "" ]; then 
	echo "WARNING: base dataset (-o baseDS) for sector/constants/slice files was not given on command line; reverting to prun_job.sh defaults"
	baseDS=${bankdir}
    fi;
    echo "Default baseDS: ${baseDS}"
    if isGridDS ${baseDS}; then
	if isGridDS ${bankdir}; then
	    echo ; #do nothing
	else
	    echo ERROR: -o baseDS is used to get files from a grid dataset
	    echo ERROR: while the -t option is used to take file from a local directory
	    echo ERROR: -o baseDS option can olny be used along with \"-t grid_dataset\".
	    echo ERROR: Exiting...;
	    exit -1;
	fi;
    fi;
    #use the NEWBANKDIR variable to search for patterns in the "patt" directory 
    #when patterns are downloaded from a grid dataset
    NEWBANKDIR=
    if isGridDS ${bankdir}; then 
	echo Redefining bankdir in order to point it to the downloaded files from grid dataset
	NEWBANKDIR=(-t ./);
    fi;
    echo NEWBANKDIR is ${NEWBANKDIR[@]};
}



# This function downloads files from a dataset that ran in per-subregion mode
# extracts them, and performs trackmerger/roadmerger for a given region or pattern point
# For now, this merges everything in one job - sequentially.
function mergeSubregions() {
    dataset=$1;      # output of TrigFTKSim running
    loopRegions=$2;  # loopRegions=1 loops over all regions; otherwise it loops over ipattPoints (effmode)
    realRegion=$3;   # if looping over ipattPoints, gives the real region number
    # get the options that the user should have passed along
    SKIP_MERGE_REGIONS=`extractOption SKIP_MERGE_REGIONS`
    DO_NOT_MERGE_ROADS=`extractOption DO_NOT_MERGE_ROADS`
    MERGED_ROADS=`extractOption MERGED_ROADS`;
    EXPLORE_FTK=`extractOption EXPLORE_FTK`;
    if [ ! -n "${MERGED_ROADS}" ]; then MERGED_ROADS=0; fi;
    if [ ! -n "${realRegion}" ]; then realRegion=0; fi;
    # download input datasets
    set +x
    if isGridDS ${inputDS}; then
	if [ "${loopRegions}" == "1" ]; then
	    # symlink inputs if merging a regular ftk run
	    getSomeWrapperFiles ${inputDS} ${COPY_FROM_DCACHE}
	else
	    # copy inputs if running effcurve (because effcurve script cannot run on dcache)
	    getSomeWrapperFiles ${inputDS} 1
	fi;
    fi;
    set -x
    mkdir -p patt gcon
    if [ "${loopRegions}" == "1" ]; then
	groups=`seq 0 $((NTowers-1))`
    else
	groups=`python -c "from FTKCore import patternPoints; print ' '.join([str(i) for (i,k) in enumerate(patternPoints)])"`
    fi;
    ngroups=`echo $groups | awk '{print NF}'`

    if [ -s $STAGED ]; then
	cat ${STAGED_FILES} > dataset_contents
    else
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -r ${dataset}
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f ${dataset} > dataset_contents_all
	cat dataset_contents_all | grep '\[.\]' | cut -f 2 > dataset_contents
    fi

    badRegions="" # list of the uncompleted regions
    goodRegions="" # list of complete regions
    for igrp in ${groups}; do # region loop
	if [ "${loopRegions}" == "1" ]; then
	    reg=$igrp;
	else
	    reg=$realRegion;  # efficiency mode always runs over one region (region 0 by default)
#add my regions also processgridxml in ftkcore.py--LT NOTE
	fi;
	set +x
	dld_cnt=0
	unset FILES_TO_GET

	isBadRegion=0
	# Download FTK subjobs in current region/patternPoint
	for sub in `seq 0 $(expr $nsubs - 1)`; do # sub-region loop
	    # create dummy sectors files (may be needed for roadmerger)
	    echo 'unused' > patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
            # download out_files.tbz2 from each subjob:
	    # OLD version:
	    ##MYJOBID=`echo $(( $igrp * $nsubs + $sub + 1 )) | awk '{ a=$1; printf("%05d\n",a); }' `
	    ##FileName=${dataset}._${MYJOBID}.out_files.tbz2
	    # NEW version:
	    # compose file name
	    if [ "${loopRegions}" == "1" ]; then
		FileName="reg${reg}_sub${sub}_loop0.out_files.tbz2"
	    else
		FileName="reg${reg}_sub${sub}_loop${igrp}.out_files.tbz2"
	    fi;
	    echo "FileName  is $FileName"
            # account for extra suffixes, such as .tbz2.1 or .tbz2.2
	    FileToGet=`cat dataset_contents | grep $FileName | head -n1`
	    if [ ! -n "${FileToGet}" ]; then
		echo "WARNING: Failed to find file=${FileName} in dataset=${dataset}"
		echo "Region/patternPoint = ${igrp}"
		echo "Subregion = ${sub}"
		# in normal run merging, this is a disaster
		if [ "${loopRegions}" == "1" ]; then
		    echo "WARNING: failed to find files for this region"
		    badRegions="${badRegions} $igrp"
		    isBadRegion=1
		    break;
		# in effcurve merging, this is OK - we just won't have one of the patternPoints
		else
		    echo "Attempting to continue..."
		    break;
		fi;
	    fi;
	    echo "FileToGet is ${FileToGet}"
	    FILES_TO_GET[$dld_cnt]=${FileToGet}
	    ((dld_cnt++))
	done; # end loop over subregions
	getFilesFromDS ${DQ2_LOCAL_SITE_ID} ${dataset} "${FILES_TO_GET[*]}" 1
	echo "Untarring downloaded files..."
	for FileToGet in ${FILES_TO_GET[*]}; do # file loop
	    echo "Untarring ${FileToGet}"
    	    # cannot use tar directly on /pnfs files, so cat it first
            # if the files are not on dcap the direct use of tar is preferred and fixes some instability
            tar xvfj ${dataset}/${FileToGet}
	    # delete original tar file:
	    rm -f ${dataset}/${FileToGet}
	    # delete subfolders of output/ that we don't need for merging
	    # (e.g., in split arch, this will delete intermediate 8L results)
	    if [ -d "output" ]; then
		cd output/
		local runstring4tmp=`relayerRunstring ${runstring} 4`  # preserve 4L to which 11L is symlinked
		find . -maxdepth 1 -type d ! -name . -a ! -name run -a ! -name other -a ! -name "${runstring}*" -a ! -name "${runstring4tmp}*" -exec rm -rf {} \;
		cd -
	    fi;
	    # delete roads if we don't plan to merge them
	    if [ -n "${DO_NOT_MERGE_ROADS}" ]; then
		find output/ -maxdepth 2 -type d -name roads -o -name merged -exec rm -rf {} \;
	    fi;
	done # end file loop
	echo "Space usage after downloading and untarring:"
	du -shc output/*

	# check that we downloaded all subregions successfully
	if [ $isBadRegion -eq 0 ]; then
	    if [ ! "${dld_cnt}" -eq "${nsubs}" ]; then
		echo "WARNING: skipping patternPoint = ${igrp} because we only downloaded ${dld_cnt}/${nsubs} files"
		echo "Attempting to continue..."
		find output/ -maxdepth 2 -type d -name roads -exec rm -rf {} \;
		continue;
	    fi;
	    goodRegions="${goodRegions} $igrp"
	else
	    echo "WARNING: could not find all the input files for region $igrp"
	    continue;
	fi
	set -x

	# run per-region merging first. Always merger tracks per-region, and merge roads only if requested.
	# Note: for split arch, SKIP_MERGE_REGIONS should be 1 because roads are already merged in the original job!
	if [ ! -n "${SKIP_MERGE_REGIONS}" ]; then
            # regular mode: merge roads and tracks in each region and run plots script (#roads/tracks simulation)
	    if [ ${TFmode} == "trackfitter711" ]; then
		extraTMopts=":ncoords=14"  # inline option, so use column!
	    else
		extraTMopts=""
	    fi
	    if [ "${loopRegions}" == "1" ]; then
		np=${maxpatterns}
		if [ ! -n "${DO_NOT_MERGE_ROADS}" ]; then
		    action=[MERGED_ROADS=1:ENCODE_SUBREGION=0],roadmerger${reg},[MERGED_ROADS=0${extraTMopts}],trackmerger${reg},STOP,[MERGED_TRACKS=1],plot_${reg}
		else
		    action=[MERGED_ROADS=0${extraTMopts}],trackmerger${reg}
		fi;
    	    # efficiency mode: we only have region 0, so merge roads and tracks right away
	    else
		np=`python -c "from FTKCore import patternPoints; print patternPoints[${igrp}]"`
		if [ ! -n "${DO_NOT_MERGE_ROADS}" ]; then
		    action=[MERGED_ROADS=1:ENCODE_SUBREGION=0],roadmerger${reg},STOP,[MERGED_ROADS=0${extraTMopts}],trackmerger${reg}
		else
		    action=[MERGED_ROADS=0${extraTMopts}],trackmerger${reg}
		fi
	    fi;
	    ./ftk.py "${extraArgs[@]}" -t ./ -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ./output/ -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -o ENCODE_SUBREGION=0,DELETE_ROADS_AFTER_USE=1 -a ${action} 
	    checkReturn
	fi;
	echo "Space usage after running per-region roadmerger/trackmerger:"
	du -shc output/*
	# delete the original tracks and roads to free space in the WN
	find output/ -maxdepth 2 -type d -name roads -exec rm -rf {} \;
	find output/ -maxdepth 2 -type d -name tracks -exec rm -rf {} \;
    done; # end loop over regions OR patternPoints
    
    # Now run some actions over the entire detector (all 8 regions)
    # regular mode: do global trackmerger & analysis
    # Note: MERGED_ROADS is needed to determine track folder structure ($reg/*.root vs $reg/$sub/*.root)
    #       It should be 0 for all TSP/Option-A merging, and 1 for split arch merging
    if [ "${loopRegions}" == "1" ]; then
	np=${maxpatterns}
	if [ ${mode} == "merge711" -o "${TFmode}" == "trackfitter711" ]; then
	    extraTMopts="ncoords=14,"   # external option (given via -o), so use comma!
	else
	    extraTMopts=""
	fi

	# at this point, 8 individual trackmerger$reg jobs have been run, so we can use alltrackmerger for final merge (removed the ana option):
	if [ -z "$badRegions" ]; then
	    action=[MERGED_ROADS=${MERGED_ROADS}],alltrackmerger
	    ./ftk.py "${extraArgs[@]}" -t ./ -d ${runstring}_np${np} -o ${extraTMopts}MAXPATTERNS=${np} -r ./output/ -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${action}
	    checkReturn
	fi

    # efficiency mode: run effcurve script and make the plots
    else
	action=[MERGED_ROADS=${MERGED_ROADS}],effcurve${realRegion}

	./ftk.py "${extraArgs[@]}" -t ./ -d ${runstring} -r ./output/ -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${action}
	checkReturn
    fi;

    if [ "$EXPLORE_FTK" != "" ]; then
	echo execute FTKExplorer after mergeSubregions
##################################################################################################################
# Looking for existence of directory tree and setting $bad/goodRegions: this is done for tracks folder
	if [ -d "output/${runstring}_np${maxpatterns}/tracks_merge/${NTowers}" ]; then
	    echo "There is a global merged folder"
	else
	    for i in $(seq 0 $((NTowers-1))); do
		if [ -d "output/${runstring}_np${maxpatterns}/tracks_merge/$i" ]; then
		    if [ -z "$goodRegions" ]; then goodRegions="$i"; else goodRegions="$goodRegions $i"; fi
		else
		    if [ -z "$badRegions" ]; then badRegions="$i"; else badRegions="$badRegions $i"; fi
		fi
	    done
	    echo "There isn't a global merged folder" 
	    echo "Bad Regions = $badRegions"
	    echo "Good Regions = $goodRegions"
	fi

#################################################################################################################

        # at this point, 8 individual trackmerger$reg jobs have been run, so we can use alltrackmerger for final merge:
	np=${maxpatterns}
	
	if [ -z "$badRegions" ]; then
	    echo No bad regions
	    #action=[MERGED_ROADS=${MERGED_ROADS}],STOP,ana
	else 
	    echo "WARNING: alltrackmerger cannot be done, trying to run TrigFTKAna for the good regions"
	    for  gsb in $goodRegions; do
	        #action=[MERGED_ROADS=${MERGED_ROADS}],ana${gsb}
		echo $gsb
	    done
	fi
	
	ls -lR output/${runstring}_np${np};
	./ftk_explorer.py --batch -r ${NTowers} -s ${nsubs} -L ${MYnumLayers} -i FTKExplorerDFStudy.in -o FTKDFHistos.root -S FTKDFCache.pickle output/${runstring}_np${np};
	checkReturn
	
        # move the output files within the output directory
	myOutDir=output/${runstring}_np${np}_inp${inp}
	mkdir -p ${myOutDir}
	mv FTKExplorerDFStudy.data ${myOutDir}
	mv FTKDFHistos.root ${myOutDir}
	mv FTKDFCache.pickle ${myOutDir}
	mv FTKDF*.{eps,png} ${myOutDir}
	#removed merged roads and tracks if FTKExplorer is called
	find output/ -maxdepth 2 -type d -name merged -exec rm -rf {} \;
	find output/ -maxdepth 2 -type d -name tracks_merge -exec rm -rf {} \;
    fi

    # clean up and move the results into output directory that will be tarred up and saved in a dataset
    cp output/run/${runstring}/effcurve/${realRegion}/stdout* output/${runstring}/effcurve/${realRegion}/
    find output/ -maxdepth 2 -type d -name tracks -exec rm -rf {} \;
    du -shc output/*
    mkdir -p ${OUTPUTDIR}
    mv output/* ${OUTPUTDIR}/
}


# This function downloads files from a dataset that ran in per-subregion mode
# extracts them, and performs trackmerger/roadmerger for a given region or pattern point
# For now, this merges everything in one job - sequentially.
function effcurveTowers() {
    dataset=$1;      # output of TrigFTKSim running

    # get the options that the user should have passed along
    SKIP_MERGE_REGIONS=`extractOption SKIP_MERGE_REGIONS`
    DO_NOT_MERGE_ROADS=`extractOption DO_NOT_MERGE_ROADS`
    MERGED_ROADS=`extractOption MERGED_ROADS`;
    EXPLORE_FTK=`extractOption EXPLORE_FTK`;
    if [ ! -n "${MERGED_ROADS}" ]; then MERGED_ROADS=0; fi;
    if [ ! -n "${realRegion}" ]; then realRegion=0; fi;
    # download input datasets
    set +x
    # copy inputs if running effcurve (because effcurve script cannot run on dcache)
    getSomeWrapperFiles ${inputDS} 1

    set -x
    mkdir -p patt gcon
    regions=`seq 0 7`
    pattpoints=`python -c "from FTKCore import patternPoints; print ' '.join([str(i) for (i,k) in enumerate(patternPoints)])"`

    npattpoints=`echo $groups | awk '{print NF}'`

    if [ -s $STAGED ]; then
	cat ${STAGED_FILES} > dataset_contents
    else
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -r ${dataset}
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f ${dataset} > dataset_contents_all
	cat dataset_contents_all | grep '\[.\]' | cut -f 2 > dataset_contents
    fi

    badRegions="" # list of the uncompleted regions
    goodRegions="" # list of complete regions
    for igrp in $pattpoints; do # pattern points loop
	isBadGroup=0

	for reg in ${regions}; do # region loop
	    set +x
	    dld_cnt=0
	    unset FILES_TO_GET

	    nBadSubRegions=0

	    # Download FTK subjobs in current region/patternPoint
	    for sub in `seq 0 $(expr $nsubs - 1)`; do # sub-region loop
		# create dummy sectors files (may be needed for roadmerger)
		echo 'unused' > patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
		# download out_files.tbz2 from each subjob:
		# compose file name
		FileName="reg${reg}_sub${sub}_loop${igrp}.out_files.tbz2"

		echo "FileName  is $FileName"
		# account for extra suffixes, such as .tbz2.1 or .tbz2.2
		FileToGet=`cat dataset_contents | grep $FileName | head -n1`
		if [ ! -n "${FileToGet}" ]; then
		    echo "WARNING: Failed to find file=${FileName} in dataset=${dataset}"
		    echo "Region/patternPoint = ${igrp}"
		    echo "Subregion = ${sub}"
		    ((nBadSubRegions++))
		    echo "Attempting to continue..."
		    continue;
		fi;
		echo "FileToGet is ${FileToGet}"
		FILES_TO_GET[$dld_cnt]=${FileToGet}
		((dld_cnt++))
	    done; # end loop over subregions

	    if [ $nBadSubregions -ne $nsubs ]; then 
		isBadGroups=1
		break
	    fi

	    getFilesFromDS ${DQ2_LOCAL_SITE_ID} ${dataset} "${FILES_TO_GET[*]}" 1
	    echo "Untarring downloaded files..."
	    for FileToGet in ${FILES_TO_GET[*]}; do # file loop
		echo "Untarring ${FileToGet}"
		# cannot use tar directly on /pnfs files, so cat it first
		# if the files are not on dcap the direct use of tar is preferred and fixes some instability
		tar xvfj ${dataset}/${FileToGet}
		# delete original tar file:
		rm -f ${dataset}/${FileToGet}
		# delete subfolders of output/ that we don't need for merging
		# (e.g., in split arch, this will delete intermediate 8L results)
		if [ -d "output" ]; then
		    cd output/
		    local runstring4tmp=`relayerRunstring ${runstring} 4`  # preserve 4L to which 11L is symlinked
		    find . -maxdepth 1 -type d ! -name . -a ! -name run -a ! -name other -a ! -name "${runstring}*" -a ! -name "${runstring4tmp}*" -exec rm -rf {} \;
		    cd -
		fi;
		# delete roads if we don't plan to merge them
		if [ -n "${DO_NOT_MERGE_ROADS}" ]; then
		    find output/ -maxdepth 2 -type d -name roads -o -name merged -exec rm -rf {} \;
		fi;
	    done # end file loop
	    echo "Space usage after downloading and untarring:"
	    du -shc output/*
	done
    done

    if [ "$EXPLORE_FTK" != "" ]; then
	echo execute FTKExplorer after mergeSubregions for effcurveTowers
	echo Annovi: to complete 
	exit -1;

	
	ls -lR output/${runstring}_np${np};
	./ftk_explorer.py --batch -r ${NTowers} -s ${nsubs} -L ${MYnumLayers} -i FTKExplorerDFStudy.in -o FTKDFHistos.root -S FTKDFCache.pickle output/${runstring}_np${np};
	checkReturn
	
        # move the output files within the output directory
	myOutDir=output/${runstring}_np${np}_inp${inp}
	mkdir -p ${myOutDir}
	mv FTKExplorerDFStudy.data ${myOutDir}
	mv FTKDFHistos.root ${myOutDir}
	mv FTKDFCache.pickle ${myOutDir}
	mv FTKDF*.{eps,png} ${myOutDir}
	#removed merged tracks if FTKExplorer is called
	find output/ -maxdepth 2 -type d -name merged -exec rm -rf {} \;
	#find output/ -maxdepth 2 -type d -name tracks_merge -exec rm -rf {} \;
    fi

    action=[MERGED_ROADS=${MERGED_ROADS}],neweffcurve
    ./ftk.py "${extraArgs[@]}" -t ./ -d ${runstring} -r ./output/ -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${action}
    checkReturn
    
    # clean up and move the results into output directory that will be tarred up and saved in a dataset
    cp output/run/${runstring}/effcurve/${realRegion}/stdout* output/${runstring}/effcurve/${realRegion}/
    find output/ -maxdepth 2 -type d -name tracks -exec rm -rf {} \;
    du -shc output/*
    mkdir -p ${OUTPUTDIR}
    mv output/* ${OUTPUTDIR}/
}


# This function runs an analysis job on merged tracks and/or roads
function runAnalysis() {
    dataset=$1;      # output of TrigFTKSim running
    loopRegions=$2;  # loopRegions=1 loops over all regions; otherwise it loops over ipattPoints (effmode)
    realRegion=$3;   # if looping over ipattPoints, gives the real region number
    # get the options that the user should have passed along
    #SKIP_MERGE_REGIONS=`extractOption SKIP_MERGE_REGIONS`
    DO_NOT_MERGE_ROADS=`extractOption DO_NOT_MERGE_ROADS`
    MERGED_ROADS=`extractOption MERGED_ROADS`;
    if [ ! -n "${MERGED_ROADS}" ]; then MERGED_ROADS=0; fi;
    if [ ! -n "${realRegion}" ]; then realRegion=0; fi;
    # download input datasets
    set +x
    if isGridDS ${inputDS}; then
	getSomeWrapperFiles ${inputDS} ${COPY_FROM_DCACHE}
    fi;
    set -x

    # ftk.py needs the bank directories
    mkdir -p patt gcon

    if [ -s $STAGED ]; then
	cat ${STAGED_FILES} > dataset_contents
    else
	DQ2_LS_CMD -r ${dataset} 2>&1
	DQ2_LS_CMD -f ${dataset} 2>&1 > dataset_contents_all
	/usr/bin/python $(which dq2-ls) -f ${dataset} 2>&1 
	cat dataset_contents_all
	cat dataset_contents_all | grep '\[.\]' | cut -f 2 > dataset_contents
	cat dataset_contents
    fi

    set +x
    dld_cnt=0
    unset FILES_TO_GET

    FileName="reg0_sub0_loop0.out_files.tbz2"
    echo "FileName  is $FileName"
    # account for extra suffixes, such as .tbz2.1 or .tbz2.2
    FileToGet=`cat dataset_contents | grep $FileName | head -n1`
    if [ ! -n "${FileToGet}" ]; then
	echo "Failed: Failed to find file=${FileName} in dataset=${dataset}"
	exit -1
    fi;
    echo "FileToGet is ${FileToGet}"
    FILES_TO_GET[$dld_cnt]=${FileToGet}
    getFilesFromDS ${DQ2_LOCAL_SITE_ID} ${dataset} "${FILES_TO_GET[*]}" 1

    echo "Untarring downloaded files..."
    for FileToGet in ${FILES_TO_GET[*]}; do # file loop
	echo "Untarring ${FileToGet}"
    	# cannot use tar directly on /pnfs files, so cat it first
	cat ${dataset}/${FileToGet} | bzip2 -dc | tar xvf -
	# delete original tar file:
	rm -f ${dataset}/${FileToGet}
	# delete subfolders of output/ that we don't need for merging
	# (e.g., in split arch, this will delete intermediate 8L results)
	if [ -d "output" ]; then
	    cd output/
	    local runstring4tmp=`relayerRunstring ${runstring} 4`  # preserve 4L to which 11L is symlinked
	    find . -maxdepth 1 -type d ! -name . -a ! -name run -a ! -name other -a ! -name "${runstring}*" -a ! -name "${runstring4tmp}*" -exec rm -rf {} \;
	    cd -
	fi;
	# delete roads if we don't plan to merge them

	echo "Space usage after downloading and untarring:"
	du -shc output/*

	# check that we downloaded all subregions successfully
	# end loop over regions OR patternPoints
    done    
    
    # Set the analysis output file
    case ${ANAmode} in
	"ana")
	    outfileopt="-o anaout=ana.${inp}";;
	"effstudy")
	    outfileopt="-o anaeff=anaeff.${inp}";;
	*)
	    outfileopt="-o anaout=anatest.${inp}";;
    esac

##################################################################################################################                    
# Looking for existence of directory tree and setting $bad/goodRegions: this is done for tracks folder                                  
    if [ -d "output/${runstring}_np${maxpatterns}/tracks_merge/8" ]; then
	echo "There is a global merged folder"
    else
	for i in $(seq 0 $((NTowers-1))); do
	    if [ -d "output/${runstring}_np${maxpatterns}/tracks_merge/$i" ]; then
		if [ -z "$goodRegions" ]; then goodRegions="$i"; else goodRegions="$goodRegions $i"; fi
	    else
		if [ -z "$badRegions" ]; then badRegions="$i"; else badRegions="$badRegions $i"; fi
	    fi
	done
	echo "There isn't a global merged folder" 
	echo "Bad Regions = $badRegions"
	echo "Good Regions = $goodRegions"
    fi

#################################################################################################################

    # at this point, 8 individual trackmerger$reg jobs have been run, so we can use alltrackmerger for final merge:
    np=${maxpatterns}
    if [ -z "$badRegions" ]; then
	#action=[MERGED_ROADS=${MERGED_ROADS}],STOP,ana
	set -x
	./ftk.py "${extraArgs[@]}" -t ./ -d ${runstring}_np${np} -o ${extraTMopts}MAXPATTERNS=${np} -r ./output/ -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${ANAmode} $outfileopt
	checkReturn
	set +x
    else 
	echo "WARNING: alltrackmerger cannot be done, trying to run TrigFTKAna for the good regions"
	for  gsb in $goodRegions; do
	    #action=[MERGED_ROADS=${MERGED_ROADS}],ana${gsb}
	    set -x
	    ./ftk.py "${extraArgs[@]}" -t ./ -d ${runstring}_np${np} -o ${extraTMopts}MAXPATTERNS=${np} -r ./output/ -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${ANAmode}${gsb} $outfileopt
	    checkReturn
	    set +x
	done
    fi

    # clean up and move the results into output directory that will be tarred up and saved in a dataset
    find output/ -maxdepth 2 -type d -name merged -exec rm -rf {} \;
    find output/ -maxdepth 2 -type d -name tracks_merge -exec rm -rf {} \;
    du -shc output/*
    mkdir -p ${OUTPUTDIR}
    mv output/* ${OUTPUTDIR}/
}



### This function runs ftk_explorer on a sample to produce standard plots
function runExplorer() {
    echo STARTING runExplorer function $@
    dataset=$1;      # output of TrigFTKSim running
    loopRegions=$2;  # loopRegions=1 loops over all regions; otherwise it loops over ipattPoints (effmode)
    realRegion=$3;   # if looping over ipattPoints, gives the real region number
    # get the options that the user should have passed along
    #SKIP_MERGE_REGIONS=`extractOption SKIP_MERGE_REGIONS`
    DO_NOT_MERGE_ROADS=`extractOption DO_NOT_MERGE_ROADS`
    MERGED_ROADS=`extractOption MERGED_ROADS`;
    if [ ! -n "${MERGED_ROADS}" ]; then MERGED_ROADS=0; fi;
    if [ ! -n "${realRegion}" ]; then realRegion=0; fi;
    # download input datasets
    set +x
    if isGridDS ${inputDS}; then
	getSomeWrapperFiles ${inputDS} ${COPY_FROM_DCACHE}
    fi;
    set -x

    # ftk.py needs the bank directories
    mkdir -p patt gcon

    if [ -s $STAGED ]; then
	cat ${STAGED_FILES} > dataset_contents
    else
	DQ2_LS_CMD -r ${dataset} 2>&1
	DQ2_LS_CMD -f ${dataset} 2>&1 > dataset_contents_all
	/usr/bin/python $(which dq2-ls) -f ${dataset} 2>&1 
	cat dataset_contents_all
	cat dataset_contents_all | grep '\[.\]' | cut -f 2 > dataset_contents
	cat dataset_contents
    fi

    set +x
    dld_cnt=0
    unset FILES_TO_GET

    FileName="reg0_sub0_loop0.out_files.tbz2"
    echo "FileName  is $FileName"
    # account for extra suffixes, such as .tbz2.1 or .tbz2.2
    FileToGet=`cat dataset_contents | grep $FileName | head -n1`
    if [ ! -n "${FileToGet}" ]; then
	echo "Failed: Failed to find file=${FileName} in dataset=${dataset}"
	exit -1
    fi;
    echo "FileToGet is ${FileToGet}"
    FILES_TO_GET[$dld_cnt]=${FileToGet}
    getFilesFromDS ${DQ2_LOCAL_SITE_ID} ${dataset} "${FILES_TO_GET[*]}" 1

    echo "Untarring downloaded files..."
    for FileToGet in ${FILES_TO_GET[*]}; do # file loop
	echo "Untarring ${FileToGet}"
    	# cannot use tar directly on /pnfs files, so cat it first
	cat ${dataset}/${FileToGet} | bzip2 -dc | tar xvf -
	# delete original tar file:
	rm -f ${dataset}/${FileToGet}
	# delete subfolders of output/ that we don't need for merging
	# (e.g., in split arch, this will delete intermediate 8L results)
	if [ -d "output" ]; then
	    cd output/
	    local runstring4tmp=`relayerRunstring ${runstring} 4`  # preserve 4L to which 11L is symlinked
	    find . -maxdepth 1 -type d ! -name . -a ! -name run -a ! -name other -a ! -name "${runstring}*" -a ! -name "${runstring4tmp}*" -exec rm -rf {} \;
	    cd -
	fi;
	# delete roads if we don't plan to merge them

	echo "Space usage after downloading and untarring:"
	du -shc output/*

	# check that we downloaded all subregions successfully
	# end loop over regions OR patternPoints
    done    
    
    # uses the EXPmode variable to define the kind of study to be done
    echo ${EXPmode}

##################################################################################################################                    
# Looking for existence of directory tree and setting $bad/goodRegions: this is done for tracks folder                                  
    if [ -d "output/${runstring}_np${maxpatterns}/tracks_merge/${NTowers}" ]; then
	echo "There is a global merged folder"
    else
	for i in $(seq 0 $((NTowers-1))); do
	    if [ -d "output/${runstring}_np${maxpatterns}/tracks_merge/$i" ]; then
		if [ -z "$goodRegions" ]; then goodRegions="$i"; else goodRegions="$goodRegions $i"; fi
	    else
		if [ -z "$badRegions" ]; then badRegions="$i"; else badRegions="$badRegions $i"; fi
	    fi
	done
	echo "There isn't a global merged folder" 
	echo "Bad Regions = $badRegions"
	echo "Good Regions = $goodRegions"
    fi

#################################################################################################################

    # at this point, 8 individual trackmerger$reg jobs have been run, so we can use alltrackmerger for final merge:
    np=${maxpatterns}

    if [ -z "$badRegions" ]; then
	echo No bad regions
	#action=[MERGED_ROADS=${MERGED_ROADS}],STOP,ana
    else 
	echo "WARNING: alltrackmerger cannot be done, trying to run TrigFTKAna for the good regions"
	for  gsb in $goodRegions; do
	    #action=[MERGED_ROADS=${MERGED_ROADS}],ana${gsb}
	    echo $gsb
	done
    fi

    ./ftk_explorer.py --batch -r ${NTowers} -s ${nsubs} -L ${MYnumLayers} -i FTKExplorerDFStudy.in -o FTKDFHistos.root -S FTKDFCache.pickle output/${runstring}_np${np}
    checkReturn
    
    # move the output files within the output directory
    myOutDir=output/${runstring}_np${np}_inp${inp}
    mkdir -p ${myOutDir}
    mv FTKExplorerDFStudy.data ${myOutDir}
    mv FTKDFHistos.root ${myOutDir}
    mv FTKDFCache.pickle ${myOutDir}
    mv FTKDF*.{eps,png} ${myOutDir}
    
    # clean up and move the results into output directory that will be tarred up and saved in a dataset
    find output/ -maxdepth 2 -type d -name merged -exec rm -rf {} \;
    find output/ -maxdepth 2 -type d -name tracks_merge -exec rm -rf {} \;
    du -shc output/*
    mkdir -p ${OUTPUTDIR}
    mv output/* ${OUTPUTDIR}/
}

# Generalized function that runs 11L/7L/8L/TSP TrigFTKSim jobs
# Arguments: arrayOfRegions arrayOfSubregions arrayOfPatternPoints
# Supports running with N*M jobs, or N jobs (all subregions processed in one grid job)
# In the latter case, it will also merge and analyze the tracks in the region
function runTSP() {
    local regions=$1
    local subregions=$2
    local patternPoints=$3
    local extra_setup=$4
    local split_mode=$5
    echo "Running TSP bank..."
    echo regions ${regions} 
    echo subregions ${subregions} 
    echo patternPoints ${patternPoints}
    echo extra_setup ${extra_setup}
    echo split_mode ${split_mode}
    echo UNSPLIT_SECTORS ${UNSPLIT_SECTORS}
    echo MAKECACHE ${MAKECACHE}
    echo SECTORSASPATTERNS ${SECTORSASPATTERNS}
    # if we are processing only one subregion, disable road/track merging
    nsubrun=`echo $subregions | awk '{print NF;}'`  # number of subregions to be processed in this batch
    MERGED_ROADS=0 # MERGED_ROADS is now always 0, 1 should be used only if option B is restored
    if [ "${nsubrun}" -eq "1" -a "${nsubs}" -gt "1" ]; then MERGED_ROADS=0; fi;
    # download input datasets
    set +x
    if isGridDS ${inputDS}; then
	getSomeWrapperFiles ${inputDS} ${COPY_FROM_DCACHE}
    fi;
    set -x
    runstring_orig=${runstring}
    numLayers_orig=${numLayers}
    baseDS_orig=${baseDS}
    for reg in ${regions}; do
        # run roadfinder in each subregion, deleting the large patt files once they are not needed anymore:
	runstring=${runstring_orig}
	numLayers=${numLayers_orig}
	baseDS=${baseDS_orig}
	# in split mode, make sure we have a sector lookup table
	if [ "${split_mode}" == "1" ]; then
	    if isGridDS ${baseDS}; then
		retrieveBankFiles ${baseDS} lookup/sectors_4_8_11_reg${reg}.${_DAT}
	    else
		checkFiles ${bankdir}  lookup/sectors_4_8_11_reg${reg}.${_DAT}
	    fi
	fi;
	sublist=""
	for sub in ${subregions}; do
	    date
	    sublist="${sublist}_${sub}"
	    # we no longer need sectors
	    if [ "0" -eq "1" ]; then
		if isGridDS ${baseDS}; then
		    retrieveBankFiles ${baseDS} patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
		else
		    checkFiles ${bankdir} patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
		fi
	    else
		mkdir -p patt/
		echo "unused" > patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
	    fi;
	    if isGridDS ${bankdir}; then
		if [ -n "$SECTORSASPATTERNS" ]; then # use sector file as pattern file
		    if [ ${nsubs} -eq 1 ]; then
			retrieveBankFiles ${bankdir} patt/sectors_raw_${bankNumLayers}_reg${reg}.${_PATT}
			ln -s sectors_raw_${bankNumLayers}_reg${reg}.${_PATT} patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT};
		    else
			retrieveBankFiles ${bankdir} patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
			ln -s sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT} patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT};
		    fi
		    ls -l ${bankdir} patt/
		else
		    retrieveBankFiles ${bankdir} patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
		fi;
	    else
		checkFiles ${bankdir} patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
	    fi
	    for np in $patternPoints; do
		echo "Running roadfinder: reg=${reg}, sub=${sub}, MAXPATTERNS=${np}"
		# important note: we now run an additional checkroads action that makes sure that
		# we have the same number of output road ntuples in each subregion.
		./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},_DAT=${_DAT},MERGED_ROADS=${MERGED_ROADS} -a ${extra_setup},${RFmode}${reg}_${sub},STOP,checkroads${reg}${sublist}
		checkReturn
		if [ -n "$MAKECACHE" ]; then
		    echo Saving pcache output
		    mv patt/*pcache* ${OUTPUTDIR}/../
		fi 
		echo "Space usage after running ${RFmode}${reg}_${sub}:"
		du -shc ${OUTPUTDIR}/* ${OUTPUTDIR}/../*pcache*
		find . -maxdepth 2
	    done;
	    rm -f patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
	done;
	rm -f lookup/sectors_4_8_11_reg${reg}.${_DAT}
	for np in $patternPoints; do
	    ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=${MERGED_ROADS} -a ${extra_setup},check
	    checkReturn
            # run roadmerger with an option to delete original (unmerged) roads
	    if [ "${MERGED_ROADS}" -eq "1" ]; then
		./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},DELETE_ROADS_AFTER_USE=1 -a ${extra_setup},roadmerger${reg},makelinks
		checkReturn
	    fi;
	done;
        # clean up sectors
	for sub in ${subregions}; do
	    rm -f patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
	done;
	# in split mode, track fitting is done using 11L configuration
	if [ "${split_mode}" == "1" ]; then
	    echo "Relayering runstring: ${runstring}"
	    numLayers=${nLayersFinalFit}L
	    bankNumLayers=${nLayersFinalFit}L
	    runstring=`relayerRunstring ${runstring} 11`
	    echo "New runstring: ${runstring}"
	fi;

        # download constants
	if [ -n "$UNSPLIT_SECTORS" ]; then # constants aren't splitted in sub-regions
	    if isGridDS ${baseDS}; then
		retrieveBankFiles ${baseDS} gcon/corrgen_raw_${bankNumLayers}_reg${reg}.${_GCON}
	    else
		checkFiles ${bankdir} gcon/corrgen_raw_${bankNumLayers}_reg${reg}.${_GCON}
	    fi
	    if [ "${mode}" == "711" -o "${TFmode}" == "trackfitter711" ]; then
		# if in 711 mode download also the synchronized set
		if isGridDS ${baseDS}; then
		    retrieveBankFiles ${baseDS} gcon/corrgen_raw_${nLayersFinalFitOld}L_reg${reg}.${_GCON}
		    if [ "${TRACKFITTER_MODE}" == "1" ] || [ "${TRACKFITTER_MODE}" == "3" ]; then
                        retrieveBankFiles ${baseDS} patt/sectors_raw_${bankNumLayers}_reg${reg}.conn
		    fi
		else
		    checkFiles ${bankdir} gcon/corrgen_raw_${nLayersFinalFitOld}L_reg${reg}.${_GCON}
		    if [ "${TRACKFITTER_MODE}" == "1" ] || [ "${TRACKFITTER_MODE}" == "3" ]; then
                        checkFiles ${bankdir} patt/sectors_raw_${bankNumLayers}_reg${reg}.conn # 7/8 TODO : unhardcode
		    fi
		fi
	    fi
	else # case of constants splitted in sub-regions
	    for sub in ${subregions}; do
		if isGridDS ${baseDS}; then
		    retrieveBankFiles ${baseDS} gcon/corrgen_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
		else
		    checkFiles ${bankdir} gcon/corrgen_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
		fi
		if [ "${mode}" == "711" -o "${TFmode}" == "trackfitter711" ]; then
		# if in 711 mode download also the synchronized set
		    if isGridDS ${baseDS}; then
			retrieveBankFiles ${baseDS} gcon/corrgen_raw_${nLayersFinalFitOld}L_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
			if [ "${TRACKFITTER_MODE}" == "1" ] || [ "${TRACKFITTER_MODE}" == "3" ]; then
                            retrieveBankFiles ${baseDS} patt/sectors_raw_8L_${nsubs}M_reg${reg}_sub${sub}.conn
			fi
		    else
			checkFiles ${bankdir} gcon/corrgen_raw_${nLayersFinalFitOld}L_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
			if [ "${TRACKFITTER_MODE}" == "1" ] || [ "${TRACKFITTER_MODE}" == "3" ]; then
                            checkFiles ${bankdir} patt/sectors_raw_8L_${nsubs}M_reg${reg}_sub${sub}.conn # 7/8 TODO : unhardcode
			fi
		    fi
		fi
	    done;
	fi # end fit constants download block

	for np in $patternPoints; do
            # run trackfitter, a single call will run all the subregions
	    if [ "${MERGED_ROADS}" -eq "1" ]; then
		./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${extra_setup},${TFmode}${reg},STOP,checktracks${reg},STOP,check
		checkReturn
	    else
		for sub in ${subregions}; do
		    ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -a ${extra_setup},${TFmode}${reg}_${sub},STOP,checktracks${reg}_${sub},STOP,check
		    checkReturn
		done
	    fi;
	done;

        # clean up constants
	if [ -n "$UNSPLIT_SECTORS" ]; then # constants aren't splitted in sub-regions
	    rm -f gcon/corrgen_raw_${bankNumLayers}_reg${reg}.${_GCON}
	    if [ "${TFmode}" == "trackfitter711" ]; then
		rm -f gcon/corrgen_raw_${nLayersFinalFitOld}L_reg${reg}.${_GCON}
		if [ "${TRACKFITTER_MODE}" == "1" ] || [ "${TRACKFITTER_MODE}" == "3" ]; then
                    rm -f patt/sectors_raw_${bankNumLayer}_reg${reg}.conn
		fi
	    fi
	else
	    for sub in ${subregions}; do
		rm -f gcon/corrgen_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
		if [ "${TFmode}" == "trackfitter711" ]; then
		    rm -f gcon/corrgen_raw_${nLayersFinalFitOld}L_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
		    if [ "${TRACKFITTER_MODE}" == "1" ] || [ "${TRACKFITTER_MODE}" == "3" ]; then
                        rm -f patt/sectors_raw_${bankNumLayer}_${nsubs}M_reg${reg}_sub${sub}.conn
		    fi
		fi	    
	    done
	fi

	# if all subregions have been run, we can merge the tracks and run analysis/plot jobs
	if [ "${MERGED_ROADS}" -eq "1" ]; then
	    for np in $patternPoints; do
                # run trackmerger and analysis (will only work if TrigFTKAna was supplied), then print summary
		# note that unmerged roads have already been deleted by this point!
		if [ ${TFmode} == "trackfitter711" ]; then
		    # in 7/11 mode the final tracks have 14 coordinates,
		    # despite the use of 7L pattern banks
		    extraTMargs="[ncoords=14],"
		else
		    extraTMargs=""
		fi
		./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -o MAXPATTERNS=${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON} -a ${extra_setup},makelists${reg},${extraTMargs},trackmerger${reg},STOP,ana${reg},plot_${reg},check
		checkReturn
	    done;
	fi;
    done;

    DO_NOT_WRITE_ROADS=`extractOption DO_NOT_WRITE_ROADS`
    # delete roads if we don't plan to merge them
    if [ -n "${DO_NOT_WRITE_ROADS}" ]; then
        find ${OUTPUTDIR} -maxdepth 2 -type d -name roads -exec rm -rvf {} \;
    fi;
}

# Generalized function that runs split (8L+4L) TrigFTKSim jobs IN REGIONS (!)
# Arguments: arrayOfRegions arrayOfPatternPoints[4L]
# Supports running with N jobs ONLY (all subregions processed in one grid job)
# It will also merge and analyze the tracks in the region
# Main idea: use runTSP() to run 8L and 4L parts
function runSPLIT_reg() {
    local regions=$1
    local patternPoints=$2   # refers to 4L pattern points!
    runstring4=`extractOption runstring4`  # 4L runstring that also encodes nsubs: raw_4L_25x156I225x36_dataset_1M
    if [ "${runstring4}" == "" ]; then
	echo 'ERROR: you must specify -o runstring4=raw_4L_25x156I225x36_dataset_1M to run in split mode'
	exit -1;
    fi;
    # load split arch settings from runCM.sh script
    sed -i -e 's#local_enabled=true#local_enabled=false#' ./runCM.sh
    source ./runCM.sh

    # run 8L part. We assume the default -d / -m / MAXPATTERNS parameters refer to 8L settings
    # first, set up 8L banks
    bankdir8=`extractOption bankdir8`
    if [ -n "$bankdir8" ]; then 
	bankdir=${bankdir8}
    fi;
    baseDS8=`extractOption baseDS8`
    if [ -n "$baseDS8" ]; then 
	baseDS=${baseDS8}
    fi;
    setBankDirs
    # additional settings
    runstring=`relayerRunstring ${runstring} 8`
    nsubs=`extractNsubsFromRunstring ${runstring}`
    nsubsMinus1=`expr $nsubs - 1`
    nsubs8=${nsubs}  # needs to be passed to 4L stage as SCTTRK_NSUBS
    maxpatterns8=`extractOption MAXPATTERNS8`
    if [ "${maxpatterns8}" == "" ]; then maxpatterns8=${maxpat8}; fi;
    extra8Lops="[HITWARRIOR=${HW8}:HW_NDIFF=${HW_NDIFF8}:SCTTRK_MODE=0:ROADWARRIOR=${RW8}:REQUIRE_FIRST=0:MAX_MISSING_PLANES=${nsctmiss}:MAX_MISSING_SCT_PAIRS=${nsctmisspair}:RESTRICT_SCT_PAIR_LAYER=${restrictsctplayer}:RESTRICT_SCT_PAIR_MODULE=${restrictsctpmod}]"
    runTSP "${regions}" "`seq 0 ${nsubsMinus1}`" "${maxpatterns8}" "${extra8Lops}"
    
    # run 4L part (after redefining some variables)
    # first, set up 4L/11L banks
    bankdir4=`extractOption bankdir4`
    if [ -n "$bankdir4" ]; then 
	bankdir=${bankdir4}
    fi;
    baseDS4=`extractOption baseDS4`
    if [ -n "$baseDS4" ]; then 
	baseDS=${baseDS4}
    fi;
    setBankDirs
    # additional settings
    runstring=${runstring4}
    SSname=`echo ${runstring} | awk 'BEGIN{FS="_";}; {print $3}'`
    nsubs=`extractNsubsFromRunstring ${runstring}`
    nsubsMinus1=`expr $nsubs - 1`
    numLayers=4L
    bankNumLayers=4L
    extra4Lops="[SCTTRK_MODE=1:SCTTRK_NSUBS=${nsubs8}:ROADWARRIOR=0:REQUIRE_FIRST=${requireb}:MAX_MISSING_PLANES=${npixmiss}:MAX_MISSING_SCT_PAIRS=${nsctmisspair}:RESTRICT_SCT_PAIR_LAYER=${restrictsctplayer}:RESTRICT_SCT_PAIR_MODULE=${restrictsctpmod}:SCTTRK_MAX_MISSING_PLANES=${nscttrkmiss}:HW_NDIFF=${HW_NDIFF11}:HITWARRIOR=${HW11}:ROADWARRIOR=${RW11}]"
    runTSP "${regions}" "`seq 0 ${nsubsMinus1}`" "${patternPoints}" "${extra4Lops}" "1"
}

# !!!!!!!!!!THIS CODE IS NOT READY YET - USE runSPLIT_reg FOR NOW!!!!!!!!!!
# Generalized function that runs split (8L+4L) TrigFTKSim jobs IN SUBREGIONS
# Arguments: patternPoint
# Main idea: download ALL needed files first, and then use "-a newsplit" in ftk.py
function runSPLIT() {
    local np=$1   # refers to 4L pattern points!
    runstring4=`extractOption runstring4`  # 4L runstring that also encodes nsubs: raw_4L_25x156I225x36_dataset_1M
    if [ "${runstring4}" == "" ]; then
	echo 'ERROR: you must specify -o runstring4=raw_4L_25x156I225x36_dataset_1M to run in split mode'
	exit -1;
    fi;
    # split arch settings are defined inside FTKCore.py and can be overridden via the sp_xxxx variables.
    
    # Prepare 8L part. We assume the default -d / -m / MAXPATTERNS parameters refer to 8L settings
    bankdir8=`extractOption bankdir8`
    if [ -n "$bankdir8" ]; then 
	bankdir=${bankdir8}
    fi;
    baseDS8=`extractOption baseDS8`
    if [ -n "$baseDS8" ]; then 
	baseDS=${baseDS8}
    fi;
    setBankDirs
    runstring=`relayerRunstring ${runstring} 8`
    SSname=`echo ${runstring} | awk 'BEGIN{FS="_";}; {print $3}'`
    ss8L=${SSname}
    nsubs=`extractNsubsFromRunstring ${runstring}`
    nsubs8=${nsubs}  # needs to be passed to 4L stage as SCTTRK_NSUBS
    maxpat8=`extractOption MAXPATTERNS8`
    numLayers=8L
    bankNumLayers=8L
    mkdir -p patt/
    echo "unused" > patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
    retrieveBankFiles ${baseDS} gcon/corrgen_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub}.${_GCON}
    retrieveBankFiles ${bankdir} patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
    
    # run 4L part (after redefining some variables)
    # first, set up 4L/11L banks
    bankdir4=`extractOption bankdir4`
    if [ -n "$bankdir4" ]; then 
	bankdir=${bankdir4}
    fi;
    baseDS4=`extractOption baseDS4`
    if [ -n "$baseDS4" ]; then 
	baseDS=${baseDS4}
    fi;
    setBankDirs
    # additional settings
    runstring=${runstring4}
    SSname=`echo ${runstring} | awk 'BEGIN{FS="_";}; {print $3}'`
    ss4L=${SSname}
    nsubs=`extractNsubsFromRunstring ${runstring}`
    nsubs4=${nsubs}
    maxpat4=`extractOption MAXPATTERNS4`
    numLayers=4L
    bankNumLayers=4L
    extra4Lops="[SCTTRK_MODE=1:SCTTRK_NSUBS=${nsubs8}:ROADWARRIOR=0:REQUIRE_FIRST=${requireb}:MAX_MISSING_PLANES=${npixmiss}:MAX_MISSING_SCT_PAIRS=${nsctmisspair}:RESTRICT_SCT_PAIR_LAYER=${restrictsctplayer}:RESTRICT_SCT_PAIR_MODULE=${restrictsctpmod}:SCTTRK_MAX_MISSING_PLANES=${nscttrkmiss}:HW_NDIFF=${HW_NDIFF11}:HITWARRIOR=${HW11}:ROADWARRIOR=${RW11}]"

    retrieveBankFiles ${baseDS} lookup/sectors_4_8_11_reg${reg}.${_DAT}
    # we need ALL of 4L subregions:
    for sub4 in `seq 0 ${nsubs}`; do
	# we no longer need sectors
	mkdir -p patt/
	echo "unused" > patt/sectors_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub4}.${_PATT}
	retrieveBankFiles ${baseDS} gcon/corrgen_raw_${bankNumLayers}_${nsubs}M_reg${reg}_sub${sub4}.${_GCON}
	retrieveBankFiles ${bankdir} patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub4}.${_PATT}
    done;

    ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -d ${runstring}_np${np} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},_DAT=${_DAT} -o sp_base8=./,sp_ss8L=${ss8L},sp_nsubs8=${nsubs8},sp_maxpat8=${maxpat8},sp_base4=./,sp_ss4L=${ss4L},sp_nsubs4=${nsubs4},sp_maxpat4=${maxpat4} -a newsplit${reg}_${sub};
    checkReturn
}

# A workhorse function that reduces the size of a pattern file down to maxpat patterns
# maxpat=-1 simply makes a copy of the input pattern file (useful to convert .tbz2 patterns to .patt.bz2)
# this function can read bzip'ed and/or dcache-hosted pattern files
function reduce_pattern_size() {
    local infile=$1
    local outfile=$2
    local maxpat=$3
    echo "In reduce_pattern_size: IN=${infile}, OUT=${outfile}, MAXPAT=${maxpat}"
    if [ ! -s "${infile}" ]; then
	echo "ERROR: cannot find pattern file ${infile}"
	exit 1;
    fi;
    # bunzip2 on the fly?
    INCAT=cat
    file -L ${infile} | grep -q bz2 && INCAT="bzip2 -dc"
    echo "READING the pattern file with ${INCAT}"
    # bzip2 -9 on the fly?
    OUTCAT=cat
    echo ${outfile} | grep -q bz2 && OUTCAT='bzip2 -9'
    echo "WRITING the pattern file with ${OUTCAT}"
    echo "cat ${infile} | ${INCAT} | awk -v maxpat=${maxpat} 'BEGIN{n=0};NR==1{$1=maxpat;print};NR!=1{print;n++;if(n>=maxpat&&maxpat>0)exit(0)}' | ${OUTCAT} > $outfile"
    cat ${infile} | ${INCAT} | awk -v maxpat=${maxpat} 'BEGIN{n=0};NR==1{$1=maxpat;print};NR!=1{print;n++;if(n>=maxpat&&maxpat>0)exit(0)}' | ${OUTCAT} > $outfile
}

# download a pattern file and reduce it
function runREDUCE () {
    infile=patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
    outfile=${ROOTDIR}/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.patt.bz2
    retrieveBankFiles ${bankdir} ${infile}
    reduce_pattern_size ${infile} ${outfile} ${maxpatterns}
    rm ${infile}
    # create a dummy output directory for Panda
    if [ -s ${outfile} ]; then
	mkdir ${OUTPUTDIR}
	echo "DONE!" > ${OUTPUTDIR}/INFO
	/bin/ls -l ${outfile} >> ${OUTPUTDIR}/INFO
    fi;
}

# download the pattern bank and transform it in a DB format.
function runMakeDCBank () {
    # set the position of the input and output banks
    infile=patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
    outfile=${ROOTDIR}/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATTDB}

    # get the input
    retrieveBankFiles ${bankdir} ${infile}
    
    INPUTBANK=${SCRIPTDIR}/${infile}
    ./ftk.py -a makedc${reg}_${sub} -o INBANK=${INPUTBANK},OUTBANK=${outfile} "${extraArgs[@]}" ${NEWBANKDIR[@]}
    checkReturn

    mkdir -p ${OUTPUTDIR}
    ../standalone/ftk_DCBankStat ${outfile} ${OUTPUTDIR}/DCBankStat_r${reg}_s${sub}.root
    checkReturn

    rm -f ${infile}
    #echo "Create the DB index on Pattern.fatherID filed"
    #sqlite3 $outfile 'create index fatherID_idx on Pattern(fatherID);' || echo "Failed"
    #echo "Bank convertion ended"
   
}

# Workhorse function that changes the subregion division in a pattern bank
function resub_patterns () {
    local infiles=$1
    local outfile=$2
    local factor=$3
    local maxpat=$4
    local unsplit_sectors=$5
    echo "In resub_patterns: OUT=${outfile}, FACTOR=${factor}, MAXPAT=${maxpat}, UNSPLIT_SECTORS=${unsplit_sectors}"
    echo "IN = ${infiles}"
    # check input pattern files
    for infile in ${infiles}; do
	if [ ! -s "${infile}" ]; then
	    echo "ERROR: cannot find pattern file ${infile}"
	    exit 1;
	fi;
    done;
    # bunzip2 on the fly?
    INCAT=cat
    file -L ${infile} | grep -q bz2 && INCAT="bzip2 -dc"
    echo "READING pattern files with ${INCAT}"
    # bzip2 -9 on the fly?
    OUTCAT=cat
    echo ${outfile} | grep -q bz2 && OUTCAT='bzip2 -9'
    echo "WRITING the pattern file with ${OUTCAT}"
    rm -f info.L info.N
    cat ${infiles} | ${INCAT} | ${SCRIPTDIR}/./merge_subs.awk -v factor=${factor} -v maxpat=${maxpat} -v unsplit_sectors=${unsplit_sectors} | bzip2 -9 > tmp.patt.bz2
    L=`head -n1 info.L`
    N=`head -n1 info.N`
    bzcat tmp.patt.bz2 | sort -rnk `expr $L + 3` | awk -v L=$L -v N=$N 'BEGIN{print N,L; i=0;} NF>3{$1=i; print $0; i++;}' | ${OUTCAT} > $outfile
    echo PRINTING out first lines of the output file with bzcat
    bzcat $outfile | head
    rm -f tmp.patt.bz2
}

# changes the subregion division in a pattern bank
# this allows to, eg, convert 16M bank to a 4M bank
# should be called with -d string of the *final* pattern bank
function runRESUB () {
    factor=`extractOption factor`
    echo UNSPLIT_SECTORS ${UNSPLIT_SECTORS}
    if [ ! -n "${factor}" ]; then factor=2; fi;
    nsubs_orig=`expr ${factor} \* ${nsubs}`
    echo "Converting reg=${reg} sub=${sub} from ${nsubs_orig} to ${nsubs} subregions"
    infiles=""
    for ((i=0;i<$factor;i++)); do
	isub=`expr ${sub} + ${i} \* ${nsubs}`
	infiles="$infiles patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs_orig}M_reg${reg}_sub${isub}.${_PATT}"
    done;
    outfile=${ROOTDIR}/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.patt.bz2
    retrieveBankFiles ${bankdir} ${infiles}
    resub_patterns "${infiles}" ${outfile} ${factor} ${maxpatterns} ${UNSPLIT_SECTORS}
    rm ${infiles}
    # create a dummy output directory for Panda
    if [ -s ${outfile} ]; then
	mkdir ${OUTPUTDIR}
	echo "DONE!"
	echo "DONE!" > ${OUTPUTDIR}/INFO
	/bin/ls -l ${outfile}
	/bin/ls -l ${outfile} >> ${OUTPUTDIR}/INFO
    fi;
}

# Main function that run patterns from constants production
# The results of this run need to be merged using mode=merge_pconst
# It supports bootstrapping 8L and 4L patterns from 11L patterns
function runPCONST () {
    runNum=`extractOption runNum`
    echo UNSPLIT_SECTORS ${UNSPLIT_SECTORS}
    if [ -n "${runNum}" ]; then
	run=${runNum}
    else
	run=1
    fi;
    #redefine reg and sub for mode 9
    reg=${reg}
    sub=${loop}  # enumerates subruns (aka patt-from-const group loops)
    ## Please note that in mode 9 the "sub" used by ftk.py is actually a subrun number.
    ## Files will be properly renamed during the splitting step

    echo "Making patterns from constants in region $reg, subregion (aka subrun) $sub, run $run"
    FILELIST_unz=`echo {gcon/slices_${bankNumLayers}_reg${reg}.root,gcon/corrgen_raw_${bankNumLayers}_reg${reg}.gcon,patt/sectors_raw_${bankNumLayers}_reg${reg}.patt}`;
    FILELIST=`echo {gcon/slices_${bankNumLayers}_reg${reg}.root,gcon/corrgen_raw_${bankNumLayers}_reg${reg}.${_GCON},patt/sectors_raw_${bankNumLayers}_reg${reg}.${_PATT}}`;
    retrieveBankFiles $baseDS $FILELIST;
    if [ "${pc_mode}" == "1" ]; then
	# In PC_MODE=1, we make 8L patterns by bootstrapping from 11L. The -d string specifies 11L
	# but we also need to explicitly get 8L sectors to accomplish sector lookup.
	# These 8L sectors are stored in -o baseDS8=blabla command line variable
	baseDS8=`extractOption baseDS8`
	retrieveBankFiles ${baseDS8} patt/sectors_raw_8L_reg${reg}.${_PATT}
	# add them to the FILELIST so they are deleted later
	FILELIST_unz=`echo {${FILELIST_unz},patt/sectors_raw_8L_reg${reg}.patt}`
	FILELIST=`echo {${FILELIST},patt/sectors_raw_8L_reg${reg}.${_PATT}}`
    fi;
    if [ "${pc_mode}" == "2" ]; then
	# In PC_MODE=2, we make 4L patterns by bootstrapping from 11L. The -d string specifies 11L
	# but we also need to explicitly get 4L sectors and 8L sectors&constants (for on-the-fly 8L fitting).
	# Use -o baseDS8=blabla and -o baseDS4=bloblo to point to these sectors and constants
	baseDS4=`extractOption baseDS4`
	retrieveBankFiles ${baseDS4} patt/sectors_raw_4L_reg${reg}.${_PATT}
	baseDS8=`extractOption baseDS8`
	retrieveBankFiles ${baseDS8} patt/sectors_raw_8L_reg${reg}.${_PATT} gcon/corrgen_raw_8L_reg${reg}.${_GCON}
	# add them to the FILELIST so they are deleted later
	FILELIST_unz=`echo {${FILELIST_unz},patt/sectors_raw_8L_reg${reg}.patt,patt/sectors_raw_4L_reg${reg}.patt,gcon/corrgen_raw_8L_reg${reg}.gcon}`
	FILELIST=`echo {${FILELIST},patt/sectors_raw_8L_reg${reg}.${_PATT},patt/sectors_raw_4L_reg${reg}.${_PATT},gcon/corrgen_raw_8L_reg${reg}.${_GCON}}`
    fi;

    # since we are using TrigFTKLib, all input files must be unzipped and not on dcache:
    for f in $FILELIST; do
	newf=`echo $f | sed -e 's#.bz2$##g'`
	basefn=$(basename $f)
	exten=${basefn##*.}
	if [ "$exten" == "root" ]; then
	    # root files (slices)
	    cp $f ${f}.tmp
	    if [ -f "${f}.tmp" ]; then
		rm -f $f
		mv ${f}.tmp $f
	    fi
	else
	    # zipped files
	    file -L ${f} | grep -q bz2 && (cat ${f} | bzip2 -dc > $newf)
	fi
    done
    echo "Running..."
    # run patterns from constants

#    echo ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -q ${queue} -r ${OUTPUTDIR} -t ${PWD} -I `pwd`/ftk.py -a patterns${reg}_${sub}__${run}
    INPUT_FILENAME=patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run${run}.patt;
    if [ "$PATMERGEROOT" == "1" ]; then
# execute several loops, collect input files in
        for ((rep=0;rep<nsubs;rep++)) ; do
            echo date before running pattern job
            date
            ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -q ${queue} -r ${OUTPUTDIR} -t ${PWD} -I `pwd`/ftk.py -a patterns${reg}_${sub}__${run}
            echo date after running pattern job
            date
            checkReturn
            echo ls -l ${INPUT_FILENAME}
            ls -l ${INPUT_FILENAME}
            echo mv ${INPUT_FILENAME} patt/patt${rep}.bz2
            mv ${INPUT_FILENAME} patt/patt${rep}.bz2
            echo "===================================="
            echo "bzcat  patt/patt${rep}.bz2 | head -5"
            echo "  ..."
            echo "bzcat  patt/patt${rep}.bz2 |tail -5"
            bzcat  patt/patt${rep}.bz2 | head -5
            echo "  ..."
            bzcat  patt/patt${rep}.bz2 |tail -5
            echo "===================================="
        done
# create link
#    patt/patterns_raw_*.patt -> sub0.bz2
        ln -s patt0.bz2 ${INPUT_FILENAME}
        echo "ls -l patt/"
        echo "==========="
        ls -l patt/
        echp "==========="
    else
        echo date before running pattern job
        date
        ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -q ${queue} -r ${OUTPUTDIR} -t ${PWD} -I `pwd`/ftk.py -a patterns${reg}_${sub}__${run}
        echo date after running pattern job
        date
        checkReturn
    fi
    echo
    echo "Last 100 lines of stdout:"
    tail -n 100 ${OUTPUTDIR}/run/raw_*/patterns/${reg}/${sub}/${run}/stdout.*.log
    echo "Last 100 lines of stderr:"
    tail -n 100 ${OUTPUTDIR}/run/raw_*/patterns/${reg}/${sub}/${run}/stderr.*.log
    echo "Freeing up scratch space by deleting: ${FILELIST_unz}"
    for f in ${FILELIST_unz}; do
	rm -f ${f}
    done;
    echo "Directory listing:"
    find . -maxdepth 2 -exec ls -ld \{\} \;

    echo "Unsplit pattern file: ${INPUT_FILENAME} (already zipped)"
    if [ -s "${INPUT_FILENAME}" ]; then
	echo "Done generating patterns-from-constants:"
    else
	echo "ERROR: looks like patterns-from-const job failed or was killed"
	echo "Since no patterns were produced, quitting now"
	echo
	echo Printing log file
	echo
	LOGDIR="${OUTPUTDIR}/run/${dSTRING}/patterns/${reg}/${sub}/${run}/"
	CMD="ls -rlta $LOGDIR"
	echo running $CMD
	$CMD;
	CMD="tail -n1000 $LOGDIR/std*"
	echo running $CMD
	$CMD;
	echo
	echo Now Exiting...
	echo
	exit -1;
    fi;
    echo "head -n3 ${INPUT_FILENAME}:"
    bzcat ${INPUT_FILENAME} | head -n3

    # In PC_MODE=1, we rename the patterns from 11L to 8L, and change bankNumLayers correspondingly:
    # In PC_MODE=2, we rename the patterns from 11L to 4L, and change bankNumLayers && SSname correspondingly:
    if [ "${pc_mode}" == "1" -o "${pc_mode}" == "2" ]; then
	if [ "${pc_mode}" == "1" ]; then
	    bankNumLayers=8L;
	    MYnumLayers=8
	else
	    bankNumLayers=4L;
	    MYnumLayers=4
	fi;
	# if PC_SS4L is given, then we are definitely making 4L patterns, even if pc_mode==1
	PC_SS4L=`extractOption PC_SS4L`;
	if [ -n "${PC_SS4L}" ]; then
	    bankNumLayers=4L;      
	    MYnumLayers=4
	    SSname=${PC_SS4L};
	fi;

	INPUT_FILENAME_NEW=patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run${run}.patt;
	echo "PC_MODE=${pc_mode} -> renaming: ${INPUT_FILENAME} to ${INPUT_FILENAME_NEW}"
	mv ${INPUT_FILENAME} ${INPUT_FILENAME_NEW}
	INPUT_FILENAME=${INPUT_FILENAME_NEW}
    fi;
    if [ "$PATMERGEROOT" == "1" ]; then # use new tool: patmergeroot
        echo "============= running patmergeroot ============"
#        INPUT_BZ_FILENAME=patterns.bz2
#        ln -s ${INPUT_FILENAME} ${INPUT_BZ_FILENAME}
        set -a INPUTFILES
        INPUTFILES=(patt/patt*.bz2)
        NINPUTS=${#INPUTFILES[@]}
        echo "NINPUTS="${NINPUTS}
        echo "INPUTFILES="${INPUTFILES[@]}
        OUTPUT_ROOT_FILENAME=patt/patterns.root
        TMP_ROOT_FILENAME=patt/textimport.patt.root
        cat <<EOF > patmergeroot.in
LOGGING_PRINT_LEVEL 4
LOGGING_ABORT_LEVEL 2
MINCOVERAGE 0
COMPRESSION_TYPE LZMA
COMPRESSION_LEVEL 3
NINPUTS ${NINPUTS} ${INPUTFILES[*]}
TEXTIMPORT_ROOTFILE ${TMP_ROOT_FILENAME}
OUT_FILE ${OUTPUT_ROOT_FILENAME}
EOF
        echo "cat patmergeroot.in"
        echo "==================="
        cat patmergeroot.in
        echo "==================="
        echo ../standalone/patmergeroot < patmergeroot.in
        ../standalone/patmergeroot < patmergeroot.in

        echo "=========== list of files in patt ============"
        ls -l patt/
        echo "=========== list of files in ROOTDIR ========="
        ls ${ROOTDIR}/
	echo mv ${INPUT_FILENAME} ${ROOTDIR}/patterns_sub0.patt.bz2
	mv ${INPUT_FILENAME} ${ROOTDIR}/patterns_sub0.patt.bz2
	echo mv ${OUTPUT_ROOT_FILENAME} ${ROOTDIR}/patterns.patt.root
	mv ${OUTPUT_ROOT_FILENAME} ${ROOTDIR}/patterns.patt.root

    elif [ $nsubs -gt 1 ]; then
	echo
	echo "Now splitting patterns"
	echo
	for kk in `bash -c "echo {0..$nsubsMinus1}"`; do
            # this may need too much RAM (because we build the splitted pattern bank in-memory before writing to disk)
	    # this is so that we know npatterns after the split
	    echo $bankNumLayers
	    OUTPUT_FILENAME=patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${kk}_run`expr $run \* 1000 + $sub`.patt;
	    echo $INPUT_FILENAME;
	    echo $OUTPUT_FILENAME;
	    pwd;
	    if [ "$UNSPLIT_SECTORS" != "" ]; then # keep global SECTOR number 
		echo awk "BEGIN{i=0;nsub=$nsubs;isec=0;out[0]=0;};NR==1&&NF==2{L=\$2};(NF==$MYnumLayers+3){if((\$`expr $MYnumLayers + 2`)%nsub==$kk){\$1=isec;out[isec]=\$0;isec++;}i++;}END{print isec,L; for(j=0;j<isec;j++) print out[j];}"
		bzcat $INPUT_FILENAME | awk "BEGIN{i=0;nsub=$nsubs;isec=0;out[0]=0;};NR==1&&NF==2{L=\$2};(NF==$MYnumLayers+3){if((\$`expr $MYnumLayers + 2`)%nsub==$kk){\$1=isec;out[isec]=\$0;isec++;}i++;}END{print isec,L; for(j=0;j<isec;j++) print out[j];}"  | bzip2 -9 > $OUTPUT_FILENAME ;
	    else
		echo awk "BEGIN{i=0;nsub=$nsubs;isec=0;out[0]=0;};NR==1&&NF==2{L=\$2};(NF==$MYnumLayers+3){if((\$`expr $MYnumLayers + 2`)%nsub==$kk){\$1=isec;\$`expr $MYnumLayers + 2`=int(\$`expr $MYnumLayers + 2`/$nsubs);out[isec]=\$0;isec++;}i++;}END{print isec,L; for(j=0;j<isec;j++) print out[j];}"
		bzcat $INPUT_FILENAME | awk "BEGIN{i=0;nsub=$nsubs;isec=0;out[0]=0;};NR==1&&NF==2{L=\$2};(NF==$MYnumLayers+3){if((\$`expr $MYnumLayers + 2`)%nsub==$kk){\$1=isec;\$`expr $MYnumLayers + 2`=int(\$`expr $MYnumLayers + 2`/$nsubs);out[isec]=\$0;isec++;}i++;}END{print isec,L; for(j=0;j<isec;j++) print out[j];}"  | bzip2 -9 > $OUTPUT_FILENAME ;
	    fi
	    echo "Splitted pattern file (subregion ${kk}): ${OUTPUT_FILENAME}"
	    rm -rf pathead.txt
	    bzcat ${OUTPUT_FILENAME} | head -n3 > pathead.txt
	    cat pathead.txt
	    # final consistency check:
	    finalLay=`head -n1 pathead.txt | awk '{print $2}'`
	    if [ "${finalLay}" != "${MYnumLayers}" ]; then
		echo "ERROR: wrong number of layers in the splitted pattern file ${OUTPUT_FILENAME}:"
		echo "ERROR: ${finalLay} != ${bankNumLayers}"
		echo "ERROR: exiting..."
		exit -1;
	    fi;
   	    # we no longer tar up the result, so that mode=10 jobs can directly use these patterns
	    mv ${OUTPUT_FILENAME} ${ROOTDIR}/patterns_sub${kk}.patt.bz2
	done;
	rm $INPUT_FILENAME;
    else
	mv ${INPUT_FILENAME} ${ROOTDIR}/patterns_sub0.patt.bz2
    fi
    
    echo
    echo "Directory listing (before leaving):"
    echo
    find . -maxdepth 2 -exec ls -ld \{\} \; ;
    echo
}

# Merges patt-from-const runs generated in mode=pconst
# Expects that the unmerged patterns are bzipped, but NOT tarred (so the can be used directly)
function runPCONST_MERGE () {
    runNum=`extractOption runNum`
    echo ROOT_BASED_PATMERGE ${ROOT_BASED_PATMERGE}
    echo PATMERGEROOT ${PATMERGEROOT}

    if [ -n "${runNum}" ]; then
	run=${runNum}
    else
	run=1
    fi;
    echo "Merging patterns from constants in region $reg, subregion $sub"

    cd ${SCRIPTDIR}
    NLoops=`extractNLoops ${myDSname}`
    myDSname=`extractOption ftkDS`;
    MINCOVERAGE=`extractOption MINCOVERAGE 0`;
    echo "myDSname=$myDSname NLoops=$NLoops"
    mkdir -p tmp patt

    # get a list of mode-9 pattern files for current region/subregion
    greppat1="patterns_sub${sub}\.patt"
    greppat2="reg${reg}_sub0"
    if [ -s $STAGED ]; then
	echo "Looking for mode=pconst outputs with these patterns: |${greppat1}|, |${greppat2}|"
	cat $STAGED_FILES | grep --color=never ${greppat1} | grep --color=never ${greppat2} > tmp/thisJobListFiles
	echo "Matched `wc -l tmp/thisJobListFiles` files"
	cat tmp/thisJobListFiles
    else
	echo "Look for available replicas:"
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -r $myDSname
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f $myDSname > tmp/list_dq2output
	echo "tail tmp/list_dq2output"
	tail tmp/list_dq2output
	grep --color=never '\[.\]' tmp/list_dq2output | sed -e "s,.*user,user," | grep --color=never patt.bz2 | awk '{print $1}' | sort > tmp/listFiles;
	echo "tmp/listFiles contains `wc -l tmp/listFiles` files. First and last 5 files are:"
	head -n5 tmp/listFiles && echo "..." && tail -n5 tmp/listFiles
	cat tmp/listFiles | grep --color=never ${greppat1} | grep --color=never ${greppat2} > tmp/thisJobListFiles
	echo "tmp/thisJobListFiles contains `wc -l tmp/thisJobListFiles` files. First and last 5 files are:"
	head -n5 tmp/thisJobListFiles && echo "..." && tail -n5 tmp/thisJobListFiles
    fi;

    # prepare the list of files to be downloaded from the mode9 output dataset:
    set +x
    LIST_OF_FILES=
    nfiles=0;
    for fname in `cat tmp/thisJobListFiles`; do
	LIST_OF_FILES[nfiles]=${fname}
	((nfiles++));
    done
    if [ "${nfiles}" -eq "0" ]; then
	echo
	echo "ERROR: tmp/thisJobListFiles doesn't contain any files for reg=$reg and sub=$sub"
	echo "Exiting..."
	echo
	exit -1
    fi;
    getFilesFromDS $DQ2_LOCAL_SITE_ID $myDSname "${LIST_OF_FILES[*]}"
    set -x

    # Extract pattern run files
    mkdir -p ${SCRIPTDIR}/patt/
    mv ${myDSname}/* ${SCRIPTDIR}/patt/

    cd ${SCRIPTDIR}
    /bin/ls patt/*patterns_*.patt* > tmp/filesToBeMerged
    echo tail tmp/filesToBeMerged
    tail tmp/filesToBeMerged
    NDOWNLOADED=`wc -l tmp/filesToBeMerged | cut -f 1 -d \  `
    # if there are no files at all, fail the job:
    if [ "${NDOWNLOADED}" -le "0" ]; then
	echo ERROR: the number of downloaded pattern files is ZERO.
	echo ERROR: exiting
	exit -1;
    fi
    if [ $NDOWNLOADED -ne $NLoops ]; then
	echo DOWNLOAD_CHECK: `wc tmp/filesToBeMerged`
	echo NLoops=$NLoops
	if [ $NDOWNLOADED -ge $(( $(( $NLoops - 1 )) * 90 / 100 )) ]; then
	    echo WARNING: not leaving even if the number of downloaded pattern files does not match NLoops.
	    echo WARNING: $NDOWNLOADED -ge $(( $(( $NLoops - 1 )) * 90 / 100 ))  "passing the ($NLoops - 1)*90% threshold"
	else
	    echo ERROR: $NDOWNLOADED -ge $(( $(( $NLoops - 1 )) * 90 / 100 ))  "NOT passing the ($NLoops - 1)*90% threshold"
	    echo ERROR: exiting...
	    exit -1;
	fi;
    else
	echo DOWNLOAD_CHECK: passed $NDOWNLOADED -ne $NLoops
    fi

    echo list input files with -l
    ls -l patt/
    echo total size of `ls patt/ | wc -l` pattern files:
    du -shcD patt/

    echo ""
    echo "PATMERGEROOT: $PATMERGEROOT"
    if [ "$PATMERGEROOT" == "1" ]; then # use new tool: patmergeroot
         #if [ -n "$PATMERGEROOT" ]; then # use new tool: patmergeroot
         # generate steering file with following flags:
         # 
         #  LOGGING_PRINT_LEVEL  4    # verbosity level
         #  LOGGING_ABORT_LEVEL  2
         #  MINCOVERAGE	         2    # cut on coverage, applied to MERGED_ROOTFILE and OUT_FILE
         #  OUT_FILE                  # output file (use .root if root-format is desired)
         #  NSUB                 0    # do not split into subregions
         #  NINPUTS	              # number of input files
         #  ...	                      # n-lines of input files
         # 
         NINPUTS=`/bin/ls patt/*patterns_*.patt* | wc -l`
	 echo "LOGGING_PRINT_LEVEL 4" > job_patmergeroot.in
	 echo "LOGGING_ABORT_LEVEL 2" >> job_patmergeroot.in
	 echo "MINCOVERAGE ${MINCOVERAGE}" >> job_patmergeroot.in
	 echo "NSUB 0" >> job_patmergeroot.in
	 echo "OUT_FILE patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run0.patt.bz2" >> job_patmergeroot.in
	 echo "NINPUTS ${NINPUTS}" >> job_patmergeroot.in
	 /bin/ls patt/*patterns_*.patt* >> job_patmergeroot.in
	 
	 echo " [prun_functions.sh::runPCONST_MERGE] Running patmergeroot and printing steering file:"
	 cat job_patmergeroot.in
	 ../standalone/patmergeroot job_patmergeroot.in
	 RETCODE=$?
	 
         ## remove temporary file, and remove files that have been copied
	 echo " [prun_functions.sh::runPCONST_MERGE] deleting temporary merged patterns"
	 ##/bin/rm patt/*patterns_*.patt* # input patterns (but NOT remove output pattern!)
	 rm patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run0.patt.bz2.textImport.root
	 rm patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run0.patt.bz2.root

    else 
        # NEW patmerge (no need to unzip files first!)
	NINPUTS=`/bin/ls patt/*patterns_*.patt* | wc -l`
	echo "NINPUTS ${NINPUTS}" > job_patmerge.in
	/bin/ls patt/*patterns_*.patt* >> job_patmerge.in
	echo "OUT_FILE patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run0.patt.bz2" >> job_patmerge.in
	echo "DELETE_FILES_AFTER_USE 0" >> job_patmerge.in
	echo "VMEM_LIMIT 15700000" >> job_patmerge.in   #limit max RAM usage
#if [ "${1}" == "ANALY_SLAC_LMEM" ]; then
#	echo "SETTING VMEM LIMIT FOR SLAC_LMEM" 
#	echo "VMEM_LIMIT 9700000" >> job_patmerge.in   #limit max RAM usage
#    else
#	echo "VMEM_LIMIT 2700000" >> job_patmerge.in   #limit max RAM usage
#   fi;
	echo "MINCOVERAGE ${MINCOVERAGE}" >> job_patmerge.in
	echo " Running patmerge:"
	cat job_patmerge.in
	../standalone/patmerge job_patmerge.in
	RETCODE=$?
    fi



    # OLD patmerge [DEPRECATED due to many limitations]
    # only works with unzipped patterns and uses TrigFTKLib/src/pattgen
    # only runs in batches of 8 files
    if [ "0" -eq "1" ]; then
	cp ${ROOTDIR}/merge.sh .
	echo RUNNING: ./merge.sh . patt raw_${bankNumLayers}_${SSname} $nsubs $reg $sub
	./merge.sh . patt raw_${bankNumLayers}_${SSname} $nsubs $reg $sub
	RETCODE=$?
    fi;

    echo merge RETCODE=$RETCODE
    if [ $RETCODE -eq 0 ]; then
	echo
	echo pattern merge ran successfully
	echo;
    else
	echo ERROR: merge RETCODE=$RETCODE;
	exit -1;
    fi

    OUT_FILE=patt/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}_run0.patt.bz2
    echo;
    echo produced file $OUT_FILE;
    echo;
    ls -l $OUT_FILE;
    mkdir -p tmp
    bzip2 -dc $OUT_FILE | awk 'BEGIN{i=0;nhit[0]=0;npatt[0]=0;};NF==2{L=$2;TOTPATT=$1;printf "TOTPATT=%d L=%d\n",TOTPATT,L;};(NF==L+3){NHIT=$NF; if (0){printf "%d  \n", NHIT}; if (NHIT!=nhit[i]) { if (nhit[i]!=0) { npatt[i+1]=$1; if(0){printf "i=%d  nhit[i]=%d  npatt[i+1]=%d\n", i, nhit[i], npatt[i+1]}; i++; nhit[i]=NHIT; }; if (i==0) { nhit[i]=NHIT; };  }; };END{npatt[i+1]=TOTPATT; for (j=0;j<=i;j++) { printf "nhit=%4d\tnpatt=%8d\tdelta=%8d\tfrac=%f\n",nhit[j],npatt[j+1], npatt[j+1]-npatt[j], 1.*(npatt[j+1]-npatt[j])/TOTPATT };}' > tmp/countPatternHits
    cp tmp/countPatternHits ${ROOTDIR}
    echo
    tail -n 20 tmp/countPatternHits | sed -e "s,^,INFO: ,"

    echo "RENAME merged pattern files"
    ls -l patt/patterns*
    for fname in patt/patterns*; do
	mv $fname `echo $fname | sed -e "s,run[0-9]*,run$run,"`;
    done
    # bzip patterns (ONLY if not bzipped already):
    cd patt
    find . -name patterns\*.patt -maxdepth 1 -exec bzip2 -9 {} \;
    cd -

    # save the results. we no longer tar them, so they can be tmp/countPatternHits
    mkdir -p ${OUTPUTDIR}
    mv ${ROOTDIR}/countPatternHits ${OUTPUTDIR}
    mv patt/patterns* ${ROOTDIR}/patterns_raw_${bankNumLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.patt.bz2
    echo
    echo "Directory listing (before leaving):"
    echo
    find . -maxdepth 2 -exec ls -ld \{\} \; ;
}


# Merges patt-from-const runs generated in mode=pconst
# using root files 
function runPCONST_MERGE_ROOT () {
    runNum=`extractOption runNum`

    if [ -n "${runNum}" ]; then
	run=${runNum}
    else
	run=1
    fi;
    echo "Merging patterns from constants in region $reg, subregion $sub"

    cd ${SCRIPTDIR}
    myDSname=`extractOption ftkDS`;
    NLoops=`extractNLoops ${myDSname}`
    MINCOVERAGE=`extractOption MINCOVERAGE 0`;
    echo "myDSname=$myDSname NLoops=$NLoops"
    mkdir -p tmp patt

    # get a list of mode-9 pattern files for current region/subregion
    greppat1="patterns.patt.root"
    greppat2="reg${reg}_sub0"
    if [ -s $STAGED ]; then
	echo "Looking for mode=pconst outputs with these patterns: |${greppat1}|, |${greppat2}|"
	cat $STAGED_FILES | grep --color=never ${greppat1} | grep --color=never ${greppat2} > tmp/thisJobListFiles
	echo "Matched `wc -l < tmp/thisJobListFiles` files"
	cat tmp/thisJobListFiles
    else
	echo "Look for available replicas:"
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -r $myDSname
	DQ2_LS_CMD -L $DQ2_LOCAL_SITE_ID -f $myDSname > tmp/list_dq2output
	echo "tail tmp/list_dq2output"
	tail tmp/list_dq2output
	grep --color=never '\[.\]' tmp/list_dq2output | sed -e "s,.*user,user," | grep --color=never patt.bz2 | awk '{print $1}' | sort > tmp/listFiles;
	echo "tmp/listFiles contains `wc -l tmp/listFiles` files. First and last 5 files are:"
	head -n5 tmp/listFiles && echo "..." && tail -n5 tmp/listFiles
	cat tmp/listFiles | grep --color=never ${greppat1} | grep --color=never ${greppat2} > tmp/thisJobListFiles
	echo "tmp/thisJobListFiles contains `wc -l tmp/thisJobListFiles` files. First and last 5 files are:"
	head -n5 tmp/thisJobListFiles && echo "..." && tail -n5 tmp/thisJobListFiles
    fi;

    # prepare the list of files to be downloaded from the mode9 output dataset:
    set -a INPUTFILES
    INPUTFILES=(`cat tmp/thisJobListFiles`)
    NINPUTS=${#INPUTFILES[@]}
    echo "NINPUTS="${NINPUTS}
    if [ "${NINPUTS}" -eq "0" ]; then
	echo
	echo "ERROR: tmp/thisJobListFiles doesn't contain any files for reg=$reg and sub=$sub"
	echo "Exiting..."
	echo
	exit -1
    fi
    getFilesFromDS $DQ2_LOCAL_SITE_ID $myDSname "${INPUTFILES[*]}"

    echo "files have been copied to "$myDSname
    absoluteDSpath=`readlink -f $myDSname`

    echo "absolute path to input files: "$absoluteDSpath
    ln -s $absoluteDSpath patt.input

    echo
    echo "list of all input files: "
    echo
    /bin/ls -l patt.input/
    echo

    # Extract pattern run files
    mkdir -p ${SCRIPTDIR}/patt/

    set -a INPUTFILES
    INPUTFILES=(patt.input/*.root*)
    NINPUTS=${#INPUTFILES[@]}
    echo "NINPUTS="${NINPUTS}
    echo "INPUTFILES="${INPUTFILES[@]}

    # if there are no files at all, fail the job:
    if [ "${NINPUTS}" -le "0" ]; then
	echo ERROR: the number of downloaded pattern files is ZERO.
	echo ERROR: exiting
	exit -1;
    fi
    if [ $NINPUTS -ne $NLoops ]; then
	echo DOWNLOAD_CHECK: $NINPUTS $NLoops
	echo NLoops=$NLoops
	if [ $NINPUTS -ge $(( $(( $NLoops - 1 )) * 90 / 100 )) ]; then
	    echo WARNING: not leaving even if the number of downloaded pattern files does not match NLoops.
	    echo WARNING: $NINPUTS -ge $(( $(( $NLoops - 1 )) * 90 / 100 ))  "passing the ($NLoops - 1)*90% threshold"
	else
	    echo ERROR: $NINPUTS -ge $(( $(( $NLoops - 1 )) * 90 / 100 ))  "NOT passing the ($NLoops - 1)*90% threshold"
	    echo ERROR: exiting...
	    exit -1;
	fi;
    else
	echo DOWNLOAD_CHECK: passed $NINPUTS -ne $NLoops
    fi

    mFinal=`extractOption factor`
    TMP_ROOT_FILENAME=patt/tmp.root
    cat <<EOF > job_patmergeroot.in
LOGGING_PRINT_LEVEL 4
LOGGING_ABORT_LEVEL 2
MINCOVERAGE ${MINCOVERAGE}
NSUB ${mFinal}
COMPRESSION_TYPE LZMA
COMPRESSION_LEVEL 3
OUT_FILE patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}
TEXTIMPORT_ROOTFILE ${TMP_ROOT_FILENAME}
NINPUTS ${NINPUTS} ${INPUTFILES[*]}
EOF
    echo "Running patmergeroot and printing steering file:"
    cat job_patmergeroot.in
    ../standalone/patmergeroot job_patmergeroot.in
    RETCODE=$?
    echo "========= ls patt/ ========="
    ls -l patt/
    echo
    for ((isub=0;isub<mFinal;isub++)) ; do
        echo mv patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}_sub${isub}.bz2 patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}_sub${isub}.patt.bz2
        mv patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}_sub${isub}.bz2 patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}_sub${isub}.patt.bz2
    done
    echo mv patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}.root patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}.patt.root
    mv patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}.root patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}.patt.root
         ## remove temporary file, and remove files that have been copied
    echo "deleting temporary merged patterns"
	 ##/bin/rm patt/*patterns_*.patt* # input patterns (but NOT remove output pattern!)
    rm -f ${TMP_ROOT_FILENAME}

    echo merge RETCODE=$RETCODE

    if [ $RETCODE -eq 0 ]; then
	echo
	echo pattern merge ran successfully
	echo;
    else
	echo ERROR: merge RETCODE=$RETCODE;
	exit -1;
    fi

    mkdir -p tmp
    rm -f tmp/countPatternHits
    for ((final=0;final<mFinal;final++)) ; do
        OUT_FILE=patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}_sub${final}.patt.bz2
        echo;
        echo produced file $OUT_FILE;
        echo;
        ls -l $OUT_FILE;
        bzip2 -dc $OUT_FILE | awk 'BEGIN{i=0;nhit[0]=0;npatt[0]=0;};NF==2{L=$2;TOTPATT=$1;printf "TOTPATT=%d L=%d\n",TOTPATT,L;};(NF==L+3){NHIT=$NF; if (0){printf "%d  \n", NHIT}; if (NHIT!=nhit[i]) { if (nhit[i]!=0) { npatt[i+1]=$1; if(0){printf "i=%d  nhit[i]=%d  npatt[i+1]=%d\n", i, nhit[i], npatt[i+1]}; i++; nhit[i]=NHIT; }; if (i==0) { nhit[i]=NHIT; };  }; };END{npatt[i+1]=TOTPATT; for (j=0;j<=i;j++) { printf "nhit=%4d\tnpatt=%8d\tdelta=%8d\tfrac=%f\n",nhit[j],npatt[j+1], npatt[j+1]-npatt[j], 1.*(npatt[j+1]-npatt[j])/TOTPATT };}' >> tmp/countPatternHits
    done
    cp tmp/countPatternHits ${ROOTDIR}
    echo
    tail -n 100 tmp/countPatternHits | sed -e "s,^,INFO: ,"

    # save the results. we no longer tar them, so they can be tmp/countPatternHits
    mkdir -p ${OUTPUTDIR}
    mv ${ROOTDIR}/countPatternHits ${OUTPUTDIR}
    for ((final=0;final<mFinal;final++)) ; do
        OUT_FILE=patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}_sub${final}.patt.bz2
        mv ${OUT_FILE} ${ROOTDIR}/
    done
    mv patt/patterns_raw_${bankNumLayers}_${SSname}_${mFinal}M_reg${reg}.patt.root ${ROOTDIR}/
    echo
    echo "Directory listing (before leaving):"
    echo
    find . -maxdepth 2 -exec ls -ld \{\} \; ;
}
