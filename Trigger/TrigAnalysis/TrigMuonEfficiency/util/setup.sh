#!/bin/sh

EXTERNAL_HEADER_PATH=
USE_TDT=0
FOR_HSG3=0
if [ $# -eq 1 ]; then
  EXTERNAL_HEADER_PATH=$1
  EXTERNAL_HEADER_NAME=`basename ${EXTERNAL_HEADER_PATH}`
  CLASS_NAME=${EXTERNAL_HEADER_NAME%.h}
  grep "D3PDReader" $1 > /dev/null
  NOT_D3PD_READER=$?
elif [ $# -eq 2 ]; then
    EXTERNAL_HEADER_PATH=$1
    EXTERNAL_HEADER_NAME=`basename ${EXTERNAL_HEADER_PATH}`
    CLASS_NAME=${EXTERNAL_HEADER_NAME%.h}
    grep "D3PDReader" $1 > /dev/null
    NOT_D3PD_READER=$?
  if [ "$2" == "TDT" ]; then
    USE_TDT=1
  elif [ "$2" == "HSG3" ]; then
    FOR_HSG3=1
  else
    echo "usage> $0 <path to header file> [optional: TDT or HSG3]"
    exit 1
  fi
else
  echo "usage> $0 <path to header file> <optional: TDT or HSG3>"
  exit 1
fi


if [ $NOT_D3PD_READER = 1 ]; then
#------------------------------------------------------------------#
# for root analysis with direct access to ntuple variables
#------------------------------------------------------------------#
  cat > tmp.header <<EOF
#include <vector>
#include <string>
using namespace std;
EOF

  cat ${EXTERNAL_HEADER_PATH} >> tmp.header
  mv tmp.header ${EXTERNAL_HEADER_PATH}

  for ii in MuonTriggerMatching ElectronTriggerMatching
  do
    cat > ../TrigMuonEfficiency/${ii}.h <<EOF
#include "${EXTERNAL_HEADER_PATH}"
typedef ${CLASS_NAME} MTT__D3PDEvent;
EOF
  cat ../skeleton/standard_h > ../Root/${ii}.cxx

  done

else
#------------------------------------------------------------------#
# for root analysis with D3PDReader to access ntuple variables
#------------------------------------------------------------------#
  for ii in MuonTriggerMatching ElectronTriggerMatching
  do
    if [ ${USE_TDT} -eq 1 ]; then
      PRELIMINARY_DEFS="#define __USE_TRIGROOTANALYSIS__"
    else
      PRELIMINARY_DEFS=""
    fi
    cat > ../TrigMuonEfficiency/${ii}.h <<EOF
#ifndef __TrigMuonEfficiency_h_preheader__
#define __TrigMuonEfficiency_h_preheader__
${PRELIMINARY_DEFS}
namespace D3PDReader {
  class ${CLASS_NAME};
}
#define MTT__D3PDEvent D3PDReader::${CLASS_NAME}
#include "${EXTERNAL_HEADER_PATH}"
#endif
EOF
    echo ${PRELIMINARY_DEFS} > ../Root/${ii}.cxx
    if [ ${FOR_HSG3} -eq 1 ]; then
      cat ../skeleton/d3pdreader_hsg3_h >> ../Root/${ii}.cxx
    else
      cat ../skeleton/d3pdreader_h >> ../Root/${ii}.cxx
    fi
  done

  cat > tmp__h << EOF
#ifndef LEPTON_TRIGGER_MATCHING
#define LEPTON_TRIGGER_MATCHING
#include "TrigMuonEfficiency/MuonTriggerMatching.h" 
#include "TrigMuonEfficiency/ElectronTriggerMatching.h" 
#endif
EOF
  cat ../Root/LinkDef.h >> tmp__h
  mv tmp__h ../Root/LinkDef.h
fi

for ii in MuonTriggerMatching ElectronTriggerMatching
do
  cat ../skeleton/${ii}_cxx >> ../Root/${ii}.cxx
  cat ../skeleton/${ii}_h >> ../TrigMuonEfficiency/${ii}.h
done

# eof
