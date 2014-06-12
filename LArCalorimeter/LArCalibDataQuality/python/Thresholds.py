# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Collection of Thresholds used by the LArCalibValiation algorithms
# The thresholds are dependent on the geometrical location of the cell
# and the gain. The CaloCellGroup(List) objects (from /Calorimter/CaloIdentifier)
# to set the threshold dependent on the location of the cell


# Syntax: vector of strings
# 1. String: Default values (Three values of HIGH, MEDIUM and LOW gain)
# Following Strings: Overwrites for particular detector regions. The geometrical
# location is given by a sequence of six fields, separated by '/' and enclosed by '[ ]'.
# It its followed by the values (again three). 

#Example:
## exampleGroup=["0, 21.1",                               #Default value
##               "[EMB,EMECIW,EMECOW/C/*/*/*/*] 1, 22.2", #Something else for the EM C side
##               "[FCAL/A/*/*/*/*] 2, 24.4",      # Again something else for the FCAl A
##               "[EMB/C/1/0/0-32/*] 3, 32.2",    # This one for the 
##               "[EMECIW/*/1/0/3/4] 4, 42.2",    # 
##              ]

pedThr=["1.5, 0.9, 0.9",  #EMB, EMEC, FCAL
        "[HEC/*/*/*/*/*] 5.0, 1.3, 1.0",                 #HEC   
        ]

rmsThr=["0.8, 0.2, 0.2", #EMB, EMEC, FCAL
        "[HEC/*/*/*/*/*] 4.0, 0.6, 0.2",                 #HEC   
        ]


pedThrFEB=["0.2, 0.2, 0.2", #EMB, EMEC FCAL
           # SD    S L R E P
           "[HEC/*/*/*/*/*] 0.25, 0.1, 0.1",                 #HEC   
           "[FCAL/*/*/*/*/*] 0.1, 0.1, 0.1",                #FCAL
           ]

rmsThrFEB=["0.2, 0.2, 0.2",  #Three values for three gains (H M L)
           # SD    S L R E P
           "[EMB,EMECIW, EMECOW/*/*/*/*/*] 0.05, 0.05, 0.05 ",     #EMB Accordion
           "[EMECIW, EMECOW/*/*/*/*/*] 0.05, 0.007, 0.007 ",    #EMEC
           "[HEC/*/*/*/*/*] 0.6, 0.07, 0.004",                 #HEC   
           "[FCAL/*/*/*/*/*] 0.02, 0.005, 0.004",               #FCAL
           ]


acThr=["0.05, 0.05, 0.05",  #Three values for three gains (H M L)
       # SD    S L R E P
       #"[HEC/*/*/*/*/*] 0.05, 0.05, 0.05",                 #HEC   
       #"[FCAL/*/*/*/*/*] 0.05, 0.05, 0.05",                #FCAL
       ]

acThrFEB=["0.04, 0.04, 0.04",  #Three values for three gains (H M L)
       # SD    S L R E P
          "[EMECIW, EMECOW/*/*/*/*/*] 0.05, 0.05 0.05",
          "[HEC/*/*/*/*/*] 0.003, 0.003, 0.004",                 #HEC   
          "[FCAL/*/*/*/*/*] 0.03, 0.03, 0.03",                #FCAL
          ]


rampThr=["0.015, 0.004, 0.004",  #Three values for three gains (H M L)
         # SD    S L R E P
         "[EMECIW, EMECOW/*/*/*/*/*] 0.006, 0.004 0.004",
         "[HEC/*/*/*/*/*] 0.02, 0.015, 0.015",       
         "[FCAL/*/*/*/*/*] 0.004, 0.0015, 0.0015",
         ]

rampThrFEB=["0.004, 0.004, 0.004",
            "[EMECIW, EMECOW/*/*/*/*/*] 0.004, 0.004 0.004",
            "[HEC/*/*/*/*/*] 0.001, 0.004, 0.003",
            "[FCAL/*/*/*/*/*] 0.0025, 0.0004, 0.0006",
            ]

cwAmpThr=["10, 4, 4",
          "[EMECIW, EMECOW/*/*/*/*/*] 6, 1.5, 1.5 " ,
          "[HEC/*/*/*/*/*] 15, 10 , 10" , 
          "[FCAL/*/*/*/*/*] 5, 1, 1", 
          ]


cwFWHMThr=["8, 4, 4",
           "[EMECIW, EMECOW/*/*/*/*/*] 5, 3, 3" ,
           "[HEC/*/*/*/*/*] 20, 20, 20" , 
           "[FCAL/*/*/*/*/*] 4, 2, 2", 
           ]


cwAmpThrFEB=["5, 5, 5",
             "[EMECIW, EMECOW/*/*/*/*/*]5, 0.3, 2" ,
             "[HEC/*/*/*/*/*] 5, 5, 5" , 
             "[FCAL/*/*/*/*/*] 3, 1, 2", 
             ]


cwFWHMThrFEB=["50, 4, 4",
              "[EMECIW, EMECOW/*/*/*/*/*] 2, 1, 1" ,
              "[HEC/*/*/*/*/*] 30, 30, 30" , 
              "[FCAL/*/*/*/*/*] 50, 50, 50", 
              ]
