
# File: AtlasExternalArea/share/pack_dist_check.sh
# Author: Grigori Rybkine <Grigori.Rybkine@cern.ch>

#
# In case <package> exports external s/w,
# check mandatory distribution kit related macros,
# <package>_export_paths and <package>_native_version,
# expected by PackDist distribution kit building tool
#

#
# Errors can be turned into warnings by specifying
# -level=WARNING|warning option
# or
# unsetting PEDANTIC environment variable
#

LS="ls -H -d -p -m"

package=
files=
version=
level=

self="`basename $0`"

while test "$#" -gt 0; do
    arg=$1
    shift
    
    case ${arg} in
	-package=*)
	package=${arg#"-package="}
	;;
	-files=*)
	_files=${arg#"-files="}
	if [ -n "$files" ]; then files="$files ${_files}"; else files="${_files}"; fi
	;;
	-version=*)
	version=${arg#"-version="}
	;;
	-level=*)
	level=${arg#"-level="}
	;;
	*)
	    \printf "${self}: Error: %s\n" \
		"unrecognised option/argument: ${arg}" >&2
	    exit 2
	    ;;
    esac
done

[ -n "$package" ] || {
    \printf "${self}: Error: %s\n" \
	"-package option unspecified" >&2
    exit 2; }
[ -n "$files" ] || {
    \printf "${self}: Warning: %s\n" \
	"-files option unspecified" >&2
    exit 0; }

case ${level} in
    [Ww][Aa][Rr][Nn][Ii][Nn][Gg]*) level=Warning;;
    [Ee][Rr][Rr][Oo][Rr]*) level=Error;;
    *) level=;;
esac
if [ -z "${level}" ]; then
    if [ -z "${PEDANTIC}" ]; then
	level=Warning
    else
	level=Error
    fi
fi

retval=0

stdout=">/dev/null"
[ -n "${cmtmsg}" ] && echo "${self}: checking ${package}_export_paths paths"
[ -n "${makecmd}" ] && { set -v; stdout=""; }
eval ${LS} ${files} ${stdout}
[ $? -eq 0 ] || [ ${level} = Warning ] || retval=1
set +v

if [ -z "$version" ]; then
    \printf "${self}: ${level}: %s\n" \
	"${package}_native_version unspecified" >&2
    [ ${level} = Warning ] || retval=1
fi

exit ${retval}
