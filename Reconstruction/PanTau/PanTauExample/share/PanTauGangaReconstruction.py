# @file: PanTauGangaReconstruction.py
# @purpose: generic job submission script to run PanTau reconstruction with Ganga

__doc__    = "generic job submission script to run PanTau reconstruction with Ganga"
__author__ = "Robindra Prabhu"
__version__= "$Revision: 478539 $"


# --- Job modifiers: ---

nameprefix="PanTau_"              # output AOD prefix
namesuffix=".recon.AOD.v15040000" # output AOD suffix
numevents_outputfiles=500         # number of desired events in output AODs

# --- Lists of datasets on the NAF: ---

ztautau = ['misal1_mc12.005179.ZtautauNoEF.digit.RDO.v12000605_tid013376'
           ]
wtauhad = ['misal1_csc11.005107.pythia_Wtauhad.digit.RDO.v12003103_tid003860',
           'misal1_csc11.005107.pythia_Wtauhad.digit.RDO.v12003103_tid004537'
           ]
su1     = ['misal1_csc11.005401.SU1_jimmy_susy.digit.RDO.v12003107_tid006467',
           'misal1_csc11.005401.SU1_jimmy_susy.digit.RDO.v12003107_tid008572',
           ]
su3     = ['misal1_csc11.005403.SU3_jimmy_susy.digit.RDO.v12003107_tid006463',
           'misal1_csc11.005403.SU3_jimmy_susy.digit.RDO.v12003107_tid006468'
           ]
j0      = ['misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103/',
           #'misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103_tid004079',
           #'misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103_tid010585',
           #'misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103_tid016351',
           #'misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103_tid016352',
           #'misal1_csc11.005009.J0_pythia_jetjet.digit.RDO.v12003103_tid018026'   
           ]
j1      = ['ideal2_mc12.005010.J1_pythia_jetjet.digit.RDO.v13003004_tid018228',
           'ideal2_mc12.005010.J1_pythia_jetjet.digit.RDO.v13003004_tid018229'
           ]
j2      = ['ideal2_mc12.005011.J2_pythia_jetjet.digit.RDO.v13003004_tid018733',
           'ideal2_mc12.005011.J2_pythia_jetjet.digit.RDO.v13003004_tid018735'          
           ]
j3      = [#'ideal2_mc12.005012.J3_pythia_jetjet.digit.RDO.v13003004/'
           'ideal2_mc12.005012.J3_pythia_jetjet.digit.RDO.v13003004_tid018737',
           #'ideal2_mc12.005012.J3_pythia_jetjet.digit.RDO.v13003004_tid018738',
           'ideal2_mc12.005012.J3_pythia_jetjet.digit.RDO.v13003004_tid018740'
           ]

sample_list = []
sample_list.append( ztautau )
#sample_list.append( wtauhad )
#sample_list.append( su1 )
#sample_list.append( su3 )
#sample_list.append( j0 )
#sample_list.append( j1 )
#sample_list.append( j2 )
#sample_list.append( j3 )

geometryTag=''
numfilespersubjob=0

for sample in sample_list:
    
    nametag = sample[0].split('.')[2]
    
    if sample is j1 or sample is j2 or sample is j3:
        geometryTag = 'DetDescrVersion="ATLAS-CSC-02-00-00"'
        numfilespersubjob = int(numevents_outputfiles)/int(25)
    else:
        geometryTag = 'DetDescrVersion="ATLAS-CSC-01-02-00"'
        numfilespersubjob = int(numevents_outputfiles)/int(50)

    # --- Job application: ---

    j = Job()
    j.name="PanTauReco_"
    j.application=Athena()
    j.application.option_file=['$TestArea/Reconstruction/PanTau/PanTauExample/share/PanTau_topOptions.py']
    #j.application.options = '-d'
    j.application.options = '-c'+geometryTag
    j.application.exclude_from_user_area=["*.o","*.root*","*.exe"]
    j.application.prepare_old(athena_compile=False)
    #j.application.max_events=2
    
    # --- Inputdata options: ---
    
    j.inputdata=DQ2Dataset()    
    j.inputdata.dataset= sample

    # --- Outputdata options: ---

    #j.outputdata=ATLASOutputDataset()
    j.outputdata=DQ2OutputDataset()
    j.outputdata.outputdata=['AOD.pool.root']
    #j.outputdata.location = '/scratch/current/atlas/prabhu/'
    #j.outputdata.location = 'DESY-HH_SCRATCHDISK'
    j.outputdata.location = 'DESY-HH_LOCALGROUPDISK'
    j.outputdata.use_shortfilename = False
    j.outputdata.datasetname = nameprefix + nametag + namesuffix

    # --- Splitter options: ---

    j.splitter=AthenaSplitterJob()
    j.splitter.numfiles_subjob=numfilespersubjob
    #j.splitter.match_subjobs_files=True
    #j.splitter.numsubjobs=2

    # --- Merger options: ---
    
    #j.merger=AthenaOutputMerger()

    # --- Backend options: ---

    #j.backend=Local()
    j.backend=SGE()

    # --- Job submission: ---
    
    j.name = j.name + nametag
    print 'submitting ', j.name
    j.submit()
