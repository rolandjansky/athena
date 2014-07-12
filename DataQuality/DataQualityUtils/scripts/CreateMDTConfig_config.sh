#!/bin/bash --norc
#
# Author Paolo Bagnaia <paolo.bagnaia@cern.ch>
# and Elena Solfaroli Camillocci <elena.solfaroli.camillocci@cern.ch>
# [Manual in CreateMDTConfig_readme.sh]
#
# create a "config" file for DQMF - PB + ESC 100319 v 3.1.2
#
# ./CreateMDTConfig_config.sh -i inputfile -o outputfile
#                    [-r refrootfile] [-d "BA BC EA EC"] [-s 1-16] [-h]
#
vers=3.1.2
echo -en "\n\n `date +'%d-%b-%y %T'` - preconfig - vers $vers\n\n" >&2
if [ $# -lt 1 ] ; then
   echo -en "\nfor help, type \"./CreateMDTConfig_config.sh -h\"\n\n"
   exit
fi
cinfile="./doesnot/exist"
outfile="/dev/stdout"
rootfile="./ref_file.root"
myfiles=`dirname $BASH_SOURCE`/CreateMDTConfig_files.sh
#
# defaults "BA" and sect 1-16, or use the "-d " and "-s " args
#
declare -ar detect_name=("-" BA BC EA EC)
declare -ar detect_nam1=("-" "Barrel_A" "Barrel_C" "Endcap_A" "Endcap_C")
declare -a  detect_flag=( 0   0  0  0  0)
arg_detect=" BA "
arg_sector="1-16"
while getopts "i:o:d:r:s:h" opt ; do
   case "$opt" in
      i) cinfile=$OPTARG      ;;
      o) outfile=$OPTARG      ;;
      d) arg_detect=`echo "$OPTARG" | tr "[:lower:]" "[:upper:]" | sed -r s/' +'/' '/g` ;;
      r) rootfile=`readlink -f $OPTARG` ;;
      s) arg_sector=$OPTARG   ;;
      h) echo -en "./CreateMDTConfig_config.sh -i inputfile [-o outputfile] "  >&2 ;
         echo -en "[-r refrootfile] [-b A-C] [-d \"BA BC EA EC\"] [-h]\n\n" >&2 ;
         exit ;;
      *) echo -en "\nfor help, type \"./CreateMDTConfig_config.sh -h\"\n\n" ; exit ;;
   esac
done


#
for (( i=1 ; i<=5 ; i++ )) ; do
   temp=`echo "$arg_detect" | cut -d " " -f $i-$i`
   if [ "$temp" != "" ] ; then
      for (( j=1 ; j<=4 ; j++ )) ; do
         if [ $temp == ${detect_name[$j]} ] ; then
            detect_flag[$j]=1
            echo -en "$i $temp $j\n" >&2
         fi
      done
   fi
done
let  sector_start=`echo "$arg_sector" | cut -f 1-1 -d "-"`
let  sector_end=`echo   "$arg_sector" | cut -f 2-2 -d "-"`
echo -en "input  file : $cinfile \n" >&2
echo -en "output file : $outfile \n" >&2
echo -en "ref.   file : $rootfile \n" >&2
echo -en "detector    : " >&2
for (( j=1 ; j<=4 ; j++ )) ; do
   if [ ${detect_flag[$j]} == 1 ] ; then
      echo -en "${detect_name[$j]} " >&2
   fi
done
echo -en "\nsectors     : $arg_sector \t($sector_start-$sector_end)\n" >&2
#
if [ ! -f $cinfile ] ; then
   echo -en "\nfile $cinfile not present - exit\n\n" >&2
   exit
fi
if ( [ $outfile != "/dev/stdout" ] && [ -f $outfile ] ) ; then
   echo -en "\nfile $outfile already present - exit\n\n" >&2
   exit
fi
if [ ! -f $rootfile ] ; then
   echo -en "\nfile $rootfile not present - exit\n\n" >&2
   exit
fi
#
function get_file_chambers() {
         grep -e '^use_ ' $cinfile | cut -f 2 -d ' ' ;}
function get_detect_number() {
         get_file_chambers | grep -c "${detect_nam1[$i_detect]}" ;}
function get_sector_number() {
         tag1="Sector0$i_sector"
         if (( $i_sector >= 10 )) ; then tag1="Sector$i_sector" ; fi
         tag="${detect_nam1[$i_detect]}/$tag1"
         get_file_chambers | grep -c "$tag";}
function get_chamber()       {
         tag1="Sector0$i_sector"
         if (( $i_sector >= 10 )) ; then tag1="Sector$i_sector" ; fi
         tag="${detect_nam1[$i_detect]}/$tag1"
         echo $tag >&2
         get_file_chambers | grep -e "$tag" | cut -f 5-5 -d '/' ;}
#
# loops
#
exec 7> $outfile
echo -en "#######################"  >&7
echo -e " created by $USER with v $vers on "`date +"%d-%b-%y %T"` >&7
echo -e "# Histogram Assessments" >&7
echo -e "#######################\n\n" >&7
echo -e "### One algorithm per histo" >&7
#
# GLOBAL
#
echo -e "dir GLOBAL {" >&7
echo -e "  output = ALLMDTs" >&7
echo -e "  hist phi_vs_eta {" >&7
echo -e "    algorithm = GatherData" >&7
echo -e "    display   = LogZ" >&7
echo -e "  }" >&7
echo -e "  hist Global_HitsPerML_Barrel {" >&7
echo -e "    algorithm = MDTOverview_Global" >&7
echo -e "    display   = LogZ" >&7
echo -e "    reference = BasicRef" >&7
echo -e "  }" >&7
echo -e "  hist Global_HitsPerML_Endcap {" >&7
echo -e "    algorithm = MDTOverview_Global" >&7
echo -e "    display   = LogZ" >&7
echo -e "    reference = BasicRef" >&7
echo -e "  }" >&7
echo -e "} ### End of GLOBAL" >&7
#
# MDT
#
echo -e "dir MDT {" >&7
echo -e "\nloop on MDT : \n" >&2
let loop=0
for (( i_detect=1; i_detect<=4 ; i_detect++ )) ; do                             # detector
#  echo -en "detector $i_detect ${detect_name[$i_detect]} ${detect_flag[$i_detect]}\n" >&2
   if [ ${detect_flag[$i_detect]} == 0 ] ; then continue ; fi
   dir_detect=${detect_nam1[$i_detect]}
   let n_dete=`get_detect_number`
   echo -e "\t\t\t\t\t$dir_detect\t\t: $n_dete chambers" >&2
   if [[ $n_dete = 0 ]] ; then continue ; fi
   echo -e "  dir $dir_detect {" >&7
   echo -e "    output = ${dir_detect}"'/'"MDT/AllSectors" >&7
   $myfiles "a_detect.config" | sed -r s/___dete/$dir_detect/ >&7               # <-- a
   for (( i_sector=$sector_start; i_sector<=$sector_end ; i_sector++ )) ; do    # sector
      dir_sector='Sector0'$i_sector
      if (( $i_sector >= 10 )) ; then dir_sector='Sector'$i_sector ; fi
      let n_sect=`get_sector_number`
      echo -e "\t\t\t\t\t$dir_detect $dir_sector \t: $n_sect chambers" >&2
      if [[ $n_sect = 0 ]] ; then continue ; fi
      echo -e "    dir $dir_sector {" >&7
      echo -e "      output = ${dir_detect}"'/'"MDT/${dir_sector}"'/'"AllChambers" >&7
      vectype=""
      for cham in `get_chamber` ; do                                            # chamber
         dir_name='MDT/'"${dir_detect}"'/'"${dir_sector}"'/'"${cham}"
         let loop++
#        echo -e "$loop\t: $dir_name" >&2                                       # comment
         $myfiles "b_chamber.config"  | sed -r s/___dete/$dir_detect/ | \
         sed s/___sect/$dir_sector/g  | sed s/___cham/$cham/g >&7               # <-- b
         vectype=${vectype/"${cham:0:3} "}"${cham:0:3} "
      done
      $myfiles 'c_overview1.config' >&7                                         # <-- c
      let ntyp=${#vectype}/4
      if ( ! [ ntyp == 0 ] ) ; then
         for (( ityp=0 ; ityp<ntyp ; ityp++ )) ; do
            let icha=ityp*4
            type=${vectype:$icha:3}
            $myfiles "d_overview2.config" | sed -r s/___type/$type/ >&7         # <-- d
         done
      fi
      $myfiles "e_overview3.config" >&7                                         # <-- e
      echo -e "    } ### End of $dir_sector" >&7
   done
   echo -e "  } ### End of $dir_detect" >&7
done
echo -e "} ### End of MDT" >&7
#
#  MDTvsRPC and MDTvsTGC (new 2.7.4)
#
let loop=0
rpcloop=0
tgcloop=0
for (( i_detect=1 ; i_detect<=4 ; i_detect++ )) ; do                            # detector
    if ( [ $i_detect == 3 ] && [ $rpcloop == 1 ] ) ; then
       echo -e "} ### End of MDTvsRPC\n" >&7
    fi
    echo -en "MDTvsRPC/TGC $i_detect ${detect_name[$i_detect]} ${detect_flag[$i_detect]}\n" >&2
    if [ ${detect_flag[$i_detect]} == 0 ] ; then continue ; fi
    let n_dete=`get_detect_number`
    if [[ $n_dete = 0 ]] ; then continue ; fi
    if ( [ $i_detect -lt 3 ] && [ $rpcloop == 0 ] ) ; then
       vsname="MDTvsRPC"
       rpcloop=1
    fi
    if ( [ $i_detect -gt 2 ] && [ $tgcloop == 0 ] ) ; then
       vsname="MDTvsTGC"
       tgcloop=1
    fi
    echo -e "dir $vsname {" >&7
    dir_detect=${detect_nam1[$i_detect]}
    echo -e "  dir $dir_detect {" >&7
    for (( i_sector=$sector_start; i_sector<=$sector_end ; i_sector++ )) ; do   # sector
       dir_sector='Sector0'$i_sector
       if (( $i_sector >= 10 )) ; then dir_sector='Sector'$i_sector ; fi
       let n_sect=`get_sector_number`
       if [[ $n_sect = 0 ]] ; then continue ; fi
       let loop++
       echo -e "$loop\t: $dir_sector" >&2
       $myfiles "f_trigdaq.config"    | sed -r s/___dete/$dir_detect/ | \
          sed s/___sect/$dir_sector/g | sed s/MDTvs___/$vsname/g >&7            # <-- f
    done
    echo -e "  } ### End of $dir_detect" >&7
done
if [ $tgcloop == 1 ] ; then
  echo -e "} ### End of MDTvsTGC\n" >&7
fi
#
#  output
#
################################################################################
ginfile="g_output_temp.txt"
function put_g_values() {
         declare -a vline=(`grep -e "^$2" -m 1 $ginfile`)
         if [ "${vline[0]}" == "" ] ; then
            echo -en "no line for tag $2\n" > /dev/stdout ; exit
         fi
         echo -en "$1  algorithm = ${vline[1]}\n" >&7
         echo -en "$1  weight    = ${vline[2]}\n" >&7
         }
function put_g_bracket() { echo -en "$1} ### End of $2\n" >&7 ;}
function put_g_valbra()  { put_g_values "$1" "$3" ; put_g_bracket "$1" "$2" ;}
#
s0="" ; s1="  " ; s2="    " ; s3="      " ; s4="        " ; s5="          "
echo -en "#############\n"   >&7
echo -en "# Output\n"        >&7
echo -en "#############\n\n" >&7
echo -en "## The 'top_level' must always be defined\n" >&7
echo -en "${s0}output top_level {\n" >&7
put_g_values "$s0" "/top_level/NULL/NULL/NULL/NULL/"
#
echo -en "${s1}output ALLMDTs {\n" >&7
put_g_valbra "$s1" "ALLMDTs" "/ALLMDTs/NULL/NULL/NULL/NULL/"
#
for (( i_detect=1; i_detect<=4 ; i_detect++ )) ; do                             # detector
   echo -en "detector $i_detect ${detect_name[$i_detect]} ${detect_flag[$i_detect]}\n" >&2
   if [ ${detect_flag[$i_detect]} == 0 ] ; then continue ; fi
   dir_detect=${detect_nam1[$i_detect]}
   let n_dete=`get_detect_number`
   echo -e "\t\t\t\t\t$dir_detect\t\t: $n_dete chambers" >&2
   if [[ $n_dete = 0 ]] ; then continue ; fi
   echo -en "${s1}output $dir_detect {\n" >&7
   put_g_values "$s1" "/$dir_detect/NULL/NULL/NULL/NULL/"
#
   echo -en "${s2}output MDT {\n" >&7
   put_g_values "$s2" "/$dir_detect/MDT/NULL/NULL/NULL/"
   echo -en "${s3}output AllSectors {\n" >&7
   put_g_valbra "$s3" "AllSectors" "/$dir_detect/MDT/AllSectors/NULL/NULL/"
   for (( i_sector=$sector_start; i_sector<=$sector_end ; i_sector++ )) ; do    # sector
      dir_sector='Sector0'$i_sector
      if (( $i_sector >= 10 )) ; then dir_sector='Sector'$i_sector ; fi
      let n_sect=`get_sector_number`
      echo -e "\t\t\t\t\t$dir_detect $dir_sector \t: $n_sect chambers" >&2
      if [[ $n_sect = 0 ]] ; then continue ; fi
      echo -en "${s3}output $dir_sector {\n" >&7
      put_g_values "$s3" "/$dir_detect/MDT/$dir_sector/NULL/NULL/"
      echo -en "${s4}output AllChambers {\n" >&7
      put_g_valbra "$s4" "AllChambers" "/$dir_detect/MDT/$dir_sector/AllChambers/NULL/"
      for cham in `get_chamber` ; do
         echo -en "${s4}output $cham {\n" >&7                                   # chamber
         put_g_values "$s4" "/$dir_detect/MDT/$dir_sector/$cham/NULL/"
         echo -en "${s5}output Chamber {\n" >&7                                 # chamber
         put_g_valbra "$s5" "Chamber" "/$dir_detect/MDT/$dir_sector/$cham/Chamber/"
         echo -en "${s5}output Occupancy {\n" >&7
         put_g_valbra "$s5" "Occupancy" "/$dir_detect/MDT/$dir_sector/$cham/Occupancy/"
         echo -en "${s5}output DeadStatus {\n" >&7
         put_g_valbra "$s5" "DeadStatus" "/$dir_detect/MDT/$dir_sector/$cham/DeadStatus/"
         echo -en "${s5}output Efficiency {\n" >&7
         put_g_valbra "$s5" "Efficiency" "/$dir_detect/MDT/$dir_sector/$cham/Efficiency/"
         put_g_bracket "$s4" "$cham"    # End of chamber
      done
      put_g_bracket "$s3" "$dir_sector" # End of sector
   done
   put_g_bracket "$s2" "MDT"            # End of MDT
#
# MDTvsXXX
#
   vstag="MDTvsRPC"
   if [ $i_detect -gt 2 ] ; then vstag="MDTvsTGC" ; fi
   echo -en "${s2}output $vstag {\n" >&7
   put_g_values "$s2" "/$dir_detect/$vstag/NULL/NULL/NULL/"
   for (( i_sector=$sector_start; i_sector<=$sector_end ; i_sector++ )) ; do    # sector
      dir_sector='Sector0'$i_sector
      if (( $i_sector >= 10 )) ; then dir_sector='Sector'$i_sector ; fi
      let n_sect=`get_sector_number`
      echo -e "\t\t\t\t\t$dir_detect $dir_sector \t: $n_sect chambers" >&2
      if [[ $n_sect = 0 ]] ; then continue ; fi
      echo -en "${s3}output $dir_sector {\n" >&7
      put_g_values "$s3" "/$dir_detect/$vstag/$dir_sector/NULL/NULL/"
      put_g_bracket "$s3" "$dir_sector" # End of sector
   done
   put_g_bracket "$s2" "$vstag"         # End of MDTvsXXX
   put_g_bracket "$s1" "$dir_detect"    # End of detector
done
put_g_bracket "$s0" "top_level"         # End of top_level
################################################################################
#
#  references, algorithms, thresholds
#
$myfiles "h_refe_algo_thr.config" | sed -r s\\___refroot\\$rootfile\\ >&7       # <-- h
echo -en "\n\n `date +'%d-%b-%y %T'` - preconfig - vers $vers - end\n\n" >&2
echo -en "#\n# end - `date +"%d-%b-%y %T"`\n" >&7
exec 7>&-
exit
#
