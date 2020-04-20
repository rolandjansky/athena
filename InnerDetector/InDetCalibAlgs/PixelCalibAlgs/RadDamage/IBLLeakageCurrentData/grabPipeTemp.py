#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 

# Python script from Nick Dann
# Clean-ups for clarity and additional comments from Jennet Dickinson
# Search for JENNET to find where to replace file paths etc

# Comments from Nick:
# Python script to grab IBL DCS data from the ATLAS DCS data viewer.
# DO NOT USE THE MULTITHREADING FUNCTION!
# grabData('dataType') is the function you should call to download DCS data. Program works from inside CERN network
# There are some strings you'll have to change to point to locations on your file system. BE CAREFUL; no idea what this will do if it can't find the right file structure.
# Timezones might be an issue; I used a PC running on UTC to get around this; there's probably a better solution, but that's what I did

#!/usr/bin/env python3
import os
import time
import shutil
import subprocess
import datetime
import fileinput
import sys
import random
from os.path import expanduser
import threading
from datetime import date, timedelta

# Function to iterate through dates between start and end in steps of delta. ~ish
def perdelta(start, end, delta):
    curr = start
    while curr < end:
        yield curr, min(curr + delta, end)
        curr += delta

# Function to iterate through dates between end and start in steps of delta. ~ish
def perdeltadown(start, end, delta):
    curr = end
    while curr > start:
        yield max(curr - delta, start),curr
        curr -= delta

# Function to return list of all files in search folder path, sorted alphabetically
def findFiles (searchFolderPath):
	
	try:
		os.chdir(searchFolderPath)
	except IOError:
		print('No entries in ' + searchFolderPath)
		return -1
	
	todaysList=[]

	# SOMEHOW, this searches the searchFolderPath and returns a list of contained files in well... files.
	for src_dir, dirs, files in os.walk(searchFolderPath):
		
		# Parse through the files, appending the file name to a list of files.
		for file_ in files:
			
			sortedFile = os.path.join(src_dir, file_)
			todaysList.append(str(sortedFile))
		
		# Now we've got a list containing all files we want to add, sort it alphabetically and return them.
	todaysList.sort()
	return todaysList

# Function to return required dates
def checkDates(dataType,dataFolder):
	
	# Placeholder declarations
	fillerDate = datetime.datetime(2000,1,1,1,1,1,1)
	tempDate = datetime.datetime(2000,1,1,1,1,1,1)
	returnList =[fillerDate,fillerDate,fillerDate]

	startingDates=[]
	endDates=[]
	
        # Check: what dates have I already downloaded?
	# If no file containing previously used dates, create one with default values. 
	# "No any file found!" is what my motherboard says whilst booting my home PC, I found it funny and copied it over here
	if not os.path.exists(dataFolder+dataType+".txt"):
		print("No any file found! at " + dataFolder+dataType +" Making default values")

		datesFile = open(dataFolder+dataType+".txt",'w')
		firstTempDate = datetime.datetime(2015,1,1,1,1,1,1)
		lastTempDate = datetime.datetime(2015,1,1,1,1,1)

		datesFile.write(dataType + " " + str(firstTempDate) + " " + str(lastTempDate) +"\n")
		startingDates.append(firstTempDate)
		endDates.append(lastTempDate)
		
		datesFile.close()
                
	# If dates file exists, read from it
	else:
		print("Found " + dataFolder+dataType+".txt")
		datesFile = open(dataFolder+dataType+".txt",'r')

		for dateLine in datesFile:
			tempDatesLine = dateLine.split()
			firstTemp = tempDatesLine[1].split('-')
			lastTemp = tempDatesLine[3].split('-')

			firstTempTime = tempDatesLine[2].split(':')
			lastTempTime = tempDatesLine[4].split(':')

			firstTempTimes = firstTempTime[2].split('.')
			lastTempTimes = lastTempTime[2].split('.')

			if len(firstTempTimes)<2:
				firstTempTimes.append(0)
			if len(lastTempTimes)<2:
				lastTempTimes.append(0)

			firstTempDate = datetime.datetime(int(firstTemp[0]), int(firstTemp[1]), int(firstTemp[2]), int(firstTempTime[0]),int(firstTempTime[1]), int(firstTempTimes[0]), int(firstTempTimes[1]))
			lastTempDate = datetime.datetime(int(lastTemp[0]), int(lastTemp[1]), int(lastTemp[2]), int(lastTempTime[0]), int(lastTempTimes[0]),int(lastTempTimes[0]), int(lastTempTimes[1]))
			
			startingDates.append(firstTempDate)
			endDates.append(lastTempDate)

		datesFile.close()

	datesList=[startingDates,endDates]
	# Return start and end dates for each sensor
	return datesList

# Function to save data of type dataType in dataFolder for all sensors, for specified dates. Only used to HV_IMeas
def grabDataStave(dataType):

	# url is the line you're supposed to use. url2 is the line that works better.
	url = 'http://atlas-ddv.cern.ch:8089/multidata/getDataSafely'
	url2 = 'http://atlas-ddv.cern.ch:8089/multidata/downloadTxtData'

        # JENNET changed the output file path         
        defaultPath = os.path.expanduser('/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/')
        # The data is saved to dataFolder / dataType / moduleGroup / startDate-endDate.txt                 
        # This script will autogenerate folders for this.                          
        dataFolder = defaultPath + "/IBLData/rawData/"

	# Check top level directory exists, then make folder for HV I, T, HV V, LV I and.. actually should get LV V
	if not os.path.exists(dataFolder):
		os.mkdir(dataFolder)
	# For dataType in ['HV_IMeas', 'TModule','HV_VMeas','PP4LV']:
	if not os.path.exists(dataFolder+dataType):
		os.mkdir(dataFolder+dataType)

	[startDates,endDates] = checkDates(dataType,dataFolder)
	currentDay = date.today()
	firstDay = endDates[0].date()-timedelta(days=1)

#        firstDay = datetime.date(2018,3,7)
#        currentDay = datetime.date(2018,3,10)
	# Iterate through all data in below dates, with a time step of time delta. (So from 1/1/2016 - 15/5/2016, with one file per day). 	
		
	# Iterate through staves, modules and sides
	sensorNumber=0

	if firstDay+timedelta(days=1)<currentDay:
		for s, e in perdelta( firstDay, currentDay, timedelta(days=1)):
			if not os.path.exists(dataFolder+dataType+'/'):
				os.mkdir(dataFolder+dataType+'/')

			if not os.path.exists(dataFolder+dataType+'Stave/'):
				os.mkdir(dataFolder+dataType+'Stave/')

			saveFileName2 = dataFolder+dataType+'/'+s.strftime("%Y_%m_%d") +'-' +e.strftime("%Y_%m_%d")+ '.txt'
			if os.path.exists(saveFileName2):
				os.remove(saveFileName2)

			saveFile = open(saveFileName2,'w')
			introubleCount = []
			introubleMean=0

			staveString="stave"
                        staveLetter="stave"
                        letterDict = {1:'a',2:'b',3:'c',4:'d',5:'e',6:'f',7:'g',8:'h',9:'i',10:'j',11:'k',12:'l',13:'m',14:'n'}
			for staveNumber in range (1,15):
				if staveNumber<10:
					staveString = "0" + str(staveNumber)
				else:
					staveString =  str(staveNumber)
                                staveLetter = letterDict[staveNumber]
                                if staveNumber%2 == 1:
                                    staveLetter += '28'
                                else:
                                    staveLetter += '30'

				# Generate save file name. I save in format YYYY/MM/DD, so it is in alphabetical order.
				saveFileName = dataFolder+dataType+'Stave/'+s.strftime("%Y_%m_%d") +'-' +e.strftime("%Y_%m_%d")+ 'Stave'+ staveString + '.txt'
				if os.path.exists(saveFileName):
					os.remove(saveFileName)

                                dataType2 = dataType + staveLetter

				# Create wget command
				cmd = 'wget --post-data "queryInfo=atlas_pvssPIX, alias, LI_S' + staveString + '_A_' + dataType2 + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_C_' + dataType2 + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!" ' + url + ' --output-document='+ saveFileName

				# Execute wget command
				os.system(cmd)

				if (os.path.getsize(saveFileName) < 300): 
#i.e. if the file is small, download probably failed, try a different command

					sleepTime = random.randint(30, 62)
					time.sleep(sleepTime)
					os.remove(saveFileName)

                                        cmd = 'wget --post-data "queryInfo=atlas_pvssPIX, alias, LI_S' + staveString + '_A_' + dataType2 + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!atlas_pvssPIX, alias, LI_S' + staveString + '_C_' + dataType2 + ', ' + s.strftime("%d-%m-%Y") + ' 00:00, ' + e.strftime("%d-%m-%Y") + ' 00:01, , , , , ,no, , +2!" ' + url + ' --output-document='+ saveFileName

					# Execute wget command
					os.system(cmd)

					if (0 < os.path.getsize(saveFileName) < 300): 
# If this happens again, even url2 couldn't save us. You're gonna have to do stuff yourself

						os.remove(saveFileName)
						print ("bork bork at " + s.strftime("%Y_%m_%d") + " please sign off yourself!" )
						break

				bloop = open(saveFileName,'r')

				if (staveNumber!=1):
					saveFile.write("!!!")
				
				introuble = 0

				for bloopLine in bloop:
					saveFile.write(bloopLine)
					introuble+=1

				introubleMean+=introuble
				bloop.close()

				introubleCount.append(introuble)
				sleepTime = random.randint(30, 62)
				time.sleep(sleepTime) # Go to sleep so the DCS people don't yell at you

			breakThatStick = False

			if introubleMean > 30:
				for checker in range (0,14):
					if introubleCount[checker]<2:
# If a sensor's data is less than 2 entries, whilst there are more than 30 entries for the day.
						print ( "borkalork in stave " + str(checker+1) )
						breakThatStick=True

			if breakThatStick == True:
				print ("breaking now at " + s.strftime("%d-%m-%Y") )
				break

			saveFile.close()
			firstTempDate = min ( datetime.datetime(s.year, s.month, s.day,0,0,0,0), startDates[0] )
			startDates[0] = firstTempDate

			lastTempDate = max ( datetime.datetime(e.year, e.month, e.day,0,0,0,0), endDates[0])
			endDates[0] = lastTempDate

			datesFile = open(dataFolder+dataType+".txt",'w')
			
			datesFile.write(dataType + " " + str(firstTempDate) + " " + str(lastTempDate) +"\n")

			datesFile.close()
			
			sleepTime = random.randint(10, 30)
			time.sleep(sleepTime)

################################################################################  
# PROGRAM STARTS HERE #                                                           
################################################################################  
def main():

#    grabData('HV_VMeas')
#    grabData('PP4LV')

    # grabData no longer works reliably for HV_IMeas, use grabStaveData instead
    # Use this if grabData returns an error about very big DB request
#    grabDataStave('HV_IMeas')
#    grabDataStave('TModule')
    grabDataStave('ENV_TT')

if __name__ == "__main__":
    main()
