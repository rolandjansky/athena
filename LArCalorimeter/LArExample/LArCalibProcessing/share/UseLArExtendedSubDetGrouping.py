printfunc ('LArExtendedSubDetGrouping')

from LArConditionsCommon.LArExtendedSubDetGrouping import LArExtendedSubDetGrouping
LArExtendedSubDetGrouping = LArExtendedSubDetGrouping()
LArExtendedSubDetGrouping.setWithCorr(False)

## defined gain :

## HIGH
if ( Gain=="HIGH" ):
    gain= [0]

## MEDIUM    
elif  ( Gain=="MEDIUM" ) :
    gain= [1]

## LOW    
elif  ( Gain=="LOW" ) :
    gain= [2]

else:
    gain=[0,1,2]

## defined partition

if (int(RunNumberFlag)<100000) :
    ## EMB + EMBPS A 64 (32+32)   
    if ( Partition=='EB-EMBA' and ( Gain=="HIGH" or Gain=="MEDIUM" ) ) :
        partition =['EMBAPS','EMBA']
        printfunc ('EMBA+EMBAPS partition')
    elif ( Partition=='EB-EMBC' and ( Gain=="HIGH" or Gain=="MEDIUM" ) ) :
        partition =['EMBCPS','EMBC']
        printfunc ('EMBC+EMBCPS partition')
    elif ( Partition=='EB-EMBA' and Gain=="LOW" ) :
        partition =['EMBA']
        printfunc ('EMBA partition')
    elif ( Partition=='EB-EMBC' and Gain=="LOW" ) :
        partition =['EMBC']
        printfunc ('EMBC partition')

else :
    ## EMB  A+C    
    if ( Partition=='EB-EMBA' and ( PartitionTypeGeneric!='EMBPS' ) ) :
        partition =['EMBA']
        printfunc ('EMBA partition')
    elif ( Partition=='EB-EMBC' and ( PartitionTypeGeneric!='EMBPS' ) ) :
        partition =['EMBC']
        printfunc ('EMBC partition')
        
    ## EMBPS  A+C     
    if ( Partition=='EB-EMBA' and ( PartitionTypeGeneric=='EMBPS' ) ) :
        partition =['EMBAPS']
        printfunc ('EMBAPS partition')
    elif ( Partition=='EB-EMBC' and ( PartitionTypeGeneric=='EMBPS' ) ) :
        partition =['EMBCPS']
        printfunc ('EMBCPS partition')
        
        
## EMEC + PS A  44    
if ( Partition=='EB-EMECA' ) :
    partition =['EMECAPS','EMECA']
elif ( Partition=='EB-EMECC' ) :
    partition =['EMECCPS','EMECC']

## HEC A + C   8 (4+4)  
elif ( PartitionType=='HEC'):
    partition =['HECA','HECC']

## FCAL A + C   2 (1+1)
elif ( PartitionType=='FCAL') :   
    partition =['FCALA','FCALC']

selection = LArExtendedSubDetGrouping.getChannelSelection(partition,gain)
ChannelSelection='<channelSelection>'+selection+'</channelSelection>'
printfunc (ChannelSelection)
printfunc ("CoolChannel Selection for ", partition, " and ",gain, " gain. ")
    
    

   
