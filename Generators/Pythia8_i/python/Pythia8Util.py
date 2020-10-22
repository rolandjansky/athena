# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode
import glob,subprocess,os,time,shutil
import xml.dom.minidom
import filecmp

__author__ = 'Giancarlo Panizzo <giancarlo.panizzo@cern.ch>'

class Pythia8Util(EvgenAlg):
    """
    A python-only algorithm to monitor Pythia8 behaviour / settings
    """

    def __init__(self, name="Pythia8Util", outputParticleDataFile="ParticleData.local.xml", threshold=50):
        super(Pythia8Util,self).__init__(name=name)
        self.threshold = threshold

        self.njobs = 1
        self.mode = 0
        self.Pythia8Commands = []
        self.outputParticleDataFile = outputParticleDataFile

    def ensure_toplevel(self,file_name):
        """ Insert given string as a new line at the beginning of a file """
        # define name of temporary dummy file
        dummy_file = file_name + '.bak'
        # open original file in read mode and dummy file in write mode
        with open(file_name, 'r') as read_obj, open(dummy_file, 'w') as write_obj:
            # Write given line to the dummy file
            write_obj.write("<particledata>"+'\n')
            # Read lines from original file one by one and append them to the dummy file
            for line in read_obj:
                write_obj.write(line)
            write_obj.write("</particledata>"+' \n')
        # remove original file
        os.remove(file_name)
        # Rename dummy file as the original file
        os.rename(dummy_file, file_name)


    def cmpsettings(self, requestedsettings):
        if ( filecmp.cmp("Settings_before.log","Settings_after.log") ):
          self.msg.info("Settings match before and after initialisation of Pythia8.")
        else:
          if requestedsettings.has_key("PartonShowers:model"):
             if (requestedsettings["PartonShowers:model"] !="1"):
               self.msg.info("Settings before and after initialisation of Pythia8 don't match, because you asked PartonShowers:model = %s" % requestedsettings["PartonShowers:model"])
          else:
             self.msg.warning("Settings before and after initialisation of Pythia8 don't match.")
          with open("Settings_before.log") as f1:
              f1_lines = f1.read().splitlines()
          with open("Settings_after.log") as f2:
              f2_lines = f2.read().splitlines()
          # Find and print the diff:
          if (len(f1_lines) == len(f2_lines)):
            for iline in range(len(f1_lines)):
                if  (f1_lines[iline]!=f2_lines[iline]):
                  if ( requestedsettings.has_key(f2_lines[iline].split("=")[0]) ):
                    self.msg.warning("  >>      %s != %s. You requested %s." % (f1_lines[iline],f2_lines[iline].split("=")[1],requestedsettings[f2_lines[iline].split("=")[0]]) )
                  else:
                    self.msg.info("  >>      %s != %s" % (f1_lines[iline],f2_lines[iline].split("=")[1]) )
          else:
            self.msg.warning("Not even the -number- of settings before and after initialisation of Pythia8 matches, check yourself what's going wrong.")

    def cmpparticledata(self,pids,rpd):
        # open particledata before initialisation
        f1=self.outputParticleDataFile
        f2=self.outputParticleDataFile
        if f1.endswith('xml'):
          f1 = f1[:-3]
        f1 += "orig.xml"
        try:
          doc1 = xml.dom.minidom.parse(f1);
        except:
          # not yet a problem, particledata files needs to be decorated to be xml compliant
          self.ensure_toplevel(f1)
          try:
            doc1 = xml.dom.minidom.parse(f1);
          except:
            self.msg.error("Bad file, exiting")
            return;

        # open particledata after initialisation
        try:
          doc2 = xml.dom.minidom.parse(f2);
        except:
          # not yet a problem, particledata files needs to be decorated to be xml compliant
          self.ensure_toplevel(f2)
          try:
            doc2 = xml.dom.minidom.parse(f2);
          except:
            self.msg.error("Bad file, exiting")
            return;
    
  
        particles1 = doc1.getElementsByTagName("particle")
        particles2 = doc2.getElementsByTagName("particle")
        if (particles1.length == particles2.length):
           self.msg.info("Number of particles before and after Pythia8 initialisation matches ( %d )" % particles1.length)
        else:
           self.msg.warning("Mismatch in number of particles before and after Pythia8 initialisation. Before: %d , after: %d" % (particles1.length,particles2.length) )
        for  part1 in particles1:
          if ( part1.getAttribute("id") in pids):
            for  part2 in particles2:
              if ( part2.getAttribute("id") == part1.getAttribute("id") ):
                for attrk2, attrv2 in part2.attributes.items():
                  requested="none"
                  trypartprop = part2.getAttribute("id")+":"+attrk2
                  if ( trypartprop in rpd ):
                    requested = rpd[trypartprop]
                  if ( not ( attrk2 in part1.attributes.keys() ) ):
                    self.msg.info("You asked Pythia8 to modify properties for particle %s (%s). Attribute \"%s\" added after initialisation, check it. Requested: %s. After init:  %s."  % (part2.getAttribute("name"), part2.getAttribute("id"), attrk2, requested, attrv2) )
                  for attrk1, attrv1 in part1.attributes.items():
                    if (attrk1 == attrk2 and attrv1 != attrv2):
                      self.msg.warning("You asked Pythia8 to modify properties for particle %s (%s). Attribute \"%s\" modified after initialisation. Requested: %s. Before init: %s. After init:  %s." % (part1.getAttribute("name"), part1.getAttribute("id"), attrk1, requested, attrv1, attrv2) )



    def initialize(self):
        self.msg.info('Running in serial mode.')
        return StatusCode.Success

    def execute(self):
        return StatusCode.Success

    def finalize(self):
        # get list of PIDs for which user asked some changes
        joparticles = [ i.replace(" ", "").split(":")[0] for i in self.Pythia8Commands if (i[0].isdigit()) ]
        requestedparticledata = { i.replace(" ", "").split("=")[0]:i.split("=")[1].strip() for i in self.Pythia8Commands if (i[0].isdigit()) }
        requestedsettings = { i.replace(" ", "").split("=")[0]:i.split("=")[1].strip() for i in self.Pythia8Commands if (not i[0].isdigit()) }
        self.cmpsettings(requestedsettings) 
        self.cmpparticledata(joparticles,requestedparticledata)
        return StatusCode.Success

