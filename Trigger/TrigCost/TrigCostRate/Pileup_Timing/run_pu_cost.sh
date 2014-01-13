mu=$1
user=`whoami`
OUTDIR=/afs/cern.ch/user/t/trigcomm/w0/rates/${user}/testarea/GenTopTriggerMC

cd $OUTDIR
source $OUTDIR/setup_pro.sh
#echo `pwd`
cd $TestArea/Trigger/TrigCost/TrigCostRate

#echo "Copying file over from trigcomm:"
#echo "cp /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/run_${mu}/TrigCostAL_mu${mu}.root /tmp/schae/."

cp $OUTDIR/run_${mu}/TrigCostAL_mu${mu}.root /tmp/$user/.

echo "python $TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py /tmp/schae/TrigCostAL_mu$mu.root -k TrigCost -o /tmp/schae/cost_html_mu$mu -r /tmp/schae/cost_mu$mu.root --test='RoiCpuPu' --do-time --rosXML $TestArea/Trigger/TrigCost/TrigCostRate/data/rob-ros-2011.xml &> /tmp/schae/cost_mu$mu.log &" #tail -f /tmp/schae/cost_mu$mu.log"

python $OUTDIR/Trigger/TrigCost/TrigCostRate/share/runRates.py /tmp/schae/TrigCostAL_mu$mu.root -k TrigCost -o /tmp/schae/cost_html_mu$mu -r /tmp/schae/cost_mu$mu.root --test="RoiCpuPu" --do-time --rosXML $OUTDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-2011.xml 

#echo "cp /tmp/schae/cost_mu$mu.root /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/run_${mu}/."
cp /tmp/$user/cost_mu$mu.root $OUTDIR/run_${mu}/.
#echo "Finished!!"