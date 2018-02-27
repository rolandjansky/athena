# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Helper functions for setting up a job options proxy
import os,shutil

def get_immediate_subdirectories(a_dir):
    return [name for name in os.listdir(a_dir)
            if os.path.isdir(os.path.join(a_dir, name))]

def mk_jo_proxy(targetbasepath, pkgname, proxypath, addtosearch=True):
    "Make a JO proxy dir such that the MCxxJobOptions/dddd dirs contents are found via include(MCxxJobOptions/yyyy)"
    if proxypath:
        if os.path.exists(proxypath):
            shutil.rmtree(proxypath)
        os.mkdir(proxypath)
    os.environ['LOCAL_INSTALL_DIR'] = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]
    os.environ['LOCAL_DATA_DIR'] = (os.environ['DATAPATH']).split(":")[0]

    dirlist =  get_immediate_subdirectories(targetbasepath)
    subdirlist=dirlist;
    for dd in (dirlist):
      if (('.svn' not in dd) and ('cmt' not in dd) and ('_joproxy' not in dd)):
        deepdir = os.path.join(targetbasepath, dd)
        subdirlist1 = get_immediate_subdirectories(deepdir)
        subdirlist = subdirlist+["%s" % dd+"/%s" % item for item in subdirlist1]
    for d in (subdirlist):
        # TODO: we could _maybe_ add the appropriate share/DSIDxxxx/ dir to the path based on the jobConfig arg... too much magic?
        if (('.svn' not in d) and ('cmt' not in d) and ('_joproxy' not in d) and ('share/' not in d)):
          dpath = os.path.join(proxypath, d)
          if proxypath:
                   os.mkdir(dpath)

          if ('nonStandard' in dpath):
                dpath_ex = os.path.join(dpath,pkgname)
                if proxypath:
                   os.mkdir(dpath_ex)

                os.symlink(os.path.join(targetbasepath, d), os.path.join(dpath_ex, "nonStandard"))
          else :
                os.symlink(os.path.join(targetbasepath, d), os.path.join(dpath, pkgname))
          if addtosearch:
            os.environ["JOBOPTSEARCHPATH"] = dpath+":"+os.environ["JOBOPTSEARCHPATH"]
            os.environ["DATAPATH"] =os.path.join(targetbasepath, d)+":"+os.environ["DATAPATH"]

    os.environ["JOBOPTSEARCHPATH"] = os.environ['LOCAL_INSTALL_DIR']+":"+os.environ["JOBOPTSEARCHPATH"]
    os.environ["DATAPATH"] = os.environ['LOCAL_DATA_DIR']+":"+os.environ["DATAPATH"]