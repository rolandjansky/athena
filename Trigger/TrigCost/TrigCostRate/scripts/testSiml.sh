
n=1000000
p=--prescaleXML=data/examplePS.xml
p=""

#$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py /castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/mc09_7TeV.105001.pythia_minbias.digit.e468_s624_s633_d238_28_04_2010_Phys1E31sL1C_AtlasP1HLT-15.5.6.11 -k TrigRate -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/costs/test-siml --test=siml $p -n $n

#$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py /castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/mc09_7TeV.105001.pythia_minbias.digit.e468_s624_s633_d238_28_04_2010_Phys1E31sL1C_AtlasP1HLT-15.5.6.11_ignorePrescales_False -k TrigRate -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/costs/test-rate --test=rate $p -n $n


#$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py /castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/mc09_7TeV.105001.pythia_minbias.digit.e468_s624_s633_d238_28_04_2010_Phys1E31sL1C_AtlasP1HLT-15.5.6.11 -k TrigRate -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/costs/test-pred-init --test=pred-init --ignore=always $p -n $n

#$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py /castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/mc09_7TeV.105001.pythia_minbias.digit.e468_s624_s633_d238_28_04_2010_Phys1E31sL1C_AtlasP1HLT-15.5.6.11 -k TrigRate -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/costs/test-pred-make --test=pred-make --ignore=always $p -n $n

$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py /castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/mc09_7TeV.105001.pythia_minbias.digit.e468_s624_s633_d238_28_04_2010_Phys1E31sL1C_AtlasP1HLT-15.5.6.11 -k TrigRate -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/costs/test-pred-pred --test=pred-pred --ignore=always $p -n $n --prescaleXML=data/examplePS.xml