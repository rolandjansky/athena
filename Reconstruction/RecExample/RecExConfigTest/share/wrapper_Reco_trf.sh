#!/bin/sh

if [[ -z "$jobopts" ]]
then
    jobopts="jobo"
fi

echo $@
Reco_trf.py "$@"; true && echo "starting find_cfg_dups.py" && \
sed 's/.*athena.py \(.\+\) -c.*/ \1/g;s/ /\ninclude(\"/g;s/\.py/.py\")/g' *_runathena > $jobopts.py && \
find_cfg_dups.py --fast $jobopts.py | gzip -c > find-cfg-dups.log.gz

