# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from xml.dom.minidom import Document

class TestStatistics:
    def __init__(self, testname):
        self.testsuitetestname = testname
        self.results = {}

    def log(self): pass

    def __iadd__(self, (name, result, message)):
        self.results[name] = (result, message)
        return self

    def __str__(self): 
        m  = '******************************\n'
        m += '**** Results for tests.%s ****\n' % self.testsuitetestname
        m += '******************************\n'
        m += 'Total: %d, PASS: %d, FAIL: %d \n\n' % (self.summary())
        
        for testname, (status, message) in self.results.items():
            m += '%s: %s' % ({True:'PASS', False:'FAIL'}.get(status), testname)
            if message: m+=message
            m += '\n'
        return m

    def summary(self):
        totalTests = len(self.results.keys())
        totalPass  = len([k for k,v in self.results.items() if v[0]])
        totalFail  = len([k for k,v in self.results.items() if not v[0]])
        return (totalTests, totalPass, totalFail)

    def toxml(self):
        doc = Document()
        e = doc.createElement
        t = doc.createTextNode
        testsuitetestEl = e('testsuitetest')
        testsuitetestnameEl = e('testsuitetestname')
        testsuitetestnameEl.appendChild(t(self.testsuitetestname))
        testsuitetestEl.appendChild(testsuitetestnameEl)

        # Make the summary
        tot, ok, fail = self.summary()
        testsuitetestsummaryEl = e('summary')
        totEl = e('totaltests')
        totEl.appendChild(t(str(tot)))
        totokEl = e('totalok')
        totokEl.appendChild(t(str(ok)))
        totfailEl = e('totalfail')
        totfailEl.appendChild(t(str(fail)))

        [testsuitetestEl.appendChild(child) for child in [totEl, totokEl, totfailEl]]

        # Now the individual tests
        for testname, (status, message) in self.results.items():
            testEl = e('test')
            testnameEl = e('testname')
            testnameEl.appendChild(t(testname))
            statEl = e('status')
            statEl.appendChild(t(str(status)))
            messEl = e('message')
            messEl.appendChild(t(message))

            [testEl.appendChild(f) for f in [testnameEl, statEl, messEl]]

            testsuitetestEl.appendChild(testEl)

        return testsuitetestEl
    # doc.appendChild(testsuitetestEl)
    #  return doc
