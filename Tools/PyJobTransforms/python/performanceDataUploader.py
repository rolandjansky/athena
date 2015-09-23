# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.performanceDataUploader
#  @brief Upload job performance stats to AMI


__all__ = ('timelimited', 'TimeLimitExpired','PerformanceUploader')
__version__ = '5  2011-07-24'

from threading import Thread


try:  # UGLY! private method __stop
    _Thread_stop = Thread._Thread__stop  
except AttributeError:  # _stop in Python 3.0
    _Thread_stop = Thread._stop  


class TimeLimitExpired(Exception):
    '''Exception raised when time limit expires.
    '''
    pass


def timelimited(timeout, function, *args, **kwds):
    
    class _Timelimited(Thread):
        _error_  = TimeLimitExpired  # assume timeout
        _result_ = None
        
        def run(self):
            try:
                self._result_ = function(*args, **kwds)
                self._error_ = None
            except Exception, e:  #XXX as for Python 3.0
                self._error_ = e
        
        def _stop(self):
             # UGLY! force the thread to stop by (ab)using
             # the private __stop or _stop method, but that
             # seems to work better than these recipes
             # <http://code.activestate.com/recipes/496960/>
             # <http://sebulba.wikispaces.com/recipe+thread2>
            if self.isAlive():
                _Thread_stop(self)
        
    
    
    if not hasattr(function, '__call__'):
        raise TypeError('function not callable: %s' % repr(function))
        
    if timeout is None:  # shortcut
        return function(*args, **kwds)
        
    if timeout < 0:
        raise ValueError('timeout invalid: %s' % repr(timeout))
        
    t = _Timelimited()
    t.start()
    t.join(timeout)
    
    if t._error_ is None:
        return t._result_
    
    if t._error_ is TimeLimitExpired:
        t._stop()
        raise TimeLimitExpired('timeout %r for %s' % (timeout, repr(function)))
    else:
        raise t._error_


import os
import sys


class PerformanceUploader:
    
    def __init__(self, production):
        self.isConnected=0
        self.connection=None
        self.production=None

        fn=os.getenv('TZAMIPW')
        if fn is not None:
            print "upload will be done using direct db access."
            return None

        print "upload will be done using AMI procedures."
        if production:
            self.amiclient=False
            try:
                from pyAMI.exceptions import AMI_Error
                from pyAMI.client import AMIClient
                self.amiclient = AMIClient()
                self.production=production
            except ImportError:
                print "WARNING unable to import AMI from pyAMI with standard $PYTHONPATH."
            except Exception, e:
                print 'WARNING: ', e
                print sys.exc_info()[0]
                print sys.exc_info()[1]
            except:
                print "Unexpected error:", sys.exc_info()[0]
    
    
    def __del__(self):
        self.closeConnection()
    
    
    def establishConnection(self):
        if self.isConnected==True:
            return True
        
        try:
            import cx_Oracle
        except ImportError:
            print 'major fault: cx_Oracle not found.'
            return False
            
        fn=os.getenv('TZAMIPW')
        
        if fn is None:
            print 'Performance data will not be uploaded to AMI'
            return False
            
        with open(fn, 'r') as f:
            for line in f:
                try:
                    kori = line[:line.index('/')] 
                    lozi = line[line.index('/')+1:line.index('@')]
                    serv = line[line.index('@')+1:]
                    self.connection = cx_Oracle.connect(kori,lozi,serv,threaded=True)   
                    print 'connection created.'
                except cx_Oracle.DatabaseError, exc:
                    error, = exc.args
                    print "performanceDataUploader.py - problem in establishing connection to AMI db"
                    print "performanceDataUploader.py Oracle-Error-Code:", error.code
                    print "performanceDataUploader.py Oracle-Error-Message:", error.message
                    continue
                except:
                    print "Unexpected error (establishConnection 1st step):", sys.exc_info()[0]
                    continue
                
                try:    
                    self.cursor = cx_Oracle.Cursor(self.connection)     
                    self.isConnected=True
                    # print 'cursor created.'
                    return True
                except cx_Oracle.DatabaseError, exc:
                    error, = exc.args
                    print "performanceDataUploader.py - problem in creating cursor"
                    print "performanceDataUploader.py Oracle-Error-Code:", error.code
                    print "performanceDataUploader.py Oracle-Error-Message:", error.message
                except:
                    print "Unexpected error (establishConnection second step):", sys.exc_info()[0]
                
                self.connection.close()
                self.connection=None
                print 'connection closed'
                    
        return False
    
    
    def closeConnection(self):
        if self.isConnected==True:
            try:
                self.cursor.close()
                self.connection.close()
                self.isConnected=False
            except cx_Oracle.DatabaseError, exc:
                error, = exc.args
                print "performanceDataUploader.py - problem in closing connection to AMI db"
                print "performanceDataUploader.py Oracle-Error-Code:", error.code
                print "performanceDataUploader.py Oracle-Error-Message:", error.message
            except:
                print "Unexpected error (closeConnection):", sys.exc_info()[0]
    
    
    def uploadDataSize(self, format, runnr, stream, tag, ne, collectionSize):
        
        if self.production:
            self.AMIuploadDataSize(format, runnr, stream, tag, ne, collectionSize)
            return
        if self.establishConnection()==False:
            return
        try:
            self.cursor.callproc('CREATETAGANDSTREAM',[tag, stream])
            self.connection.commit()
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error (createTagAndStream):", sys.exc_info()[0]
        
        L=[]
        for obj in collectionSize.keys():
            L.append([ne, runnr, collectionSize[obj], format, stream, tag, obj ])
        try:    
            insert = "INSERT INTO tempobjectsize ( events, runnumber, objectsize, datatype, stream, reprotag, objectname) VALUES (:1, :2, :3, :4, :5, :6, :7)"
            self.cursor.prepare(insert)
            self.cursor.executemany(None, L)
            print 'Inserted: ' + str(self.cursor.rowcount) + ' rows.'
            
            self.connection.commit()
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error:", sys.exc_info()[0]
        
        print 'performanceDataUploader.py: Collection sizes uploaded to AMI.'
    
    
    def AMIuploadDataSize(self, format, runnr, stream, tag, ne, collectionSize):
        L=[]
        
        for obj in collectionSize.keys():
            L.append([int(ne), runnr, collectionSize[obj], format, stream, tag, obj ])
        
        try:
            #print myValues
            argv=[]
            argv.append("TempDataSize")
            argv.append("output=xml")
            argv.append("amiAutoAddUser")
            argv.append("tag='"+tag+"'")
            argv.append("stream='"+stream+"'")
            argv.append("values="+L.__str__())
            # argv.append("AMIUser=ivukotic")
            # argv.append("AMIPass=12345")       
            result = self.amiclient.execute(argv)
            print result.output()
            print 'Inserted: ' + str(len(L)) + ' rows.'
        except Exception, msg:      
            print msg
        except:
            print "Unexpected error:", sys.exc_info()[0]
                
        print 'performanceDataUploader.py: Collection sizes uploaded to AMI.'
    
    
    def uploadAlgoPerformances(self, tag, processingStep, stream, runnr, filename):
        if self.establishConnection()==False:
            return
        
        try:
            # print 'If needed add tag and stream info'    
            self.cursor.callproc('CREATETAGANDSTREAM',[tag, stream])
            self.connection.commit()
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error (createTagAndStream):", sys.exc_info()[0]
        
        try:        
            # print 'If needed add processingStep'
            self.cursor.callproc('CREATEPROCESSINGSTEP',[processingStep])
            self.connection.commit()
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error (createProcessingStep):", sys.exc_info()[0]
                
            print 'Sending algo data to DB.'
            allInfo={}
            currentAlg=''
            events=0
            user_max=0
            user_total=0
            with open(filename, 'r') as f:
                lines=f.readlines()
                for l in lines:
                    if len(l)<2: continue
                    l=l.strip()
                    if l[0]=='[': 
                        currentAlg=l.strip('[').rstrip(']')
                        if currentAlg.count('Callback')>0:
                            li=currentAlg.index('[0')
                            ri=currentAlg.index(':Call')
                            if li!=0 and ri!=0:
                                currentAlg=currentAlg.replace(currentAlg[li:ri],'')
                    if l.count('cpu_user_total'): user_total=float(l.replace('cpu_user_total = ',''))
                    if l.count('cpu_user_max'): user_max=float(l.replace('cpu_user_max = ',''))
                    if l.count('cpu_user_nbr'): 
                        events=int(l.replace('cpu_user_nbr = ',''))
                        if user_total-user_max>0 and events>0:
                            allInfo[currentAlg]=[events, user_total-user_max]
                            
            L=[]                
            for alg in allInfo.keys():
               # print runnr, tag, stream, processingStep, alg, allInfo[alg][0], allInfo[alg][1]
               if len(alg)<150:
                   L.append([ allInfo[alg][1], allInfo[alg][0], runnr, processingStep, stream, tag, alg ])
               else:
                   print 'performanceDataUploader.py PROBLEM. THIS THING HAS EXTREMALY LONG NAME. PLEASE CHANGE IT.', alg
                   
            insert = "INSERT INTO tempalgoperformance ( cputime, events, runnumber, algo_proc_step, stream, reprotag, algoname) VALUES (:1, :2, :3, :4, :5, :6, :7)"
        
        try:    
            self.cursor.prepare(insert)
            self.cursor.executemany(None, L)
            print 'Inserted: ' + str(self.cursor.rowcount) + ' rows.'
            
            self.connection.commit()
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error:", sys.exc_info()[0]
            
        print 'performanceDataUploader: Algo performance uploaded to AMI.'
    
    
    def uploadJobPerformance(self, tag, processingStep, stream, runnr, filename):
        if self.establishConnection()==False:
            return
        try:
            print 'Sending Job data to DB.'
            
            import PerfMonComps.PerfMonSerializer as pmon_ser
            hdr                     = pmon_ser.extract_pmon_files(filename)
            data                    = pmon_ser.extract_pmon_infos(hdr['infos'].name)
            data                    = data['perf_summary']
            alldata                 = [runnr, tag, stream, processingStep]
            alldata.append( data['ini']['cpu'][0] )
            alldata.append( data['evt']['cpu'][0] )
            alldata.append( data['fin']['cpu'][0] )
            alldata.append( data['ini']['real'][0] )
            alldata.append( data['evt']['real'][0] )
            alldata.append( data['fin']['real'][0] )
            alldata.append( data['ini']['vmem'][0]*1024.0 )
            alldata.append( data['evt']['vmem'][0]*1024.0 )
            alldata.append( data['fin']['vmem'][0]*1024.0 )
            
            alldata.append( float( (data['job']['statm']['VmPeak'].split(' '))[0] ) )
            alldata.append( float( (data['job']['statm']['VmRSS'].split(' '))[0] ) )
            alldata.append( data['job']['vmem_slope'])
            alldata.append( data['evt']['nentries'])
            print  alldata
            
            self.cursor.callproc('ACCEPTJOBDATA',alldata)
            self.connection.commit()
            
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error:", sys.exc_info()[0]
        print 'performanceDataUploader: Job performance uploaded to AMI.'
    
    
    def uploadPerfMonSD(self, tag, processingStep, stream, runnr, filename):
        
        if self.production:
            self.AMIuploadPerfMonSD( tag, processingStep, stream, runnr, filename)
            return
            
        try:
            if self.establishConnection()==False:
                return
            
            import PerfMonComps.PMonSD as pm
            d = pm.parse(filename)
            if d is None:
                print 'detailed algo information is missing from the PerfMonSD file'
                return         
            d=d[0]
            if 'steps_comps' not in d.keys():
                print 'detailed algo information is missing from the PerfMonSD file'
                return
                
            print 'Sending PerfMonSD data to DB.'
            
            # print 'If needed add tag and stream info'    
            self.cursor.callproc('CREATETAGANDSTREAM',[tag, stream])
            self.connection.commit()
            
            # print 'If needed add processingStep'
            self.cursor.callproc('CREATEPROCESSINGSTEP',[processingStep])
            self.connection.commit()
            
            
            print 'Sending algo data to DB...'
            
            L=[]  
            i=d['steps_comps']['ini']
            e=d['steps_comps']['evt']
            f=d['steps_comps']['fin']
            
              
            for algoname in i.keys():
                if len(algoname) > 150:
                    print 'performanceDataUploader.py PROBLEM. THIS THING HAS EXTREMALY LONG NAME. PLEASE CHANGE IT.', algoname
                    continue
                cpu=0;ini=0;fin=0
                
                ini=i[algoname]['cpu']
                ev=i[algoname]['n']
                
                if  algoname in e.keys():
                    cpu=e[algoname]['cpu']
                    ev=e[algoname]['n']                
                
                if algoname in f.keys():
                    fin=f[algoname]['cpu']
                
                if cpu<0.01 and fin<0.01 and ini<0.01: continue
                
                L.append([cpu, ini, fin, ev, runnr, processingStep, stream, tag, algoname])
                #print [cpu, ini, fin, ev, runnr, processingStep, stream, tag, algoname]
                    
            # print 'algos:', len(L)
            
            insert = "INSERT INTO tempalgoperformance ( cputime, cpuini, cpufin, events, runnumber, algo_proc_step, stream, reprotag, algoname) VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9)"
            
            self.cursor.prepare(insert)
            self.cursor.executemany(None, L)
            print 'Inserted: ' + str(self.cursor.rowcount) + ' rows.'
            
            self.connection.commit()
            
            print 'performanceDataUploader: Algo performance uploaded to AMI.'
            
            if 'special' not in d.keys():
                print 'job information is missing from the PerfMonSD file'
                return
                
            print 'Uploading job info'
            alldata = [runnr, tag, stream, processingStep]
            s=d['special']
            
            alldata.append( s['values']['cpu_bmips']                   )
            alldata.append( s['timings']['evtloop_time']['cpu']        )
            alldata.append( s['timings']['overhead_time']['cpu']       )
            alldata.append( s['leaks']['leakperevt_evt11to50']['malloc'] )
            alldata.append( s['timings']['evtloop_time']['wall']       )
            alldata.append( s['timings']['overhead_time']['wall']      )
            
            alldata.append( s['values']['vmem_mean'] )
            alldata.append( s['values']['jobcfg_walltime'] )
            alldata.append( s['leaks']['leakperevt_evt101plus']['malloc'] )
            
            alldata.append( s['values']['vmem_peak'] )
            alldata.append( s['values']['rss_mean'] )
            alldata.append( s['leaks']['leakperevt_evt101plus']['vmem'] )
            
            alldata.append( s['nevts'] )
            
            #print  alldata
            
            self.cursor.callproc('ACCEPTJOBDATA',alldata)
            self.connection.commit()
            
        except cx_Oracle.DatabaseError, exc:
            error, = exc.args
            print "performanceDataUploader.py Oracle-Error-Code:", error.code
            print "performanceDataUploader.py Oracle-Error-Message:", error.message
        except:
            print "Unexpected error:", sys.exc_info()[0]
            
        print 'performanceDataUploader: Job performance uploaded to AMI.'
	    
    
		
    def AMIuploadPerfMonSD(self, tag, processingStep, stream, runnr, filename):
        try:
            
            import PerfMonComps.PMonSD as pm
            d = pm.parse(filename)
            if d is None:
                print 'detailed algo information is missing from the PerfMonSD file'
                return         
            d=d[0]
            if 'steps_comps' not in d.keys():
                print 'detailed algo information is missing from the PerfMonSD file'
                return
            
            print 'Sending PerfMonSD data to AMI DB.'
            
            L=[]
            
            i=d['steps_comps']['ini']
            e=d['steps_comps']['evt']
            f=d['steps_comps']['fin']
            
            
            for algoname in i.keys():
                if len(algoname) > 150:
                    print 'performanceDataUploader.py PROBLEM. THIS THING HAS EXTREMALY LONG NAME. PLEASE CHANGE IT.', algoname
                    continue
                cpu=0;ini=0;fin=0
                
                ini=i[algoname]['cpu']
                ev=i[algoname]['n']
                
                if  algoname in e.keys():
                    cpu=e[algoname]['cpu']
                    ev=e[algoname]['n']                
                    
                if algoname in f.keys():
                    fin=f[algoname]['cpu']
                    
                if cpu<0.01 and fin<0.01 and ini<0.01: continue
                
                L.append([cpu, ini, fin, ev, runnr, processingStep, stream, tag, algoname])
                #L.append([cpu, ini, fin, ev, runnr, 'TEST', 'TEST', 'TEST', algoname])
                #print [cpu, ini, fin, ev, runnr, processingStep, stream, tag, algoname]
            
            try:
                #print myValues
                argv=[]
                argv.append("TempAlgoPerformance")
                argv.append("output=xml")
                argv.append("tag='"+tag+"'")
                argv.append("stream='"+stream+"'")
                argv.append("processingStepName='"+processingStep+"'")
                argv.append("values="+L.__str__())
                # argv.append("AMIUser=ivukotic")
                # argv.append("AMIPass=12345")       
                result = self.amiclient.execute(argv)
                print result.output()
                print 'Inserted: ' + str(len(L)) + ' rows.'
                
            except Exception, msg:      
                print msg
            except:
                print "Unexpected error:", sys.exc_info()[0]
            
            print 'performanceDataUploader: Algo performance uploaded to AMI.'
            
             
            if 'special' not in d.keys():
                print 'job information is missing from the PerfMonSD file'
                return
                
            print 'Uploading job info'
            alldata = [runnr, tag, stream, processingStep]
            s=d['special']
            
            alldata.append( s['values']['cpu_bmips']                   )
            alldata.append( s['timings']['evtloop_time']['cpu']        )
            alldata.append( s['timings']['overhead_time']['cpu']       )
            alldata.append( s['leaks']['leakperevt_evt11to50']['malloc'] )
            alldata.append( s['timings']['evtloop_time']['wall']       )
            alldata.append( s['timings']['overhead_time']['wall']      )
            
            alldata.append( s['values']['vmem_mean'] )
            alldata.append( s['values']['jobcfg_walltime'] )
            alldata.append( s['leaks']['leakperevt_evt101plus']['malloc'] )
            
            alldata.append( s['values']['vmem_peak'] )
            alldata.append( s['values']['rss_mean'] )
            alldata.append( s['leaks']['leakperevt_evt101plus']['vmem'] )
            
            alldata.append( s['nevts'] )
            
            #print  alldata
            
            argv=[]
            argv.append("AcceptJobData")
            argv.append("output=xml")
            argv.append("alldata="+alldata.__str__())
#            argv.append("AMIUser=ivukotic")
#            argv.append("AMIPass=12345")
            result = self.amiclient.execute(argv)
            print result.output()
            print 'performanceDataUploader: Job performance uploaded to AMI.'
            
        except Exception, msg:      
            print 'performanceDataUploader: EXCEPTION in Algo performance uploading to AMI.',msg
        except:
            print "Unexpected error:", sys.exc_info()[0]
            
    




if __name__ == '__main__':

    import sys, time, threading


    for t, s in ((2.0, 1), (1.0, 20)):  # note, 20!
        try:
            print 'timelimit',t,'\texec time:',s
            r = timelimited(t, time.sleep, s)
            print 'done in time'
        except Exception, e:  #XXX as for Python 3.0
            r = e
            print 'timeout'

     # check that all created threads stopped
    for t in threading.enumerate():
        if t.isAlive() and repr(t).startswith('<_Timelimited('):
           failed('thread %r still alive', t)
           break
    else:
        print 'all _Timelimited threads stopped'




