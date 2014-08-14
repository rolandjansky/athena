package atlantis.utils;

import atlantis.globals.AGlobals;
import atlantis.utils.xml.AXMLErrorHandler;
import java.io.InputStream;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;
import org.junit.BeforeClass;
import org.junit.Test;
import org.w3c.dom.Document;
import testutils.AtlantisInit;

/**
 * Tests for the ANewIdHelper class. Tests are performed by fully 
 * decoding compact identifiers of several subsystems.
 *
 * @author Eric Jansen
 */
public class ANewIdHelperTest {
    
    public ANewIdHelperTest() {}
    
    @BeforeClass
    public static void setUpClass() throws Exception {
        AtlantisInit.init();
        String geometryBase = AGlobals.instance().getHomeDirectory() + "geometry/";
        InputStream isIdDict = AUtilities.getFileAsStream(geometryBase + "ATLAS_IDS.xml");

        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setValidating(true);
        DocumentBuilder parser = factory.newDocumentBuilder();
        parser.setErrorHandler(new AXMLErrorHandler());
        
        Document xmlDictDoc = parser.parse(isIdDict, geometryBase);
        ANewIdHelper.construct(xmlDictDoc.getDocumentElement());
        
        System.out.println("The loaded identifier structure is:\n"
                + ANewIdHelper.instance().getDictionary("ATLAS"));
    }
    
    @Test
    public void testPixel() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("InnerDetector/Pixel/negative_endcap/0/15/0/86/76", idHelper.getFullIdentifier(8661285809946624l));
    }
    
    @Test
    public void testSCT() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("InnerDetector/SCT/negative_endcap/1/23/2/0/683", idHelper.getFullIdentifier(137085611));
        assertEquals("InnerDetector/SCT/positive_endcap/8/50/0/1/609", idHelper.getFullIdentifier(219756129));
    }
    
    @Test
    public void testTRT() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("InnerDetector/TRT/negative_endcap/2/2/6/7", idHelper.getFullIdentifier(270604512));
        assertEquals("InnerDetector/TRT/negative_endcap/11/1/7/9", idHelper.getFullIdentifier(280010016));
        assertEquals("InnerDetector/TRT/negative_barrel/9/0/18/7", idHelper.getFullIdentifier(311445728));
        assertEquals("InnerDetector/TRT/positive_barrel/5/2/22/10", idHelper.getFullIdentifier(340875584));
        assertEquals("InnerDetector/TRT/positive_endcap/7/10/1/13", idHelper.getFullIdentifier(376767904));
        assertEquals("InnerDetector/TRT/positive_endcap/31/13/7/18", idHelper.getFullIdentifier(402038336));
    }
    
    @Test
    public void testLAr() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("LArCalorimeter/LArEM/negative-endcap-outer-wheel/2/1/37/72", idHelper.getFullIdentifier(3225785215439863808l));
        assertEquals("LArCalorimeter/LArEM/positive-barrel/1/0/167/15", idHelper.getFullIdentifier(765546014));
        assertEquals("LArCalorimeter/LArEM/negative-barrel/2/0/4/113", idHelper.getFullIdentifier(759171298));
    }
    
    @Test
    public void testTILE() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("TileCalorimeter/Barrel/positive/28/0/1/0/0", idHelper.getFullIdentifier(1149698064));
        assertEquals("TileCalorimeter/Extended-barrel/negative/0/14/0/0/0", idHelper.getFullIdentifier(1207963136));
    }
    
    @Test
    public void testHEC() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("LArCalorimeter/LArHEC/negative-endcap-outer-wheel/1/0/5/22", idHelper.getFullIdentifier(815095808));
        assertEquals("LArCalorimeter/LArHEC/negative-endcap-outer-wheel/2/0/6/22", idHelper.getFullIdentifier(823746560));
        assertEquals("LArCalorimeter/LArHEC/positive-endcap-outer-wheel/2/1/2/20", idHelper.getFullIdentifier(860438528));
        assertEquals("LArCalorimeter/LArHEC/positive-endcap-outer-wheel/3/0/8/0", idHelper.getFullIdentifier(866123776));
        assertEquals("LArCalorimeter/LArHEC/positive-endcap-outer-wheel/3/1/3/29", idHelper.getFullIdentifier(869126144));
    }
    
    @Test
    public void testFCAL() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("LArCalorimeter/LArFCAL/negative-endcap-outer-wheel/1/4/13", idHelper.getFullIdentifier(873046016));
        assertEquals("LArCalorimeter/LArFCAL/negative-endcap-outer-wheel/1/45/0", idHelper.getFullIdentifier(878313472));
        assertEquals("LArCalorimeter/LArFCAL/positive-endcap-outer-wheel/1/3/0", idHelper.getFullIdentifier(906362880));
        assertEquals("LArCalorimeter/LArFCAL/positive-endcap-outer-wheel/2/23/14", idHelper.getFullIdentifier(917487616));
        assertEquals("LArCalorimeter/LArFCAL/positive-endcap-outer-wheel/3/15/15", idHelper.getFullIdentifier(924835840));
    }

    @Test
    public void testMDT() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("MuonSpectrometer/BIL/-6/1/MDT/1/3/1", idHelper.getFullIdentifier(6922067811640541184l));
        assertEquals("MuonSpectrometer/BIL/-6/2/MDT/2/2/34", idHelper.getFullIdentifier(1611748384));
        assertEquals("MuonSpectrometer/BIL/1/5/MDT/1/2/3", idHelper.getFullIdentifier(1615597632));
        assertEquals("MuonSpectrometer/BOL/1/5/MDT/1/2/7", idHelper.getFullIdentifier(1682706624));
        assertEquals("MuonSpectrometer/EOL/2/3/MDT/2/1/6", idHelper.getFullIdentifier(1884438688));
        assertEquals("MuonSpectrometer/EIS/-1/6/MDT/2/3/36", idHelper.getFullIdentifier(2050843744));
    }
    
    @Test
    public void testRPC() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("MuonSpectrometer/BML/-6/6/RPC/1/2/2/2/0/4", idHelper.getFullIdentifier(1645583372));
    }
    
    @Test
    public void testCSC() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("MuonSpectrometer/CSS/-1/1/CSC/2/4/0/6", idHelper.getFullIdentifier(2064056960));
    }
    
    @Test
    public void testTGC() throws Exception {
        ANewIdHelper idHelper = ANewIdHelper.instance();
        assertEquals("MuonSpectrometer/T4E/1/10/TGC/2/1/15", idHelper.getFullIdentifier(2035436656));
    }
}
