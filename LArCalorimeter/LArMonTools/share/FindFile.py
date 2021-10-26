#!/usr/bin/python

import os

def FindFile(pathenvvar, subdir="",filename=""):
    if os.environ.has_key(pathenvvar):
        path=os.environ[pathenvvar]
    else:
        print("Environment Variable",pathenvvar,"not found.")
        return ""
    
    spath=path.split(":")
    for thedir in spath:
        if not subdir == "": 
            dir=thedir+"/"+subdir
        else:
            dir=thedir	
        if os.path.isdir(dir):
            files=os.listdir(dir)
            for file in files:
                if file==filename:
                    print('FindFile: File',filename,'found')
                    return dir+"/"+filename
    return ""		
     
