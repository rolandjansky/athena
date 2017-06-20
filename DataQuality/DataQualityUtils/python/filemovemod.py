# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def move_files(prefix, config):
    import os, shutil, subprocess
    
    filemap = config.filemap
    if filemap == {}:
        return
    for src, dest in filemap.items():
        print src, dest
        if os.access(src, os.R_OK):
            if dest.startswith('root://'):
                try:
                    subprocess.check_call(['xrdcp', src, os.path.join(dest, prefix + '_' + os.path.basename(src))])
                except subprocess.CalledProcessError, e:
                    print e
            else:
                try:
                    shutil.copy2(src, os.path.join(dest, prefix + '_' + os.path.basename(src)))
                except IOError, e:
                    print e
                
