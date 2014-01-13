#!/usr/bin/env python
import sys, subprocess, os, time, string, copy, re, math
import xml.dom.minidom
from multiprocessing import Pool
from TrigCostPython import TrigCostXML,TrigCostCool
from TrigCostRate.TrigCostRateConfig import search_file
import traceback

# Location of the XML files
xmlpath = os.getenv('XMLPATH')
defaultds = search_file("TrigCostRate/data/TrigEB_ntuple.xml", xmlpath)

#----------------------------------------------------------------------
# Class describing jobs 
#----------------------------------------------------------------------
class JobDesc:    
    def __init__(self) :
        self.datasets=[]
        self.AllOverlap=False
        self.name='test'
        self.ebdataset='TrigEB_ntuple.xml'
        self.PSxml=None
        self.predlumi=None
        self.predrun=None
        self.predlb=None
        self.predmu=None
        self.usetiny=False
        self.defaultps=None
        self.defaultpt=None
        self.groupxml=None
        self.reducehtml=None
        self.doOnlineComp=None
        self.doHistPlotsOnly=None
        self.trpFileName=None
        self.plotLumiCorrect=1.0
        self.rateItems={}
        self.rateLabels={}
        self.rateTitle={}
        self.key='.root'

joblist=[]

if __name__ == '__main__' :

    print "runEB = ",sys.argv[0]
    
    #-----------------------------------------------------------------------
    #  Options definitions
    #-----------------------------------------------------------------------
    from optparse import OptionParser

    p = OptionParser(usage='-o <output location>')

    p.add_option('-i', '--input', type = 'string', default='', 
                 dest = 'indir', help = 'directory where ntuples are located' )

    p.add_option('-o', '--output', type = 'string', default='', 
                 dest = 'outdir', help = 'directory where html directories are placed' )

    p.add_option( '--max-num-files',   type="int",          default=None,  dest="max_num_files",    help="Max number of files to be used. set to -1 for all")    


    p.add_option('', '--forwardArgs', type='string', default=None,
                 dest = 'forwardArgs', help = 'Arguments to forward to runRates.py')


    p.add_option('', '--overlap', default=False, action='store_true',
                 dest = 'overlap', help = 'calculate overlaps' )

    p.add_option('', '--pred-run', type='int', default=None,
                 dest = 'predrun', help = 'run number for runRates.py')

    p.add_option('', '--pred-lb', type='int', default=None,
                 dest = 'predlb', help = 'lumi block number for runRates.py')
    
    p.add_option('', '--lb-beg', type='int', default=None,
                 dest = 'lbbeg', help = 'first lumi block number for rates in data for runRates.py')
    
    p.add_option('', '--lb-end', type='int', default=None,
                 dest = 'lbend', help = 'last lumi block number for rates in data for runRates.py')

    p.add_option('', '--pred-lumi', type='float', default=None,
                 dest = 'predlumi', help = 'luminosity value for runRates.py')

    p.add_option('', '--pred-mu', type='float', default=None,
                 dest = 'predmu', help = 'pile-up mu value for runRates.py')
    
    p.add_option('', '--pileupVtxEffA', type='float', default=None,
                 dest = 'pileupVtxEffA', help = 'pile-up efficiency parameter a')
    
    p.add_option('', '--pileupVtxEffB', type='float', default=None,
                 dest = 'pileupVtxEffB', help = 'pile-up efficiency parameter b')

    p.add_option('', '--max-proc', type='int', default=4,
                 dest = 'max_proc', help = 'give the maximum number of processors (SHOULD NOT EXCEED 4!)' )

    p.add_option('', '--dry-run', default=False, action='store_true',
                 dest = 'dry_run', help = 'do not run job, but print the command' )

    p.add_option('-d', '--debug', default=False, action='store_true',
                 dest = 'debug', help = 'go to single process for easier debuging' )

    p.add_option('-l', '--joblist', type='string', default='',
                 dest = 'joblistname', help = 'Specify the job list' )
    
    p.add_option('-v', '--verbose', default=False, action='store_true',
                 dest = 'verbose', help = 'Show commands, show lb in comparisons labels' )
                 
    p.add_option('', '--compRates-css', type='string', default='',
                 dest = 'compRatesCss', help = 'Location of css file for compRates.py' )

    p.add_option('', '--compRates-js', type='string', default='',
                 dest = 'compRatesJs', help = 'Location of js file for compRates.py' )

    p.add_option('', '--compRates-favicon', type='string', default='',
                 dest = 'compRatesFavicon', help = 'Location of favicon file for compRates.py' )

    p.add_option('', '--compRates-snippet', type='string', default='',
                 dest = 'compRatesSnippet', help = 'Location of HTML snippet file for compRates.py' )

    p.add_option('-k', '--file-key', type='string', default='TrigRate',
                 dest = 'file_key', help = 'File Key usually TrigRate but may need .root for some cases' )    
    
    p.add_option('', '--useSubset', type='float', default=1,
                 dest = 'useSubset', help='only process subset of lumiblock, number must be between 0 and 1' )

    

    (options, args) = p.parse_args()

    #-----------------------------------------------------------------------
    # Fill job list
    #-----------------------------------------------------------------------
    import EBJobList
    EBJobList.FillJobList(joblist,options.joblistname)

    #-----------------------------------------------------------------------
    # Read the Dataset XML file
    #-----------------------------------------------------------------------
    class dataset_descr :

        def __init__(self) :
            pass

        def __eq__(this, rhs) :
            return self.name == rhs

        name = ''
        filename = ''
        path = ''
        bgxml = ''
        dqxml = ''
        muxml = ''
        tinyxml = ''

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

            ds.name       = GetData(set.childNodes, 'name')
            ds.filename   = GetData(set.childNodes, 'filename')
            ds.path       = GetData(set.childNodes, 'path')
            ds.bgxml      = GetData(set.childNodes, 'bgxml')
            ds.dqxml      = GetData(set.childNodes, 'dqxml')
            ds.muxml      = GetData(set.childNodes, 'muxml')
            ds.tinyxml    = GetData(set.childNodes, 'tinyxml')

            ds_location[ds.name] = ds

        return ds_location

    #-----------------------------------------------------------------------
    # Run Jobs
    #-----------------------------------------------------------------------
    def RunJob(jd):
    
        # Order of tasks
        #   Get rates from data (trp)
        #   Obtain predictions
        #   Compare rates to predictions
        #   Make plots
    
        # define the prediction run or lumi
        if options.predlumi:
            pred_lumi = str(options.predlumi)
        elif jd.predlumi:
            pred_lumi = jd.predlumi

        lb_beg = None
        lb_end = None
        if options.lbbeg and options.lbend:
            if options.lbbeg>options.lbend:
                print "Warning, data-lb-beg>data-lb-end.  These values will be swapped."
                lb_beg = str(options.lbend)
                lb_end = str(options.lbbeg)
            else:
                lb_beg = str(options.lbbeg)
                lb_end = str(options.lbend)
        elif options.predlb:
            lb_beg = str(options.predlb)
            lb_end = str(options.predlb)
        pred_lumiblock = lb_beg
		
        # Get run
        if options.predrun:
            pred_run = str(options.predrun)
        elif jd.predrun:
            pred_run = str(jd.predrun)
        
        # Get mu
        if options.predmu:
            pred_mu = str(options.predmu)
        elif jd.predmu:
            pred_mu = str(jd.predmu)

        # Define the output directory
        if not os.path.isdir(options.outdir):
            os.mkdir(options.outdir)
        out_name = jd.name
        if(jd.name == None):
            out_name = "Rates"
        output_loc = '%s/%s' %(options.outdir,out_name)
        
        # Get pred lumi
        if options.predlumi or jd.predlumi:
            output_loc += '_%s' %(pred_lumi)
        if options.predrun or jd.predrun:
            output_loc += '_%s_%s' %(pred_run, lb_beg)
        if options.predmu or jd.predmu:
            output_loc += '_mu%s' %(pred_mu)

        # Create logfile
        log = os.path.join(output_loc,"runRates.log")
        if not os.path.isdir(output_loc):
            os.mkdir(output_loc)
        log_file = open(log, 'w')

        # Read Datafile XML info
        # first check for file locally
        fileloc = jd.ebdataset
        EBfilename=''
        if os.path.exists(fileloc):
            EBfilename=fileloc
        else:
#            # then look on castor
#            castorarea='/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/'
#            fileloc = castorarea+jd.ebdataset

#            from subprocess import Popen, PIPE
#            output = Popen(['/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select','ls',fileloc], stdout=PIPE).communicate()

#            if output[0] != '':
#                EBfilename=output_loc+"/"+jd.ebdataset
#                if os.path.exists(EBfilename):
#                    os.remove(EBfilename)
#                castor_cmd = 'xrdcp root://eosatlas/'+fileloc+' '+output_loc
#                subprocess.call(string.split(castor_cmd," "), stdout=log_file, stderr=log_file)
#            else:
             # then look in release
             fileloc = "TrigCostRate/"+jd.ebdataset
             EBfilename = search_file(fileloc, xmlpath)

        print 'Using file '+EBfilename
        ds_info = ReadXmlFile(EBfilename)

        if (jd.predrun or options.predrun):
            trp_name = 'output_%s_%s_%s.xml'%(pred_run,lb_beg,lb_end)
            trp_file_name = output_loc+"/"+trp_name

        # Commands for trp
        command_trp = ''
        if jd.doOnlineComp and (jd.predrun or options.predrun):
            proc_args_trp = ['exampleCost.py --trp --dorates']
            proc_args_trp += ["--run "+str(pred_run)+" --lb-beg="+str(lb_beg)+" --lb-end="+str(lb_end)]
            proc_args_trp += ["--writexml "+output_loc+"/output.xml"]
            command_trp=str(' ').join(proc_args_trp)

        # Get rates from data (trp)
        if jd.doOnlineComp and (jd.predrun or options.predrun):
            if options.dry_run:
                print 'command_trp:'
                print command_trp            
                print ''
            else:
                if options.verbose:
                    print command_trp            
                    print ''
                print "TRP data fetch command: ",string.split(command_trp," ")
                subprocess.call(string.split(command_trp," "), stdout=log_file, stderr=log_file)
        
        
        # Print warning for dry run (only for using ranges of lb)
        if options.dry_run:
            if lb_beg and lb_end:
                if lb_beg != lb_end:
                    print 'Note: There is a range of lumiblocks when getting rates from data.'
                    print 'TRP splits lumiblock ranges by prescale keys.  As a result the lumiblocks in the dry run for the following commands may not match the lumiblocks that are actually used.'
        
        
        # Get pred_lumiblock
        lb_start  = -1
        lb_finish = -1
        if lb_beg and lb_end:
            if lb_beg == lb_end:
                pred_lumiblock = lb_beg
            else:
                list = os.listdir(output_loc)
                lb_starts   = []
                lb_finishes = []
                lb_ns       = []
                
                max_n = -1
                # Populate arrays
                for file in list:
                    if 'output' in file and 'xml' in file:
                        parts = file.split('_')
                        lb_start  = int(parts[2])
                        lb_finish = int(parts[3].split('.')[0])
                        lb_n      = lb_finish-lb_start+1
                        lb_starts.append(lb_start)
                        lb_finishes.append(lb_finish)
                        lb_ns.append(lb_n)
                # Find best range
                for i in range(len(lb_ns)):
                    if lb_ns[i]>max_n:
                        max_n = lb_ns[i]
                        lb_start  = lb_starts[i]
                        lb_finish = lb_finishes[i]
                        pred_lumiblock = str(int(0.5*(lb_start+lb_finish)))
        pred_lumiblock = str(pred_lumiblock)
        if lb_start==-1 and lb_finish==-1:
            lb_start  = pred_lumiblock
            lb_finish = pred_lumiblock
        
        # Update trp_file_name
        if (jd.predrun or options.predrun):
            trp_name = 'output_%s_%s_%s.xml'%(pred_run,lb_start,lb_finish)
            trp_file_name = output_loc+"/"+trp_name

        # Commands for predictions
        proc_args_pred = ['runRates.py']
        # Build runRates argument list

        # Get input file from command line or XML
        job_name = None
        input_file = None
        if(options.indir):
            input_file = options.indir
            # File the correct run number in XML file for this input file
            for ds_name, ds in ds_info.items():
                if(input_file.find(ds_name) is not -1):
                    job_name = ds_name
        elif(jd.name):
            input_file = os.path.join(ds_info[jd.name].path,ds_info[jd.name].filename)
            job_name = jd.name
        if(job_name == None or input_file == None):            
            print "Error.  No Input file or dataset in XML"
            print "jd.name=",jd.name
            print "options.indir=",options.indir
            print "job_name=",job_name
            print "input_file=",input_file
            return

        proc_args_pred.append(input_file)

        proc_args_pred += ["--useSubset="+str(options.useSubset)]
        
        proc_args_pred += ['-o', output_loc]    
        if(jd.key):
            proc_args_pred += ['-k', jd.key]
        

        if ds_info[job_name].bgxml:
            proc_args_pred+=["--data","--predict-rates"]
            if(jd.usetiny):
                proc_args_pred+=["--do-bg"]
                # a hack for now, will need to be removed when --do-bg-fromXML is no longer the default
                #ds_info[job_name].bgxml = "enhanced_bias.xml"
            else:
                proc_args_pred+=["--do-bg-fromXML"]

            # search locally first for BG file
            if os.path.exists(ds_info[job_name].bgxml):
                bgfile = ds_info[job_name].bgxml
            else:
                bgfile = search_file("TrigCostRate/"+ds_info[job_name].bgxml, xmlpath)

            proc_args_pred+=["--bg-scaleXML="+bgfile]
            if ds_info[job_name].muxml != '':
                proc_args_pred+=["--readTiny-lb-mu-xml="+ds_info[job_name].muxml]
            if ds_info[job_name].tinyxml != '':
                proc_args_pred+=["--readTiny-eos-xml="+ds_info[job_name].tinyxml]

            if ds_info[job_name].dqxml:
                dqfile = search_file("TrigCostRate/"+ds_info[job_name].dqxml, xmlpath)
                print "TrigCostRate/"+ds_info[job_name].dqxml, xmlpath
                proc_args_pred+=["--dq-xml="+dqfile]

            if options.predlumi or jd.predlumi:
                proc_args_pred+=["--pred-lumi="+pred_lumi]

            if options.predrun or jd.predrun:
                proc_args_pred+=["--pred-run="+pred_run]
                proc_args_pred+=["--pred-lb="+pred_lumiblock]
        else:
            proc_args_pred+=['--xsec',ds_info[job_name].xsec]
            proc_args_pred+=['--lumi',jd.predlumi]

        # Set the Run Module to a unique location
        #proc_args += ['--runModXML',  ds+'RunModule.xml']

        if jd.reducedhtml:
            proc_args_pred+=["--do-reduced-html"]

        if options.max_num_files:
            print "Max files = ",options.max_num_files
            proc_args_pred+=["--max-num-files",str(options.max_num_files)]

        if options.forwardArgs:
            print "ForwardArgs= ",string.split(options.forwardArgs," ")
            proc_args_pred+=string.split(options.forwardArgs," ")
        
        # add prescale xml if desired
        default_ps = -1.0
        default_pt = -1.0
        if jd.PSxml:
            proc_args_pred+=["--prescaleXML=",jd.PSxml]
            default_ps = -1.0
            default_pt = -1.0
        else:
            if jd.predrun or options.predrun:
                # simulating online run only use PS set in config
                default_ps = -1.0
                default_pt = -1.0
            else:
                # don't use the menu PS but set them all to 1
                default_ps = 1.0
                default_pt = -1.0
        if jd.defaultps:
            default_ps = jd.defaultps
        if jd.defaultpt:
            default_pt = jd.defaultpt
        
        proc_args_pred+=["--default-ps="+str(default_ps)]
        proc_args_pred+=["--default-pt="+str(default_pt)]
        
        # run pile-up predictions
        if options.predmu or jd.predmu:
            proc_args_pred+=["--doPileup --nColl="+pred_mu]
            proc_args_pred+=["--pileupVtxEffA=%.4g"%options.pileupVtxEffA]
            proc_args_pred+=["--pileupVtxEffB=%.4g"%options.pileupVtxEffB]
            
            proc_args_pred+=["--pileupWeightFilename="+output_loc+"/pileupWeights.root"]
            if ds_info[job_name].muxml != '':
                proc_args_pred+=["--readTiny-lb-mu-xml="+ds_info[job_name].muxml]
            if ds_info[job_name].tinyxml != '':
                proc_args_pred+=["--readTiny-eos-xml="+ds_info[job_name].tinyxml]
                proc_args_pred+=["--readTiny-bcid-xml="+ds_info[job_name].tinyxml]

        # added groups from XML
        if jd.groupxml:
            proc_args_pred+=["--groupXML",jd.groupxml]

        # add overlaps if desired
        if jd.AllOverlap or options.overlap:
            proc_args_pred+=["--do-overlap"  ]
            proc_args_pred+=["--do-unique"   ]
            #proc_args_pred+=["--do-bandwidth"]
            
        # Get pred_lb
        if options.predrun:
            if options.predlb:
                pred_lumiblock = options.predlb
            elif not (options.lbbeg and options.lbend):
                print "Must specify lb with predrun"
                return
        elif jd.predrun:
            if not jd.predlb:
                print "Must specify lb with predrun"
                return
            pred_lumiblock = jd.predlb
        
        command_pred=str(' ').join(proc_args_pred)
                
        if options.dry_run:
            print 'Command pred: '
            print command_pred
            print ''
        else:
            if options.verbose:
                print command_pred
                print ''
            subprocess.call(string.split(command_pred," "), stdout=log_file, stderr=log_file)
        

        # Commands for comparisons
        command_comp = ''
        if jd.doOnlineComp and (jd.predrun or options.predrun):
            proc_args_comp = ['compRates.py']
            proc_args_comp += [output_loc+"/TriggerCosts.xml"]
            proc_args_comp += [trp_file_name]
            proc_args_comp += ["-o"+output_loc+"/compare"]
            
            if options.compRatesCss:
                proc_args_comp += ['--css=%s/style.css'%options.compRatesCss]
            if options.compRatesJs:
                proc_args_comp += ['--js-prefix=%s'%options.compRatesJs]
            if options.compRatesFavicon:
                proc_args_comp += ['--favicon=%s'%options.compRatesFavicon]
            if options.compRatesSnippet:
                proc_args_comp += ['--html-snippet=%s'%options.compRatesSnippet]
                
            if options.verbose:
                proc_args_comp += ["--labels=PRED (run %s lb %s),ONLINE (run %s lb %s %s)"%(pred_run,pred_lumiblock,pred_run,lb_start,lb_finish)]
            else:
                proc_args_comp += ["--labels=PRED,ONLINE"]
            proc_args_comp += ["--ratio-columns=2"] 
            proc_args_comp += ["--show-err"]
            proc_args_comp += ["--show-diff"]
            command_comp=str(' ').join(proc_args_comp)
            if options.dry_run:
                print 'command_comp:'
                print command_comp
                print ''
            else:
                if options.verbose:
                    print command_comp
                    print ''
                # Pass the array to subprocess to parse the arguments correctly
                print "compRates command: proc_args_comp"
                subprocess.call(proc_args_comp, stdout=log_file, stderr=log_file)

        # Commands for plots
        command_plot_list = []
        if (jd.doOnlineComp or jd.doHistPlotsOnly) and (jd.predrun or options.predrun):

            if(jd.trpFileName != None):
                trp_file_name = jd.trpFileName

            # Get the luminosity
            lbset = TrigCostCool.GetLumiblocks(int(pred_run),int(pred_lumiblock),int(pred_lumiblock),'')
            mean_lumi = "\\\\ Luminosity = %4.1e cm^-2 s^-1" % (lbset.GetDeliveredLumi()/lbset.GetTotalTime()*1e30)
            mean_lumi += "\\\\ ATLAS Preliminary"

            lumi_corr = str(jd.plotLumiCorrect)
            proc_args_plot = ['histRates.py']
            proc_args_plot += [output_loc+"/TriggerCosts.xml"]
            proc_args_plot += [trp_file_name]
            # Online rates from trp added later
            
            proc_args_plot += ["--labels=PRED,ONLINE"]
            proc_args_plot += ["--leftmargin=0.4"]
            proc_args_plot += ["--lumicorrect",lumi_corr]

            for type,items in jd.rateItems.iteritems():
                command_plot_tmp = copy.deepcopy(proc_args_plot)
                command_plot_tmp += ["-o",output_loc+"/compare/"+type+".set"]
                command_plot_tmp += ["--plotlabel=\'"+jd.rateTitle[type]+mean_lumi+"\'"]
                command_plot_tmp += ["--ratelist="+items]
                command_plot_tmp += ["--ratenames="+jd.rateLabels[type]]
                
                command_plot_list.append(command_plot_tmp)
        
            if options.dry_run:
                print 'command_plot:'
                for command_plot in command_plot_list:
                    print string.join(command_plot)
                    print ''
            else:
                # Make histograms
                for command_plot in command_plot_list:
                    command_plot[2] = trp_file_name
                    if options.verbose:
                        print command_plot
                        print ''
                    print "Plotting :",command_plot
                    subprocess.call(command_plot, stdout=log_file, stderr=log_file)
        
        
        
    #-----------------------------------------------------------------------
    # Run Jobs
    #-----------------------------------------------------------------------
    if options.debug:
        RunJob(joblist[0])
    else:
        try:
            pool = Pool(processes=4)              # start 4 worker processes
            pool.map(RunJob,joblist)              # run all the jobs
        except Exception,e:
            print
            print "--------------------------------------------------------------------------------"
            print
            print "You got an exception:"
            print "Running with --debug will make it much easier to figure out where it came from"
            print
            print "--------------------------------------------------------------------------------"
            print
            traceback.print_exc(e)
            raise e

