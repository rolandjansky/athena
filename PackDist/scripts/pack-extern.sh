#!/bin/bash
# File: pack-extern.sh
#
# Pack external software interfaced to by a cmt package.
#
# Author: Christian Arnault <arnault@lal.in2p3.fr>
# Author: Grigori Rybkine <Grigori.Rybkine@cern.ch>.
#

[ "$PACKEXTERNDBG" ] && set -x

: ${externcache:?} ${patchdir:?} ${overwrite:?}

: ${scriptdir:?}
. $scriptdir/pack-common.sh

: ${CMTCONFIG:?}
: ${SITEROOT:?}
#: ${tmpdir:?}
#: ${tmppatch:?}

##################################### functions #####################################
_patch ()
{
#
# Parameters: 'project' 'release' 'package'
# Create a patched requirements file, install it
# in the right place in the directory tree under tmppatch and
# print its path on the standard output: 'patch-path'
#
# Return status: 0 if a patched file is created, 1 otherwise
#
[ -d "${patchdir}" ] ||
{ warn "patchdir=$patchdir: No such directory" $FUNCNAME; return 1; }

# Check for patches
if [ -f ${patchdir}/${1}/${2}/requirements -o \
     -f ${patchdir}/${1}/${2}/${3}/requirements ]; then

local projpatch=${patchdir}/${1}/${2}/requirements pkgpatch=${patchdir}/${1}/${2}/${3}/requirements
local reqpatch=$(mktemp -d -t tmp.XXXXXXXXXX)/requirements || { error "mktemp" $FUNCNAME; return 1; }
>|${reqpatch}

if [ -f $pkgpatch ]; then
    if head -n 1 $pkgpatch | grep -q '^#!'; then
	$pkgpatch <requirements >>${reqpatch}
    else
	cat requirements $pkgpatch >>${reqpatch}
    fi
    [ $? -eq 0 ] || { error "Cannot apply: $pkgpatch" $FUNCNAME; }
fi

if [ -f $projpatch ]; then
#     echo >>${reqpatch}
    cat $projpatch >>${reqpatch} || { error "Cannot apply: $projpatch" $FUNCNAME; }
fi

[ -s ${reqpatch} ] || { rm -rf $(dirname $reqpatch)
warn "Empty patch(es) provided: $1 $2 $3" $FUNCNAME; return 1; }

# if _install_patched $1 $2 $3 ${reqpatch}; then
#     rm -rf $(dirname $reqpatch) || return 0
# else echo ${reqpatch}
# fi
_install_patched $1 $2 $3 ${reqpatch} >/dev/null
echo ${reqpatch}

else return 1
fi
}

_install_patched ()
{
#
# Parameters: 'project' 'release' 'package' 'patched-path'
# Install patched requirements file 'patched-path'
# in the right place in the directory tree under tmppatch and
# print its path on the standard output: 'installed-path'
#
[ $# -eq 4 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
[ -d "${tmppatch}" ] ||
{ error "tmppatch=$tmppatch: No such directory" $FUNCNAME; return 1; }
local reqdir=$(pwd | sed -n 's#^.*\('$1/$2'\)#\1#p')
[ "$reqdir" ] || { error "$(pwd): $1 $2 $3" $FUNCNAME; return 1; }
mkdir -p ${tmppatch}/$reqdir || { error "mkdir" $FUNCNAME; return 1; }
info "Writing patched requirements: $1 $2 $3" $FUNCNAME
[ "$PACKEXTERNDBG" ] && cat $4 >&2
cp -p $4 ${tmppatch}/$reqdir/requirements ||
{ error "cp" $FUNCNAME; return 1; }
echo ${tmppatch}/$reqdir/requirements
}

_mkname ()
{
#
# Parameters: 'package' 'version' ['platform']
#                                 (bin noarch debuginfo)
#
[ $# -eq 2 -o $# -eq 3 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

if [ $# -eq 2 -o "$3" == bin ]; then
    sed -e 's/[-.]/_/g' -e "s/${1}_${1}/${1}/" <<<${1}_${2}_${CMTCONFIG} || { error "sed" $FUNCNAME; return 1; }
elif [ $3 = debuginfo ]; then
    sed -e 's/[-.]/_/g' -e "s/${1}_${1}/${1}/" <<<${1}_${2}_${CMTCONFIG}_debuginfo || { error "sed" $FUNCNAME; return 1; }
else
    sed -e 's/[-.]/_/g' -e "s/${1}_${1}/${1}/" <<<${1}_${2}_${3} || { error "sed" $FUNCNAME; return 1; }
fi
}

_bad_symlinks ()
{
#
# Parameters: [FILE...]
# Print on the standard output broken symlinks paths (format: 'path:' 'error message')
# The error message format is different at least with find (GNU findutils) 4.4.2 (SLC6),
# e.g,
# find: File system loop detected; `dst/l_cur' is part of the same file system loop as `dst'.

# better not to rely on it
#
find "$@" -follow 2>&1 >/dev/null | sed 's/^find:[ ]*//'
# -L option is not supported by GNU find version 4.1.20 (SL4)
# but supported by GNU find version 4.2.27 (SL5) and find on Mac OS X 10.5, 10.6
# -follow supported by all (is deprecated on Mac OS X 10.5)
#find -L "$@" 2>&1 >/dev/null | sed 's/^find:[ ]*//'
#
#find "$@" -follow -fprint /dev/null 2>&1 | sed 's/^find:[ ]*//'
#find $* -follow -fprint /dev/null 2>&1 | sed 's/^find:[ ]*//'
# 	    [ -d $_s ] && symlinks -rv $_s | grep ^dangling >|$tmperr
# 	    if [ -s $tmperr ]; then
# 		[ "$PACKEXTERNDBG" ] && cat $tmperr
# 		{ error "Symlinks below cannot be followed:" $FUNCNAME; cat $tmperr 1>&2; }
#                 # does not work - bug in symlinks
# 		#symlinks -rv $_d | grep ^dangling | cut -d' ' -f2 >|$tmperr
# 		find $_d/ -noleaf | xargs file | grep broken | cut -d: -f1 >|$tmperr
# 		[ "$PACKEXTERNDBG" ] && cat $tmperr
# 	    fi
}

_chk_links ()
{
[ -d "$1" ] || return 0
local tmperr=$(mktemp -t tmp.XXXXXXXXXX) || return 1

type symlinks >/dev/null || { warn "type" $FUNCNAME; return 0; }
symlinks -rv $1 |
    awk -v tmperr=$tmperr '
    /^absolute|^other_fs|^dangling|^messy/ { print >tmperr; next }
    { print }
' |
while read line; do

grep -q $(abspath $1) <<<"$(
    abspath $(
    awk '
    {
        if (substr($4, 1, 1) == "/") print $4
        else { n = split($2, a, "/")
        print substr($2, 1, length($2) - length(a[n])) $4 }
    }
    ' <<<$line
    )
)" || echo $line >>$tmperr

done

[ -s $tmperr ] && { warn "Symlinks will not be followed:" $FUNCNAME; cat $tmperr >&2; }

rm -f $tmperr || return 0
}

_tar_extern ()
{
#
# Parameters: 'name'
#
[ $# -eq 1 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

local n=${#src[*]}
[ $n -gt 0 ] || return 0
if [ ${uname} = Linux ]; then
    local tar_basic_opts="-v --owner=root --group=root --mode=u=rwX,go=rX --no-anchored --wildcards"
    noleaf=yes
#    group=yes
else
    local tar_basic_opts="-v"
    unset noleaf
#    unset group
fi

local tar_cmd=tar
#local tar_basic_opts="-v --owner=root ${group:+--group=root} --mode=u=rwX,go=rX --no-anchored --wildcards"
#local tar_basic_opts="-v --owner=root --group=root --mode=u=rwX,go=rX --no-anchored --wildcards"
#local tar_basic_opts="-v --owner=root --group=root --mode=u=rwX,go=rX"

local fpath=${externcache}/kits/${1}.tar

# create an empty archive to append to
${tar_cmd} ${tar_basic_opts} -cf ${fpath} -T /dev/null 1>&2 ||
{ error "${tar_cmd}" $FUNCNAME; return 1; }

local empty_sum=$(sha1sum ${fpath}) || { error "sha1sum" $FUNCNAME; return 1; }

cd ${tempcopydir} || { error "cd" $FUNCNAME; return 1; }
#local n=${#src[*]}

[ "${mode:-}" != dbg ] || unset debuginfo_suffix

for (( i=0 ; i!=n ; i++ )) ; do
    #s=$(echo ${src[$i]} | sed 's#/\+$##')
    local s=$(sed 's#/\+$##' <<<${src[$i]}) || { error "sed" $FUNCNAME; return 1; }
    local d=$(sed 's#/\+$##' <<<${dst[$i]}) || { error "sed" $FUNCNAME; return 1; }

	rm -rf ${tempcopydir}/*
 	[ -f $s -o -d $s ] || { error "$(stat -L $s 2>&1)" $FUNCNAME; continue; }

 	if [ "${follow_symlinks}" ]; then
	    warn "Following symlinks to pack '$s' as '$d'" $FUNCNAME
	    mkdir -p $(dirname $d) || { error "mkdir" $FUNCNAME; continue; }
	    ln -sf $s $d || { error "ln" $FUNCNAME; continue; }

	    local tmperr=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; continue; }
	    _bad_symlinks $s >|$tmperr
	    if [ -s $tmperr ]; then
		{ error "Symlinks cannot be followed:" $FUNCNAME; cat $tmperr 1>&2; }
		_bad_symlinks $d | cut -d: -f1 >|$tmperr
		[ "$PACKEXTERNDBG" ] && cat $tmperr 1>&2
	    fi

	    ${tar_cmd} ${tar_basic_opts} -hrf ${fpath} \
		-X $tmperr \
		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
		${debuginfo_suffix:+--exclude='*'${debuginfo_suffix}} \
		$d 1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }

	    rm -f $tmperr
 	elif [ . == $(dirname $d) ]; then
	    [ -f $s ] && \
	    { error "Cowardly refusing to pack '$s' as '$d' - no top directory" $FUNCNAME; continue; }
	    ln -sf $s $d || { error "ln" $FUNCNAME; continue; }
	    ${tar_cmd} ${tar_basic_opts} --no-recursion -hrf ${fpath} \
		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
		${debuginfo_suffix:+--exclude='*'${debuginfo_suffix}} \
		$d 1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }
	    find $d/ ${noleaf:+-noleaf} -mindepth 1 -maxdepth 1 | \
		${tar_cmd} ${tar_basic_opts} -rf ${fpath} -T - \
		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
		${debuginfo_suffix:+--exclude='*'${debuginfo_suffix}} \
		1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }
# 	    find $d/ ${noleaf:+-noleaf} -mindepth 1 -maxdepth 1 | xargs \
# 		${tar_cmd} ${tar_basic_opts} -rf ${fpath} \
# 		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
# 		1>&2 # || { error "${tar_cmd}" $FUNCNAME; return 1; }
# 	    case $? in
# 		123|124) error "${tar_cmd}" $FUNCNAME; continue ;;
# 		125|126|127|1) error "xargs" $FUNCNAME; continue ;;
# 	    esac
	    [ ${uname} != Linux ] || _chk_links $s
 	elif [ -f $s ]; then
	    mkdir -p $(dirname $(dirname $d)) || { error "mkdir" $FUNCNAME; continue; }
	    ln -sf $(dirname $s) $(dirname $d) || { error "ln" $FUNCNAME; continue; }
	    ${tar_cmd} ${tar_basic_opts} -rf ${fpath} \
		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
		${debuginfo_suffix:+--exclude='*'${debuginfo_suffix}} \
		$d 1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }

	    [ -h $s ] &&
	    warn "Symlink $s -> $(readlink $s) packed as '$d'" $FUNCNAME
 	else
#	    [ -h $s ] &&
            if [ -h $s ]; then
		warn "Following symlink $s -> $(readlink $s) to pack as '$d'" $FUNCNAME
		follow=yes
	    else
		unset follow
	    fi
 	    mkdir -p $(dirname $d) || { error "mkdir" $FUNCNAME; continue; }
 	    ln -sf $s $d || { error "ln" $FUNCNAME; continue; }
	    ${tar_cmd} ${tar_basic_opts} --no-recursion -hrf ${fpath} \
		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
		${debuginfo_suffix:+--exclude='*'${debuginfo_suffix}} \
		$d 1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }
	    find $d/ ${noleaf:+-noleaf} ${follow:+-follow} -mindepth 1 -maxdepth 1 | \
		${tar_cmd} ${tar_basic_opts} -rf ${fpath} -T - \
		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
		${debuginfo_suffix:+--exclude='*'${debuginfo_suffix}} \
		1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }
# 	    find $d/ ${noleaf:+-noleaf} -mindepth 1 -maxdepth 1 | xargs \
# 		${tar_cmd} ${tar_basic_opts} -rf ${fpath} \
# 		-X $scriptdir/extern-junk.ptn ${exclude_objfiles:+--exclude='*.o'} \
# 		1>&2 #|| { error "${tar_cmd}" $FUNCNAME; return 1; }
# 	    case $? in
# 		123|124) error "${tar_cmd}" $FUNCNAME; continue ;;
# 		125|126|127|1) error "xargs" $FUNCNAME; continue ;;
# 	    esac
	    [ ${uname} != Linux ] || _chk_links $s
#	    _chk_links $s
 	fi

done
cd $OLDPWD

local payload_sum=$(sha1sum ${fpath}) || { error "sha1sum" $FUNCNAME; return 1; }
if [ "$empty_sum" = "$payload_sum" ]; then
    info "Nothing to include: $fpath" $FUNCNAME
    rm -f ${fpath} || { error "rm" $FUNCNAME; return 1; }
    return 0
fi

gzip -vf ${fpath} >&2 || {
rm -f ${fpath}
error "gzip" $FUNCNAME; return 1; }
}

_tar_extern_debuginfo ()
{
#
# Parameters: 'name'
#
[ $# -eq 1 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }

[ -n "${debuginfo_suffix-}" ] || return 0

local n=${#src[*]}
[ $n -gt 0 ] || return 0
if [ ${uname} = Linux ]; then
    local tar_basic_opts="-v --owner=root --group=root --mode=u=rwX,go=rX --no-anchored --wildcards"
    noleaf=yes
#    group=yes
else
    local tar_basic_opts="-v"
    unset noleaf
#    unset group
fi

local tar_cmd=tar
#local tar_basic_opts="-v --owner=root ${group:+--group=root} --mode=u=rwX,go=rX --no-anchored --wildcards"
#local tar_basic_opts="-v --owner=root --group=root --mode=u=rwX,go=rX --no-anchored --wildcards"
#local tar_basic_opts="-v --owner=root --group=root --mode=u=rwX,go=rX"

local fpath=${externcache}/kits/${1}.tar

# create an empty archive to append to
${tar_cmd} ${tar_basic_opts} -cf ${fpath} -T /dev/null 1>&2 ||
{ error "${tar_cmd}" $FUNCNAME; return 1; }

local empty_sum=$(sha1sum ${fpath}) || { error "sha1sum" $FUNCNAME; return 1; }

cd ${tempcopydir} || { error "cd" $FUNCNAME; return 1; }
#local n=${#src[*]}

rm -rf ${tempcopydir}/*
for (( i=0 ; i!=n ; i++ )) ; do
    #s=$(echo ${src[$i]} | sed 's#/\+$##')
    local s=$(sed 's#/\+$##' <<<${src[$i]}) || { error "sed" $FUNCNAME; return 1; }
    local d=$(sed 's#/\+$##' <<<${dst[$i]}) || { error "sed" $FUNCNAME; return 1; }

 	[ -f $s -o -d $s ] || { error "$(stat -L $s 2>&1)" $FUNCNAME; continue; }

 	if [ "${follow_symlinks}" ]; then
	    warn "Following symlinks to pack '$s' as '$d'" $FUNCNAME
	    mkdir -p $(dirname $d) || { error "mkdir" $FUNCNAME; continue; }
	    ln -sfn $s $d || { error "ln" $FUNCNAME; continue; }

	    local tmperr=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; continue; }
	    _bad_symlinks $s >|$tmperr
	    if [ -s $tmperr ]; then
		{ error "Symlinks cannot be followed:" $FUNCNAME; cat $tmperr 1>&2; }
	    fi

	    local tmplist=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; continue; }
	    find -L $d ${noleaf:+-noleaf} -name "*$debuginfo_suffix" -fprint $tmplist 2>/dev/null
	    ${tar_cmd} ${tar_basic_opts} -hrf ${fpath} -T $tmplist 1>&2 ||
	    { error "${tar_cmd}" $FUNCNAME; continue; }

	    if [ "$PACKEXTERNDBG" ]; then
		echo "extern_debuginfo:" >&2
		cat $tmplist >&2
	    fi
	    rm -f $tmperr $tmplist
 	elif [ -f $s ]; then
	    [[ $d == *$debuginfo_suffix ]] || continue
 	    [ . != $(dirname $d) ] ||
	    { error "Cowardly refusing to pack '$s' as '$d' - no top directory" $FUNCNAME; continue; }

	    mkdir -p $(dirname $(dirname $d)) || { error "mkdir" $FUNCNAME; continue; }
	    ln -sfn $(dirname $s) $(dirname $d) || { error "ln" $FUNCNAME; continue; }
	    ${tar_cmd} ${tar_basic_opts} -rf ${fpath} \
		$d 1>&2 || { error "${tar_cmd}" $FUNCNAME; continue; }

	    [ -h $s ] &&
	    warn "Symlink $s -> $(readlink $s) packed as '$d'" $FUNCNAME
 	else
            if [ -h $s ]; then
		warn "Following symlink $s -> $(readlink $s) to pack as '$d'" $FUNCNAME
	    fi
 	    mkdir -p $(dirname $d) || { error "mkdir" $FUNCNAME; continue; }
 	    ln -sfn $s $d || { error "ln" $FUNCNAME; continue; }

	    local tmplist=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; continue; }
	    find -H $d ${noleaf:+-noleaf} -name "*$debuginfo_suffix" -fprint $tmplist ||
	    { error "find" $FUNCNAME; continue; }
	    ${tar_cmd} ${tar_basic_opts} -rf ${fpath} -T $tmplist 1>&2 ||
	    { error "${tar_cmd}" $FUNCNAME; continue; }

	    if [ "$PACKEXTERNDBG" ]; then
		echo "extern_debuginfo:" >&2
		cat $tmplist >&2
	    fi
	    rm -f $tmplist
 	fi

done
cd $OLDPWD

local payload_sum=$(sha1sum ${fpath}) || { error "sha1sum" $FUNCNAME; return 1; }
if [ "$empty_sum" = "$payload_sum" ]; then
    info "Nothing to include: $fpath" $FUNCNAME
    rm -f ${fpath} || { error "rm" $FUNCNAME; return 1; }
    return 0
fi

gzip -vf ${fpath} >&2 || {
rm -f ${fpath}
error "gzip" $FUNCNAME; return 1; }
}

_getver ()
{
#
# Determine package version and print on the standard output
# (empty if cannot be determined )
#
# Parameters: 'name'
#
local fpath=${externcache}/cache/${1}.pacman

if [ -f $fpath ]; then
sed -e '/^[ \t]*version(.*)[ \t]*$/!d' -e 's/version(//' -e 's/)[ \t]*$//' -e "s/['\"]//g" ${fpath} || { error "sed" $FUNCNAME; return 1; }
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

fpath=${externcache}/cache/${1}.pacman

if [ "$5" ]; then
    ver=$5
else
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

description('External Package ${1}')
version('${ver}')
${reqs}

EOF

# add preinstall-script
if [ -s "$4" ]; then
    echo "shell('$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $4)')" >>${fpath}
fi

if [ -f ${externcache}/kits/${1}.tar.gz ]; then

cat <<EOF >>${fpath}

downloadUntar('../kits/${1}.tar.gz')

EOF
else
    warn "${externcache}/kits/${1}.tar.gz: No such file" $FUNCNAME
fi

# add postinstall-script
if [ -s "$3" ]; then
    echo "shell(\"$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $3)\")" >>${fpath}
    #echo "shell('$(sed s/@INSTALL_PREFIX@/\$PACMAN_INSTALLATION/g $3)')" >>${fpath}
fi
}

_add_opt_dep ()
{
#
# for external dbg package
# add dependency on opt mode package
# as the former contains dbg mode ONLY
# (to aviod files conflicts with the latter)
#
# Parameters: 'name' ['requires-file']
#
    local name_opt=$(sed "s/dbg/opt/" <<<${1}) || { error "sed" $FUNCNAME; return 1; }
    if [ ${1} != "${name_opt}" ]; then
	if [ -f "${2:-}" ] && ! grep -q "${name_opt}" ${2}; then
	    echo "${name_opt}" >>${2} || { error "echo" $FUNCNAME; return 1; }
	elif [ ! -f "${2:-}" ]; then
	    echo "${name_opt}"
	fi
	[ -f ${externcache}/cache/${name_opt}.pacman ] &&
	[ -f ${externcache}/kits/${name_opt}.tar.gz ] ||
	warn "Not packed: ${name_opt}" $FUNCNAME
    fi
}

##################################### main ##########################################
#info "$(date)" $FUNCNAME

package=`cmt show macro_value package`
project=`cmt show macro_value ${package}_project`
release=`cmt show macro_value ${package}_project_release`

if reqpatch=$(_patch ${project} ${release} ${package}); then
    export CMTUSERCONTEXT=$(dirname $reqpatch)
else unset reqpatch
fi

# Check if there are external packages required
for pkg in `cmt -q -tag_add=ATLAS show macro_value ${package}_requires`; do
    [ -f "${tmpreq:-}" ] || tmpreq=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; exit 1; }
    echo "$pkg" >>$tmpreq || { error "echo" $FUNCNAME; exit 1; }
done

# Check if there is external software to pack
export_paths=`cmt -q -tag_add=ATLAS show macro_value ${package}_export_paths`
# Remove leading and trailing whitespace (to make the value empty if it was whitespace)
src=(${export_paths})

[ ${#src[*]} -gt 0 ] || [ -s "${tmpreq:-}" ] || exit 0
#[ "$export_paths" ] || [ -s "$tmpreq" ] || exit 0

export_paths=${src[*]}

if [ -n "$export_paths" ]; then

dst=($(cmt -q -tag_add=ATLAS,STANDALONE show macro_value ${package}_export_paths))
dst=(${dst[*]#${SITEROOT}/})

info "source:\n'${src[*]}'" $FUNCNAME
info "destination:\n'${dst[*]}'" $FUNCNAME

[ ${#src[*]} -eq ${#dst[*]} ] ||
{ error "source # ${#src[*]}, destination # ${#dst[*]}" $FUNCNAME; exit 1; }

debuginfo_suffix=$(cmt -q -tag_add=PACK,ATLAS show macro_value ${package}_debuginfo_suffix)

if [ $# -eq 1 ] && [ -f "${1}" ]; then
    n=${#src[*]}
    for (( i=0 ; i!=n ; i++ )) ; do
	printf '%s\0%s\0%s\n' "${src[$i]}" "${dst[$i]}" "${debuginfo_suffix}" >>"${1}"
#	echo ${src[$i]} ${dst[$i]} >>"${1}"
    done
fi

fi

if [ -s "${tmpreq:-}" ]; then
info "requires:\n'$(<$tmpreq)'" $FUNCNAME
fi

native_version=`cmt -q -tag_add=PACK show macro_value ${package}_native_version`
if [ -n "$native_version" ]; then
    if [ LCG_Interfaces = "$(cmt -q show macro_value ${package}_offset)" ] || \
	cmt -q show uses|grep -q -F -w LCG_Interfaces; then
	LCG_config_version=$(cmt -q show macro_value LCG_config_version)
	_LCG_config_version=$(echo ${LCG_config_version}|sed -n 's/^\([[:digit:]]\{1,\}\).*/\1/p')
	if [ -n "${_LCG_config_version}" ] && [ ${_LCG_config_version} -ge 68 ] && \
	    [ -n "$export_paths" ] && echo "${dst[*]}"|grep -q -F "/LCG_${LCG_config_version}/" && \
	    ! echo "$native_version"|grep -q -F "_lcgcmt${LCG_config_version}"; then
	    version=${native_version}_lcgcmt${LCG_config_version}
	fi
    fi
    [ -z ${version+t} ] && version=$native_version
else
version=`cmt show macro_value version`
[ "${version}" = "v*" ] &&
{ warn "Invalid CMT package version: $version: $project $release $package" $FUNCNAME;
version=v1; }
    warn "Macro ${package}_native_version unspecified: $project $release $package $version" $FUNCNAME
fi

platform=`cmt -q -tag_add=PACK show macro_value ${package}_platform`

#
# Need to handle dbg mode
#
if [ -n "$export_paths" ] && [[ ${CMTCONFIG} == *dbg* ]] && [[ "${dst[*]}" != *dbg* ]] && [ -z $platform ]; then
#if [[ ${CMTCONFIG} == *dbg* ]] && [[ "${export_paths}" != *dbg* ]] && [ -z $platform ]; then
    info "No debug build available, using optimised" $FUNCNAME
    platform=$(sed 's/dbg/opt/' <<<${CMTCONFIG}) ||
    { error "sed" $FUNCNAME; exit 1; }
fi

if [ -n "$export_paths" ] && [[ ${CMTCONFIG} == *dbg* ]] && [ -z ${platform} ]; then
    n=${#src[*]}
    for (( i=0 ; i!=n ; i++ )) ; do
	if [[ ${dst[$i]} != *dbg* ]]; then
            #
            # Pack dbg mode ONLY into external dbg tarball
            #
	    [ -f "${tmpreq:-}" ] || tmpreq=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; exit 1; }
	    _add_opt_dep $(_mkname $package $version $platform) ${tmpreq} || exit 1
	    # _add_opt_dep ${name} ${tmpreq} || exit 1
	    info "Skipped non-debug path: ${dst[$i]}" $FUNCNAME
	    unset src[$i] dst[$i]
	fi
    done
    # After possibly unsetting some array member(s) with path(s) to skip
    # still want array members to be indexed contiguously
    src=(${src[*]})
    dst=(${dst[*]})
fi

follow_symlinks=`cmt -q -tag_add=PACK show macro_value ${package}_follow_symlinks`
include_objfiles=`cmt -q -tag_add=PACK show macro_value ${package}_include_objfiles`
if [ -n "$include_objfiles" ]; then
    unset exclude_objfiles
else
    exclude_objfiles=yes
fi
if [ -n "$export_paths" ] &&
    [ "${debuginfo}" = yes ] && [ -n "${debuginfo_suffix}" ] && [[ ${CMTCONFIG} != *dbg* ]]; then
    do_debuginfo=yes
else
    unset do_debuginfo
fi

if [ "${do_debuginfo:-}" = yes ]; then
# name=`_mkname $package $version $platform`
    if [ -n "${platform:-}" ]; then
	platform_debuginfo=${platform}_debuginfo
	[ "${platform}" != noarch ] ||
	warn "${package}_platform set to noarch and ${package}_debuginfo_suffix to non-empty" $FUNCNAME
    fi
    echo "$(_mkname $package $version $platform)" "$package" "$version" "$(_mkname $package $version ${platform_debuginfo:-${CMTCONFIG}_debuginfo})" || exit 1
    # echo "$(_mkname $package $version $platform)" "$package" "$version" "$(_mkname $package $version ${platform_debuginfo:-debuginfo})" || exit 1
else
    echo "$(_mkname $package $version $platform)" "$package" "$version" || exit 1
fi

for pltfm in bin debuginfo; do
    [ ${pltfm} = debuginfo ] && [ "${do_debuginfo:-}" != yes ] && continue

    if [ ${pltfm} = bin ]; then
	name=$(_mkname $package $version $platform) || exit 1
    elif [ ${pltfm} = debuginfo ]; then
	name=$(_mkname $package $version ${platform_debuginfo:-${CMTCONFIG}_debuginfo}) || exit 1
	# name=$(_mkname $package $version ${platform_debuginfo:-debuginfo}) || exit 1
    fi

    fpath=${externcache}/cache/$name.pacman
    if [ -f $fpath -a "${overwrite}" != yes ]; then
	
	info "Existing $fpath will not be overwritten" $FUNCNAME
	
	if [ "${verify}" = yes ]; then
	    spath=${externcache}/kits/$name.tar.gz
	    if [ -f $spath -a -n "${src[*]}" ]; then
		newer ${follow_symlinks:+-L} -p $spath ${src[*]}
	    elif [ -f $spath -a -z "${src[*]}" ]; then
		warn "$spath: No 'export_paths' to verify" $FUNCNAME
	    elif [ ! -f $spath -a -n "${src[*]}" ]; then
		error "$spath: No such file" $FUNCNAME
	    fi
	fi
	
    else

	tempcopydir=$(mktemp -d -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; exit 1; }
	if [ ${pltfm} = bin ]; then
	    _tar_extern $name || { \rm -rf $tempcopydir; exit 1; }
	elif [ ${pltfm} = debuginfo ]; then
	    _tar_extern_debuginfo ${name} || { \rm -rf $tempcopydir; exit 1; }
	fi
	_pacman ${name} ${tmpreq:-} || { \rm -rf $tempcopydir; exit 1; }
	\rm -rf $tempcopydir

    fi
    [ ! -f $PACKRPM/bin/pack-rpm-extern.sh ] || . $PACKRPM/bin/pack-rpm-extern.sh || exit 1
done
