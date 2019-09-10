# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

test_strings = [
    'simple([(10et, 0eta320)])',
     'simple([(10et, 0eta320)(20et, 0eta320)])',
    # 'or([] simple([10et]) and([] simple([20et]) simple([30et])))',
    'or([] simple([(10et)]) simple([(20et)(40et)]))',
    'and([] simple([(10et)]) simple([(20et)]))',
    'not([] simple([(10et)]) )',
    'not([] and([] simple([(40et, 0eta320)]) simple([(100et, 0eta320)])))',
    'or([] not([] simple([(40et, 0eta320)])) not([] simple([(100et, 0eta320)])))',
    'or([] and([] simple([(40et, 0eta320)]) simple([(40et, 0eta320)])) not([] simple([(100et, 0eta320)])))',


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

            simple     # apply overlapping mod, et cuts
            (
              [(30width50, 30et) (35width45, 40et)]
            )
          )
        )
      )
    )
  )
               
  simple([(p320etap500, 100et) (n500etan320)]),
)""",

    'and([] simple([(50et)(70et)]) dijet([(900mass, 26dphi)]))',
    'and([]simple([(50et)(70et)])combgen([(2)]dijet([(900mass,26dphi)]) simple([(10et)(20et)])))',
    'and([]simple([(81et)(81et)])combgen([(2)(50et)(70et)]dijet([(34mass,26dphi)])))',
    'simple([(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)])',
    'partgen([(20et,0eta320)]simple([(40et,0eta320)(50et,0eta320)])simple([(35et,0eta240)(55et,0eta240)]))',
    'simple([(10et, neta0)(20et, peta)])', # missing low value for eta - take default

]


if __name__ == '__main__':
    print 'There are %d test cases' % len(test_strings)
