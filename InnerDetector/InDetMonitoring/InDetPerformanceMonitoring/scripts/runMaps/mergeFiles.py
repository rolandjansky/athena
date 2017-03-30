#
# author: Steffen Henkelmann
# comment: quick solution
from __future__ import division
from subprocess import Popen,PIPE,STDOUT
import subprocess
import os,sys,json
import shutil
import ROOT as r


batchqueue="8nh"
jobname="MD07102016_2016"
filename="ZmumuValidationUserSel"
expectedNumberOfFiles = 7


def convert_bytes(num):
    """
    this function will convert bytes to MB.... GB... etc
    """
    for x in ['bytes', 'KB', 'MB', 'GB', 'TB']:
        if num < 1024.0:
            return "%3.1f %s" % (num, x)
        num /= 1024.0


def file_size(file_path):
    """
    this function will return the file size
    """
    if os.path.isfile(file_path):
        file_info = os.stat(file_path)
        return convert_bytes(file_info.st_size)



import glob
loc = os.environ['PWD']+"/"

os.chdir(str(loc)+'BatchJobOutput/')
os.chdir(str(loc)+"BatchJobOutput/jobs/" + str(batchqueue))


filesToMerge = []
fileLoc = os.getcwd()
#print fileLoc

for subdirs,dirs, files in os.walk(fileLoc):
	if str(jobname) in (subdirs) and dirs:
		for dir in dirs:
			fileL = str(subdirs) + "/"+str(dir) + "/" + str(filename) + ".root"
			if (fileL) != -1:
                            f = r.TFile.Open(fileL)
                            if f:
                                print 'INFO :: File exists and can be opened > ', fileL
				filesToMerge.append(fileL)
                            else:
                                print 'WARNING :: Something is wrong with the following file (job still running probably)\n'+str(fileL)
#		print filesToMerge, "  -- No. of files : ", len(filesToMerge)
if len(filesToMerge) == expectedNumberOfFiles:
    for file in filesToMerge:
	cmd = 'hadd -f -k ' + str(filename) + "_Merged_NoF-" + str(len(filesToMerge)) + ".root" + "  "
	if len(filesToMerge) == 0:
            print "WARNING :: Failed to find files"
            continue
	for i in filesToMerge:
            cmd = cmd + "  " + i
    print 'INFO :: command that will be executed : \n', cmd

    response = raw_input("\n QUESTION :: Do you want to continue and merge? >> : ").lower()
    yes = set(['yes','y', 'ye', 'YES','Yes','j','Ja','ja'])
    no = set(['no','n','NO','No'])
    if response not in yes:
        print "INFO :: You did not answer with yes ... exiting!"
        sys.exit()
    else:
        print "INFO :: Start merging"
        os.system(cmd)

else:
    print 'EXIT :: The number of expectedFiles is not equal to the number of finished files. Files that run through: ',len(filesToMerge), ' Files that were expected: ',expectedNumberOfFiles, ' ... exiting'
    sys.exit()
