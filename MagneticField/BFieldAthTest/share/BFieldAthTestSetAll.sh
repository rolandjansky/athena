#
 echo "Starting BFieldAthTestSetAll.sh"
#
#
#B field
 ln -fs /afs/cern.ch/atlas/offline/data/BmagAtlas02.data fieldmap.dat
#
#Take joboptions files 
 cp $BFIELDATHTESTROOT/share/BFieldAthTest_jobOptions.txt jobOptions.txt 
#
#Stagein one tape 
 stagein -h atlas003d -M /castor/cern.ch/atlas/tape/Y01628/Y01628.1.al -p atlas_stage mu100Pt
 ln -fs mu100Pt ZEBRA.P
#stagein -h atlas003d -M /castor/cern.ch/atlas/project/dc1/simul/data/001103/dc1.001103.simul.0001.test.mu_minus_20.zebra  -p atlas_stage mu20Pt
#ln -fs mu20Pt ZEBRA.P
#
