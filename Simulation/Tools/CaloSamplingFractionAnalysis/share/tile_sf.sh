#!/bin/zsh

nevt=${1:-10000}
if [ $# -gt 0 ]
then shift
fi
args="${(j. .)${@}:- -00 -05 +05 -10 +10 -15 +15 -20 +20 -25 +25 -30 +30 -35 +35 -40 +40 -45 +45 -50 +50 -55 +55 -60 +60 -90 +90}"

Zbeam=20.
if [ ! -f tile_sf.C ]
then get_files tile_sf.C
     sed "s|-15.,15.|-${Zbeam},${Zbeam}|g" tile_sf.C > t.C
     mv -f t.C tile_sf.C
fi

for v in `echo $args`
do if [[ "$v" -lt 1 ]] && [[ "$v" -gt -1 ]] && [ "${v/./}" != "${v}" ]
   then p="Eta=$v"
   elif [[ "$v" -lt 90 ]] && [[ "$v" -gt -90 ]]
   then p="Z=0; Theta=$v"
   elif [[ "$v" -eq 90 ]] || [[ "$v" -eq -90 ]]
   then p="Z=2550; Theta=$v"
   elif [[ "$v" -gt  2280 ]] && [[ "$v" -lt  4250 ]]
   then p="Theta=90; Z=${v/+/}"
   elif [[ "$v" -lt -2280 ]] && [[ "$v" -gt -4250 ]]
   then p="Theta=-90; Z=${v/-/}"
   else echo "Ignoring invalid value $v"
        continue
   fi
   echo "========== Running athena for $p: $nevt events =========="
   athena.py -c "EvtMax=$nevt; PID=11; E=100000; Zbeam=[-$Zbeam,$Zbeam]; FileSuffix='$v'; $p;" TileSimEx/jobOptions_TileTB_Sim.py  
   athena.py -c "doHitNtuple=True; doD3PD=True; doTileNtuple=False; FileSuffix='$v';" TileSimEx/jobOptions_TileTB_Dig.py
   root -b -q "tile_sf.C(\"$v\");" > sf$v.log
done

#grep SF sf*.log
get_files TileSamplingFraction_analysis.C
root -b -q "TileSamplingFraction_analysis.C" > TileSamplingFraction_analysis.log

