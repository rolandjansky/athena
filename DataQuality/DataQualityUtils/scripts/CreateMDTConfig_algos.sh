#!/bin/bash --norc
#
# Author Paolo Bagnaia <paolo.bagnaia@cern.ch>
# and Elena Solfaroli Camillocci <elena.solfaroli.camillocci@cern.ch>
# [Manual in CreateMDTConfig_readme.sh]
#
# choose the weights and algos for CreateMDTConfig_config - PB 100319 v 3.1.2
#
# ./CreateMDTConfig_algos.sh -c chamber_list_input
#           [ output NOT changeable -o g_output_temp.txt]
#
usag="./CreateMDTConfig_algos.sh -c chamber_list_input"
usag=$usag" [not anymore : -u user_algos_input -o g_output_temp.txt]"
vers=3.1.2
debug=1
echo -en "\n\n `date +'%d-%b-%y %T'` - CreateMDTConfig_algos - vers $vers\n\n" >&2
#
if [ $# != 2 ] ; then
   echo -en "called with $# args\n$usag\n\n" >&2
   exit
fi
cinfile="./doesnot/exist"
uinfile="temp.user_algos.txt"
outfile="g_output_temp.txt"
while getopts "hc:u:" opt ; do
   case "$opt" in
      c) cinfile=$OPTARG       ;;
#     o) outfile=$OPTARG       ;;
#     u) uinfile=$OPTARG       ;;
      h | * ) echo -en "$usag\n\n" >&2 ; exit ;;
   esac
done
echo -en "c-input file : $cinfile - " >&2
echo -en "u-input file : $uinfile - " >&2
echo -en "output file  : $outfile \n" >&2
#
if [ ! -f $cinfile ] ; then
   echo -en "file $cinfile not present - exit\n" >&2
   exit
fi
if [ -f $uinfile ] ; then
   echo -en "file $uinfile already present - exit\n" >&2
   exit
fi
if ( [ $outfile != "/dev/stdout" ] && [ -f $outfile ] ) ; then
   echo -en "file $outfile already present - exit\n" >&2
   exit
fi
#
`dirname $BASH_SOURCE`/CreateMDTConfig_files.sh "user_algos.txt" >> $uinfile
#
exec 7> $outfile
echo -e "#    created by $USER with v $vers on "`date +"%d-%b-%y %T"` >&7
echo -en "#\n#\n" >&7
################
declare -ar detect_name=("-" BA BC EA EC)
declare -ar detect_nam1=("-" "Barrel_A" "Barrel_C" "Endcap_A" "Endcap_C")
function get_file_chambers() {
         grep -e '^use_ ' $cinfile | cut -f 2 -d ' ' ;}
function get_chamber()       {
         tag1="Sector0$i_sector"
         if (( $i_sector >= 10 )) ; then tag1="Sector$i_sector" ; fi
         tag="${detect_nam1[$i_detect]}/$tag1"
         echo $tag >&2
         get_file_chambers | grep -e "$tag" | cut -f 5-5 -d '/' ;}
function put_values()        {
         let item++
         lvl1[$item]=$f1 ; lvl2[$item]=$f2 ; lvl3[$item]=$f3
         lvl4[$item]=$f4 ; lvl5[$item]=$f5 ; algo[$item]=$fa ; wei[$item]=$fw  ;}
#
f1="NULL"  ; f2="NULL" ; f3="NULL" ; f4="NULL" ; f5="NULL"
fa="doesnotexist"
fw=-1.0
let item=0
################################################################################
f1="top_level" ; put_values
f1="ALLMDTs"   ; put_values
for (( i_detect=1; i_detect<=4 ; i_detect++ )) ; do                            # detector
   if [ $debug -gt 0 ] ; then
      echo -en "=== ${detect_nam1[$i_detect]}\n" >&2
   fi
   f1="${detect_nam1[$i_detect]}"
   f2="NULL" ; f3="NULL" ; f4="NULL" ; f5="NULL" ; put_values
   f2="MDT"  ; put_values
   f3="AllSectors" ; put_values
   for (( i_sector=1; i_sector<=16 ; i_sector++ )) ; do                         # sector
      dir_sector='Sector0'$i_sector
      if (( $i_sector >= 10 )) ; then dir_sector='Sector'$i_sector ; fi
      if [ $debug -gt 0 ] ; then
         echo -en "\t\t=== $dir_sector\n" >&2
      fi
      f3="$dir_sector" ; f4="NULL" ; f5="NULL" ; put_values
      f4="AllChambers" ; f5="NULL" ; put_values
      for cham in `get_chamber` ; do                                            # chamber
         if [ $debug -gt 1 ] ; then
            echo -en "\t\t\t\t cham $cham\n" >&2
         fi
         f4="$cham"
         f5="NULL"       ; put_values
         f5="Chamber"    ; put_values
         f5="Occupancy"  ; put_values
         f5="DeadStatus" ; put_values
         f5="Efficiency" ; put_values
      done
   done
   f2="MDTvsRPC" ; if [ $i_detect -gt 2 ] ; then f2="MDTvsTGC" ; fi
   f3="NULL" ; f4="NULL" ; f5="NULL" ; put_values
   for (( i_sector=1; i_sector<=16 ; i_sector++ )) ; do                         # sector
      dir_sector='Sector0'$i_sector
      if (( $i_sector >= 10 )) ; then dir_sector='Sector'$i_sector ; fi
      if [ $debug -gt 0 ] ; then
         echo -en "\t\t=== $dir_sector\n" >&2
      fi
      f3="$dir_sector" ; put_values
   done
done
################################################################################
let nitem=item
echo -en "\n\n\n=== end - $nitem items - user_changes \"`wc -l $uinfile`\" :\n\n" >&2
exec 6< $uinfile
eof=0
for (( loop=1; loop<=1000000 ; loop++ )) ; do
   if [ $eof != 0 ] ; then break ; fi
   read -u 6 iden fa left
   eof=$?
   if [[ $iden == "" || ${iden:0:1} == "#" ]] ; then continue ; fi
   fw=`echo "$left" | cut -f 1-1 -d '#' | sed -r s/' +'/''/g`
   if [ $debug -gt 3 ] ; then echo -en "[$loop] $iden\n" >&2 ; fi
   if [[ `echo "$iden" | cut -f 6- -d '/'` == "" ]] ; then     # not enough fields
      echo -en "\n\nline $loop : \"$iden\" : not enough fields specified - exit\n" >&2
      exit
   fi
   f1=`echo "$iden" | cut -f 2-2 -d '/'`
   f2=`echo "$iden" | cut -f 3-3 -d '/'`
   f3=`echo "$iden" | cut -f 4-4 -d '/'`
   f4=`echo "$iden" | cut -f 5-5 -d '/'`
   f5=`echo "$iden" | cut -f 6-6 -d '/'`
   if [ $debug -gt 0 ] ; then
      echo -en "[$loop]\t/$f1/$f2/$f3/$f4/$f5/\t$fa\t$fw" >&2
      if [ $debug -gt 1 ] ; then echo -en "\n" >&2 ; fi
   fi
   let nmod=0
   for  (( item=1; item<=nitem ; item++ )) ; do
      if [[ $f1 != '*' && $f1 != ${lvl1[$item]} ]] ; then continue ; fi
      if [[ $f2 != '*' && $f2 != ${lvl2[$item]} ]] ; then continue ; fi
      if [[ $f3 != '*' && $f3 != ${lvl3[$item]} ]] ; then continue ; fi
      if [[ $f4 != '*' && $f4 != ${lvl4[$item]} ]] ; then continue ; fi
      if [[ $f5 != '*' && $f5 != ${lvl5[$item]} ]] ; then continue ; fi
      let nmod++
      if [ $debug -gt 1 ] ; then
         echo -en    "\t[$nmod]\t/${lvl1[$item]}/${lvl2[$item]}/" >&2
         echo -en "${lvl3[$item]}/${lvl4[$item]}/${lvl5[$item]}/\n" >&2
      fi
      if [[ $fa != '=' ]] ; then algo[$item]="$fa" ; fi
      if [[ $fw != '=' ]] ; then  wei[$item]="$fw" ; fi
   done
   if [ $debug -gt 0 ] ; then echo -en "\t$nmod changes\n" >&2 ; fi
done
exec 6<&-
################################################################################
for  (( item=1; item<=nitem ; item++ )) ; do
   echo -en "/${lvl1[$item]}/${lvl2[$item]}/${lvl3[$item]}/" >&7
   echo -en  "${lvl4[$item]}/${lvl5[$item]}/" >&7
   echo -en "   ${algo[$item]}   ${wei[$item]}\n" >&7
done
echo -en "#\n# end - `date +'%d-%b-%y %T'`\n" >&7
exec 7>&-
echo -en "\nend - `date +'%d-%b-%y %T'`\n" >&2
rm -f $uinfile
exit
