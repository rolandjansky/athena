#!/bin/sh

schema=$1
folder=$2

AtlCoolConsole.py ${schema} <<EOF 2>/dev/null | grep -v ">>>" | grep -v "Connected"
listtags ${folder}
EOF

