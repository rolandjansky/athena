#!/bin/bash

[ "$PROJRUNDBG" ] && set -x

scriptdir=$(dirname $0)
. $scriptdir/pack-common.sh
scriptdir=$(abspath $scriptdir)
export scriptdir

help ()
{
cat <<EOF
Usage: `basename $0` [OPTION]...
Pack project with external packages it depends upon, or only external package(s).

  -L <path>      create log file in directory <path>
  -C <path>      create packages in directory <path>
  -X <path>      create external packages in directory <path>
                 (takes precedence over -C value)
  -J <path>      create project packages in directory <path>
                 (takes precedence over -C value)
  -o <path>      create RPMs in directory <path>
  -x <path>      create external RPMs in directory <path>
                 (takes precedence over -o value)
  -j <path>      create project RPMs in directory <path>
                 (takes precedence over -o value)
  -b <path>      pack project located in project base directory <path>
  -P <project>   pack project <project>
  -r <id>        pack project release <id>
  -p <package>   pack external package <package> (may be multiple)
  -D             pack debug build
  -I             create debug information packages (for not debug build)
  -S <suffix>    suffix of debug information files (for not debug build)
  -t <tag>[,<tag>...]  use additional tag(s) for setup (may be multiple)
  -T <cmtconfig>  force CMTCONFIG <cmtconfig> (platform tag) for setup
  -d             pack documentation files also
  -F             overwrite existing tarballs of external packages
  -U             overwrite existing tarballs of project packages
  -k             keep existing tarballs of settings packages
                 (default is overwrite if their versions differ from those in use)
  -R             recursively pack projects required by the project being packed
  -V             verify that existing tarballs (of project and/or external packages) are up-to-date
  -w             wait until packing completion
  -q             do not write anything to standard output
  -n             do not execute, only dump configuration
  -B <path>      use directory <path> for temporary files
  -c <path>      use configuration file <path>
  -h             display this help and exit
  -v             output version information and exit

The PACKOPTS environment variable specifies a space separated list of default options
to be placed in front of any explicit options.

Report bugs to <Grigori.Rybkine@cern.ch>.
EOF
}

version ()
{
if [ -r $scriptdir/../ChangeLog ]; then
printf "`basename $0` (PackDist) %s\nWritten by Grigori Rybkine.\n" \
`sed -n '/^[[:blank:]]*PackDist-\([0-9][0-9]\(-[0-9][0-9]\)\{2,3\}\)[[:blank:]]*$/{s//\1/p
q
}' $scriptdir/../ChangeLog`
else
printf "`basename $0` (PackDist) %s\nWritten by Grigori Rybkine.\n" \
`cd $scriptdir/../cmt && cmt show version|sed -n '/^[[:blank:]]*PackDist-\([0-9][0-9]\(-[0-9][0-9]\)\{2,3\}\)[[:blank:]]*$/s//\1/p'`
fi
}

parse_opts ()
{
local msg="Try '`basename $0` -h' for more information."

while getopts ":L:C:X:J:o:x:j:b:P:r:p:DIS:t:T:dFUkRVwqnB:c:hv" opt; do
    case $opt in
	L) eval logdir=${OPTARG} ;;
	C) eval pacmancache=${OPTARG} ;;
	X) eval externcache=${OPTARG} ;;
	J) eval projcache=${OPTARG} ;;
	o) eval rpmdir=${OPTARG} ;;
	x) eval externdir=${OPTARG} ;;
	j) eval projdir=${OPTARG} ;;
	b) eval projbase=${OPTARG} ;;
	P) project=${OPTARG} ;;
	r) release=${OPTARG} ;;
	p) if [ "$packages" ]; then
	       packages="$packages ${OPTARG}"
	   else packages=${OPTARG}; fi ;;
	D) mode=dbg ;; # Debug
	I) debuginfo=yes ;;
	S) debuginfosuffix=${OPTARG} ;;
	t) if [ "$tags" ]; then
	       tags="$tags ${OPTARG}"
	   else tags=${OPTARG}; fi ;;
	T) cmtconfig=${OPTARG} ;;
	d) docfiles=yes ;;
	F) overwrite=yes ;; # Fresh
	U) upgrade=yes ;; # Upgrade
	k) keep=yes ;;
	R) recursive=yes ;;
	V) verify=yes ;;
	w) waiting=yes ;;
	q) quiet=yes ;;
	n) configure=yes ;;
	B) eval builddir=${OPTARG} ;;
	c) config_read -e ${OPTARG} || return 1 ;;
	h) help ; exit 0 ;;
	v) version ; exit 0 ;;
	\?) error "Invalid option: -${OPTARG}\n$msg" $FUNCNAME; return 1 ;;
	:) error "Argument required for option: -${OPTARG}\n$msg" $FUNCNAME; return 1 ;;
    esac
done

shift $(($OPTIND - 1))

[ $# -eq 0 ] || { error "Invalid argument(s): $*\n$msg" $FUNCNAME; return 1; }
}

config_read ()
{
_error_level=0
tmpcfg=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
tmperr=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
for f; do
    if [ "X$f" = "X-e" ]; then
	_error_level=2
	continue
    elif [ "X$f" = "X-w" ]; then
	_error_level=1
	continue
    elif [ "X$f" = "X-i" ]; then
	_error_level=0
	continue
    fi
#     [ -f "$f" ] || continue
    if [ ! -r "$f" ]; then
	case ${_error_level} in
	    2) error "Cannot read file '$f'" $FUNCNAME; return 1 ;;
	    1) warn "Cannot read file '$f'" $FUNCNAME; continue ;;
	    0) continue ;;
	esac
    fi
#    awk -v tmperr=$tmperr '
    awk 'BEGIN { tmperr="'$tmperr'" }
        NF == 0 || /^#/ { next }
        { sub(/[ \t]#.*$/, " "); if (NF == 0) next }
        /^[ \t]*[^ \t=:]+[ \t]*[=:]/ { sub(/[=:]/, " "); print; next }
        { printf "%s: line %d: %s\n", FILENAME, NR, $0 >> tmperr }
#        { printf "%s: line %d:\n%s\n", FILENAME, NR, $0 >> tmperr }
    ' "$f" >>$tmpcfg || { error "awk" $FUNCNAME; return 1; }
done
[ -s $tmperr ] &&
{ error "Invalid syntax:\n$(<$tmperr)" $FUNCNAME; rm -f $tmperr $tmpcfg; return 2; }
rm -f $tmperr
while read opt val; do
    case $opt in
	cmthome) eval cmthome=${val} ;; # eval ${opt}=${val}
	setuproot) eval setuproot=${val} ;; # eval ${opt}=${val}
	logdir) eval logdir=${val} ;;
	pacmancache) eval pacmancache=${val} ;;
	externcache) eval externcache=${val} ;;
	projcache) eval projcache=${val} ;;
	rpmdir) eval rpmdir=${val} ;;
	externdir) eval externdir=${val} ;;
	projdir) eval projdir=${val} ;;
	patchdir) eval patchdir=${val} ;;
	projbase) eval projbase=${val} ;;
	project) project=${val} ;;
	release) release=${val} ;;
	mainpkg) mainpkg=${val} ;;
	container) container="${val}" ;;
	packages) if [ "$packages" ]; then
	              packages="$packages ${val}"
		  else packages=${val}; fi ;;
	mode) mode=${val} ;;
	debuginfo) debuginfo=${val} ;;
	debuginfosuffix) debuginfosuffix=${val} ;;
	tags) if [ "$tags" ]; then
	          tags="$tags ${val}"
	      else tags=${val}; fi ;;
	cmtconfig) cmtconfig=${val} ;;
	docfiles) docfiles=${val} ;;
	overwrite) overwrite=${val} ;;
	upgrade) upgrade=${val} ;;
	keep) keep=${val} ;;
	recursive) recursive=${val} ;;
	verify) verify=${val} ;;
	waiting) waiting=${val} ;;
	quiet) quiet=${val} ;;
	configure) configure=${val} ;;
	builddir) eval builddir=${val} ;;
	cmtcache) cmtcache=${val} ;;
	*) error "Invalid option: '${opt}'" $FUNCNAME; rm -f $tmpcfg; return 1 ;;
    esac
done <${tmpcfg}
#rm -f $tmperr $tmpcfg
rm -f $tmpcfg
}

config_dump () {
cat <<EOF
cmthome: $cmthome
setuproot: $setuproot
logdir: $logdir
pacmancache: $pacmancache
externcache: $externcache
projcache: $projcache
rpmdir: $rpmdir
externdir: $externdir
projdir: $projdir
patchdir: $patchdir
projbase: $projbase
project: $project
release: $release
mainpkg: $mainpkg
container: $container
packages: $packages
mode: $mode
debuginfo: $debuginfo
debuginfosuffix: $debuginfosuffix
tags: $tags
cmtconfig: $cmtconfig
docfiles: $docfiles
overwrite: $overwrite
upgrade: $upgrade
keep: $keep
recursive: $recursive
verify: $verify
waiting: $waiting
quiet: $quiet
configure: $configure
builddir: $builddir
cmtcache: $cmtcache
EOF
}

parse_bool ()
{
local i v
for i; do
    eval v=\$$i
    case $v in [yY]*|1) eval $i=yes ;; *) eval $i=no ;; esac 
done
}

config_read -e $scriptdir/defaults.cfg -i $HOME/.pack.cfg -w $PACKCFG || exit
parse_opts $PACKOPTS "$@" || exit
shift $#

[ "${uname:-}" ] || uname=$(uname)

[ -t 2 ] || printf "\n`version`\n\n" >&2

parse_bool debuginfo docfiles overwrite upgrade keep recursive verify waiting quiet configure

for pkg in $packages; do
    pkg=`echo $pkg | sed -e 's#^[^/]#/&#' -e 's#[^/]$#&/#'`
    if [ "$selected" ]; then
	selected="$selected $pkg"
    else selected=$pkg
    fi
done

[ -z "$tags" ] || tags=$(tr -s ' \t' ',,' <<<"$tags" | sed 's/^,//;s/,$//')
#config_dump; exit 18

[ $(wc -w <<<"$container") -le 2 ] ||
{ error "Wrong container package specification: '$container'\n\t(must be: [container-name [access-path]])" $FUNCNAME; exit 1; }
[ "$container" ] && container=$(sed 's/[ \t]\+/ /g' <<<"$container")

: ${project:?}
[ $project = CMT ] || : ${release:?}
#: ${release:?}
#: ${projbase:?}

#[ -d "$projbase" ] && { projbase=$(abspath $projbase) || exit; }
: ${cmtcache:?}

if [ "${debuginfo:-}" = yes ] && [ "${mode:-}" != dbg ]; then
    [ -n "${debuginfosuffix:-}" ] || { error "'debuginfosuffix' must be non-empty" $FUNCNAME; exit 1; }
fi

_setuproot ()
{
setuproot=$(abspath $setuproot) || return
export AtlasSetup=${setuproot}

if [ -n "${projbase:-}" ]; then
    if [ -d ${projbase} ]; then
	cmtprojectpath=/afs/cern.ch/atlas/software/builds:/afs/cern.ch/atlas/project/tdaq/prod:/afs/cern.ch/atlas/offline/external
####	_cmd=". $setuproot/scripts/asetup.sh ${project},${release},${mode},notest,ignore${tags:+,${tags}} --releasesarea=${projbase}:${cmtprojectpath} ${PROJRUNDBG:+--debugprint}"
	_cmd=". $setuproot/scripts/asetup.sh ${project},${release},${mode},notest,ignore${tags:+,${tags}} ${cmtconfig:+--cmtconfig=${cmtconfig}} --releasebase=${projbase} ${PROJRUNDBG:+--debugprint}"
    else
	warn "${projbase}: No such directory" $FUNCNAME
    fi
    [ -d ${projbase}/${project}/${release} ] || unset projbase
fi
[ -n "${_cmd:-}" ] ||
_cmd=". $setuproot/scripts/asetup.sh ${project},${release},${mode},notest,ignore,builds${tags:+,${tags}} ${cmtconfig:+--cmtconfig=${cmtconfig}} ${PROJRUNDBG:+--debugprint}"

info "asetup command=${_cmd}" $FUNCNAME

[ "$PROJRUNDBG" ] && set +x
${_cmd} || { error "${_cmd}" $FUNCNAME; return 1; }
[ "$PROJRUNDBG" ] && set -x

if [ -n "${projbase:-}" ]; then
    for p in $(echo $cmtprojectpath | tr -s ':' ' '); do
	path_append CMTPROJECTPATH $p
    done
    path_prepend CMTPROJECTPATH ${projbase}
fi

if [ -z "${projbase:-}" ]; then
    for _path in `echo "${CMTPATH:-}" | tr -s ':' ' '`; do
	if [ -d ${_path} ]; then
	    [ ${release} = $(basename ${_path}) ] || continue
	    _p=$(dirname ${_path}) || return
	    [ ${project} = $(basename ${_p}) ] || continue
	    projbase=$(dirname ${_p}) || return
	    #projbase=$(dirname $(dirname ${_p})) || return
	    break
	else
	    warn "${_path}: No such directory" $FUNCNAME
	fi
    done
fi
unset CMTPATH
if [ -z "${projbase:-}" ]; then
    for _p in `echo "${CMTPROJECTPATH:-}" | tr -s ':' ' '`; do
	if [ -d ${_p}/${project}/${release} ]; then
	    projbase=${_p} || return
	    break
	else
	    warn "${_p}: No such directory" $FUNCNAME
	fi
    done
fi
[ -n "${projbase:-}" ] ||
{ error "${project} ${release}: Cannot locate" $FUNCNAME; return 2; }

#asetup LCGCMT,LCGCMT_59,builds,notest --releasebase=/afs/cern.ch/atlas/software/builds
#asetup LCGCMT,LCGCMT_59,builds,notest --releasebase=/afs/cern.ch/atlas/offline/external
#asetup LCGCMT,LCGCMT_59,builds,notest
#asetup LCGCMT,LCGCMT_59,notest --releasebase=/afs/cern.ch/atlas/offline/external

#> cmt version                                                        v1r22
#> echo $CMTCONFIG
# i686-slc5-gcc43-opt
#> echo $CMTPROJECTPATH
# /afs/cern.ch/atlas/software/builds:/afs/cern.ch/atlas/project/tdaq/prod:/afs/cern.ch/atlas/offline/external
#> echo $CMTEXTRATAGS 
# ATLAS
#> echo $CMTSITE 
# CERN

#asetup AtlasConditions,16.3.0,builds,notest,ignore,noruntime
#source $AtlasSetup/scripts/asetup.sh AtlasConditions,16.3.0,builds,notest,ignore,noruntime
#. $setuproot/scripts/asetup.sh AtlasConditions,16.3.0,builds,notest,ignore,noruntime
#asetup 16.2.0,builds,notest --releasebase=/afs/cern.ch/atlas/software/builds
}

_cmthome ()
{
cmthome=$(abspath $cmthome) || exit
cd ${cmthome} || return 1
unset CMTPATH CMTPROJECTPATH
unset CMTROOT CMTSITE
if [ $project == AtlasRelease ]; then
    [ "$PROJRUNDBG" ] && set +x
    . ${cmthome}/setup.sh -tag_add=${release},${mode},noTest${tags:+,${tags}} || exit 1
    [ "$PROJRUNDBG" ] && set -x
    if [ -z ${projbase} ]; then
	projbase=`cmt -q show macro_value ATLAS_DIST_AREA`
	: ${projbase:?}
    else
	export CMTPATH=$(sed 's#^:*[^:]*#'${projbase}/${release}'#' <<<"$CMTPATH")
    fi
    [ -d ${projbase}/${release} ] || 
    { error "${projbase}/${release} must exist and be a directory" $FUNCNAME; exit 1; }

elif [ $project == LCGCMT -o $project == Gaudi ]; then
    [ "$PROJRUNDBG" ] && set +x
    . ${cmthome}/setup.sh -tag_add=${mode},noTest${tags:+,${tags}} || exit 1
    [ "$PROJRUNDBG" ] && set -x
    if [ -z ${projbase} ]; then
	projbase=`cmt -q show macro_value EXTERNAL_PROJECT_AREA`
	: ${projbase:?}
    fi
    [ -d ${projbase}/${project}/${release} ] ||
    { error "${projbase}/${project}/${release}: No such directory" $FUNCNAME; exit 1; }
#    export CMTPATH=${projbase}/${project}/${release}
#    export CMTPROJECTPATH=${projbase}
    path_prepend CMTPROJECTPATH ${projbase}
    export CMTPROJECTPATH
    unset CMTHOME
    unset CMTPATH

elif [ $project == CMT ]; then
    [ "$PROJRUNDBG" ] && set +x
    . ${cmthome}/setup.sh -tag_add=${mode},noTest${tags:+,${tags}} || exit 1
    [ "$PROJRUNDBG" ] && set -x
    cmtinfo=(`cmt show uses | awk '
$1 == "use" && $2 == "CMT" { gsub(/[()]/, "", $4); print $4, $3; exit }
'`)
    [ ${#cmtinfo[*]} -eq 2 ] || 
    { error "Wrong CMT info: ${cmtinfo[*]}" $FUNCNAME; exit 1; }
    [ -z ${projbase} ] && { projbase=${cmtinfo[0]}; : ${projbase:?}; }
    [ -z ${release} ] && { release=${cmtinfo[1]}; : ${release:?}; }

    [ -d ${projbase}/${project}/${release} ] || 
    { error "${projbase}/${project}/${release}: No such directory" $FUNCNAME; exit 1; }
    #{ warn "${projbase}/${project}/${release}: No such directory" $FUNCNAME; }
    export CMTPROJECTPATH=${projbase}
    unset CMTHOME
    unset CMTPATH

else
    [ "$PROJRUNDBG" ] && set +x
    . ${cmthome}/setup.sh -tag_add=${project},${release},${mode},noTest,builds${tags:+,${tags}} || exit 1
    [ "$PROJRUNDBG" ] && set -x
    if [ -z ${projbase} ]; then
	projbase=`echo $CMTPROJECTPATH | sed 's#^:\+##' | cut -d: -f1`
	: ${projbase:?}
    else
	#export CMTPATH=${projbase}/${project}/${release}
	#export CMTPROJECTPATH=$(sed 's#^:*[^:]*#'${projbase}'#' <<<"$CMTPROJECTPATH")
	path_prepend CMTPROJECTPATH ${projbase}
	export CMTPROJECTPATH
    fi
    unset CMTHOME
    unset CMTPATH
    [ -d ${projbase}/${project}/${release} ] || 
    { error "${projbase}/${project}/${release}: No such directory" $FUNCNAME; exit 1; }
fi
cd $OLDPWD
}

if [ $project = CMT ]; then
    unset setuproot
    [ "${cmthome:-}" ] ||
    { error "cmthome must be set" $FUNCNAME; exit 1; }
fi

if [ "${setuproot:-}" ]; then
    _setuproot || exit
elif [ "${cmthome:-}" ]; then
    warn "Obsolete cmthome use" $FUNCNAME
    _cmthome || exit
else
    error "Neither cmthome or setuproot set" $FUNCNAME; exit 1
fi

: ${CMTCONFIG:?}

[ -z "$pacmancache" ] || pacmancache=$(abspath $pacmancache) || exit

if [ -z "$externcache" ]; then
    [ ! -d "$pacmancache" ] &&
    { error "pacmancache or externcache must exist and be a directory" $FUNCNAME; exit 1; }
    info "setting externcache to pacmancache=$pacmancache" $FUNCNAME; externcache=$pacmancache
fi
[ -d "$externcache" ] || { error "externcache=$externcache must exist and be a directory" $FUNCNAME; exit 1; }

if [ -z "$projcache" ]; then
    [ ! -d "$pacmancache" ] &&
    { error "pacmancache or projcache must exist and be a directory" $FUNCNAME; exit 1; }
    info "setting projcache to pacmancache=$pacmancache" $FUNCNAME; projcache=$pacmancache
fi
[ -d "$projcache" ] || { error "projcache=$projcache must exist and be a directory" $FUNCNAME; exit 1; }

#[ -d "$pacmancache" ] || { error "$pacmancache must exist and be a directory" $FUNCNAME; exit 1; }

for i in rpmdir externdir projdir; do
    eval d=\$$i
    if [ "$d" ]; then
	[ -d "$d" ] || { error "$d: No such directory" $FUNCNAME; exit 1; }
	eval export $i=$(abspath $d) || exit
	[ "$PACKRPM" ] || export PACKRPM=$(dirname $scriptdir)/share
    fi
done

if [ -z "$rpmdir" ]; then
    if [ "$externdir" ] && [ -z "$projdir" ]; then
	info "setting rpmdir to externdir=$externdir" $FUNCNAME
	export rpmdir=$(abspath $externdir) || exit
    elif [ -z "$externdir" ] && [ "$projdir" ]; then
	info "setting rpmdir to projdir=$projdir" $FUNCNAME
	export rpmdir=$(abspath $projdir) || exit
    fi
fi

[ -d "$patchdir" ] || { error "patchdir=$patchdir must exist and be a directory" $FUNCNAME; exit 1; }

[ -d "$logdir" ] || { error "logdir=$logdir must exist and be a directory" $FUNCNAME; exit 1; }

# Make paths absolute
for i in logdir externcache projcache patchdir projbase; do
    eval v=\$$i; eval $i=$(abspath $v) || exit
done
[ -z "$builddir" ] || builddir=$(abspath $builddir) || exit

if [ "$configure" == yes ]; then
    config_dump
    exit
fi

# Create auxiliary directories and files
#[ -d $pacmancache/kits ] || mkdir $pacmancache/kits || exit 1
#[ -d $pacmancache/cache ] || mkdir $pacmancache/cache || exit 1

[ -d $externcache/kits ] || mkdir $externcache/kits || exit 1
[ -d $externcache/cache ] || mkdir $externcache/cache || exit 1

[ -d $projcache/kits ] || mkdir $projcache/kits || exit 1
[ -d $projcache/cache ] || mkdir $projcache/cache || exit 1

[ -d "$logdir/$project/$release" ] || mkdir -p $logdir/$project/$release || exit 1
logfile=$logdir/$project/$release/`basename $0 .sh`-${CMTCONFIG}.log

#TMPLOG=$(mktemp -t tmp.XXXXXXXXXX) || exit 1
#mv --backup=t $TMPLOG $logfile || exit 1
#chmod +r $logfile
_backup ()
{
for f; do
  [ -f "$f" ] || continue
  i=1
  while [ -f ${f}.~${i}~ ]; do
    i=$(expr ${i} + 1) || { error "expr" $FUNCNAME; return 1; }
  done
  mv $f ${f}.~${i}~ || { error "mv" $FUNCNAME; return 1; }
done
}

_backup $logfile || exit 1

main ()
{
info "Start `date`\t@ `hostname`" $FUNCNAME

[ -z "$mainpkg" -a $project == AtlasRelease ] && mainpkg=AtlasRelease
: ${mainpkg:?}
cd $projbase/${release}/$mainpkg/*/cmt || exit 1

info "Start packing from `pwd -P`" $FUNCNAME

if [ "$selected" ]; then
#    cmd="pacmancache=${pacmancache} patchdir=${patchdir} overwrite=${overwrite} platform=${platform} cmt -quiet broadcast -global -select='${selected}' - ${scriptdir}/pack-extern.sh"
    cmd="externcache=${externcache} patchdir=${patchdir} overwrite=${overwrite} cmt broadcast -global -select='${selected}' - ${scriptdir}/pack-extern.sh"
    info "$cmd" $FUNCNAME
    eval $cmd
else
    requires=$(mktemp -t tmp.XXXXXXXXXX) || return 1

#    cmd="pacmancache=${pacmancache} patchdir=${patchdir} overwrite=${overwrite} platform=${platform} cmt -quiet broadcast -global - ${scriptdir}/pack-extern.sh"
    cmd="externcache=${externcache} patchdir=${patchdir} overwrite=${overwrite} cmt broadcast -global - ${scriptdir}/pack-extern.sh"
    info "$cmd" $FUNCNAME
    eval $cmd | tee -a ${requires} || { error "tee" $FUNCNAME; return 1; }
    sed '/^#/d' ${requires} >${requires}.tmp || { error "sed" $FUNCNAME; return 1; }
    mv -f ${requires}.tmp ${requires} || { error "mv" $FUNCNAME; return 1; }
#    sed -i '/^#/d' ${requires}

    cmd="projbase=${projbase} project=${project} release=${release} projcache=${projcache} upgrade=${upgrade} mode=${mode} debuginfo=${debuginfo} debuginfosuffix=${debuginfosuffix} docfiles=${docfiles} builddir=${builddir} cmthome=${cmthome} uname=${uname} setuproot=${setuproot} ${scriptdir}/pack-proj.sh"
    info "$cmd" $FUNCNAME
    eval requires=${requires} $cmd

    rm -f ${requires} || return 0

fi

info "Finish `date`\t@ `hostname`" $FUNCNAME
}

proj ()
{
info "Start `date`\t@ `hostname`" $FUNCNAME

cmd="selected='$selected' projbase=${projbase} project=${project} release=${release} container='${container}' projcache=${projcache} upgrade=${upgrade} mode=${mode} debuginfo=${debuginfo} debuginfosuffix=${debuginfosuffix} docfiles=${docfiles} builddir=${builddir} cmthome=${cmthome} setuproot=${setuproot} externcache=${externcache} patchdir=${patchdir} overwrite=${overwrite} keep=$keep recursive=$recursive relocate=yes verify=$verify cmtcache=$cmtcache waiting=$waiting quiet=$quiet uname=${uname} ${scriptdir}/pack-proj.sh"
    info "$cmd" $FUNCNAME
    eval $cmd

local retval=$? 
info "Finish `date`\t@ `hostname`" $FUNCNAME
return $retval
}

# Do the packing
printf "\n`version`\n\n" >>$logfile
case $quiet in
    no) info "Current configuration:\n`config_dump`" $FUNCNAME 2>&1 | tee -a $logfile ;;
    yes) info "Current configuration:\n`config_dump`" $FUNCNAME >>$logfile 2>&1 ;;
    *) error "Invalid value: quiet: $quiet" $FUNCNAME ;;
esac

if [ $project == AtlasRelease ]; then
    { time main; } >>$logfile 2>&1 &
# elif [ $project == LCGCMT ]; then
#     { time extern_proj; } >>$logfile 2>&1 &
else

case $waiting in
    no) case $quiet in
	    no) { time proj; } >>$logfile 2>&1 &
	        info "`date`\tPID: $!\t@ `hostname`" $FUNCNAME ;;
            yes) { time proj; } >>$logfile 2>&1 & ;;
            *) error "Invalid value: quiet: $quiet" $FUNCNAME ;;
        esac ;;
    yes) case $quiet in
            no) info "`date`\tPID: $$\t@ `hostname`" $FUNCNAME
	        { time proj; } 2>&1 | tee -a $logfile ;;
	    yes) { time proj; } >>$logfile 2>&1; retval=$? ;;
	    *) error "Invalid value: quiet: $quiet" $FUNCNAME ;;
	 esac ;;
esac

fi

info "\n\nLog file: $logfile" $FUNCNAME

exit $retval
