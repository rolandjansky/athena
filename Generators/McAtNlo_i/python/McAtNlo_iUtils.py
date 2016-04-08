# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# This utility is designed to be plugged in production 
# of MC@NLO evgen samples. It helps to skip standalone  
# preparation of MC@NLO input samples rather they are generated 
# on the fly using the methods of the utility
# 
# @author Gia Khoriauli <gia@mail.cern.ch>, October 2012

import os
import string

from AthenaCommon import Logging

herwigversion=''
mcatnloversion=''


def fixparrecord ( proc = 105200 , evprefix='ttbar' ) :
   """
   Performes all process depended fixes of records in a .dat file.
   Production responsibles of samples should provide corresponding fixes if needed. 
   """
   log = Logging.logging.getLogger('McAtNlo_i.McAtNlo_iUtils.fixparrecord')
   
   if proc==105200 :
      log.debug('Nothing to do with the .dat file for DIS 105200')



def fixeventrecord ( proc = 105200 , evprefix='ttbar' ) :
   """
   Performes all process depended fixes of records in a .events file.
   Production responsibles of samples should provide corresponding fixes if needed. 
   """
   log = Logging.logging.getLogger('McAtNlo_i.McAtNlo_iUtils.fixeventrecord')
   
   if proc==105200 :
      if os.path.isfile(evprefix+'.events') :
         log.debug(evprefix+'.events exist and is going to be modified')  
         os.system('sed  -e \'s/0.8040\([DEe]\)+02\ /8.0399\\1+01\ /1\' -i '+evprefix+'.events')
      else :
         log.warning(evprefix+'.events DOES NOT exist')  



def configmcatnlo ( pardict={}, outfilename='MCatNLO_template.inputs', infilename='MCatNLO.inputs' ) :
   """
   Initializes a sub-set of MC@NLO parameters from the provided list of parameters:values. Remining parameters will keep the default values.
   """
   log = Logging.logging.getLogger('McAtNlo_i.McAtNlo_iUtils.configmcatnlo')
   
   if not os.path.isfile(infilename) :
      log.warning('Configuration file '+ infilename +' is not found')  
      return False
   
   log.info('The following parameters of MC@NLO will be set to the provided values: '+ str(pardict))  
 
   #create a new mc@nlo script with modifed parameter values out of the original script 
   fiout = open(outfilename, 'w')
   fi = open(infilename, 'r')
   
   line=fi.readline()
   while not line=='' : 
      log.debug(line)  
      
      changepar=False;
      for par in pardict.keys() :
         indx=line.find(par)
         if indx == 0 :
            changepar=True 
            lineparts=line.split("=")
            linenew=lineparts[0]+'='+str(pardict[par])+'\n'
            log.debug(' ---------->  '+ linenew)  
            fiout.write(linenew)
            break;

      if not changepar :
         fiout.write(line)
      
      line=fi.readline()

   fi.close()
   fiout.close()
   return True



def generateinput (processid=105200, numofevents=13000, indxofoutfile=1, pardict={}) :
   """
   This method generates mc@nlo input of events for further processing of these events
   under the Athena framework.
   """ 
   log = Logging.logging.getLogger('McAtNlo_i.McAtNlo_iUtils.generateinput')
   log.info('Start MC@NLO event generation of the sample with DIS='+str(processid)+'. Number of generated events='+str(numofevents))

   env=os.environ
   envkeys=env.keys()

 
   # ----- CMT ENV ---->
   CMTCONFIG=''
   if 'CMTCONFIG' in envkeys :
      CMTCONFIG=env['CMTCONFIG']
      log.debug('CMTCONFIG: '+CMTCONFIG)  
   else:
      log.fatal('CMTCONFIG is not determined. Try to setup Athena first and run again. Now, retreat.')  
      quit()
   
   CMTPROJECTPATH=''
   if 'CMTPROJECTPATH' in envkeys :
      CMTPROJECTPATH=env['CMTPROJECTPATH']
      CMTPROJECTPATH=string.split(CMTPROJECTPATH,':')
      CMTPROJECTPATH=CMTPROJECTPATH[0]
      log.debug('CMTPROJECTPATH: '+CMTCONFIG)  
   else:
      log.fatal('CMTPROJECTPATH is not determined. Try to setup Athena first and run again. Now, retreat.')  
      quit()
   # <---- CMT ENV -----
   
   
   # ----- PDF ---->
   LHAPATH=''
   if 'LHAPATH' in envkeys :
      LHAPATH=env['LHAPATH']
      log.debug('LHAPATH: '+LHAPATH)  
   else:
      log.fatal('LHAPATH is not determined. Try to setup Athena first and run again. Now, retreat.')  
      quit()
   
   LHAPDFSET=LHAPATH
   LHAPATH=LHAPATH.replace('/share/PDFsets','')
   LHAVERSION=string.split(LHAPATH,"/")
   LHAVERSION=LHAVERSION[-1]
   log.debug('LHAVERSION: '+LHAVERSION)  
   
   LHAPATH=LHAPATH+'/'+CMTCONFIG
   if not os.path.isdir(LHAPATH) or not os.path.isdir(LHAPDFSET) :
      log.fatal('LHAPATH (or LHAPDFSET) points to a wrong location ('+LHAPATH+'). Fix this and run again. Now, retreat.')  
      quit()
   #/cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/17.2.2/sw/lcg/external/MCGenerators_hepmc2.06.05/lhapdf/5.8.5/i686-slc5-gcc43-opt/lib/
   # <---- PDF -----
   
   
   # ----- GENERATORS ---->
   HepMCVERS='' #'2.06.05'
   if 'HepMCVERS' in envkeys :
      HepMCVERS=env['HepMCVERS'].rstrip()
      log.debug('HepMCVERS: '+HepMCVERS)  
   else :
      log.fatal('HepMCVERS is not determined. Try to setup Athena first and run again. Now, retreat.')
      quit()
 
   herwig=CMTPROJECTPATH + '/sw/lcg/external/MCGenerators_hepmc'+HepMCVERS+'/herwig/' + herwigversion + '/' + CMTCONFIG  
   if not os.path.isdir(herwig) :
      log.fatal('herwig path (' + herwig + ') points to a wrong location. Fix this and run again. Now, retreat.')  
      quit()
   else :
      log.debug('herwig: '+herwig)

   #/afs/cern.ch/sw/lcg/external/MCGenerators_hepmc2.06.05/mcatnlo/3.42/i686-slc5-gcc43-opt
   #In order to run on the GRID the mc@nlo path has to be this:
   #	/cvmfs/atlas.cern.ch/repo/sw/software/../../sw/lcg/external/MCGenerators_hepmc2.06.05/..
   mcatnlo='/afs/cern.ch/sw/lcg/external/MCGenerators_hepmc'+HepMCVERS+'/mcatnlo/'+ mcatnloversion

   if not os.path.isdir(mcatnlo) :
      log.fatal('mcatnlo path (' + mcatnlo + ') points to a wrong location. Fix this and run again. Now, retreat.')  
      quit()
   else :
      log.debug('mcatnlo: '+mcatnlo)
   
   mcatnlolib=mcatnlo + '/' + CMTCONFIG
   if not os.path.isdir(mcatnlolib) :
      #lib can be installed directly in the head directory
      mcatnlolib=mcatnlo

   if not os.path.isdir(mcatnlolib+'/lib/archive') :
      log.fatal('mcatnlo library archive (' + mcatnlolib + ') doesn\'t exist. Fix this and run again. Now, retreat.')  
      quit()
   else :
      log.debug('mcatnlolib: '+mcatnlolib)
   
   mcatnloworkdir=''
   if not os.path.isdir(mcatnlo+'/share/examples') :
      if not os.path.isdir(mcatnlo+'/examples') :
         log.fatal('Neither mcatnlo/share/examples nor mcatnlo/examples directory exist. Try to fix this or use other mc@nlo version and run again. Now, retreat.')  
         quit()
      else :
         mcatnloworkdir=mcatnlo+'/examples'
   else :
      mcatnloworkdir=mcatnlo+'/share/examples'
   # <---- GENERATORS -----
   
   
   # ----- CONFIGURE MC@NLO ---->
   os.system('cp -r '+mcatnloworkdir+' .')
   os.chdir('./examples')
   if os.path.isdir('./Linux') :
      os.system('rm -rf ./Linux')
   os.system('pwd')
   os.system('ls -l')

   #mc@nlo fails if the Athena setup is 32 bit but the host machine has the 64 bit architecture. to avoid this the mc@nlo config file has to be modified.
   #mc@nlo Makefile has to be modified as well in order to include FFLAGS but this has to be done after the configuration script is executed. 
   #  this fix should be provided by the mc@nlo side. for now, do this myself.
   archtype = CMTCONFIG.split("-")[0].rstrip()
   if archtype.find('i686')>-1 :
      print 'archtype  =  ', archtype
      log.info('Run is with the 32-bit setup of Athena. Configure mc@nlo with the -m32 option for a Fortran compiler')
      #os.system('sed  -e \'s/FFLAGS_OPT="-O2"/FFLAGS_OPT="-O2 -m32"/1\' -i configure') 
      os.system(r"sed  -e 's/FFLAGS_OPT=\"-O2\"/FFLAGS_OPT=\"-O2 -m32\"/1' -i configure") 

   os.system( './configure --lcgplatform='+CMTCONFIG+'  --with-herwigversion='+herwigversion+'  --with-herwig='+herwig+' --with-mcatnlo='+mcatnlolib+'  --with-lhapdfversion='+LHAVERSION+'  --with-lhapdf='+LHAPATH+'  --with-lhapdfsets='+LHAPDFSET )
   #os.system( './configure --lcgplatform='+CMTCONFIG+'  --with-herwigversion='+herwigversion+'  --with-herwig='+herwig+' --with-mcatnloversion='+mcatnloversion+'  --with-lhapdfversion='+LHAVERSION+'  --with-lhapdf='+LHAPATH+'  --with-lhapdfsets='+LHAPDFSET )

   if archtype.find('i686')>-1 :
      #  these fixes should be provided by the mc@nlo side. for now, do this myself.
      #os.system('sed  -e \'s/$(FC)\ tmpmain.f\ -o/$(FC)\ tmpmain.f\ $(FFLAGS)\ -o/g\' -i Makefile') 
      os.system(r"sed  -e 's/$(FC) tmpmain.f -o/$(FC) tmpmain.f $(FFLAGS) -o/g' -i Makefile") 

   # set run configuration of mc@nlo parameters
   if not configmcatnlo(pardict, 'MCatNLO_template.inputs', 'MCatNLO.inputs') :
      log.fatal('Was not able to setup MC@NLO parameters. Retreat.')
      quit()
   
   #build a random number seed if the corresponding MC@NLO parameter is initialized with a string value 
   if 'RNDEVSEED' in pardict.keys() :
      tmprndseed = pardict['RNDEVSEED']
      if type( tmprndseed ) == str :
         rndseed = processid + numofevents * (indxofoutfile-1)
         log.debug('RNDEVSEED = ' + str(rndseed))
         os.system('sed  -e s/RNDEVSEED='+tmprndseed+'/RNDEVSEED='+str(rndseed)+'/1   ./MCatNLO_template.inputs   >  ./MCatNLO_'+str(processid)+'.inputs')
      else :
         os.system('cp ./MCatNLO_template.inputs   ./MCatNLO_'+str(processid)+'.inputs')
      os.system( 'chmod +x ./MCatNLO_'+str(processid)+'.inputs' )
      os.system( 'rm -f ./MCatNLO_template.inputs' )
   else :
      log.warning('Warning: initial random seed for event generation is not updated! This will lead to duplicated events.')
      log.warning('Include \'RNDEVSEED\':\'somestring\' or \'RNDEVSEED\':N in parameter dictionary, where N is an intager number.')
   # <---- CONFIGURE MC@NLO -----
   
   
   # ----- RUN MC@NLO  &  PREPARE ITS OUTPUT FOR FURTHER PROCESSING  ---->
   # generate events
   os.system( 'source ./MCatNLO_'+str(processid)+'.inputs' )

   #safety checks 
   fi=open('./MCatNLO_'+str(processid)+'.inputs')
   if not fi :
      log.fatal('Can\'t open ./MCatNLO_'+str(processid)+'.inputs. Retreat.')
      quit()
    
   parlist=['EVPREFIX', 'EXEPREFIX']
   evprefix=''
   exeprefix=''
   parcount=0
   line=fi.readline()
   while not line=='' : 
      for par in parlist :
         indx=line.find(par)
         if indx == 0 :
            if par=='EVPREFIX' :
               evprefix=line.split("=")[1].rstrip()
               parcount+=1
            elif par=='EXEPREFIX' :
               exeprefix=line.split("=")[1].rstrip()
               parcount+=1
      if parcount==len(parlist) :
         break 
      line=fi.readline()
 
   if not os.path.isdir('./Linux') :
      log.fatal('Linux directory was not created. Retreat.')  
      quit()
   os.chdir('./Linux')
   if not os.path.isfile(evprefix+'.events') :
      log.fatal(evprefix+'.events file was not created. Retreat.')  
      quit()
   if not os.path.isfile(exeprefix+'MCinput') :
      log.fatal(exeprefix+'MCinput file was not created. Retreat.')  
      quit()
   
   #apply the process dependent correction to the .events and .dat files if any
   fixeventrecord( processid, evprefix )
   fixparrecord(   processid, evprefix )
   
   #prepare setup for Athena run
   os.chdir('../../')
   os.system('pwd')
   os.system( 'ln -sf ./examples/Linux/'+exeprefix+'MCinput '+evprefix+'._00001.dat' )
   os.system( 'ln -sf ./examples/Linux/'+evprefix+'.events ' +evprefix+'._00001.events' )
   #os.system( 'sed  -e \'s/'+evprefix+'.events'+'/events.lhe/1\' -i '+evprefix+'._00001.dat')
   os.system( r"sed  -e 's/"+evprefix+".events"+"/events.lhe/1' -i "+evprefix+"._00001.dat")
   # <---- RUN MC@NLO  &  PREPARE ITS OUTPUT FOR FURTHER PROCESSING -----


