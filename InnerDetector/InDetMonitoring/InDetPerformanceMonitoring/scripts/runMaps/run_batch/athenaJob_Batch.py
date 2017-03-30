#!/python
import glob,os,sys
import time,datetime
#function definition
def chunks(l, amount):
        if amount < 1:
            raise ValueError('amount must be positive integer')
        chunk_len = len(l) // amount
        leap_parts = len(l) % amount
        remainder = amount // 2  # make it symmetrical
        i = 0
        while i < len(l):
            remainder += leap_parts
            end_index = i + chunk_len
            if remainder >= amount:
                remainder -= amount
                end_index += 1
            yield l[i:end_index]
            i = end_index

#inputFileDir = '/afs/cern.ch/user/s/sthenkel/eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00267360/data15_13TeV.00267360.express_express.recon.ESD.f597/'
#inputFileDir = '/afs/cern.ch/user/s/sthenkel/eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00267073/data15_13TeV.00267073.physics_Main.recon.DESD_ZMUMU.f594_m1440_f594/'
#inputFiles = glob.glob(str(inputFileDir)+'*')
InputData = sys.argv[1]#'Run_periodA.txt'

dsid = InputData.split('DSID-')[1].split('_')[0]

inputFiles = [line.rstrip('\n') for line in open(InputData)]
job=os.path.splitext(InputData.split('/')[1])[0]

#reducedList =[]
#for x in inputFiles[0:5]:
#	reducedList.append(x)

athenaVersion = 'AtlasProduction,20.7.7.9'
nJobs=1
#nJobs=5
#nJobs=len(inputFiles)
QUEUE = "atlasb1_long"
MEM_LIMIT="8000000"
#QUEUE = "8nh"
ts = time.time()
timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d-%H-%M')


#chunk = list(chunks(reducedList,nJobs))
chunk = list(chunks(inputFiles,nJobs))
counter = 0
#print inputFiles
for itemlist in chunk:
	loc = os.environ["PWD"]+"/"
#	loc = locToRunJobs
	testarea=os.environ["TestArea"]+"/"
	os.system("mkdir -p "+str(loc))
	os.system("mkdir -p "+str(loc)+"BatchJobHandler")
	os.system("mkdir -p "+str(loc)+"BatchJobOutput")
	os.system("mkdir -p "+str(loc)+"BatchJobHandler/batchscripts")
	os.system("mkdir -p "+str(loc)+"BatchJobHandler/batchscripts/"+str(job))
	os.system("mkdir -p "+str(loc)+"BatchJobHandler/LSF")
	os.system("mkdir -p "+str(loc)+"BatchJobHandler/LSF/"+str(job))
	os.system("mkdir -p "+str(loc)+"BatchJobHandler/LSF/"+str(job)+"/"+str(counter)+ "_" + str(dsid) )
	path = str(testarea)+"InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/"
	command = "athena.py "+path+"share/runzmumu_maps.py -c \"inputFiles = "+str(itemlist)+"\""
	jobname = "batch_script_"+str(job)+"_"+str(counter) + "_" + str(dsid)
	# create submission script
	script = open(str(loc)+"BatchJobHandler/batchscripts/"+str(job)+"/batch_script_"+str(job)+"_"+str(counter)+ "_" + str(dsid)+".lsf","w")
	script.write("#BSUB -J "+str(jobname)+" \n")
	script.write("#BSUB -o "+str(loc)+"BatchJobHandler/LSF/"+str(job)+"/"+str(counter)+ "_" + str(dsid) +"/"+str(jobname)+".log \n")
	script.write("#BSUB -e "+str(loc)+"BatchJobHandler/LSF/"+str(job)+"/"+str(counter)+ "_" + str(dsid) +"/"+str(jobname)+".err \n")
	script.write("#BSUB -q "+str(QUEUE)+"\n")
	script.write("#BSUB -M "+str(MEM_LIMIT)+"\n")
	script.write("\n")
	script.write("cd "+testarea+" \n")
	script.write("source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh \n")
	script.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh "+str(athenaVersion)+",here \n")

	script.write("cd "+str(loc)+" \n")
	script.write("mkdir -p "+str(loc)+"BatchJobOutput/jobs \n")
	script.write("mkdir -p "+str(loc)+"BatchJobOutput/jobs/"+str(QUEUE)+"/"+str(job)+"_"+str(timestamp)+"_OUT"+" \n")
	script.write("mkdir -p "+str(loc)+"BatchJobOutput/jobs/"+str(QUEUE)+"/"+str(job)+"_"+str(timestamp)+"_OUT"+"/"+str(counter)+ "_" + str(dsid)+" \n")
	script.write("cd "+str(loc)+"BatchJobOutput/jobs/"+str(QUEUE)+"/"+str(job)+"_"+str(timestamp)+"_OUT"+"/"+str(counter)+ "_" + str(dsid)+" \n")
	script.write(command+"\n")
	script.write("cd - \n")
	script.close()

	print "Sending job no.: "+str(counter)
	print '###########################################################'
	print command
	print '###########################################################'
	os.system("chmod 755 "+str(loc)+"BatchJobHandler/batchscripts/"+str(job)+"/"+str(jobname)+".lsf")
	os.system("bsub -q "+str(QUEUE)+" < "+str(loc)+"BatchJobHandler/batchscripts/"+str(job)+"/"+str(jobname)+".lsf")
	counter+=1
	print 'started chunk no: ', str(counter)
