#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import os
import datetime, time

# Function to iterate through dates between start and end in steps of delta. ~ish                                                                                                    
def perdelta(start, end, delta):
    curr = start
    while curr < end:
        yield curr, min(curr + delta, end)
        curr += delta

# Function to return required dates                                                                                                                                                       
def checkDates(datesFileName):
        #placeholder declarations                                                                                                                                                          
        fillerDate = datetime.datetime(2000,1,1,1,1,1,1)
        tempDate = datetime.datetime(2000,1,1,1,1,1,1)
        returnList =[fillerDate,fillerDate,fillerDate]
	tempRun = 0

        startingDates=[]
        endDates=[]

        #if no file containing previously used dates, create one with default values.                                                                                                
        #"No any file found!" is what my motherboard says whilst booting my home PC, I found it funny and copied it over here                                                        
        if not os.path.exists(datesFileName):
                print("No any file found! at " + datesFileName + ". Making default values")

                datesFile = open(datesFileName,'w')
                firstTempDate = datetime.datetime(2015,4,2,1,1,1,1)
                lastTempDate = datetime.datetime(2015,4,2,1,1,1)

                datesFile.write(str(tempRun) + " " + str(firstTempDate) + " " + str(lastTempDate) +"\n")
                startingDates.append(firstTempDate)
                endDates.append(lastTempDate)
                datesFile.close()

        #if dates file exists, read from it                                                                                                                                        
        else:
                print("Found " + datesFileName)
                datesFile = open(datesFileName,'r')

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
        return datesList

outPath = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/Lumi/"
outputName = outPath + "/runData.txt"
outputSummary = outPath + "/runs.txt"

inPath = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/rawData/Lumi/"

[startDates,endDates] = checkDates(outputSummary)

currentDay = datetime.date(2018,11,7)
firstDay = startDates[0].date()

fout = open(outputName,'a+')

if firstDay + datetime.timedelta(days=1)<currentDay:
	for s, e in perdelta( firstDay, currentDay, datetime.timedelta(days=1)):
            
            s_str = str(s).replace('-','_')
            e_str = str(e).replace('-','_')
            print(s_str)
            filename = inPath + s_str + "-" + e_str + ".txt"
            fin = open(filename,"r")
            
            i = -1
            for line in fin:
                i = i+1
                if i < 3: 
                    continue

                array=line.rstrip().split()

                startDateTime=array[0].split('/UTC')[0].split('[')[1]
                startDate = startDateTime.split('-')
                startTime = startDateTime.split(':')
                start = datetime.datetime(int(startDate[0]),int(startDate[1]),int(startDate[2].split(':')[0]),int(startTime[1]),int(startTime[2]),int(startTime[3]))

                endDateTime=array[1].split('/UTC')[0]
                endDate = startDateTime.split('-')
                endTime = startDateTime.split(':')
                end = datetime.datetime(int(endDate[0]),int(endDate[1]),int(endDate[2].split(':')[0]),int(endTime[1]),int(endTime[2]),int(endTime[3]))
                
                [run,lb] = array[2].split('/')
                fill = int(array[3])
                length = float(array[6])
                lumi = float(array[10])
                intlumi = length*lumi

                # Match Nick's output format
                fout.write(str(run) + " " + str(fill) + " " + str(lb) + " " + str(time.mktime(start.timetuple())) + " " + str(length) + " " + str(0) + " " + str(0) + " " + str(lumi) + " " + str(intlumi) + "\n")


fout.close()
            
summaryFile = open(outputSummary,'w')
summaryFile.write(str(run) + " " + str(startDates[0]) + " " + str(end) +"\n")


