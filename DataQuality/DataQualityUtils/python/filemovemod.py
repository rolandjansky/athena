# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def move_files(prefix, config):
    import os, shutil
    
    filemap = config.filemap
    if filemap == {}:
        return
    for src, dest in filemap.items():
        if os.access(src, os.R_OK):
            try:
                shutil.copy2(src, os.path.join(dest, prefix + '_' + os.path.basename(src)))
            except IOError, e:
                print e
