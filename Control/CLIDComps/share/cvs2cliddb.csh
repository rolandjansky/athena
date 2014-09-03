#!/bin/csh -f
#
# 05feb03 - cetull@lbl.gov
# WARNING: This shell script makes heavy use of regexp magic. Edit only
# if you are very comfortable with regular expressions.
#
if (${#argv} > 1) then
   echo "usage: $0 [TOPDIRS=$CMTPATH]"
   exit -1;
else if (${#argv} == 0) then
   set TOPDIRS=$CMTPATH
else
   set TOPDIRS=$1
endif

set TOP_DIRS="`echo $TOPDIRS | sed -e 's/:/ /g'`"


touch clid.junk || exit "No write access to $PWD" && exit -1
touch clid.cvs1 || exit "No write access to $PWD" && exit -1
foreach TOPDIR  ( $TOP_DIRS )
    if (! -d $TOPDIR) then
	echo "No such directory $TOPDIR"
	exit -2
    endif

    
    echo "Looking under ${TOPDIR}"
    echo -n "Please wait, this may take a while."

    foreach f ( `find $TOPDIR -type f \( -name '*.h' -o -name '*.icc' -o -name '*.cxx' \) -print` )
	echo -n '.'
	egrep -n 'CLASS_DEF' $f clid.junk | sed -e 's/[	 ]//g' >> clid.cvs1
	egrep -n 'CLID.*=' $f clid.junk | sed -e 's/[	 ]//g' >> clid.cvs1
    end
    echo ""
end
#
touch clid.cvs
egrep ':CLASS_DEF2*(.*,[0-9]*,[0-9]*)' clid.cvs1 >> clid.cvs
egrep 'CLID&*CLID_.*=[0-9]*;' clid.cvs1 >> clid.cvs
#
touch clid.tmp
egrep ':CLASS_DEF2*\(' clid.cvs \
    | sed -e 's/.*:CLASS_DEF2*(\(.*\),\([0-9]*\),[0-9]*).*/\2 \1/' \
    >> clid.tmp
egrep 'CLID&*CLID_.*=[0-9]*;' clid.cvs \
    | sed -e 's/.*CLID&*CLID_\(.*\)=\([0-9]*\);.*/\2 \1/' \
    >> clid.tmp
sort -n clid.tmp | uniq > clid.sort
#
sed -e 's/.* //' clid.sort | sort | uniq \
    >> clid.names
sed -e 's/ .*//' clid.sort | sort | uniq \
    >> clid.ids
#
sed -e 's/.* //' clid.sort | sort | uniq -c | egrep -v ' 1	' \
    >> clid.duplicatenames
sed -e 's/ .*//' clid.sort | sort | uniq -c | egrep -v ' 1	' \
    >> clid.duplicateids
#
touch clid.duplicates
foreach f ( `sed -e 's/ *[0-9]*	*//' clid.duplicate{ids,names}` )
    echo =========================== $f >> clid.duplicates
    egrep $f clid.cvs >> clid.duplicates
end
#
echo ''
echo 'File clid.sort contains a candidate clid.db'
echo 'File clid.duplicatenames contains a list of duplicated names with count'
echo 'File clid.duplicateids contains a list of duplicated CLIDs with count'
echo 'File clid.duplicates contains the lines for duplicate names and CLIDs'
echo ''
#
