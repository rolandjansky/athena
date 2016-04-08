#!/usr/bin/env python
__version__ = "$Revision: 1.1.1.1 $"
__author__      = "Ilija Vukotic <ivukotic@cern.ch>"

from cmd import Cmd as c
import sys

class browser(c):
    """Command line browser for performance data stored in AMI database."""
    prompt='>>>'
    intro = "Command line browser for performance data stored in AMI database."
    
    def __init__(self):
        c.__init__(self)
        
        try:
            from pyAMI.pyAMI import AMI
        except ImportError:
            print "WARNING unable to import AMI from pyAMI with standard $PYTHONPATH."
            sys.exit(0)
        except Exception, e:
            print 'WARNING: ', e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sys.exit(0)        
        
        self.amiclient=AMI()
        self.head='sql=SELECT '
        self.amiCommand=['SearchQuery','processingStep=utility','project=coll_sizes_tier0']  
        
        
        self.riVars=['mu','lumi','time','use','readyForPhysics','ext']
        
        self.debug=False
        self.table=''
        self.allvariables=[]
        self.variables=set()        
        self.cuts=set()
        self.format={'runNumber':0,'events':0,'use':0,'readyForPhysics':0}
        self.format.update({'mu':3,'lumi':3,'ext':3,'time':3})
        self.format.update({'RSS_MEAN':3,'EVTLOOP_CPU':3,'LEAK_51_MALLOC':3,'CPU_BMIPS':3,'OVERHEAD_WALL':3,'LEAK_11_50_MALLOC':3,'VMEM_MEAN':3,'JOBCFG_WALL':3,'VMEM_PEAK':3,'OVERHEAD_CPU':3,'EVTLOOP_WALL':3,'LEAK_51_VMEM':3,'TASKS':0})
        self.format.update({'cpuIni':3,'cpuFin':3,'cpuTime':3})
        self.format.update({'objectSize':3})
        self.format.update({ 'stream':-20, 'reproTag':-10, 'jobProcessingStep':-10,'algo_proc_step':-10,'algoName':-40,'algoCategoryName':-15,'dataType':-5,'objectName':-30,'category':-12})  
    
    
    def getTable(self):
        if self.table=='':
            print 'Did not define table to use. Please select a table to browse using command "table".'
        else: return self.table
    
    
     
    def execute(self, sel):
        comm=[]
        for i in self.amiCommand: comm.append(i)
        comm.append(sel)
        if self.debug: print comm
        result=(self.amiclient.execute(comm)).getDict()
        if 'Element_Info' in result.keys():
            return result['Element_Info']
        else: return []    
    
    
    
    def where(self):
        res=''
        for cut in self.cuts:
            cut=cut.replace('dataType','objectSize.dataType')
            res+= cut +' and '
        return res.rstrip('and ')
    
    
    def what(self):
        res=''
        for var in self.variables:
            if var=='dataType': var='objectSize.dataType'
            res+=var+','
        return res.rstrip(',')
    
        
    def fromTable(self, what, where):
        res = ' FROM ' + self.table+' '
        
        foundRI=False
        foundCAT=False
        
        if self.table=='objectSize':
            for v in self.riVars:
                if v in what or v in where:
                    foundRI=True
            for v in ['objectName','category']:
                if v in what or v in where:
                    foundCAT=True
                    
            if foundRI: res += ', runInfo '
            if foundCAT: res += ', object '
            if foundRI or foundCAT: 
                res += ' WHERE '
                if foundRI: 
                    res += 'objectsize.runNumber=runInfo.run '
                    if foundCAT: res += 'and '
                if foundCAT: res+= 'objectsize.objectFK=object.objectID and objectsize.datatype=object.datatype '
                
        if self.table=='jobPerformance':
            for v in self.riVars:
                if v in what or v in where:
                    foundRI=True
            if foundRI: res += ', runInfo  WHERE jobPerformance.runNumber=runInfo.run '
                
            
        foundNAM=False    
        foundCAT=False        
        if self.table=='algoPerformance':
            for v in self.riVars:
                if v in what or v in where:
                    foundRI=True
            if 'algoName' in what or 'algoName' in where:
                foundNAM=True
            if 'algoCategoryName' in what or 'algoCategoryName' in where:
                foundCAT=True
            
            if foundRI: res += ', runInfo '
            if foundNAM or foundCAT: res += ', algoref '
            
            if foundRI or foundNAM or foundCAT: 
                res += ' WHERE '
                if foundRI: 
                    res += 'algoperformance.runNumber=runInfo.run '
                    if foundNAM or foundCAT: res += 'and '
                if foundNAM: 
                    res+= 'algoperformance.algorefFK=algoref.algorefID '
                    if foundCAT: res += 'and '
                if foundCAT:
                    res += 'algoperformance.algorefFK=algoref.algorefID'

        if 'WHERE' not in res:
            if where!='': res += ' WHERE '
        else:
            if where!='':res += ' AND '
        return res
    
    
    def printRows(self):
        what  = 'count(*)' 
        where = self.where()
        iz    = self.fromTable(what, where)
        sel   = self.head + what + iz + where
        # print sel
        result=self.execute(sel)
        if 'row_1' in result.keys():
            rows=result['row_1']['COUNT(*)'] 
        print 'rows selected: ', rows
    
    
    def stat(self):
        #print self.variables
        for var in self.variables:
            if self.format[var]>0: continue
            what  = ' distinct '+ var
            where = self.where()
            iz    = self.fromTable(what,where)
            sel   = self.head + what + iz + where
            result=self.execute(sel)
            print '----- unique values of',var,'------', len(result)
            for v in result.values():
                print v[var]
        
    
        
                
    def show(self, rowsToShow, verbose=1):
        what  = self.what()
        where = self.where()
        iz    = self.fromTable(what,where)
        sel   = self.head + what + iz + where
        
        if rowsToShow>30000 and verbose==1:
            print 'that is too many rows to show'
            return
            
        if rowsToShow!=-1:
            if 'WHERE' not in sel: sel+= ' WHERE rownum < '+str(rowsToShow)
            else: sel+=' AND rownum < '+str(rowsToShow)
            
        result=(self.execute(sel))
        data=[]
        if verbose==1:
            for v in self.variables:
                print v, '   |    ', 
            print '\n==================================='
        
        for r in result:
            row=[]
            
            strrow=''
            for v in self.variables:
                if self.format[v]==0:
                    i= int(result[r][v])
                    row.append(i)
                    strrow+=repr(i).rjust(6)+' | '
                elif self.format[v]>0:
                    f = float(result[r][v])
                    row.append(f)
                    fs='{0:12.'+str(self.format[v])+'f} | '
                    s=fs.format(f)
                    strrow+= str(s)
                else:
                    s = str(result[r][v])
                    row.append(s)
                    strrow+=s.rjust(-self.format[v])+' | '
                    
            if verbose==1:
                print strrow
            
            data.append(row)
        return data
    

    
    
    
    def do_debug(self,line):
        """
        toggles debuging mode in which it prints out SQL queries given
        """
        if self.debug==True: self.debug=False
        else: self.debug=True
    
    
    def checkVariable(self):
        if len(self.variables)==0:
            print 'Please add one or more varibles you would like to browse using command "add".'
            print ' These are available: ', self.allvariables
            return -1
        return 1
    
    
    def do_add(self, line):
        """
        adds a column to browse.
        syntax: add <column name>
        to add multiple columns separate them by space. 
        to add all the columns use *
        """
        if not self.getTable(): return
        if line:
            words=line.split()
            for w in words:
                if w=='*':
                    for v in self.allvariables: self.variables.add(v)
                    print 'All the variables added.'
                    return
                if w in self.allvariables:
                    print 'column (', w, ') added'
                    self.variables.add(w)
                else: 
                    print 'Column', w, 'does not exist in this table'
                    print 'These are available: ', self.allvariables
        else: print "argument missing. please choose one or more of the following: ", self.allvariables
    
    
    def do_remove(self, line):
        """
        remove a column from browsing.
        syntax: remove <column name>
        to remove multiple columns separate them by space. 
        """
        if not self.getTable(): return
        if line:
            words=line.split()
            for w in words:
                if w in self.allvariables: 
                    if w in self.variables:
                        self.variables.remove(w)
                        print 'removed ',w
                    else:
                        print 'variable',w,'was not added so could not be removed.'
                else: print 'column', w, 'does not exist in this table'
        else: print "argument missing. please choose one or more of the following: ", self.allvariables
    
                    
    def do_addCut(self, line):
        """
        accepts a cut in the following ways:
        runnumber=180624
        runnumber between 180200 and 180700  
        stream='MinBias'
        stream='L1Calo' or stream='Muons'
        objectname like '%HLT%'
        (don't make a cut on more than one variable)
        """
        if not self.getTable(): return
        if line:
            for w in self.allvariables:
                if w in line:
                    self.cuts.add(line)
                    print 'cut ('+line+') added.'
        else:
            print 'please provide cut expression.'
    
        
    def do_removeCut(self, line):
        """
        removes a cut. accepts no arguments.
        """
        cuts=list(self.cuts)
        if len(self.cuts)==0:
            print "you don't have any cuts defined."
            return
        for e,c in enumerate(cuts):
            print str(e)+'.',c
        ind = int(raw_input('Please select number of the cut to remove: ' ))
        self.cuts.remove(cuts[ind])
    
       
    
    def do_show(self, line):
        """
        prints a table of requested colums
        optional parameter is number of rows to show. default is 10. 
        """
        if not self.getTable(): return
        if self.checkVariable()==-1: return
        rowsToShow=10
        if line:
            try:
                rowsToShow=int(line)
            except:
                print 'number is required.'
                return
        self.show(rowsToShow)
    
    
    def do_stat(self, line):
        """
        prints a number of data records selected 
        optionally table of unique records for all requested columns is printed.
        syntax: stat [uni]
        """
        if self.getTable():
            self.printRows()
            if line:
                self.stat()
    
    
    def do_table(self, choice):
        """ 
        select the table you would like to browse.
        syntax: table <obj>
        possibilities:
        <object>    - object sizes
        <alg>       - algorithm performances
        <job>       - job performances
        """
        if choice:
            if choice=='object': 
                table='objectSize'
                self.allvariables=['runNumber','stream','reproTag','dataType','events','objectSize','objectName','category']
            elif choice=='alg': 
                table='algoPerformance'
                self.allvariables=['runNumber','stream','reproTag','algo_proc_step','events','cpuIni','cpuFin','cpuTime','algoName','algoCategoryName']
            elif choice=='job': 
                table='jobPerformance'
                self.allvariables=['runNumber','stream','reproTag','events','jobProcessingStep','EVTLOOP_CPU','EVTLOOP_WALL','OVERHEAD_CPU','CPU_BMIPS','OVERHEAD_WALL','JOBCFG_WALL','VMEM_PEAK','VMEM_MEAN','RSS_MEAN','LEAK_51_VMEM','LEAK_51_MALLOC','LEAK_11_50_MALLOC','TASKS']
            else: 
                print "you choice not recognized! Please choose one of the following: 'object','alg','job'."
                return
        else: 
            print "argument missing. Please choose one of the following: 'object','alg','job'."
            return
        if table!='':
            self.allvariables+=self.riVars
            self.table=table
            self.variables=set()
            self.cuts=set()
            self.printRows()
    
    
    def do_export(self, line):
        """
        exports the data to a file.
        syntax: export <filename> [format]
        format - csv or pickle(default)  
        """
        if line:
            w=line.split()
            print 'exporting the data to file:', w[0]
            
            if len(w)==2 and w[1]=='csv':
                print 'writing CSV format'
                res=self.show(-1,0)
                import csv
                writer = csv.writer( open(w[0]+'.csv', 'wb'), delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
                writer.writerow(list(self.variables))
                for row in res: writer.writerow(row)
                
            else:
                print 'writing gzipped pickle'
                res=self.show(-1,0)  
                try:
                    import cPickle as pickle
                except:
                    import pickle
                import gzip
                fil = gzip.GzipFile(w[0]+'.pickle.gz', 'wb')
                pickle.dump(self.variables,fil,-1)
                pickle.dump(res, fil, -1)
                
        else: print "argument missing. Please provide the filename to export data to."
    
    
    def do_exit(self, line):
        return True
    


if __name__ == '__main__':
    browser().cmdloop()