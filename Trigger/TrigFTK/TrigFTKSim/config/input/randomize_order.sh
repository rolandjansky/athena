#!/bin/bash

fin=$1
fout=$2
if [ -z "${fin}" ]; then
    echo "USAGE: $0 INPUT_FILE OUTPUT_FILE" > /dev/stderr
    exit 1;
fi

for i in `cat ${fin}`; do echo "$RANDOM $i"; done | sort | sed -r 's/^[0-9]+ //' > ${fout}

