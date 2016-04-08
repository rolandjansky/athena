#
#

Run=$1
stream=$2   #calibration_IDTracks
path=$3 
eosheader="root://eosatlas.cern.ch"
daqmerge="daq"

echo " Run =  ${Run}"
if [ -z "$path" ]
then 
    echo " Empty input path. Setting it to ./data/"
    if [ ! -d "data" ]
	then
	mkdir ./data
    fi
    
    path="./data/"
fi


if [ -z "$stream" ] 
then 
    echo " Empty stream. Setting it to calibration_IDTracks"
    stream="calibration_IDTracks"
fi

DISK="SCRATCHDISK"
#outputFile="List_${Run}_${stream}.list"
outputFile="Run_${Run}_${stream}.txt"
echo " Ouptut file -> ${outputFile}"
rucio list-file-replicas data15_13TeV:data15_13TeV.00$Run.${stream}.${daqmerge}.RAW | grep $DISK | cut -d ':' -f3,4,5 | cut -d '=' -f2 > $path$outputFile
#rucio list-file-replicas data15_13TeV:data15_13TeV.00$Run.${stream}.merge.DESDM_ZMUMU.f628_m1453_f628_m1501 | grep $DISK | cut -d ':' -f3,4,5 | c#ut -d '=' -f2 > $path$outputFile
sed -i -e 's/^/root:\/\/eosatlas\//g' $path$outputFile

nfiles=`wc ${path}${outputFile} | cut -d '.' -f1`
#cat $path$outputFile
echo " Run: ${Run} saved in ${path}${outputfile} with ${nfiles} lines"