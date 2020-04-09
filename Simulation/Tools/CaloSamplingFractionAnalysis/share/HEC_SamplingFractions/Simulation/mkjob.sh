#!/bin/sh
#
#  Create working directories with necessary files to run simulations:
#  - different primary particles
#  - different Z-positions of primary vertices (i.e. scans over HEC)
#  - 25 sub-jobs with 200 events
#
if [ $# != 1 ] ; then
   echo " *** Argument is missing:  Physics list  !!! *** "
   exit
fi
#
export PhLis=$1
#
export DIRM=$PWD
#
export SimDIR=$DIRM/10.1-$PhLis
mkdir -p $SimDIR
#
export GenDIR=/eos/user/k/kiryunin/HEC_SF/Generation
#
export ENER=e0100
#
#
cat >dum0.sh <<\end0 
#
#  Primary vertices
#
   ./dum1.sh $1 sc1  # scan over front HEC wheel
   ./dum1.sh $1 sc2  # scan over rear  HEC wheel
# 
end0
#
cat >dum1.sh <<\end1  
#
#  Loop over sub-jobs
#
   for (( is = 1 ; is <= 25 ; is ++ )) ; do
      ./dum2.sh $1"_"$ENER"_"$2 $is
   done   
#
end1
#
cat >dum2.sh <<\end2  
#
   echo " === Job: $1 $2 === "
#
#  Create working directory to run simulations and dump calibration hits
#
   if [ $2 -lt 10 ] ; then   
      subjb=0$2
   else
      subjb=$2
   fi
   njob=$(($2-1))
#   
   dir=$SimDIR/$1/$subjb
#
   if test -s $dir ; then
      echo " *** Directory $dir does exist !!! *** "
      exit
   fi        
#
   mkdir -p $dir
   cd $dir
#
   ln -s $GenDIR/$1/evgen.pool.root .   
#
   cp -i $DIRM/preInclude.VertexPosOff.py .
   cp -i $DIRM/addMCpart.py .
#
#  Prepare file to run simulations
#
   fsim=$DIRM/simul.sh
#
   seed0=5432100
   seed1=$((${seed0}+$njob))
   sed -e 's/default:'$seed0'/default:'$seed1'/' $fsim >dumm1
#
   skip0=0
   skip1=$((200*$njob))
   sed -e 's/skipEvents '$skip0'/skipEvents '$skip1'/' dumm1 >dumm2
#
   sed -e 's/PhysicsList/'$PhLis'/' dumm2 >simJob.sh
   rm -f dumm?
#   
   chmod 755 simJob.sh
#
#  Prepare file to dump calibration hits
#
   fdump=$DIRM/dumphit.sh
#
   sed -e 's/calohitD3PD_from_esd/'$subjb'/' $fdump >dumpJob.sh
#
   chmod 755 dumpJob.sh
#
#
   cd $DIRM
   echo " === made === "
#   
end2
#
chmod 755 dum?.sh
#
#  Primary particles 
#
./dum0.sh ele  # electrons
./dum0.sh pos  # positrons
./dum0.sh gam  # gammas
#
#
rm -f dum?.sh
#
exit
########################################################################
