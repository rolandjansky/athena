from GaudiSequencer.PyCompsExt import PyEvtFilterFromFile

evtSelector = PyEvtFilterFromFile('eventSelector')
if 'selectorInputFile' in locals():  
  evtSelector.input_file = selectorInputFile

from AthenaCommon.AlgSequence import AthSequencer
filtseq =  AthSequencer ("AthFilterSeq")
filtseq += evtSelector
