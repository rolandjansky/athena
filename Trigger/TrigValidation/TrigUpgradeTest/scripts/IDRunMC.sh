get_files -data caloRoIData.dat
source RecExCommon_links.sh
athena.py --threads=1 ViewAlgsTest/BasicMTTrackingTrigger.py
