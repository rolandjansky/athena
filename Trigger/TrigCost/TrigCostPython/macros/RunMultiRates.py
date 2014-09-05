#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys, subprocess, os, time, string
import xml.dom.minidom
from multiprocessing import Pool
from TrigCostPython import TrigCostXML
#from TrigCostPython import RatesTwikiTable
from AddPieChart import AddPieChart 

# Where the TrigMon code is checked out
#ratecodepath = os.getenv('TestArea')

def search_file(filename, search_path):
   """Given a search path, find file
   """
   file_found = 0 
   paths = string.split(search_path, os.pathsep)
   for path in paths:
      if os.path.exists(os.path.join(path, filename)):
         file_found = 1 
         break
   if file_found:
      return os.path.abspath(os.path.join(path, filename))
   else:
      return None

if __name__ == '___main__':
   search_path = '/bin' + os.pathsep + '/usr/bin'  # ; on windows, : on unix
   find_file = search_file('ls',search_path)
   if find_file:
      print "File found at %s" % find_file
   else:
      print "File not found"
																														


xmlpaths =  os.environ.get("XMLPATH")   
TrigMonCostsNtuple = search_file("TrigCostRate/TrigMonCosts_ntuple_index.xml",xmlpaths); 

#----------------------------------------------------------------------
# Class describing jobs 
#----------------------------------------------------------------------
class JobDesc:    
    def __init__(self) :
        self.datasets=[]
        self.AllOverlap=False
        self.name='test'
        self.PSxml=None
        self.bgxml=None
        self.dqxml=None
        self.predlumi=None
        self.predrun=None
        self.predlb=None
        self.groupxml=None
        self.key='TrigRate'

joblist=[]

if __name__ == '__main__' :
    #-----------------------------------------------------------------------
    #  Options definitions
    #-----------------------------------------------------------------------
    from optparse import OptionParser

    p = OptionParser(usage='<rates xml> -o <output location>')

    p.add_option('-o', '--output', type = 'string', default='', 
                 dest = 'outdir', help = 'directory where html directories are placed' )

    p.add_option('', '--dataset-xml', type='string', default=TrigMonCostsNtuple,
                 dest = 'ds_xml', help = 'Location of dataset description' )

    p.add_option('', '--forwardArgs', type='string', default=None,
                 dest = 'forwardArgs', help = 'Arguments to forward to runRates.py')

    p.add_option('--max-proc', type='int', default=4,
                 dest = 'max_proc', help = 'give the maximum number of processors (SHOULD NOT EXCEED 4!)' )

    p.add_option('--no-pie', default=False, action='store_true',
                 dest = 'no_pie', help = 'do not make the pie charts' )

    p.add_option('--dry-run', default=False, action='store_true',
                 dest = 'dry_run', help = 'do not run job, but print the command' )

    p.add_option('--debug', default=False, action='store_true',
                 dest = 'debug', help = 'go to single process for easier debuging' )

    p.add_option('--write-twiki', default=False, action='store_true',
                 dest = 'write_twiki', help = 'Add results to rates twiki table' )

    p.add_option('-l', '--joblist', type='string', default='',
                 dest = 'joblistname', help = 'Specify the job list' )



    (options, args) = p.parse_args()


    #-----------------------------------------------------------------------
    # Fill job list
    #-----------------------------------------------------------------------
    import MultiRatesJobList
    MultiRatesJobList.FillJobList(joblist,options.joblistname)

    #-----------------------------------------------------------------------
    # Read the Dataset XML file
    #-----------------------------------------------------------------------
    class dataset_descr :

        def __init__(self) :
            pass

        def __eq__(this, rhs) :
            return self.name == rhs

        name = ''
        path = ''
        xsec = ''
        ncoll = ''
        menu = ''
        project = ''
        version = ''

    def ReadXmlFile(filename):

        def GetData(nodelist, name):

            for val in nodelist:
                if val.nodeName != name:
                    continue

                svalue = ''
                icount = int(0)
                for s in val.childNodes:
                    icount = icount+1
                    svalue = s.data

                if icount == 1:
                    return svalue
                else:
                    return '' 

            raise Exception('Failed to find data')

        # create and initialize parser
        try :
            myf = open(filename,'r')
        except IOError :
            print "Could not find dataset file!, Please provide the correct path"

        dom = xml.dom.minidom.parse(myf)    

        ds_location = dict() 
        for set in dom.getElementsByTagName('dataset') :
            ds = dataset_descr() 

            ds.name    = GetData(set.childNodes, 'name')
            ds.path    = GetData(set.childNodes, 'path')
            ds.menu    = GetData(set.childNodes, 'menu')
            ds.xsec    = GetData(set.childNodes, 'cross_section')
            ds.ncoll   = GetData(set.childNodes, 'collisions_per_evt')
            ds.project = GetData(set.childNodes, 'AtlasProject')
            ds.version = GetData(set.childNodes, 'AtlasVersion')

            ds_location[ds.name] = ds

        return ds_location


    #-----------------------------------------------------------------------
    # Read Datafile XML info
    #-----------------------------------------------------------------------
    #ds_info = TrigCostXML.ReadDatasetXML(options.ds_xml)
    ds_info = ReadXmlFile(options.ds_xml)

    # check datasets are in ds_info
    for jd in joblist:
        for ds in jd.datasets:                
            if ds not in ds_info.keys() :
                print "ERROR, " + ds + " not found in dataset description xml.  This dataset will be skipped"
                sys.exit(-1)
            else :
                if ds_info[ds].path.find('data') != -1 and jd.dqxml == None :
                    print 'WARNING, running in data ntuples, but no data quality XML is provided'

    #-----------------------------------------------------------------------
    # Run Jobs
    #-----------------------------------------------------------------------
    def RunJob(jd):

        # Holds arguments for runRates.py
        proc_args = ['runRates.py']

        # Build runRates argument list
        for ds in jd.datasets:
            proc_args.append(os.path.join(ds_info[ds].path,ds))

        if not os.path.isdir(options.outdir):
            os.mkdir(options.outdir)
        output_loc = '%s/html_%s' %(options.outdir,jd.name)
        proc_args += ['-o', output_loc]    
        proc_args += ['-k', jd.key]

        # Set the Run Module to a unique location
        #proc_args += ['--runModXML',  ds+'RunModule.xml']

        if options.forwardArgs:
            proc_args.append(options.forwardArgs)

        if jd.bgxml:
            proc_args+=["--data","--do-bg","--bg-scaleXML="+jd.bgxml,"--predict-rates"]
            if jd.dqxml:
                proc_args+=["--dq-xml="+jd.dqxml]
            if jd.predlumi:
                # all the eb config args should be conditional on something (more info in xml?)        
                proc_args+=["--pred-lumi",jd.predlumi]
            if jd.predrun:
                if not jd.predlb:
                    print "Must specify lb with predrun"
                    sys.exit(-1)
                proc_args+=["--pred-run="+jd.predrun]
                proc_args+=["--pred-lb="+jd.predlb]
        else:
            proc_args+=['--xsec',ds_info[ds].xsec]
            proc_args+=['--lumi',jd.predlumi]

        # add prescale xml if desired
        if jd.PSxml:
            proc_args+=["--prescaleXML",jd.PSxml]
            proc_args+=["--default-ps=-1.0"]
            proc_args+=["--default-pt=-1.0"]
        else:
            if jd.predrun:
                # simulating online run only use PS set in config
                proc_args+=["--default-ps=-1.0"]
                proc_args+=["--default-pt=-1.0"]
            else:
                # don't use the menu PS but set them all to 1
                proc_args+=["--default-ps=1.0"]
                proc_args+=["--default-pt=-1.0"]

        # added groups from XML
        if jd.groupxml:
            proc_args+=["--groupXML",jd.groupxml]

        # add overlaps if desired
        if jd.AllOverlap:
            proc_args+=["--do-overlap"  ]
            proc_args+=["--do-unique"   ]
            proc_args+=["--do-bandwidth"]


        # print only for dry run
        command=str(' ').join(proc_args)
        if options.dry_run :
            print command
            return

        # actually run it
        log = os.path.join(output_loc,"runRates.log")
        if not os.path.isdir(output_loc):
            os.mkdir(output_loc)
        log_file = open(log, 'w')
        subprocess.call(string.split(command," "), stdout=log_file, stderr=log_file)

        # add the pie chart to it
        if not options.no_pie:
            AddPieChart(output_loc)

    #-----------------------------------------------------------------------
    # Run Jobs
    #-----------------------------------------------------------------------
    if options.debug:
        RunJob(joblist[0])
    else:
        pool = Pool(processes=4)              # start 4 worker processes
        pool.map(RunJob,joblist)              # run all the jobs


