#!/bin/sh
# File: rpm-erase.sh
#
# Author: Grigori Rybkine <Grigori.Rybkine@cern.ch>.
#
__version__='0.3.1'
__date__='Fri Mar 28 2008'
__author__='Grigori Rybkine <Grigori.Rybkine@cern.ch>'

_help ()
{
cat <<EOF
Usage: `basename $0` [OPTION]... PACKAGE [PACKAGE]...
Erase (uninstall) RPM PACKAGE(s).

  -p <path>      use the database in directory <path>
  -D             erase as many package(s) upon which PACKAGE(s) depend as possible
  -k <pattern>   do not erase package(s) which match (basic) regular expression <pattern>
  -y             assume Yes to all queries and do not prompt
  -h             display this help and exit
  -v             output version information and exit

Report bugs to <Grigori.Rybkine@cern.ch>.
EOF
}

_version ()
{
echo "`basename $0` version $__version__"
echo Written by Grigori Rybkine.
}

_parse_opts ()
{
local msg="Try '`basename $0` -h' for more information."

unset dbpath deps keep reply

while getopts ":p:Dk:yhv" opt; do
    case $opt in
	p) eval dbpath=${OPTARG} ;;
	D) deps=yes ;;
	k) keep="${OPTARG}" ;;
	y) reply=yes ;;
	h) _help; exit 0 ;;
	v) _version; exit 0 ;;
	\?) echo -e "`basename $0`: invalid option: -${OPTARG}\n$msg"; exit 1;;
	:) echo -e "`basename $0`: argument required for option: -${OPTARG}\n$msg"; exit 1;;
    esac
done

shift $(($OPTIND - 1))

[ $# -gt 0 ] || { echo -e "`basename $0`: no argument(s) specified\n$msg"; exit 1; }
}

_reqpkg_d ()
{
# 
# depth-first
#
local created=no
[ -f "$tmpdone" ] ||
# Do not consider the /bin/sh capability
{ tmpdone=$(mktemp) && created=yes && echo /bin/sh >>$tmpdone; } || return

local tmpreq=$(mktemp) || return
local tmppkg=$(mktemp) || return
local reqnm pkg

for pkg; do

rpm ${dbpath:+--dbpath ${dbpath}} -q --quiet $pkg || continue
rpm ${dbpath:+--dbpath ${dbpath}} -q --qf '[%{REQUIRENAME}\n]' $pkg >|$tmpreq || continue

for reqnm in $(<$tmpreq); do
    grep -q "^$reqnm\$" $tmpdone && continue
    echo $reqnm >>$tmpdone || return
    rpm ${dbpath:+--dbpath ${dbpath}} -q --qf '[%{NAME}\n]' --whatprovides $reqnm >|$tmppkg || continue
    _exclude "$keep" $tmppkg || return
    cat $tmppkg || return
    if [ "$deps" ]; then
	_reqpkg_d $(<$tmppkg) || return
    fi
done

done

rm -f $tmpreq $tmppkg
[ $created == no ] || rm -f $tmpdone
}

_reqpkg ()
{
[ $# -gt 0 ] || return 0

local created=no
[ -f "$tmpdone" ] ||
# Do not consider the /bin/sh capability
{ tmpdone=$(mktemp) && created=yes && echo /bin/sh >>$tmpdone; } || return

local tmpreq=$(mktemp) || return
local tmpnm=$(mktemp) || return
local tmppkg=$(mktemp) || return
local pkg reqnm

for pkg; do
rpm ${dbpath:+--dbpath ${dbpath}} -q --quiet $pkg || continue
rpm ${dbpath:+--dbpath ${dbpath}} -q --qf '[%{REQUIRENAME}\n]' $pkg >|$tmpnm || continue
cat $tmpnm >>$tmpreq || return
done
sort -u -o $tmpreq $tmpreq || return

for reqnm in $(<$tmpreq); do
    grep -q "^$reqnm\$" $tmpdone && continue
    echo $reqnm >>$tmpdone || return
    rpm ${dbpath:+--dbpath ${dbpath}} -q --qf '[%{NAME}\n]' --whatprovides $reqnm >|$tmpnm || continue
    cat $tmpnm >>$tmppkg || return
done
sort -u -o $tmppkg $tmppkg || return

rm -f $tmpreq $tmpnm

_exclude "$keep" $tmppkg || return
cat $tmppkg || return
if [ "$deps" ]; then
    _reqpkg $(<$tmppkg) || return
fi

rm -f $tmppkg
[ $created == no ] || rm -f $tmpdone
}

_erase ()
{
local tmpdone=$(mktemp) || return

local total=`<$1 sort -u | wc -l` || return
local nmbr=0 erased pkg
while true; do
    erased=0
    for pkg in $(<$1); do
	grep -q "^$pkg\$" $tmpdone && continue
	rpm ${dbpath:+--dbpath ${dbpath}} --test -e "$pkg" >/dev/null 2>&1 || continue
	rpm ${dbpath:+--dbpath ${dbpath}} -e "$pkg" || continue
	#let erased++ nmbr++ || return
	erased=`expr $erased + 1` || return
	nmbr=`expr $nmbr + 1` || return
	#echo -e "\t$nmbr: $pkg\t[$((nmbr*100/total))%]" || return
	#echo -e "\t$nmbr: $pkg\t[$(expr $nmbr '*' 100 / $total)%]" || return
	printf "%5d: %-65s [%3d%%]\n" $nmbr $pkg $(expr $nmbr '*' 100 / $total) || return
	echo $pkg >>$tmpdone || return
	_exclude "^$pkg\$" $1 || return
    done
    [ $erased -gt 0 ] || break
done
rm -f $tmpdone
}

_uniq ()
{
#
# Discard all but one of identical lines from standard input,
# writing to standard output.
#
awk '{ if (vue[$0] != 1) { vue[$0] = 1; print } }'
}

_exclude ()
{
#
# Discard all lines of file $2 which match (basic) regular expression $1.
#
[ "$1" ] || return 0
sed -i '\%'"$1"'%d' "$2"
}

main ()
{
_parse_opts "$@"
shift $(($OPTIND - 1))

local tmppkg=$(mktemp) || return

if rpm ${dbpath:+--dbpath ${dbpath}} -q "$@" &&
    rpm ${dbpath:+--dbpath ${dbpath}} --test -e "$@"; then

    rpm ${dbpath:+--dbpath ${dbpath}} -q --qf '%{NAME}\n' "$@" | _uniq >>$tmppkg || return

    _exclude "$keep" $tmppkg || return
    if [ "$deps" ]; then
	echo -n "Building Dependency Tree..."
	local tmppkgs=$(mktemp) || return
	if [ "$depth" != yes ]; then
	    _reqpkg $(<$tmppkg) | _uniq >>$tmppkgs || return
	else
	    _reqpkg_d $(<$tmppkg) | _uniq >>$tmppkgs || return
	fi
	cat $tmppkgs >>$tmppkg || return
	rm -f $tmppkgs
	echo " Done"
#	_exclude "$keep" $tmppkg || return
    fi

    if [ "$reply" != yes ]; then

	echo "Try to REMOVE the following packages:"
	<$tmppkg sort -u | sed 's/^/\t/' || return
	echo "`<$tmppkg sort -u | wc -l` removed."
	echo -n "Do you want to continue? [Y/n] "
	read reply

    fi

    case "$reply" in
	y*|Y*)
	    echo "Executing RPM (-e)..."
	    _erase $tmppkg || return
	    echo "Done."
	    if [ -s $tmppkg ]; then
		echo "Could not REMOVE the following packages:"
		<$tmppkg sort -u | sed 's/^/\t/' || return
		echo "`<$tmppkg sort -u | wc -l` not removed."
	    fi
	    ;;
	*) echo "Abort.";;
    esac

else
    rm -f $tmppkg
    return 1

fi
retval=$?
rm -f $tmppkg
return $retval
}

main "$@"
