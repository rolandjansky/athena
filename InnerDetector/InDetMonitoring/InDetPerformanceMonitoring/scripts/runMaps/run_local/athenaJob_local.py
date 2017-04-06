#!/python
import glob,os,sys
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


InputData = sys.argv[1]#e.g.: 'Run_periodA.txt'
inputFiles = [line.rstrip('\n') for line in open(InputData)]
print str(inputFiles)

NumberOfFiles = 1
reducedList =[]
for x in inputFiles[:NumberOfFiles]:
       reducedList.append(x)
NoOfChunks = 1

chunk = list(chunks(reducedList,NoOfChunks))
#chunk = list(chunks(inputFiles,NoOfChunks))
counter = 0
path = "$TestArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/"
for itemlist in chunk:
	command = "athena.py "+path+"share/runzmumu_recommendations.py -c \"inputFiles = "+str(itemlist)+"\""
#	command = "athena.py "+path+"share/runzmumu_maps_refitEnabled_readPoolEnabled.py -c \"inputFiles = "+str(itemlist)+"\""
	print command
	loc = os.path.abspath('.')
	os.system("mkdir "+str(counter))
	os.chdir(str(counter))
	os.system(command)
	os.chdir(loc)
	counter+=1
#	print len(chunk),str(itemlist),len(itemlist)
	print 'started chunk no: ', str(counter)
