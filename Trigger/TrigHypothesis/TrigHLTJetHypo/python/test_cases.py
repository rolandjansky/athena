# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

test_strings = [
    'ijet([(10et, 0eta320)])',
     'ijet([(10et, 0eta320)(20et, 0eta320)])',
    # 'or([] ijet([10et]) and([] ijet([20et]) ijet([30et])))',
    'or([] ijet([(10et)]) ijet([(20et)(40et)]))',
    'and([] ijet([(10et)]) ijet([(20et)]))',
    'not([] ijet([(10et)]) )',
    'not([] and([] ijet([(40et, 0eta320)]) ijet([(100et, 0eta320)])))',
    'or([] not([] ijet([(40et, 0eta320)])) not([] ijet([(100et, 0eta320)])))',
    'or([] and([] ijet([(40et, 0eta320)]) ijet([(40et, 0eta320)])) not([] ijet([(100et, 0eta320)])))',



"""
partition     
(
  [(4,2)]   # partition 6 input jets to 4, 2 
  splitter  # send the 4s to first 2 children, 2 to third.
    assert len(o) == 1
  (
    [(1,1)(2)]
    sum # mass cut on sum of input jets (ord.=4)
    (
      [(800m900)]
    )
    partition        #  partition 4 jets into non-overlapping
                     #  sets ord 2, 2
    (
      [(2,2)]
      pair           # sum inputs, apply deta cut to the 2 sums
      (
        [(40deta)]
        sum          # child gets two lists, applys mass cuts
        ( 
          [(75m85)(80m90)]
          partition  # passes on 2 sum to regroup to single jets
          (
            [() (1)]
            ijet     # apply aoverlapping mod, et cuts
            (
              [(30width50, 30et) (35width45, 40et)]
            )
          )
        )
      )
    )
  )
               
  ijet([(p320etap500, 100et) (n500etan320)]),
)""",


#     """cascade(
#    [800m900]             # conditions for cascade (only one here)
#      dijet(              # first daughter scenario
#        [ 80m90]          # condition(s) for first daughter scenario
#         ijet([10et])     # grandaughter scenario 1 for first daugh scen.
#         ijet([20et])     # grandaughter scenario 2 for first daugh scen.
#           )              # end of first dijet scenario 
#      dijet(              # second daughter scenario
#        [85m95]
#          ijet([30et]) 
#          ijet([40et])
#            )             # end of second dijet scenario
#    )                     # end of cascade scenario
#    """

    'and([] ijet([(50et)(70et)]) dijet([(900mass, 26dphi)]))',
    'and([]simple([(50et)(70et)])dijet([(900m,26dphi)]))',
]
