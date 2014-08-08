# File: pack-common.sh
#
# Functions used by all scripts.
#
# Author: Grigori Rybkine <Grigori.Rybkine@cern.ch>.
#

#
# Usage: info|warn|error [message [location]]
#
info () { echo -e ${scriptname:=`basename $0`}: ${2:+$2:} INFO: "$1" 1>&2; }
#info () { echo -e "${scriptname:=`basename $0`}: ${2:+$2: }INFO:" "$1" 1>&2; }
#info () { echo -e "`basename $0`: ${2:+$2: }INFO:" "$1"; }
warn () { echo -e "${scriptname:=`basename $0`}: ${2:+$2: }WARNING:" "$1" 1>&2; }
error () { echo -e "${scriptname:=`basename $0`}: ${2:+$2: }ERROR:" "$1" 1>&2; }

abspath ()
{
#
# Usage: abspath FILEPATH
#
# if FILEPATH is invalid or empty,
# the result is an empty string
#
[ $# -eq 1 ] || { error "Wrong number of arguments: $#" $FUNCNAME; return 1; }
if [ -f "$1" ]; then
    local d=`dirname $1`
    d=`cd $d || exit; pwd -P` ||
    { error "Cannot resolve path: $1" $FUNCNAME; return 1; }
    echo $d/`basename $1`
elif [ -d "$1" ]; then
    (cd $1 || exit; pwd -P) ||
    { error "Cannot resolve path: $1" $FUNCNAME; return 1; }
#    (cd $1; /bin/pwd)
else
    error "No such file or directory: $1" $FUNCNAME; return 1
fi
}

duf ()
{
#
# Usage: duf [OPTION]... [FILE]...
# Show information about the filesystem on which the current directory resides,
# display the total amount of disk space required for FILEs, or standard input,
# or the current directory by default.
#
#  -p PATH   show information about the filesystem on which PATH resides
#
local path
OPTIND=1
while getopts "p:" opt; do
    case $opt in
	p) path=${OPTARG} ;;
	\?) error "Invalid usage" $FUNCNAME; return 1 ;;
    esac
done

shift $(($OPTIND - 1))

[ "$path" ] || path=$PWD

#local fstype=$(df -P -T $path|tail -n 1|awk '{print $2}')
# use POSIX output (and options)
local -a fsinfo=(`df -kP $path|tail -n 1|awk '{print $1, $2, $4}'`) ||
{ error "awk" $FUNCNAME; return 1; }
[ "${fsinfo[0]}" ] || { error "Cannot determine filesystem type: $path" $FUNCNAME; return 1; }
#[ "$fstype" ] || { error "Cannot determine filesystem type: $path" $FUNCNAME; return 1; }

if [[ ${fsinfo[0]} == [Aa][Ff][Ss] ]]; then
    fsinfo=(`fs lq $path|tail -n 1|awk '{print $1, $2, $2 - $3}'`)
fi
# local -a fsinfo=(`
# case $fstype in
#     afs) fs lq $path|tail -n 1|awk '{print $1, $2, $2 - $3}' ;;
#     *) df -P $path|tail -n 1|awk '{print $1, $2, $4}' ;;
# esac
# `)

# use POSIX options of du -ksL (c - grand total, not POSIX)
case $# in
    0) local reqspace=$(xargs du -k -s -c -L |
    awk '$2 == "total" { sum += $1 }; END{ print sum }') ;;
    *) local reqspace=$(du -k -s -c -L "$@" |
    awk '$2 == "total" { sum += $1 }; END{ print sum }') ;;
esac

local fmt="%-20s %15s %15s %15s\n"
printf "$fmt" "Filesystem" "1024-blocks" "Available" "Required"
printf "$fmt" ${fsinfo[0]} ${fsinfo[1]} ${fsinfo[2]} ${reqspace}
}

newer ()
{
#
# Usage: newer -p PATH [OPTION]... [FILE]...
#
#  -L   dereference all symbolic links
#

local path follow

OPTIND=1
while getopts "p:L" opt; do
    case $opt in
	p) path=${OPTARG} ;;
	L) follow=yes ;;
	\?) error "Invalid usage" $FUNCNAME; return 1 ;;
    esac
done

shift $(($OPTIND - 1))

[ "$path" ] || { error "PATH must be supplied with '-p'" $FUNCNAME; return 1; }

local tmpnew=$(mktemp -t tmp.XXXXXXXXXX) || { error "mktemp" $FUNCNAME; return 1; }

case $# in
#    0) xargs findnewer -p $path ${follow:+-L} ;;
    0) xargs -r sh -c "findnewer -p $path ${follow:+-L} \$0 \$@" >>$tmpnew ;;
    *) findnewer -p $path ${follow:+-L} "$@" >|$tmpnew ;;
esac

local retval=$?

[ -s $tmpnew ] && { warn "$path: Newer files available:" $FUNCNAME; cat $tmpnew >&2; }
rm -f $tmpnew

case $retval in
    123|124) error "findnewer" $FUNCNAME; return 1 ;;
    125|126|127|1) error "xargs" $FUNCNAME; return 1 ;;
    2) error "findnewer" $FUNCNAME; return 2 ;;
esac
}

findnewer ()
{
#
# Usage: findnewer -p PATH [OPTION]... [FILE]...
#
#  -L   dereference all symbolic links
#

local path follow

OPTIND=1
while getopts "p:L" opt; do
    case $opt in
	p) path=${OPTARG} ;;
	L) follow=yes ;;
	\?) error "Invalid usage" $FUNCNAME; return 2 ;;
    esac
done

shift $(($OPTIND - 1))

[ "$path" ] || { error "PATH must be supplied with '-p'" $FUNCNAME; return 2; }

[ ${uname} != Linux ] || noleaf=yes
find $(echo_F "$@") ${noleaf:+-noleaf} ${follow:+-follow} -newer $path -ls ||
#find ${follow:+-L} $(echo_F "$@") ${noleaf:+-noleaf} -newer $path -ls ||
{ error "find" $FUNCNAME; return 2; }
}

echo_F ()
{
#
# Usage: echo_F [FILE]...
# Append forward slash (/) to the names of FILEs that are symbolic links to existing directories.
# Print the names of the other FILEs unchanged.
#
local s first=y
for s; do
    if [ "$first" ]; then
	echo -n "$s"; unset first
    else
	echo -n " $s"
    fi
    if [ -h "$s" -a -d "$s" ]; then
	[[ $s == ?*/ ]] || echo -n /
    fi
done
}

export -f findnewer error echo_F
#export scriptname

path_prepend ()
{
#
# Usage: path_prepend PATHVARIABLE PATH
#
[ $# -eq 2 ] || return 0
[ "$1" ] && [ "$2" ] || return 0

local val=$2
for p in $(eval echo \$$1 | tr -s ':' ' '); do
    [ $p = $2 ] || val=${val}:$p
done
eval $1=$val
}

path_append ()
{
#
# Usage: path_append PATHVARIABLE PATH
#
[ $# -eq 2 ] || return 0
[ "$1" ] && [ "$2" ] || return 0

local val
for p in $(eval echo \$$1 | tr -s ':' ' '); do
    [ $p = $2 ] || if [ -n "${val}" ]; then val=${val}:$p; else val=$p; fi
done
if [ -n "${val}" ]; then val=${val}:$2; else val=$2; fi
eval $1=$val
}
