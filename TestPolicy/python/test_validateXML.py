# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import unittest
import validateXML
from functools import partial


class DTDTests(unittest.TestCase):
    def setUp(self):
        dtdname = 'unifiedTestConfiguration.dtd'
        urlbase = 'http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/'
        urlaltbase = 'https://atlas-rtt.cern.ch/prod/alternateDTD/'
        self.dtd = os.path.join(urlbase, dtdname)
        self.altdtd = os.path.join(urlaltbase, dtdname)
        self.baddtd = os.path.join(urlbase, 'inexistant.dtd')
        self.dtds = [self.dtd, self.altdtd]

    def test_dtd_unreachable(self):
        with validateXML.DTD(self.dtd, timeout=0) as dtd:  # guarantee timeout
            self.assertTrue(dtd.unreachable)

    def test_dtd_inexistant(self):
        with validateXML.DTD(self.baddtd, timeout=3) as dtd:
            self.assertTrue(dtd.inexistant)

    def test_retrieve_dtd_contents(self):
        with validateXML.DTD(self.dtd, timeout=15) as dtd:
            self.assertTrue(dtd.contents)

    def test_use_fallback_dtd(self):
        dtds = [self.baddtd, self.altdtd]
        contents, url = validateXML.getFirstAvailableDTDContents(*dtds)
        self.assertTrue(contents)
        self.assertEqual(url, self.dtds[1])

    def test_no_dtds_available(self):
        bad = [self.baddtd, self.baddtd]
        contents, url = validateXML.getFirstAvailableDTDContents(*bad)
        self.assertEqual(contents, None)
        self.assertEqual(url, None)

    def test_contents_empty_for_bad_dtd(self):
        contents = validateXML.getDTDContent(self.baddtd)
        self.assertEqual(contents, None)


class XMLValidationTests(unittest.TestCase):
    def setUp(self):
        self.xmlfile = 'test.xml'

    def tearDown(self):
        try:
            os.remove(self.xmlfile)
        except OSError:
            pass

    def _write(self, xmlcontent):
        with open(self.xmlfile, 'w') as f:
            f.write(xmlcontent)

    def test_xml_path_exists(self):
        xmlsnippet = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
        <unifiedTestConfiguration>
            <atn>
                <TEST type="script">
                    <author>_deprecated_</author>
                    <mailto>_deprecated_</mailto>
                </TEST>
            </atn>
        </unifiedTestConfiguration>
        """
        self._write(xmlsnippet)
        with validateXML.XMLFile(self.xmlfile) as xmlfile:
            el = xmlfile.docEl
            pathExists = partial(validateXML.xmlPathExists, el)
            self.assertTrue(pathExists('atn/TEST/mailto'))
            self.assertFalse(pathExists('atn/TEST/inexistant'))

    def test_get_tag(self):
        xmlsnippet = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
        <unifiedTestConfiguration>
            <atn>
                <TEST type="script">
                    <author>first</author>
                    <author>second</author>
                </TEST>
            </atn>
        </unifiedTestConfiguration>
        """
        self._write(xmlsnippet)
        with validateXML.XMLFile(self.xmlfile) as xmlfile:
            el = xmlfile.docEl
            tag = validateXML.getTag(el, 'author')
            badtag = validateXML.getTag(el, 'inexistant')
            self.assertTrue(tag.firstChild.nodeValue == 'first')
            self.assertTrue(badtag is None)

    def test_deprecated_tags(self):
        deprecatedATNtags = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
        <unifiedTestConfiguration>
            <atn>
                <TEST type="script">
                    <author>_deprecated_</author>
                    <mailto>_deprecated_</mailto>
                </TEST>
            </atn>
        </unifiedTestConfiguration>
        """
        self._write(deprecatedATNtags)
        with validateXML.XMLFile(self.xmlfile) as xmlfile:
            self.assertTrue(xmlfile.isValid())
            self.assertTrue(xmlfile.checkForDeprecatedTags())

    def test_non_unique_jobids(self):
        repeatedUserJobIds = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
        <unifiedTestConfiguration>
            <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">
                <rttContactPerson>contact_person</rttContactPerson>
                <mailto>contact@example.com</mailto>
                <jobList>
                    <jobTransform userJobId="theJobID">
                        <jobTransformJobName>name</jobTransformJobName>
                        <jobTransformCmd>ls</jobTransformCmd>
                        <group>TheJobGroup</group>
                    </jobTransform>
                    <jobTransform userJobId="theJobID">
                        <jobTransformJobName>name</jobTransformJobName>
                        <jobTransformCmd>ls</jobTransformCmd>
                        <group>TheJobGroup</group>
                    </jobTransform>
                </jobList>
                <jobGroups>
                    <jobGroup name="TheJobGroup" parent="Transform">
                        <keepFilePattern>*.out</keepFilePattern>
                        <auxFilePattern>*.py</auxFilePattern>
                    </jobGroup>
                </jobGroups>
            </rtt>
        </unifiedTestConfiguration>
        """
        self._write(repeatedUserJobIds)
        with validateXML.XMLFile(self.xmlfile) as xmlfile:
            self.assertFalse(validateXML.hasLegalUserJobIds(xmlfile.docEl))

    def test_bad_chars_in_jobid(self):
        badUserJobIds = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
        <unifiedTestConfiguration>
            <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">
                <rttContactPerson>contact_person</rttContactPerson>
                <mailto>contact@example.com</mailto>
                <jobList>
                    <jobTransform userJobId="the JobID">
                        <jobTransformJobName>name</jobTransformJobName>
                        <jobTransformCmd>ls</jobTransformCmd>
                        <group>TheJobGroup</group>
                    </jobTransform>
                    <jobTransform userJobId="theJobID">
                        <jobTransformJobName>name</jobTransformJobName>
                        <jobTransformCmd>ls</jobTransformCmd>
                        <group>TheJobGroup</group>
                    </jobTransform>
                    <jobTransform userJobId="//theJobID">
                        <jobTransformJobName>name</jobTransformJobName>
                        <jobTransformCmd>ls</jobTransformCmd>
                        <group>TheJobGroup</group>
                    </jobTransform>
                </jobList>
                <jobGroups>
                    <jobGroup name="TheJobGroup" parent="Transform">
                        <keepFilePattern>*.out</keepFilePattern>
                        <auxFilePattern>*.py</auxFilePattern>
                    </jobGroup>
                </jobGroups>
            </rtt>
        </unifiedTestConfiguration>
        """
        self._write(badUserJobIds)
        with validateXML.XMLFile(self.xmlfile) as xmlfile:
            self.assertFalse(validateXML.hasLegalUserJobIds(xmlfile.docEl))

            ids = validateXML.getJobIds(xmlfile.docEl)
            bad = [jobid for jobid in ids
                   if validateXML._idHasIllegalChars(jobid)]
            self.assertTrue(len(bad) == 2)

    def test_valid_xml(self):
        validXML = """<?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE unifiedTestConfiguration SYSTEM "http://www.hep.ucl.ac.uk/atlas/AtlasTesting/DTD/unifiedTestConfiguration.dtd">
        <unifiedTestConfiguration>
            <rtt xmlns="http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt">
                <rttContactPerson>contact_person</rttContactPerson>
                <mailto>contact@example.com</mailto>
                <jobList>
                    <jobTransform userJobId="theJobID">
                        <jobTransformJobName>name</jobTransformJobName>
                        <jobTransformCmd>ls</jobTransformCmd>
                        <group>TheJobGroup</group>
                    </jobTransform>
                </jobList>
                <jobGroups>
                    <jobGroup name="TheJobGroup" parent="Transform">
                        <keepFilePattern>*.out</keepFilePattern>
                        <auxFilePattern>*.py</auxFilePattern>
                    </jobGroup>
                </jobGroups>
            </rtt>
        </unifiedTestConfiguration>
        """
        self._write(validXML)
        with validateXML.XMLFile(self.xmlfile) as xmlfile:
            self.assertTrue(xmlfile.isValid())


if __name__ == '__main__':
    unittest.main()
