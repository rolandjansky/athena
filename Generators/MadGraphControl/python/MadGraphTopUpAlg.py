from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode
import glob,subprocess,os,time,shutil

__author__ = 'Zach Marshall <ZLMarshall@lbl.gov>'

class MadGraphTopUpAlg(EvgenAlg):
    """
    A python-only algorithm for running more events in MadGraph
    """

    def __init__(self, name="MG_TopUp", topUpSvcName="EvgenOTFTopUpSvc", threshold=50):
        super(MadGraphTopUpAlg,self).__init__(name=name)
        self.topUpSvcName = topUpSvcName
        self.topUpSvc = None
        self.threshold = threshold

        self.njobs = 1
        if 'ATHENA_PROC_NUMBER' in os.environ:
            self.njobs = os.environ['ATHENA_PROC_NUMBER']
        self.fileList = []

    def initialize():
        self.topUpSvc = getattr(svcMgr, self.topUpSvcName, None)
        if self.topUpSvc is not None:
            self.msg.info('Got the top up service')
        else:
            self.msg.warning( 'Could not get the top up service!  Will be a null-op.' )
        if self.njobs>1:
            self.msg.info('Lucky you - you are running on a full node queue.  Will re-configure for '+str(self.njobs)+' jobs.')
        return StatusCode.Success

    def execute():
        # If we did not manage to get the top up service, just get out of here
        if self.topUpSvc is None: return StatusCode.Success

        # See if it's time to make more events
        if float(self.topUpSvc.getNPerFile()-self.topUpSvc.getNUsedSoFar())*self.topUpSvc.getEfficiency()>self.threshold:
            # Plenty of events left - get out of here
            return StatusCode.Success

        # Not enough events!  Top it up!
        e_to_generate = self.topUpSvc.getNPerFile()

        a_dir = None
        # Call another generate - first find out where to go
        if os.access('madevent',os.R_OK):
            self.msg.info( 'Found a grid pack at madevent/ - using it for generation' )
            a_dir = 'madevent'
        elif len( glob.glob( '*PROC*' ) )==0:
            self.msg.error( 'Need to re-run, but cannot find directory for running!' )
            return StatusCode.Failure
        else:
            a_dir = sorted( glob.glob( '*PROC*' ) )[-1]
            self.msg.info( 'Running from process directory '+a_dir )

        currdir=os.getcwd()
        os.chdir(a_dir)

        # Modify the random number seed - increment by 2M
        oldcard_f = open('Cards/run_card.dat','r')
        oldlines = oldcard_f.readlines()
        oldcard_f.close()
        newcard = open('Cards/run_card.dat','w')
        for line in oldlines:
            if ' nevents ' in line:
                newcard.write('  %i       = nevents ! Number of unweighted events requested \n'%(self.topUpSvc.getNPerFile()))
            elif ' iseed ' in line:
                old_seed = int( line.split()[0] )
                newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(old_seed+2000000))
            else:
                newcard.write(line)
        oldcard.close()
        newcard.close()
        return run_card_new

        # Do the actual generation - should we use the generate* functions in MGC_Utils directly?
        self.msg.info( 'Started generating at '+str(time.asctime()) )
        if self.njobs>1:
            self.msg.info('Running parallel generation.  Should be nice and fast.')
            generate = subprocess.Popen(['bin/generate_events',str(mode),str(self.njobs),'OTFTopUp'],stdin=subprocess.PIPE)
        else:
            self.msg.info('Running serial generation.  This will take a bit more time than parallel generation.')
            generate = subprocess.Popen(['bin/generate_events','0','OTFTopUp'],stdin=subprocess.PIPE)
        generate.wait()
        self.msg.info( 'Finished generating at'+str(time.asctime()) )

        # Move the events to something local...
        a_new_file_name = 'otf_lhe.events'
        if   len( glob.glob( currdir+'/*events.[*0-9]' ) )>0:
            a_new_file_name = sorted( glob.glob( currdir+'/*events.[*0-9]' ) )[-1]
            a_new_file_name = a_new_file_name[:a_new_file_name.rfind('.')]+str( int(a_new_file_name.split('.')[-1])+1 )
        elif len( glob.glob( currdir+'/*events' ) )>0:
            a_new_file_name = sorted( glob.glob( currdir+'/*events' ) )[-1]+'.1'

        # Make sure we have trimmed off any directory names
        if '/' in a_new_file_name: a_new_file_name = a_new_file_name.split('/')[-1]

        # Unzip the files and move them in place
        unzip = subprocess.Popen(['gunzip','Events/OTFTopUp/unweighted_events.lhe.gz'])
        unzip.wait()
        shutil.move('Events/OTFTopUp/unweighted_events.lhe',currdir+'/'+a_new_file_name)

        # Go back
        os.chdir(currdir)

        # Remove the oldest file to save disk space
        self.fileList += [ a_new_file_name ]
        if len(self.fileList)>2:
            # One old file, one new file, and delete the third
            self.msg.info('Removing old file '+self.fileList[-3])
            if os.access( currdir+'/'+self.fileList[-3] , os.R_OK ):
                remove_old = subprocess.Popen(['rm',(currdir+'/'+self.fileList[-3])])
                remove_old.wait()

        # Pass the information back to the top up service
        self.topUpSvc.newFile( a_new_file_name )

        return StatusCode.Success

