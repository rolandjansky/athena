# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############################################################################################
# @file: Script to extract information for previously uploaded runs in the COOL DB.
#
# @Author: Peter Vankov <peter.vankov@cern.ch>
#
###########################################################################################

import os,sys,time,glob,fnmatch, string, datetime
from time import gmtime, strftime
import urllib,urllib2
from Ft.Xml.Domlette import NonvalidatingReader

#############################################################################################
### --> Definitions:
#############################################################################################
def StartDate(period):
    ##--> "period" is in days
    start_date = (datetime.date.today() - datetime.timedelta(period)).isoformat()
    return start_date

#############################################################################################
def ConvertedRunNumber(rNumber):
    RNumber = long(rNumber)
    return RNumber << 32

#############################################################################################
def RunListInDBTotal(start_date, end_date, file_name, string_pattern):
    prefix="http://atlas-service-runinformation.web.cern.ch/atlas-service-runinformation/cgi-bin/runnumbers.py?"
    url=prefix+"start.date="+start_date+"%2012:06:00&end.date="+end_date+"%2012:06:00&format=html"
    # print url
    request=urllib2.Request(url)
    u=urllib2.urlopen(request)
    DocString = u.read()
    
    DocString_1 = DocString.replace ( '<td>', ' ' )
    DocString_2 = DocString_1.replace ( '</td>', ' ' )
    DocString_3 = DocString_2.replace ( '<tr>', ' ' )
    DocString_4 = DocString_3.replace ( '</tr>', ' ' )
    DocString_5 = DocString_4.replace ( '<th>Name</th><th>Number</th><th>Start At (UTC)</th><th>Duration</th><th>Partition</th>', ' ' )

    ## create temporary file
    f=open(file_name, 'w')
    f.write(DocString_5)
    f.close()
    
    Runs = []
    if os.path.exists(file_name):
        f = open(file_name,'r') 
        for line in f.readlines(): 
            if line.find(string_pattern)>= 0:
                ar=string.split(line)
                Runs.append(int(ar[1]))
        f.close()
    else:
        print "-->ERROR: athena_checkUpload: Can't find input file !"

    ## delete the temporary file
    os.remove(file_name)

    return Runs

#############################################################################################
def channelValueQuery(server, schema, dbname, folder, iov0, iov1, tag, channels):
    timespan= str(iov0) + '-' + str(iov1)
    prefix='http://voatlas11.cern.ch:8080/cooldb/'
    d = '/'
    url = prefix+server+d+schema+d+dbname+folder+'/timespan/'+timespan+'/tag/'+tag+'/channels/'+channels
    httpHeaders={'Accept':'text/xml'}
    textData=None
    # print url
    request=urllib2.Request(url,textData, httpHeaders)
    u=urllib2.urlopen(request)
    xmlDocString = u.read()
    return xmlDocString

#############################################################################################
def RunListInDB(array, server, schema, dbname, folder, tag, channels):
    Array_runsInDB=[]
    if (len(array) != 0):
        for i in range(len(array)):
            runNumber=array[i]
            iovSince=ConvertedRunNumber(runNumber)
            iovUntil=ConvertedRunNumber(runNumber+1)-1

            derived_string=channelValueQuery(server, schema, dbname, folder, iovSince, iovUntil, tag, channels)

            derived=NonvalidatingReader.parseString(derived_string,uri="dummy")
            numNoisyModules=derived.xpath(u'count(//channel)')

            if numNoisyModules !=0.0:
                Array_runsInDB.append(runNumber)
    else:
        Array_runsInDB.append(-999)

    return Array_runsInDB
     

#############################################################################################
def GetNumberOfNoisyModulesInDB(array, server, schema, dbname, folder, tag, channels):
    Array_numNoisyModules=[]
    if (array[0] != -999):
        for i in range(len(array)):
            runNumber=array[i]
            iovSince=ConvertedRunNumber(runNumber)
            iovUntil=ConvertedRunNumber(runNumber+1)-1

            derived_string=channelValueQuery(server, schema, dbname, folder, iovSince, iovUntil, tag, channels)
            
            derived=NonvalidatingReader.parseString(derived_string,uri="dummy")
            numNoisyModules=derived.xpath(u'count(//channel)')
            if numNoisyModules !=0.0:
                Array_numNoisyModules.append(numNoisyModules)
    else:
       Array_numNoisyModules.append(-999) 

    return Array_numNoisyModules

#############################################################################################
def GetNumberOfNoisyStripsInDB(array, server, schema, dbname, folder, tag, channels):
    Array_numNoisyStrips = []
    if (array[0] != -999):
        for i in range(len(array)):
            runNumber=array[i]
            iovSince=ConvertedRunNumber(runNumber)
            iovUntil=ConvertedRunNumber(runNumber+1)-1

            derived_string=channelValueQuery(server, schema, dbname, folder, iovSince, iovUntil, tag, channels)
 
            derived=NonvalidatingReader.parseString(derived_string,uri="dummy")
            numNoisyModules=derived.xpath(u'count(//channel)')
            if numNoisyModules !=0.0:

                allDefectStripsList=(derived.xpath(u"//channel/value[@name='DefectList']"))
                numNoisyStrips=0
                numNoisyStripsAdds=0

                for strips in allDefectStripsList:
                    words=strips.firstChild.data.split()
                    for j in range(len(words)):
                        jk=words[j]
                        if jk.find("-")>=0:
                            sep=jk.replace ( '-', ' ' )
                            sep1=sep.split()
                            numNoisyStripsAdds=numNoisyStripsAdds+int(sep1[1])-int(sep1[0])
                        
                    numNoisyStrips=numNoisyStrips+len(strips.firstChild.data.split())

                Array_numNoisyStrips.append(numNoisyStrips + numNoisyStripsAdds)

    else:
        Array_numNoisyStrips.append(-999)
        
    return Array_numNoisyStrips

#############################################################################################
def GetNoisyModulesAverageInDB(array):
    sum=0
    if (len(array) != 0):
        for j in range(len(array)):
            sum=sum+array[j]
            NoisyModulesAverage = sum/len(array)
    else:
        NoisyModulesAverage = -999     
    return NoisyModulesAverage 
    
#############################################################################################
def DisplayInfoForRunsInDB(runs, noisy_mods, noisy_strips):
    if (len(runs) != 0):
        for i in range(len(runs)):
            print "Run: ",runs[i], " --> Num. noisy modules: ", noisy_mods[i], " --> Num. noisy channels: ", noisy_strips[i]
    else:
        print "--> No information found."

#############################################################################################
