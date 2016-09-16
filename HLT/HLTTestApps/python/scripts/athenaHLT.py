#!/bin/sh

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: athenaHLT.py 578197 2014-01-13 19:16:56Z ricab $
# Ricardo Abreu <ricardo.abreu@cern.ch>

"""true"

### Ugly hack to preload certain libraries
### (typically malloc)

python_path=`which python`

preload=`which hlt_test_apps_preload.sh`
if [ -z preload ]; then
    echo "hlt_test_apps_preload.sh was not found >&2"
else
    . "$preload"
fi

"exec" "$python_path" "-tt" "$0" "$@";

"""

from HLTTestApps import script_main
from HLTTestApps.application import file_based

if __name__ == '__main__':
  script_main(file_based)
  
