export MYINDS="TXT.10248297._000705.tar.gz.1,TXT.14277219._000011.tar.gz.1"; export MYOUTDS=EVNT.10248297._000186.pool.root.1; export IRANDOM=1; export IRANDOM=1075${IRANDOM}; 
Gen_tf.py --ecmEnergy=13000 --jobConfig=/afs/cern.ch/work/s/schaffer/public/work-21.prophecy4f_mig/run/PhPy8EG_Prophecy/JobOptions/346695 --maxEvents=2000 --randomSeed=${IRANDOM} --outputEVNTFile=$MYOUTDS --inputGeneratorFile=$MYINDS


