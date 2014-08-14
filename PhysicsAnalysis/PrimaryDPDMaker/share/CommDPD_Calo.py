##-----------------------------------------------------------------------------
## Name: CommDPD_Calo.py
##
## Author: John Morris
## Email:  john.morris@cern.ch
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the Calo commissioning/cosmics DPD.
##
##-----------------------------------------------------------------------------

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteCaloCommStream.StreamName
fileName   = buildFileName( primDPD.WriteCaloCommStream )
if primDPD.isVirtual() == False:
    CaloCommStream = MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    CaloCommStream = MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    


if primDPD.ApplySkimming() :
    # Have to include this jobOption to set up the trigger info!!!
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

    # Now, define the trigger fitlers
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter

    # The jet triggers
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J0')
    topSequence.CaloCommStream_TriggerFilter_L1_J0.trigger = 'L1_J5_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J1')
    #topSequence.CaloCommStream_TriggerFilter_L1_J1.trigger = 'L1_J10_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J2')
    #topSequence.CaloCommStream_TriggerFilter_L1_J2.trigger = 'L1_J10_win6_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J3')
    topSequence.CaloCommStream_TriggerFilter_L1_J3.trigger = 'L1_J15_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J4')
    topSequence.CaloCommStream_TriggerFilter_L1_J4.trigger = 'L1_J30_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J5')
    topSequence.CaloCommStream_TriggerFilter_L1_J5.trigger = 'L1_J55_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J6')
    topSequence.CaloCommStream_TriggerFilter_L1_J6.trigger = 'L1_J75_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_J7')
    #topSequence.CaloCommStream_TriggerFilter_L1_J7.trigger = 'NOT_USED'

    # The forward jet triggers
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_FJ0')
    topSequence.CaloCommStream_TriggerFilter_L1_FJ0.trigger = 'L1_FJ3_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_FJ1')
    topSequence.CaloCommStream_TriggerFilter_L1_FJ1.trigger = 'L1_FJ18_EMPTY'


    # The tau triggers
    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU0')  # Not used
    #topSequence.CaloCommStream_TriggerFilter_L1_TAU0.trigger = 'L1_TAU5_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU1') # Not used
    #topSequence.CaloCommStream_TriggerFilter_L1_TAU1.trigger = 'L1_TAU6_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU2') # Not used
    #topSequence.CaloCommStream_TriggerFilter_L1_TAU2.trigger = 'L1_TAU6I_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU3') # Not used
    #topSequence.CaloCommStream_TriggerFilter_L1_TAU3.trigger = 'L1_TAU8_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU4') # Not used
    #topSequence.CaloCommStream_TriggerFilter_L1_TAU4.trigger = 'L1_TAU11_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU5')
    #topSequence.CaloCommStream_TriggerFilter_L1_TAU5.trigger = 'L1_TAU11I_EMPTY'
    
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU6')
    topSequence.CaloCommStream_TriggerFilter_L1_TAU6.trigger = 'L1_TAU20_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TAU7')
    topSequence.CaloCommStream_TriggerFilter_L1_TAU7.trigger = 'L1_TAU30_EMPTY'


    # The EM triggers
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM0')
    topSequence.CaloCommStream_TriggerFilter_L1_EM0.trigger = 'L1_EM3_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM1') # Not used
    #topSequence.CaloCommStream_TriggerFilter_L1_EM1.trigger = 'L1_EM4_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM2')
    topSequence.CaloCommStream_TriggerFilter_L1_EM2.trigger = 'L1_EM5_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM3')
    topSequence.CaloCommStream_TriggerFilter_L1_EM3.trigger = 'L1_EM5I_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM4')
    topSequence.CaloCommStream_TriggerFilter_L1_EM4.trigger = 'L1_EM10_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM5')
    topSequence.CaloCommStream_TriggerFilter_L1_EM5.trigger = 'L1_EM10I_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM6')
    topSequence.CaloCommStream_TriggerFilter_L1_EM6.trigger = 'L1_EM14_EMPTY'

    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_EM7')
    #topSequence.CaloCommStream_TriggerFilter_L1_EM7.trigger = 'NOT_USED'

    # The transverse energy triggers
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TE0')
    topSequence.CaloCommStream_TriggerFilter_L1_TE0.trigger = 'L1_TE150_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_TE1')
    topSequence.CaloCommStream_TriggerFilter_L1_TE1.trigger = 'L1_TE360_EMPTY'

    # The jet energy triggers
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_JE0')
    topSequence.CaloCommStream_TriggerFilter_L1_JE0.trigger = 'L1_JE120_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_JE1')
    topSequence.CaloCommStream_TriggerFilter_L1_JE1.trigger = 'L1_JE280_EMPTY'

    # The missing energy triggers
    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_XE0')
    topSequence.CaloCommStream_TriggerFilter_L1_XE0.trigger = 'L1_XE10_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_XE1')
    topSequence.CaloCommStream_TriggerFilter_L1_XE1.trigger = 'L1_XE15_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_XE2')
    topSequence.CaloCommStream_TriggerFilter_L1_XE2.trigger = 'L1_XE20_EMPTY'

    topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_XE3')
    topSequence.CaloCommStream_TriggerFilter_L1_XE3.trigger = 'L1_XE30_EMPTY'


    # This is a placeholder for other triggers
    #topSequence += TriggerFilter('CaloCommStream_TriggerFilter_L1_')
    #topSequence.CaloCommStream_TriggerFilter_L1_.trigger = 'L1_'


    # Now define the prescales
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler

    # Define prescale for L1_J0
    topSequence += PrimaryDPDPrescaler("CaloCommStream_Prescale_L1_J0")
    topSequence.CaloCommStream_Prescale_L1_J0.AcceptAlgs += ["CaloCommStream_TriggerFilter_L1_J0"]
    topSequence.CaloCommStream_Prescale_L1_J0.Prescale = 20
    
    # Define prescale for L1_J3    
    topSequence += PrimaryDPDPrescaler("CaloCommStream_Prescale_L1_J3")
    topSequence.CaloCommStream_Prescale_L1_J3.AcceptAlgs += ["CaloCommStream_TriggerFilter_L1_J3"]
    topSequence.CaloCommStream_Prescale_L1_J3.Prescale = 2
    

    # Define prescale for L1_EM0
    topSequence += PrimaryDPDPrescaler("CaloCommStream_Prescale_L1_EM0")
    topSequence.CaloCommStream_Prescale_L1_EM0.AcceptAlgs += ["CaloCommStream_TriggerFilter_L1_EM0"]
    topSequence.CaloCommStream_Prescale_L1_EM0.Prescale = 10
    
    
    # Define prescale for L1_Tau4
    topSequence += PrimaryDPDPrescaler("CaloCommStream_Prescale_L1_TAU6")
    topSequence.CaloCommStream_Prescale_L1_TAU6.AcceptAlgs += ["CaloCommStream_TriggerFilter_L1_TAU6"]
    topSequence.CaloCommStream_Prescale_L1_TAU6.Prescale = 10
    
    

    # This is a placeholder for other prescalers
    #topSequence += PrimaryDPDPrescaler("CaloCommStream_Prescale_L1_")
    #topSequence.CaloCommStream_Prescale_L1_.AcceptAlgs += [""]
    #topSequence.CaloCommStream_Prescale_L1_.Prescale = 


    # Here, all the filters that should be used are listed.
    # AcceptAlgs is a logical OR of all listed algorithms
    # RequireAlgs is a logical AND of all listed algorithms
    # For more complex combinations, please use the LogicalFilterCombiner
    CaloCommStream.AcceptAlgs( [ "CaloCommStream_Prescale_L1_J0",
                                 #"CaloCommStream_TriggerFilter_L1_J1",
                                 #"CaloCommStream_TriggerFilter_L1_J2",
                                 "CaloCommStream_Prescale_L1_J3",
                                 "CaloCommStream_TriggerFilter_L1_J4",
                                 "CaloCommStream_TriggerFilter_L1_J5",
                                 "CaloCommStream_TriggerFilter_L1_J6",
                                 #"CaloCommStream_TriggerFilter_L1_J7",
                                 "CaloCommStream_TriggerFilter_L1_FJ0",
                                 "CaloCommStream_TriggerFilter_L1_FJ1",
                                 #"CaloCommStream_TriggerFilter_L1_TAU0",
                                 #"CaloCommStream_TriggerFilter_L1_TAU1",
                                 #"CaloCommStream_TriggerFilter_L1_TAU2",
                                 #"CaloCommStream_TriggerFilter_L1_TAU3",
                                 #"CaloCommStream_TriggerFilter_L1_TAU4",
                                 #"CaloCommStream_TriggerFilter_L1_TAU5",
                                 "CaloCommStream_Prescale_L1_TAU6",
                                 "CaloCommStream_TriggerFilter_L1_TAU7",
                                 "CaloCommStream_Prescale_L1_EM0",
                                 #"CaloCommStream_TriggerFilter_L1_EM1",
                                 "CaloCommStream_TriggerFilter_L1_EM2",
                                 "CaloCommStream_TriggerFilter_L1_EM3",
                                 "CaloCommStream_TriggerFilter_L1_EM4",
                                 "CaloCommStream_TriggerFilter_L1_EM5",
                                 "CaloCommStream_TriggerFilter_L1_EM6",
                                 #"CaloCommStream_TriggerFilter_L1_EM7",
                                 "CaloCommStream_TriggerFilter_L1_TE0",
                                 "CaloCommStream_TriggerFilter_L1_TE1",
                                 "CaloCommStream_TriggerFilter_L1_JE0",
                                 "CaloCommStream_TriggerFilter_L1_JE1",
                                 "CaloCommStream_TriggerFilter_L1_XE0",
                                 "CaloCommStream_TriggerFilter_L1_XE1",
                                 "CaloCommStream_TriggerFilter_L1_XE2",
                                 "CaloCommStream_TriggerFilter_L1_XE3",
                                 ] )
    pass


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

#Simplest event skimming
#CaloCommStream.SetPrescale(5)

