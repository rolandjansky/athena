# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

test_strings = [
    'z([]simple([(10et, 0eta320)]))',
    'z([] simple([(10et, 0eta320)(20et, 0eta320)]))',
    # 'or([] simple([10et]) and([] simple([20et]) simple([30et])))',
    'z([] or([] simple([(10et)]) simple([(20et)(40et)])))',
    'z({} and([] simple([(10et)]) simple([(20et)])))',
    'z([] not([] simple([(10et)]) ))',
    'z([] not([] and([] simple([(40et, 0eta320)]) simple([(100et, 0eta320)]))))',
    'z([] or([] not([] simple([(40et, 0eta320)])) not([] simple([(100et, 0eta320)]))))',
    'z([] or([] and([] simple([(40et, 0eta320)]) simple([(40et, 0eta320)])) not([] simple([(100et, 0eta320)]))))',

    'z([] and([] simple([(50et)(70et)]) dijet([(900djmass, 26djdphi)])))',
    'z([] and([]simple([(50et)(70et)])combgen([]dijet([(900djmass,26djdphi)]) simple([(10et)(20et)]))))',
    'z([]and([]simple([(81et)(81et)])combgen([(50et, eta100)]dijet([(26djdphi)]))))',
    'z([]simple([(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)]))',
    'z([] partgen([(20et,0eta320)]simple([(40et,0eta320)(50et,0eta320)])simple([(35et,0eta240)(55et,0eta240)])))',
    'z([] simple([(10et, neta0)(20et, peta)]))', # missing low value for eta - take default

]


if __name__ == '__main__':
    print('There are %d test cases' % len(test_strings))
