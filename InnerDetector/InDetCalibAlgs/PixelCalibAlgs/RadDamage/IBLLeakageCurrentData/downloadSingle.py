#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 

# A script that re-downloads a single day's worth of data
# Very useful if something went wrong in the initial download
# Replace s and e with the desired start and end dates

import datetime
import os

url = 'http://atlas-ddv.cern.ch:8089/multidata/getDataSafely'
url2 = 'http://atlas-ddv.cern.ch:8089/multidata/downloadTxtData'


s = datetime.datetime(2016,12,30,0,0,0)
e = datetime.datetime(2016,12,31,0,0,0)

dataFolder = '/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/rawData/'
dataType = 'ENV_TT'
 
if not os.path.exists(dataFolder+dataType+'/'):
	os.mkdir(dataFolder+dataType+'/')
	
if not os.path.exists(dataFolder+dataType+'Stave/'):
	os.mkdir(dataFolder+dataType+'Stave/')

saveFileName2 = dataFolder+dataType+'/'+s.strftime("%Y_%m_%d") +'-' +e.strftime("%Y_%m_%d")+ '.txt'
if os.path.exists(saveFileName2):
	os.remove(saveFileName2)

saveFile = open(saveFileName2,'w')

staveString="stave"
for staveNumber in range (1,15):
	if staveNumber<10:
		staveString = "0" + str(staveNumber)
	else:
		staveString =  str(staveNumber)

	# Generate save file name. I save in format YYYY/MM/DD, so it is in alphabetical order.
	saveFileName = dataFolder+dataType+'Stave/'+s.strftime("%Y_%m_%d") +'-' +e.strftime("%Y_%m_%d")+ 'Stave'+ staveString + '.txt'
	if os.path.exists(saveFileName):
		os.remove(saveFileName)
			
	# Create wget command
	cmd = 'wget --post-data "queryInfo=atlas_pvssPIX, alias, LI_S' + staveString + '_A_M1_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_A_M2_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_A_M3_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_A_M4_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_C_M1_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_C_M2_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_C_M3_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_C_M4_' + dataType + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!" ' + url + ' --output-document='+ saveFileName
		
	# Execute wget command
	os.system(cmd)

	bloop = open(saveFileName,'r')

#	if (staveNumber!=1):
#		saveFile.write("!!!")
		
	for bloopLine in bloop:
		saveFile.write(bloopLine)
			
	bloop.close()
			
