# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def findMin(path)

minimum=10000
epochWithMinimum=-1


import os
command = "cp "+path+" ."
os.system(command)
os.system('source cleanFile.sh')


for line in file('chron.txt'):
	line = line.split()
	epoch = int(line[0])
	trainingError = float(line[1])
	testError = float(line[2])   
	if testError < minimum:
		minimum = testError
		epochWithMinimum = epoch
		
return epochWithMinimum
		




