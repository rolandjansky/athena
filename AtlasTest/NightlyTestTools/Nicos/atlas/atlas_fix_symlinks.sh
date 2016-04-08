#!/bin/sh
#
# File:-
#   atlas_fix_symlinks.sh
#
# derived from script copy_release.sh of
#   David Quarrie   <David.Quarrie@cern.ch>
# by
#   Alex Undrus     <undrus@bnl.gov>
#

#+
# Shell functions
#-

function show_help() {
    echo "Usage: ${script} [Options] [release]"
    echo "  Options:"
    echo "    -f|--from <dir>         'from' (source) directory"
    echo "    -h|--help               Display this help"
    echo "    -s|--silent             No output feedback"
    echo "    -t|--to <dir>           Specify 'to' (destination) directory"
    echo "    -v|--verbose            Verbose output feedback"
    echo	
}

#+
# Main Program
#-

# --- Defaults for parameters controlled by command line options

feedback=default
fromLoc=
toLoc=

#+
# Parse the options and arguments
#-

while [ $# -ne 0 ]; do
    case $1 in
        -f   | --from)         fromLoc=$2; shift;;
        -h   | --help)         show_help; exit 0;;
        -s   | --silent)       feedback=silent;;
        -t   | --to)           toLoc=$2; shift;;
        -v   | --verbose)      feedback=verbose;;
        -*   | --*)            show_help; exit 1;;
	esac
	shift
done

#+
# Fixup symbolic links
#-

if [ "$toLoc" = "" ]; then
echo "atlas_fix_symlinks: Error : destination dir not specified in command line"
exit 1
fi
if [ "$fromLoc" = "" ]; then
echo "atlas_fix_symlinks: Error : source dir not specified in command line"
exit 1
fi
if [ ! -d "$toLoc" ]; then
echo "atlas_fix_symlinks: Error : destination dir $toLoc does not exist"
exit 1
fi
if [ ! -d "$fromLoc" ]; then
echo "atlas_fix_symlinks: Error : source dir $fromLoc does not exist"
exit 1
fi
prevdir=`pwd`
cd $fromLoc
fromLoc=`/bin/pwd`
cd $prevdir

	if [ "$toLoc" = "$fromLoc" ]; then
    	echo "atlas_fix_symlinks: Source and destination are identical ($toLoc) - symlink fixup bypassed"
	else
		if [ "$feedback" != "silent" ]; then
			echo "atlas_fix_symlinks: Fixing up symlinks in ${toLoc}..."
		fi
		if [ -z "$null" ]; then
			cd $toLoc
			if [ "$feedback" = "verbose" ]; then
				echo "atlas_fix_symlinks: Finding symlinks..."
			fi
			if [ -z "$platform" ]; then
				symlinks=`/usr/bin/find . -type l -print`
			else
				symlinks=`/usr/bin/find . -type l -print | grep $platform`
			fi
			if [ "$feedback" = "verbose" ]; then
				echo "atlas_fix_symlinks: Fixing up symlinks..." 
			fi
			for link in $symlinks; do
				oldtgt=`/bin/ls -l $link | /usr/bin/awk '{print $11}'`
				match=`echo $oldtgt | /bin/grep $fromLoc`
				if [ ! -z "$match" ]; then
					newtgt=`echo $oldtgt | /bin/sed s:${fromLoc}:${toLoc}:`
					linkdir=`dirname $link`
					if [ ! -w $linkdir ]; then
						chmod u+w $linkdir
					fi
					rm -f $link
					/bin/ln -fs $newtgt $link
					if [ "$feedback" = "verbose" ]; then
						echo "atlas_fix_symlinks: $link (was $oldtgt) changed to $newtgt" 
					fi
				else
					if [ "$feedback" = "verbose" ]; then
						echo "atlas_fix_symlinks: $link (was $oldtgt) not changed" 
					fi
				fi
			done
			if [ "$feedback" != "silent" ]; then
				echo "atlas_fix_symlinks: Symlink fixup complete."
			fi
		else
			echo "atlas_fix_symlinks: "cd $toLoc"
			echo "atlas_fix_symlinks: "Finding symlinks..." 
			if [ -z "$platform" ]; then
				echo "atlas_fix_symlinks: "/usr/bin/find . -type l -print"
			else
				echo "atlas_fix_symlinks: "/usr/bin/find . -type l -print | grep $platform"
			fi
			echo "atlas_fix_symlinks: "Fixing up symlinks..." 
			echo "atlas_fix_symlinks: "Symlink fixup complete."
		fi
	fi

exit 0
