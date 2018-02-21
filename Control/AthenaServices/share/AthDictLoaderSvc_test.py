# @file: AthenaServices/share/AthDictLoaderSvc_test.py
# @purpose: make sure we can reliably load dictionaries with AthDictLoaderSvc
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    October 2008

import AthenaPython.PyAthena as PyAthena
StatusCode = PyAthena.StatusCode

class AthDictLoaderTestAlg (PyAthena.Alg):
    """simple algorithm to test the AthDictLoaderSvc capabilities
    """

    def __init__ (self, name="AthDictLoaderTestAlg", **kw):
        kw['name'] = name
        super (AthDictLoaderTestAlg, self).__init__(**kw)

        self.dict_tests = kw.get(
            'dict_tests',
            ('std::string',
             'std::basic_string<char>',
             'std::vector<std::string>',
             'std::vector<std::basic_string<char> >',
             'std::vector<double>',
             #'egamma',      # not enough karma (AtlasEvent)
             #'egamma_p1',   # not enough karma (AtlasEvent)
             'std::vector<std::string,std::allocator<std::string> >',
             'std::map<int,int>',
             'std::map<std::string,std::string>',
             'std::map<std::string, std::string>',
             'std::map<std::basic_string<char>,std::basic_string<char> >',
             'std::pair<int,int>',
             'std::pair<std::string,std::string>',
             'std::pair<std::string, std::string>',
             'std::pair<std::basic_string<char>,std::basic_string<char> >',
             'std::pair<std::basic_string<char>, std::basic_string<char> >',
             'std::set<std::string>',
             'std::set<std::basic_string<char> >',
             'std::set<unsigned int>',
             #'DataVector<TruthParticle>',      # not enough karma (AtlasEvent)
             #'INavigable4Momentum',            # not enough karma (AtlasEvent)
             #'INavigable4MomentumCollection',  # not enough karma (AtlasEvent)
             #'IParticleContainer',             # not enough karma (AtlasEvent)
             #'DataVector<egamma>',             # not enough karma (AtlasEvent) 
             'ElementLinkInt_p1',
             'ElementLink_p1<unsigned int>',
             # FIXME: we should allow this kind of user-friendlyness...
             #'ElementLink<INavigable4MomentumCollection>',
             ))
        import os
        if os.environ.get('AtlasProject','') == 'ManaCore':
            # disable these tests as ElementLinkxyz are not in Mana
            t = []
            for ii in self.dict_tests:
                if not (ii in ('ElementLinkInt_p1',
                               'ElementLink_p1<unsigned int>')):
                    t.append(ii)
            self.dict_tests = tuple(t)
        # handle to AthDictLoaderSvc
        self.dict_svc = None
        return

    def initialize (self):
        self.msg.info ('retrieving the dictionary loader svc...')
        self.dict_svc = PyAthena.py_svc ('AthDictLoaderSvc',
                                         iface='IDictLoaderSvc')
        if self.dict_svc is None:
            self.msg.error ('could not retrieve the dict. loader svc')
            return StatusCode.Failure
        return StatusCode.Success

    def execute (self):
        DICT_SCOPE = 7
        _info = self.msg.info
        _info ('testing loading of dictionaries...')
        all_good = True
        for name in self.dict_tests:
            _info ('-'*40)
            _info ('--> [%s]', name)
            type_name = self.dict_svc.load_type (name).Name(DICT_SCOPE)
            _info ('<-- [%s]', type_name)
            if type_name == '':
                all_good = False
                self.msg.error ('problem loading dict for type [%s]', name)
        if not all_good:
            self.msg.error ('problem occured !')
            return StatusCode.Failure
        _info ('test of dictionary loading SUCCEEDED')
        return StatusCode.Success

    def finalize (self):
        return StatusCode.Success
    pass # AthDictLoaderTestAlg

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += AthDictLoaderTestAlg (OutputLevel = Lvl.INFO)

theApp.EvtMax = 1
