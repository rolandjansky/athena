#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Python script from Nick Dann                                  
# Clean-ups for clarity and additional comments from Jennet Dickinson
# Search for JENNET to find where to replace file paths etc     
# Nick set this up multithreaded, but Jennet prefers not to run this way,
# so she can more easily see what is going on

# Comments from Nick:
# Multithreaded python script which finds files downloaded from the ATLAS DDV, reads in the data, then saves the data in seperate files for each DCS group.
# Files from DDV should be in format YYYY_MM_DD-YYYY_MM_DD.txt
# I save to SENSORNAME.ssv (space seperated variable). I usually save times as YYYY-MM-DD HH:MM:SS UTC timestamp value. Times are CERN time (CEST or CET).

# scanDataThread is the thread declaration
# scanData (dataType) is the function which you should call to start the searching; datatype should be a string.

#!/usr/bin/env python 
import os
import shutil
import subprocess
import datetime, time
import fileinput
import sys
import random
from os.path import expanduser
import threading

def sensorToNumber (stave,side,module): #converts from IBL stave, senors and side to DCS group number (<112)
	

	number = 8 * (int(stave)-1) + module
	if side == 'C':
		number+=4
	return number-1

def findFiles (searchFolderPath): #find all files in searchFolderPath, return as list
	
	try:
		os.chdir(searchFolderPath)
	except IOError:
		print('No entries in ' + searchFolderPath)
		return -1

	todaysList=[]

	#SOMEHOW, this searches the search Folder and returns a list of contained files in well... files.
	for src_dir, dirs, files in os.walk(searchFolderPath):
		#i parse through the files, I think. For each, I check if there's a reference to it in the 
		#diary entry for the day. If not, I go ahead and append the figure to the relevent diary entry.
		for file_ in files:
			sortedFile = os.path.join(src_dir, file_)
			todaysList.append(str(sortedFile))
		#now we've got a list containing all files we want to add, sort it alphabetically and add them.
	todaysList.sort()
	return todaysList

#Function that actually reads in data from fileName, saves it to... somewhere. Probably dataFolder/datatype/modulename.ssv . It also returns
#stuff like the most recent dates for all DCS groups (endDates), and number of datapoints found for each DCS group (tempDataNumbers).
#Probably says something if it can't read the data as well. Gonna be honest, DDV is a bit of a nightmare and I've spent too long messing
#around with it to remember what my bodges are.
def appendEntries(fileName,dataType,dataFolder,dateEntries,largestDates,firstDate,lastDate,borkDate):
	
	try:
		rawFiles = open(fileName,'r')
	except IOError:
		print ("could not open file " + str(fileName) + " fool")
		return firstDate  #So this probably crashes if filename can't be opened. Try not to let that happen.

	returnList = [firstDate,largestDates,borkDate,dateEntries,firstDate]

	print(firstDate)

	index_Jennet = 0
	for dataLine in rawFiles: #read in each line of fileName and do things to it. Does this work with empty files? I'm guessing so.
		
		print(index_Jennet)
		index_Jennet = index_Jennet + 1
		#We actually have two types of data file from DDV, one containing lots of commas, one which doesn't. The len(commasplit) thing is how I
		#differentiate between the two types.

		if len(dataLine)>5:
			commaSplit = dataLine.split(',')

			if '!!!!' in dataLine: #Our entry is blank, leg it!
				return returnList

			if len(commaSplit)<2:

				#file is from DownloadTxtData. Erm, I'm not gonna comment this, rough idea is split tempLines, 0th element is the sensor name, 
				#grab the DCS flags from the name, use it to make teh module name. If that module name is unexpected, break and report an error

				tempLines = dataLine.split()
				name = tempLines[0]
				A = name[4:6]
				B = name[7]
				C = name[10]
				
				moduleName = 'LI_S' + str(A) +'_'+ str(B) + '_M' + str(C)

				moduleNumber = int(8 * (int(A)-1) + int(C)-1)
				if B =='C':
					moduleNumber +=4
	
				if (name!=moduleName+"_"+dataType) or (len(tempLines)<3) : #This section is true if the data is bad; break and return
					returnList[2]  = lastDate
					print ("\nBORK BORK BORK! Error in data values of " + fileName + " for " + dataType + " borkDate " + str(returnList[2]) )
					print ("First line reads " + dataLine+"\n")
					print ("should read " + moduleName+"_"+dataType+"\n")
					
					borkRepork = open(dataFolder+dataType+"BORKBORKBORK.txt",'w')
					borkRepork.write("filename " + fileName + "date range " + str(firstDate) + "-" + str(lastDate) + " first line " + dataLine + '\n')
					borkRepork.close()
					return returnList
				
				outName = dataFolder+dataType+'/' + moduleName +'.ssv' #save file name made here!
				
				date = tempLines[1].split('-')
				time1 = tempLines[2].split(':')
				data = 0

				if dataType == 'TModule': #Oh yeah, DDV switched to reporting absolute values to magnitudes for bias and current at some point.
					data = tempLines[3]
				else: #if current or bias, only use absolute values, instead of y'know, signed values. Stupid DDV.
					data = abs(float(tempLines[3]))
				dateTime = datetime.datetime(int(date[2]), int(date[1]), int(date[0]), int(time1[0]), int(time1[1]), int(time1[2]),int(time1[3]))

				#This checks if the values are within the expected range, if not the below is true and it borks and returns.
				if (dateTime>lastDate+datetime.timedelta(hours = 2)) or (dateTime<firstDate -datetime.timedelta(hours = 2)) :
					returnList[2]  = lastDate
					print ("\nBORK BORK BORK! Error in date of " + fileName + " for " + dataType + " borkDate " + str(returnList[2]) )
					print ("First line reads " + dataLine + "\n")
					print ("should read " + moduleName+"_"+dataType+"\n")
					
					borkRepork = open(dataFolder+dataType+"BORKBORKBORK.txt",'w')
					borkRepork.write("filename " + fileName + "date range " + str(firstDate) + "-" + str(lastDate) + " first line " + dataLine + '\n')
					borkRepork.close()
					return returnList

				
				if dateTime>returnList[1][moduleNumber]: #if the end  time for data is bigger than the previous max value for this DCS group. I think
					returnList[3][moduleNumber]+=1

					#format the line for saving to the finished file area he clunked clunkily. Change this if you wanna change the output format.
					outputLine = moduleName+'_'+dataType + " " + str(dateTime) + " " +  str( time.mktime(dateTime.timetuple()) ) + " " + str(data) + "\n"

					returnList[1][moduleNumber] = dateTime

					#check if output file already exists, if not create it, if so append previous file.
					if os.path.isfile(outName) == False:
					
						output = open(outName,'w')
						output.write(outputLine)
						output.close()
					else: #AI=='11' and BI =='A' and CI == '1':
						output = open(outName,'a')
						output.write(outputLine)
						output.close()
				
			else:
			#from getDataSafely, format here splits data based on !!! between DCS groups.
				
				moduleList = dataLine.split('!!!')

				for module in moduleList: #Hah, look at me using accurate labels for my variables! 

					elements = module.split(',')
					name = elements[0]
					A="A"
					B="A"
					C="A"
					if len(name)>9:
					#print (name)
						A = name[4:6]
						B = name[7]
						C = name[10]
					
					moduleName = 'LI_S' + str(A) +'_'+ str(B) + '_M' + str(C)
					
					try:

						moduleNumber = int(8 * (int(A)-1) + int(C)-1)
						if B =='C':
							moduleNumber +=4

						outName = dataFolder+dataType+'/' + moduleName +'.ssv'

						#check if file exists, make it if not, append it if so.
						if os.path.isfile(outName) == False:
							output = open(outName,'w')
						else: 
							output = open(outName,'a')

						position=0

						#I'm like 90% sure each element is a single time (so, time and sensor reading). This makes this process slow.
						for element in elements:

							if position!=0: #the first element is the DCS group name; not hella useful.
								
								tempLines = element.split()
								date= tempLines[1].split('-')
								time1 = tempLines[2].split(':')
								data = 0

								#Make sure we use absolute values for current and bias.
								if dataType == 'TModule':
									data = tempLines[0]
								else:
									data = abs(float(tempLines[0]))

								dateTime = datetime.datetime(int(date[2]), int(date[1]), int(date[0]), int(time1[0]), int(time1[1]), int(time1[2]),int(time1[3]))

								#true if data stuff
								if (dateTime>lastDate+datetime.timedelta(hours = 2)) or (dateTime<firstDate -datetime.timedelta(hours = 2)) :
									returnList[2]  = lastDate
									print ("\nBORK BORK BORK! Error in date of " + fileName + " for " + dataType + " borkDate " + str(returnList[2]) )
									print ("First line reads " + dataLine + "\n")
									print ("Should read " + moduleName+"_"+dataType+"\n")
									
									borkRepork = open(dataFolder+dataType+"BORKBORKBORK.txt",'w')
									borkRepork.write("filename " + fileName + "date range " + str(firstDate) + "-" + str(lastDate) + " first line " + dataLine + '\n')
									borkRepork.close()
									return returnList

								
								if dateTime>returnList[1][moduleNumber]: #if time in right range, output.
									returnList[3][moduleNumber]+=1
									outputLine = moduleName + "_" + dataType + " " + str(dateTime) + " " + str( time.mktime(dateTime.timetuple()) ) + " " + str(data) + "\n"

									returnList[1][moduleNumber] = dateTime

									output.write(outputLine)



							position+=1
							
						output.close()

					except: #we failed at something, so print out an error message and run away screaming
						returnList[2]  = lastDate
						print("Something broke :( \n")
						print("Could be an error in data values of " + fileName + " for " + dataType + " borkDate " + str(returnList[2]) )
						print("First line reads " + name + "\n")

						borkRepork = open(dataFolder+dataType+"BORKBORKBORK.txt",'w')
						borkRepork.write("filename " + fileName + "date range " + str(firstDate) + "-" + str(lastDate) + " first line " + dataLine + '\n')
						borkRepork.close()
						print("Try running again... often this is an issue with the connection to eos")
						return returnList

	return returnList


def scanData (dataType):

	#home directory definition
	fillerDate = datetime.datetime(2000,1,1,1,1,1,1)
	tempDate = datetime.datetime(2000,1,1,1,1,1,1)
	returnList =[fillerDate,fillerDate,fillerDate,False,"LI_S00_0_M0"]
	# JENNET setsfile paths
	homeDirectory =  os.path.expanduser('/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/')

	#define path to folder
	dataFolder = homeDirectory + "/IBLData/processedData/"
	inputFolder = homeDirectory + "/IBLData/rawData/"+dataType+"/"
	entriesFolder = homeDirectory + "/IBLData/rawData/entriesPerDay/" + dataType + "/"
	
	#make directories if they don't exist
	if not os.path.exists(dataFolder+dataType):
		os.mkdir(dataFolder+dataType)
		
	if not os.path.exists(entriesFolder):
		os.mkdir(entriesFolder)

	#declare some stuff, so it's in scope when we need it
	startingDates=[]
	endDates=[]
	filesProcessed=[]
	dataPoints=[]
	dailyPoints=[]
	smallestEndDate=fillerDate
	

	#This section checks to see if there's a file containing the dates we've worked up till. 
	if not os.path.exists(dataFolder+dataType+".txt"):

		#No dates file found, so create one. 
		print("No any file found! at " + dataFolder+dataType +" Making default values")
		#set default max and min values for each sensor

		datesFile = open(dataFolder+dataType+".txt",'w')
		firstTempDate = datetime.datetime(2015,5,1,0,0,0,0)
		lastTempDate = datetime.datetime(2015,5,1,0,0,0,1)
		smallestEndDate = lastTempDate

		for stave in range(1,15):
			staveString = str(stave)
			if stave<10:
				staveString="0"+str(stave)
			for side in ['A','C']:
				for DCSGroup in range(1,5):
					moduleName = 'LI_S' + str(staveString) + '_' + side + '_M' + str(DCSGroup)
					datesFile.write(moduleName +" " + str(firstTempDate) + " " + str(lastTempDate) +" 0 0\n")
					startingDates.append(firstTempDate)
					endDates.append(lastTempDate)
					filesProcessed.append(0)
					dataPoints.append(0)
					dailyPoints.append(0)

		datesFile.close()

	else: #dates file exists, so read dates for each DCS group
		print("Found " + dataFolder+dataType+".txt")
		datesFile = open(dataFolder+dataType+".txt",'r')

		holder=0		

		for dateLine in datesFile: #read each line in, each line should correspond to one DCS group

			tempDatesLine = dateLine.split()
			filesProcessed.append(int(tempDatesLine[5]) )
			dataPoints.append( int( tempDatesLine[6]) )
			dailyPoints.append(0)

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
			lastTempDate = datetime.datetime(int(lastTemp[0]), int(lastTemp[1]), int(lastTemp[2]), int(lastTempTime[0]), int(lastTempTime[1]),int(lastTempTimes[0]), int(lastTempTimes[1]))
			
			startingDates.append(firstTempDate)
			endDates.append(lastTempDate)

			if holder==0:
				returnList[0] = firstTempDate
				returnList[1] = lastTempDate
				smallestEndDate=lastTempDate

			
			else:
				if firstTempDate<returnList[0]:
					returnList[0] = firstTempDate
				if lastTempDate>returnList[1]:
					returnList[1] = lastTempDate
				if lastTempDate < smallestEndDate:
					smallestEndDate = lastTempDate


			holder+=1

		datesFile.close()
		print ("Investigating " + dataType + " from " + str(smallestEndDate))

	
	holder = 0

	#call function to return list of all files in input folder
	fileList = findFiles(inputFolder)

	firstTempDate = startingDates[0]
	lastTempDate = endDates[0]
	numberFiles = len(fileList)
	fileNumber = 0

	#iterate through all files from file list, opening them if they're in the time period of interest.
	for fileName in fileList:

#		print(fileName)

		end = len(fileName)
		endDate1 = fileName[end-4:end]
		endDate2 = fileName[end-7:end-6]

		if endDate1=='.txt' and endDate2=='_': #check file ends with text, and contains underscore in expected place; Could make this more rigorous
			
			tempDataNumber = 0
			startDate = fileName[end-23:end-15]
			endDate = fileName[end-12:end-4]
			endDateSplit = endDate.split('_')
			endDateFile = datetime.datetime(2000+int(endDateSplit[0]),int(endDateSplit[1]),int(endDateSplit[2]),0,0,0,1)

			startDateSplit = startDate.split('_')
			startDateFile = datetime.datetime(2000+int(startDateSplit[0]),int(startDateSplit[1]),int(startDateSplit[2]),0,0,0,1)
			
			if endDateFile > smallestEndDate: #data from region of interest
				#APPEND FILES DOING THINGS HERE
				
				lastTempDate = endDateFile
				[firstTempDate,endDates,fillerDate,tempDataNumbers,smallestEndDate] = appendEntries(fileName,dataType,dataFolder,dailyPoints,endDates,startDateFile,endDateFile,fillerDate)
				#append entries called here. Editing of data files done at that location.

				for i in range(0,112): #112 DCS groups for IBL, if you're doing something else, change that number.
					
						filesProcessed[i] +=1 #number of files 
						dataPoints[i]+=tempDataNumbers[i] #number of data points for each DCS group
						
				holderX=0

				#this does something. Probably recording number of data points per day for each DCS group
				for stave in range(1,15):
					staveString = str(stave)
					if stave<10:
						staveString="0"+str(stave)
					for side in ['A','C']:
						for DCSGroup in range(1,5):
							moduleName = 'LI_S' + str(staveString) + '_' + side + '_M' + str(DCSGroup)
							outName = entriesFolder + moduleName + ".txt"
							dataLine = str(startDate) + " " + str(tempDataNumbers[holderX]) + "\n"
							tempDataNumbers[holderX]=0

							if os.path.isfile(outName) == False:
								output = open(outName,'w')
								output.write(dataLine)
								output.close()

							else: 
								output = open(outName,'a')
								output.write(dataLine)
								output.close()
							holderX +=1

				#check if the list is bork-a-dorked. If borked, save and break
				if returnList[2]!=fillerDate:
					returnList[2] = fillerDate
					datesFile = open(dataFolder+dataType+".txt",'w')
					tempHolder=0
					for stave in range(1,15):
						staveString = str(stave)
						if stave<10:
							staveString="0"+str(stave)
						for side in ['A','C']:
							for DCSGroup in range(1,5):
								moduleName = 'LI_S' + str(staveString) + '_' + side + '_M' + str(DCSGroup)
								datesFile.write(moduleName +" " + str(startingDates[tempHolder]) + " " + str(endDates[tempHolder]) +" " + str(filesProcessed[tempHolder])  + " " + str ( dataPoints[tempHolder] ) + "\n")
								tempHolder+=1

					datesFile.close()
					

					return returnList

		sys.stdout.flush()
		fileNumber+=1

	#open file, go through lists, if value < smallest, set to that,

	#if number bigger, set to that
	#if any value smaller than previous largest value, bork bork!

	holder+=1

	datesFile = open(dataFolder+dataType+".txt",'w')
	tempHolder=0
	for staveX in range(1,15):
		staveStringX = str(staveX)
		if staveX<10:
			staveStringX="0"+str(staveX)
		for sideX in ['A','C']:
			for DCSGroupX in range(1,5):
				
				moduleNameX = 'LI_S' + str(staveStringX) + '_' + sideX + '_M' + str(DCSGroupX)

				datesFile.write(moduleNameX +" " + str(startingDates[tempHolder]) + " " + str(endDates[tempHolder]) +" " + str(filesProcessed[tempHolder]) + " " + str ( dataPoints[tempHolder] )  +"\n")
				
				tempHolder+=1

	datesFile.close()
	returnList[3]=True

	return returnList

class scanDataThread (threading.Thread):
	def __init__(self, threadID, name):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name

	def run(self):
		print ("Starting getIBLDate.py for " + self.name)
		returnList = scanData(self.name)
		print ( "Exiting getIBLDate.py for " + self.name + " with no issues, data range from " + str(returnList[0]) + " to " + str(returnList[1]) )

################################################################################  
# PROGRAM STARTS HERE #                                                           
################################################################################  
def main():

	thread1 = scanDataThread (1,'HV_VMeas' )
	thread1.start()

        thread2 = scanDataThread (2,'PP4LV' )
        thread2.start()

        thread3 = scanDataThread (3,'HV_IMeas' )
        thread3.start()

        thread4 = scanDataThread (4,'TModule' )                                              
        thread4.start()    

if __name__ == "__main__":
	main()
