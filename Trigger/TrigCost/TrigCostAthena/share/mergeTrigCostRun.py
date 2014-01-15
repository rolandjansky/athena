#!/usr/bin/env python

from optparse import OptionParser
import os,commands,re
import sys
import time
from datetime import datetime
#import mergeCost
#
# Parse command line options
#
import TrigCostRate.TrigCostRateConfig as TrigCostConfig
p = TrigCostConfig.prepareRatesParser()
p.add_option( '--run',      type="string",       default="", dest="run",             help="run number for the reprocessing")
p.add_option( '--versn',    type="string",       default="", dest="version",         help="CAF reprocessing version")
p.add_option( '--InPathL2', type="string",       default="", dest="il2path",         help="Input castor Directory L2")
p.add_option( '--InPathEF', type="string",       default="", dest="iefpath",         help="Input castor Directory EF")
p.add_option( '--mergeType',type="int",          default=2,  dest="mergeType",       help="Type of merging: 2 merges RoIs and ROBs, 3 only TE, 4 all of those, 1 none of those.")
p.add_option( '--OutPath',  type="string",       default="", dest="opath",           help="Output castor Directory ")
p.add_option( '--clean',    action="store_true", default=False, dest="clean",        help="Clean the output directory first")
p.add_option( '--doit',     action="store_true", default=False, dest="doit",         help="Do it")
p.add_option( '--mkdir',    action="store_true", default=False, dest="mkdir",        help="make directory")

(opts, args) = p.parse_args()

#import os,commands
#--------------------------------------------------
def getCastorDirectoryList(path):
	cmd  = 'nsls %s/ ' % (path)
	if re.search('/eos/',path):
		cmd = "/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select ls %s " %(path)
		print 'Looking for files on eos: ',cmd
	file = ["%s" %i for i in commands.getoutput(cmd).split('\n')]
	return file

#--------------------------------------------------
def CheckFile(path,tfile):
	from ROOT import TFile
	print '%s/%s' %(path.rstrip('/'),tfile)
	f=None
	if path.count('eos/'):
		f = TFile.Open('root://eosatlas/%s/%s' %(path.rstrip('/'),tfile))
	elif path.count('castor/'):
		f = TFile.Open('root://castoratlas/%s/%s' %(path.rstrip('/'),tfile))
		
	if f: # and f.IsOpen() and not f.IsZombie():
		#f.Close()
		print 'OK'
		return True
	else:
		if path.count('eos/'):
			os.system('/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select ls %s/%s' %(path.rstrip('/'),tfile))
		elif path.count('castor/'):
			os.system('nsls %s/%s' %(path.rstrip('/'),tfile))
		#return False

	return True

#--------------------------------------------------
def CopyFileLocally(path,file):
	print '    path: ',path
	print '    file: ',file
	area=None
	if re.search('castor/',path):
		area='castor'
	if re.search('eos/',path):
		area='eos'
	if area==None:
		return path,file

	cmd='xrdcp root://'+area+'atlas/'+path.rstrip('/')+'/'+file+' /tmp/.' 
	print '   comd: ',cmd
	os.system(cmd)

	return '/tmp',file

if __name__=="__main__":            

	# set the input directories #169751 for HI
	l2p="/castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/data10_hi/physics_bulk/00"+opts.run+"/data10_hi.00"+opts.run+".physics_bulk.TrigRateL2.NTUP.c"+opts.version
	efp="/castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/data10_hi/physics_bulk/00"+opts.run+"/data10_hi.00"+opts.run+".physics_bulk.TrigRateEF.NTUP.c"+opts.version
	op="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.00"+opts.run+".physics_bulk.TrigRateAL.NTUP.c"+opts.version

	# Run 186923
	if len(opts.run)>0 and int(opts.run)>175000: 
		l2p="/castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/data11_7TeV/physics_EnhancedBias/00"+opts.run+"/data11_7TeV.00"+opts.run+".physics_EnhancedBias.TrigRateL2.NTUP.c"+opts.version
		efp="/castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/data11_7TeV/physics_EnhancedBias/00"+opts.run+"/data11_7TeV.00"+opts.run+".physics_EnhancedBias.TrigRateEF.NTUP.c"+opts.version
		op="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.00"+opts.run+".physics_EnhancedBias.TrigRateAL.NTUP.c"+opts.version
	if len(opts.il2path):
		l2p=opts.il2path
	if len(opts.iefpath):
		efp=opts.iefpath
	if len(opts.opath):
		op=opts.opath
	l2in = getCastorDirectoryList(l2p)
	efin = getCastorDirectoryList(efp)
	outal= getCastorDirectoryList(op)
	if opts.clean:
		os.system('rfmkdir '+op)
	if opts.mkdir:
		os.system('/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select mkdir -p '+op)
	for l2 in l2in:
		n=l2[l2.find('_CAF_')+5:l2.rfind('.root')]
		n=l2[l2.find('._')+2:l2.rfind('.root')]
		print "result string at 105: " 
		print n
		found=False
		out_file_name=''
		for o in outal:
			nout=o[o.find('_CAF_')+5:o.rfind('.root')]
			nout=o[o.find('._')+2:o.rfind('.root')]
			if nout==n:
				print "the file is found" 
				found=True
				out_file_name=o
		if found:
			print out_file_name
			#if CheckFile(op,out_file_name):
			continue

		for ef in efin:
			nef=ef[ef.find('_CAF_')+5:ef.rfind('.root')]
			nef=ef[ef.find('._')+2:ef.rfind('.root')]
			if nef==n:
				print "merge ",l2
				print "and ",ef
				num=ef.find('TRIGRATE')
				merg=ef[:num]+'TrigRateAL'+ef[num+10:]
				print 'Combined: ',merg
				l2p_temp,l2_temp = CopyFileLocally(l2p,l2)
				efp_temp,ef_temp = CopyFileLocally(efp,ef)
				l2_temp = l2p_temp.rstrip('/')+'/'+l2_temp
				ef_temp = efp_temp.rstrip('/')+'/'+ef_temp
				runcommand='source /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/Trigger/TrigCost/TrigCostAthena/share/submit_merge.sh '+l2_temp+' '+ef_temp+' '+merg+' '+op+' ' #+l2p_temp+' '+efp_temp+' '+opts.mergeType
				print "Run command: ",runcommand
				if opts.doit:
					os.system(runcommand)
	print "Dear Tae: Please send this path to the requestor: "+op
