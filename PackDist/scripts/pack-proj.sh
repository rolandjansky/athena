#!/bin/bash
# File: pack-proj.sh
#
# Pack project in five parts:
#                             binary platform - corresponds to CMTCONFIG value
#                             debuginfo files (for not debug build)
#                             source code
#                             documentation files
#                             other architecture independent files
#
# Author: Grigori Rybkine <Grigori.Rybkine@cern.ch>.
#

[ "$PACKPROJDEBUG" ] && set -x

: ${projbase:?}
: ${project:?} ${release:?}
#: ${platform:?}
: ${projcache:?} # ${upgrade:?} #${patchdir:?} ${overwrite:?}

: ${scriptdir:?}
. $scriptdir/pack-common.sh

: ${cmthome:?}
: ${cmtcache:?}

# : ${requires:?}
#[ -f "$requires" ] || requires=$(mktemp) || return 1

: ${externcache:?} ${patchdir:?} ${overwrite:?}

: ${CMTCONFIG:?}

##################################### functions #####################################
_mktmpdir ()
{
#
# Make temporary directory, set 'tmpdir' and,
# if possible, the TMPDIR environment variable to it
#
# (the latter allows to call mktemp(1) with specifying no template
# and have all temporary files rooted in 'tmpdir')
#
# Parameters: ['directory' in which to place the temporary directory]
#

#: ${builddir:?}
#[ -d "$builddir" ] || builddir=$(mktemp -d) || return 1
if [ -d "${1}" ]; then
    tmpdir=$(mktemp -d ${1}/pack.$$.XXXXXXXXXX) || return 1
else # make temp dir in TMPDIR, if set, or in /tmp
    tmpdir=$(mktemp -dt pack.$$.XXXXXXXXXX) || return 1
fi

#trap 'rm -rf $tmpdir; exit 1' 1 2 15
trap 'rm -rf $tmpdir; exit 1' 2 15

# want all temp files in 'tmpdir'
export TMPDIR=$tmpdir ||
warn "Temporary directory may not be cleaned up on completion: $TMPDIR" $FUNCNAME
tmppatch=$tmpdir/patch
mkdir $tmppatch || { error "mkdir" $FUNCNAME; return 1; }
if [ -n "${PACKRPM:-}" ]; then
    builddir=$tmpdir/BUILD
    mkdir ${builddir} || { error "mkdir" $FUNCNAME; return 1; }
    export builddir || { error "export" $FUNCNAME; return 1; }
    buildrootdir=$tmpdir/BUILDROOT
    mkdir ${buildrootdir} || { error "mkdir" $FUNCNAME; return 1; }
    export buildrootdir || { error "export" $FUNCNAME; return 1; }
    if [ -z "${specdir+T}" ]; then # specdir is unset
	specdir=$tmpdir/SPECS
	mkdir ${specdir} || { error "mkdir" $FUNCNAME; return 1; }
	export specdir || { error "export" $FUNCNAME; return 1; }
    fi
fi
}

_mkname ()
{
#
# Parameters: 'project' 'release' 'platform'
#                                 (bin src doc noarch debuginfo)
#
if [ $3 == bin ]; then
    sed 's/[-.]/_/g' <<<${1}_${2}_${CMTCONFIG} || { error "sed" $FUNCNAME; return 1; }
elif [ $3 == debuginfo ]; then
    sed 's/[-.]/_/g' <<<${1}_${2}_${CMTCONFIG}_debuginfo || { error "sed" $FUNCNAME; return 1; }
else
    sed 's/[-.]/_/g' <<<${1}_${2}_${3} || { error "sed" $FUNCNAME; return 1; }
fi
}

_mkver ()
{
#
# Make package version and print on the standard output
#
# Parameters: ['version']
#
local ver

ver=$(awk '
{ n=split($0, a, /[-vrp.]/)
for (i=1; i<=n; i++) {
  if (a[i] ~ /[[:digit:]]+/)
    if (v != "") v=v"."a[i]
    else v=a[i]
}
print v }
' <<<"$1")
echo ${ver:-0}
}

_getver ()
{
#
# Determine package version and print on the standard output
# (empty if cannot be determined )
#
# Parameters: 'name'
#
local fpath

fpath=${projcache}/cache/${1}.pacman

#[ -f $fpath ] || { echo 0; return; }
[ -f $fpath ] || return 0
sed -e '/^[ \t]*version(.*)[ \t]*$/!d' -e 's/version(//' -e 's/)[ \t]*$//' -e "s/['\"]//g" $fpath || { error "sed" $FUNCNAME; return 1; }
# [ "$_ver" ] && { echo $_ver; return; }
# echo 0
}

_mkcntnr ()
{
#
# Parameters: 'project'
# Print on the standard output: 'container' (container-name [access-path])
#
[ $# -eq 1 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
case $1 in
    LCGCMT) echo LCG_Release ;;
    Gaudi) echo Gaudi ;;
    Atlas?*) echo ${1}Release ;;
    tdaq-common) echo TDAQCRelease ;;
    dqm-common) echo DQMCRelease ;;
    *) error "Unknown project: $1" $FUNCNAME; return 1 ;;
esac
}

_getcntnr ()
{
#
# Parameters: 'project-release-directory' 'project' 'release'
# Print on the standard output: 'container' (container-name [access-path])
#
[ $# -eq 3 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local -a cntnr
if [ -f $1/cmt/project.cmt ]; then
# determine which package is container
cntnr=($(awk '$1 == "container" && $2 ~ /./ { print $2, $4; exit }' $1/cmt/project.cmt))
case ${#cntnr[*]} in
    0) warn "No container specification: $1/cmt/project.cmt" $FUNCNAME; return 1 ;;
    1) : ;;
    2) cntnr[1]=$(sed 's#/*$#/#' <<<${cntnr[1]}) ;;
    *) error "Invalid container specification: ${cntnr[*]}: $1/cmt/project.cmt" $FUNCNAME
	return 1 ;;
esac
echo ${cntnr[*]}
# case ${#cntnr[*]} in
#     0) warn "Cannot determine container package of $2 $3 ($1)" $FUNCNAME
# 	cntnr=($(_mkcntnr $2)) || return 1 ;;
#     1) : ;;
# #    1) cntnr[1]=. ;;
#     2) cntnr[1]=$(sed 's#/*$#/#' <<<${cntnr[1]}) ;;
#     *) error "Wrong container package specification for $2 $3 ($1): ${cntnr[*]}" $FUNCNAME
# 	return 1 ;;
# esac
local proj=$(awk '$1 == "project" && $2 ~ /./ { print $2; exit }' $1/cmt/project.cmt)
[ "$proj" ] || warn "No project specification: $1/cmt/project.cmt" $FUNCNAME
else
    error "$1/cmt/project.cmt: No such file" $FUNCNAME; return 1
fi
}

_gen_expr ()
{
#
# Parameters: 'patterns-file'...
# Print on the standard output `find' expression which is true for any pathname matching
# any of the patterns in 'patterns-file'...
#
[ $# -ge 1 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
awk '
BEGIN { expr = " (" }
{ if (NR != 1) expr = expr " -o"
  if (substr($1, 1, 1) != "*") expr = expr " -path */"$1
  else expr = expr " -path "$1
}
END { print expr " )" }
' "$@" || { error "awk" $FUNCNAME; return 1; }
}

_prep_lists ()
{
#
# Parameters: 'project-release-directory' 'project' 'release'
#                                    'bin-file' 'src-file' 'doc-file' 'noarch-file' 
#                                    'dirs-file' 'debuginfo-file'
#
[ $# -eq 9 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

if [ ${uname} = Linux ]; then

local tmppltfms=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
cp $scriptdir/platforms $tmppltfms || { error "cp" $FUNCNAME; return 1; }
sed '\%^'${CMTCONFIG}'$%d' ${tmppltfms} >${tmppltfms}.tmp ||
{ error "sed" $FUNCNAME; return 1; }
mv -f ${tmppltfms}.tmp ${tmppltfms} || { error "mv" $FUNCNAME; return 1; }
#sed -i '\%^'${CMTCONFIG}'$%d' ${tmppltfms} || { error "sed" $FUNCNAME; return 1; }

local opts=-noleaf
local _expr

# platforms to skip - all but ${CMTCONFIG}
_expr="$opts $(
awk '
BEGIN { expr = "(" }
{ if (NR != 1) expr = expr " -o"
  expr = expr " -name "$1
}
END { print expr " ) -prune" }
' $tmppltfms
)" || { error "awk" $FUNCNAME; return 1; }

# platform to pack - ${CMTCONFIG}
_expr="$_expr -o -name $CMTCONFIG -prune -fprint $4"

# exclude junk (print nothing)
_expr="$_expr -o $(_gen_expr $scriptdir/junk.ptn)" || return 1

# source code
_expr="$_expr -o ! -type d $(_gen_expr $scriptdir/src.ptn) -fprint $5" || return 1

# documentation files
_expr="$_expr -o ! -type d $(_gen_expr $scriptdir/doc.ptn) -fprint $6" || return 1

# anything else - noarch files and ALL directories
_expr="$_expr -o ! -type d -fprint $7 -o -fprint $8"
# anything else - after a cleanup - noarch files
#_expr="$_expr -o ( ! -type d -o -empty ) -fprint $7"

fi # end Linux

cd $(dirname $(dirname $1)) || { error "cd" $FUNCNAME; return 1; }

local projdir
if [ $2 == AtlasRelease ]; then
#    projdir=dist
    projdir=$(basename $(dirname $1))
else
    projdir=$2
fi

if [ ${uname} = Linux ]; then

set -f
    find ${projdir}/$3/ $_expr || { error "find" $FUNCNAME; return 1; }
set +f

else

$scriptdir/pack-find.py --bin-file=$4 --src-file=$5 --doc-file=$6 --noarch-file=$7 --dirs-file=$8 ${projdir}/$3/ || return 1

fi # end Linux

if [ -n "${debuginfosuffix:-}" ] && [ "${mode:-}" != dbg ]; then
    if [ -s $4 ]; then
        xargs sh -c 'find "$@" -name "*$debuginfosuffix"' find <$4 >>$9
        #xargs -I % find % -name "*$debuginfosuffix" <$4 >>$9
        case $? in
            123|124) error "find" $FUNCNAME; return 1 ;;
            125|126|127|1) error "xargs" $FUNCNAME; return 1 ;;
	esac
    fi
fi

cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }

[ -s $4 ] || warn "No binaries: $1 $2 $3" $FUNCNAME
[ -s $5 ] || warn "No source code: $1 $2 $3" $FUNCNAME
[ -s $6 ] || warn "No documentation: $1 $2 $3" $FUNCNAME
[ -s $7 ] || warn "No noarch: $1 $2 $3" $FUNCNAME
[ -s $8 ] || warn "No directories: $1 $2 $3" $FUNCNAME
if [ "${debuginfo:-}" = yes ] && [ "${mode:-}" != dbg ]; then
    [ -s $9 ] || warn "No debuginfo: $1 $2 $3" $FUNCNAME
fi

if [ "$PACKPROJDEBUG" ]; then
    echo "binaries:" >&2
    cat $4 >&2
    echo "source code:" >&2
    cat $5 >&2
    echo "documentation:" >&2
    cat $6 >&2
    echo "noarch:" >&2
    cat $7 >&2
    echo "directories:" >&2
    cat $8 >&2
    echo "debuginfo:" >&2
    cat $9 >&2
fi

rm -f $tmppltfms || return 0
}

_prep_lists_ ()
{
#
# Parameters: 'project-release-directory' 'project' 'release'
#                                    'bin-file' 'src-file' 'doc-file' 'noarch-file' 
#                                    'dirs-file'
#
[ $# -eq 8 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

local tmppltfms=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
cp $scriptdir/platforms $tmppltfms || { error "cp" $FUNCNAME; return 1; }
sed -i '\%^'${CMTCONFIG}'$%d' ${tmppltfms} || { error "sed" $FUNCNAME; return 1; }

local opts=-noleaf
local _expr

# platforms to skip - all but ${CMTCONFIG}
_expr="$opts $(
awk '
BEGIN { expr = "(" }
{ if (NR != 1) expr = expr " -o"
  expr = expr " -name "$1
}
END { print expr " ) -prune" }
' $tmppltfms
)"

# platform to pack - ${CMTCONFIG}
_expr="$_expr -o -name $CMTCONFIG -prune -fprint $4"

# exclude junk
_expr="$_expr -o $(
awk '
BEGIN { expr = " (" }
{ if (NR != 1) expr = expr " -o"
  if (substr($1, 1, 1) != "*") expr = expr " -path */"$1
  else expr = expr " -path "$1
}
END { print expr " )" }
' $scriptdir/junk.ptn
)"
# print nothing

# source code
_expr="$_expr -o $(
awk '
BEGIN { expr = "! -type d (" }
{ if (NR != 1) expr = expr " -o"
  if (substr($1, 1, 1) != "*") expr = expr " -path */"$1
  else expr = expr " -path "$1
}
END { print expr " )" }
' $scriptdir/src.ptn
)"
_expr="$_expr -fprint $5"
#_expr="$_expr -fprint $5 -fprintf $8 %h\n" # file's directory

# documentation files
_expr="$_expr -o $(
awk '
BEGIN { expr = "! -type d (" }
{ if (NR != 1) expr = expr " -o"
  if (substr($1, 1, 1) != "*") expr = expr " -path */"$1
  else expr = expr " -path "$1
}
END { print expr " )" }
' $scriptdir/doc.ptn
)"
_expr="$_expr -fprint $6"

# anything else - after a cleanup - noarch files and ALL directories
_expr="$_expr -o ! -type d -fprint $7 -o -fprint $8"
# anything else - after a cleanup - noarch files
#_expr="$_expr -o ( ! -type d -o -empty ) -fprint $7"

cd $1/../.. || { error "cd" $FUNCNAME; return 1; }
set -f
if [ $2 == AtlasRelease ]; then
#    find dist/$3 $_expr || return 1
    find $(basename $(dirname $1))/$3 $_expr || { error "find" $FUNCNAME; return 1; }
else
    find $2/$3 $_expr || { error "find" $FUNCNAME; return 1; }
fi
set +f
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }

[ -s $5 ] || warn "No source code: $1 $2 $3" $FUNCNAME
[ -s $6 ] || warn "No documentation: $1 $2 $3" $FUNCNAME

[ "$PACKPROJDEBUG" ] && cat $8 >&2

rm -f $tmppltfms || return 0
}

_prep_extern_debuginfo_list ()
{
#
# Parameters: 'project-release-directory' 'project' 'release' 'exps-file'
#                                                                         'debuginfo-file'
#
[ $# -eq 5 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

local tmpptns=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
awk '
BEGIN { FS = "\0" }
{ if ($3 != "") sfxs[$3] = 1 }
END { for (sfx in sfxs) print "*"sfx }
' "$4" <&- >> $tmpptns || { error "awk" $FUNCNAME; return 1; }

[ -s $tmpptns ] || { >| $5; return 0; }

local _expr
_expr="${noleaf:+-noleaf} $(_gen_expr $tmpptns) -fprint $5" || return 1

cd $1/${cntnr[1]}${cntnr[0]}/${verdir}cmt || { error "cd" $FUNCNAME; return 1; }
local installarea_prefix
installarea_prefix=$(cmt show macro_value ${2}_installarea_prefix) || { error "cmt" $FUNCNAME; return 1; }
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }

cd $(dirname $(dirname $1)) || { error "cd" $FUNCNAME; return 1; }
local projdir
if [ $2 == AtlasRelease ]; then
#    projdir=dist
    projdir=$(basename $(dirname $1))
else
    projdir=$2
fi
set -f
    find ${projdir}/$3/${installarea_prefix} $_expr || { error "find" $FUNCNAME; return 1; }
set +f
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }

[ -s $5 ] || info "No extern_debuginfo: $1 $2 $3" $FUNCNAME

if [ "$PACKPROJDEBUG" ]; then
    echo "extern_debuginfo:" >&2
    cat $5 >&2
fi

rm -f $tmpptns || return 0
}

_clean ()
{
#
# Parameters: 'project-release-directory' 'project' 'release'
#
[ $# -eq 3 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local projdir

if [ $2 == AtlasRelease ]; then
#    projdir=dist
    projdir=$(basename $(dirname $1))
else
    projdir=$2
fi

rm -rf $tmpdir/${projdir}/$3 || { error "rm" $FUNCNAME; return 1; }
}

_copy ()
{
#
# Usage: _copy [OPTION]... 'project-release-directory' 'project' 'release' 'names-file'...
#
#  -n   do not recursively descend directories
#  -r   recursively descend directories (the default)
#  -x PATTERN   exclude files that match PATTERN
#  -X FILE   exclude files that match any pattern in FILE
#
local opts exclude exclude_from
OPTIND=1
while getopts "nrx:X:" opt; do
    case $opt in
	n) opts=--no-recursion ;;
	r) opts=--recursion ;;
	x) exclude="${OPTARG}" ;;
	X) exclude_from="${OPTARG}" ;;
	\?) error "Invalid option: -${OPTARG}" $FUNCNAME; return 1 ;;
	:) error "Argument required for option: -${OPTARG}" $FUNCNAME; return 1 ;;
    esac
done

shift $(($OPTIND - 1))
[ $# -ge 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

cd $(dirname $(dirname $1)) || { error "cd" $FUNCNAME; return 1; }
shift 3

#local tmperr=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

local list sc
for list; do
[ -s $list ] || continue

# want to check exit status of the first tar in the pipe
exec 3>&1 || { error "exec" $FUNCNAME; return 1; }
sc=`((tar ${tar_basic_opts} ${opts} -cf - \
      -X $scriptdir/pltfm-junk.ptn \
      -X $scriptdir/extern-junk.ptn \
      ${exclude_from:+-X "${exclude_from}"} \
      ${exclude:+--exclude="${exclude}"} \
      -T $list 3>&- 4>&-; echo $? >&4) | \
      tar ${opts} -C $tmpdir -xf - >&3 3>&- 4>&-) 4>&1 >&-` ||
{ warn "Disk space on the filesystem of $tmpdir:
$(cat $list|duf -p $tmpdir)" $FUNCNAME
exec 3>&-
error "tar" $FUNCNAME; return 1; }
exec 3>&-
[ $sc -eq 0 ] || { error "tar" $FUNCNAME; return 1; }

# BSDTAR issues on stderr
# tar: getpwuid(XXXXX) failed: No such file or directory
# if no matching entry for user uid XXXXX is found in password database
# GNUTAR does NOT
done

#rm -f $tmperr
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }
}

_copy_ ()
{
#
# Parameters: 'project-release-directory' 'project' 'release' 'names-file'
#
[ $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

if [ $2 == AtlasRelease ]; then
#    rm -rf $tmpdir/dist/$3 || return 1
    rm -rf $tmpdir/$(basename $(dirname $1))/$3 || return 1
else
    rm -rf $tmpdir/$2/$3 || return 1
fi

cd $1/../.. || { error "cd" $FUNCNAME; return 1; }

local tmperr=$(mktemp -t tmp.XXXXXXXXXX) || return 1
tar ${tar_basic_opts} -cf - -X $scriptdir/junk.ptn -X $scriptdir/pltfm-junk.ptn \
    -T $4 2>|$tmperr | (cd $tmpdir; tar -xf -) || {

warn "Disk space on the filesystem of $tmpdir:
$(cat $4|duf -p $tmpdir)" $FUNCNAME

error "tar" $FUNCNAME; return 1; }
    
# cannot check exit status
[ -s $tmperr ] && { error "\n$(<$tmperr)" $FUNCNAME; return 1; }

rm -f $tmperr
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }
}

_install_dirs ()
{
#
# Parameters: ['directories-file'...]
#
cd $tmpdir || { error "cd" $FUNCNAME; return 1; }
local list
for list; do
    [ -s $list ] || continue
    xargs install -d <$list
#     xargs mkdir -p <$list
    case $? in
	123|124) error "install" $FUNCNAME; return 1 ;;
# 	123|124) error "mkdir" $FUNCNAME; return 1 ;;
	125|126|127|1) error "xargs" $FUNCNAME; return 1 ;;
    esac
done
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }
}

_symlinks ()
{
#
# Usage: _symlinks [OPTION]... 'project-release-directory' 'project' 'release' ['exps-file']
#
#  -I   relink  symbolic links to external debuginfo files
#
local debuginfo
OPTIND=1
while getopts "I" opt; do
    case $opt in
	I) debuginfo=yes ;;
	\?) error "Invalid option: -${OPTARG}" $FUNCNAME; return 1 ;;
	:) error "Argument required for option: -${OPTARG}" $FUNCNAME; return 1 ;;
    esac
done

shift $(($OPTIND - 1))
[ $# -eq 3 -o $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local tmperr=$(mktemp -t tmp.XXXXXXXXXX) || return 1
local projdir

if [ $2 == AtlasRelease ]; then
#    projdir=dist
    projdir=$(basename $(dirname $1))
else
    projdir=$2
fi

[ -d $tmpdir/${projdir}/$3 ] || return 0

$scriptdir/relink.py -S $1 -H ${projdir}/$3 ${4:+-X ${4}} ${debuginfo:+-I} $tmpdir/${projdir}/$3 >/dev/null 2>|$tmperr ||
{ [ -s $tmperr ] && cat $tmperr >&2
error "$scriptdir/relink.py" $FUNCNAME; return 1; }

[ -s $tmperr ] && warn "\n$(<$tmperr)" $FUNCNAME

if [ -z "${4:-}" ]; then
# check symlinks left
type symlinks >/dev/null || { warn "type" $FUNCNAME; return 0; }
symlinks -r $tmpdir/${projdir}/$3 | grep '^absolute\|^other_fs\|^dangling\|^messy' >|$tmperr

# check exit status instead?
[ -s $tmperr ] && warn "Suspicious symlinks left:\n$(<$tmperr)" $FUNCNAME
fi

rm -f $tmperr || return 0
}

_tar ()
{
#
# Parameters: 'project' 'release' 'name'
#
[ $# -eq 3 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
#cd $tmpdir || { error "cd" $FUNCNAME; return 1; }
local reldir

if [ $1 == AtlasRelease ]; then
#    reldir=dist/$2
    reldir=$(basename ${projbase})/$2
else
    reldir=$1/$2
fi

local fpath=${projcache}/kits/${3}.tar.gz

rm -f ${fpath} || { error "rm" $FUNCNAME; return 1; }

[ -d $tmpdir/$reldir ] ||
{ info "Nothing to include: $fpath" $FUNCNAME; return 0; }
# { install -d $reldir && info "Created directory '$reldir'" $FUNCNAME; } ||
# { error "install" $FUNCNAME; return 1; }

info "Writing $fpath" $FUNCNAME

#    -z -v --remove-files \

tar ${tar_basic_opts} -C $tmpdir -cf $fpath \
    -z -v \
    $reldir >&2 ||
{ warn "Disk space on the filesystem of `dirname $fpath`:
$(duf -p `dirname $fpath` $tmpdir/$reldir)" $FUNCNAME
rm -f $fpath
error "tar" $FUNCNAME; return 1; }

#cd $OLDPWD
rm -rf $tmpdir/$reldir || return 0
}

_patch ()
{
#
# Parameters: 'project' 'release' 'name'
#
[ $# -eq 3 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local reldir=$1/$2
[ -d "$tmppatch/$reldir" ] || return 0

local tmplist=$(mktemp -t tmp.XXXXXXXXXX) || return 1

cd $tmppatch || { error "cd" $FUNCNAME; return 1; }
#find $reldir -noleaf -type f -fprint $tmplist ||
find $reldir ${noleaf:+-noleaf} -type f >>$tmplist ||
{ error "find" $FUNCNAME; cd $OLDPWD; return 1; }
cd $OLDPWD

local fpath=${projcache}/kits/${3}.tar

gunzip -t ${fpath}.gz || { error "gunzip" $FUNCNAME; return 1; }
gunzip -f ${fpath}.gz || { error "gunzip" $FUNCNAME; return 1; }

info "Appending to ${fpath}.gz" $FUNCNAME

#tar -v ${tar_basic_opts} -rf $fpath -C $tmppatch $reldir >&2 ||
tar -v ${tar_basic_opts} -rf $fpath -C $tmppatch -T $tmplist >&2 ||
{ rm -rf $fpath; error "tar" $FUNCNAME; return 1; }
rm -f $tmplist
gzip -f ${fpath} || { error "gzip" $FUNCNAME; return 1; }
}

_settings ()
{
#
# Pack AtlasSettings, AtlasLogin packages
# Pack AtlasSetup package
# Print on the standard output information on them (format: 'name' 'package' 'version')
#
local tmppre=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local name fpath curver nver
local -a pkg

cd ${cmthome} || { error "cd" $FUNCNAME; return 1; }
cmt show uses | awk '
$1 == "use" && $2 ~ /Atlas(Settings|Login)/ { gsub(/[()]/, "", $4); print $2, $3, $4}
' |
while read -a pkg; do
    #name=$(sed "s/${pkg[0]}-${pkg[0]}/${pkg[0]}/" <<<${pkg[0]}-${pkg[1]})
    name=${pkg[0]}

#    echo $name ${pkg[0]} ${pkg[1]}

    fpath=${projcache}/kits/$name.tar.gz
    curver=$(_mkver ${pkg[1]})
    if [ ${curver} == "$(_getver $name)" ]; then
	if [ -f ${fpath} ]; then
	    info "Existing ${fpath} will not be overwritten" $FUNCNAME
	else
	    error "${fpath}: No such file" $FUNCNAME
	fi
    elif [ -f ${fpath} -a -f ${projcache}/cache/$name.pacman -a "${keep}" == yes ]; then
	warn "Keeping ${fpath} ($(_getver $name)) while using different version: ${pkg[0]} ${pkg[1]} ${pkg[2]}" $FUNCNAME
	nver=$(tar -ztf ${fpath}|sed -n 's!^'${pkg[0]}'/\([^/]\+\)/*$!\1!p') ||
	{ error "sed" $FUNCNAME; }
	if [ "$nver" ]; then
	    pkg[1]=$nver
	else
	    error "Cannot determine native version: ${fpath}" $FUNCNAME
	fi
    else
	info "Writing ${fpath}" $FUNCNAME

	tar -v ${tar_basic_opts} -cf ${fpath} -z \
	    -X $scriptdir/junk.ptn \
	    -C ${pkg[2]} ${pkg[0]}/${pkg[1]} 1>&2 ||
	{ rm -f ${fpath}; error "tar" $FUNCNAME; }
	_pre ${pkg[2]}/${pkg[0]}/${pkg[1]} ${pkg[0]} ${pkg[1]} >|${tmppre}
	_pacman $name "" "" ${tmppre} ${curver}
    fi

    echo $name ${pkg[0]} ${pkg[1]}

    [ ! -f $PACKRPM/bin/pack-rpm-settings.sh ] || . $PACKRPM/bin/pack-rpm-settings.sh

done # || return 1
cd $OLDPWD

if [ -d "${setuproot:-}" ]; then
    cd ${setuproot}/cmt || { error "cd" $FUNCNAME; return 1; }
    pkg[0]=$(basename ${setuproot})
    #pkg[0]=$(cmt show macro_value package)
    pkg[1]=$(cmt show macro_value version)
    pkg[2]=$(dirname ${setuproot})

    name=${pkg[0]}

    fpath=${projcache}/kits/$name.tar.gz
    curver=$(_mkver ${pkg[1]})
    if [ ${curver} == "$(_getver $name)" ]; then
	if [ -f ${fpath} ]; then
	    info "Existing ${fpath} will not be overwritten" $FUNCNAME
	else
	    error "${fpath}: No such file" $FUNCNAME
	fi
    elif [ -f ${fpath} -a -f ${projcache}/cache/$name.pacman -a "${keep}" == yes ]; then
	warn "Keeping ${fpath} ($(_getver $name)) while using different version: ${pkg[0]} ${pkg[1]} ${pkg[2]}" $FUNCNAME
        # determine native version in the tarball
        nver=$(tar -O -zxf ${fpath} ${pkg[0]}/cmt/version.cmt) ||
        { error "tar" $FUNCNAME; }
        if [ "$nver" ]; then
            pkg[1]=$nver
        else
            error "Cannot determine native version: ${fpath}" $FUNCNAME
        fi
    else
	info "Writing ${fpath}" $FUNCNAME

	tar -v ${tar_basic_opts} -cf ${fpath} -z \
	    -X $scriptdir/junk.ptn \
	    -C ${pkg[2]} ${pkg[0]} 1>&2 ||
	{ rm -f ${fpath}; error "tar" $FUNCNAME; }
	_pre ${pkg[2]}/${pkg[0]} ${pkg[0]} ${pkg[1]} >|${tmppre}
	_pacman $name "" "" ${tmppre} ${curver}
    fi

    echo $name ${pkg[0]} ${pkg[1]}

    [ ! -f $PACKRPM/bin/pack-rpm-settings.sh ] || . $PACKRPM/bin/pack-rpm-settings.sh

    cd $OLDPWD
fi

rm -f $tmppre || return 0
}

_pre ()
{
#
# Print on the standard output pre-install script
#
# Parameters: 'project-release-directory' 'project'|'package' 'release'|'version'
#
if [ $2 == AtlasSettings ]; then
awk '
BEGIN { printf "mkdir -p @INSTALL_PREFIX@/.cmtsite; if [ -f @INSTALL_PREFIX@/.cmtsite/requirements ]; then mv -f @INSTALL_PREFIX@/.cmtsite/requirements @INSTALL_PREFIX@/.cmtsite/requirements.orig; cmtconfig_=`awk \"/^macro cmtconfig_default/{print \\\\\\$3}\" @INSTALL_PREFIX@/.cmtsite/requirements.orig`; fi; printf \""}
{ gsub(/\$/,"\\$"); printf "%s\\n", $0 }
END { printf "\" >@INSTALL_PREFIX@/.cmtsite/requirements; if [ \"${cmtconfig_:-}\" ]; then sed \"s/^macro cmtconfig_default.*/macro cmtconfig_default ${cmtconfig_}/g\" @INSTALL_PREFIX@/.cmtsite/requirements >@INSTALL_PREFIX@/.cmtsite/requirements.$$ && mv -f @INSTALL_PREFIX@/.cmtsite/requirements.$$ @INSTALL_PREFIX@/.cmtsite/requirements; fi" }
' $scriptdir/basic-requirements.template

[ $? -eq 0 ] || { error "awk" $FUNCNAME; return 1; }

elif [ $2 == AtlasLogin ]; then
awk '
BEGIN { printf "mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/requirements ]; then mv -f @INSTALL_PREFIX@/cmtsite/requirements @INSTALL_PREFIX@/cmtsite/requirements.orig; cmtconfig_=`awk \"/^macro cmtconfig_default/{print \\\\\\$3}\" @INSTALL_PREFIX@/cmtsite/requirements.orig`; fi; printf \""}
FILENAME == "'$scriptdir/requirements.template'" { gsub(/\$/,"\\$"); if (NR != 1) printf "\\n%s", $0; else printf "%s", $0 }
FILENAME == "'$scriptdir/platforms'" { printf "\\t%s %s", $1, $1 }
END { printf "\\n\" >@INSTALL_PREFIX@/cmtsite/requirements; if [ \"${cmtconfig_:-}\" ]; then sed \"s/^macro cmtconfig_default.*/macro cmtconfig_default ${cmtconfig_}/g\" @INSTALL_PREFIX@/cmtsite/requirements >@INSTALL_PREFIX@/cmtsite/requirements.$$ && mv -f @INSTALL_PREFIX@/cmtsite/requirements.$$ @INSTALL_PREFIX@/cmtsite/requirements; fi" }
' $scriptdir/requirements.template $scriptdir/platforms

[ $? -eq 0 ] || { error "awk" $FUNCNAME; return 1; }

elif [ $2 == AtlasSetup ]; then
cat <<\EOF
mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.sh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.sh @INSTALL_PREFIX@/cmtsite/asetup.sh.orig; fi; printf "export AtlasSetup=@INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh \$*\nalias asetup=\'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.sh; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.csh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.csh @INSTALL_PREFIX@/cmtsite/asetup.csh.orig; fi; printf "setenv AtlasSetup @INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh \$*\nalias asetup \'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.csh; asetup_defaults="standalone = True\nreleasesarea = @INSTALL_PREFIX@ `dirname @INSTALL_PREFIX@` $(dirname $(dirname @INSTALL_PREFIX@))/\${CMTCONFIG}\n"; if [ ! -f @INSTALL_PREFIX@/cmtsite/asetup_defaults ]; then  printf "${asetup_defaults}" >@INSTALL_PREFIX@/cmtsite/asetup_defaults; elif ! printf "${asetup_defaults}" | cmp -s @INSTALL_PREFIX@/cmtsite/asetup_defaults -; then printf "${asetup_defaults}" >@INSTALL_PREFIX@/cmtsite/asetup_defaults.new; fi
EOF

#mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.sh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.sh @INSTALL_PREFIX@/cmtsite/asetup.sh.orig; fi; printf "export AtlasSetup=@INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh \$*\nalias asetup=\'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.sh; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.csh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.csh @INSTALL_PREFIX@/cmtsite/asetup.csh.orig; fi; printf "setenv AtlasSetup @INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh \$*\nalias asetup \'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.csh; asetup_defaults="standalone = True\nreleasesarea = @INSTALL_PREFIX@ `dirname @INSTALL_PREFIX@`\n"; if [ ! -f @INSTALL_PREFIX@/cmtsite/asetup_defaults ]; then  printf "${asetup_defaults}" >@INSTALL_PREFIX@/cmtsite/asetup_defaults; elif ! printf "${asetup_defaults}" | cmp -s @INSTALL_PREFIX@/cmtsite/asetup_defaults -; then printf "${asetup_defaults}" >@INSTALL_PREFIX@/cmtsite/asetup_defaults.new; fi

#mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.sh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.sh @INSTALL_PREFIX@/cmtsite/asetup.sh.orig; fi; printf "export AtlasSetup=@INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh \$*\nalias asetup=\'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.sh; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.csh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.csh @INSTALL_PREFIX@/cmtsite/asetup.csh.orig; fi; printf "setenv AtlasSetup @INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh \$*\nalias asetup \'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.csh; if [ -f @INSTALL_PREFIX@/cmtsite/asetup_defaults ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup_defaults @INSTALL_PREFIX@/cmtsite/asetup_defaults.orig; fi; printf "standalone = True\nreleasesarea = @INSTALL_PREFIX@ `dirname @INSTALL_PREFIX@`\n" >@INSTALL_PREFIX@/cmtsite/asetup_defaults

#mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.sh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.sh @INSTALL_PREFIX@/cmtsite/asetup.sh.orig; fi; printf "export AtlasSetup=@INSTALL_PREFIX@/AtlasSetup\nif [ -z \\\"\${AtlasSetupSite:-}\\\" ]; then\n  export AtlasSetupSite=@INSTALL_PREFIX@/cmtsite/defaults\nfi\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh --releasesarea=@INSTALL_PREFIX@ standalone \$*\nalias asetup=\'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh --releasesarea=@INSTALL_PREFIX@ standalone\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.sh; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.csh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.csh @INSTALL_PREFIX@/cmtsite/asetup.csh.orig; fi; printf "setenv AtlasSetup @INSTALL_PREFIX@/AtlasSetup\nif ( \$?AtlasSetupSite == 0 ) then\n  setenv AtlasSetupSite @INSTALL_PREFIX@/cmtsite/defaults\nendif\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh --releasesarea=@INSTALL_PREFIX@ standalone \$*\nalias asetup \'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh --releasesarea=@INSTALL_PREFIX@ standalone\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.csh; if [ -f @INSTALL_PREFIX@/cmtsite/defaults ]; then mv -f @INSTALL_PREFIX@/cmtsite/defaults @INSTALL_PREFIX@/cmtsite/defaults.orig; fi; printf "releasesarea = `dirname @INSTALL_PREFIX@`\n" >@INSTALL_PREFIX@/cmtsite/defaults

#mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.sh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.sh @INSTALL_PREFIX@/cmtsite/asetup.sh.orig; fi; printf "export AtlasSetup=@INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh --input=@INSTALL_PREFIX@/cmtsite/defaults \$*\nalias asetup=\'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.sh --input=@INSTALL_PREFIX@/cmtsite/defaults\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.sh; if [ -f @INSTALL_PREFIX@/cmtsite/asetup.csh ]; then mv -f @INSTALL_PREFIX@/cmtsite/asetup.csh @INSTALL_PREFIX@/cmtsite/asetup.csh.orig; fi; printf "setenv AtlasSetup @INSTALL_PREFIX@/AtlasSetup\nsource @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh --input=@INSTALL_PREFIX@/cmtsite/defaults \$*\nalias asetup \'source @INSTALL_PREFIX@/AtlasSetup/scripts/asetup.csh --input=@INSTALL_PREFIX@/cmtsite/defaults\'\n" >@INSTALL_PREFIX@/cmtsite/asetup.csh; if [ -f @INSTALL_PREFIX@/cmtsite/defaults ]; then mv -f @INSTALL_PREFIX@/cmtsite/defaults @INSTALL_PREFIX@/cmtsite/defaults.orig; fi; printf "standalone = True\nreleasesarea = @INSTALL_PREFIX@ `dirname @INSTALL_PREFIX@`\n" >@INSTALL_PREFIX@/cmtsite/defaults
[ $? -eq 0 ] || { error "cat" $FUNCNAME; return 1; }

#awk '
#BEGIN { printf "; printf \""}
#{ printf "\\t%s \\\"%s\\\"", $1, $1 }
#END { printf "\" >>@INSTALL_PREFIX@/cmtsite/requirements" }
#' $scriptdir/platforms
# awk '
# BEGIN { printf "PATH=/bin:/usr/bin; mkdir -p @INSTALL_PREFIX@/cmtsite; if [ -f @INSTALL_PREFIX@/cmtsite/requirements ]; then mv -f @INSTALL_PREFIX@/cmtsite/requirements @INSTALL_PREFIX@/cmtsite/requirements.orig; cmtconfig_=`awk \"/^set CMTCONFIG/{print \\$3}\" @INSTALL_PREFIX@/cmtsite/requirements.orig`; fi; printf \""}
# { gsub(/\$/,"\\$"); printf "%s\\n", $0 }
# END { printf "\" >@INSTALL_PREFIX@/cmtsite/requirements; if [ \"$cmtconfig_\" ]; then sed -i \"/^set CMTCONFIG/d\" @INSTALL_PREFIX@/cmtsite/requirements; sed -n \"/^set CMTCONFIG/,\\$p\" @INSTALL_PREFIX@/cmtsite/requirements.orig >>@INSTALL_PREFIX@/cmtsite/requirements; fi" }
# ' $scriptdir/requirements.template
#[ $? -eq 0 ] || { error "awk" $FUNCNAME; return 1; }

#elif [ $2 == AtlasCore ]; then
#elif [[ $2 == Atlas?* || $2 == LCGCMT ]]; then
else
: ${cmtver:?}
: ${CMTCONFIG:?}
if [ "${relocate:-}" = yes ]; then
cat <<EOF
sed "s/^macro cmtconfig_default.*$/macro cmtconfig_default $CMTCONFIG/g" @INSTALL_PREFIX@/cmtsite/requirements >@INSTALL_PREFIX@/cmtsite/requirements.\$\$ && mv -f @INSTALL_PREFIX@/cmtsite/requirements.\$\$ @INSTALL_PREFIX@/cmtsite/requirements; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; cmt config
EOF
else
cat <<EOF
sed "s/^set CMTCONFIG.*$/set CMTCONFIG $CMTCONFIG/g" @INSTALL_PREFIX@/cmtsite/requirements >@INSTALL_PREFIX@/cmtsite/requirements.\$\$ && mv -f @INSTALL_PREFIX@/cmtsite/requirements.\$\$ @INSTALL_PREFIX@/cmtsite/requirements; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; cmt config
EOF
fi
#PATH=/bin:/usr/bin; sed -i "s/^set CMTCONFIG.*$/set CMTCONFIG $CMTCONFIG/g" @INSTALL_PREFIX@/cmtsite/requirements; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; cmt -quiet config
[ $? -eq 0 ] || { error "cat" $FUNCNAME; return 1; }
fi
}

_post ()
{
#
# Print on the standard output post-install script
#
# Parameters: 'project-release-directory' 'project'|'package' 'release'|'version'
#
: ${cmtver:?}
: ${CMTCONFIG:?}
: ${verdir?} ${cntnr[0]:?} #${cntnr[1]:?}
case ${CMTCONFIG} in
    *dbg*) local mode=dbg ;;
    *) local mode=opt ;;
esac

local cntnrcmt=${cntnr[1]}${cntnr[0]}/${verdir}cmt

cd $1/$cntnrcmt || return 1
[ -f ${tmppatch}/$2/$3/$cntnrcmt/requirements ] &&
export CMTUSERCONTEXT=$(echo ${tmppatch}/$2/$3/$cntnrcmt)
local project_post=$(cmt -q -tag_add=PACK show macro_value $(sed 's/[-.]/_/g' <<<${2})_post)
#local project_post=$(cmt -tag_add=PACK show macro_value ${2}_post)
unset CMTUSERCONTEXT
cd $OLDPWD

if [ "${relocate:-}" = yes ]; then

if [ $2 == AtlasRelease ]; then
: ${CMTCONFIG:?}
case ${CMTCONFIG} in
    *dbg*) local mode=dbg ;;
    *) local mode=opt ;;
esac
cat <<EOF
. @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; export CMTCONFIG=$CMTCONFIG; cd @INSTALL_PREFIX@/.cmtsite; [ -f ./setup.sh ] || cmt config; . ./setup.sh; cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt; cmt br - cmt -no_cleanup config; ${project_post:+${project_post}; }true
EOF
#unset CMTROOT; cd @INSTALL_PREFIX@/.cmtsite; . ./setup.sh; cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt; cmt br - cmt -no_cleanup config; ${project_post:+${project_post}; }true
else
cat <<EOF
. @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; export CMTCONFIG=$CMTCONFIG; cd @INSTALL_PREFIX@/.cmtsite; [ -f ./setup.sh ] || cmt config; . ./setup.sh; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt br - 'cmt -no_cleanup config'; ${project_post:+${project_post}; }true
EOF
#unset CMTROOT; cd @INSTALL_PREFIX@/.cmtsite; . ./setup.sh; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt br - 'cmt -no_cleanup config'; ${project_post:+${project_post}; }true
fi

else

if [ $2 == AtlasRelease ]; then
: ${CMTCONFIG:?}
case ${CMTCONFIG} in
    *dbg*) local mode=dbg ;;
    *) local mode=opt ;;
esac
cat <<EOF
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${3},${mode},noTest; cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt; cmt br - cmt -no_cleanup config; cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/Control/AthenaRunTime/*/cmt; . ./setup.sh; cmt build library_links; ${project_post:+cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt; ${project_post}; }true
EOF

#PATH=/bin:/usr/bin; unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${3},${mode},noTest; cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt; cmt -quiet br - cmt -quiet -no_cleanup config; cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/Control/AthenaRunTime/*/cmt; . setup.sh; cmt build library_links; ${project_post:+cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt; ${project_post}; }true
elif [ $2 == LCGCMT ]; then
cat <<EOF
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH CMTHOME CMTUSERCONTEXT; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt br - 'cmt -no_cleanup config;cmt build library_links'; ${project_post:+${project_post}; }true
EOF

#PATH=/bin:/usr/bin; unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt -quiet br - 'cmt -quiet -no_cleanup config;cmt -quiet build library_links'; ${project_post:+${project_post}; }true
elif [ $2 == Gaudi ]; then
cat <<EOF
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH CMTHOME CMTUSERCONTEXT; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt br - 'cmt -no_cleanup config;cmt build library_links'; ${project_post:+${project_post}; }true
EOF

#PATH=/bin:/usr/bin; unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt -quiet br - 'cmt -quiet -no_cleanup config;cmt -quiet build library_links'; ${project_post:+${project_post}; }true
elif [[ $2 == Atlas?* ]]; then # Atlas projects
cat <<EOF
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${2},${3},${mode},noTest; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt br - 'cmt -no_cleanup config;cmt build library_links'; ${project_post:+${project_post}; }true
EOF

#PATH=/bin:/usr/bin; unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${2},${3},${mode},noTest; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt -quiet br - 'cmt -quiet -no_cleanup config;cmt -quiet build library_links'; ${project_post:+${project_post}; }true
else
cat <<EOF
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH CMTHOME CMTUSERCONTEXT; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt br - 'cmt -no_cleanup config;cmt build library_links'; ${project_post:+${project_post}; }true
EOF

#PATH=/bin:/usr/bin; unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt -quiet br - 'cmt -quiet -no_cleanup config;cmt -quiet build library_links'; ${project_post:+${project_post}; }true
# cat <<EOF
# PATH=/bin:/usr/bin; unset CMTROOT CMTSITE; . @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest; unset CMTPATH; cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt; cmt -quiet br - cmt -quiet -no_cleanup config; ${project_post:+${project_post}; }true
# EOF

fi

fi
}

_preun ()
{
#
# Print on the standard output preun-install script
#
# Parameters: 'project-release-directory' 'project'|'package' 'release'|'version'
#
: ${cmtver:?}
: ${CMTCONFIG:?}
: ${verdir?} ${cntnr[0]:?} #${cntnr[1]:?}
case ${CMTCONFIG} in
    *dbg*) local mode=dbg ;;
    *) local mode=opt ;;
esac

local cntnrcmt=${cntnr[1]}${cntnr[0]}/${verdir}cmt

cd $1/$cntnrcmt || return 1
[ -f ${tmppatch}/$2/$3/$cntnrcmt/requirements ] &&
export CMTUSERCONTEXT=$(echo ${tmppatch}/$2/$3/$cntnrcmt)
local project_preun=$(cmt -q -tag_add=PACK show macro_value $(sed 's/[-.]/_/g' <<<${2})_preun)
#local project_preun=$(cmt -tag_add=PACK show macro_value ${2}_preun)
local installarea=$(cmt -q show macro_value ${2}_installarea_prefix)
unset CMTUSERCONTEXT
cd $OLDPWD

if [ "${relocate:-}" = yes ]; then

: ${cmtver:?}
: ${CMTCONFIG:?}
if [ $2 == AtlasRelease ]; then
case ${CMTCONFIG} in
    *dbg*) local mode=dbg ;;
    *) local mode=opt ;;
esac
if [ "${project_preun:-}" ]; then
# sed -i "s/^macro cmtconfig_default.*$/macro cmtconfig_default $CMTCONFIG/g" @INSTALL_PREFIX@/.cmtsite/requirements
# unset CMTROOT; cd @INSTALL_PREFIX@/.cmtsite;
# (. @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; cmt config)
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
. @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh
export CMTCONFIG=$CMTCONFIG
cd @INSTALL_PREFIX@/.cmtsite;
[ -f ./setup.sh ] || cmt config
. ./setup.sh;
cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt;
${project_preun}
cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt;
true
fi
EOF
fi
else
if [ "${project_preun:-}" ]; then
# sed -i "s/^macro cmtconfig_default.*$/macro cmtconfig_default $CMTCONFIG/g" @INSTALL_PREFIX@/.cmtsite/requirements
# unset CMTROOT; cd @INSTALL_PREFIX@/.cmtsite;
# (. @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh; cmt config)
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
. @INSTALL_PREFIX@/CMT/${cmtver}/mgr/setup.sh
export CMTCONFIG=$CMTCONFIG
cd @INSTALL_PREFIX@/.cmtsite;
[ -f ./setup.sh ] || cmt config
. ./setup.sh;
cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt
${project_preun}
cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
true
fi
EOF
fi
fi

else

if [ $2 == AtlasRelease ]; then
: ${CMTCONFIG:?}
case ${CMTCONFIG} in
    *dbg*) local mode=dbg ;;
    *) local mode=opt ;;
esac
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite;
. @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${3},${mode},noTest;
${project_preun:+cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt;
${project_preun}
}cd @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/AtlasRelease/*/cmt;
rm -rf @INSTALL_PREFIX@/$(basename $(dirname $1))/$3/$installarea/${CMTCONFIG}/lib;
true
fi
EOF

#PATH=/bin:/usr/bin;
elif [ $2 == LCGCMT ]; then
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite;
. @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest;
unset CMTPATH CMTHOME CMTUSERCONTEXT;
${project_preun:+cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt
${project_preun}
}cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
rm -rf @INSTALL_PREFIX@/$2/$3/$installarea/${CMTCONFIG}/lib;
true
fi
EOF

#PATH=/bin:/usr/bin;
#cmt -quiet br - 'rm -f Makefile setup.sh setup.csh cleanup.sh cleanup.csh';
#cmt -quiet br - 'cmt -quiet remove library_links; rm -f Makefile setup.sh setup.csh cleanup.sh cleanup.csh';

elif [ $2 == Gaudi ]; then
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite;
. @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest;
unset CMTPATH CMTHOME CMTUSERCONTEXT
${project_preun:+cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
${project_preun}
}cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
cmt br - 'cmt remove library_links';
true
fi
EOF

#PATH=/bin:/usr/bin;
#cmt -quiet br - 'cmt show applied_patterns|grep -q declare_installed_libraries&&cmt -quiet remove library_links; rm -f Makefile setup.sh setup.csh cleanup.sh cleanup.csh';

elif [[ $2 == Atlas?* ]]; then # Atlas projects
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite;
. @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${2},${3},${mode},noTest;
${project_preun:+cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
${project_preun}
}cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
rm -rf @INSTALL_PREFIX@/$2/$3/$installarea/${CMTCONFIG}/lib;
true
fi
EOF

#PATH=/bin:/usr/bin;
#cmt -quiet br - 'rm -f Makefile setup.sh setup.csh cleanup.sh cleanup.csh';

else
cat <<EOF
if [ \$1 -eq 0 ]; then
shift \$#;
unset CMTROOT CMTSITE; cd @INSTALL_PREFIX@/cmtsite;
. @INSTALL_PREFIX@/cmtsite/setup.sh -tag_add=${mode},noTest;
unset CMTPATH CMTHOME CMTUSERCONTEXT;
${project_preun:+cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
${project_preun}
}cd @INSTALL_PREFIX@/$2/$3/$cntnrcmt;
cmt br - 'cmt remove library_links';
true
fi
EOF

#PATH=/bin:/usr/bin;
#cmt -quiet br - 'cmt -quiet remove library_links; rm -f Makefile setup.sh setup.csh cleanup.sh cleanup.csh';

fi

fi
}

_pacman ()
{
#
# Write pacman file
#
# Parameters: 'name' ['requires-file' ['postinstall-script' ['preinstall-script' ['version']]]]
#
local reqs fpath ver _ver

fpath=${projcache}/cache/${1}.pacman

if [ "$5" ]; then
    ver=$5
else
#    ver=$(expr $(_getver ${1}) + 1 2>/dev/null) ||
    _ver=$(_getver ${1}) || { _ver=; warn "${fpath}: Version reset" $FUNCNAME; }
    ver=$(expr ${_ver:-0} + 1) ||
    { ver=1; warn "${fpath}: Version set to ${ver}" $FUNCNAME; }
fi

info "Writing ${fpath}, version ${ver}" $FUNCNAME

# add dependencies
if [ -s "$2" ]; then

while read req rest; do
    reqs="${reqs}
package('${req}')"
done <${2}

fi

# write pacman file
cat <<EOF >|${fpath}

description('Package ${1}')
version('${ver}')
${reqs}

EOF

# add preinstall-script
if [ -s "$4" ]; then
    echo "shell('$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $4)')" >>${fpath}
fi

if [ -f ${projcache}/kits/${1}.tar.gz ]; then
cat <<EOF >>${fpath}

downloadUntar('../kits/${1}.tar.gz')

EOF
else
    warn "${projcache}/kits/${1}.tar.gz: No such file" $FUNCNAME
fi

# add postinstall-script
if [ -s "$3" ]; then
    echo "shell(\"$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $3)\")" >>${fpath}
    #echo "shell('$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $3)')" >>${fpath}
fi
}

_requires ()
{
#
# Print on the standard output project dependencies (format: 'name' 'project' 'release')
# (
#  Atlas proj opt -> externals + CMT + AtlasSettings, AtlasLogin +
#                    proj dependencies opt +  Atlas proj noarch
#  Atlas proj src -> Atlas proj noarch
#  Atlas proj doc, noarch, debuginfo
#  Atlas proj dbg -> externals + CMT + AtlasSettings, AtlasLogin +
#                    proj dependencies dbg +  Atlas proj src
#  Gaudi the same as Atlas proj
# )
#
# Parameters: 'project-release-directory' 'project' 'release' 'platform'
#                                                             (bin src doc noarch debuginfo)
#                                                                  'container'
#                                                             (container-name [access-path])
#                                                                      ['exps-file'
#                                                                          ['binreq-file']]
#
[ $# -eq 5 -o $# -eq 6 -o $# -eq 7 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
#[ $# -eq 4 -o $# -eq 5 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local name cmtverfile cmtver
local -a proj

if [ $4 == bin ]; then

# dependencies on external software
#_extern $1 $2 $3 ${5:+"$5"} ||
_extern $1 $2 $3 "${5:-}" ${6:-} ||
warn "Cannot pack external software: $1 $2 $3" $FUNCNAME

# CMT dependency only
# first, try to determine which CMT version the project built with
for f_ in $1/${cntnr[1]}${cntnr[0]}/${verdir}$CMTCONFIG/${cntnr[0]}setup.make \
$1/${cntnr[1]}${cntnr[0]}/${verdir}$CMTCONFIG/${cntnr[0]}_setup.make \
$1/${cntnr[1]}${cntnr[0]}/${verdir}$CMTCONFIG/setup.make
do
  [ -f "$f_" ] && { cmtverfile=$f_; break; }
done

if [ -f "${cmtverfile-}" ]; then
    cmtver=$(awk '$1 == "use" && $2 == "CMT" { print $3; exit }' $cmtverfile) ||
    { warn "Cannot determine CMT version: $cmtverfile" $FUNCNAME; unset cmtver; }
elif f_=$1/${cntnr[1]}${cntnr[0]}/${verdir}cmt/setup.sh; [ -f $f_ ]; then
    cmtver=$(cd $(dirname $f_) && unset CMTROOT && . $f_ 2>/dev/null && cmt version) ||
    { warn "Cannot determine CMT version: $f_" $FUNCNAME; unset cmtver; }
fi

if [ -n "${cmtver-}" ]; then
    case $2 in
	tdaq-common|dqm-common)
	    if [ "${cmtver}" != $(cmt version) ]; then
		warn "Ignore CMT ${cmtver}: $1 $2 $3 (${CMTCONFIG})" $FUNCNAME
		cmtver=$(cmt version)
	    fi
	    ;;
    esac
    _extern_proj "" CMT $cmtver
else
    warn "Cannot determine CMT version: $1 $2 $3 (${CMTCONFIG})" $FUNCNAME
    # use currently setup CMT version
    _extern_proj "" CMT $(cmt version)
    # use the CMT version from ${cmthome}
#    _extern_proj $(cd ${cmthome}; cmt show uses | awk '
#$1 == "use" && $2 == "CMT" { gsub(/[()]/, "", $4); print $4, $2, $3; exit }
#')
fi

# dependencies on AtlasSettings, AtlasLogin
_settings || return 1

if [ -f $1/cmt/project.cmt ]; then
# direct project dependencies only
awk '$1 == "use" && $2 ~ /./ { print $2 }' $1/cmt/project.cmt |
while read projname; do
    proj=($(cd $1/cmt; cmt show projects | awk '
$1 == "'${projname}'" && $3 == "(in" { sub(/[)]$/, "", $4); print $1, $2, $4; exit }
'))
    case $recursive in
	no) 
	    name=$(_mkname ${proj[0]} ${proj[1]} bin)
	    echo $name ${proj[0]} ${proj[1]}
	    [ -f ${projcache}/cache/$name.pacman ] ||
	    warn "Not packed: ${proj[0]} ${proj[1]} ${CMTCONFIG}" $FUNCNAME ;;
	yes)
	    if grep "${proj[0]} ${proj[1]}" $tmpdone; then
		info "Done already: ${proj[0]} ${proj[1]} ${CMTCONFIG}" $FUNCNAME
	    else
		case ${proj[0]} in
		    LCGCMT)
			_extern_proj ${proj[2]} ${proj[0]} ${proj[1]} >>$tmpdone &&
			grep "${proj[0]} ${proj[1]}" $tmpdone ;;
		    *)
			_proj ${proj[2]} ${proj[0]} ${proj[1]} >>$tmpdone &&
			grep "${proj[0]} ${proj[1]}" $tmpdone ;;
		esac
	    fi ;;
	*) error "Invalid value: recursive: $recursive" $FUNCNAME; return 1 ;;
    esac
done || return 1

else warn "$1/cmt/project.cmt: No such file" $FUNCNAME
fi

# dependencies on other parts of the project
    case ${CMTCONFIG} in
	*dbg*) echo $(_mkname $2 $3 src) $2 $3 ;;
	*) echo $(_mkname $2 $3 noarch) $2 $3 ;;
    esac

elif [ $4 == src ]; then
    echo $(_mkname $2 $3 noarch) $2 $3

elif [ $4 == debuginfo ]; then
    if [ $# -eq 7 ]; then
awk '
{ if (NF == 4) print $4, $2, $3 }
' "$7" <&- || { error "awk" $FUNCNAME; return 1; }
    fi

fi
}

_extern ()
{
#
# Pack external software (Boost, Python, ROOT, ...)
# accessible via the glue packages in the current project directory ONLY
# Print on the standard output information on it (format: 'name' 'package' 'version')
#
# Parameters: 'project-release-directory' 'project' 'release' 'container'
#                                                              (container-name [access-path])
#                                                                      ['exps-file']
#
[ $# -eq 4 -o $# -eq 5 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
#[ $# -eq 3 -o $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
#[ $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local cmd
local tmpout=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

#if [ $# -eq 4 ] && [ "$4" ]; then
if [ $# -ge 4 ] && [ "$4" ]; then
    cntnr=($4); [ ${#cntnr[*]} -eq 2 ] && cntnr[1]=$(sed 's#/*$#/#' <<<${cntnr[1]})
else
    cntnr=($(_getcntnr $1 $2 $3)) || cntnr=($(_mkcntnr $2)) || return 1
fi

# determine version directory
# (<version-tag>/ - with_version_directory,  [empty] - without_version_directory)
# (CMT structure_strategy query not reliable)
if [ -d $1/${cntnr[1]}${cntnr[0]}/cmt ]; then
    verdir=
elif [ -d "$(echo $1/${cntnr[1]}${cntnr[0]}/*/cmt)" ]; then
    verdir=$(basename $(dirname $1/${cntnr[1]}${cntnr[0]}/*/cmt))/
    #verdir=*/
else
    error "Cannot determine cmt directory: $1/${cntnr[1]}${cntnr[0]}" $FUNCNAME; return 1
fi

cd $1/${cntnr[1]}${cntnr[0]}/${verdir}cmt || { error "cd" $FUNCNAME; return 1; }
info "`date`\tdirectory:\n`pwd`" $FUNCNAME

#    cmd="externcache=${externcache} patchdir=${patchdir} overwrite=${overwrite} tmppatch=${tmppatch} cmt -quiet broadcast ${selected:+-select='${selected}'} - ${scriptdir}/pack-extern.sh"
    cmd="externcache=${externcache} patchdir=${patchdir} overwrite=${overwrite} verify=$verify tmppatch=${tmppatch} cmt broadcast ${selected:+-select='${selected}'} - ${scriptdir}/pack-extern.sh"
    info "$cmd" $FUNCNAME
#    time eval $cmd | tee -a $tmpout >&2

#if [ $waiting == yes -a $quiet == no ]; then
    tail -f $tmpout >&2 & local pid=$!
#fi 

    time eval $cmd ${5:-} >>$tmpout; local retval=$?
#    time eval $cmd >>$tmpout; local retval=$?

#if [ $waiting == yes -a $quiet == no ]; then
    kill $pid
#else cat $tmpout >&2; fi

    [ $retval -eq 0 ] && ! sed '/^#/d' $tmpout && { error "sed" $FUNCNAME; return 1; }
#     [ $retval -eq 0 ] && cat $tmpout | grep -v '^#'

#info "`date`" $FUNCNAME
cd $OLDPWD || { error "cd" $FUNCNAME; return 1; }
rm -f $tmpout
return $retval
}

_extern_proj ()
{
#
# Pack external project (Gaudi, LCGCMT, CMT)
# Print on the standard output information on it (format: 'name' 'project' 'release')
#
# Parameters: 'project-release-directory' 'project' 'release' ['container']
#                                                              (container-name [access-path])
#
[ $# -eq 3 -o $# -eq 4 ] || { error "$#: Wrong number of arguments" $FUNCNAME; return 1; }
#[ $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

local name fpath

if [ $2 == Gaudi -o $2 == LCGCMT ]; then
#if [ $2 == Gaudi ]; then

    _proj $1 $2 $3 ${4:+"$4"}

elif [ $2 == LCGCMT ]; then
info "`date`\targuments:\n$*" $FUNCNAME

local tmpreq=$(mktemp -t tmp.XXXXXXXXXX) || return 1

local tmppre=$(mktemp -t tmp.XXXXXXXXXX) || return 1
local tmppost=$(mktemp -t tmp.XXXXXXXXXX) || return 1
local tmppreun=$(mktemp -t tmp.XXXXXXXXXX) || return 1

#local cmtver requires
local cmtver
local -a cntnr
local verdir

#if [ -s "$4" ]; then requires=$4; else requires=${tmpreq}; fi

#_settings >>${requires}

name=$(_mkname $2 $3 bin)
echo $name $2 $3
_requires ${1} ${2} ${3} bin "${4:-}" >|${tmpreq} || return 1
#_requires ${1} ${2} ${3} bin ${4:+"$4"} >|${tmpreq} || return 1
if [ -f ${projcache}/cache/$name.pacman -a "${upgrade}" != yes ]; then
    info "Existing ${projcache}/cache/$name.pacman will not be overwritten" $FUNCNAME
else
#    _requires ${1} ${2} ${3} bin ${4:+"$4"} >|${tmpreq}
    #_requires ${1} ${2} ${3} bin >>$requires
    # need assign the right value to cmtver
    cmtver=$(awk '$2 == "CMT" { print $3; exit }' ${tmpreq}) || return 1
    #cmtver=$(awk '$2 == "CMT" { print $3; exit }' $requires) || return 1
    _pre ${1} ${2} ${3} >|${tmppre} || return 1
    _post ${1} ${2} ${3} >|${tmppost} || return 1
    _pacman ${name} ${tmpreq} ${tmppost} ${tmppre} || return 1
    #_pacman ${name} $requires ${tmppost} ${tmppre}
fi
[ ! -f $PACKRPM/bin/pack-rpm-LCGCMT-bin.sh ] || . $PACKRPM/bin/pack-rpm-LCGCMT-bin.sh || return 1

name=$(_mkname $2 $3 noarch)
fpath=${projcache}/kits/$name.tar.gz
if [ -f $fpath -a "${upgrade}" != yes ]; then
    info "Existing $fpath will not be overwritten" $FUNCNAME
else
    info "Writing $fpath" $FUNCNAME
    tar -v ${tar_basic_opts} -cf $fpath -z \
	-X $scriptdir/junk.ptn \
	-X $scriptdir/pltfm-junk.ptn \
	--exclude=$(cd $1/${cntnr[1]}${cntnr[0]}/${verdir}cmt; \
	cmt -q show macro_value LCGCMT_installarea_prefix)/'*' \
	-C $1/../.. $2/$3 >&2 ||
{ warn "Disk space on the filesystem of `dirname $fpath`:
$(duf -p `dirname $fpath` $1)" $FUNCNAME
rm -f $fpath
error "tar" $FUNCNAME; return 1; }

    _patch ${2} ${3} ${name} || return 1
    _pacman $name || return 1
fi
[ ! -f $PACKRPM/bin/pack-rpm-LCGCMT-noarch.sh ] || . $PACKRPM/bin/pack-rpm-LCGCMT-noarch.sh || return 1

rm -f ${tmpreq} ${tmppost} ${tmppre} ${tmppreun} || return 0

elif [ $2 == CMT ]; then
: ${CMTCONFIG:?}
    # CMTv1r18p20050901Linux-i686.tar.gz from www.cmtsite.org
    name=${2}${3}${uname}-$(cut -d- -f1 <<<$CMTCONFIG)
#    name=${2}${3}Linux-$(cut -d- -f1 <<<$CMTCONFIG)

    echo $name $2 $3

    fpath=${projcache}/cache/$name.pacman
#    if [ -f $fpath ]; then
    if [ -f $fpath -a "${upgrade}" != yes ]; then
	info "Existing $fpath will not be overwritten" $FUNCNAME
    else
	info "Writing $fpath" $FUNCNAME

cat >|$fpath <<EOF
#
# CMT from a CMT cache
#
description ('Package $name')

package('${cmtcache}:CMT | version ("${3}")')
EOF

    fi

else
    error "Unknown external project: $1 $2 $3" $FUNCNAME; return 1
fi
}

_proj ()
{
#
# Pack project in five parts:
#                             binary platform - corresponds to CMTCONFIG value
#                             debuginfo files (for not debug build)
#                             source code
#                             documentation files
#                             other architecture independent files
# Print on the standard output information on it (format: 'name' 'project' 'release')
#
# Parameters: 'project-release-directory' 'project' 'release' ['container']
#                                                              (container-name [access-path])
#
### Parameters: 'project-release-directory' 'project' 'release' ['requires-file']
#
info "`date`\targuments:\n$*" $FUNCNAME
[ $# -eq 3 -o $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
local tmpbin=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmpsrc=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmpdoc=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmpnoarch=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

local tmpdirs=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmpdebuginfo=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

_prep_lists ${1} ${2} ${3} \
${tmpbin} ${tmpsrc} ${tmpdoc} ${tmpnoarch} \
${tmpdirs} ${tmpdebuginfo} || return 1

local tmpreq=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmpreqbin=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

if [ "${relocate:-}" = yes ]; then
    local tmpexps=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
    local tmpexterndebuginfo=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
fi

local tmppre=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmppost=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }
local tmppreun=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

#local pltfm name cmtver requires
local pltfm name cmtver
local -a cntnr
local verdir
local do_debuginfo

if [ "${debuginfo}" = yes ] && [ -n "${debuginfosuffix:-}" ] && [ "${mode:-}" != dbg ]; then
    do_debuginfo=yes
else
    unset do_debuginfo
fi

for pltfm in bin src doc noarch debuginfo; do
    [ ${pltfm} = doc ] && [ "${docfiles}" != yes ] && continue
    [ ${pltfm} = debuginfo ] && [ "${do_debuginfo:-}" != yes ] && continue

    name=$(_mkname ${2} ${3} ${pltfm}) || return 1

    if [ ${pltfm} = bin ]; then
	echo $name $2 $3
    fi

    if [ ${pltfm} != debuginfo ]; then
	_requires ${1} ${2} ${3} ${pltfm} "${4:-}" ${tmpexps:-} >|${tmpreq} || return 1
    else
	_requires ${1} ${2} ${3} ${pltfm} "${4:-}" "${tmpexps:-}" $tmpreqbin >|${tmpreq} || return 1
    fi
    if [ ${pltfm} = bin ] && [ "${do_debuginfo:-}" = yes ]; then
	cat $tmpreq >>$tmpreqbin || { error "cat" $FUNCNAME; return 1; }
    fi

#    _requires ${1} ${2} ${3} ${pltfm} ${4:+"$4"} >|${tmpreq} || return 1
#if [ -f "${tmpexps:-}" ]; then
#    cat ${tmpexps:-} >&2 || { error "cat" $FUNCNAME; return 1; }
#fi

#     if [ -f ${projcache}/kits/$name.tar.gz -a "${upgrade}" != yes ]; then
# 	info "Existing ${projcache}/kits/$name.tar.gz will not be overwritten" $FUNCNAME
#     else
    if [ ${pltfm} = debuginfo ] && [ -f "${tmpexps:-}" ]; then
	_prep_extern_debuginfo_list ${1} ${2} ${3} "${tmpexps:-}" ${tmpexterndebuginfo} || return 1
    fi

    if [ -f ${projcache}/cache/$name.pacman -a "${upgrade}" != yes ]; then
	info "Existing ${projcache}/cache/$name.pacman will not be overwritten" $FUNCNAME
	if [ "${verify}" == yes ]; then
	    local spath=${projcache}/kits/$name.tar.gz
	    if [ -f $spath ] && eval [ -s \${tmp${pltfm}} ]; then
		eval sed 's#^'$2/$3'#'$1'#' \${tmp${pltfm}} | newer -p $spath
	    elif [ -f $spath ] && eval [ ! -s \${tmp${pltfm}} ]; then
		warn "$spath: Empty file list" $FUNCNAME
	    elif [ ! -f $spath ] && eval [ -s \${tmp${pltfm}} ]; then
		error "$spath: No such file" $FUNCNAME
	    fi
	fi
    else
	_clean ${1} ${2} ${3} || return 1
	if [ ${pltfm} = bin ]; then
	    eval _copy -X ${tmpdebuginfo} ${1} ${2} ${3} \${tmp${pltfm}} || return 1
#	    eval _copy -x \\*${debuginfosuffix} ${1} ${2} ${3} \${tmp${pltfm}} || return 1
	elif [ ${pltfm} = debuginfo ]; then
	    eval _copy ${1} ${2} ${3} \${tmp${pltfm}} ${tmpexterndebuginfo} || return 1
	else
	    eval _copy ${1} ${2} ${3} \${tmp${pltfm}} || return 1
	fi
	if [ ${pltfm} == noarch ]; then
	    _install_dirs ${tmpdirs} || return 1
# 	    _copy -n ${1} ${2} ${3} ${tmpdirs} || return 1
	fi
	if [ ${pltfm} = debuginfo ] || [ "${mode:-}" = dbg ]; then
	    _symlinks -I ${1} ${2} ${3} ${tmpexps:-} || return 1
	else
	    _symlinks ${1} ${2} ${3} ${tmpexps:-} || return 1
	fi
	_tar ${2} ${3} ${name} || return 1
	if [ ${pltfm} == noarch ]; then
	    _patch ${2} ${3} ${name} || return 1
	fi

	if [ ${pltfm} == bin ]; then
            # need assign the right value to cmtver
	    cmtver=$(awk '$2 == "CMT" { print $3; exit }' ${tmpreq}) || { error "awk" $FUNCNAME; return 1; }
	    _pre ${1} ${2} ${3} >|${tmppre} || return 1
	    _post ${1} ${2} ${3} >|${tmppost} || return 1
	    _pacman ${name} ${tmpreq} ${tmppost} ${tmppre} || return 1
	else
	    _pacman ${name} ${tmpreq} || return 1
	fi
    fi
    [ ! -f $PACKRPM/bin/pack-rpm-proj.sh ] || . $PACKRPM/bin/pack-rpm-proj.sh || return 1
done
rm -f ${tmpbin} ${tmpsrc} ${tmpdoc} ${tmpnoarch} \
${tmpdirs} ${tmpdebuginfo} \
${tmpreq} ${tmpexps:-} ${tmppost} ${tmppre} ${tmppreun} \
${tmpreqbin} ${tmpexterndebuginfo} || return 0
}

##################################### main ##########################################
_mktmpdir $builddir || exit 1

if [ ${uname} = Linux ]; then
    tar_basic_opts="--owner=root --group=root --mode=u=rwX,go=rX --no-anchored --wildcards"
    noleaf=yes
#    group=yes
else
    tar_basic_opts=
    unset noleaf
#    unset group
fi
#tar_basic_opts="--owner=root ${group:+--group=root} --mode=u=rwX,go=rX --no-anchored --wildcards"
#tar_basic_opts="--owner=root --group=root --mode=u=rwX,go=rX --no-anchored --wildcards"
#tar_basic_opts="--owner=root --group=root --mode=u=rwX,go=rX"

tmpdone=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; exit 1; }

if [ "$selected" ]; then

    _extern ${projbase}/${project}/${release} ${project} ${release} "${container:-}" >>$tmpdone
    #_extern ${projbase}/${project}/${release} ${project} ${release} ${container:+"$container"} >>$tmpdone
    #_extern ${projbase}/${project}/${release} ${project} ${release} "${container}"

else

#_settings >>${requires}
if [ $project == AtlasRelease ]; then
    #_settings >>${requires}
    _proj ${projbase}/${release} ${project} ${release} ${requires}
#elif [ $project == LCGCMT ]; then
elif [ $project == LCGCMT -o $project == CMT ]; then
    _extern_proj ${projbase}/${project}/${release} ${project} ${release} ${container:+"$container"} >>$tmpdone
else
    _proj ${projbase}/${project}/${release} ${project} ${release} ${container:+"$container"} >>$tmpdone
    #_proj ${projbase}/${project}/${release} ${project} ${release} ${requires}
fi

fi

retval=$?
[ $retval -eq 0 ] && [ -s "$tmpdone" ] && cat $tmpdone
# [ -s "$tmpdone" ] && cat $tmpdone >&2
[ -f "$tmpdone" ] && rm -rf $tmpdone

rm -rf $tmpdir
###rm -rf $requires
exit $retval
#####################################################################################
