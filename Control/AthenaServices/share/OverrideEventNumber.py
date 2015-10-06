## @file OverrideEventNumber.py
# @brief should allow changing run/evt number in a EventInfo. Usage:
# @code
# include( "AthenaServices/OverrideEventNumber.py" )
# EventMixer.OutputRunNumber = new_run_number
# @endcode
#
# $Id: OverrideEventNumber.py,v 1.4 2007-06-16 00:55:21 calaf Exp $

include.block( "AthenaServices/OverrideEventNumber.py" )

theApp.ExtSvc += [ "MixingEventSelector/EventMixer" ]
theApp.EvtSel = "EventMixer"

EventMixer = Service( "EventMixer" )
#assuming you have less than 100K events in your file :-)
EventMixer.TriggerList += [ "EventSelectorAthenaPool/EventSelector:0:99999" ]

#you may want to add
#EventMixer.OutputLevel      = VERBOSE
#MessageSvc.setVerbose += [ "MixingEventSelector::TriggerList" ]

