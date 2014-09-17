echo "do 'source setup-Athena.sh'"
source setup-Athena.sh

echo "do 'source /afs/cern.ch/atlas/offline/external/GRID/DA/panda-client/latest/etc/panda/panda_setup.sh'"
source /afs/cern.ch/atlas/offline/external/GRID/DA/panda-client/latest/etc/panda/panda_setup.sh

echo "do '/afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup_py.sh'"
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup_py.sh

echo " "
echo "do 'voms-proxy-init -voms atlas'"
voms-proxy-init -voms atlas

echo " "
echo "do 'voms-proxy-info -all'"
voms-proxy-info -all
