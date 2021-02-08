#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
import MdtRawDataMonitoring.MdtRawMonLabels as labels
from ROOT import TBox, kGray, TLine
from MdtRawDataMonitoring.MdtRawMonLabels import * # noqa

def getMDTLabel(x,y):
    labelx = getattr(labels, x)
    labely = getattr(labels, y)
    return labelx,labely


def getMDTLabelx(x):
    labelx = getattr(labels, x)
    return labelx


def putBoxMdtGlobal(h, ecap):
    if(ecap == "B"):
        offset = 8
        # Left Side
        putBox(h, 0, 0, 6, 16, offset); putLine(h, 6, 0, 6, 16, offset); putLine(h, 2, 16, 6, 16, offset)
        putBox(h, 0, 16, 2, 18, offset); putLine(h, 2, 16, 2, 18, offset); putLine(h, 0, 18, 2, 18, offset)
        putBox(h, 0, 20, 2, 22, offset); putLine(h, 2, 20, 2, 22, offset); putLine(h, 0, 20, 2, 20, offset); putLine(h, 0, 22, 2, 22, offset)  # 3
        putBox(h, 0, 24, 2, 26, offset); putLine(h,2,24,2,26,offset); putLine(h, 0, 24, 2, 24, offset); putLine(h, 0, 26, 2, 26, offset)  # 5
        putBox(h, 0, 28, 2, 30, offset); putLine(h,2,28,2,30,offset); putLine(h,0,28,2,28,offset); putLine(h,0,30,2,30,offset)  # 7
        putBox(h, 0, 32, 2, 34, offset);  putLine(h,2,32,2,34,offset); putLine(h,0,32,2,32,offset); putLine(h,0,34,2,34,offset)  # 9
        putBox(h, 0, 36, 3, 38, offset);  putLine(h,3,36,3,38,offset); putLine(h,0,36,3,36,offset); putLine(h,2,38,3,38,offset)  # 11M
        putBox(h, 0, 38, 2, 40, offset);  putLine(h,2,38,2,40,offset); putLine(h,0,40,2,40,offset)  # 11R
        putBox(h, 0, 42, 2, 44, offset);  putLine(h,0,42,2,42,offset); putLine(h,2,42,2,44,offset); putLine(h,0,44,2,44,offset)  # 13
        putBox(h, 0, 46, 3, 48, offset);  putLine(h,3,46,3,48,offset); putLine(h,0,46,3,46,offset); putLine(h,2,48,3,48,offset)  # 15M
        putBox(h, 0, 48, 2, 50, offset);  putLine(h,2,48,2,50,offset); putLine(h,0,50,2,50,offset)  # 15R
        putBox(h, 0, 52, 2, 106, offset); putLine(h,0,52,2,52,offset); putLine(h,0,106,2,106,offset)
        putBox(h, 0, 108, 1, 110, offset); putLine(h,1,108,1,110,offset); putLine(h,0,108,1,108,offset); putLine(h,0,110,1,110,offset)
        putBox(h, 0, 112, 2, 116, offset); putLine(h,2,112,2,116,offset); putLine(h,0,112,2,112,offset)

        putLine(h,2,52,2,106,offset)

        # Mid Section
        putBox(h, 8, 0, 9, 106, offset); putLine(h,8,0,8,106, offset); putLine(h,8,106,9,106, offset); putLine(h,9,0,9,106, offset)
        putBox(h, 8, 108, 9, 110, offset); putLine(h,8,108,9,108, offset); putLine(h,8,110,9,110, offset); putLine(h,8,108,8,110, offset); putLine(h,9,108,9,110, offset)
        putBox(h, 8, 112, 9, 116, offset); putLine(h,8,112,9,112, offset); putLine(h,8,112,8,116, offset); putLine(h,9,112,9,116, offset)

        # Right Side
        putBox(h, 11, 0, 17, 16, offset);     putLine(h,11,0,11,16, offset); putLine(h,11,16,15,16, offset)
        putBox(h, 15, 16, 17, 18, offset);  putLine(h,15,16,15,18, offset); putLine(h,15,18,17,18, offset)  # 1
        putBox(h, 15, 20, 17, 22, offset);  putLine(h,15,20,15,22, offset); putLine(h,15,20,17,20, offset); putLine(h,15,22,17,22, offset)  # 3
        putBox(h, 15, 24, 17, 26, offset);  putLine(h,15,24,15,26, offset); putLine(h,15,24,17,24, offset); putLine(h,15,26,17,26, offset)  # 5
        putBox(h, 15, 28, 17, 30, offset);  putLine(h,15,28,15,30, offset); putLine(h,15,28,17,28, offset); putLine(h,15,30,17,30, offset)  # 7
        putBox(h, 15, 32, 17, 34, offset);    putLine(h,15,32,15,34, offset); putLine(h,15,32,17,32, offset); putLine(h,15,34,17,34, offset)  # 9
        putBox(h, 14, 36, 17, 38, offset);    putLine(h,14,36,14,38, offset); putLine(h,14,36,17,36, offset); putLine(h,14,38,15,38, offset)  # 11M
        putBox(h, 15, 38, 17, 40, offset);    putLine(h,15,38,15,40, offset); putLine(h,15,40,17,40, offset)  # 11R
        putBox(h, 15, 42, 17, 44, offset);    putLine(h,15,42,15,44, offset); putLine(h,15,42,17,42, offset); putLine(h,15,44,17,44, offset)  # 13
        putBox(h, 14, 46, 17, 48, offset);    putLine(h,14,46,14,48, offset); putLine(h,14,46,17,46, offset); putLine(h,14,48,15,48, offset)  # 15M
        putBox(h, 15, 48, 17, 50, offset);    putLine(h,15,48,15,50, offset); putLine(h,15,50,17,50, offset)  # 15R
        putBox(h, 15, 52, 17, 106, offset);  putLine(h,15,52,17,52, offset); putLine(h,15,106,17,106, offset)
        putBox(h, 16, 108, 17, 110, offset);   putLine(h,16,108,16,110, offset); putLine(h,16,108,17,108, offset); putLine(h,16,110,17,110, offset)
        putBox(h, 15, 112, 17, 116, offset);   putLine(h,15,112,15,116, offset); putLine(h,15,112,17,112, offset)

        putLine(h,15,52,15,106, offset)

    elif(ecap == "E"):
        offset = 6
        # Bottom: NEW
        putBox(h, 0, 0, 4, 32, offset); putBox(h, 4, 8, 5, 10, offset)
        putLine(h, 4, 0, 4, 8, offset); putLine(h, 4, 8, 5, 8, offset); putLine(h, 5, 8, 5, 10, offset); putLine(h, 4, 10, 5, 10, offset); putLine(h, 4, 10, 4, 32, offset)  # Left side
        putBox(h, 8, 0, 12, 32, offset); putBox(h, 7, 8, 8, 10, offset)
        putLine(h, 8, 0, 8, 8, offset); putLine(h, 7, 8, 8, 8, offset); putLine(h, 7, 8, 7, 10, offset); putLine(h, 7, 10, 8, 10, offset);  putLine(h, 8, 10, 8, 32, offset)

        # Left
        putBox(h, 0, 32, 1, 34, offset); putLine(h, 1, 32, 4, 32, offset); putLine(h, 1, 32, 1, 34, offset)  # phi 1
        putBox(h, 0, 34, 4, 36, offset); putLine(h, 1, 34, 4, 34, offset); putLine(h, 4, 34, 4, 36, offset); putLine(h, 2, 36, 4, 36, offset)  # phi 2
        putBox(h, 0, 36, 2, 38, offset); putLine(h, 2, 36, 2, 38, offset)  # phi 3
        putBox(h, 0, 38, 4, 40, offset); putLine(h, 2, 38, 4, 38, offset); putLine(h, 4, 38, 4, 40, offset); putLine(h, 2, 40, 4, 40, offset)  # phi 4
        putBox(h, 0, 40, 2, 42, offset); putLine(h, 2, 40, 2, 42, offset)  # phi 5
        putBox(h, 0, 42, 4, 44, offset); putLine(h, 2, 42, 4, 42, offset); putLine(h, 4, 42, 4, 44, offset); putLine(h, 2, 44, 4, 44, offset)  # phi 6
        putBox(h, 0, 44, 2, 46, offset); putLine(h, 2, 44, 2, 46, offset)  # phi 7
        putBox(h, 0, 46, 4, 48, offset); putLine(h, 2, 46, 4, 46, offset); putLine(h, 4, 46, 4, 48, offset); putLine(h, 1, 48, 4, 48, offset)  # phi 8
        putBox(h, 0, 48, 1, 50, offset); putLine(h, 1, 48, 1, 50, offset)  # phi 9
        putBox(h, 0, 50, 4, 52, offset); putLine(h, 1, 50, 4, 50, offset); putLine(h, 4, 50, 4, 52, offset); putLine(h, 2, 52, 4, 52, offset)  # phi 10
        putBox(h, 0, 52, 2, 54, offset); putLine(h, 2, 52, 2, 54, offset)  # phi 11
        putBox(h, 0, 54, 4, 56, offset); putLine(h, 2, 54, 4, 54, offset); putLine(h, 4, 54, 4, 56, offset); putLine(h, 2, 56, 4, 56, offset)  # phi 12
        putBox(h, 0, 56, 2, 58, offset); putLine(h, 2, 56, 2, 58, offset)  # phi 13
        putBox(h, 0, 58, 4, 60, offset); putLine(h, 2, 58, 4, 58, offset); putLine(h, 4, 58, 4, 60, offset); putLine(h, 2, 60, 4, 60, offset)  # phi 14
        putBox(h, 0, 60, 2, 62, offset); putLine(h, 2, 60, 2, 62, offset)  # phi 15
        putBox(h, 0, 62, 4, 64, offset); putLine(h, 2, 62, 4, 62, offset); putLine(h, 4, 62, 4, 64, offset); putLine(h, 1, 64, 4, 64, offset)  # phi 16
        putBox(h, 0, 64, 1, 96, offset); putLine(h, 1, 64, 1, 96, offset); putLine(h, 0, 96, 1, 96, offset)

        # Right
        putBox(h, 11, 32, 12, 34, offset); putLine(h, 8, 32, 11, 32, offset); putLine(h, 11, 32, 11, 34, offset)  # phi 1
        putBox(h, 8, 34, 12, 36, offset);  putLine(h, 8, 34, 11, 34, offset); putLine(h, 8, 34, 8, 36, offset); putLine(h, 10, 36, 8, 36, offset)  # phi 2
        putBox(h, 10, 36, 12, 38, offset); putLine(h, 10, 36, 10, 38, offset)  # phi 3
        putBox(h, 8, 38, 12, 40, offset);  putLine(h, 10, 38, 8, 38, offset); putLine(h, 8, 38, 8, 40, offset); putLine(h, 10, 40, 8, 40, offset)  # phi 4
        putBox(h, 10, 40, 12, 42, offset); putLine(h, 10, 40, 10, 42, offset)  # phi 5
        putBox(h, 8, 42, 12, 44, offset);  putLine(h, 10, 42, 8, 42, offset); putLine(h, 8, 42, 8, 44, offset); putLine(h, 10, 44, 8, 44, offset)  # phi 6
        putBox(h, 10, 44, 12, 46, offset); putLine(h, 10, 44, 10, 46, offset)  # phi 7
        putBox(h, 8, 46, 12, 48, offset);  putLine(h, 10, 46, 8, 46, offset); putLine(h, 8, 46, 8, 48, offset); putLine(h, 11, 48, 8, 48, offset)  # phi 8
        putBox(h, 11, 48, 12, 50, offset); putLine(h, 11, 48, 11, 50, offset)  # phi 9
        putBox(h, 8, 50, 12, 52, offset);  putLine(h, 11, 50, 8, 50, offset); putLine(h, 8, 50, 8, 52, offset); putLine(h, 10, 52, 8, 52, offset)  # phi 10
        putBox(h, 10, 52, 12, 54, offset); putLine(h, 10, 52, 10, 54, offset)  # phi 11
        putBox(h, 8, 54, 12, 56, offset);  putLine(h, 10, 54, 8, 54, offset); putLine(h, 8, 54, 8, 56, offset); putLine(h, 10, 56, 8, 56, offset)  # phi 12
        putBox(h, 10, 56, 12, 58, offset); putLine(h, 10, 56, 10, 58, offset)  # phi 13
        putBox(h, 8, 58, 12, 60, offset);  putLine(h, 10, 58, 8, 58, offset); putLine(h, 8, 58, 8, 60, offset); putLine(h, 10, 60, 8, 60, offset)  # phi 14
        putBox(h, 10, 60, 12, 62, offset); putLine(h, 10, 60, 10, 62, offset)  # phi 15
        putBox(h, 8, 62, 12, 64, offset);  putLine(h, 10, 62, 8, 62, offset); putLine(h, 8, 62, 8, 64, offset); putLine(h, 11, 64, 8, 64, offset)  # phi 16

        putBox(h, 11, 64, 12, 96, offset); putLine(h, 11, 64, 11, 96, offset); putLine(h, 12, 96, 11, 96, offset)


def putBox(h, x1, y1, x2, y2, offset):
    box = TBox(x1-offset, y1*0.5, x2-offset, y2*0.5)
    box.SetFillColor(kGray)
    box.SetLineColor(kGray)
    h.GetListOfFunctions().Add(box)


def putLine(h, x1, y1, x2, y2, offset, c=1):
    line = TLine(x1-offset, y1*0.5, x2-offset, y2*0.5)
    line.SetLineColor(c)
    h.GetListOfFunctions().Add(line)
