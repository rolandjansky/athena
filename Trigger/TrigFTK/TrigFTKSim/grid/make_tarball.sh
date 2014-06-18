#!/bin/bash

function usage () {
    echo "Prepares the TrigFTK* package tarballs to be shipped along with the grid job"
    echo "USAGE:"
    echo "./make_tarball TrigFTKSim /path/to/TrigFTKSim TrigFTKAna /path/to/TrigFTKAna"
    echo "Order is important - e.g., TrigFTKSim has to come before TrigFTKAna"
}

# check that we have an even number of arguments
argdiv=`expr "$#" % 2`
if [ "$argdiv" -ne "0" ]; then
    usage
fi

# choose verbosity level:
stdout=/dev/null
#stdout=/dev/stdout

cur="$PWD"
tmp=/tmp/`whoami`  # TODO: check that /tmp is actually available in write-mode


# save the list of package names and locations
unset pkgnames
unset locations
npkg=0
i=0
for arg in $@; do
    if [ "$i" -eq "0" ]; then
	pkgnames[$npkg]=$arg
    elif [ "$i" -eq "1" ]; then
	locations[$npkg]=$arg
	((npkg++))
    else
	echo "ERROR: this cannot happen"
	exit 1;
    fi;
    ((i++))
    if [ "$i" -eq "2" ]; then i=0; fi;
done;

shift $# # clean the arguments to avoid issues with the TrigFTKAna/bootstrap.sh
# loop over packages
for ii in `seq 0 $(expr $npkg - 1)`; do
    loc=${locations[$ii]}
    pkgname=${pkgnames[$ii]}
    if [ -d ${loc} ]; then
	rm -f ${cur}/${pkgname}.tar.bz2
	rm -rf ${tmp}/${pkgname}
	mkdir -p ${tmp}/${pkgname}
	
        # make a copy of current TrigFTK* tree
	cd ${loc} && tar cf ${tmp}/${pkgname}/content.tar --exclude './svn' *
	cd ${tmp}/${pkgname} && tar xf content.tar && rm -f content.tar;
        # cleanup object files
	if [ -d standalone ]; then
	    cd standalone
	    if [ -f "bootstrap.sh" ]; then 
		source bootstrap.sh &> ${stdout}
	    fi;
	    make clean &> ${stdout}
	    cd - &>/dev/null
	else
	    if [ -f "bootstrap.sh" ]; then
		FTKCLASSES=../TrigFTKSim/standalone/libftk_classes.so
		if [ ! -e ${FTKCLASSES} ]; then
		    if [ ! -d "../TrigFTKSim/standalone" ]; then
			echo "ERROR: TrigFTKAna can only be packaged along with TrigFTKSim"
			exit 1;
		    fi
		    echo "Creating dummy file ${FTKCLASSES}" > ${stdout}
		    echo "DUMMY" > ${FTKCLASSES}
		fi;

		source bootstrap.sh &> ${stdout}
		make clean &> ${stdout}
		rm -f ${FTKCLASSES}
	    fi;
	fi;
        # cleanup .svn and the large grid definition xml files
	cd ${tmp}/${pkgname}
	find . -type d -name \.svn | xargs rm -rf
	find . -type f -name jobinput\*xml | xargs rm -f
	find . -type f -name \.ftk_\* | xargs rm -f
	find . -type f -name TrigFTK\*bz2 | xargs rm -f
	find . -maxdepth 1 -type d -name i\*  | xargs rm -rf
	#rm -f config/input/*
	find config  -type f -name inputprod\* 2>/dev/null | xargs rm -rf
	#find . -type d -name i\*  | xargs rm -rf
	find . -type f -name dq2cache.pickle | xargs rm -f
	find . -type f -name \*root | xargs rm -f
        # make TrigFTK*.tar.bz2
	cd ${tmp} && tar cfj ${cur}/${pkgname}.tar.bz2 ${pkgname} 
    else
	echo "ERROR: couldn't find package ${pkgname} in ${loc}"
    fi;
done;

# clean up
for pkgname in ${pkgnames[@]}; do
    rm -rf ${tmp}/${pkgname}
done;

# return to original PWD
cd ${cur}
