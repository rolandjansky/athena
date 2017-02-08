#!/usr/bin/python 
# Usage: python removeMainpages.py 
 
import os 
import sys 
 
for subdir, dirs, files in os.walk('.'): 
    # print subdir, dirs, files 
    mainpage_file = ''
    if 'mainpage.h' in files:
        mainpage_file='/mainpage.h'
    if 'MainPage.h' in files:
        mainpage_file='/MainPage.h'
    if mainpage_file!='':
        temp = subdir.split('/')
        package = temp[-2] 
        print 'Updating mainpage for package',package 
        
        old_file = open(subdir+mainpage_file, 'r') 
        new_file = open(subdir+'/packagedoc.h', 'w') 
        for line in old_file: 
            # Can definitely write this in a more elegant way!
            if '@mainpage' in line or '\mainpage' in line: 
                components = line.split() 
                newline = '@page '+package+'_page '+" ".join([str(i) for i in components[1:]]) + '\n'
            elif '@section' in line or '\section' in line: 
                components = line.split() 
                sectionName = components[1] 
                newline = '@section '+package+'_'+sectionName+' '+" ".join([str(i) for i in components[2:]]) + '\n'
            elif '@include requirements' in line or '@htmlinclude used_packages.html' in line:
                # these don't make sense any more so just remove
                continue
            else: 
                newline=line 
             
            new_file.write(newline)  
        old_file.close() 
        new_file.close()  
        os.remove(subdir+'/mainpage.h')
         
         
    # for cdir in dirs: 
    #     if cdir=='doc': 
    #         print 'checking dir ',subdir, 'files =',files 
    #         for f in files: 
    #             print f 
    #             if f.find('mainpage') > 0: 
    #                 print 'Found a mainpage with subdir,dirs=',subdir,dirs
