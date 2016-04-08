
#Set up the environment and get COOL authorisations
echo "Setting up ATLAS release 15.6.9."
source ~/cmthome/setup.sh -tag=32,opt,15.6.9,gcc34,oneTest

run=$1

pushd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/i686-slc4-gcc34-opt

echo "Getting COOL authentications."
export CORAL_AUTH_PATH=$HOME/private

echo "PIXEL B-LAYER"
./PIX.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/pixelBlayer.xml $run \
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/pixelBlayer_$run

echo "PIXEL BARREL"
./PIX.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/pixelBarrel.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/pixelBarrel_$run

echo "PIXEL ENDCAP A"
./PIX.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/pixelEndcapA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/pixelEndcapA_$run

echo "PIXEL ENDCAP C"
./PIX.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/pixelEndcapC.xml $run 
#>& afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/pixelEndcapC_$run

echo "SCT BARREL"
./SCT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/sctBarrel.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/sctBarrel_$run

echo "SCT ENDCAP A"
./SCT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/sctEndcapA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/sctEndcapA_$run

echo "SCT ENDCAP C"
./SCT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/sctEndcapC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/sctEndcapC_$run

echo "TRT BARREL"
./TRT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/trtBarrel.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/trtBarrel_$run

echo "TRT ENDCAP A"
./TRT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/trtEndcapA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/trtEndcapA_$run

echo "TRT ENDCAP C"
./TRT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/trtEndcapC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/trtEndcapC_$run

echo "LAR EMBA"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larEMBA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larEMBA_$run

echo "LAR EMBC"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larEMBC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larEMBC_$run

echo "LAR EMECA"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larEMECA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larEMECA_$run

echo "LAR EMECC"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larEMECC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larEMECC_$run

echo "LAR FCALC"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larFCALA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larFCALA_$run

echo "LAR FCALA"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larFCALC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larFCALC_$run

echo "LAR HECA"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larHECA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larHECA_$run

echo "LAR HECC"
./LAR.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/larHECC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/larHECC_$run

#echo "TILE BARREL A"
#./TILE.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tileBarrelA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tileBarrelA_$run

#echo "TILE BARREL C"
#./TILE.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tileBarrelC.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tileBarrelC_$run

#echo "TILE ENDCAP A"
#./TILE.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tileEndcapA.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tileEndcapA_$run

#echo "TILE ENDCAP C"
#./TILE.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tileEndcapC.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tileEndcapC_$run

#echo "MDT Barrel A"
#./MDT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/mdtBarrelA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/mdtBarrelA_$run

#echo "MDT Barrel C"
#./MDT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/mdtBarrelC.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/mdtBarrelC_$run

#echo "MDT Endcap A"
#./MDT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/mdtEndcapA.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/mdtEndcapA_$run

#echo "MDT Endcap C"
#./MDT.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/mdtEndcapC.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/mdtEndcapC_$run

#echo "RPC Barrel A"
#./RPC.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/rpcBarrelA.xml $run 
##>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/rpcBarrelA_$run

#echo "RPC Barrel C"
#./RPC.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/rpcBarrelC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/rpcBarrelC_$run

#echo "CSC Endcap A"
#./CSC.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/cscEndcapA.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/cscEndcapA_$run

#echo "CSC Endcap C"
#./CSC.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/cscEndcapC.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/cscEndcapC_$run

#echo "TDQ L1 CALO"
#./TDQ.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tdqL1CAL.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tdqL1CAL_$run

#echo "TDQ L1 CTP"
#./TDQ.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tdqL1CTP.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tdqL1CTP_$run

#echo "TDQ L1 MUE"
#./TDQ.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tdqL1MUE.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tdqL1MUE_$run

#    echo "TDQ L1 MUB"
#    ./TDQ.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/tdqL1MUB.xml $run
#    #>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/tdqL1MUB_$run

echo "LUCID A"
./LCD.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/lucidA.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/lucidA_$run

echo "LUCID C"
./LCD.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/lucidC.xml $run 
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/lucidC_$run

echo "SOLENOID MAGNET"
./MAG.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/magnetSOL.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/Solenoid_$run

    echo "TOROID MAGNET"
    ./MAG.exe /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/scripts/magnetTOR.xml $run
#>& /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/logfiles/Toroid_$run
