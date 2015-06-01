#!/usr/bin/env python
# @file RecExCommon/recexcommon-links.py
# @purpose generic python script to install (symlink) files needed for
#          RecExCommon-based jobs to run properly

__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = """\
generic python script to install (symlink) files needed for
RecExCommon-based jobs to run properly.
"""

### imports -------------------------------------------------------------------
import sys
import os
import os.path as osp
import shutil

### functions -----------------------------------------------------------------
def find_joboption(fname):
    from AthenaCommon.Utils.unixtools import FindFile as _find_file
    from AthenaCommon.Include import optionsPath
    return _find_file(fname, optionsPath, os.R_OK)

def main():
    import PyUtils.Logging as L
    msg = L.logging.getLogger('recex-links')
    msg.setLevel(L.logging.INFO)
    msg.info('Setup links to all the necessary files to run RecExExample...')

    # configuration files
    fname = "RecExCommon/myTopOptions.py"
    msg.info("%s: all in one", fname)
    top_options = find_joboption(fname)
    msg.info("copying from [%s]", top_options)
    if osp.exists(osp.basename(top_options)):
        os.remove(osp.basename(top_options))
    shutil.copy(src=top_options, dst=osp.basename(top_options))

    fname = "jobOptions.py"
    if osp.exists(fname):
        os.remove(fname)
    os.symlink(osp.basename(top_options), "jobOptions.py")

    # poolfilecatalog
    pfc = "PoolFileCatalog.xml"
    if osp.exists(pfc):
        if osp.exists(pfc+"_save"):
            os.remove(pfc+"_save")
        os.rename(pfc, pfc+"_save")

    msg.info("run-time auxiliary files...")
    from subprocess import check_call
    # no longer set up a replica
    # check_call("setupLocalDBReplica_CERN.sh")
    #

    msg.info("now build [%s] file", pfc)
    newfile = '/afs/cern.ch/atlas/project/rig/referencefiles/RTTinputFiles/MC15_13TeV/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r6699_10evt.pool.root'
    if osp.isfile(newfile):
        # new file exists
        rdo_fname = newfile
    else:
        rdo_fname = osp.join(os.environ['ATLASTESTDATA'],
                             'mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.digit.RDO.e1513_s1499_s1504_d700_10evt.pool.root')
    check_call(["pool_insertFileToCatalog.py", rdo_fname])

    lfn_name = "top_default.pool.root"
    msg.info('registering [lfn:%s]...', lfn_name)
    check_call(["FCregisterLFN",
                "-p", rdo_fname,
                "-l", lfn_name])

    msg.info('Setup links to all the necessary files to run RecExExample... [done]')
    return 0

if __name__ == "__main__":
    sys.exit(main())
    
