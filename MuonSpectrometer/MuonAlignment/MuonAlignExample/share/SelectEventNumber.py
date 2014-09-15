from TrkAlignGenTools.AlignmentFLags import AlignmentFlags as align
align.readEventList=True

from TrkAlignGenAlgs.SelectEventNumberGetter import SelectEventNumberGetter
selectEvtNumber = SelectEventNumberGetter().SelectEventNumberHandle()
