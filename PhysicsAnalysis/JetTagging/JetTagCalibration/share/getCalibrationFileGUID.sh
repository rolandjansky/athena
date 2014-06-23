#!/bin/sh

##########################

## script to get calibration file GUID from the database given a tag

## Georges Aad <aad@cppm.in2p3.fr>

#########################

if [ $# = 0 ]; then
  echo "Usage: $0 tag [options]"
  echo "options are:"
  echo "--db data base name - default \"oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_GLOBAL;dbname=OFLP200\""
  echo "--tagger tagger name - default IP3D"
  echo "--jet jet collection name - default Cone4H1Tower"
  echo "--verbose print more detailed output"
  echo "--help print this help message"
  exit
fi

if [ $1 = "--help" ]; then
  echo "Usage: $0 tag [options]"
  echo "options are:"
  echo "--db data base name - default \"oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_GLOBAL;dbname=OFLP200\""
  echo "--tagger tagger name - default IP3D"
  echo "--jet jet collection name - default Cone4H1Tower"
  echo "--verbose print more detailed output"
  echo "--help print this help message"
  exit
fi

verbose=0
dbName="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_GLOBAL;dbname=OFLP200"
taggerName=IP3D
jetCollectionName=Cone4H1Tower

tagname=$1

iarg=1
args=($@)


for opt in $@
do

  if [ $opt = "--db" ]; then
    dbName=${args[$iarg]}
  fi

  if [ $opt = "--tagger" ]; then
    taggerName=${args[$iarg]}
  fi

  if [ $opt = "--jet" ]; then
    jetCollectionName=${args[$iarg]}
  fi

  if [ $opt = "--verbose" ]; then
    verbose=1
  fi

  (( iarg++ ))

done


echo "running $0 with tag = $tagname, db = $dbName, tagger = $taggerName and jet collection = $jetCollectionName"

tmpfile=tmp$$
logfile=log$$

cat > $tmpfile <<EOF

listchans /GLOBAL/BTagCalib/$taggerName
exit
EOF

AtlCoolConsole.py $dbName <  $tmpfile > $logfile

channum=`awk '$2 == chan { channum=$1 };END {print channum}' chan=$jetCollectionName $logfile`
channum="${channum:=0}"

if [ $verbose = 1 ]; then 
 cat $logfile
fi

rm $logfile
rm $tmpfile

if [ $channum = 0 ]; then 
  echo jet collection $jetCollectionName not known for tagger $taggerName
  exit
fi

echo channel number is $channum


cat > $tmpfile <<EOF

usetag $tagname
usechan $channum
less /GLOBAL/BTagCalib/$taggerName
exit
EOF


AtlCoolConsole.py $dbName < $tmpfile > $logfile

fileguid=`cat $logfile | grep fileGUID | awk '{print $6}' | cut -d] -f1`

if [ $verbose = 1 ]; then 
 cat $logfile
fi

rm $logfile
rm $tmpfile

echo
echo "------------------------------------------------"
echo file GUID is $fileguid
echo "------------------------------------------------"
echo
