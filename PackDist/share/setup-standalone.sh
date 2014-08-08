#
# Usage: . ./setup-standalone.sh [-H CMTHOME] [TAG]...
# Setup the environment for standalone installation of a project
# containing patches for the projects via asetup command [found in CMTHOME]
# with the tags [TAG]...
#
# Example:
# . ./setup-standalone.sh -H /afs/cern.ch/atlas/software/releases/17.0.3/cmtsite 17.0.3 32
#

unset cmthome
if [ $# -ge 1 ]; then
    if [ $1 = -H ]; then
	shift
	if [ $# -ge 1 ]; then
	    cmthome=$1; shift
	else
	    echo "setup-standalone.sh: ERROR: Argument required for option: -H" >&2; return 1
	fi
    fi
fi

tags=$(echo $* | tr -s ' ' ',')
shift $#

if [ -n "$cmthome" ]; then
    source $cmthome/asetup.sh ${tags}
else
    asetup ${tags}
fi
setupstandalonestatus=$?
unset cmthome tags
return $setupstandalonestatus
