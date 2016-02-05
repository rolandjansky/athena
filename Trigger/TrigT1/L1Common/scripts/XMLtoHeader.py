#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os.path
import sys
import getopt
import xml.etree.ElementTree as ET
from datetime import date

#--------------------------------------
#get command line arguments
#--------------------------------------
argv = sys.argv

inputXMLname = '../schema/L1CTSpecifications.xml'
outputNames = 'L1Common'

opts, remainder = getopt.getopt( argv[1:], 'i:o:', ['inputXML=','outputNames='])
#print "OPTIONS: ", opts
#print "REMAINDER: ", remainder

for opt,arg in opts:
    if opt in ['-i','--inputXML']:
        inputXMLname = arg
    elif opt in ['-o','--outputNames']:
        outputNames = arg
    else:
        print "option %s not known.." % opt
        
        
        
#-----------------------------------
# some definitions
#-----------------------------------

typeDictCpp = {'u32' :'uint32_t','u16':'uint16_t','s32' :'int32_t','s16':'int16_t','string':'string', 'bool':'bool','float':'float','double':'double'}
typeDictJava = {'u32' :'long','u16':'long','s32' :'int','s16':'int','string':'String', 'bool':'Boolean','float':'Single','double':'Double'}

versionDict = {}

if 'dataformat' in os.path.basename(outputNames):
    versionDict = {'v0':'Before moving the L1A position to the eformat-version field', 'v1': 'After moving the L1A position to the eformat-version field to free bits for the HLT counter', 'v2':'First version of CTP data with programmable extra words (up to 50), and time difference in BC to previous L1A', 'v3':'Including also the turn counter in the payload','v4':'Version with increased number of inputs/items for run-2 (->beginning 2015)'}#,'vZ': 'dummy for testing'}

if 'L1Common' in os.path.basename(outputNames):
    #versionDict = {'v0':'Before long shutdown I (before 2015)', 'v1': 'Version after long shutdown I','v2':'dummy','v3':'dummy','v4':'dummy'}
    versionDict = {'v0':'Before long shutdown I (before 2015)', 'v1': 'Version after long shutdown I'}
    
    
versionDict.keys().sort()
#print versionDict.keys()
message_missingElement = 'Concerning child #{number}: Sorry, no {element} specified. This is a necessary element. Please make sure to specify it in the config file.'



#----------------------
#Read the XML tree
#----------------------
tree = ET.parse(inputXMLname)
root = tree.getroot()

#---------------------------------------------
#Get all versions that exist for an element
#---------------------------------------------

nameVersionsDict={}
childIndex = 0
for child in root:
    childIndex +=1
    if child.find('name') is None:
        #print message_missingElement.format(number=childIndex, element='name')
        exit(1)
    elif child.find('name').text[1:-1] in nameVersionsDict.keys():
        nameVersionsDict[child.find('name').text[1:-1]].append(child.attrib['ns'])
    else:
        versionList = [child.attrib['ns']]
        nameVersionsDict[child.find('name').text[1:-1]] = versionList

#print nameVersionsDict
staticList = [] #parameters that didn't change so far
for name in nameVersionsDict.keys():
    if len(nameVersionsDict[name])==1:
        staticList.append(name)
    for vers in nameVersionsDict[name]:
        count = nameVersionsDict[name].count(vers)
        if count>1:
            #print 'ERROR: Element "',name,'" occurs several times with same version (' , vers, '). Please fix config file.'
            exit(1)


#------------------------------------------------------------------
#Get number of changed parameters compared to v0 for each version
#------------------------------------------------------------------
numChangeVersionDict = {}
numChangeVersionDict['v0'] = 0
changedParameterNames = []
for v in versionDict.keys()[1:]:
    childIndex = 0
    for child in root:
        childIndex +=1
        if child.attrib['ns'] is None:
            #print message_missingElement.format(number=childIndex, element='ns')
            exit(1)
        else:
            if child.attrib['ns']==v:
                if child.find('name').text[1:-1] not in changedParameterNames:
                    changedParameterNames.append(child.find('name').text[1:-1])

        numChangeVersionDict[v] = len(changedParameterNames)    




def CreateFiles(time):
#=====================================
# actually create the output files
#=====================================

    #--------- the definition of CTP version class--------------------------------
    cppBaseStartstring = "/*\n*This file was automatically created by XMLtoHeader.py\n* Created on:             {creationTime}\n* Created with config:    {xmlFile}\n*\n* Author: Ruth Poettgen\n* Contact: ruth.poettgen@cern.ch\n**Brief: Defines class for different CTP versions.\n*/\n\n\n#include <inttypes.h>\n#include <assert.h>\n#include <iostream>\n#include <stdlib.h>\n#include <map>\n#include <iomanip>\n#include <sstream>\n\n"
    start = cppBaseStartstring.format(creationTime = time, xmlFile = inputXMLname)

    baseNameRoot = os.path.basename(outputNames)
    baseName = baseNameRoot + 'Version'
    fileName = outputNames + 'Version'
    baseHeader = open(fileName+'.h','w')
    baseHeader.write(start)
    
    
    baseHeader.write('#ifndef '+baseName.upper() + '_H\n')
    baseHeader.write('#define '+baseName.upper() + '_H\n')
    baseHeader.write('\nclass '+baseName + '{\n\n')
    
    protectedString = "\nprotected:\n"
    protectedString += "    //---------------------------------------------------------------------------------------------------\n"
    protectedString += "    //  For explanation of the variables and there values in different versions see config XML file.\n"
    protectedString += "    //---------------------------------------------------------------------------------------------------\n"
    
    functionString = ''
    
    publicString = "\npublic: \n"
    publicString += '\n    ' + baseName + '(unsigned int version) {\n'
    
    dumpString = '\n    std::string dump() const {\n';
    dumpString += '\n        std::ostringstream s;\n';
    dumpString += '\n        std::ostringstream tmp;\n\n';
    dumpString += '        s << \"For a description of the parameters for different CTP version, see schema/L1CoreSpecifications.xml in L1CommonCore.\" << std::endl;\n\n';
    dumpString += '        s << \"|-------------------------------------------------------------|\" << std::endl;\n';
    dumpString += '        s << \"|                    Name                 |       Value       |\" << std::endl;\n';
    dumpString += '        s << \"|-------------------------------------------------------------|\" << std::endl;\n';
    
    #dumpString += '        s << \"|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\" << std::endl;\n';
    #dumpString += '        s << \"|                    Name                 |      Value                          |                                Comment                                                                                   |\" << std::endl;\n';
    #dumpString += '        s << \"|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\" << std::endl;\n';
    
    changedValuesDict = {}
    numberOfChanges = 0
    #--------------------------------------------
    #get start strings for the other files
    #--------------------------------------------

    pyStartstring = "#!/usr/bin/env python\n\n#This file was automatically created by XMLtoHeader.py\n# Created on:             {creationTime}\n# Created with config:    {xmlFile}\n# \n# Author: Ruth Poettgen\n# Contact: ruth.poettgen@cern.ch\n\n\n\nimport sys\n\nclass {classname}(object):\n    def __init__(self):\n"
    
    #javaStartstring = "/*\n*This file was automatically created by XMLtoHeader.py\n* Created on:             {creationTime}\n* Created with config:    {xmlFile}\n*\n* Author: Ruth Poettgen\n* Contact: ruth.poettgen@cern.ch\n**Brief: Defines some CTP parameters. {version}.\n*/\n\n\npackage {pack};\n\npublic class {classname} "
        
        
    #-------------------------------------------------------
    # write one file/one constructor part for each version
    #-------------------------------------------------------
    for v in versionDict.keys():
        
        if (v!='v0'):
            changedValuesDict[v] = []
        
        pyStartstring = pyStartstring.format(creationTime = time,version = versionDict[v], xmlFile = inputXMLname, classname=baseNameRoot)
        headerName = outputNames + "_" + v + ".py"
        pyHeader = open(headerName,'w')
        pyHeader.write(pyStartstring)
        
        #javaStartstring = javaStartstring.format(creationTime = time, xmlFile = inputXMLname,version = versionDict[v],pack='L1Common.'+ outputNames,classname=outputNames+'_'+v)
        #headerName = outputNames + "_" + v + ".java"
        #javaHeader = open(headerName,'w')
        #javaHeader.write(javaStartstring + '{\n\n')
        
        
        childIndex=0
        nameList=[]
        
        for child in root:
            hasChanged = False
            childIndex=childIndex+1
            #print 'looking at child #', childIndex
            
            if child.attrib['ns'] not in versionDict.keys():
                #print 'Version ID ', child.attrib['ns'], ' not recognised. Should be one of ', versionDict.keys()
                exit(1)
            
            #check if all the attributes necessary are set
            if child.find('name') is None:
                #print message_missingElement.format(number=childIndex, element='name')
                exit(1)
            if child.find('type') is None:
                #print message_missingElement.format(number=childIndex, element='type')
                exit(1)
            if child.find('value') is None:
                #print message_missingElement.format(number=childIndex, element='value')
                exit(1)
                
                
            #check wether parameter has already been written, don't write again
            if child.find('name').text[1:-1] in nameList:
                #print child.find('name').text[1:-1], ' already written. Skipping.'
                continue
            #else:
                #print 'Found new parameter: ', child.find('name').text[1:-1]
            
            
            #-------------------------------------------------------------
            #get latest version of parameter for the required CTP version
            #-------------------------------------------------------------
            
            if versionDict.keys().index( child.attrib['ns'] )>versionDict.keys().index(v):
                #print 'Version (' , child.attrib['ns'] , ') is beyond what we are looking for right now (' , v, ') . Skipping.' 
                continue
            
            tmp_v=v
            while tmp_v not in nameVersionsDict[child.find('name').text[1:-1]]:
                #print tmp_v, 'is not contained in ', nameVersionsDict[child.find('name').text[1:-1]]
                index = versionDict.keys().index(tmp_v)
                tmp_v=versionDict.keys()[index-1]
                #print '--> Going to write ' , tmp_v , ' instead'
            
            if tmp_v!='v0':
                hasChanged=True
            
            if child.attrib['ns']!=tmp_v:
                #print 'Version (' , child.attrib['ns'] , ') is not what we are looking for right now. Skipping.'
                continue
                
                
            #-------------------------------------------------------------
            # write the files
            #-------------------------------------------------------------
                
            typeText = child.find('type').text
            nameText = child.find('name').text[1:-1]
            valueText = child.find('value').text
            
                   
            pyHeader.write('        self.'+nameText.ljust(30)+'= ')
       
            if 'mult' in typeText:  #arrays need special treatment

                #javaHeader.write('    '+'public static ' + typeDictJava[ typeText[0:-5] ] +'[] ')
                #javaHeader.write(nameText.ljust(30)+'= ')
                #javaHeader.write(' { ' + valueText +' };')
                
                pyHeader.write( ('[ ' + valueText +  ' ]').ljust(50) )
                
                
                listOfElements = valueText.split(',')
                numberOfElements = len(listOfElements)
                
                if v=='v0':
                                        
                    protectedString += '    '
                    protectedString += typeDictCpp[typeText[0:-5]] + ' '
                    protectedString += (nameText+ '['+ str(numberOfElements) + ']').ljust(20)+ '='
                    protectedString += '{ ' + valueText + '};'
                
                
                    functionString += '\n    const '+typeDictCpp[typeText[0:-5]] + '* get'
                    functionString += nameText + '() const { return ' + nameText + '; }'
                    
                    
                    dumpString += '\n        s << \"|\" << std::left  << std::setw(40)  << \"  ' + nameText + '\" << \" | \";\n'
                    dumpString += '        tmp.clear(); tmp.str(\"\");\n        tmp<< \"[\";\n'
                    dumpString += '        for (int i=0; i <' + str(numberOfElements) + '; i++) {\n'
                    dumpString += '            if (i) tmp<<\",\";\n'
                    dumpString += '            tmp << this->' + nameText + '[i];\n        }\n        tmp<<\"]\";\n'
                    dumpString += '        s <<        std::right << std::setw(17)  << tmp.str()  << \" | \" << std::endl;\n\n'
                    #dumpString += '        s <<        std::right << std::setw(35)  << tmp.str()  << \" | \";\n'
                    #if child.find('comment') is None:
                     #   dumpString += '        s <<        std::left  << std::setw(120) << ' + '\"--\"' + '  << \" | \" << std::endl;\n\n'
                    #else:
                     #   dumpString += '        s <<        std::left  << std::setw(120) << ' + child.find('comment').text + '  << \" | \" << std::endl;\n\n'
                
                
               
                if hasChanged:
                    changedValuesDict[v].append([nameText,valueText])

            else:
                #javaHeader.write('    public static ' + typeDictJava[typeText] +' ')
                #javaHeader.write(nameText.ljust(30)+'= ')
                #javaHeader.write(valueText +';')
                
                if (valueText)[0] not in ['0','1','2','3','4','5','6','7','8','9']:
                    val = valueText
                    pyHeader.write( ('self.' + val.replace('+',' + self.')).ljust(10))
                else:
                    pyHeader.write(valueText.ljust(10))
                    
                
                if v=='v0':
                    functionString += '\n    '+typeDictCpp[typeText] + ' get'
                    functionString += nameText + '() const { return ' + nameText + '; }'
                    
                    protectedString += '    ' + typeDictCpp[typeText] + ' '
                    protectedString += nameText 
                    protectedString += ' = ' + valueText + ';'

                    dumpString += '        s << \"|\" << std::left  << std::setw(40)  << \"  ' + nameText + '\" << \" | \";\n'
                    dumpString += '        s <<        std::right << std::setw(17)  << this->'  + nameText +  '  << \" | \" << std::endl;\n\n'
                    #dumpString += '        s <<        std::right << std::setw(35)  << this->'  + nameText +  '  << \" | \";\n'
                    #if child.find('comment') is None:
                        #dumpString += '        s <<        std::left  << std::setw(120) << ' + '\"--\"' + '  << \" | \" << std::endl;\n\n'
                    #else:
                        #dumpString += '        s <<        std::left  << std::setw(120) << ' + child.find('comment').text + '  << \" | \" << std::endl;\n\n'

                  
                if hasChanged:
                    changedValuesDict[v].append([nameText,valueText])
                
               
        
            #--- comment -------------------------
            if child.find('comment') is None:
                pyHeader.write('\n')
                #javaHeader.write('\n')
            else:
                pyHeader.write( '    # ' + child.find('comment').text[1:-1] + '\n')
            if v =='v0':
                protectedString += '\n'
                            
            nameList.append(nameText)
    
    
        
    
        #------------------------------
        # finalise the file writing
        #------------------------------
  
        pyHeader.write('\n\n' + baseNameRoot + '_' + v + ' = ' + baseNameRoot + '()'+'\n\n' + 'del ' + baseNameRoot)
        #javaHeader.write('\n}') 
        
    dumpString += '        s << \"|-------------------------------------------------------------|\" << std::endl;\n\n';
    #dumpString += '        s << \"|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\" << std::endl;\n\n';
    dumpString += '        return s.str();\n\n    }'

    nVersions = len(versionDict.keys())
   
    #access function
    selection= '        if (version>=' + str(nVersions)+') {\n';
    selection+= '            std::cerr << \"ERROR: invalid CTP version requested (\" << version << \"). Should be in [0,'+ str(nVersions-1)+ '].Setting it to the latest version ('+str(nVersions-1)+'). \" << std::endl;\n'
    selection += '            version='+str(nVersions-1)+';\n'
    selection+= '        }\n\n'

    
    versionInt = 1
    for v in changedValuesDict.keys():
        selection += '        if (version==' + str(versionInt) + ' ) {\n'
        for pair in changedValuesDict[v]:
            if ',' in pair[1]:
                listOfValues = pair[1].split(',')
                numValues = len(listOfElements)
                selection+='\n'
                valIndex = 0
                for val in listOfValues:
                    selection+= '            ' + pair[0] + '[' + str(valIndex) + '] = ' + val + ';\n'
                    valIndex+=1
            else:
                selection+='            ' + pair[0] + ' =  ' +  pair[1] + ';\n'
            
        selection += '        }\n\n'
        versionInt+=1
        numberOfChanges = len(changedValuesDict.keys())
        
   
    baseHeader.write(protectedString+'\n\n')
    baseHeader.write(publicString +'\n\n')
    baseHeader.write(selection)
    baseHeader.write('    };\n\n')
    baseHeader.write(functionString+'\n\n')
    baseHeader.write(dumpString+'\n\n')
    baseHeader.write('\n};\n\n')
    baseHeader.write('#endif //' + baseName.upper() + '_H')

  
now = date.today().strftime("%d/%m/%y")
          
if __name__ == "__main__":
    CreateFiles(now)


