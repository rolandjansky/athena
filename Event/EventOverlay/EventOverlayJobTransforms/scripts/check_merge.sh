#!/bin/bash

if [ $# -lt 3 ]
then
  echo "Usage example: `basename $0` test stream outset <skipdq2>"
  exit 65
fi

testn=$1
stream=$2
outset=$3
skipdq2=$4

mkdir -p ~/nfs3/zerobias_skim_${testn}/merge/
cd ~/nfs3/zerobias_skim_${testn}/merge/

if [ "$skipdq2" != "" ]; then echo "skipping dq2"; else dq2-get -f "*tgz*" group.dataprep.BSmultipleselector.GRL.${testn}_EXT${stream}.merge.*.test${outset}/ ; fi
rm -rf tarball_PandaJob_*
for f in `ls group.dataprep.BSmultipleselector.GRL.${testn}_EXT${stream}.merge.*.test${outset}.*/*.tgz* `; do echo $f; tar xzf $f; done

for p in `ls -d tarball_PandaJob_* `; do
 grep "Job successfully completed" ${p}/pilotlog.txt  > /dev/null
 if [ $? -eq 0 ]; then echo "Good job"; 
 else 
  echo "Failed job";
  rm -rf $p
 fi
done

echo -n "total events: "
grep "number of events written" tarball_PandaJob_*/athena_stdout.txt | cut -d ':' -f 3 | awk '{total = total + $1}END{print total}'

echo -n "total files: "
grep "number of events written" tarball_PandaJob_*/athena_stdout.txt | grep -c number

echo -n "files with 100 events: "
grep "number of events written" tarball_PandaJob_*/athena_stdout.txt | grep -c "100"

echo -n "total jobs: "
ls tarball_PandaJob_*/athena_stdout.txt | grep -c athena

