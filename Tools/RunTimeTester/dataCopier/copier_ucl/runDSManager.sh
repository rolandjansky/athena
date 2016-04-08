#! /bin/bash
set +e
source /home/en/RTT/setupDQ2env.sh
source /usr/local/lcg/etc/profile.d/grid_env.sh
/usr/local/bin/python2.3 /home/en/RTT/dataCopier/src/DataSetManager.py  /home/en/RTT/dataCopier/dsm_cfg_nightly.xml