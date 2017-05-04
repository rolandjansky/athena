# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jun 30, 2011

@author: galster
'''

import re

class XMLTag:
    def __init__(self):
        self.Clear()
        
    def Clear(self):
        self.__name = ""
        self.__attributes = {}
        self.__single = False
        self.__content = ""
        self.__children = []
        self.__childNames = {}
        
    
    __name = str()
    def GetName(self):
        return self.__name
    def SetName(self, val):
        self.__name = str(val)
        
    __attributes = dict()
    def GetAttribute(self, name):
        if self.__attributes.has_key(name):
            return self.__attributes[str(name)]
        else:
            return None

    def SetAttribute(self, name, val):
        self.__attributes[str(name)] = str(val)
    
    __single = bool()
    def GetIsSingle(self):
        return self.__single
    def SetIsSingle(self, value):
        self.__single = bool(value)
        if self.__single:
            self.__content = None
    
    __content = str()
    def GetContent(self):
        return self.__content
    def SetContent(self, val):
        self.__content = val
    
    __children = list()
    def GetChildren(self):
        return self.__children
    
    def CountChildren(self):
        return len(self.__children)
    
    def GetChildNumber(self, no):
        return self.__children[int(no)]
    
    
    
    __childNames = dict()
    def GetChildNames(self):
        return self.__childNames
    
    def GetChildNamed(self, name):
        siblings = self.GetChildrenNamed(name)
        if len(siblings):
            return siblings[0]
        else:
            return None
    
    def GetChildrenNamed(self, name):
        children = []
        for i in self.ChildNameToNumbers(name):
            children.append(self.GetChildNumber(i))
        return children
        
    
    def AddChild(self, tag):
        self.__childNames[len(self.__children)] = tag.GetName()
        self.__children.append(tag)

    
    def RemoveChildNumber(self, no):
        if self.__childNames.has_key(no):
            self.__childNames.pop(no)
    
    def RemoveChildrenNamed(self, name):
        for i in self.ChildNameToNumbers(name):
            self.RemoveChildNumber(i)
        
    
    def ChildNameToNumbers(self, name):
        return [k for k, v in self.__childNames.iteritems() if str(v).lower() == name.lower()]
    
    def FromFile(self, fileName):
        f = file(fileName)
        self.FromString("".join(f.readlines()))
    
    def FromString(self, string):
        patternSanitize = ">\\s+<"
        regexpSanitize = re.compile(patternSanitize)
        string = re.sub(regexpSanitize, "><", string)
        
        patternTag = "<(\\/?)(\\w+)([^>]*)>"
        regexpTag = re.compile(patternTag)
        
        patternStartTag = "<(\\w+)([^>]*)>"
        regexpStartTag = re.compile(patternStartTag)
        
        startTag = re.search(regexpStartTag, string)
        
        if startTag == None:
            self.SetContent(string)
        else:
            self.SetName(startTag.group(1))
            self.AttributesFromString(startTag.group(2))
            if len(startTag.group(2)):
                self.SetIsSingle(startTag.group(2)[-1] == "/")
            else:
                self.SetIsSingle(False)
            
            
            if self.GetIsSingle() == False:
                patternLikeTag = "<(\\/?)" + self.GetName() + "([^\\w])"
                regexpLikeTag = re.compile(patternLikeTag)

            
            
                contentBegin = startTag.end()
                contentEnd = -1
                depth = 0
                for tag in re.finditer(regexpLikeTag, string):
                    if tag.group(2) != "/":
                        if tag.group(1) == "/":
                            depth -= 1
                        else:
                            depth += 1
                    
                    if depth == 0:
                        contentEnd = tag.start()
                        break;
                        
                content = string[contentBegin:contentEnd]
                self.SetContent(content)
                
                depth = 0;
                childBegin = -1
                childEnd = -1
                for tag in re.finditer(regexpTag, content):

                    
                    isSingle = False;
                    if len(tag.group(3)):
                        isSingle = (tag.group(3)[-1] == "/")
                    
                    if isSingle and depth == 0:
                        child = XMLTag()
                        child.AttributesFromString(tag.group(3))
                        child.SetName(tag.group(2))
                        child.SetIsSingle(isSingle)
                        self.AddChild(child)
                
                    else:
                        if not isSingle:
                            if tag.group(1) == "/":
                                depth -= 1
                            else:
                                depth += 1
                        
                        if depth == 1 and childBegin == -1:
                            childBegin = tag.start()
                        
                        if depth == 0 and childEnd == -1:
                            childEnd = tag.end()
                            
                            childString = content[childBegin:childEnd]
                                
                            child = XMLTag()
                            child.FromString(childString)
                        
                            self.AddChild(child)
                            
                            childBegin = -1
                            childEnd = -1                            
        
        
    
    
    def AttributesFromString(self, string):
        if len(string):
            re.I = True
            patternAttrib = "(\\w*)\\=\\\"([^\\\"]*)\\\""
            regexpAttrib = re.compile(patternAttrib)
            for match in re.findall(regexpAttrib, string):
                self.SetAttribute(match[0],match[1]) 
            
    
    
    def ToString(self):
        string = str();
        if len(self.__name):
            string = "<" + self.__name
                        
            for name, value in self.__attributes.items():
                string += " " + name + "=\"" + value + "\""
            
            if self.__single:
                string += " />"
            else:
                string += ">"
                
                for child in self.__children:
                    string += child.ToString()
            
                string += "</" + name + ">"
                         

        else:
            string = self.__content
        
        return string