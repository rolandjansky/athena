# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, subprocess, os, time
import xml.dom.minidom
import TrigMonCosts.TrigCostConfig as TrigCostConfig
from TrigCostPython import RatesTwikiTable 

rate_calculations = [
#                      'data10_7TeV.00158268.physics_EnhancedBias.merge.RAW_01-08-2010_Physicsppv1_AtlasCAFHLT-15.6.9.14.2_TriggerMenuPython-00-11-47_TrigMuonHypo-00-01-31',
#                      'data10_7TeV.00158268.physics_EnhancedBias.merge.RAW_09-08-2010_Physicsppv1_AtlasP1HLT-15.6.9.16_TriggerMenuPython-00-11-59',
#                     'mc09_7TeV.105001.pythia_minbias.digit.RDO.e517_s764_s767_d300_21-06-2010_Physicsppv1_AtlasCAFHLT-rel_0-15.6.X.Y.Z_TMP-00-10-90',
#                     'mc09_7TeV.105001.pythia_minbias.digit.RDO.e517_s764_s767_d300_21-06-2010_Testppv1_AtlasCAFHLT-rel_0-15.6.X.Y.Z_TMP-00-10-90',
#                      'mc09_7TeV.105001.pythia_minbias.digit.RDO.e517_s764_s767_d300_22-08-2010_Physicsppv1_AtlasP1HLT-15.6.9.16',
                       'data10_7TeV.00162882.physics_EnhancedBias.daq.RAW_31-08-2010_Physicsppv1_AtlasCAFHLT-rel_1-15.6.X.Y.Z' 
                    ]

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage='<rates xml> -o <output location>')

p.add_option('-o', '--output', type = 'string', default='', 
             dest = 'outdir', help = 'directory where html directories are placed' )

p.add_option('', '--dataset-xml', type='string', default='../data/TrigMonCosts_ntuple_index.xml',
             dest = 'ds_xml', help = 'Location of dataset description' )

p.add_option('', '--only-groups', action="store_true", default=False,
             dest = 'only_make_groups', help = 'Make group xml with diagonsitc and stop' )

p.add_option('-k', '--key', type='string', default='TrigCost',
             dest = 'key', help = 'Key to match datasets, passed to runRates -k' )

p.add_option('--dq-xml', type='string', default=None,
             dest = 'dq_xml', help = 'Path to data qualtiy xml' )

p.add_option('--max-proc', type='int', default=4,
             dest = 'max_proc', help = 'give the maximum number of processors (SHOULD NOT EXCEED 4!)' )

p.add_option('--dry-run', default=False, action='store_true',
             dest = 'dry_run', help = 'do not run job, but print the command' )

p.add_option('--write-twiki', default=False, action='store_true',
             dest = 'write_twiki', help = 'Add results to rates twiki table' )


(options, args) = p.parse_args()

if len(args) == 1 :
    rate_calculations = [args[0]]
    print rate_calculations


def main() :


    ds_info = ReadXmlFile(options.ds_xml)
    
    # check that the requested datasets are in the xml
    for calc in rate_calculations :
        
        if calc not in ds_info.keys() :
            print "WARNING, " + calc + " not found in dataset description xml.  This dataset will be skipped"
            
            rate_calculations.remove(calc)
  
        else :
            if ds_info[calc].path.find('data') != -1 and options.dq_xml == None :
                print 'WARNING, running in data ntuples, but no data quality XML is provided'


    #now run rates
    files = []
    processes = []
    for calc in rate_calculations :
   
        #holds arguments for TrigCost
        proc_args = []

        output_loc = '%s/html_%s' %(options.outdir, calc)
        proc_args += ['-o', output_loc]

        proc_args += ['-k', options.key]

        #determine luminosity 
        lumi = '1e31'
        if calc.count('7TeV')  > 0 :
            if calc.count('InitialBeam') > 0 :
                lumi = '1e29'
            else :
                lumi = '1e31'
        elif calc.count('900GeV') > 0 :
            lumi = '1e28'
        proc_args += ['--lumi', lumi]

        output = subprocess.Popen(['nsls', ds_info[calc].path+'/'+calc], stdout=subprocess.PIPE).communicate()[0]
        #check for TrigConf.root on castor, if exists, copy it locally and give it to runRates
        if 'TrigConf.root' in output :
            print "Need to use TrigConf"
            s = subprocess.Popen(['xrdcp', 'root://castoratlas/' + ds_info[calc].path + '/'+calc+'/TrigConf.root', '.'])
            s.wait()
            os.system('mv TrigConf.root TrigConf_%s.root' %calc)
            proc_args += ['-c', 'TrigConf_'+calc+'.root']
  
        #if these are data ntuples 
        if ds_info[calc].path.find('data') != -1 :
            proc_args += ['--data']
            if options.ds_xml != None :
                proc_args += ['--dq-xml', options.ds_xml]
       
        else : #these are MC ntuples 
            proc_args += ['--xsec', ds_info[calc].xsec, '--lumi', lumi, '--nColl', ds_info[calc].ncoll]
   
        proc_args += ['--runModXML', 'RunModule_'+calc+'.xml', '--do-overlap', '--do-unique', '--do-bandwidth', '--nevents', '10000'] 
        #proc_args += ['--runModXML', 'RunModule_'+calc+'.xml', '--nevents', '10000' ] 
        
        ds = ds_info[calc].path + '/' + calc
  
        log = calc + '.log'
      
        log_file = open(log, 'w')
#        testarea = os.getenv('TestArea')

        command = ['runRates.py', ds] + proc_args 
        print command
        if options.dry_run :
            print ' '.join(command)
            sys.exit(1)

        processes.append(subprocess.Popen(command, stdout=log_file, stderr=log_file))
        #processes.append(subprocess.Popen(['python', testarea + '/PhysicsNtuple/TrigMonCosts/macros/runRates.py', ds] ))
        
        while len(processes) >= options.max_proc :
            time.sleep(30)
            for proc in processes :
                if proc.poll() != None :
                    processes.remove(proc)

    #wait for remaining jobs
    for proc in processes :
        proc.wait()
            
    AddPieCharts()

    if options.write_twiki :
        for calc in rate_calculations :
            output_loc = '%s/html_%s' %(options.outdir, calc)
            RatesTwikiTable.WriteTwikiTable(calc, '/afs/cern.ch/user/j/jkunkle/www/RatesTable/rates_table_test.twiki',output_loc)

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

def MakeTriggerMenus(ds_info) :
    for calc in rate_calculations :
   
        ds = ds_info[calc].path + '/' + calc
        LVL1Menu = 'Menu_'+calc
   
        alt_args = ['--writeXML', LVL1Menu, '--nevents', '1',
                    '--runModXML', calc+'_runModule_temp.xml']

def AddPieCharts() :

    import makePieCharts
    for calc in rate_calculations :

        output_loc = '%s/html_%s' %(options.outdir, calc)
       
        charts = makePieCharts.MakePieCharts('%s/TriggerCosts.xml' %output_loc, 'BW', output=output_loc) 

        charts_keep = []
        for ch in charts :
            if ch.GetName().find('rates') != -1 :
                charts_keep.append(ch)

        os.system('mv %s/index.html %s/index.html.tmp' %(output_loc, output_loc))

        old_index = open('%s/index.html.tmp' %output_loc, 'r')
        new_index = open('%s/index.html' %output_loc, 'w')

        do_addition = False
        for line in old_index :
            if do_addition == False :
                new_index.write(line)
            else :
                new_index.write('\n')
                new_index.write('<table>\n<tr>\n')
                for ch in charts_keep :
                    new_index.write('<td><a STYLE="text-decoration:none" href="%s.png"><IMG height=180 width=220 src="%s.png"></a></td>\n' %(ch.GetName(), ch.GetName()))
                new_index.write('</tr>\n<tr>\n')
                for ch in charts_keep :
                    new_index.write('<td align="center"><a STYLE="text-decoration:none"  href="%s.png">%s</a></td>' \
                             %(ch.GetName(), ' '.join(ch.GetName().split('_')[0:-1])))
                            
                new_index.write('</tr>\n</table>\n')

                new_index.write(line)
                do_addition = False

            if line.find('TriggerCosts.xml') != -1 :
                do_addition = True
         
        old_index.close()
        new_index.close()
        os.system('rm %s/index.html.tmp' %output_loc)

if __name__ == '__main__' :
    main()
