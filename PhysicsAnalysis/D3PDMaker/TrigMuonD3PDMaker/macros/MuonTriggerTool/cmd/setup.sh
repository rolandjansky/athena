#!/bin/sh

HEADER_PATH=$1
HEADER_NAME=`basename ${HEADER_PATH}`
CLASS_NAME=${HEADER_NAME%.h}


cat > tmp.header <<EOF
#include <vector>
#include <string>
using namespace std;

EOF
cat $1 >> tmp.header
mv tmp.header $1


cat > ../MuonTriggerTool/MuonTriggerMatching.h <<EOF
#include "${HEADER_PATH}"
typedef ${CLASS_NAME} D3PDEvent;

EOF

cat ../template/MuonTriggerMatching.h >> ../MuonTriggerTool/MuonTriggerMatching.h
# eof
