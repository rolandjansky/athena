#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Class for streaming the events reconstructed by the trigger.
#
# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - few changes.
# --2010-03-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - filters of stream tag for the data streaming implemented


import eformat
import sys
import os
import time
import logging

streamer_log = logging.getLogger("BatchHLTApp.Streamer")

from TrigTransformOld.HLTJobLib import *
from TrigTransformOld.FileCollection import *

class Streamer(Step):
        """ A class to distribute the events into the corresponding "stream" file """

        def __init__(self, outputdir, inclusive, maxfiles,prefix,physics_suffix,error_suffix,tail,pver,filters=[],dolumi=False ):
                """ A constructor for the Streamer class """          
                Step.__init__(self,'STREAMER') 
                self.outputdir        = outputdir        
                self.maxFiles                = maxfiles        
                self.inEx                = inclusive
                self.prefix                = prefix                # this could be data09_cos.00121041
                self.physics_suffix        = physics_suffix         # this could be Reproc.RAW
                self.error_suffix        = error_suffix                # this could be Error.RAW
                self.tail                = tail                         # this could be _lb001._CAF-03
                self.pver               = pver
                self.outputfiles        = {}
                self.allfiles                = []
                self.filename2key        = {}
                self.total_writen        = 0
                self.filters            = filters
                self.dolumi                = dolumi 
                self.overlaps                = {} 
                self.summary={                        \
                          'name':self.name,        \
                          'time': 0,                \
                          'outputfiles': {},                \
                          'total_writen': 0,                \
                          'overlaps':{},                        \
                     }
                # overlaps has the next form: 
                # reference        
                
        def toDict(self): 
                return self.summary

        def _filter(self,stream_tags):
                if len(self.filters)==0: return stream_tags
                
                output_stream = []
                for s in stream_tags:
                        for f in self.filters:
                                if re.compile(f).search(s.type+'_'+s.name):
                                        output_stream.append(s)
                return output_stream
        
        def _getCorrespondingFileNames(self, event):
                """ Internal service for the Streamer class. """        
                """ Returns the name of the files that corresponds to this event. The name follows the TDAQ standards"""
                """ filename = daq . CSC . runNO . Stream-type . Stream-route . LB . SFO . _Number . data  """
                """ Note that more than one file can correspond to the same event due to inclusive/exclusive streaming """

                filenames        = []        

                StreamTags        = self._filter(event.stream_tag())
                LumiBlock        = event.lumi_block()
        

                for stream in StreamTags:
                        blumi        = stream.obeys_lumiblock
                        myname        = stream.name
                        mytype        = stream.type

                        suffix  = self.physics_suffix
                        if mytype.find('debug')!=-1: 
                                suffix = self.error_suffix 
                        
                        if blumi and self.dolumi:
                                filename = "%s.%s_%s.%s._lb%04d.%s" % (self.prefix,mytype,myname,suffix,event.lumi_block(),self.tail)
                        else:
                                filename = "%s.%s_%s.%s._lb0000.%s" % (self.prefix,mytype,myname,suffix,self.tail)
                        
                        #print "filename is %s" % filename
                        
                        filenames.append(filename)
                        self.filename2key [filename] = (mytype,myname)

                return filenames        

        def _addOverlaps(self, event):
                StreamTags        = self._filter(event.stream_tag())
                for r_stream in StreamTags:
                        for o_stream in StreamTags:
                                reference_stream = "%s_%s" % (r_stream.type,r_stream.name)
                                other_stream     = "%s_%s" % (o_stream.type,o_stream.name)
                                if not self.overlaps.has_key(reference_stream):
                                        self.overlaps[reference_stream]={}
                                if not self.overlaps[reference_stream].has_key(other_stream):
                                        self.overlaps[reference_stream][other_stream]=0
                                self.overlaps[reference_stream][other_stream]+=1

        
        def _renameFile(self, name):
                flst = [f for f in os.listdir('.') if re.compile(name).match(f)]
                if flst==None: return None
                
                try:
                        for f in flst:
                                dest   = f.replace('RAW','RAW.%s'%self.pver)
                                shutil.move(f,dest)
                        return '.'.join(dest.split('.')[:-2])
                except:
                        return None
                        
                        
        def streamIt(self, event):
                files = self._getCorrespondingFileNames(event)
                self._addOverlaps(event)
                for f in files:
                        if not self.outputfiles.has_key(f):
                                self.outputfiles[f] = eformat.ostream(run_number=event.run_no(),core_name= f)
                        newEvent = eformat.write.FullEventFragment(event)
                        self.outputfiles[f].write(newEvent)
                
                if len(files)>0: 
                        self.total_writen +=1

        def getOutputFiles(self):
                return self.allfiles

        def streamFile(self,fileset):
                streamer_log.info("streaming...")
                if len(self.filters)!=0:
                        streamer_log.info("setting the following filters:")
                for f in self.filters:
                        streamer_log.info("  "+str(f))
                
                input = eformat.istream(fileset.files)
                self.start()
                for event in input:
                        #cntEvt+=1
                        self.streamIt(event)
                # now create the FileCollection structures 
                allfiles=[]
                names = {}
                for f in self.outputfiles.keys():
                        del(self.outputfiles[f])
                        cname = f
                        if self.pver!='': cname=self._renameFile(f)
                        reconstructed=UnProcessedFileSet(core_name=cname)
                        (streamtype,streamname)=self.filename2key[f]
                        entry = (reconstructed,streamtype,streamname)
                        allfiles.append(entry)
                        for n in reconstructed.files:
                                events = ProcessedFileSet(n).fileSummary()['eventnumber']
                                names.update( {n: events} )
        
                # fill with 0s empty overlaps        
                streams = self.overlaps.keys()
                for reference in streams:
                        for running in streams:
                                if not self.overlaps[running].has_key(reference):
                                        self.overlaps[running][reference]=0
        
                self.allfiles=allfiles
                self.stop()
                self.summary['outputfiles']=names
                self.summary['overlaps']=self.overlaps
                self.summary['total_writen']=self.total_writen

                for k in names.keys(): streamer_log.info( 'created file %s' % k )
                self.wrapUp()
                return self.allfiles


def main():
        if len(sys.argv) != 3 :
          print " usage: %s  <file1> <outputdir> " % sys.argv[0]
          sys.exit(1)
        
        input_file = sys.argv[1]
        output_dir = sys.argv[2]
        
        st        = Streamer(output_dir,False,30,"whatever")
        st.streamFile(ProcessAbleFileSet(input_file))
        result= Step().getStats()
        history= Step().getHistory()
        result.serialize('result.pickle')        
        history.serialize('history.pickle')        
        

if __name__ == '__main__': 
        main()
