#!/bin/sh
# ========================================================================= #
#                                                                           #
#    patch_fincludes.sh [Arguments]                                         #
#                                                                           #
#    Shell script to patch the -I<include> paths for FORTRAN compilations   #
#    in order to overcome a problem with g77 that crashes if an <include>   #
#    path exceeds ~100 characters in length.                                #
#                                                                           #
#    Arguments:                                                             #
#        $1      <package>_cmtpath                                          #
#        $2      <package>_root                                             #
#                                                                           #
# ========================================================================= #

#+
# Main Program
#-

# set -x

# --- Default parameters

# Setup the -I<include> path, which already has any local component
# converted into relative paths.
old_includes=`cmt show macro_value part_fincludes`
new_includes=$old_includes

if [ $# -ge 2 ]; then

# Determine the set of ATLAS projects and their locations

    atlas_path=`echo "$CMTPROJECTPATH" | awk -F: '{print $1}'`
    if [ ! -z "${atlas_path}" ]; then
        atlas_proj=`cmt show projects | grep ${atlas_path} | awk '{print $1}'`

# Setup symlinks to each project in the InstallArea corresponding to this
# package it they don't already exist. The $1 argument, if it exists, should
# reference the directory containing the InstallArea.

        if [ -d ${1}/InstallArea/include ]; then
            cd ${1}/InstallArea/include
            for proj in `echo ${atlas_proj}`; do
                if [ ! -e $proj ]; then
                    ln -s -n ${atlas_path}/$proj $proj
                fi
            done

# Modify the include paths to be relative to the InstallArea. Make sure that only project 
# include paths are modified, and not include paths to external packages (this applies
# particularly to kit installations)
            old_offset=`echo $2 | sed "s#$1##"`
            new_offset=`echo "${old_offset}/.." | sed -e 's#^/##' -e 's#/$##' -e 's#[^/]*#..#g'`
            for proj in `echo ${atlas_proj}`; do
                new_includes=`echo $old_includes | sed -e "s#${atlas_path}/${proj}#${new_offset}/InstallArea/include/${proj}#g"`
                old_includes=${new_includes}
            done
        fi        
    fi
fi

# Return the patched include paths
echo $new_includes
exit 0
