#!/bin/bash

# art-description: DirectIOART AthAnalysis ParticleSelectionAlg protocol=ROOT
# art-type: grid
# art-output: *.pool.root
# art-include: 21.2/AthAnalysis

set -e

athena --filesInput="root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1" DirectIOART/DirectIOARTAthAnalysisJobOptions.py

echo "art-result: $? DirectIOART_AthAnalysis_ParticleSelectionAlg_protocol_ROOT"