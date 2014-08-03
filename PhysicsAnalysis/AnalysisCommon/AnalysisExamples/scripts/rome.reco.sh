#! /usr/bin/env sh

export INDIR=/castor/cern.ch/user/d/drebuzzi/q02initialprod/dig9.0.4/forAOD

for FILENAME in `nsls $INDIR`; do
  bsub -q 8nh -R " mem > 600 " rome.reco.job $FILENAME $INDIR
done

#export FILENAME=q02initialprod.0001.H_2mu.q02dig.digits.0001.pool.root
#bsub -q 8nm -R " mem > 600 " rome.reco.job $FILENAME $INDIR

