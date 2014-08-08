#!/bin/sh

msg="Usage: `basename $0` [OPTION]... SOURCE DEST
  or: `basename $0` [OPTION]... -t PATH...
In first form, relocate the ATLAS distribution kit from SOURCE to DEST.
In second form, locate the ATLAS distribution kit in PATH and transform
as described for -t option (implies -i option).

  -i  edit files in-place, without moving them from SOURCE to DEST
  -c  only relocate files in cmtsite
  -t  transform setup scripts so that full paths in them may be changed by setting ATLAS_RELEASE_BASE environment variable
  -h  display this help and exit
  -v  output version information and exit

Report bugs to <Grigori.Rybkine@cern.ch>."

ver="`basename $0` (PackDist) 0.7.0
Written by Grigori Rybkine."

unset _inplace _config _transform

while getopts "icthv" opt; do
  case $opt in
    i) _inplace=y ;;
    c) _config=y ;;
    t) _transform=y ;;
    h) echo "$msg"; exit 0 ;;
    v) echo "$ver"; exit 0 ;;
    \?) echo "$msg"; exit 1 ;;
  esac
done

shift $(($OPTIND - 1))

transform ()
{
    p=$(find "$1" -path '*/cmtsite/requirements' -print -quit) || return
    [ -f "$p" ] || { echo >&2 "Cannot find release in $1"; return 1; }
    siteroot=$(awk '$1 == "set" && $2 == "SITEROOT" {print $3; exit}' $p) || return
    [ -d "$siteroot" ] || { echo >&2 "Cannot determine SITEROOT from $p"; return 1; }
    resiteroot=$(echo "${siteroot}"|awk '{gsub("/", "\\/");print}') || return

    for suffix in .sh .csh; do
if [ ${suffix} = .csh ]; then
sed_script='/^[^#]*'"${resiteroot}"'/{
i\
if (! ${?ATLAS_RELEASE_BASE}) then
p
i\
else
s/'"${resiteroot}"'/${ATLAS_RELEASE_BASE}/g
a\
endif
}'
elif [ ${suffix} = .sh ]; then
sed_script='/^[^#]*'"${resiteroot}"'/{
s/'"${resiteroot}"'/${ATLAS_RELEASE_BASE:-'"${resiteroot}"'}/g
}'
fi

find "${siteroot}"/.cmtsite "${siteroot}"/cmtsite -noleaf -type f \( -name setup${suffix} -o -name cleanup${suffix} -o -name asetup${suffix} \) \! -exec grep -F -q ATLAS_RELEASE_BASE '{}' \; -exec sed -i "${sed_script}" '{}' \+ || retval=1

[ "${_config:-}" != y ] || continue

find "${siteroot}"/CMT -noleaf -type f \( -path \*/mgr/setup${suffix} -o -path \*/mgr/cleanup${suffix} \) \! -exec grep -F -q ATLAS_RELEASE_BASE '{}' \; -exec sed -i "${sed_script}" '{}' \+ || retval=1

find "${siteroot}" -noleaf -type f \( -path \*/cmt/setup${suffix} -o -path \*/cmt/cleanup${suffix} \) \! -exec grep -F -q ATLAS_RELEASE_BASE '{}' \; -exec sed -i "${sed_script}" '{}' \+ || retval=1

# optimised environment setup - setup scripts
find "${siteroot}" -noleaf -type f \( -path \*/standalone-setup${suffix} -o -path \*/standalone-init${suffix} \) \! -exec grep -F -q ATLAS_RELEASE_BASE '{}' \; -exec sed -i "${sed_script}" '{}' \+ || retval=1
done

# optimised environment setup - requirements file
sed_script='/^[^#]*'"${resiteroot}"'/{
s/'"${resiteroot}"'/${SITEROOT}/g
}'
find "${siteroot}" -noleaf -type f -name requirements -exec grep -q '^# CMTFLAGS:' '{}' \; -exec grep -F -q "${siteroot}" '{}' \; -exec sed -i "${sed_script}" '{}' \+ || retval=1
    return ${retval}
}

retval=0
if [ "${_transform-}" = y ]; then
    [ $# -ge 1 ] || set $PWD || exit
    for p; do
	transform "$p" || retval=1
    done
    exit ${retval}
fi

[ $# -eq 2 ] || { echo "$msg"; exit 1; }

src=$(echo $1|sed -e 's#/\{1,\}#/#g' -e 's#/*$##')
dst=$(echo $2|sed -e 's#/\{1,\}#/#g' -e 's#/*$##')

if [ `expr $src : '/'` -eq 0 ]; then
  src=${PWD}/${src}
fi
if [ `expr $dst : '/'` -eq 0 ] ; then
  dst=${PWD}/${dst}
fi

[ -d ${src}/.cmtsite ] ||
{ echo >&2 "Distribution kit in '$src' built with PackDist prior to PackDist-00-05-00: Cannot relocate"; exit 1; }

if [ "${_inplace}" != y ]; then
  [ ! -d ${dst} ] || { echo >&2 "Directory '${dst}' already exists"; exit 2; }
  mv ${src} ${dst} || exit
  _dst=${dst}
else
  _dst=${src}
fi

find ${_dst}/.cmtsite ${_dst}/cmtsite -noleaf -type f \( -name requirements\* -o -name asetup_defaults\* -o -name setup.sh -o -name setup.csh -o -name cleanup.sh -o -name cleanup.csh -o -name asetup.sh -o -name asetup.csh \) | xargs -r sed -i 's,'${src}','${dst}',g' || exit

find ${_dst}/cmtsite -noleaf -type f -name asetup_defaults\* | xargs -r sed -i 's,'`dirname ${src}`','`dirname ${dst}`',g' || exit

[ "${_config:-}" != y ] || exit 0

find ${_dst}/CMT -noleaf -type f \( -path \*/mgr/setup.sh -o -path \*/mgr/setup.csh -o -path \*/mgr/cleanup.sh -o -path \*/mgr/cleanup.csh \) | xargs -r sed -i 's,'${src}','${dst}',g' || exit

find ${_dst} -noleaf -type f \( -path \*/cmt/setup.sh -o -path \*/cmt/setup.csh -o -path \*/cmt/cleanup.sh -o -path \*/cmt/cleanup.csh \) | xargs -r sed -i 's,'${src}','${dst}',g' || exit

find ${_dst}/DBRelease -noleaf -type f -name \*.xml | xargs -r sed -i 's,'${src}','${dst}',g' || exit

sed_script='s,'"${src}"','"${dst}"',g'
# optimised environment setup - setup scripts
find "${_dst}" -noleaf -type f \( -path \*/standalone-setup.sh -o -path \*/standalone-init.sh -o -path \*/standalone-setup.csh -o -path \*/standalone-init.csh \) -exec sed -i "${sed_script}" '{}' \+ || retval=1

# optimised environment setup - requirements file
find "${_dst}" -noleaf -type f -name requirements -exec grep -q '^# CMTFLAGS:' '{}' \; -exec sed -i "${sed_script}" '{}' \+ || retval=1

exit ${retval}
