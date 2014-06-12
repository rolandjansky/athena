# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


ACmap = {1:'A+','1':'A+','A+':'A+',-1:'C-','-1':'C-','C-':'C-'}
BadChannels = ['deadReadout','deadCalib','deadPhys','almostDead','short','unstable','distorted','lowNoiseHG','highNoiseHG','unstableNoiseHG','lowNoiseMG','highNoiseMG','unstableNoiseMG','lowNoiseLG','highNoiseLG','unstableNoiseLG','missingFEB','peculiarCL','problematicFor?','sporadicBurstNoise']
BadFEBs = {0:'deadAllBit',1:'deadReadoutBit',2:'inErrorBit',3:'deactivatedInOKSBit',16:'maskParity',17:'maskBCID',18:'maskSampleHeader',19:'maskEVTID',20:'maskScacStatus',21:'maskScaOutOfRange',22:'maskGainMismatch',23:'maskTypeMismatch',24:'maskNumOfSamples',25:'maskEmptyDataBlock',26:'maskDspBlockSize',27:'maskCheckSum',28:'maskMissingHeader',29:'maskBadGain'}
DETlist = ['EMB','EMEC','HEC','FCAL']
global CHANNELS
CHANNELS={}
import string,math

########################################################################
mp = ['noise_acceptance_AVG','acceptance_AVG','signal_AVG','quality_AVG','fractionOverQ','DB','5Sigma','hiEth','Quality','ENLB','EN','Warning','Error','Cluster','LCE0','LCE1','LCE2','LCE3']
# 0:GREEN 1:RED 2:BLUE 3:PINK 4:YELLOW 5:BLACK  
col = [1,0,2,3,3,2,1,0,3,2,1,4,1,5,0,1,2,3]
def GetClass(plot):
  for a in range(18):
    if mp[a] in plot:
     return col[a]


########################################################################
def BeamBackground(phicell,beambkg):
  if not beambkg:
    return False
  else:
    dphi = 0.25
    if abs(phicell) < dphi: return True
    if abs(phicell-math.pi)< dphi*2: return True
    if abs(phicell+math.pi)< dphi*2: return True
    return False
    

##########################################################################
def ProposedFlag(beambkg,run_spec,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,expert,r):

  ## Check if a cell has DB status different from noisy
  ECAL_flag = -1
  for k in [expert,expert+1]:   
    if r[k][0]&(0x7927F)!=0x0: ECAL_flag = 1
  
  ## Check if cell already flagged in UPD4
  ## Sporadic: bit 19 (0x80000)
  ## HighNoise: bit 8 (0x100)
  UPD4_flag = -1
  for k in [expert,expert+1]:   
    if r[k][0]&(0x80000)!=0x0: UPD4_flag = 1
    if r[k][0]&(0x100)!=0x0: UPD4_flag = 1

  ## Check if it's bulk or not
  IsBulk = -1
  if (run_spec['proc_ver'] == bulkproc or run_spec['proc_ver'] == cleanbulkproc or run_spec['proc_ver'] == reproc): IsBulk = 1
  if run_spec['proc_ver'] == 103: IsBulk =1

  ## Get cluster matched variable
  dR = r[-1]

  ## Store results of each DQ test
  R = [ [-1,-1,-1,-1] , [-1,-1,-1,-1] , [-1,-1,-1] , [-1,-1,-1,-1] ]
  for i in range(4):
    for j in range(len(R[i])):
      if len(r[i+expert+2][j])>0: R[i][j] = r[i+expert+2][j][0][0]
      
  ## change flag by clicking on a cell
  flag = '<td onclick="ToggleFlag(this)" '

  ###################################
  ## Automatic diagnosis for Cells ##
  ###################################

  sporadic_threshold = 50
  sporadic_threshold_lce = 500
  
  ### Set flags for noisy cells associated to clusters
  if dR>0:
    ## check CaloCells column
    if R[1][0]>0 and R[1][1]>0:

      if R[1][3]>0:
        qfclean= R[1][0]-(R[1][0]*R[1][3])/100

        if qfclean < sporadic_threshold:

          if not BeamBackground(float(r[9]),beambkg): # Mask beam background only for sporadic cells
            flag += 'style="background-color:#FF6600">sporadicBurstNoise'
          else:
            #flag += 'style="background-color:#009900">BeamBkg'
            flag += 'style="background-color:#FF6600">sporadicBurstNoise'
        else:
          flag += 'style="background-color:#FF0000">highNoiseHG'
      else:
        flag += 'style="background-color:#FF0000">highNoiseHG' 

    ## check Sporadic column
    elif R[2][0]>0:

      if R[2][2]>0:
        qfclean= R[2][0]-(R[2][0]*R[2][2])/100
        if qfclean < sporadic_threshold:
          if not BeamBackground(float(r[9]),beambkg): # Mask beam background only for sporadic cells
            flag += 'style="background-color:#FF6600">sporadicBurstNoise'
          else:
            flag += 'style="background-color:#009900">BeamBkg'             
        else:
          flag += 'style="background-color:#FF0000">highNoiseHG'
      else:
        flag += 'style="background-color:#FF0000">highNoiseHG'
    ## check flag from LArCellEmpty column
    ## (ES) or (bulk if not already flagged in UPD4)
    elif (R[3][1]>0 and IsBulk<0) or (R[3][1]>0 and IsBulk>0 and UPD4_flag<0):

      if BeamBackground(float(r[9]),beambkg): # Mask beam background for all cells
        flag += 'style="background-color:#009900">BeamBkg'             
      else:
        if R[3][3]>0:
          qfclean= R[3][1]-(R[3][1]*R[3][3])/100
          if qfclean < sporadic_threshold_lce:
            flag += 'style="background-color:#78c7c7">sporadicBurstNoise'
          else:
            flag += 'style="background-color:#307D7E">highNoiseHG'
        else:
          flag += 'style="background-color:#307D7E">highNoiseHG'


   ##############################################################      
   ### Set flags for some cells even if not responsible for clusters        
  else:
    ##check CaloCell column
    if R[1][0]>100:
    #if R[1][0]>50:    
      if BeamBackground(float(r[9]),beambkg):
        flag += 'style="background-color:#009900">BeamBkg'
      else:
        if R[1][3]>0:
          if R[1][3]>0.8:
            flag += 'style="background-color:#FF6600">sporadicBurstNoise'
          else:   
            flag += 'style="background-color:#FF0000">highNoiseHG'
        else:
          flag += 'style="background-color:#FF0000">highNoiseHG'
    
    ## check Sporadic column
    elif R[2][0]>0:
      if R[2][2]>0:
        qfclean= R[2][0]-(R[2][0]*R[2][2])/100
        if qfclean < sporadic_threshold*10:
          if not BeamBackground(float(r[9]),beambkg):
            flag += 'style="background-color:#FF6600">sporadicBurstNoise'
          else:
            flag += 'style="background-color:#009900">BeamBkg'                                 
        else:
          flag += 'style="background-color:#FF0000">highNoiseHG'
      else:
        flag += 'style="background-color:#FF0000">highNoiseHG'
    ## check flag from LArCellEmpty column
    ## (ES) or (bulk if not already flagged in UPD4)
    elif (R[3][0]>3 and IsBulk<0): #or (R[3][0]>3 and IsBulk>0 and UPD4_flag<0):      
      if BeamBackground(float(r[9]),beambkg):
        flag += 'style="background-color:#009900">BeamBkg'                     
      else:
        if R[3][3]>0:
          qfclean= R[3][1]-(R[3][1]*R[3][3])/100
          if qfclean < sporadic_threshold_lce*10:
            flag += 'style="background-color:#78c7c7">sporadicBurstNoise'
          else:
            flag += 'style="background-color:#307D7E">highNoiseHG'
        else:
          flag += 'style="background-color:#307D7E">highNoiseHG'
        #ignore LCE only at bulk level    
        #if R[3][3]>0:
        #  qfclean= R[3][1]-(R[3][1]*R[3][3])/100
        #  if qfclean < sporadic_threshold_lce*10:
        #    flag += 'style="background-color:#78c7c7">LCEonly'
        #  else:
        #    flag += 'style="background-color:#307D7E">LCEonly'
        #else:
        #  flag += 'style="background-color:#307D7E">LCEonly'
    ##elif R[3][0]>0:
    ## print "DEBUG: Channel %s %s not associated to cluster, in LCE file, not put on the display
    ##Average Energy=%.2f <br>"%(str(hex(r[0])),str(hex(r[1])),R[3][0])
    # Set warnings for cell already known or with high deviations
    elif (R[1][0]>0 and R[1][1]>0) or (R[0][0]>0 and R[0][1]>0):
      flag += 'style="background-color:#FFFF00">Warning:ECAL'
    ## Look for pedestal shifts
    elif abs(R[0][2])>0:
      ped_threshold = 25 # EM Barrel and EMEC OW
      if r[2]==1 and (r[4]==3 or r[4]==10 or r[4]==16 or r[4]== 22): ped_threshold = 100 # EMEC IW FT=3/10/16/22
      if r[2]==2: ped_threshold = 300 # HEC
      if r[2]==3: ped_threshold = 500 # FCAL
      if abs(R[0][2])>ped_threshold:
        flag += 'style="background-color:#FFFFCC">PedShift:ECAL'

  if  'BeamBkg' in flag:
    if R[1][0]>100:
      if R[1][3]>0:
        if R[1][3]>0.8:
          flag = '<td onclick="ToggleFlag(this)"style="background-color:#FF6600">sporadicBurstNoise'
        else:   
          flag = '<td onclick="ToggleFlag(this)"style="background-color:#FF0000">highNoiseHG'
      else:
        flag = '<td onclick="ToggleFlag(this)"style="background-color:#FF0000">highNoiseHG'
    
          
  ### Overwrite flags in case of beam background
  #if beambkg:
  #  #if 'sporadicBurstNoise' in flag or 'highNoiseHG' in flag:
  #  if 'sporadicBurstNoise' in flag:
  #    dphi = 0.25
  #    phicell = float(r[9])
  #    if abs(phicell) < dphi or abs(phicell-math.pi)<dphi or abs(phicell+math.pi)<dphi:
  #      flag = '<td onclick="ToggleFlag(this)" style="background-color:#009900">BeamBkg'          
  #      # print "debug phi = %f <br>"%phicell
    
  ### Keep only Sporadic and HighNoiseHG to fill defects
  if 'sporadicBurstNoise' in flag or 'highNoiseHG' in flag:
    part = DETlist[r[2]]+ACmap[r[3]][:-1]
    if CHANNELS.has_key(part):
      is_duplicate=False
      for ch in CHANNELS[part]:
        if str(r[0]) in ch: is_duplicate= True
      if not is_duplicate:
        CHANNELS[part].append("(%i,%.2f,%.2f)"%(r[0],r[8],r[9]))
        #print "DEBUG",part,r[0],r[8],r[9],"<br>"
    else: CHANNELS[part] = ["(%i,%.2f,%.2f)"%(r[0],r[8],r[9])]
  ### end
  if not 'style' in flag: flag+='>'
  return flag+'</td>'


#######################################################################################################################
def PrintRows(beambkg,clusters,url,run_spec,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,expert,channels=[]):
  import operator
  SortedChannels = sorted(channels,key=operator.itemgetter(2,3,4,5,6,7))
  nRows = 0

  for i in SortedChannels:
    ## Store results of each DQ test (RawChannels/CaloCells/Sporadic/LArCellEmpty)
    ## i[12:15] = [[-3s,+3s,E,Sat.Q],[>Eth,>5s,DB,Sat.Q],[E>20,ENLB,Sat.Q],[<E>,E>1,LB,Sat.Q]]
    ## R[0:4] = [[-3s,+3s,E,Sat.Q],[>Eth,>5s,DB,Sat.Q],E>20,[E>20,ENLB,Sat.Q],[<E>,E>1,LB,Sat.Q]]
    R = [ [-1,-1,-1,-1] , [-1,-1,-1,-1] , [-1,-1,-1], [-1,-1,-1,-1] ]
    for index in range(4):
      for j in range(len(i[index+12])):
        if len(i[index+12][j])>0:
          R[index][j] = i[index+12][j][0][0]
          #print "index,j,i[index+12][j],i[index+12][j][0][0],'<br>'

    ## Check if QFactor is filled:
    if R[0][3]>0 or R[1][3]>0 or R[2][2]>0:
      ## if nothing filled but Q factor, skip the channel
      mysum = 0
      mysum += R[0][0]+R[0][1]+R[0][2]
      mysum += R[1][0]+R[1][1]+R[1][2]
      mysum += R[2][0]+R[2][1]
      mysum += R[3][0]+R[3][1]+R[3][2]
      if mysum == -11: continue

    ## Check if only LCE is filled in BULK. Skip if already flagged in UPD4      
    if run_spec['proc_ver']==bulkproc or run_spec['proc_ver']==cleanbulkproc or run_spec['proc_ver']==reproc :
      ## sum results of the last column. If empty, result is -4

      mysum=0
      for index in range(3):
        for j in range(len(R[index])):
          mysum+=R[index][j]
      ## Check UPD4 status
      ## Sporadic: bit 19 (0x80000)
      ## HighNoise: bit 8 (0x100)
      LCE_flag = -1
      for k in [expert,expert+1]:   
        if i[k][0]&(0x80000)!=0x0: LCE_flag = 1
        if i[k][0]&(0x100)!=0x0: LCE_flag = 1
      if (LCE_flag > 0) and (mysum==-11): 
        #print 'DEBUG: skipping channel %s %i %i<br>'%(str(hex(i[0])),mysum,LCE_flag)
        continue

    ## And now print all the remaining channels
    out = '<tr class="out%d" title="%d">\n'%(nRows%2,nRows)
    out += '<td>'+str(hex(i[0]))+'</td>\n<td>'+str(hex(i[1]))+'</td>\n'
    out += '<td style="text-align:right">'+DETlist[i[2]]+'</td><td style="text-align:left">'+ACmap[i[3]]+'</td>\n'
    for j in i[4:8]: out += '<td>'+str(j)+'</td>\n'
    for j in i[8:10]: out += '<td>%.4f</td>\n' % (j)
    if expert>10:
      out += '<td>%s</td>\n' % (i[10])
      out += '<td>%.3f</td>\n' % (i[11])
      out += '<td>%s</td>\n' % (i[12])
      out += '<td>'+str(hex(i[13])).rstrip('L')+'</td>\n'
    for k in [expert,expert+1]:
      status=''
      if i[k][0]!=0:
        for j in range(20):
          if i[k][0]&(1<<j)!=0x0: status += BadChannels[j]+' '
      if i[k][1]!=0:
        for j in BadFEBs.keys():
          if i[k][1]&(1<<j)!=0x0: status += BadFEBs[j]+' '
      out += '<td style="width:70px">'+status.rstrip(' ')+'</td>\n'
 
    for j in range(4):
      K = i[expert+2+j]
      out += '<td>' 
      for k in K:
        if len(k)>0:
          k.sort()
          k.reverse()
          for g in k:
            if 'LCE' in g[2]: 
              lce_url='https://atlas-larmon.cern.ch/WebDisplayExtractor/LCE/Plots/%s_EnergyCut_Algo0.png'%run_spec['low_run']
              out+='<a class="det%d" href="%s" title="%s">%d</a>&nbsp;' % (GetClass(g[2]),lce_url,g[1],g[0]) 
            #elif 'hiEth' in g[2]:
            #  out+='<a class="det%d" href="%s/%s" title="%s">%.2f</a>&nbsp;' % (GetClass(g[2]),url,g[2],g[1],g[0])
            else:
              out+='<a class="det%d" href="%s/%s" title="%s">%.0f</a>&nbsp;' % (GetClass(g[2]),url,g[2],g[1],g[0]) 
      out += '</td>\n'
    dR = i[-1]
    if dR>0: out += '<td>%.3f</td>\n' % (dR) 
    else: out += '<td></td>\n'
    if clusters: out += ProposedFlag(beambkg,run_spec,esproc,cleanesproc,bulkproc,cleanbulkproc,reproc,expert,i)
    else: out += '<td></td>\n'

    nRows += 1
    print out+'</tr>\n'
