#!/bin/sh
# ============================================================================= #
#                                                                               #
#    list_files.sh [options] [dirs]                                             #
#                                                                               #
#    Unix script to list files in the specified directories                     #
#                                                                               #
#    Options:                                                                   #
#                                                                               #
#      -t|--type <type>      Specify the file type (e.g. so) (default = all)    #
#                            Multiple such types may be specified               #
#                                                                               #
# ============================================================================= #

#+
# Main Program
#-

# set -x

# --- Default parameters

dirList=
typeList=
fileList=

# --- Input parameters

while [ $# -ne 0 ]; do
    case $1 in
	    -s=*)        dirList="$dirList `echo $1 | sed 's/-s=//'`";;
        -t | --type) typeList="$typeList $2"; shift;;
        *)           dirList="$dirList $1";;
    esac
    shift
done

# --- Loop over directory list

for dir in $dirList; do
    if [ -d $dir ]; then
        for type in $typeList; do
	        files=`ls $dir`
			if [ -z "$valid" ]; then
			    for file in $files; do
                    good=`echo $file | grep [.]$type`
					if [ ! -z $good ]; then
			            fileList="$fileList $dir/$file"
					fi
				done
			fi
	    done
    fi
done
echo $fileList
